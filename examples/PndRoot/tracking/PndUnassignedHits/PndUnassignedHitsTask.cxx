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
 * PndUnassignedHitsTask.cpp
 *
 *  Created on: Nov 11, 2018
 *      Author: Tobias Stockmanns <t.stockmanns@fz-juelich.de>
 */

#include "PndUnassignedHitsTask.h"

#include <FairRootManager.h>

#include "TString.h"
#include "TClonesArray.h"
#include "PndTrackCand.h"

#include "PndSttHit.h"
#include "PndGemHit.h"
#include "PndSdsHit.h"
#include "PndFtsHit.h"

ClassImp(PndUnassignedHitsTask);

void PndUnassignedHitsTask::SetParContainers() {}

InitStatus PndUnassignedHitsTask::Init()
{

  if (fHitBranches.size() == 0) {
    AddHitBranch("MVDHitsPixel");
    AddHitBranch("MVDHitsStrip");
    AddHitBranch("STTHit");
    AddHitBranch("GEMHit");
    AddHitBranch("FTSHit");
  }

  RegisterBranches();
  RegisterTrackCands();

  return kSUCCESS;
}

void PndUnassignedHitsTask::Exec(Option_t *opt)
{
  FillTrackCands();
  //    std::cout << "PndUnassignedHitsTask::Exec TrackCands filled!" << std::endl;
  for (auto branch : fHitBranches) {
    //        std::cout << "PndUnassignedHitsTask::Exec Fill unassigned hits: " << branch.first << std::endl;
    TString unassignedName = branch.first;
    unassignedName += fUnassignedBranchExtension;

    if (fUnassignedHitBranches[unassignedName] == nullptr) {
      LOG(error) << "-E- PndUnassignedHitsTask::Exec() Branch does not exist: " << unassignedName;
    }

    fUnassignedHitBranches[unassignedName]->Delete();

    FillUnassignedHits(branch.first);
  }
}

void PndUnassignedHitsTask::AddHitBranch(TString branchName)
{
  fHitBranches[branchName] = nullptr;
}

void PndUnassignedHitsTask::AddTrackCands(TString trackCandName)
{
  fTrackCands[trackCandName] = nullptr;
}

void PndUnassignedHitsTask::FillTrackCands()
{
  fHitsInTracks.Reset();
  for (auto cands : fTrackCands) {
    for (int i = 0; i < cands.second->GetEntries(); i++) {
      PndTrackCand *cand = (PndTrackCand *)cands.second->At(i);
      std::vector<PndTrackCandHit> hits = cand->GetSortedHits();
      for (auto hit : hits) {
        fHitsInTracks.AddLink(hit, kFALSE);
      }
    }
  }
}

void PndUnassignedHitsTask::RegisterBranches()
{
  FairRootManager *ioman = FairRootManager::Instance();

  for (auto branch : fHitBranches) {
    branch.second = (TClonesArray *)ioman->GetObject(branch.first);
    TString unassignedName = branch.first;
    unassignedName += fUnassignedBranchExtension;
    fHitBranches[branch.first] = branch.second;
    std::cout << "PndUnassignedHitsTask::RegisterBranches " << branch.first << " " << fUnassignedBranchExtension << std::endl;
    fUnassignedHitBranches[unassignedName] = ioman->Register(unassignedName, branch.second->GetClass()->GetName(), "UnassignedHits", GetPersistency());
  }
}

void PndUnassignedHitsTask::RegisterTrackCands()
{
  FairRootManager *ioman = FairRootManager::Instance();

  std::cout << "PndUnassignedHitsTask::RegisterTrackCands" << std::endl;
  for (auto cand : fTrackCands) {
    cand.second = (TClonesArray *)ioman->GetObject(cand.first);
    //        std::cout << "PndUnassignedHitsTask::RegisterTrackCands " << cand.first << " " << cand.second << std::endl;
    fTrackCands[cand.first] = cand.second;
  }
}

