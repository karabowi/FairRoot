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

#include "BSEmcCrystalPositionPar.h"

#include <fstream>
#include <stdlib.h>
#include <string>
#include <utility>

#include "TObjArray.h"
#include "TObjString.h"
#include "TObject.h"
#include "TString.h"
#include "TVector3.h"

#include "FairParamList.h"
#include "fairlogger/Logger.h"

std::string BSEmcCrystalPositionPar::fgParameterName = "EmcCrystalPositionPar";
BSEmcCrystalPositionPar::BSEmcCrystalPositionPar(const char *t_name, const char *t_title, const char *t_context) : FairParGenericSet(t_name, t_title, t_context), fPositions()
{
  clear();
  fPositions.reset(new TObjArray());
  fPositions->SetOwner(kTRUE);
}

BSEmcCrystalPositionPar::BSEmcCrystalPositionPar(const BSEmcCrystalPositionPar &t_posPar) : FairParGenericSet(t_posPar)
{
  fPositions.reset(new TObjArray(*(t_posPar.GetPositions())));
}

BSEmcCrystalPositionPar::~BSEmcCrystalPositionPar(void) {}

BSEmcCrystalPositionPar &BSEmcCrystalPositionPar::operator=(const BSEmcCrystalPositionPar &t_posPar)
{
  fPositions.reset(new TObjArray(*(t_posPar.GetPositions())));
  return *this;
}

void BSEmcCrystalPositionPar::putParams(FairParamList *t_list)
{
  LOG(debug) << "BSEmcCrystalPositionPar::putParams(FairParamList* t_list)";
  if (t_list == nullptr) {
    LOG(warn) << "BSEmcCrystalPositionPar::putParams(FairParamList* t_list) returning early as list is nullptr";
    return;
  }
  LOG(debug) << "BSEmcCrystalPositionPar::putParams(FairParamList* t_list) adding fRelations";
  UpdatePositionArray();
  t_list->addObject("Positions", fPositions.get());
}

Bool_t BSEmcCrystalPositionPar::getParams(FairParamList *t_list)
{
  LOG(debug) << "BSEmcCrystalPositionPar::getParams(FairParamList *t_list)";

  if (t_list == nullptr) {
    return kFALSE;
  }
  if (!t_list->fillObject("Positions", fPositions.get())) {
    return kFALSE;
  }

  FillMap();
  return kTRUE;
}

void BSEmcCrystalPositionPar::FillMap()
{
  LOG(debug) << "BSEmcCrystalPositionPar::FillMap()";
  fPositionMap.clear();
  if (fPositions == nullptr) {
    LOG(error) << "BSEmcCrystalPositionPar::FillMap() - fPositions is nullptr! Exiting!";
    exit(-1);
  }
  if (fPositions->GetEntriesFast() == 0) {
    return;
  }
  LOG(debug) << "BSEmcCrystalPositionPar::FillMap() - There are " << fPositions->GetEntriesFast() << " Position entries";
  TString tmpstr;
  for (Int_t i = 0; i < fPositions->GetEntriesFast(); i++) {
    TObjString *relation = dynamic_cast<TObjString *>(fPositions->At(i));

    TString relationStr = relation->GetString();
    std::unique_ptr<TObjArray> tokens{relationStr.Tokenize(":")};
    Int_t crystalId = 0;
    TVector3 vectorvalues[4];
    if (tokens->GetEntriesFast() > 1) {
      tmpstr = (dynamic_cast<TObjString *>(tokens->At(0)))->GetString();
      crystalId = tmpstr.Atoi();
      Int_t counter = 0, veccounter = 0;
      for (Int_t tokenId = 1; tokenId < tokens->GetEntriesFast(); ++tokenId) {
        tmpstr = (dynamic_cast<TObjString *>(tokens->At(tokenId)))->GetString();
        vectorvalues[veccounter][counter] = tmpstr.Atof();
        counter = (counter + 1) % 3;
        if (counter == 0) {
          ++veccounter;
        }
      }
      fPositionMap[crystalId] = BSEmcCrystalPositionData{vectorvalues[0], vectorvalues[1], vectorvalues[2], vectorvalues[3]};
    }
    tokens->Delete();
  }
}

