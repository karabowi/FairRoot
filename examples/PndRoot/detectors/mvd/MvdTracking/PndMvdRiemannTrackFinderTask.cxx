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

#include "PndMvdRiemannTrackFinderTask.h"

#include <iostream>

// Root includes
#include "TROOT.h"
#include "TString.h"
#include "TClonesArray.h"
#include "TParticlePDG.h"

// framework includes
#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairRunAna.h"
#include "FairLogger.h"

// PndMvd includes
#include "PndTrackCand.h"
#include "PndMvdSttGemRiemannTrackFinder.h"

#include "PndSttMapCreator.h"

PndMvdRiemannTrackFinderTask::PndMvdRiemannTrackFinderTask()
  : FairTask("MVD Riemann Track Finder"), fHitBranch(), fTrackBranch(""), fEventNr(0), fMaxSZChi2(1), fMaxSZDist(10), fMinPointDist(1), fMaxDist(1), fB(2.0), fHitArray(),
    fTrackCandArray(nullptr), fRiemannTrackArray(nullptr), fTrackArray(nullptr), fCutDistH(nullptr), fCutChi2H(nullptr), fInitDone(kFALSE), fPersistence(kTRUE),
    fStopFunctor(nullptr), fTimeGapFunctor(nullptr), fSttParameters(nullptr), fTubeArray(nullptr)
{
  PndGeoHandling::Instance();
}

PndMvdRiemannTrackFinderTask::~PndMvdRiemannTrackFinderTask() {}

void PndMvdRiemannTrackFinderTask::SetParContainers()
{
  FairRuntimeDb *rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fSttParameters = (PndGeoSttPar *)rtdb->getContainer("PndGeoSttPar");
}

InitStatus PndMvdRiemannTrackFinderTask::ReInit()
{
  InitStatus stat = kSUCCESS;
  return stat;
}

