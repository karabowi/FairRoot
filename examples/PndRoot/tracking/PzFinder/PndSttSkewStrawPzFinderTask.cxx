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
 * PndSttSkewStrawPzFinderTask.cxx
 *
 *  Created on: Feb 8, 2016
 *      Author: walan603
 */

#include "PndSttSkewStrawPzFinderTask.h"
#include <PndSttSkewStrawPzFinderData.h>

#include <FairEventHeader.h>
#include <FairRootManager.h>
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairField.h"
#include "FairMultiLinkedData.h"
#include "FairLogger.h"

//#include "PndSttMapCreator.h"
#include "PndStt2GeoHandler.h"
#include "PndGeoSttPar.h"
#include "PndSttTube.h"
#include "PndSttHit.h"
#include "PndRiemannTrack.h"

#include "TMultiGraph.h"
#include "TVector2.h"
#include "TVector3.h"
#include "TCanvas.h"
#include "TH2D.h"

#include <iostream>

ClassImp(PndSttSkewStrawPzFinderTask);

using namespace std;

PndSttSkewStrawPzFinderTask::PndSttSkewStrawPzFinderTask() : fStoredata(false), fWithRiemann(false), fMethod(1), fStepTheta(90), fStepR(50), OutputPrefix("")
{
  FairRuntimeDb *rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fSttParameters = (PndGeoSttPar *)rtdb->getContainer("PndGeoSttPar");
}

PndSttSkewStrawPzFinderTask::~PndSttSkewStrawPzFinderTask() {}

// For sorting skew straws according to layer
struct sort_pred {
  bool operator()(const std::pair<PndSttHit, int> &left, const std::pair<PndSttHit, int> &right) { return left.second < right.second; }
};

InitStatus PndSttSkewStrawPzFinderTask::Init()
{
  FairRootManager *ioman = FairRootManager::Instance();

  FairField *Field = FairRunAna::Instance()->GetField();
  Double_t po[3], BB[3];
  po[0] = 0.;
  po[1] = 0.;
  po[2] = 0.;
  Field->GetFieldValue(po, BB);

  if (!ioman) {
    LOG(error) << " PndSttSkewStrawPzFinderTask::Init: RootManager not instantiated!";
    return kFATAL;
  }

  fEventHeader = (TClonesArray *)ioman->GetObject("EventHeader.");
  if (!fEventHeader) {
    LOG(warn) << "  PndSttSkewStrawPzFinderTask::Init: No EventHeader array! Needed for EventNumber";
    return kERROR;
  }

  if (TrackCandBranchName == "") {
    TrackCandBranchName = "CombiTrackCand";
  }
  //	fTrackCand = (TClonesArray*) ioman->GetObject("CombiTrackCand");
  //	fTrackCand = (TClonesArray*) ioman->GetObject("IdealTrackCand");
  //	fTrackCand = (TClonesArray*) ioman->GetObject("SttMvdGemTrackCand");
  fTrackCand = (TClonesArray *)ioman->GetObject(TrackCandBranchName);
  if (!fTrackCand) {
    LOG(warn) << "  PndSttSkewStrawPzFinderTask::Init: No IdealTrackCand array!";
    return kERROR;
  }

  if (TrackBranchName == "") {
    TrackBranchName = "CombiTrack";
  }
  //	fTrack = (TClonesArray*) ioman->GetObject("CombiTrack");
  //	fTrack = (TClonesArray*) ioman->GetObject("IdealTrack");
  //	fTrack = (TClonesArray*) ioman->GetObject("SttMvdGemTrack");
  fTrack = (TClonesArray *)ioman->GetObject(TrackBranchName);
  if (!fTrack) {
    LOG(warn) << "  PndSttSkewStrawPzFinderTask::Init: No IdealTrack array!";
    return kERROR;
  }

  if (fWithRiemann) {
    if (TrackRiemannBranchName == "") {
      TrackRiemannBranchName = "CombiRiemannTrack";
    }
    //	fRiemannTrack = (TClonesArray*) ioman->GetObject("CombiRiemannTrack");
    fRiemannTrack = (TClonesArray *)ioman->GetObject(TrackRiemannBranchName);
    if (!fRiemannTrack) {
      LOG(warn) << "  PndSttSkewStrawPzFinderTask::Init: No CombiRiemannTrack array!";
      return kERROR;
    }
  }

  fSTTHits = (TClonesArray *)ioman->GetObject("STTHit");
  if (!fSTTHits) {
    cout << "-W- PndSttSkewStrawPzFinderTask::Init: fSTTHits not instantiated,"
            " return!"
         << endl;
    return kERROR;
  }

  fMVDHitsPixel = (TClonesArray *)ioman->GetObject("MVDHitsPixel");
  if (!fMVDHitsPixel) {
    cout << "-W- PndSttSkewStrawPzFinderTask::Init: fMVDHitsPixel not instantiated,"
            " return!"
         << endl;
    return kERROR;
  }

  fMVDHitsStrip = (TClonesArray *)ioman->GetObject("MVDHitsStrip");
  if (!fMVDHitsStrip) {
    cout << "-W- PndSttSkewStrawPzFinderTask::Init: fMVDHitsStrip not instantiated,"
            " return!"
         << endl;
    return kERROR;
  }

  fGEMHits = (TClonesArray *)ioman->GetObject("GEMHit");
  if (!fGEMHits) {
    cout << "-W- PndSttSkewStrawPzFinderTask::Init: fGEMHits not instantiated,"
            " return!"
         << endl;
    return kERROR;
  }

  if (OutputPrefix == "") {
    OutputPrefix = "Final";
  }
  fFinalTrackCand = ioman->Register(OutputPrefix + "TrackCand", "PndTrackCand", "STT", kTRUE);

  fFinalTrack = ioman->Register(OutputPrefix + "Track", "PndTrack", "STT", kTRUE);

  fFinalRiemannTrack = ioman->Register(OutputPrefix + "RiemannTrack", "PndRiemannTrack", "STT", kTRUE);

  if (fStoredata) {
    fFinalPzData = ioman->Register(OutputPrefix + "PzData", "PndSttSkewStrawPzFinderData", "STT", kTRUE);
  }

  // Get tube array
  fTubeArray = PndStt2GeoHandler::Instance(fSttParameters)->GetTubeArray();
  //PndStt2GeoHandler *geoH = new PndStt2GeoHandler(fSttParameters);
  //fTubeArray = geoH->GetTubeArray();
  //PndSttMapCreator *mapper = new PndSttMapCreator(fSttParameters);
  //fTubeArray = mapper->FillTubeArray();

  // Instantiate PzFinder
  PzFinder = new PndSttSkewStrawPzFinder(fTubeArray, fSTTHits);
  PzFinder->WithRiemann(false);
  PzFinder->setMethod(fMethod);
  PzFinder->setStepR(fStepR);
  PzFinder->setStepTheta(fStepTheta);
  LOG(info) << " PndSttSkewStrawPzFinderTask: Initialization successful";
  return kSUCCESS;
}

