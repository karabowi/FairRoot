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

////////////////////////////////////////////////////////////
//
// PndTrkAddTCA
//
// Class to add primaries and secondaries
//
// authors: Lia Lavezzi - University of Torino (2015)
//
////////////////////////////////////////////////////////////

#include "PndTrkAddTCA.h"

// track(cand)
#include "PndTrackCand.h"
#include "PndTrackCandHit.h"
#include "PndTrack.h"

// fairroot
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"
// ROOT
#include "TClonesArray.h"

#include <iostream>

using namespace std;

// -----   Default constructor   -------------------------------------------
PndTrkAddTCA::PndTrkAddTCA() : FairTask("add TCAs", 0) {}

// -----   Destructor   ----------------------------------------------------
PndTrkAddTCA::~PndTrkAddTCA()
{

  delete fPrimaryTrackArray;
  delete fSecondaryTrackArray;
  delete fTrackArray;
}
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
InitStatus PndTrkAddTCA::Init()
{

  fEventCounter = 0;

  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    cout << "-E- PndTrkAddTCA::Init: "
         << "RootManager not instantiated, return!" << endl;
    return kFATAL;
  }

  // -- HITS -------------------------------------------------
  //
  // I
  fPrimaryTrackArray = (TClonesArray *)ioman->GetObject("SttMvdGemTrack");
  if (!fPrimaryTrackArray) {
    LOG(warn) << " PndTrkAddTCA::Init: No primary track array, return!";
    return kERROR;
  }
  //
  // II
  fSecondaryTrackArray = (TClonesArray *)ioman->GetObject("Track");
  if (!fSecondaryTrackArray) {
    LOG(warn) << " PndTrkAddTCA::Init: No secondary track array, return!";
    return kERROR;
  }

  fTrackArray = new TClonesArray("PndTrack");
  fTrackCandArray = new TClonesArray("PndTrackCand");
  ioman->Register("CombiTrack", "pr", fTrackArray, kTRUE);
  ioman->Register("CombiTrackCand", "pr", fTrackCandArray, kTRUE);

  return kSUCCESS;
}

// -------------------------------------------------------------------------
void PndTrkAddTCA::Exec(Option_t *)
{
  fTrackArray->Delete();
  fTrackCandArray->Delete();

  if (fVerbose)
    cout << "PndTrkAddTCA:: *********************** " << fEventCounter << " ***********************" << endl;
  fEventCounter++;

  PndTrack *track = nullptr;
  // primaries
  for (int itrk = 0; itrk < fPrimaryTrackArray->GetEntriesFast(); itrk++) {
    track = (PndTrack *)fPrimaryTrackArray->At(itrk);

    TClonesArray &clref = *fTrackArray;
    Int_t size = clref.GetEntriesFast();
    PndTrack *outputtrack = new (clref[size]) PndTrack(track->GetParamFirst(), track->GetParamLast(), track->GetTrackCand());
    outputtrack->SetFlag(333);
    if (fabs(outputtrack->GetParamFirst().GetMomentum().Z()) < 1e-10)
      outputtrack->SetFlag(-333);

    TClonesArray &clref2 = *fTrackCandArray;
    size = clref2.GetEntriesFast();
    new (clref2[size]) PndTrackCand(track->GetTrackCand()); // PndTrackCand *outputtrackcand =  //[R.K.03/2017] unused variable
  }

  // secondaries
  for (int itrk = 0; itrk < fSecondaryTrackArray->GetEntriesFast(); itrk++) {
    track = (PndTrack *)fSecondaryTrackArray->At(itrk);

    TClonesArray &clref = *fTrackArray;
    Int_t size = clref.GetEntriesFast();
    PndTrack *outputtrack = new (clref[size]) PndTrack(track->GetParamFirst(), track->GetParamLast(), track->GetTrackCand());
    outputtrack->SetFlag(track->GetFlag());

    TClonesArray &clref2 = *fTrackCandArray;
    size = clref2.GetEntriesFast();
    new (clref2[size]) PndTrackCand(track->GetTrackCand()); // PndTrackCand *outputtrackcand =  //[R.K.03/2017] unused variable
  }
}

ClassImp(PndTrkAddTCA)
