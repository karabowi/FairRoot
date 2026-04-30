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

#include "PndTrackingEventInfo.h"
#include "FairMultiLinkedData_Interface.h"
#include "FairLink.h"
#include <iostream>
#include <vector>
#include "PndTrack.h"
#include "PndTrackCand.h"
#include "PndMCTrack.h"
#include "PndSttHit.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include <map>

using namespace std;

PndTrackingEventInfo::PndTrackingEventInfo(PndTrack *currentTrack, FairLink currentLink) : fEventPurity(-1) {}

PndTrackingEventInfo::~PndTrackingEventInfo() {}

map<int, int> PndTrackingEventInfo::GetNumTimesHitUsed(PndTrack *currentTrack)
{

  std::cout << "PndTrackingEventInfo::GetNumTimesHitUsed" << std::endl;

  fMapHitIdTimesUsed.clear();

  FairMultiLinkedData linksSttHits = currentTrack->GetLinksWithType(FairRootManager::Instance()->GetBranchId("STTHit"));

  std::cout << "Num links to SttHit: " << linksSttHits.GetNLinks() << std::endl;

  for (int i = 0; i < (int)linksSttHits.GetNLinks(); ++i) {

    PndSttHit *currentHit = (PndSttHit *)FairRootManager::Instance()->GetCloneOfLinkData(linksSttHits.GetLink(i));
    fMapHitIdTimesUsed[currentHit->GetTubeID()]++;
  }

  return fMapHitIdTimesUsed;
}

void PndTrackingEventInfo::CalcEventPurity(PndTrack *currentTrack, FairLink currentLink)
{

  int nHitsEventIndex = 0;
  int nHitsAllIndex = 0;
  int eventIndex = -1;

  // std::cout << "Current track fair link: " << currentTrack->GetEntryNr()  << std::endl;

  // std::cout << "PndTrackingEventInfo::CalcEventPurity" << std::endl;
  // std::cout << "Before loop" << std::endl;
  // std::cout << "Event index: " << nHitsEventIndex << ", All index: " << nHitsAllIndex << std::endl;

  // std::vector<FairLink> sortedMcTracks = currentTrack->GetSortedMCTracks();

  // std::cout << "Number of MC tracks " << sortedMcTracks.size() << std::endl;

  // eventIndex=sortedMcTracks[0].GetEntry();

  // eventIndex=currentLink.GetEntry();

  eventIndex = currentTrack->GetEntryNr().GetEntry();

  // std::cout << "Entry of link: " << eventIndex << std::endl;

  FairMultiLinkedData sttHitsFairLinks = currentTrack->GetLinksWithType(FairRootManager::Instance()->GetBranchId("STTSortedHits"));

  for (int j = 0; j < sttHitsFairLinks.GetNLinks(); j++) {
    // std::cout << "In loop" << std::endl;
    // std::cout << "Event index: " << nHitsEventIndex << ", All index: " << nHitsAllIndex << std::endl;

    // std::cout << "Entry of hit: " << sttHitsFairLinks.GetLink(j).GetEntry() << std::endl;

    nHitsAllIndex++;

    if (sttHitsFairLinks.GetLink(j).GetEntry() == eventIndex) {
      nHitsEventIndex++;
    }
  }

  // std::cout << "After loop" << std::endl;
  // std::cout << "Event index: " << nHitsEventIndex << ", All index: " << nHitsAllIndex << std::endl;

  if (nHitsAllIndex != 0) {
    fEventPurity = (double)nHitsEventIndex / nHitsAllIndex;
  }
}

ClassImp(PndTrackingEventInfo);
