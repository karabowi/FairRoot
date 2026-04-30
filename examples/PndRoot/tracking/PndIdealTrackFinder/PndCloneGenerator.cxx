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
 * PndCloneGenerators.cpp
 *
 *  Created on: Apr 12, 2010
 *      Author: stockman
 */

#include "PndCloneGenerator.h"

#include "FairRootManager.h"
#include "FairMCPoint.h"
#include "FairHit.h"

#include "PndTrackCand.h"
#include "PndTrack.h"
#include "PndMCTrack.h"
#include "PndGemMCPoint.h"
#include "PndGemHit.h"

#include "TRandom.h"
ClassImp(PndCloneGenerator);

PndCloneGenerator::PndCloneGenerator()
{
  // TODO Auto-generated constructor stub
}

PndCloneGenerator::~PndCloneGenerator()
{
  // TODO Auto-generated destructor stub
}

void PndCloneGenerator::Exec(Option_t *)
{

  fTrackCand->Delete();
  fTrack->Delete();
  fTrackCandMap.clear();
  fTrackCandCloneMap.clear();

  std::cout << "Event #" << FairRootManager::Instance()->GetEntryNr() << std::endl;
  CreateTrackCands();
  CreateTrackCandClones();
  SetFirstLastHit(fTrackCandMap);
  std::cout << "PndIdealTrackFinder:Found Tracks:" << std::endl;
  CreateTracks();
  fTrackCandMap = fTrackCandCloneMap;
  SetFirstLastHit(fTrackCandMap);
  CreateTracks();
}

void PndCloneGenerator::CreateTrackCandClones()
{
  for (auto trackCandIter : fTrackCandMap) {
    PndTrackCand myTrackCand = trackCandIter.second;
    int nHits = myTrackCand.GetNHits();
    if (nHits > 1) {
      PndTrackCand clone1;
      clone1.SetInsertHistory(kTRUE);
      PndTrackCand clone2;
      clone2.SetInsertHistory(kTRUE);
      for (int hitIter = 0; hitIter < nHits / 2; hitIter++) {
        clone1.AddHit((FairLink)(myTrackCand.GetSortedHit(hitIter)), myTrackCand.GetSortedHit(hitIter).GetRho());
      }
      for (int hitIter = nHits / 2; hitIter < nHits; hitIter++) {
        clone2.AddHit((FairLink)(myTrackCand.GetSortedHit(hitIter)), myTrackCand.GetSortedHit(hitIter).GetRho());
      }
      fTrackCandMap[trackCandIter.first] = clone1;
      fTrackCandCloneMap[trackCandIter.first] = clone2;
    }
  }
}

void PndCloneGenerator::SetFirstLastHit(std::map<FairLink, PndTrackCand> &trackCandMap)
{
  for (auto trackIter : trackCandMap) {
    PndTrackCandHit first = trackIter.second.GetSortedHit(0);
    PndTrackCandHit last = trackIter.second.GetSortedHit(trackIter.second.GetNHits() - 1);

    FairMultiLinkedData array;
    FairMultiLinkedData_Interface *links = 0;
    TString hitBranchName;
    hitBranchName = FairRootManager::Instance()->GetBranchName(first.GetType());
    links = (FairMultiLinkedData_Interface *)fBranchMap[hitBranchName]->At(first.GetIndex());
    FairMCPoint *firstpoint = 0;
    if (links != 0) {
      firstpoint = GetFairMCPoint(hitBranchName, links, array);
      if (firstpoint != 0)
        fFirstPointMap[links->GetSortedMCTracks().front()] = *firstpoint;
    }
    hitBranchName = FairRootManager::Instance()->GetBranchName(last.GetType());
    links = (FairMultiLinkedData_Interface *)fBranchMap[hitBranchName]->At(last.GetIndex());
    FairMCPoint *lastpoint = 0;
    if (links != 0) {
      lastpoint = GetFairMCPoint(hitBranchName, links, array);
      if (lastpoint != 0)
        fLastPointMap[links->GetSortedMCTracks().front()] = *lastpoint;
    }
  }
}
