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

/*
 * PndGFRecoHitFactoryCreator.cxx
 *
 *  Created on: 19.04.2022
 *      Author: tstockmanns
 */

#include <FairRuntimeDb.h>
#include <FairRootManager.h>
#include <FairRunAna.h>

#include <PndGFRecoHitFactoryCreator.h>
#include "PndGeoSttPar.h"
#include "PndGeoFtsPar.h"
#include "PndSttMapCreator.h"
#include "PndFtsMapCreator.h"
#include "PndFtsRecoHitProducer2.h"
#include "PndSttRecoHitProducer2.h"
#include "PndSdsHit.h"
#include "PndSdsRecoHit2.h"
#include "PndSttHit.h"
#include "PndSttRecoHit2.h"
#include "PndGemHit.h"
#include "PndGemRecoHit2.h"
#include "PndMdtHit.h"
#include "PndMdtRecoHit2.h"
#include "PndFtsHit.h"
#include "PndFtsRecoHit2.h"

PndGFRecoHitFactoryCreator::PndGFRecoHitFactoryCreator()
{
  // TODO Auto-generated constructor stub
}

PndGFRecoHitFactoryCreator::~PndGFRecoHitFactoryCreator()
{
  // TODO Auto-generated destructor stub
}

Bool_t PndGFRecoHitFactoryCreator::Init()
{
  // Get ROOT Manager
  FairRootManager *ioman = FairRootManager::Instance();
  if (ioman == 0) {
    LOG(error) << "RootManager not instantiated!";
    return kFALSE;
  }

  // STT map loading
  FairRuntimeDb *rtdb = FairRunAna::Instance()->GetRuntimeDb();
  PndGeoSttPar *sttParameters = (PndGeoSttPar *)rtdb->getContainer("PndGeoSttPar");
  TClonesArray *tubeArray = nullptr;
  if (sttParameters->GetGeometryType() != -1) {
    PndSttMapCreator *mapper = new PndSttMapCreator(sttParameters);
    fTubeArray = mapper->FillTubeArray();
  }

  // FTS map loading
  PndGeoFtsPar *ftsParameters = (PndGeoFtsPar *)rtdb->getContainer("PndGeoFtsPar");
  TClonesArray *ftsTubeArray = nullptr;
  if (ftsParameters->GetGeometryType() != -1) {
    PndFtsMapCreator *ftsMapper = new PndFtsMapCreator(ftsParameters);
    fFtsTubeArray = ftsMapper->FillTubeArray();
  }

  if (fBranches.size() == 0) {
    SetDefaultBranches();
  }

  InitBranches();

  // Build hit factory -----------------------------
  fTheRecoHitFactory = new genfit::MeasurementFactory<genfit::AbsMeasurement>();

  FillRecoHitFactory();

  return kTRUE;
}

void PndGFRecoHitFactoryCreator::InitBranches()
{
  for (auto &branch : fBranches) {
    branch.second = static_cast<TClonesArray *>(FairRootManager::Instance()->GetObject(branch.first));
  }
}

void PndGFRecoHitFactoryCreator::FillRecoHitFactory()
{
  for (auto branch : fBranches) {
    if (FairRootManager::Instance()->GetBranchId(branch.first) < 0)
      continue;
    fTheRecoHitFactory->addProducer(FairRootManager::Instance()->GetBranchId(branch.first), CreateProducer(branch.first, branch.second));
  }
}

genfit::AbsMeasurementProducer<genfit::AbsMeasurement> *PndGFRecoHitFactoryCreator::CreateProducer(TString branchName, TClonesArray *tcArray)
{
  genfit::AbsMeasurementProducer<genfit::AbsMeasurement> *producer = nullptr;
  if (branchName.Contains("MVD")) {
    producer = new genfit::MeasurementProducer<PndSdsHit, PndSdsRecoHit2>(tcArray);
  } else if (branchName.Contains("STT")) {
    if (fTubeArray != nullptr) {
      producer = new PndSttRecoHitProducer2<PndSttHit, PndSttRecoHit2>(tcArray, fTubeArray);
    } else {
      LOG(warning) << "No tubeArray to generate SttRecoHitProducer2";
    }
  } else if (branchName.Contains("GEM")) {
    producer = new genfit::MeasurementProducer<PndGemHit, PndGemRecoHit2>(tcArray);
  } else if (branchName.Contains("MDT")) {
    producer = new genfit::MeasurementProducer<PndMdtHit, PndMdtRecoHit2>(tcArray);
  } else if (branchName.Contains("FTS")) {
    if (fFtsTubeArray != nullptr) {
      producer = new PndFtsRecoHitProducer2<PndFtsHit, PndFtsRecoHit2>(tcArray, fFtsTubeArray);
    } else {
      LOG(warning) << "No ftsTubeArray to generate FtsRecoHitProducer2";
    }
  }
  if (producer == nullptr)
    LOG(warning) << "No matching branch name for " << branchName;
  return producer;
}

void PndGFRecoHitFactoryCreator::SetDefaultBranches()
{
  fBranches["MVDHitsPixel"] = nullptr;
  fBranches["MVDHitsStrip"] = nullptr;
  fBranches["STTHit"] = nullptr;
  fBranches["GEMHit"] = nullptr;
  fBranches["MDTHit"] = nullptr;
  fBranches["FTSHit"] = nullptr;
}
