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
 * PndTrackingQATask.cxx
 *
 *  Created on: 21.06.2023
 *      Author: tstockmanns
 */

#include "PndTrackingQA.h"
#include "PndTrackingQATask.h"
#include "PndSttMapCreator.h"
#include "PndTrackingQAQualityNumbers.h"

#include "FairRuntimeDb.h"
#include "FairRun.h"

#include "TClonesArray.h"

ClassImp(PndTrackingQATask);

PndTrackingQATask::PndTrackingQATask(TString trackBranchName, TString idealBranchName, Bool_t pndTrackData)
  : FairTask("TrackingQA"), fMCInfoBranchName("MCTrackInfo"), fRecoInfoBranchName("RecoTrackInfo"), fTrackBranchName(trackBranchName), fIdealTrackBranchName(idealBranchName),
    fPndTrackNotTrackCand(pndTrackData)
{
}

PndTrackingQATask::~PndTrackingQATask() {}

InitStatus PndTrackingQATask::Init()
{
  ioman = FairRootManager::Instance();
  if (!ioman) {
    std::cout << "-E- PndTrackingQATask::Init: "
              << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  fTrack = dynamic_cast<TClonesArray *>(ioman->GetObject(fTrackBranchName));
  fMCTrack = dynamic_cast<TClonesArray *>(ioman->GetObject("MCTrack"));
  fIdealTrack = dynamic_cast<TClonesArray *>(ioman->GetObject(fIdealTrackBranchName));
  fSttHitArray = dynamic_cast<TClonesArray *>(ioman->GetObject("STTHit"));

  if (fTrack == nullptr) {
    std::cout << "-E- PndTrackingQATask::Init "
              << "no track branch " << fTrackBranchName << std::endl;
    return kFATAL;
  }
  if (fMCTrack == nullptr) {
    std::cout << "-E- PndTrackingQATask::Init "
              << "no MC track branch " << std::endl;
    return kFATAL;
  }

  if (fIdealTrack == nullptr) {
    std::cout << "-E- PndTrackingQATask::Init "
              << "no ideal track branch " << fIdealTrackBranchName << std::endl;
    return kFATAL;
  }

  // MC info for quality
  fMCTrackInfo = new TClonesArray("PndTrackingQAMCInfo");
  ioman->Register(fMCInfoBranchName, "QualityAssurance", fMCTrackInfo, kTRUE); // CHECK
  fRecoTrackInfo = new TClonesArray("PndTrackingQARecoInfo");
  ioman->Register(fRecoInfoBranchName, "QualityAssurance", fRecoTrackInfo, kTRUE); // CHECK

  if (fBranchNames.size() == 0) {
    AddHitsBranchName("MVDHitsPixel");
    AddHitsBranchName("MVDHitsStrip");
    AddHitsBranchName("STTHit");
    AddHitsBranchName("GEMHit");
    AddHitsBranchName("FTSHit");
  }

  for (auto branchName : fBranchNames) {
    ioman->GetObject(branchName);
  }

  if (fPossibleTrackFunctorName.Length() == 0)
    fPossibleTrackFunctorName = "StandardTrackFunctor";

  SetFunctor();

  // ----------------------------------------   maps of STT tubes
  PndSttMapCreator *mapperStt = new PndSttMapCreator(fSttParameters);
  fSttTubeArray = mapperStt->FillTubeArray();
  // ----------------------------------------------------  end map

  return kSUCCESS;
}

void PndTrackingQATask::SetParContainers()
{
  FairRuntimeDb *rtdb = FairRun::Instance()->GetRuntimeDb();
  fSttParameters = (PndGeoSttPar *)rtdb->getContainer("PndGeoSttPar");
}

void PndTrackingQATask::SetFunctor()
{
  fPossibleTrackFunctor = PndTrackFunctor::make_PndTrackFunctor(fPossibleTrackFunctorName.Data());
}

void PndTrackingQATask::Exec(Option_t *)
{
  fMCTrackInfo->Delete();
  fRecoTrackInfo->Delete();

//  std::cout << std::endl;
//  std::cout << "***** Event " << FairRootManager::Instance()->GetEntryNr() << " *****" << std::endl;

  PndTrackingQA qaAna(fTrackBranchName, fIdealTrackBranchName, fPossibleTrackFunctor, fPndTrackNotTrackCand);
  qaAna.SetVerbose(fVerbose);
  qaAna.SetHitsBranchNames(fBranchNames);
  //  if (fSecondaryDefinitionPCA)
  //    qaAna.SetSecondaryDefinitionPCA(fdPCA);
  qaAna.SetSttInfo(fSttHitArray, fSttTubeArray);

  qaAna.Init();
  //
  qaAna.AnalyseEvent(fRecoTrackInfo, fMCTrackInfo);

  FillQASummary();
}

void PndTrackingQATask::FillQASummary()
{
  int mcOffset = TrackingQA::qualityNumbers::kPossibleSec - TrackingQA::qualityNumbers::kMcPossibleSec;

  for (int i = 0; i < fMCTrackInfo->GetEntries(); i++) {
    PndTrackingQAMCInfo *mcInfo = dynamic_cast<PndTrackingQAMCInfo *>(fMCTrackInfo->At(i));
    fSummary.AddResult(mcInfo->GetMCQuality() - mcOffset);
    fSummary.AddResult(mcInfo->GetQuality());
    if (mcInfo->GetQuality() > 0) {
      fSummary.AddResult(TrackingQA::qualityNumbers::kFound);
    } else {
      fSummary.AddResult(TrackingQA::qualityNumbers::kNotFound);
    }
    if (mcInfo->GetNofRecoTracks() > 0) {
      fSummary.AddResult(TrackingQA::qualityNumbers::kClone, mcInfo->GetNofRecoTracks() - 1);
    }
  }
  int nGhosts = GetNGhosts();
  fSummary.AddResult(TrackingQA::qualityNumbers::kGhost, nGhosts);
}

int PndTrackingQATask::GetNGhosts()
{
  int nGhosts = 0;
  for (int i = 0; i < fRecoTrackInfo->GetEntries(); i++) {
    PndTrackingQARecoInfo *recoInfo = dynamic_cast<PndTrackingQARecoInfo *>(fRecoTrackInfo->At(i));
    if (!(TrackingQA::RecoTrackFound(recoInfo))) {
      nGhosts++;
    }
  }
  return nGhosts;
}

void PndTrackingQATask::Finish()
{
  FairSink *sink = ioman->GetSink();
  sink->WriteObject(&fSummary, "TrackingQASummary", TObject::kSingleKey);

  LOG(info) << fSummary;
}
