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
 * PndSttCellTrackFinderAnalysisTask.cpp
 *
 *  Created on: Jun 26, 2013
 *      Author: schumann
 */

// Root includes
#include "TROOT.h"
#include "TClonesArray.h"

// framework includes
#include "FairRunAna.h"
#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairHit.h"
#include "FairEventHeader.h"
#include "FairLogger.h"

#include "PndSttCellTrackFinderAnalysisTask.h"
#include "PndTrackCand.h"
#include "PndTrackCandHit.h"
#include "PndRiemannTrack.h"
#include "PndMCMatch.h"
#include "PndMCEntry.h"
#include "PndMCResult.h"
#include "PndMCTrack.h"
//#include "PndSttMapCreator.h"
#include "PndStt2GeoHandler.h"
#include "PndSttHit.h"
#include "PndGeoSttPar.h"
#include "PndSttTube.h"

#include "TGraph.h"
#include "TArc.h"
#include "TLine.h"
#include "TMath.h"

#include <iostream>
#include <vector>
#include <set>
#include <string>

ClassImp(PndSttCellTrackFinderAnalysisTask);

using namespace std;

void PndSttCellTrackFinderAnalysisTask::SetParContainers()
{
  FairRuntimeDb *rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fSttParameters = (PndGeoSttPar *)rtdb->getContainer("PndGeoSttPar");
}