void PndSttSkewStrawPzFinderTask::Exec(Option_t *)
{
  FairRootManager *ioman = FairRootManager::Instance();

  PzFinder->Reset();

  FairEventHeader *myEventHeader = (FairEventHeader *)fEventHeader;
  Int_t eventNumber = myEventHeader->GetMCEntryNumber();

  cout << "---" << endl;
  cout << "Exec event: " << eventNumber << endl;
  cout << "---" << endl;

  /* Filling std vectors with PndSttHits of each track, the PndTrack and PndRiemannTrack
   * for each corresponding track candidate produced by the SttCellTrackFinder.
   */

  vector<vector<PndSttHit>> AllSkewedHits;
  vector<vector<FairHit>> AllMvdPixelHits;
  vector<vector<FairHit>> AllMvdStripHits;
  vector<vector<FairHit>> AllGemHits;
  vector<PndTrackCand> AllTrackCands;
  vector<PndRiemannTrack> AllRiemannTracks;
  vector<PndTrack> AllTracks;

  for (int i = 0; i < fTrack->GetEntriesFast(); ++i) {
    AllTracks.push_back(*(PndTrack *)(fTrack->At(i)));
  }

  for (int i = 0; i < fTrackCand->GetEntriesFast(); ++i) {
    AllTrackCands.push_back(*(PndTrackCand *)(fTrackCand->At(i)));
  }
  if (fWithRiemann) {
    for (int i = 0; i < fRiemannTrack->GetEntriesFast(); ++i) {
      AllRiemannTracks.push_back(*(PndRiemannTrack *)(fRiemannTrack->At(i)));
    }
  }

  PndTrackCand myTrackCand;
  vector<PndTrackCandHit> tracklist;
  PndSttHit hit;
  FairHit mvdhitpixel;
  FairHit mvdhitstrip;
  FairHit gems;
  PndSttTube myTube;
  for (int i = 0; i < fTrackCand->GetEntriesFast(); ++i) {
    myTrackCand = *(PndTrackCand *)fTrackCand->At(i);
    tracklist = myTrackCand.GetSortedHits();
    vector<pair<PndSttHit, int>> hitPairs;
    vector<FairHit> mvdpixelhits;
    vector<FairHit> mvdstriphits;
    vector<FairHit> gemhits;
    for (unsigned int j = 0; j < tracklist.size() - 1; ++j) {
      if (tracklist.at(j).GetDetId() == ioman->GetBranchId("STTHit")) {

        hit = *(PndSttHit *)fSTTHits->At(tracklist.at(j).GetHitId());
        myTube = *(PndSttTube *)fTubeArray->At(hit.GetTubeID());
        if (myTube.IsSkew()) {
          //					hitPairs.push_back(pair<PndSttHit,int> (hit,hit.GetTubeID()));
          hitPairs.push_back(pair<PndSttHit, int>(hit, myTube.GetLayerID()));
        }
        // Sorting the skewed hits based on their layer ID
        std::sort(hitPairs.begin(), hitPairs.end(), sort_pred());
      }
      if (tracklist.at(j).GetDetId() == ioman->GetBranchId("MVDHitsPixel")) {
        mvdhitpixel = *(FairHit *)fMVDHitsPixel->At(tracklist.at(j).GetHitId());
        mvdpixelhits.push_back(FairHit(mvdhitpixel));
      }
      if (tracklist.at(j).GetDetId() == ioman->GetBranchId("MVDHitsStrip")) {
        mvdhitstrip = *(FairHit *)fMVDHitsStrip->At(tracklist.at(j).GetHitId());
        mvdstriphits.push_back(FairHit(mvdhitstrip));
      }
      if (tracklist.at(j).GetDetId() == ioman->GetBranchId("GEMHit")) {
        gems = *(FairHit *)fGEMHits->At(tracklist.at(j).GetHitId());
        gemhits.push_back(FairHit(gems));
      }
    }

    vector<PndSttHit> skewHits;
    for (unsigned int j = 0; j < hitPairs.size(); j++) {
      skewHits.push_back(hitPairs.at(j).first);
    }

    AllSkewedHits.push_back(skewHits);
    AllMvdPixelHits.push_back(mvdpixelhits);
    AllMvdStripHits.push_back(mvdstriphits);
    AllGemHits.push_back(gemhits);
  }

  // Add all PndTracks in one event. Add corresponding Riemann track for each track.
  // Add also list of skewed Stt hits for each track.
  PzFinder->AddPndTracks(AllTracks);
  PzFinder->AddPndTrackCands(AllTrackCands);
  PzFinder->WithRiemann(fWithRiemann);
  if (fWithRiemann) {
    PzFinder->AddPndRiemannTracks(AllRiemannTracks);
  }

  PzFinder->AddPndSttHits(AllSkewedHits);
  PzFinder->AddMvdPixelHits(AllMvdPixelHits);
  PzFinder->AddMvdStripHits(AllMvdStripHits);
  PzFinder->AddGemHits(AllGemHits);
  PzFinder->ExtractPz();

  // Get all tracks where z component was successfully found
  for (int i = 0; i < PzFinder->NumPndTrackCands(); ++i) {
    PndTrackCand *myCand = new ((*fFinalTrackCand)[i]) PndTrackCand(PzFinder->GetTrackCand(i));
    PndTrack *myTrack = new ((*fFinalTrack)[i]) PndTrack(PzFinder->GetTrack(i));
    myTrack->SetTrackCandRef(myCand);
    myTrack->SetTrackCand(*myCand);
    myTrack->SetLinks(*myCand->GetPointerToLinks());
  }

  for (int i = 0; i < PzFinder->NumPndRiemannTracks(); ++i) {
    PndRiemannTrack *myRiemannTrack = new ((*fFinalRiemannTrack)[i]) PndRiemannTrack(PzFinder->GetRiemannTrack(i));
  }

  if (fStoredata) {
    for (int i = 0; i < PzFinder->NumPndPzData(); ++i) {
      PndSttSkewStrawPzFinderData *myPzData = new ((*fFinalPzData)[i]) PndSttSkewStrawPzFinderData(PzFinder->GetPzData(i));
    }
  }
}

void PndSttSkewStrawPzFinderTask::FinishEvent()
{
  if (fTrackCand)
    fTrackCand->Delete();
  if (fTrack)
    fTrack->Delete();

  if (fWithRiemann)
    fRiemannTrack->Delete();

  fVectorPndTrackCand.clear();
  fVectorPndTrack.clear();

  if (fFinalTrackCand)
    fFinalTrackCand->Delete();
  if (fFinalTrack)
    fFinalTrack->Delete();
  if (fFinalRiemannTrack)
    fFinalRiemannTrack->Delete();
  if (fStoredata)
    fFinalPzData->Delete();
}
