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

#include "BSEmcLateralClusterSplittingProcess.h"

void BSEmcLateralClusterSplittingProcess::SetDetectorName(const std::string &t_detectorName)
{
  BSEmcExpClusterSplittingProcess::SetDetectorName(t_detectorName);
  if (fLateralClusterSplittingParName == "") {
    fLateralClusterSplittingParName = BSEmcLateralClusterSplittingPar::fgParameterName + fDetectorName;
  }
  fParameterList.push_back(fLateralClusterSplittingParName);
}

void BSEmcLateralClusterSplittingProcess::SetupParameters(const PndParameterRegister *t_paramRegister)
{
  BSEmcExpClusterSplittingProcess::SetupParameters(t_paramRegister);
  BSEmcLateralClusterSplittingPar *par = dynamic_cast<BSEmcLateralClusterSplittingPar *>(t_paramRegister->GetParameter(fLateralClusterSplittingParName));
  fParArray1 = par->GetParArray1();
  fParArray2 = par->GetParArray2();
  fParArray3 = par->GetParArray3();
  fParArray4 = par->GetParArray4();
}

Double_t BSEmcLateralClusterSplittingProcess::CalculateWeight(Int_t t_currentDigiDetID, Int_t t_currentMaxDetId, const TVector3 &t_currentdigisPosition,
                                                              const std::map<Int_t, DigiLocationInfo> &t_centroidPositions) const
{
  Double_t weight = 0;
  Double_t myEnergy = 0;
  Double_t myDistance = 0;
  Double_t totalDistanceEnergy = 0;
  TVector3 currentMaxDigiPos{0, 0, 0};
  for (const auto &centroidPosItr : t_centroidPositions) {
    Int_t centroidPosDetId = centroidPosItr.second.fDetectorId;
    TVector3 centroidPos = centroidPosItr.second.fLocation;
    Double_t theDistance = NAN;
    if ((t_currentDigiDetID) == (centroidPosDetId)) {
      theDistance = 0.0;
    } else {
      TVector3 distance{centroidPos - t_currentdigisPosition};
      theDistance = distance.Mag();
    }
    const TVector3 &maxDigiPos = fPositionPar->GetPosition(t_currentMaxDetId);
    const TVector3 &showerPos = centroidPosItr.second.fLocation;
    if (t_currentMaxDetId == centroidPosDetId) {
      // i.e. the maximum we are trying to find the distance from is
      // the one for which we are currently trying to make a subCluster
      myDistance = theDistance;
      myEnergy = centroidPosItr.second.fEnergy;
      currentMaxDigiPos = maxDigiPos;
    }

    totalDistanceEnergy +=
      centroidPosItr.second.fEnergy * LateralDevelopment(centroidPosItr.second.fSubClusterEnergy, t_currentdigisPosition, maxDigiPos, showerPos, fMoliereRadius);
  }

  if (totalDistanceEnergy > 0.0) {
    const Double_t subclusterenergy = t_centroidPositions.at(t_currentMaxDetId).fSubClusterEnergy;
    const TVector3 &showercenter = t_centroidPositions.at(t_currentMaxDetId).fLocation;
    weight = myEnergy * LateralDevelopment(subclusterenergy, t_currentdigisPosition, currentMaxDigiPos, showercenter, fMoliereRadius) / (totalDistanceEnergy);
  } else {
    weight = 0;
  }
  return weight;
}

Double_t BSEmcLateralClusterSplittingProcess::LateralDevelopment(const Double_t t_totalEnergy, const TVector3 &t_currentDigiPos, const TVector3 &t_maxDigiPos,
                                                                 const TVector3 &t_showerCenter, const Double_t t_moliereRadius) const
{
  Double_t CenterTheta = TMath::RadToDeg() * (t_showerCenter.Theta());
  Double_t p1 = fParArray1.At(0) * exp(-fParArray1.At(1) * t_totalEnergy) + fParArray1.At(2) * pow(CenterTheta - fParArray1.At(3), 2) + fParArray1.At(4);
  Double_t p2 = fParArray2.At(0) * exp(-fParArray2.At(1) * t_totalEnergy) + fParArray2.At(2) * pow(CenterTheta - fParArray2.At(3), 2) + fParArray2.At(4);
  Double_t p3 = fParArray3.At(0) * exp(-fParArray3.At(1) * t_totalEnergy) + fParArray3.At(2) * pow(CenterTheta - fParArray3.At(3), 2) + fParArray3.At(4);
  // Double_t p4 = fParArray4.At(0) * exp(-fParArray4.At(1) * t_totalEnergy) + fParArray4.At(2) * pow(CenterTheta - fParArray4.At(3),2) + fParArray4.At(4);
  Double_t p4 = (fParArray4.At(0) * pow(CenterTheta - fParArray4.At(1), 2) + fParArray4.At(2)) * exp(-(fParArray4.At(3) * CenterTheta + fParArray4.At(4)) * t_totalEnergy) +
                fParArray4.At(5) * pow(CenterTheta - fParArray4.At(6), 2) + fParArray4.At(7);

  Double_t r = t_currentDigiPos.Mag() * TMath::Sin(t_showerCenter.Angle(t_currentDigiPos));
  Double_t r_seed = t_maxDigiPos.Mag() * TMath::Sin(t_showerCenter.Angle(t_maxDigiPos));

  Double_t xi = r - p2 * r * exp(-pow(r / p3 / t_moliereRadius, p4));
  Double_t xi_seed = r_seed - p2 * r_seed * exp(-pow(r_seed / p3 / t_moliereRadius, p4));
  Double_t delta_xi = xi - xi_seed;

  if (delta_xi < 0)
    delta_xi = 0;
  return exp(-p1 * delta_xi / t_moliereRadius);
}

ClassImp(BSEmcExpClusterSplittingProcess)
