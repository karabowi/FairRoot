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
 * PndMissingPzCleanerTask.cpp
 *
 *  Created on: Jun 26, 2013
 *      Author: schumann
 */

// Root includes
#include "TROOT.h"
#include "TClonesArray.h"

#include "FairRootManager.h"

#include "PndTrack.h"

#include "PndMissingPzCleanerTask.h"

ClassImp(PndMissingPzCleanerTask);

void PndMissingPzCleanerTask::SetParContainers() {}

InitStatus PndMissingPzCleanerTask::Init()
{

  FairRootManager *ioman = FairRootManager::Instance();

  if (!ioman) {
    std::cout << "-E- PndMissingPzCleanerTask::Init: "
              << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  // get data-object
  fTracks = (TClonesArray *)ioman->GetObject(fInputTrackBranch);

  if (fOutputTrackBranch.Length() == 0) {
    fOutputTrackBranch = fInputTrackBranch;
    fOutputTrackBranch.Append("_filtered");
  }

  fFilteredTracks = ioman->Register(fOutputTrackBranch, "PndTrack", "Tracks", GetPersistency());

  return kSUCCESS;
}

void PndMissingPzCleanerTask::Exec(Option_t *)
{

  //	if (fVerbose > 0) {
  //		std::cout << "============= Begin PndMissingPzCleanerTask::Exec" << std::endl;
  //		std::cout << std::endl;
  //	}
  FairRootManager *ioman = FairRootManager::Instance();
  fFilteredTracks->Delete();

  for (int i = 0; i < fTracks->GetEntriesFast(); i++) {
    PndTrack *myTrack = (PndTrack *)fTracks->At(i);
    int nNoStt = myTrack->GetLinksWithType(ioman->GetBranchId("MVDHitsPixel")).GetNLinks();
    nNoStt += myTrack->GetLinksWithType(ioman->GetBranchId("MVDHitsStrip")).GetNLinks();
    nNoStt += myTrack->GetLinksWithType(ioman->GetBranchId("GEMHit")).GetNLinks();
    int nStt = myTrack->GetLinksWithType(ioman->GetBranchId("STTHit")).GetNLinks();

    if ((nStt > 0) && (nNoStt == 0)) {
      if ((myTrack->GetParamFirst().GetPz() > 0.015) && (myTrack->GetParamFirst().GetPz() < 0.025)) {
        myTrack->SetFlag(-30);
        std::cout << ioman->GetEntryNr() << "/" << i << " : Wrong Pz Track found: " << *myTrack << std::endl;
      }
    }

    if (!((fRemoveTrack == kTRUE) && (myTrack->GetFlag() == -30)))
      new ((*fFilteredTracks)[fFilteredTracks->GetEntriesFast()]) PndTrack(*myTrack);
    else
      std::cout << "Track removed " << i << std::endl;
  }
}

void PndMissingPzCleanerTask::FinishEvent()
{
  if (fTracks != 0)
    fTracks->Delete();
}

void PndMissingPzCleanerTask::Finish() {}