// -----   Public method Init   --------------------------------------------
InitStatus PndMvdRiemannTrackFinderTask::Init()
{
  FairRootManager *ioman = FairRootManager::Instance();

  if (!ioman) {
    std::cout << "-E- PndMvdRiemannTrackFinderTask::Init: "
              << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  // Get input array
  //  if (FairRunAna::Instance()->IsTimeStamp()){
  //	  if (fTimeBasedHitBranch.size() == 0){
  //		  std::cout << "-W- PndMvdRiemannTrackFinderTask::Init: " << "No Branch Names given with AddHitBranch(TString branchName)! Standard BranchNames taken!" << std::endl;
  //		  fTimeBasedHitBranch.push_back(FairTSBufferParameters("MVDHitsPixel", new TimeGap(), 10));
  //	  }
  //  }
  if (fHitBranch.size() == 0) {
    std::cout << "-W- PndMvdRiemannTrackFinderTask::Init: "
              << "No Branch Names given with AddHitBranch(TString branchName)! Standard BranchNames taken!" << std::endl;
    fHitBranch.push_back("MVDHitsPixel");
    fHitBranch.push_back("MVDHitsStrip");
    fHitBranch.push_back("STTHit");
    fHitBranch.push_back("GEMHit");
  }

  for (int i = 0; i < (int)fHitBranch.size(); i++) {
    InitHitArray(fHitBranch[i]);
  }

  fTrackCandArray = new TClonesArray("PndTrackCand");
  ioman->Register("MVDRiemannTrackCand", "MVD", fTrackCandArray, fPersistence);

  fTrackArray = new TClonesArray("PndTrack");
  ioman->Register("MVDTrack", "MVD", fTrackArray, fPersistence);

  fRiemannTrackArray = new TClonesArray("PndRiemannTrack");
  //  ioman->Register("MVDRiemannTrack", "MVD", fRiemannTrackArray, kTRUE);

  fStopFunctor = new StopTime();
  fTimeGapFunctor = new TimeGap();

  PndSttMapCreator *mapper = new PndSttMapCreator(fSttParameters);
  fTubeArray = mapper->FillTubeArray();

  LOG(info) << " PndMvdRiemannTrackFinderTask: Initialisation successfull";
  fInitDone = kTRUE;
  return kSUCCESS;
}

void PndMvdRiemannTrackFinderTask::AddHitBranch(TString branchName)
{
  if (fInitDone == kFALSE)
    fHitBranch.push_back(branchName);
  else
    LOG(warn) << " AddHitBranch has to be called before the Init() of the task!";
}

// void PndMvdRiemannTrackFinderTask::AddHitBranch(FairTSBufferParameters param)
//{
//	if (fInitDone == kFALSE)
//		fTimeBasedHitBranch.push_back(param);
//	else
//		std::cout << "-W- AddHitBranch has to be called before the Init() of the task!" << std::endl;
//}

// -----   Public method Exec   --------------------------------------------
void PndMvdRiemannTrackFinderTask::Exec(Option_t *)
{

  if (fVerbose > 0)
    std::cout << std::endl;
  if (fVerbose > 0)
    std::cout << "------------- event " << fEventNr << "----------------" << std::endl;

  // Reset output array
  if (!fTrackCandArray)
    Fatal("Exec", "No trackCandArray");

  fTrackCandArray->Delete();
  fTrackArray->Delete();
  // fRiemannTrackArray->Delete();

  PndMvdSttGemRiemannTrackFinder trackFinder;
  trackFinder.SetVerbose(fVerbose);
  trackFinder.SetSttTubeArray(fTubeArray);

  FillHitArray();

  FairRootManager *ioman = FairRootManager::Instance();

  for (int i = 0; i < (int)fHitBranch.size(); i++) {
    trackFinder.AddHits(fHitArray[i], ioman->GetBranchId(fHitBranch[i]));
    if (fVerbose > 0)
      std::cout << "TrackFinder.AddHits: " << fHitBranch[i] << " Number: " << ioman->GetBranchId(fHitBranch[i]) << std::endl;
  }
  trackFinder.SetMaxSZChi2(fMaxSZChi2);
  trackFinder.SetMinPointDist(fMinPointDist);
  trackFinder.SetMaxPlaneDistance(fMaxDist);
  trackFinder.SetMaxSZDist(fMaxSZDist);
  trackFinder.SetMinNumberOfHits(4);
  // trackFinder.SetGeoH(fGeoH);

  trackFinder.SetCutDistH(fCutDistH);
  trackFinder.SetCutChi2H(fCutChi2H);

  trackFinder.FindTracks();

  if (fVerbose > 0)
    std::cout << " -I- PndMvdRiemannTrackFinderTask::Exec : Found Tracks: " << trackFinder.NTracks() << " in event no. " << fEventNr++ << std::endl;
  if (fVerbose > 0)
    std::cout << " -I- PndMvdRiemannTrackFinderTask::Exec : ----------------" << std::endl;

  for (int i = 0; i < trackFinder.NTracks(); i++) {
    PndTrackCand *myCand = new ((*fTrackCandArray)[i]) PndTrackCand(trackFinder.GetTrackCand(i));
    //	  if (fVerbose > 1)
    //	  {
    //		  std::cout << "Track " << i << std::endl;
    //		  std::cout << "Links: ";
    //		  ((FairMultiLinkedData*) myCand)->Print();
    //		  std::cout << std::endl;
    //	  }

    myCand->CalcTimeStamp();
    //	  if (fVerbose > 1)
    //		  trackFinder.GetTrack(i).Print();
    // PndRiemannTrack myTrack = trackFinder.GetTrack(i);

    PndTrack *myTrack = new ((*fTrackArray)[i]) PndTrack(trackFinder.GetPndTrack(i, fB));
    if (myCand->GetTimeStamp() == 0) {
      myTrack->SetTimeStamp(0.0001 * (i + 1));
      myCand->SetTimeStamp(0.0001 * (i + 1));

    } else {
      myTrack->SetTimeStamp(myCand->GetTimeStamp());
      myTrack->SetTimeStampError(myCand->GetTimeStampError());
    }
    myTrack->SetLink(FairLink("MVDRiemannTrackCand", i));
    myTrack->SetTrackCand(*myCand);

    if (fVerbose > 1) {
      std::cout << i << ": ";
      myTrack->Print();
    }
    //	  PndRiemannTrack* newTrack = new ((*fRiemannTrackArray)[i])PndRiemannTrack();
    //	  TVectorD origin = myTrack.orig();
    //	  newTrack->init(origin[0], origin[1], myTrack.r(), myTrack.dip(), 0);
    //	  for (int j = 0; j < myTrack.getNumHits(); j++){
    //		  newTrack->addHit(*myTrack.getHit(j));
    //	  }
  }
  fTrackCandArray->Sort();
  fTrackArray->Sort();

  for (int i = 0; i < fTrackCandArray->GetEntriesFast(); i++) {
    PndTrackCand *myCand = (PndTrackCand *)fTrackCandArray->At(i);
    if (fVerbose > 1) {
      std::cout << "Track " << i << std::endl;
      std::cout << "Links: ";
      ((FairMultiLinkedData *)myCand)->Print();
      std::cout << std::endl;
    }
  }
}

void PndMvdRiemannTrackFinderTask::FinishEvent()
{
  fTrackCandArray->Delete();
  fTrackArray->Delete();
  //	fRiemannTrackArray->Delete();
}

void PndMvdRiemannTrackFinderTask::InitHitArray(TString branchName)
{
  TClonesArray *tempArray = (TClonesArray *)FairRootManager::Instance()->GetObject(branchName);
  if (tempArray == 0) {
    std::cout << "-W- PndMvdRiemannTrackFinderTask::Init: "
              << "No hitArray for BranchName " << branchName.Data() << std::endl;
  }
  fHitArray.push_back(tempArray);
}

void PndMvdRiemannTrackFinderTask::FillHitArray()
{
  Double_t eventTime = -1;
  if (FairRunAna::Instance()->IsTimeStamp()) {
    for (size_t i = 0; i < fHitArray.size(); i++) {
      fHitArray[i]->Delete();
    }

    fHitArray[0] = FairRootManager::Instance()->GetData("MVDHitsPixel", fTimeGapFunctor, 10); // FairRootManager::Instance()->GetEventTime() +
    std::cout << "PixelHits: " << fHitArray[0]->GetEntriesFast() << std::endl;
    if (fHitArray[0]->GetEntriesFast() > 1) {
      FairTimeStamp *data = (FairTimeStamp *)fHitArray[0]->At(1);
      eventTime = data->GetTimeStamp();
      std::cout << "EventTime: " << eventTime << std::endl;
      fHitArray[1] = FairRootManager::Instance()->GetData("MVDHitsStrip", fStopFunctor, eventTime - 10, fStopFunctor, eventTime + 10);
      std::cout << "StripHits: " << fHitArray[1]->GetEntriesFast() << std::endl;
    }
  }
}

ClassImp(PndMvdRiemannTrackFinderTask);
