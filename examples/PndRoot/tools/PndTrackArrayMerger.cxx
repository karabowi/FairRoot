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


#include "PndTrackArrayMerger.h"
#include "PndTrackCand.h"
#include "PndTrack.h"
#include <iostream>

PndTrackArrayMerger::PndTrackArrayMerger() : fPersistance(kTRUE), fOutputBranch("ALLTracks"), fOutputArray(), fInputArrayList(), fInputBranchList() {}

PndTrackArrayMerger::PndTrackArrayMerger(TString s) : fPersistance(kTRUE), fOutputBranch(s), fOutputArray(), fInputArrayList(), fInputBranchList() {}

PndTrackArrayMerger::~PndTrackArrayMerger() {}

void PndTrackArrayMerger::SetParContainers()
{
  return;
}

InitStatus PndTrackArrayMerger::ReInit()
{
  return kSUCCESS;
}

InitStatus PndTrackArrayMerger::Init()
{
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    std::cout << "-E- PndSdsStripHitProducer::Init: "
              << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  // setup input arrays
  TClonesArray *tmparray;
  for (std::vector<TString>::iterator iter = fInputBranchList.begin(); iter != fInputBranchList.end(); ++iter) {
    tmparray = (TClonesArray *)ioman->GetObject((*iter).Data());
    if (!tmparray) {
      Error("Init()", "No %s array! Skipping that name.", (*iter).Data());
      continue;
    }
    TString namebuff = (TString)tmparray->GetClass()->GetName();
    if (namebuff == "PndTrack") {
      fInputArrayList.push_back(tmparray);
    }
  }

  // setup output array
  fOutputArray = ioman->Register(fOutputBranch, "PndTrack", "AllTracks", fPersistance);
  // fOutputArray = new TClonesArray("PndTrack");
  // ioman->Register(fOutputBranch, "AllTracks", fOutputArray, fPersistance);

  return kSUCCESS;
}

void PndTrackArrayMerger::Exec(Option_t *)
{
  fOutputArray->Clear();
  // copy data from input arrays to output array
  TClonesArray *tmparray;
  PndTrack *tmptrk;
  Int_t namenum = 0;
  TString brname;
  Int_t entries = 0;

  for (std::vector<TClonesArray *>::iterator iter = fInputArrayList.begin(); iter != fInputArrayList.end(); ++iter) {
    tmparray = *iter;
    if (tmparray != 0) {
      fOutputArray->AbsorbObjects(tmparray, 0, tmparray->GetEntries() - 1);
    }
  }

  return;
}

void PndTrackArrayMerger::FinishEvent()
{
  // called after all Tasks did their Exex() and the data is copied to the file
  //  fOutputArray->Clear();
  FinishEvents();
}

ClassImp(PndTrackArrayMerger);