InitStatus PndSttCellTrackFinderAnalysisTask::Init()
{

  FairRootManager *ioman = FairRootManager::Instance();

  if (!ioman) {
    cout << "-E- PndSttCellTrackFinderAnalysisTask::Init: "
         << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  fMCMatch = (PndMCMatch *)ioman->GetObject("MCMatch");

  if (!fMCMatch) {
    LOG(warn) << "  PndSttCellTrackFinderAnalysisTask::Init: No MCMatch array! Needed for MC Truth";
    return kERROR;
  }

  fEventHeader = (TClonesArray *)ioman->GetObject("EventHeader.");
  if (!fEventHeader) {
    LOG(warn) << "  PndSttCellTrackFinderAnalysisTask::Init: No EventHeader array! Needed for EventNumber";
    return kERROR;
  }

  // get data-object
  fMCTrack = (TClonesArray *)ioman->GetObject("MCTrack");
  fFirstTrackCand = (TClonesArray *)ioman->GetObject("FirstTrackCand");
  fFirstRiemannTrack = (TClonesArray *)ioman->GetObject("FirstRiemannTrack");
  fCombiTrackCand = (TClonesArray *)ioman->GetObject("CombiTrackCand");
  fCombiRiemannTrack = (TClonesArray *)ioman->GetObject("CombiRiemannTrack");
  fSTTHits = (TClonesArray *)ioman->GetObject("STTHit");

  // get tube-array
  fTubeArray = PndStt2GeoHandler::Instance(fSttParameters)->GetTubeArray();
  //PndSttMapCreator *mapper = new PndSttMapCreator(fSttParameters);
  //fTubeArray = mapper->FillTubeArray();

  // Initializing histograms
  fHistoNumberOfLinks1 = new TH1I("fHistoNumberOfLinks1", "Number Of Links Per Track Candidate ; # Links  ;  # Track Candidates ", 5, -0.5, 4.5);

  fHistoNumberOfTracklets1 = new TH1I("fHistoNumberOfTracklets1", "Number Of Tracklets Per MCTrack; #Tracklets ;  # MCTracks ", 20, -0.5, 19.5);

  fHistoNumberOfAssignedHits1 = new TH1I("fHistoNumberOfAssignedHits1", "Ratio Of Assigned And Unassigned STTHits ;  ;  # Hits ", 2, -0.5, 1.5);

  fHistoNumberOfHits1 = new TH1I("fHistoNumberOfHits1", "Number Of Hits Per Tracklet ; # Hits ;  # Tracklets ", 40, -0.5, 39.5);

  fHistoNumberOfErrors1 = new TH1I("fHistoNumberOfErrors1", "Number Of Errors Per Event ; # Errors ;  # Events ", 11, -1.5, 9.5);

  fHistoNumberOfLinks2 = new TH1I("fHistoNumberOfLinks2", "Number Of Links Per Track Candidate ; # Links  ;  # Track Candidates ", 5, -0.5, 4.5);

  fHistoNumberOfTracklets2 = new TH1I("fHistoNumberOfTracklets2", "Number Of Tracklets Per MCTrack; # Tracklets ;  # MCTracks ", 25, -0.5, 24.5);

  fHistoNumberOfHits2 = new TH1I("fHistoNumberOfHits2", "Number Of Hits Per Tracklet ; # Hits ;  # Tracklets ", 40, -0.5, 39.5);

  fHistoNumberOfErrors2 = new TH1I("fHistoNumberOfErrors2", "Number Of Errors Per Event ; # Errors ;  # Events ", 11, -1.5, 9.5);

  fHistoQualityCombi = new TH1I("fHistoQualityCombi", "Quality Of Trackfinding; ; # Tracks", 8, -0.5, 7.5);

  fHistoQualityFirstStep = new TH1I("fHistoQualityFirstStep", "Quality Of Trackfinding; ; # Tracks", 8, -0.5, 7.5);

  fHistoMaxHitsOfMCTrack1 = new TH1I("fHistoMaxHitsOfMCTrack1", "Max Number Of Hits At A Stretch Per MCTrack; # Hits; # MCTracks", 40, -0.5, 39.5);

  fHistoMaxHitsOfMCTrack2 = new TH1I("fHistoMaxHitsOfMCTrack2", "Max Number Of Hits At A Stretch Per MCTrack; # Hits; # MCTracks", 40, -0.5, 39.5);

  fHistoNumberOfMissingHits1 = new TH1I("fHistoNumberOfMissingHits1", "Amount Of Missing Hits Of Spurious Tracklets; Missing Hits in %; # Tracklets", 4, -0.5, 3.5);

  fHistoNumberOfMissingHits2 = new TH1I("fHistoNumberOfMissingHits2", "Amount Of Missing Hits Of Spurious Tracklets; Missing Hits in %; # Tracklets", 4, -0.5, 3.5);

  // set labels
  fHistoQualityFirstStep->GetXaxis()->SetBinLabel(1, "#MCTracks");
  fHistoQualityFirstStep->GetXaxis()->SetBinLabel(2, "#Found in pure");
  fHistoQualityFirstStep->GetXaxis()->SetBinLabel(3, "#Found in faulty");
  fHistoQualityFirstStep->GetXaxis()->SetBinLabel(4, "#Not Found");
  fHistoQualityFirstStep->GetXaxis()->SetBinLabel(5, "#TrackCands");
  fHistoQualityFirstStep->GetXaxis()->SetBinLabel(6, "#Spuriuous Cands");
  fHistoQualityFirstStep->GetXaxis()->SetBinLabel(7, "#FullyReco Cands");
  fHistoQualityFirstStep->GetXaxis()->SetBinLabel(8, "#Ghost Cands");

  fHistoQualityCombi->GetXaxis()->SetBinLabel(1, "#MCTracks");
  fHistoQualityCombi->GetXaxis()->SetBinLabel(2, "#Found in pure");
  fHistoQualityCombi->GetXaxis()->SetBinLabel(3, "#Found in faulty");
  fHistoQualityCombi->GetXaxis()->SetBinLabel(4, "#Not Found");
  fHistoQualityCombi->GetXaxis()->SetBinLabel(5, "#TrackCands");
  fHistoQualityCombi->GetXaxis()->SetBinLabel(6, "#Spuriuous Cands");
  fHistoQualityCombi->GetXaxis()->SetBinLabel(7, "#FullyReco Cands");
  fHistoQualityCombi->GetXaxis()->SetBinLabel(8, "#Ghost Cands");

  fHistoNumberOfMissingHits1->GetXaxis()->SetBinLabel(1, ">0");
  fHistoNumberOfMissingHits1->GetXaxis()->SetBinLabel(2, ">25");
  fHistoNumberOfMissingHits1->GetXaxis()->SetBinLabel(3, ">50");
  fHistoNumberOfMissingHits1->GetXaxis()->SetBinLabel(4, ">75");

  fHistoNumberOfMissingHits2->GetXaxis()->SetBinLabel(1, ">0");
  fHistoNumberOfMissingHits2->GetXaxis()->SetBinLabel(2, ">25");
  fHistoNumberOfMissingHits2->GetXaxis()->SetBinLabel(3, ">50");
  fHistoNumberOfMissingHits2->GetXaxis()->SetBinLabel(4, ">75");

  fHistoNumberOfHitsMCTrack = new TH1I("fHistoNumberOfHitsMCTrack", "Number Of Hits Per MCTrack; # Hits; # MCTracks", 70, -0.5, 69.5);

  fCanvas = new TCanvas();
  fCanvas->SetName("fCanvas");
  fCanvas->SetGrid();
  fCanvas->SetCanvasSize(700, 700);

  if (fVerbose == 2) {
    LOG(info) << " PndSttCellTrackFinderAnalysisTask::Init: Initialization successfull";
  }

  return kSUCCESS;
}

void PndSttCellTrackFinderAnalysisTask::Exec(Option_t *opt)
{

  if (fVerbose > 0) {
    cout << "============= Begin PndSttCellTrackFinderAnalysisTask::Exec" << endl;
    cout << endl;
  }

  // init fMapTubeIDToHits
  PndSttHit *hit;
  for (int j = 0; j < fSTTHits->GetEntriesFast(); ++j) {
    hit = (PndSttHit *)fSTTHits->At(j);
    fMapTubeIDToHits[hit->GetTubeID()].push_back(hit->GetLink(0).GetIndex());
    fMapHitIndexToTubeID[hit->GetLink(0).GetIndex()] = hit->GetTubeID();
  }

  fMCMatch->CreateArtificialStage("MCTrack");

  CheckFirstTracklets();
  TestRecoQualityFirstStep();
  CountMaxHitsFirstStep();

  CheckTrackletCombinations();
  TestRecoQualityCombi();
  CountMaxHitsCombi();
}

void PndSttCellTrackFinderAnalysisTask::CheckFirstTracklets()
{

  if (fVerbose > 1)
    cout << "=== CheckFirstTracklets() ===" << endl;

  // PndMCTrack* myMCTrack; //[R.K. 01/2017] unused variable?
  PndTrackCand *myTrackCand;
  vector<PndTrackCandHit> hits;
  int eventNumber;

  FairRootManager *ioman = FairRootManager::Instance();
  FairEventHeader *myEventHeader = (FairEventHeader *)fEventHeader;

  // get FairLinks of stored data
  PndMCResult myResultCandToMCTrack = fMCMatch->GetMCInfo("FirstTrackCand", "MCTrack");
  PndMCResult myResultHitsToMCTrack = fMCMatch->GetMCInfo("STTHit", "MCTrack");
  PndMCResult myResultHitToPoint = fMCMatch->GetMCInfo("STTHit", "STTPoint");

  eventNumber = myEventHeader->GetMCEntryNumber() - 1;

  if (fVerbose > 2) {
    cout << "EventNumber: " << eventNumber << endl;
    cout << "# TrackCand: " << myResultCandToMCTrack.GetNEntries() << endl;
  }

  // variables for histograms
  map<int, int> trackletsCounter; // map<MCTrack-Index, #Tracklets>
  int assignedHits = 0;
  int errorCounter = 0;

  int numberOfLinks;

  // check tracklets
  for (int i = 0; i < fFirstTrackCand->GetEntriesFast(); ++i) {

    // increase tracklet-counter for actual MCTrack-Index
    for (int j = 0; j < myResultCandToMCTrack.GetEntry(i).GetNLinks(); ++j) {
      trackletsCounter[myResultCandToMCTrack.GetEntry(i).GetLink(j).GetIndex()] += 1;
    }

    // get current TrackCand
    myTrackCand = (PndTrackCand *)fFirstTrackCand->At(i);

    // get all FairLinks of STTHits
    FairMultiLinkedData hitLinks = myTrackCand->GetLinksWithType(ioman->GetBranchId("STTHit"));

    // add number of STTHits of the tracklet to the assigned hits
    assignedHits += hitLinks.GetNLinks();
    fHistoNumberOfHits1->Fill(hitLinks.GetNLinks());

    // clean tracklet?
    if (myResultCandToMCTrack.GetEntry(i).GetNLinks() > 1) {
      // hits of tracklet point to more than one MCTrack

      // there could be more than one hit per tube -> get "real" number of different links
      numberOfLinks = GetNumLinksOfHits(hitLinks, myResultHitsToMCTrack, myResultHitToPoint);

      // fill histo with number of elements in the set (only different ones were stored)
      fHistoNumberOfLinks1->Fill(numberOfLinks);

      if (numberOfLinks > 1) {
        // faulty tracklet
        ++errorCounter;

        if (fVerbose > 2) {
          cout << "Warning: Found more than one TrackID!" << endl;
          cout << "Nlinks:" << myResultCandToMCTrack.GetEntry(i).GetNLinks() << endl;
        }
      }

    } else {
      // only one link to MCTrack
      fHistoNumberOfLinks1->Fill(1);
    }
  }

  // store data of tracklet-counter in histogram
  map<int, int>::iterator it;
  for (it = trackletsCounter.begin(); it != trackletsCounter.end(); ++it) {
    fHistoNumberOfTracklets1->Fill(it->second);
  }

  // store data of assigned and unassigned hits in histogram
  for (int i = 0; i < assignedHits; ++i) {
    fHistoNumberOfAssignedHits1->Fill("Assigned", 1);
  }
  for (int i = 0; i < fSTTHits->GetEntriesFast() - assignedHits; ++i) {
    fHistoNumberOfAssignedHits1->Fill("Unassigned", 1);
  }

  // store errcorCounter of this event in histogram
  fHistoNumberOfErrors1->Fill(errorCounter);

  //	// plot STTHits + Riemann-circle for the first 100 events
  //	if (eventNumber < 100) {
  //		DrawFirstRiemannPlots(eventNumber);
  //	}
}

void PndSttCellTrackFinderAnalysisTask::CheckTrackletCombinations()
{

  if (fVerbose > 1)
    cout << "=== CheckTrackletCombinations() ===" << endl;
  // PndMCTrack* myMCTrack; //[R.K. 01/2017] unused variable?
  PndTrackCand *myTrackCand;
  vector<PndTrackCandHit> hits;
  int eventNumber;

  FairRootManager *ioman = FairRootManager::Instance();
  FairEventHeader *myEventHeader = (FairEventHeader *)fEventHeader;

  // get FairLinks of stored data
  PndMCResult myResultCandToMCTrack = fMCMatch->GetMCInfo("CombiTrackCand", "MCTrack");
  PndMCResult myResultHitsToMCTrack = fMCMatch->GetMCInfo("STTHit", "MCTrack");
  PndMCResult myResultHitToPoint = fMCMatch->GetMCInfo("STTHit", "STTPoint");

  eventNumber = myEventHeader->GetMCEntryNumber() - 1;

  if (fVerbose > 2) {
    cout << "EventNumber: " << eventNumber << endl;
    cout << "# TrackCand: " << myResultCandToMCTrack.GetNEntries() << endl;
  }

  // variables for histogram
  map<int, int> trackletsCounter; // map<MCTrack-Index, #Tracklets>
  int errorCounter = 0;

  int numberOfLinks;

  // check each tracklet
  for (int i = 0; i < fCombiTrackCand->GetEntriesFast(); ++i) {

    // increase tracklet-counter for actual MCTrack-Index
    for (int j = 0; j < myResultCandToMCTrack.GetEntry(i).GetNLinks(); ++j) {
      trackletsCounter[myResultCandToMCTrack.GetEntry(i).GetLink(j).GetIndex()] += 1;
    }

    // get current TrackCand
    myTrackCand = (PndTrackCand *)fCombiTrackCand->At(i);

    // get all FairLinks of STTHits
    FairMultiLinkedData hitLinks = myTrackCand->GetLinksWithType(ioman->GetBranchId("STTHit"));

    // fill histo with number of hits of this trackCand
    fHistoNumberOfHits2->Fill(hitLinks.GetNLinks());

    // clean tracklet?
    if (myResultCandToMCTrack.GetEntry(i).GetNLinks() > 1) {

      // there could be more than one hit per tube -> get "real" number of different links
      numberOfLinks = GetNumLinksOfHits(hitLinks, myResultHitsToMCTrack, myResultHitToPoint);

      // fill histogram with number of elements in the set (only different ones were stored)
      fHistoNumberOfLinks2->Fill(numberOfLinks);

      if (numberOfLinks > 1) {
        // faulty tracklet
        ++errorCounter;

        if (fVerbose > 2) {
          cout << "Warning: Found more than one TrackID!" << endl;
          cout << "Nlinks:" << myResultCandToMCTrack.GetEntry(i).GetNLinks() << endl;
        }
      }

    } else {
      // only one link to MCTrack
      fHistoNumberOfLinks2->Fill(1);
    }
  }

  // store data of tracklet-counter in histogram
  map<int, int>::iterator it;
  for (it = trackletsCounter.begin(); it != trackletsCounter.end(); ++it) {
    fHistoNumberOfTracklets2->Fill(it->second);
  }

  // store errorCounter ot this event in histogramm
  fHistoNumberOfErrors2->Fill(errorCounter);

  //	// draw STTHits + Riemann-circle for the first 100 events
  //	if (eventNumber < 100) {
  //		DrawCombiRiemannPlots(eventNumber);
  //	}
}

int PndSttCellTrackFinderAnalysisTask::GetNumLinksOfHits(FairMultiLinkedData &hitLinks, PndMCResult &sttHitsToMCTrack, PndMCResult &sttHitToPoint)
{

  if (fVerbose > 3)
    cout << "GetNumLinksOfHits()" << endl;

  /* Check hits of a tracklet in hitLinks. Approach: check MCTrack-index of consecutive hits.
   * If they are not equal: tracklet might be faulty -> search for another hit in the same tube with
   * the faulty hit (if available) and get index of MCTrack. Count the different MCTrack-Indices.*/

  /* Set containing MCTrack-Indices of STTHits of one PndTrackCand.
   * If it contains more than one Index, the tracklet ist faulty. */
  set<int> trackIndices;

  /* PriorTrackIndex and curTrackIndex of prior and current hit in tracklet should be the same.*/
  int priorTrackIndex;
  int curTrackIndex;
  int priorHitIndex;
  int curHitIndex;

  int tmpHitIndex;
  int tubeID;
  FairLink curLink;

  // get MCTrackIndex the first hit points to
  priorHitIndex = hitLinks.GetLink(0).GetIndex();
  priorTrackIndex = sttHitsToMCTrack.GetEntry(priorHitIndex).GetLink(0).GetIndex();

  // only one hit?
  if (hitLinks.GetNLinks() < 2) {
    return sttHitsToMCTrack.GetEntry(priorHitIndex).GetNLinks();
  }

  if (fVerbose > 3)
    cout << "TubeID: " << fMapHitIndexToTubeID[priorHitIndex] << ", TrackID: " << priorTrackIndex << endl;

  // check each hit of myTrackCand: compare MCTrackIndex of prior and current hit
  for (int j = 1; j < hitLinks.GetNLinks(); ++j) {

    // get MCTrackIndex the current Hit points to
    curLink = hitLinks.GetLink(j);
    // !! index of STTPoint and STTHit must not be the same !!
    curHitIndex = sttHitToPoint.GetEntry(curLink.GetIndex()).GetLink(0).GetIndex();
    curTrackIndex = sttHitsToMCTrack.GetEntry(curHitIndex).GetLink(0).GetIndex();

    if (fVerbose > 3)
      cout << "TubeID: " << fMapHitIndexToTubeID[curHitIndex] << ", TrackID: " << curTrackIndex << endl;

    // consecutive hits doesn't point to same MCTrack?
    if (priorTrackIndex != curTrackIndex) {

      // more than one hit per tube?
      if (fMapTubeIDToHits[fMapHitIndexToTubeID[curHitIndex]].size() > 1) {
        // tube of current hit has more than one hit

        tubeID = fMapHitIndexToTubeID[curHitIndex];

        // get hitIndex of the other hit
        if (curHitIndex == fMapTubeIDToHits[tubeID].at(0)) {
          tmpHitIndex = fMapTubeIDToHits[tubeID].at(1);
        } else {
          tmpHitIndex = fMapTubeIDToHits[tubeID].at(0);
        }

        // insert trackIndex of the other hit into set
        trackIndices.insert(sttHitsToMCTrack.GetEntry(tmpHitIndex).GetLink(0).GetIndex());

      } else if (fMapTubeIDToHits[fMapHitIndexToTubeID[priorHitIndex]].size() > 1) {
        // tube of prior hit has more than one hit

        tubeID = fMapHitIndexToTubeID[priorHitIndex];

        // get hitIndex of the other hit
        if (priorHitIndex == fMapTubeIDToHits[tubeID].at(0)) {
          tmpHitIndex = fMapTubeIDToHits[tubeID].at(1);
        } else {
          tmpHitIndex = fMapTubeIDToHits[tubeID].at(0);
        }

        // insert trackIndex of the other prior hit into set
        trackIndices.insert(sttHitsToMCTrack.GetEntry(tmpHitIndex).GetLink(0).GetIndex());
        // inset trackIndex of current hit
        trackIndices.insert(curTrackIndex);

      } else {
        // only one hit per tube -> insert current trackIndex into set
        trackIndices.insert(curTrackIndex);

        if (fVerbose > 3)
          cout << "---------> TubeID of faulty hit: " << fMapHitIndexToTubeID[curHitIndex] << endl;
      }

    } else {
      // same trackIndices
      trackIndices.insert(curTrackIndex);
    }

    // update data of prior hit
    priorTrackIndex = curTrackIndex;
    priorHitIndex = curHitIndex;
  }

  return trackIndices.size();
}

void PndSttCellTrackFinderAnalysisTask::DrawFirstRiemannPlots(int eventNumber)
{

  PndMCResult myResultHitToPoint = fMCMatch->GetMCInfo("STTHit", "STTPoint");
  PndTrackCand *myTrackCand;
  int curHitIndex;

  fCanvas->Clear();
  fCanvas->SetTitle("STTHits and circle fits for event #" + TString::Itoa(eventNumber, 10));

  TGraph *graph = new TGraph();
  int hitCounter = 0; // for counting points of TGraph

  /* Draw points of all STTHits of each TrackCand.
   * Hits of skewed tubes were ignored.*/
  for (int i = 0; i < fFirstTrackCand->GetEntriesFast(); ++i) {

    // get current TrackCand
    myTrackCand = (PndTrackCand *)fFirstTrackCand->At(i);
    // get all FairLinks of STTHits
    FairMultiLinkedData hitLinks = myTrackCand->GetLinksWithType(FairRootManager::Instance()->GetBranchId("STTHit"));

    // for each STTHit
    for (int j = 0; j < hitLinks.GetNLinks(); ++j) {

      // Get HitIndex -> get Tube -> Get Position
      curHitIndex = hitLinks.GetLink(j).GetIndex();
      PndSttTube *tube = (PndSttTube *)fTubeArray->At(fMapHitIndexToTubeID[curHitIndex]);
      TVector3 pos = tube->GetPosition();

      // ignore skewed straw tubes
      if (!tube->IsSkew()) {
        graph->SetPoint(hitCounter, pos[0], pos[1]);
        ++hitCounter;
      }
    }
  }

  // draw graph
  graph->Draw("SAMEA*");
  graph->GetXaxis()->SetLimits(-50, 50);
  graph->GetYaxis()->SetRangeUser(-50, 50);

  // draw circles of riemannTracks

  PndRiemannTrack *myRiemannTrack;
  double r, x, y;

  // for each riemannTrack
  for (int i = 0; i < fFirstRiemannTrack->GetEntriesFast(); ++i) {
    myRiemannTrack = (PndRiemannTrack *)fFirstRiemannTrack->At(i);

    // get radius + position of center
    r = myRiemannTrack->r();
    x = myRiemannTrack->orig()[0];
    y = myRiemannTrack->orig()[1];

    // draw circle
    TArc *circle = new TArc(x, y, r);
    circle->SetFillStyle(0);
    circle->Draw("SAME");
  }

  // mark center of STT
  TLine *xLine = new TLine(-50, 0, 50, 0);
  TLine *yLine = new TLine(0, 50, 0, -50);
  xLine->Draw("SAME");
  yLine->Draw("SAME");

  // save plot
  fCanvas->Print("Plots/FirstTracklets/Event" + TString::Itoa(eventNumber, 10) + ".pdf", "pdf");
}

void PndSttCellTrackFinderAnalysisTask::DrawCombiRiemannPlots(int eventNumber)
{

  PndMCResult myResultHitToPoint = fMCMatch->GetMCInfo("STTHit", "STTPoint");
  PndTrackCand *myTrackCand;
  int curHitIndex;

  fCanvas->Clear();
  fCanvas->SetTitle("STTHits and circle fits for event #" + TString::Itoa(eventNumber, 10));

  TGraph *graph = new TGraph();
  int hitCounter = 0; // for counting the points of TGraph

  /* Draw points of all STTHits of each TrackCand.
   * Hits of skewed tubes were ignored.*/
  for (int i = 0; i < fCombiTrackCand->GetEntriesFast(); ++i) {

    // get current TrackCand
    myTrackCand = (PndTrackCand *)fCombiTrackCand->At(i);
    // get all FairLinks of STTHits
    FairMultiLinkedData hitLinks = myTrackCand->GetLinksWithType(FairRootManager::Instance()->GetBranchId("STTHit"));

    // for each STTHit
    for (int j = 0; j < hitLinks.GetNLinks(); ++j) {

      // get HitIndex -> get Tube -> get Position
      curHitIndex = hitLinks.GetLink(j).GetIndex();
      PndSttTube *tube = (PndSttTube *)fTubeArray->At(fMapHitIndexToTubeID[curHitIndex]);
      TVector3 pos = tube->GetPosition();

      // ignore skewed straw tubes
      if (!tube->IsSkew()) {
        graph->SetPoint(hitCounter, pos[0], pos[1]);
        ++hitCounter;
      }
    }
  }

  // draw graph
  graph->Draw("SAMEA*");
  graph->GetXaxis()->SetLimits(-50, 50);
  graph->GetYaxis()->SetRangeUser(-50, 50);

  // draw circles of riemannTracks
  PndRiemannTrack *myRiemannTrack;
  double r, x, y;

  // for each riemannTrack
  for (int i = 0; i < fCombiRiemannTrack->GetEntriesFast(); ++i) {
    myRiemannTrack = (PndRiemannTrack *)fCombiRiemannTrack->At(i);

    // get radius + position of center
    r = myRiemannTrack->r();
    x = myRiemannTrack->orig()[0];
    y = myRiemannTrack->orig()[1];

    // draw circle
    TArc *circle = new TArc(x, y, r);
    circle->SetFillStyle(0);
    circle->Draw("SAME");
  }

  // marl center of STT
  TLine *xLine = new TLine(-50, 0, 50, 0);
  TLine *yLine = new TLine(0, 50, 0, -50);
  xLine->Draw("SAME");
  yLine->Draw("SAME");

  // save plot
  fCanvas->Print("Plots/Combinations/EventCombi" + TString::Itoa(eventNumber, 10) + ".pdf", "pdf");
}

void PndSttCellTrackFinderAnalysisTask::CheckConformalMap()
{

  TCanvas *test = new TCanvas("test", "Circle");
  test->SetGrid();
  TGraph *circlePoints = new TGraph();
  TGraph *linePoints = new TGraph();
  int pointCounter = 0;

  double strawHits[6][2] = {{-1, 2}, {0, 3}, {1, 2}, {2, 3}, {3, 2}, {4, 3}};
  double mappedHits[6][2];
  double div;

  double refHitX = strawHits[0][0];
  double refHitY = strawHits[0][1];

  for (int i = 0; i < 6; ++i) {

    div = (strawHits[i][0] - refHitX) * (strawHits[i][0] - refHitX) + (strawHits[i][1] - refHitY) * (strawHits[i][1] - refHitY);
    mappedHits[i][0] = (strawHits[i][0] - refHitX) / div;
    mappedHits[i][1] = -(strawHits[i][1] - refHitY) / div;
    circlePoints->SetPoint(i, strawHits[i][0], strawHits[i][1]);
    linePoints->SetPoint(i, mappedHits[i][0], mappedHits[i][1]);
  }

  circlePoints->Draw("SAMEA*");
  circlePoints->GetXaxis()->SetLimits(-2, 5);
  circlePoints->GetYaxis()->SetRangeUser(-2, 5);
  linePoints->Draw("*");
  linePoints->SetMarkerColor(2);

  test->SetCanvasSize(700, 700);
  test->Print("Plots/CircleLine1.pdf", "pdf");

  // Second example
  test->Clear();

  TGraph *pointsOnCircle = new TGraph();
  TGraph *pointsOnLine = new TGraph();

  double radius = 2;
  double centerX = 1;
  double centerY = 1;

  double oldPoints[9][2];
  double mappedPoints[9][2];

  pointCounter = 0;

  // calc circle-points
  for (double y = -1; y <= 3; y += 0.5) {

    oldPoints[pointCounter][0] = TMath::Sqrt(radius * radius - ((y - centerY) * (y - centerY))) + centerX;
    oldPoints[pointCounter][1] = y;
    pointsOnCircle->SetPoint(pointCounter, oldPoints[pointCounter][0], oldPoints[pointCounter][1]);
    ++pointCounter;
  }

  // calc with error
  double errPoint[2] = {-0.5, 3};
  pointsOnCircle->SetPoint(pointCounter, errPoint[0], errPoint[1]);

  // calc mapped points, take one old point as reference point
  double refX = oldPoints[0][0];
  double refY = oldPoints[0][1];
  pointCounter = 0;

  for (int i = 0; i < 9; ++i) {
    div = (oldPoints[pointCounter][0] - refX) * (oldPoints[pointCounter][0] - refX) + (oldPoints[pointCounter][1] - refY) * (oldPoints[pointCounter][1] - refY);
    mappedPoints[pointCounter][0] = (oldPoints[pointCounter][0] - refX) / div;
    mappedPoints[pointCounter][1] = -(oldPoints[pointCounter][1] - refY) / div;
    pointsOnLine->SetPoint(pointCounter, mappedPoints[pointCounter][0], mappedPoints[pointCounter][1]);
    ++pointCounter;
  }

  double mappedErrPoint[2];
  mappedErrPoint[0] = (errPoint[0] - refX) / ((errPoint[0] - refX) * (errPoint[0] - refX) + (errPoint[1] - refY) * (errPoint[1] - refY));
  mappedErrPoint[1] = -(errPoint[1] - refY) / ((errPoint[0] - refX) * (errPoint[0] - refX) + (errPoint[1] - refY) * (errPoint[1] - refY));
  pointsOnLine->SetPoint(pointCounter, mappedErrPoint[0], mappedErrPoint[1]);

  pointsOnCircle->Draw("A*");
  pointsOnCircle->GetXaxis()->SetLimits(-2, 4);
  pointsOnCircle->GetYaxis()->SetRangeUser(-2, 4);

  TArc *circle2 = new TArc(centerX, centerY, radius);
  circle2->SetFillStyle(0);
  circle2->Draw("SAME");

  pointsOnLine->Draw("SAME*");

  test->Print("Plots/CircleLine2.pdf", "pdf");
}

void PndSttCellTrackFinderAnalysisTask::TestRecoQualityFirstStep()
{

  if (fVerbose > 0)
    cout << "===TestRecoQualityFirstStep===" << endl;

  FairRootManager *ioman = FairRootManager::Instance();

  PndMCResult trackCandToMCTrack = fMCMatch->GetMCInfo("FirstTrackCand", "MCTrack");
  PndMCResult MCTrackToSttHits = fMCMatch->GetMCInfo("MCTrack", "STTHit");

  // for checking number of links of sttHits
  PndMCResult myResultHitsToMCTrack = fMCMatch->GetMCInfo("STTHit", "MCTrack");
  PndMCResult myResultHitToPoint = fMCMatch->GetMCInfo("STTHit", "STTPoint");

  int index;

  for (int i = 0; i < fMCTrack->GetEntries() && i < MCTrackToSttHits.GetNEntries(); i++) {

    FairMultiLinkedData STTHitsOfMCTrack = MCTrackToSttHits.GetEntry(i).GetLinksWithType(ioman->GetBranchId("STTHit"));

    // MCTrack produced more than one STTHits?
    if (STTHitsOfMCTrack.GetNLinks() > 1) {

      // increase number of MCTracks in histogram
      fHistoQualityFirstStep->Fill(0);
      fFoundMC[i] = false;

      // search for current MCTrackIndex in trackCands
      for (int j = 0; j < trackCandToMCTrack.GetNEntries(); j++) {

        PndTrackCand *myTrackCand = (PndTrackCand *)fFirstTrackCand->At(j);
        FairMultiLinkedData STTHitsOfTrackCand = myTrackCand->GetLinksWithType(ioman->GetBranchId("STTHit"));

        if (trackCandToMCTrack.GetEntry(j).GetNLinks() == 1 || GetNumLinksOfHits(STTHitsOfTrackCand, myResultHitsToMCTrack, myResultHitToPoint) == 1) {
          // hits of trackCand points to one MCTrack

          if (trackCandToMCTrack.GetEntry(j).GetLink(0).GetIndex() == i) {
            // found current MCTrackIndex
            fHistoQualityFirstStep->Fill(1);
            fFoundMC[i] = true;
            goto nextMCTrack;
          }
        }
      }

      // search for current MCTrackIndex in faulty trackCands
      for (int j = 0; j < trackCandToMCTrack.GetNEntries(); j++) {

        PndTrackCand *myTrackCand = (PndTrackCand *)fFirstTrackCand->At(j);
        FairMultiLinkedData STTHitsOfTrackCand = myTrackCand->GetLinksWithType(ioman->GetBranchId("STTHit"));

        if (STTHitsOfTrackCand.GetNLinks() > 1) {
          // hits of trackCand points to more than one MCTrack

          // check all links
          for (int k = 0; k < trackCandToMCTrack.GetEntry(j).GetNLinks(); ++k) {
            if (trackCandToMCTrack.GetEntry(j).GetLink(k).GetIndex() == i) {
              // found current MCTrackIndex
              fHistoQualityFirstStep->Fill(2);
              fFoundMC[i] = true;
              goto nextMCTrack;
            }
          }
        }
      }

      // MCTrack was not found
      fHistoQualityFirstStep->Fill(3);

      if (fVerbose > 0) {
        index = STTHitsOfMCTrack.GetLink(0).GetIndex();
        cout << "!!! MCTrack not found, tube: " << fMapHitIndexToTubeID[index] << endl;
      }

    nextMCTrack:;
    }
  }

  int candTrackIndex, numMCHits, numTrackHits, numSkewedTubesMC, numSkewedTubesCand, hitIndex; // numberOfLinks,  //[R.K. 01/2017] unused variable?
  double amount;

  for (int i = 0; i < trackCandToMCTrack.GetNEntries(); i++) {
    // increase number of TrackCands in histogram
    fHistoQualityFirstStep->Fill(4);

    PndTrackCand *myTrackCand = (PndTrackCand *)fFirstTrackCand->At(i);
    FairMultiLinkedData hitLinks = myTrackCand->GetLinksWithType(ioman->GetBranchId("STTHit"));

    numTrackHits = hitLinks.GetNLinks();

    if (trackCandToMCTrack.GetEntry(i).GetNLinks() == 1 || GetNumLinksOfHits(hitLinks, myResultHitsToMCTrack, myResultHitToPoint) == 1) {
      // clean trackCand

      // get MCTrackIndex
      candTrackIndex = trackCandToMCTrack.GetEntry(i).GetLink(0).GetIndex();

      // get number of hits of MCTrack -> compare

      FairMultiLinkedData STTHitsOfMCTrack = MCTrackToSttHits.GetEntry(candTrackIndex).GetLinksWithType(ioman->GetBranchId("STTHit"));

      numMCHits = STTHitsOfMCTrack.GetNLinks();
      numSkewedTubesMC = 0;
      numSkewedTubesCand = 0;

      // get number of hits of skewed tubes of MCTrack
      for (int j = 0; j < STTHitsOfMCTrack.GetNLinks(); ++j) {

        // Get HitIndex -> get Tube -> check if skewed
        hitIndex = STTHitsOfMCTrack.GetLink(j).GetIndex();

        PndSttTube *tube = (PndSttTube *)fTubeArray->At(fMapHitIndexToTubeID[hitIndex]);
        // count skewed tubes in MCTrack
        if (tube->IsSkew())
          ++numSkewedTubesMC;
      }

      // get number of hits of skewed tubes ob trackCand
      for (int j = 0; j < hitLinks.GetNLinks(); ++j) {

        // Get HitIndex -> get Tube -> check if skewed
        hitIndex = hitLinks.GetLink(j).GetIndex();

        PndSttTube *tube = (PndSttTube *)fTubeArray->At(fMapHitIndexToTubeID[hitIndex]);
        // count skewed tubes in MCTrack
        if (tube->IsSkew())
          ++numSkewedTubesCand;
      }

      /* TrackCand may contain hits of skewedTubes but not necessarily.
       * It must not contain more hits than MCTrack.*/
      if ((numMCHits - numSkewedTubesMC) == (numTrackHits - numSkewedTubesCand)) {

        fHistoQualityFirstStep->Fill(6);
        if (fVerbose > 0) {
          index = hitLinks.GetLink(0).GetIndex();
          cout << "FULLY RECO: " << fMapHitIndexToTubeID[index] << endl;
        }

      } else {
        fHistoQualityFirstStep->Fill(5);

        amount = 1 - (double)(numTrackHits - numSkewedTubesCand) / (numMCHits - numSkewedTubesMC);

        if (amount < 0.25) {
          fHistoNumberOfMissingHits1->Fill(0);
        } else if (amount < 0.5) {
          fHistoNumberOfMissingHits1->Fill(1);
        } else if (amount < 0.75) {
          fHistoNumberOfMissingHits1->Fill(2);
        } else {
          fHistoNumberOfMissingHits1->Fill(3);
        }
      }

    } else {
      // faulty trackCand -> ghost
      fHistoQualityFirstStep->Fill(7);
    }
  }
}

void PndSttCellTrackFinderAnalysisTask::TestRecoQualityCombi()
{

  if (fVerbose > 0)
    cout << "=== TestRecoQualityCombi===" << endl;

  FairRootManager *ioman = FairRootManager::Instance();

  PndMCResult trackCandToMCTrack = fMCMatch->GetMCInfo("CombiTrackCand", "MCTrack");
  PndMCResult MCTrackToSttHits = fMCMatch->GetMCInfo("MCTrack", "STTHit");

  // for checking number of links of sttHits
  PndMCResult myResultHitsToMCTrack = fMCMatch->GetMCInfo("STTHit", "MCTrack");
  PndMCResult myResultHitToPoint = fMCMatch->GetMCInfo("STTHit", "STTPoint");

  int index;
  // for each MCTrack
  for (int i = 0; i < fMCTrack->GetEntries() && i < MCTrackToSttHits.GetNEntries(); i++) {

    // get STTHit-Links
    FairMultiLinkedData STTHitsOfMCTrack = MCTrackToSttHits.GetEntry(i).GetLinksWithType(ioman->GetBranchId("STTHit"));

    // MCTrack produced more than one STTHits?
    if (STTHitsOfMCTrack.GetNLinks() > 1) {

      // increase number of MCTracks in histogram
      fHistoQualityCombi->Fill(0);
      index = STTHitsOfMCTrack.GetLink(0).GetIndex();

      // fill histo with number of hits per MCTrack
      fHistoNumberOfHitsMCTrack->Fill(STTHitsOfMCTrack.GetNLinks());

      // search for current MCTrackIndex in pure trackCands
      for (int j = 0; j < trackCandToMCTrack.GetNEntries(); j++) {

        PndTrackCand *myTrackCand = (PndTrackCand *)fCombiTrackCand->At(j);

        // get links of trackCand to STTHits
        FairMultiLinkedData STTHitsOfTrackCand = myTrackCand->GetLinksWithType(ioman->GetBranchId("STTHit"));

        // only one link to MCTrack?
        if (trackCandToMCTrack.GetEntry(j).GetNLinks() == 1 || GetNumLinksOfHits(STTHitsOfTrackCand, myResultHitsToMCTrack, myResultHitToPoint) == 1) {
          // hits of trackCand points to one MCTrack

          if (trackCandToMCTrack.GetEntry(j).GetLink(0).GetIndex() == i) {
            // found current MCTrackIndex in pure tracklet
            fHistoQualityCombi->Fill(1);

            if (fVerbose > 0) {
              if (fFoundMC[i] != true)
                cout << "FOUND MORE: tube " << fMapHitIndexToTubeID[index] << endl;
            }
            goto nextMCTrack;
          }
        }
      }

      // search for current MCTrackIndex in faulty trackCands
      for (int j = 0; j < trackCandToMCTrack.GetNEntries(); j++) {

        PndTrackCand *myTrackCand = (PndTrackCand *)fCombiTrackCand->At(j);
        FairMultiLinkedData STTHitsOfTrackCand = myTrackCand->GetLinksWithType(ioman->GetBranchId("STTHit"));

        if (STTHitsOfTrackCand.GetNLinks() > 1) {
          // hits of trackCand points to more than one MCTrack

          // check all links
          for (int k = 0; k < trackCandToMCTrack.GetEntry(j).GetNLinks(); ++k) {
            if (trackCandToMCTrack.GetEntry(j).GetLink(k).GetIndex() == i) {
              // found current MCTrackIndex

              fHistoQualityCombi->Fill(2);

              if (fVerbose > 0) {
                if (fFoundMC[i] != true)
                  cout << "FOUND MORE: tube " << fMapHitIndexToTubeID[index] << endl;
              }

              goto nextMCTrack;
            }
          }
        }
      }

      // MCTrack was not found
      fHistoQualityCombi->Fill(3);

      if (fVerbose > 0) {
        cout << "!!! MCTrack not found, tube: " << fMapHitIndexToTubeID[index] << endl;

        if (fFoundMC[i] == true)
          cout << "FOUND LESS: tube " << fMapHitIndexToTubeID[index] << endl;
      }

    nextMCTrack:;
    }
  }

  int candTrackIndex, numMCHits, numTrackHits, numSkewedTubesMC, numSkewedTubesCand, hitIndex; // numberOfLinks,  //[R.K. 01/2017] unused variable?
  double amount;

  for (int i = 0; i < trackCandToMCTrack.GetNEntries(); i++) {
    // increase number of TrackCands in histogram
    fHistoQualityCombi->Fill(4);

    PndTrackCand *myTrackCand = (PndTrackCand *)fCombiTrackCand->At(i);
    FairMultiLinkedData hitLinks = myTrackCand->GetLinksWithType(ioman->GetBranchId("STTHit"));

    numTrackHits = hitLinks.GetNLinks();

    if (trackCandToMCTrack.GetEntry(i).GetNLinks() == 1 || GetNumLinksOfHits(hitLinks, myResultHitsToMCTrack, myResultHitToPoint) == 1) {
      // clean trackCand

      // get MCTrackIndex
      candTrackIndex = trackCandToMCTrack.GetEntry(i).GetLink(0).GetIndex();

      // get number of hits of MCTrack -> compare

      FairMultiLinkedData STTHitsOfMCTrack = MCTrackToSttHits.GetEntry(candTrackIndex).GetLinksWithType(ioman->GetBranchId("STTHit"));

      numMCHits = STTHitsOfMCTrack.GetNLinks();
      numSkewedTubesMC = 0;
      numSkewedTubesCand = 0;

      // get number of hits of skewed tubes of MCTrack
      for (int j = 0; j < STTHitsOfMCTrack.GetNLinks(); ++j) {

        // Get HitIndex -> get Tube -> check if skewed
        hitIndex = STTHitsOfMCTrack.GetLink(j).GetIndex();

        PndSttTube *tube = (PndSttTube *)fTubeArray->At(fMapHitIndexToTubeID[hitIndex]);
        // count skewed tubes in MCTrack
        if (tube->IsSkew())
          ++numSkewedTubesMC;
      }

      // get number of hits of skewed tubes of trackCand
      for (int j = 0; j < hitLinks.GetNLinks(); ++j) {

        // Get HitIndex -> get Tube -> check if skewed
        hitIndex = hitLinks.GetLink(j).GetIndex();

        PndSttTube *tube = (PndSttTube *)fTubeArray->At(fMapHitIndexToTubeID[hitIndex]);
        // count skewed tubes in MCTrack
        if (tube->IsSkew())
          ++numSkewedTubesCand;
      }

      /* TrackCand may contain hits of skewedTubes but not necessarily.
       * It must not contain more hits than MCTrack.*/
      if ((numMCHits - numSkewedTubesMC) == (numTrackHits - numSkewedTubesCand)) {
        fHistoQualityCombi->Fill(6);

        if (fVerbose > 0) {
          index = hitLinks.GetLink(0).GetIndex();
          cout << "FULLY RECO: " << fMapHitIndexToTubeID[index] << endl;
        }

      } else {
        fHistoQualityCombi->Fill(5);

        amount = 1 - (double)(numTrackHits - numSkewedTubesCand) / (numMCHits - numSkewedTubesMC);

        if (amount < 0.25) {
          fHistoNumberOfMissingHits2->Fill(0);
        } else if (amount < 0.5) {
          fHistoNumberOfMissingHits2->Fill(1);
        } else if (amount < 0.75) {
          fHistoNumberOfMissingHits2->Fill(2);
        } else {
          fHistoNumberOfMissingHits2->Fill(3);
        }
      }

    } else {
      // faulty trackCand -> ghost
      fHistoQualityCombi->Fill(7);
    }
  }
}

void PndSttCellTrackFinderAnalysisTask::CountMaxHitsFirstStep()
{

  if (fVerbose > 1)
    cout << "=== CountMaxHitsFirstStep===" << endl;

  FairRootManager *ioman = FairRootManager::Instance();

  PndMCResult trackCandToMCTrack = fMCMatch->GetMCInfo("FirstTrackCand", "MCTrack");
  PndMCResult MCTrackToSttHits = fMCMatch->GetMCInfo("MCTrack", "STTHit");

  // for checking number of links of sttHits
  PndMCResult myResultHitsToMCTrack = fMCMatch->GetMCInfo("STTHit", "MCTrack");
  PndMCResult myResultHitToPoint = fMCMatch->GetMCInfo("STTHit", "STTPoint");

  int maxHits;

  for (int i = 0; i < fMCTrack->GetEntries() && i < MCTrackToSttHits.GetNEntries(); i++) {

    FairMultiLinkedData STTHitsOfMCTrack = MCTrackToSttHits.GetEntry(i).GetLinksWithType(ioman->GetBranchId("STTHit"));

    // MCTrack produced STTHits?
    if (STTHitsOfMCTrack.GetNLinks() > 0) {

      maxHits = 0;

      // search for appropriate trackCand with max number of hits
      for (int j = 0; j < trackCandToMCTrack.GetNEntries(); j++) {

        // get trackCand and links of hits
        PndTrackCand *myTrackCand = (PndTrackCand *)fFirstTrackCand->At(j);
        FairMultiLinkedData STTHitsOfTrackCand = myTrackCand->GetLinksWithType(ioman->GetBranchId("STTHit"));

        // if pure tracklet, check number of hits
        if (trackCandToMCTrack.GetEntry(j).GetNLinks() == 1 || GetNumLinksOfHits(STTHitsOfTrackCand, myResultHitsToMCTrack, myResultHitToPoint) == 1) {

          // hits of trackCand points to one MCTrack
          if (trackCandToMCTrack.GetEntry(j).GetLink(0).GetIndex() == i) {
            // found current MCTrackIndex

            maxHits = TMath::Max(maxHits, STTHitsOfTrackCand.GetNLinks());
          }
        }
      }

      // maxHits could be 0, if no trackCand was found
      fHistoMaxHitsOfMCTrack1->Fill(maxHits);
    }
  }
}

void PndSttCellTrackFinderAnalysisTask::CountMaxHitsCombi()
{

  if (fVerbose > 1)
    cout << "=== CountMaxHitsCombi===" << endl;

  FairRootManager *ioman = FairRootManager::Instance();

  PndMCResult trackCandToMCTrack = fMCMatch->GetMCInfo("CombiTrackCand", "MCTrack");
  PndMCResult MCTrackToSttHits = fMCMatch->GetMCInfo("MCTrack", "STTHit");

  // for checking number of links of sttHits
  PndMCResult myResultHitsToMCTrack = fMCMatch->GetMCInfo("STTHit", "MCTrack");
  PndMCResult myResultHitToPoint = fMCMatch->GetMCInfo("STTHit", "STTPoint");

  int maxHits;

  for (int i = 0; i < fMCTrack->GetEntries() && i < MCTrackToSttHits.GetNEntries(); i++) {

    FairMultiLinkedData STTHitsOfMCTrack = MCTrackToSttHits.GetEntry(i).GetLinksWithType(ioman->GetBranchId("STTHit"));

    // MCTrack produced STTHits?
    if (STTHitsOfMCTrack.GetNLinks() > 0) {

      maxHits = 0;

      // search for appropriate trackCand with max number of hits
      for (int j = 0; j < trackCandToMCTrack.GetNEntries(); j++) {

        // get trackCand and links of hits
        PndTrackCand *myTrackCand = (PndTrackCand *)fCombiTrackCand->At(j);
        FairMultiLinkedData STTHitsOfTrackCand = myTrackCand->GetLinksWithType(ioman->GetBranchId("STTHit"));

        // if pure tracklet, check number of hits
        if (trackCandToMCTrack.GetEntry(j).GetNLinks() == 1 || GetNumLinksOfHits(STTHitsOfTrackCand, myResultHitsToMCTrack, myResultHitToPoint) == 1) {

          // hits of trackCand points to one MCTrack
          if (trackCandToMCTrack.GetEntry(j).GetLink(0).GetIndex() == i) {
            // found current MCTrackIndex

            maxHits = TMath::Max(maxHits, STTHitsOfTrackCand.GetNLinks());
          }
        }
      }

      // maxHits could be 0 if no trackCand was found
      fHistoMaxHitsOfMCTrack2->Fill(maxHits);
    }
  }
}

void PndSttCellTrackFinderAnalysisTask::FinishEvent()
{

  fMapTubeIDToHits.clear();
  fMapHitIndexToTubeID.clear();
  fFoundMC.clear();
}

void PndSttCellTrackFinderAnalysisTask::Finish()
{
  fHistoNumberOfLinks1->Write();
  fHistoNumberOfTracklets1->Write();

  fHistoNumberOfAssignedHits1->Write();
  fHistoNumberOfHits1->Write();

  fHistoNumberOfHits2->Write();
  fHistoNumberOfLinks2->Write();
  fHistoNumberOfTracklets2->Write();

  fHistoNumberOfErrors1->Write();
  fHistoNumberOfErrors2->Write();

  fHistoNumberOfHitsMCTrack->Write();
  fHistoMaxHitsOfMCTrack1->Write();
  fHistoMaxHitsOfMCTrack2->Write();

  fHistoQualityFirstStep->Write();
  fHistoQualityCombi->Write();

  fHistoNumberOfMissingHits1->Write();
  fHistoNumberOfMissingHits2->Write();

  if (fVerbose > 0) {

    std::cout << "fHistoQualityFirstStep: #Possible Tracks " << fHistoQualityFirstStep->GetBinContent(1) << " #Found in clean: " << fHistoQualityFirstStep->GetBinContent(2)
              << " #Found in faulty: " << fHistoQualityFirstStep->GetBinContent(3) << " #Not Found: " << fHistoQualityFirstStep->GetBinContent(4)
              << " #TrackCands: " << fHistoQualityFirstStep->GetBinContent(5) << " #Spurious: " << fHistoQualityFirstStep->GetBinContent(6)
              << " #FullyReco: " << fHistoQualityFirstStep->GetBinContent(7) << " #Ghosts: " << fHistoQualityFirstStep->GetBinContent(8) << std::endl;

    std::cout << "fHistoQualityCombi: #Possible Tracks " << fHistoQualityCombi->GetBinContent(1) << " #Found in clean: " << fHistoQualityCombi->GetBinContent(2)
              << " #Found in faulty: " << fHistoQualityCombi->GetBinContent(3) << " #Not Found: " << fHistoQualityCombi->GetBinContent(4)
              << " #TrackCands: " << fHistoQualityCombi->GetBinContent(5) << " #Spurious: " << fHistoQualityCombi->GetBinContent(6)
              << " #FullyReco: " << fHistoQualityCombi->GetBinContent(7) << " #Ghosts: " << fHistoQualityCombi->GetBinContent(8) << std::endl;
  }
}
