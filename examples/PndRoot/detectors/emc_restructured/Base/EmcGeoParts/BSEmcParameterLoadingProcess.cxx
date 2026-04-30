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

#include "BSEmcParameterLoadingProcess.h"

//------------------------------------------------------------------------
void BSEmcParameterLoadingProcess::SetDetectorName(const std::string &t_detectorName)
{
  this->fDetectorName = t_detectorName;
  this->fParameterList.push_back(BSEmcCrystalPositionPar::fgParameterName + "Barrel");
  this->fParameterList.push_back(BSEmcCrystalPositionPar::fgParameterName + "FwEndcap");
  this->fParameterList.push_back(BSEmcCrystalPositionPar::fgParameterName + "BwEndcap");
  this->fParameterList.push_back(BSEmcCrystalPositionPar::fgParameterName + "Shashlik");
  this->fParameterList.push_back(BSEmcPositionPar::fgParameterName + "Barrel");
  this->fParameterList.push_back(BSEmcPositionPar::fgParameterName + "FwEndcap");
  this->fParameterList.push_back(BSEmcPositionPar::fgParameterName + "BwEndcap");
  this->fParameterList.push_back(BSEmcPositionPar::fgParameterName + "Shashlik");
  this->fParameterList.push_back(BSEmcGeoNeighbouringRelationPar::fgParameterName + "Barrel");
  this->fParameterList.push_back(BSEmcGeoNeighbouringRelationPar::fgParameterName + "FwEndcap");
  this->fParameterList.push_back(BSEmcGeoNeighbouringRelationPar::fgParameterName + "BwEndcap");
  this->fParameterList.push_back(BSEmcGeoNeighbouringRelationPar::fgParameterName + "Shashlik");
}

//------------------------------------------------------------------------
void BSEmcParameterLoadingProcess::SetupParameters(const PndParameterRegister *t_parameter)
{
  SetupPositionParameters(t_parameter);
  SetupNeighbouringParameters(t_parameter);
}

//------------------------------------------------------------------------
void BSEmcParameterLoadingProcess::GetPositionParameterFromCrystals(BSEmcCrystalPositionPar *t_par, Int_t t_lowerDetectorId, Int_t t_upperDetectorId)
{
  if (!BSEmcStructure::Instance()->IsDetIdXtalMapSet()) {
    BSEmcStructure::Instance()->CreateDetIdXtalMap();
  }
  for (const auto &crystal : BSEmcStructure::Instance()->GetDetIdXtalMap()) {
    Int_t id = crystal.first;
    if (t_lowerDetectorId <= id && id < t_upperDetectorId) {
      BSEmcXtal *xtal = crystal.second.get();
      t_par->SetPositionDataForCrystal(id, {xtal->GetCentre(), xtal->GetFrontCentre(), xtal->GetNormalToFrontFace(), xtal->GetAxisVector()});
    }
  }
}

//------------------------------------------------------------------------
void BSEmcParameterLoadingProcess::ReadPositionParParameter(BSEmcCrystalPositionPar *t_par, Int_t t_lowerDetectorId, Int_t t_upperDetectorId)
{
  LOG(debug) << "BSEmcParameterLoadingProcess::ReadPositionParParameter(" << fPositionParFilename << ").";
  std::string line;
  std::ifstream txtfile(fPositionParFilename.Data());
  if (txtfile.is_open()) {
    while (std::getline(txtfile, line)) {
      TString entry{line};
      TString tmpstr;
      std::unique_ptr<TObjArray> tokens{entry.Tokenize(":")};
      Int_t crystalId = 0;
      TVector3 vectorvalues[4];
      if (tokens->GetEntriesFast() > 1) {
        tmpstr = (dynamic_cast<TObjString *>(tokens->At(0)))->GetString();
        crystalId = tmpstr.Atoi();
        if (t_lowerDetectorId <= crystalId && crystalId < t_upperDetectorId) {
          Int_t counter = 0, veccounter = 0;
          for (Int_t tokenId = 1; tokenId < tokens->GetEntriesFast(); ++tokenId) {
            tmpstr = (dynamic_cast<TObjString *>(tokens->At(tokenId)))->GetString();
            vectorvalues[veccounter][counter] = tmpstr.Atof();
            counter = (counter + 1) % 3;
            if (counter == 0) {
              ++veccounter;
            }
          }
          t_par->SetPositionDataForCrystal(crystalId, BSEmcCrystalPositionData{vectorvalues[0], vectorvalues[1], vectorvalues[2], vectorvalues[3]});
        }
      }
    }
    txtfile.close();
  } else {
    LOG(error) << "BSEmcParameterLoadingProcess::ReadPositionParParameter(" << fPositionParFilename << ") - Unable to open file!";
  }
}

