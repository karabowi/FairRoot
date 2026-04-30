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
// -----                PndMcClonerMulti source file                     -----
// -----                  Created 08/07/13  by  S.Spataro              -----
// -------------------------------------------------------------------------

#include "PndMcClonerMulti.h"

#include "PndMCTrack.h"
#include "PndPidCandidate.h"

#include "FairRootManager.h"
#include "FairDetector.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"

#include <iostream>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndMcClonerMulti::PndMcClonerMulti()
  : FairTask("Cloner of PndMCTrack"), fInputArray(), fPidChargedArray(), fPidNeutralArray(), fOutputArray(), fMmapMCIndex(), fCleanMC(kFALSE), fTrackBranchNamePidHypo("")
{
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndMcClonerMulti::~PndMcClonerMulti() {}
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
InitStatus PndMcClonerMulti::Init()
{

  LOG(info) << "PndMcClonerMulti::Init: INITIALIZATION *********************";

  fPidHypoStr[0] = "Electron";
  fPidHypoStr[1] = "Muon";
  fPidHypoStr[2] = "Pion";
  fPidHypoStr[3] = "Kaon";
  fPidHypoStr[4] = "Proton";

  // FairRun* sim = FairRun::Instance(); //[R.K. 01/2017] unused variable?
  // FairRuntimeDb* rtdb=sim->GetRuntimeDb(); //[R.K. 01/2017] unused variable?

  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    LOG(error) << "PndMcClonerMulti::Init: RootManager not instantiated!";
    return kFATAL;
  }

  // Get input array
  fInputArray = (TClonesArray *)ioman->GetObject("MCTrack");
  if (!fInputArray) {
    LOG(error) << "PndMcClonerMulti::Init: No MCTrack array!";
    return kERROR;
  }

  bool anychargedtca = false;
  for (int i = 0; i < 5; i++) {
    fPidChargedArray[i] = (TClonesArray *)ioman->GetObject("PidChargedCand" + fPidHypoStr[i]);
    if (!fPidChargedArray[i]) {
      LOG(warning) << "PndMcClonerMulti::Init: No PidChargedCand" + fPidHypoStr[i] + " array.";
    } else {
      LOG(info) << "PndMcClonerMulti::Init: Found PidChargedCand" + fPidHypoStr[i] + " array.";
      anychargedtca = true;
    }
  }
  if (!anychargedtca) {
    LOG(error) << "PndMcClonerMulti::Init: No PidChargedCand array found!";
    return kERROR;
  }

  fPidNeutralArray = (TClonesArray *)ioman->GetObject("PidNeutralCand");
  if (!fPidNeutralArray) {
    // TODO: Do we want to be able to exclude Neutrals, i.e. not call it an error?
    LOG(error) << "PndMcClonerMulti::Init: No PidNeutralCand array.";
    return kERROR;
  } else {
    LOG(info) << "PndMcClonerMulti::Init: Found PidNeutralCand array.";
  }

  // Create and register output array
  fOutputArray = new TClonesArray("PndMCTrack");

  ioman->Register("MCTrack" + fTrackBranchNamePidHypo, "MC", fOutputArray, kTRUE);

  LOG(info) << "PndMcClonerMulti: Intialization successfull";
  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndMcClonerMulti::Exec(Option_t *)
{

  // Reset output array
  if (!fOutputArray)
    LOG(fatal) << "PndMcClonerMulti::Exec(): No Output Array";

  fOutputArray->Clear();
  fMmapMCIndex.clear();

  if (!fCleanMC) {
    LOG(debug) << "PndMcClonerMulti: Start cloning without cleaning";
    CloneMCTracks();
  } else {
    LOG(debug) << "PndMcClonerMulti: Start cloning with cleaning";
    FindUsedMCIndices();
    CloneAndCleanMCTrack();
    CorrectMotherIndices();
    CorrectPidIndices();
  }
}
// -------------------------------------------------------------------------

// -----   Protected method FindUsedMcIndices   --------------------------------------------
void PndMcClonerMulti::CloneMCTracks()
{
  // Copy 1:1 of the MCTrack TClonesArray

  LOG(debug) << "PndMcClonerMulti::CloneMCTrack: Start";
  Int_t nMCTracks = fInputArray->GetEntriesFast();
  for (Int_t iMC = 0; iMC < nMCTracks; iMC++) {
    CloneMcTrack(iMC);
  } // Loop over MCTracks
  LOG(debug) << "PndMcClonerMulti::CloneMCTrack: Done";
}

// -----   Protected method FindUsedMcIndices   --------------------------------------------
void PndMcClonerMulti::FindUsedMCIndices()
{
  // Loop over PidChargedCand and PidNeutralCand, find the used MC indices, and fill the map
  // with this index and all the mother indices
  LOG(debug) << "PndMcClonerMulti::FindUsedMCIndices: Start";

  Int_t nCands = 0;

  // First store primary MC tracks
  nCands = fInputArray->GetEntriesFast();
  for (Int_t imc = 0; imc < nCands; imc++) {
    PndMCTrack *mctrack = static_cast<PndMCTrack *>(fInputArray->At(imc));
    if (mctrack->GetMotherID() == -1) {
      fMmapMCIndex[imc] = imc;
    } else
      break;
  }
  LOG(debug) << "PndMcClonerMulti::FindUsedMCIndices: Primary MC tracks are stored.";

  for (int i = 0; i < 5; i++) {
    if (fPidChargedArray[i] != nullptr) {
      for (Int_t iCand = 0; iCand < fPidChargedArray[i]->GetEntriesFast(); iCand++) {
        Int_t mcIndex = -1;
        LOG(debug) << "PndMcClonerMulti::FindUsedMCIndices: we're with iCand " << iCand << " and i " << i << "\n\t"
                   << "fPidChargedArray[i]=" << fPidChargedArray[i];
        PndPidCandidate *pidCand = static_cast<PndPidCandidate *>(fPidChargedArray[i]->At(iCand));
        LOG(debug) << "PndMcClonerMulti::FindUsedMCIndices: we're with iCand " << iCand << " and i " << i << " and the pidcand is fine?: " << pidCand;
        mcIndex = pidCand->GetMcIndex();
        if (mcIndex != -1)
          MarkMcTree(mcIndex);
      }
    }
  } // Loop over PidChargedCand
  LOG(debug) << "PndMcClonerMulti::FindUsedMCIndices: Done with the charged ones, do neutrals, if applicable.";

  if (fPidNeutralArray != nullptr) {
    nCands = fPidNeutralArray->GetEntriesFast();
    for (Int_t iCand = 0; iCand < nCands; iCand++) {
      Int_t mcIndex = -1;
      PndPidCandidate *pidCand = static_cast<PndPidCandidate *>(fPidNeutralArray->At(iCand));
      mcIndex = pidCand->GetMcIndex();
      MarkMcTree(mcIndex);
    } // Loop over PidNeutralCand
  }
  LOG(debug) << "PndMcClonerMulti::FindUsedMCIndices: Done";
}
// -------------------------------------------------------------------------

// -----   Protected method FindUsedMcIndices   --------------------------------------------
void PndMcClonerMulti::CloneAndCleanMCTrack()
{
  // Copy only the MCTracks which were used, and update the mother indices
  LOG(debug) << "PndMcClonerMulti::CloneAndCleanMCTrack: Start";

  for (std::map<Int_t, Int_t>::iterator it = fMmapMCIndex.begin(); it != fMmapMCIndex.end(); ++it) {
    Int_t mcidx = it->first;
    // TODO: Why old size?
    fMmapMCIndex[mcidx] = fOutputArray->GetEntriesFast();
    CloneMcTrack(mcidx);
  }
  LOG(debug) << "PndMcClonerMulti::CloneAndCleanMCTrack: Done";
}
// -------------------------------------------------------------------------

// -----   Protected method CorrectMotherIndices   --------------------------------------------
void PndMcClonerMulti::CorrectMotherIndices()
{
  // Loop over the new MCTrack TClonesArray and update the mother indices
  LOG(debug) << "PndMcClonerMulti::CorrectMotherIndices: Start";
  Int_t nmc = 0;
  nmc = fOutputArray->GetEntriesFast();
  for (Int_t imc = 0; imc < nmc; imc++) {
    PndMCTrack *mctrack = static_cast<PndMCTrack *>(fOutputArray->At(imc));
    Int_t motherID = mctrack->GetMotherID();
    Int_t secondMotherID = mctrack->GetSecondMotherID();
    if (motherID != -1)
      mctrack->SetMotherID(fMmapMCIndex[motherID]);
    if (secondMotherID != -1)
      mctrack->SetSecondMotherID(fMmapMCIndex[secondMotherID]);
  }
  LOG(debug) << "PndMcClonerMulti::CorrectMotherIndices: Done";
}
// -----   Protected method CorrectMotherIndices   --------------------------------------------
void PndMcClonerMulti::CorrectPidIndices()
{
  // Loop over Pid Candidates and set the mc indices with the new value
  LOG(debug) << "PndMcClonerMulti::CorrectPidIndices: Start";

  Int_t nCands = 0;

  for (int i = 0; i < 5; i++) {
    if (fPidChargedArray[i] != nullptr) {
      nCands = fPidChargedArray[i]->GetEntriesFast();
      for (Int_t iCand = 0; iCand < nCands; iCand++) {
        PndPidCandidate *pidCand = static_cast<PndPidCandidate *>(fPidChargedArray[i]->At(iCand));
        Int_t mcIndex = pidCand->GetMcIndex();
        pidCand->SetMcIndex(fMmapMCIndex[mcIndex]);
      }
    }
  }
  if (fPidNeutralArray != nullptr) {
    nCands = fPidNeutralArray->GetEntriesFast();
    for (Int_t iCand = 0; iCand < nCands; iCand++) {
      PndPidCandidate *pidCand = static_cast<PndPidCandidate *>(fPidNeutralArray->At(iCand));
      Int_t mcIndex = pidCand->GetMcIndex();
      pidCand->SetMcIndex(fMmapMCIndex[mcIndex]);
    }
  }
  LOG(debug) << "PndMcClonerMulti::CorrectPidIndices: Done";
}

void PndMcClonerMulti::CloneMcTrack(Int_t mcidx)
{
  if (mcidx < 0) {
    LOG(warning) << "PndMcCloner::CloneMcTrack(): negative mc index: " << mcidx;
    return;
  }
  PndMCTrack *mctrack = static_cast<PndMCTrack *>(fInputArray->At(mcidx));
  if (mctrack == nullptr) {
    LOG(warning) << "PndMcCloner::CloneMcTrack(): mc track is nullptr for index " << mcidx;
    return;
  }
  TClonesArray &clref = *fOutputArray;
  Int_t size = clref.GetEntriesFast();
  new (clref[size]) PndMCTrack(*mctrack);
}

void PndMcClonerMulti::MarkMcTree(Int_t mcIndex)
{
  LOG(debug) << "PndMcClonerMulti::MarkMcTree: storing MC tree part above and with mc index " << mcIndex;
  while (mcIndex != -1) {
    PndMCTrack *mctrack = static_cast<PndMCTrack *>(fInputArray->At(mcIndex));
    if (mctrack == nullptr) {
      LOG(error) << "PndMcClonerMulti::FindUsedMCIndices(): PndMCTrack is not existing!";
    }
    fMmapMCIndex[mcIndex] = mcIndex;
    mcIndex = mctrack->GetMotherID();
  }
}

ClassImp(PndMcClonerMulti)
