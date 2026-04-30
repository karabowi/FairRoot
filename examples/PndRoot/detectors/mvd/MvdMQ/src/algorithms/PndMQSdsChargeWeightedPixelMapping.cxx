//****************************************************************************
//*                   This file is part of PandaRoot.                        *
//*                                                                          *
//*            PandaRoot is distributed under the terms of the               *
//*              GNU General Public License (GPL) version 3,                 *
//*                 copied verbatim in the file "LICENSE".                   *
//*                                                                          *
//*  Copyright (C) 2006 - 2024 FAIR GmbH and copyright holders of PandaRoot  *
//*     The copyright holders are listed in the file "COPYRIGHTHOLDERS".     *
//*               The authors are listed in the file "AUTHORS".              *
//****************************************************************************

#include <PndMQSdsChargeWeightedPixelMapping.h>

#include "TGeoManager.h"
#include "TGeoBBox.h"
#include "TClonesArray.h"
#include "TMath.h"

PndMQSdsChargeWeightedPixelMapping::PndMQSdsChargeWeightedPixelMapping() : PndMQSdsPixelBackMapping() {}
PndMQSdsChargeWeightedPixelMapping::PndMQSdsChargeWeightedPixelMapping(PndGeoHandling *geo) : PndMQSdsPixelBackMapping(geo) {}