void BSEmcCrystalPositionPar::UpdatePositionArray()
{
  LOG(trace) << "BSEmcCrystalPositionPar::UpdatePositionArray()";
  UInt_t counter = 0;
  fPositions->Delete();
  fPositions->Clear();
  fPositions->Compress();
  for (auto &relation : fPositionMap) {
    Int_t main = relation.first;
    const BSEmcCrystalPositionData position = relation.second;
    TString entry = std::to_string(main);
    for (Int_t i = 0; i < 3; ++i) {
      entry += ":" + std::to_string(position.Center[i]);
    }
    for (Int_t i = 0; i < 3; ++i) {
      entry += ":" + std::to_string(position.FrontCenter[i]);
    }
    for (Int_t i = 0; i < 3; ++i) {
      entry += ":" + std::to_string(position.FrontFaceNormal[i]);
    }
    for (Int_t i = 0; i < 3; ++i) {
      entry += ":" + std::to_string(position.Axis[i]);
    }
    fPositions->AddLast(new TObjString(entry));
    ++counter;
  }
  LOG(trace) << "BSEmcCrystalPositionPar::UpdatePositionArray() - " << counter << " Crystals found.";
}

void BSEmcCrystalPositionPar::SetPositionDataForCrystal(Int_t t_id, const BSEmcCrystalPositionData &t_position)
{
  setChanged(kTRUE);
  fPositionMap[t_id] = t_position;
}

const BSEmcCrystalPositionData &BSEmcCrystalPositionPar::GetPositionData(Int_t t_detectorid) const
{
  auto pos = fPositionMap.find(t_detectorid);
  if (pos == fPositionMap.end()) {
    LOG(error) << "BSEmcCrystalPositionPar::GetPositionData(t_detectorid)"
               << " - no position info found for detector: " << t_detectorid << "! Exiting!";

    exit(-1);
  }
  return (*pos).second;
}

void BSEmcCrystalPositionPar::InitFromTextFile(const TString &t_filename)
{
  LOG(debug) << "BSEmcCrystalPositionPar::InitFromTextFile(const TString& t_filename: " << t_filename << ").";

  fPositions->Delete();
  std::string line;
  std::ifstream txtfile(t_filename.Data());
  if (txtfile.is_open()) {
    while (std::getline(txtfile, line)) {
      TString entry{line};
      fPositions->AddLast(new TObjString(entry));
    }
    txtfile.close();
  } else {
    LOG(error) << "BSEmcCrystalPositionPar::InitFromTextFile(const TString& t_filename: " << t_filename << ") - Unable to open file!";
  }
  FillMap();
  setChanged(kTRUE);
}

void BSEmcCrystalPositionPar::WriteToTextFile(const TString &t_filename) const
{
  std::ofstream txtfile(t_filename.Data());
  if (txtfile.is_open()) {
    for (const auto &relation : fPositionMap) {
      Int_t main = relation.first;
      const BSEmcCrystalPositionData position = relation.second;
      TString entry = std::to_string(main);
      for (Int_t i = 0; i < 3; ++i) {
        entry += ":" + std::to_string(position.Center[i]);
      }
      for (Int_t i = 0; i < 3; ++i) {
        entry += ":" + std::to_string(position.FrontCenter[i]);
      }
      for (Int_t i = 0; i < 3; ++i) {
        entry += ":" + std::to_string(position.FrontFaceNormal[i]);
      }
      for (Int_t i = 0; i < 3; ++i) {
        entry += ":" + std::to_string(position.Axis[i]);
      }
      txtfile << entry << '\n';
    }
    txtfile.close();
  }
}

void BSEmcCrystalPositionPar::SetPositionMethod(const TString &t_positionMethod)
{
  if (t_positionMethod.Contains("DEPTH")) {
    fDigiPosMethod = [](const BSEmcCrystalPositionData &xtal, Double_t depthfactor) {
      TVector3 pos = xtal.FrontCenter;
      TVector3 norm = xtal.Axis;
      norm *= depthfactor;
      pos += norm;
      return pos;
    };
    fRescaleFactor = 1.08;
  } else {
    fDigiPosMethod = [](const BSEmcCrystalPositionData &xtal, Double_t /*unused*/) {
      return xtal.FrontCenter;
    };
    fRescaleFactor = 1.;
  }
  fDigiPosMethodIsSet = kTRUE;

  LOG(debug) << "Chosen position Method " << t_positionMethod << " with rescalfactor = " << fRescaleFactor;
}

TVector3 BSEmcCrystalPositionPar::GetPosition(Int_t t_detectorId) const
{
  auto pos = fPositionMap.find(t_detectorId);
  if (pos == fPositionMap.end()) {
    LOG(error) << "BSEmcCrystalPositionPar::GetPosition(t_detectorId=" << t_detectorId << ") failed, as no crystal matched t_detectorId.";
    exit(-1);
  }
  const BSEmcCrystalPositionData &crystal = pos->second;
  if (!fDigiPosMethodIsSet) {
    LOG(error) << "BSEmcCrystalPositionPar: !!BSEmcStructure!!::Init() must be called in your run script. Aborting!";
    exit(-1);
  }
  return fDigiPosMethod(crystal, fCrystalPositionDepth);
}

ClassImp(BSEmcCrystalPositionPar);
