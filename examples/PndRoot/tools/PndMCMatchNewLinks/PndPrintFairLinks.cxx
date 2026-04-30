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

// -------------------------------------------------------------------------
// -----                PndPrintFairLinks source file             -----
// -----                  Created 18/07/08  by T.Stockmanns        -----
// -------------------------------------------------------------------------
// libc includes
#include "PndPrintFairLinks.h"
#include <iostream>
#include <exception>

// Root includes
#include "TROOT.h"
#include "TClonesArray.h"
#include "TChain.h"
#include "TChainElement.h"

// framework includes
#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairHit.h"
#include "FairMultiLinkedData_Interface.h"
#include "FairLogger.h"

//#include "PndDetectorList.h"

// -----   Default constructor   -------------------------------------------
PndPrintFairLinks::PndPrintFairLinks() : FairTask("Creates PndMCMatch"), fSelectedBranches(new TList()) {}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndPrintFairLinks::~PndPrintFairLinks() {}

// -----   Public method Init   --------------------------------------------
InitStatus PndPrintFairLinks::Init()
{
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    std::cout << "-E- PndPrintFairLinks::Init: "
              << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  TList *branchNames;
  if (fSelectedBranches->GetEntries() > 0) {
    branchNames = fSelectedBranches;
  } else {
    branchNames = ioman->GetBranchNameList();
  }

  InitBranchList(branchNames);

  PrintBranchNameList(ioman->GetBranchNameList());

  return kSUCCESS;
}

void PndPrintFairLinks::InitBranchList(TList *branches)
{
  FairRootManager *ioman = FairRootManager::Instance();
  for (int i = 0; i < branches->GetEntries(); i++) {
    TObjString *branchName = (TObjString *)branches->At(i);
    if (branchName->String().Contains("_link"))
      continue;
    if (branchName->String().EqualTo("MCTrack")) {
      continue;
    }
    if (branchName->String().Contains("."))
      continue;
    if (branchName->String().Contains("GeoTracks"))
      continue;
    if (branchName->String().Contains("Header"))
      continue;
    if (branchName->String().Contains("Info") && !branchName->String().EqualTo("MCTrackInfo"))
      continue;
    if (branchName->String().Contains("ID"))
      continue;
    if (branchName->String().Contains("PidAlgo"))
      continue;
    if (branchName->String().Contains("Riemann"))
      continue;

    std::cout << "PndPrintFairLinks::Init() branches: " << branchName->String() << std::endl;
    if ((TClonesArray *)ioman->GetObject(branchName->String()) != 0) {
      fBranches[ioman->GetBranchId(branchName->String())] = (TClonesArray *)ioman->GetObject(branchName->String());
    } else {
      LOG(error) << " PndPrintFairLinks " << branchName->String().Data() << " is not a valid branch name!";
    }
  }
}

void PndPrintFairLinks::PrintBranchNameList(TList *branches)
{
  LOG(info) << " PndPrintFairLinks Branches:";

  for (int i = 0; i < branches->GetEntries(); i++) {
    TObjString *branchName = (TObjString *)branches->At(i);
    std::cout << i << " : " << branchName->String().Data() << std::endl;
  }
  std::cout << std::endl;
}


// -----   Public method Exec   --------------------------------------------
void PndPrintFairLinks::Exec(Option_t *)
{
  std::cout << std::endl
            << "--------------------- Event " << FairRootManager::Instance()->GetEntryNr() << " at " << FairRootManager::Instance()->GetEventTime() << " ns ----------------------"
            << std::endl;
  for (std::map<Int_t, TClonesArray *>::iterator iter = fBranches.begin(); iter != fBranches.end(); iter++) {
    std::cout << std::endl << iter->first << " : " << FairRootManager::Instance()->GetBranchName(iter->first) << " Entries: " << iter->second->GetEntriesFast() << std::endl;
    for (int i = 0; i < iter->second->GetEntriesFast(); i++) {
      FairMultiLinkedData_Interface *myLinks = (FairMultiLinkedData_Interface *)iter->second->At(i);
      if (myLinks->GetPointerToLinks() != 0) {
        std::cout << i << " : " << *myLinks << std::endl << std::endl;
      }
    }
  }
}

void PndPrintFairLinks::Finish() {}

ClassImp(PndPrintFairLinks);