PndSdsHit PndMQSdsChargeWeightedPixelMapping::GetCluster(std::vector<PndSdsDigiPixel *> &pixelArray)
{
  Double_t col = 0, row = 0, charge = 0;
  Double_t tempCol = 0, tempRow = 0;
  std::vector<Double_t> timeValues;
  std::vector<Double_t> timeValueErrors;

  Int_t count = 0, mcindex = -1;
  // Double_t local[2], master[2];
  // TODO: Get away from default 10 Frontends per column?

  if (pixelArray.size() == 1) {
    if (fChargeConverter->DigiValueToCharge(*pixelArray[0]) > 0) {
      col = pixelArray[0]->GetPixelColumn() + ((Int_t)(pixelArray[0]->GetFE() % 10)) * fcols;
      if (col < 0) {
        col -= 0.5;
        col += frows / 2;
      } else
        col += 0.5;
      row = pixelArray[0]->GetPixelRow() + (pixelArray[0]->GetFE() / 10) * frows;
      if (row < 0) {
        row -= 0.5;
        row += flx / 2;
      } else
        row += 0.5;
      if (fVerbose > 1)
        std::cout << "GetCluster:col/row " << col << " " << row << std::endl;
      count = 1;
      charge = fChargeConverter->DigiValueToCharge(*pixelArray[0]);
      for (Int_t mcI = 0; mcI < pixelArray[0]->GetNIndices(); mcI++) {
        if (pixelArray[0]->GetIndex(mcI) > -1) {
          mcindex = pixelArray[0]->GetIndex(mcI);
          break;
        }
      }
      timeValues.push_back(pixelArray[0]->GetTimeStamp());
      timeValueErrors.push_back(pixelArray[0]->GetTimeStampError());
    }
  } else {
    // cout << "Multiple Hits!" << std::endl;
    for (UInt_t i = 0; i < pixelArray.size(); i++) {
      // cout << "ActCol / Row" << col << " " << row << " added Col/Row " << pixelArray[i].GetPixelColumn() << " " << pixelArray[i].GetPixelRow() << endl;
      if (fChargeConverter->DigiValueToCharge(*pixelArray[i]) > 0) {
        tempCol = pixelArray[i]->GetPixelColumn() + (Int_t)(pixelArray[0]->GetFE() % 10) * fcols;
        tempRow = pixelArray[i]->GetPixelRow() + pixelArray[0]->GetFE() / 10 * frows;
        col += (tempCol * fChargeConverter->DigiValueToCharge(*pixelArray[i]));
        row += (tempRow * fChargeConverter->DigiValueToCharge(*pixelArray[i]));
        charge += fChargeConverter->DigiValueToCharge(*pixelArray[i]);
        timeValues.push_back(pixelArray[i]->GetTimeStamp());
        timeValueErrors.push_back(pixelArray[i]->GetTimeStampError());
        count++;
        if (mcindex < 0) {
          for (Int_t mcI = 0; mcI < pixelArray[i]->GetNIndices(); mcI++) {
            if (pixelArray[i]->GetIndex(mcI) > -1) {
              mcindex = pixelArray[i]->GetIndex(mcI);
              break;
            }
          }
        } // mcindex
      }
    }
    if (count > 0) {
      if (charge > 0) {
        col /= charge;
        row /= charge;
      }
      col += 0.5;
      row += 0.5;
    } else
      col = row = 0;
  }
  if (fVerbose > 1) {
    std::cout << "Col: " << col << " Row: " << row << std::endl;
  }

  TVector3 offset = GetSensorDimensions(pixelArray[0]->GetSensorID());
  TVector3 locpos(col * flx - offset.X(), row * fly - offset.Y(), 0);
  TVector3 pos = fGeoH->LocalToMasterShortId(locpos, pixelArray[0]->GetSensorID());

  Double_t errZ = 2. * fGeoH->GetSensorDimensionsShortId(pixelArray[0]->GetSensorID()).Z();
  TMatrixD locCov(3, 3);
  locCov[0][0] = flx * flx / 12.;
  locCov[1][1] = fly * fly / 12.;
  locCov[2][2] = errZ * errZ / 12;
  TMatrixD hitCov = fGeoH->LocalToMasterErrorsShortId(locCov, pixelArray[0]->GetSensorID());
  TVector3 dpos(sqrt(hitCov[0][0]), sqrt(hitCov[1][1]), sqrt(hitCov[2][2]));
  // std::cout << "-I- PndMQSdsChargeWeightedPixelMapping Error DPos: " << dpos.x() << " " << dpos.y() << " " << dpos.z() << std::endl;

  Double_t meanTime = 0;
  // Double_t meanTimeError = 0;
  Double_t sumVar = 0;

  for (UInt_t t = 0; t < timeValues.size(); t++) {
    meanTime += timeValues[t] / (timeValueErrors[t] * timeValueErrors[t]);
    sumVar += 1 / (timeValueErrors[t] * timeValueErrors[t]);
  }

  if (sumVar > 0)
    meanTime /= sumVar;

  PndSdsHit thehit(pixelArray[0]->GetDetID(), pixelArray[0]->GetSensorID(), pos, dpos, -1, charge, pixelArray.size(), mcindex);
  thehit.SetCov(hitCov);
  thehit.SetTimeStamp(meanTime);
  thehit.SetTimeStampError(1 / sumVar);
  // std::cout << "-I- PndMQSdsChargeWeightedPixelMapping TimeStamp: " << tempTime/count << std::endl;
  return thehit;
}

TGeoHMatrix PndMQSdsChargeWeightedPixelMapping::GetTransformation(Int_t sensorID)
{
  gGeoManager->cd(fGeoH->GetPath(sensorID));
  TGeoHMatrix *transMat = gGeoManager->GetCurrentMatrix();
  if (fVerbose > 2)
    transMat->Print("");
  return *transMat;
}

TVector3 PndMQSdsChargeWeightedPixelMapping::GetSensorDimensions(Int_t sensorID)
{
  gGeoManager->cd(fGeoH->GetPath(sensorID));
  TGeoVolume *actVolume = gGeoManager->GetCurrentVolume();
  TGeoBBox *actBox = (TGeoBBox *)(actVolume->GetShape());
  TVector3 result;
  result.SetX(actBox->GetDX());
  result.SetY(actBox->GetDY());
  result.SetZ(actBox->GetDZ());

  // result.Dump();

  return result;
}