//------------------------------------------------------------------------
void BSEmcParameterLoadingProcess::FillPositionParameter(BSEmcCrystalPositionPar *t_posPar, Int_t t_inputVersion, Int_t t_lowerDetectorId, Int_t t_upperDetectorId)
{
  if (fPositionParFilename.Length() != 0) {
    ReadPositionParParameter(t_posPar, t_lowerDetectorId, t_upperDetectorId);
  } else {
    GetPositionParameterFromCrystals(t_posPar, t_lowerDetectorId, t_upperDetectorId);
  }
  t_posPar->UpdatePositionArray();
  t_posPar->setChanged(kTRUE);
  t_posPar->setInputVersion(t_inputVersion, 1);
}

//------------------------------------------------------------------------
void BSEmcParameterLoadingProcess::SetupPositionParameter(BSEmcCrystalPositionPar *t_posPar, BSEmcPositionPar *t_digiPar)
{
  t_posPar->SetPositionDepth(t_digiPar->GetDigiPositionDepth());
  t_posPar->SetPositionMethod(t_digiPar->GetDigiPositionMethod());
  t_posPar->FillMap();
}

//------------------------------------------------------------------------
void BSEmcParameterLoadingProcess::SetupPositionParameters(const PndParameterRegister *t_paramRegister)
{
  BSEmcCrystalPositionPar *barrelPar = dynamic_cast<BSEmcCrystalPositionPar *>(t_paramRegister->GetParameter(TString{BSEmcCrystalPositionPar::fgParameterName + "Barrel"}.Data()));
  BSEmcCrystalPositionPar *fwecPar = dynamic_cast<BSEmcCrystalPositionPar *>(t_paramRegister->GetParameter(TString{BSEmcCrystalPositionPar::fgParameterName + "FwEndcap"}.Data()));
  BSEmcCrystalPositionPar *bwecPar = dynamic_cast<BSEmcCrystalPositionPar *>(t_paramRegister->GetParameter(TString{BSEmcCrystalPositionPar::fgParameterName + "BwEndcap"}.Data()));
  BSEmcCrystalPositionPar *shashlikPar =
    dynamic_cast<BSEmcCrystalPositionPar *>(t_paramRegister->GetParameter(TString{BSEmcCrystalPositionPar::fgParameterName + "Shashlik"}.Data()));
  BSEmcPositionPar *barrelDigiPar = dynamic_cast<BSEmcPositionPar *>(t_paramRegister->GetParameter(TString{BSEmcPositionPar::fgParameterName + "Barrel"}.Data()));
  BSEmcPositionPar *fwecDigiPar = dynamic_cast<BSEmcPositionPar *>(t_paramRegister->GetParameter(TString{BSEmcPositionPar::fgParameterName + "FwEndcap"}.Data()));
  BSEmcPositionPar *bwecDigiPar = dynamic_cast<BSEmcPositionPar *>(t_paramRegister->GetParameter(TString{BSEmcPositionPar::fgParameterName + "BwEndcap"}.Data()));
  BSEmcPositionPar *shashlikDigiPar = dynamic_cast<BSEmcPositionPar *>(t_paramRegister->GetParameter(TString{BSEmcPositionPar::fgParameterName + "Shashlik"}.Data()));
  if (!barrelPar->IsSet() || !fwecPar->IsSet() || !bwecPar->IsSet() || !shashlikPar->IsSet()) {
    FairRun *run = FairRun::Instance();
    if (run == nullptr) {
      Fatal("SetParContainers", "No analysis run");
    }
    FillPositionParameter(barrelPar, run->GetRunId(), 100000000, 300000000);
    FillPositionParameter(fwecPar, run->GetRunId(), 300000000, 400000000);
    FillPositionParameter(bwecPar, run->GetRunId(), 400000000, 500000000);
    FillPositionParameter(shashlikPar, run->GetRunId(), 500000000, 600000000);
  }
  SetupPositionParameter(barrelPar, barrelDigiPar);
  SetupPositionParameter(fwecPar, fwecDigiPar);
  SetupPositionParameter(bwecPar, bwecDigiPar);
  SetupPositionParameter(shashlikPar, shashlikDigiPar);
}

