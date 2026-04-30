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
 * PndIdealTrackFinders.cpp
 *
 *  Created on: Apr 12, 2010
 *      Author: stockman
 */

#include "PndMCTruthMatch.h"

#include "FairRootManager.h"
#include "FairMultiLinkedData.h"
#include "FairMultiLinkedData_Interface.h"
#include "FairLogger.h"

#include "PndMCTrack.h"

ClassImp(PndMCTruthMatch);

PndMCTruthMatch::PndMCTruthMatch()
{
  // TODO Auto-generated constructor stub
}

PndMCTruthMatch::PndMCTruthMatch(TString inBranchName) : fInBranchName(inBranchName) {}

PndMCTruthMatch::~PndMCTruthMatch()
{
  // TODO Auto-generated destructor stub
}

// -----   Public method Init   --------------------------------------------
InitStatus PndMCTruthMatch::Init()
{

  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    std::cout << "-E- PndMCTruthMatch::Init: "
              << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  if (fInBranchName == "") {
    // Use hits of all tracking subsystems if nothing is given
    LOG(error) << " PndMCTruthMatch::Init no InBranchName given";
  }

  fInBranch = (TClonesArray *)ioman->GetObject(fInBranchName);

  fMCTrack = (TClonesArray *)ioman->GetObject("MCTrack");

  return kSUCCESS;
}

void PndMCTruthMatch::Exec(Option_t *)
{
  std::cout << "------------ EventNr " << FairRootManager::Instance()->GetEntryNr() << " ------------" << std::endl;
  std::cout << FairRootManager::Instance()->GetBranchId(fInBranchName) << " : " << fInBranchName << " Entries: " << fInBranch->GetEntriesFast() << std::endl;

  for (int i = 0; i < fInBranch->GetEntriesFast(); i++) {
    FairMultiLinkedData_Interface *myData = (FairMultiLinkedData_Interface *)fInBranch->At(i);
    std::cout << i << " : " << *myData << std::endl;
    std::vector<FairLink> myLinks = myData->GetSortedMCTracks();
    for (unsigned int trackIndex = 0; trackIndex < myLinks.size(); trackIndex++) {
      PndMCTrack *myTrack = (PndMCTrack *)FairRootManager::Instance()->GetCloneOfLinkData(myLinks[trackIndex]);
      std::cout << myLinks[trackIndex] << " : " << *myTrack;
      delete (myTrack);
    }
    std::cout << std::endl;
  }
}