void PndUnassignedHitsTask::FillUnassignedHits(TString branchName)
{
  std::set<FairLink> usedHits = fHitsInTracks.GetLinksWithType(FairRootManager::Instance()->GetBranchId(branchName)).GetLinks();
  //    std::cout << "PndUnassignedHitsTask::FillUnassignedHits " << fHitsInTracks.GetLinksWithType(FairRootManager::Instance()->GetBranchId(branchName)) << std::endl;
  TClonesArray *hits = fHitBranches[branchName];

  std::vector<int> unusedHits;
  for (int i = 0; i < hits->GetEntries(); i++) {
    unusedHits.push_back(i);
  }

  std::cout << "Hits: " << hits << std::endl;
  std::cout << "Size of Hits: " << hits->GetEntries() << std::endl;
  for (auto reviter = usedHits.rbegin(); reviter != usedHits.rend(); ++reviter) {
    //        std::cout << "Index to remove: " << reviter->GetIndex() << std::endl;
    unusedHits.erase(unusedHits.begin() + reviter->GetIndex());
  }

  std::cout << "Size of UnusedHits: " << unusedHits.size() << std::endl;

  if (unusedHits.size() > 0) {
    //        std::cout << "OutputBranch: " << branchName << " " << fUnassignedHitBranches[branchName += "Unassigned"] << std::endl;
    FillOutputBranch(branchName, unusedHits);
  }

  std::cout << "Size of UnassignedBranch: " << fUnassignedHitBranches[branchName += fUnassignedBranchExtension]->GetEntries() << std::endl;
}

void PndUnassignedHitsTask::FillOutputBranch(TString branchName, std::vector<int> unusedHits)
{
  //    std::cout << "FillOutputBranch: " << branchName << std::endl;
  TString outputBranchName = branchName;
  outputBranchName += fUnassignedBranchExtension;
  TClonesArray *outputArray = fUnassignedHitBranches[outputBranchName];
  std::cout << "OutputArray: " << outputArray << std::endl;
  std::cout << "HitBranch size: " << fHitBranches[branchName]->GetEntries() << std::endl;
  for (unsigned int i = 0; i < unusedHits.size(); i++) {
    //        std::cout << i << " : " << unusedHits[i] << std::endl;
    FairHit *newHit = nullptr;
    //        std::cout << *myHit << std::endl;
    if (branchName.Contains("STTHit")) {
      PndSttHit *myHit = (PndSttHit *)fHitBranches[branchName]->At(unusedHits[i]);
      if (myHit == nullptr)
        continue;
      myHit->SetEntryNr(FairLink(-1, FairRootManager::Instance()->GetEntryNr(), FairRootManager::Instance()->GetBranchId(branchName), unusedHits[i]));
      newHit = new ((*outputArray)[outputArray->GetEntries()]) PndSttHit(*(PndSttHit *)myHit);
      newHit->SetEntryNr(myHit->GetEntryNr());
    } else if (branchName.Contains("GEMHit")) {
      PndGemHit *myHit = (PndGemHit *)fHitBranches[branchName]->At(unusedHits[i]);
      if (myHit == nullptr)
        continue;
      myHit->SetEntryNr(FairLink(-1, FairRootManager::Instance()->GetEntryNr(), FairRootManager::Instance()->GetBranchId(branchName), unusedHits[i]));
      newHit = new ((*outputArray)[outputArray->GetEntries()]) PndGemHit(*(PndGemHit *)myHit);
      newHit->SetEntryNr(myHit->GetEntryNr());
    } else if (branchName.Contains("MVDHitsPixel") || branchName.Contains("MVDHitsStrip")) {
      PndSdsHit *myHit = (PndSdsHit *)fHitBranches[branchName]->At(unusedHits[i]);
      if (myHit == nullptr)
        continue;
      myHit->SetEntryNr(FairLink(-1, FairRootManager::Instance()->GetEntryNr(), FairRootManager::Instance()->GetBranchId(branchName), unusedHits[i]));
      newHit = new ((*outputArray)[outputArray->GetEntries()]) PndSdsHit(*(PndSdsHit *)myHit);
      newHit->SetEntryNr(myHit->GetEntryNr());
    } else if (branchName.Contains("FTSHit")) {
      PndFtsHit *myHit = (PndFtsHit *)fHitBranches[branchName]->At(unusedHits[i]);
      if (myHit == nullptr)
        continue;
      myHit->SetEntryNr(FairLink(-1, FairRootManager::Instance()->GetEntryNr(), FairRootManager::Instance()->GetBranchId(branchName), unusedHits[i]));
      newHit = new ((*outputArray)[outputArray->GetEntries()]) PndFtsHit(*(PndFtsHit *)myHit);
      newHit->SetEntryNr(myHit->GetEntryNr());
    }
  }
  //    fUnassignedHitBranches[branchName += "Unassigned"]->AbsorbObjects(fHitBranches[branchName],0,hits->GetEntries()-1);
}