//------------------------------------------------------------------------
void BSEmcParameterLoadingProcess::ReadNeighbouringParameter(BSEmcGeoNeighbouringRelationPar *t_par, Int_t t_lowerId, Int_t t_upperId)
{
  LOG(debug) << "BSEmcParameterLoadingProcess::ReadNeighbouringParameter(" << fNeighbouringFilename << ").";
  std::string line;
  std::ifstream txtfile(fNeighbouringFilename.Data());
  if (txtfile.is_open()) {
    while (std::getline(txtfile, line)) {
      TString entry{line};
      TString tmpstr;
      TObjArray *tokens = entry.Tokenize(":");
      Int_t crystalId = 0;
      std::vector<Int_t> neighbourIds;
      if (tokens->GetEntriesFast() > 1) {
        tmpstr = (dynamic_cast<TObjString *>(tokens->At(0)))->GetString();
        crystalId = tmpstr.Atoi();
        for (Int_t tokenId = 1; tokenId < tokens->GetEntriesFast(); ++tokenId) {
          tmpstr = (dynamic_cast<TObjString *>(tokens->At(tokenId)))->GetString();
          neighbourIds.push_back(tmpstr.Atoi());
        }
        if (t_lowerId <= crystalId && crystalId < t_upperId) {
          t_par->RegisterIdAndNeighbours(crystalId, neighbourIds);
        }
      }
    }
    txtfile.close();
  } else {
    LOG(error) << "BSEmcParameterLoadingProcess::ReadNeighbouringParameter(" << fNeighbouringFilename << ") - Unable to open file!";
  }
  t_par->UpdateRelationArray();
}

//------------------------------------------------------------------------
void BSEmcParameterLoadingProcess::LoadNeighbouringParameter(BSEmcGeoNeighbouringRelationPar *t_par, Int_t t_version, Int_t t_lowerId, Int_t t_upperId)
{
  ReadNeighbouringParameter(t_par, t_lowerId, t_upperId);
  t_par->setChanged(kTRUE);
  t_par->setInputVersion(t_version, 1);
}

//------------------------------------------------------------------------
void BSEmcParameterLoadingProcess::SetupNeighbouringParameters(const PndParameterRegister *t_paramRegister)
{
  BSEmcGeoNeighbouringRelationPar *barrelPar =
    dynamic_cast<BSEmcGeoNeighbouringRelationPar *>(t_paramRegister->GetParameter(TString{BSEmcGeoNeighbouringRelationPar::fgParameterName + "Barrel"}.Data()));
  BSEmcGeoNeighbouringRelationPar *fwEndcapPar =
    dynamic_cast<BSEmcGeoNeighbouringRelationPar *>(t_paramRegister->GetParameter(TString{BSEmcGeoNeighbouringRelationPar::fgParameterName + "FwEndcap"}.Data()));
  BSEmcGeoNeighbouringRelationPar *bwEndcapPar =
    dynamic_cast<BSEmcGeoNeighbouringRelationPar *>(t_paramRegister->GetParameter(TString{BSEmcGeoNeighbouringRelationPar::fgParameterName + "BwEndcap"}.Data()));
  BSEmcGeoNeighbouringRelationPar *shashlikPar =
    dynamic_cast<BSEmcGeoNeighbouringRelationPar *>(t_paramRegister->GetParameter(TString{BSEmcGeoNeighbouringRelationPar::fgParameterName + "Shashlik"}.Data()));
  if (!barrelPar->IsSet() || !fwEndcapPar->IsSet() || !bwEndcapPar->IsSet() || !shashlikPar->IsSet()) {
    FairRun *run = FairRun::Instance();
    if (run == nullptr) {
      Fatal(TString{"BSEmcParameterLoadingProcess::SetupNeighbouringParameters(" + fNeighbouringFilename + ")"}, "No analysis run");
    }
    LoadNeighbouringParameter(barrelPar, run->GetRunId(), 100000000, 300000000);
    LoadNeighbouringParameter(fwEndcapPar, run->GetRunId(), 300000000, 400000000);
    LoadNeighbouringParameter(bwEndcapPar, run->GetRunId(), 400000000, 500000000);
    LoadNeighbouringParameter(shashlikPar, run->GetRunId(), 500000000, 600000000);
  }
  barrelPar->FillMap();
  fwEndcapPar->FillMap();
  bwEndcapPar->FillMap();
  shashlikPar->FillMap();
}
