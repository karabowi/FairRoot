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
// -----                PndMcCloner source file                     -----
// -----                  Created 08/07/13  by  S.Spataro              -----
// -------------------------------------------------------------------------

#include "PndMcCloner.h"

#include "PndMCTrack.h"
#include "PndPidCandidate.h"

#include "FairRootManager.h"
#include "FairDetector.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"

#include "TClonesArray.h"

#include <iostream>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndMcCloner::PndMcCloner() : FairTask("Cloner of PndMCTrack"), mapMCIndex() {}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndMcCloner::~PndMcCloner() {}
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
InitStatus PndMcCloner::Init()
{

  cout << "-I- PndMcCloner::Init: "
       << "INITIALIZATION *********************" << endl;

  // FairRun* sim = FairRun::Instance(); //[R.K. 01/2017] unused variable?
  // FairRuntimeDb* rtdb=sim->GetRuntimeDb(); //[R.K. 01/2017] unused variable?

  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    cout << "-E- PndMcCloner::Init: "
         << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  // Get input array
  fInputArray = (TClonesArray *)ioman->GetObject("MCTrack");
  if (!fInputArray) {
    cout << "-E- PndMcCloner::Init: "
         << "No MCTrack array!" << endl;
    return kERROR;
  }

  fPidChargedArray = (TClonesArray *)ioman->GetObject("PidChargedCand");
  if (!fPidChargedArray) {
    cout << "-E- PndMcCloner::Init: "
         << "No PidChargedCand array!" << endl;
    return kERROR;
  }

  fPidNeutralArray = (TClonesArray *)ioman->GetObject("PidNeutralCand");
  if (!fPidNeutralArray) {
    cout << "-E- PndMcCloner::Init: "
         << "No PidNeutralCand array!" << endl;
    return kERROR;
  }

  // Create and register output array
  fOutputArray = new TClonesArray("PndMCTrack");

  ioman->Register("MCTrack", "MC", fOutputArray, kTRUE);

  LOG(info) << " PndMcCloner: Intialization successfull";

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndMcCloner::Exec(Option_t *)
{

  // Reset output array
  if (!fOutputArray)
    Fatal("Exec", "No Output Array");

  fOutputArray->Clear();
  mapMCIndex.clear();

  if (!fCleanMC) {
    CloneMCTrack();
  } else {
    FindUsedMCIndices();
    CloneAndCleanMCTrack();
    CorrectMotherIndices();
    CorrectPidIndices();
  }
}
// -------------------------------------------------------------------------

// -----   Protected method FindUsedMcIndices   --------------------------------------------
void PndMcCloner::CloneMCTrack()
{
  // Copy 1:1 of the MCTrack TClonesArray

  Int_t nMCTracks = fInputArray->GetEntriesFast();
  for (Int_t iMC = 0; iMC < nMCTracks; iMC++) {
    PndMCTrack *mctrack = (PndMCTrack *)fInputArray->At(iMC);
    TClonesArray &clref = *fOutputArray;
    Int_t size = clref.GetEntriesFast();
    new (clref[size]) PndMCTrack(*mctrack);
  } // Loop over MCTracks
}

// -----   Protected method FindUsedMcIndices   --------------------------------------------
void PndMcCloner::FindUsedMCIndices()
{
  // Loop over PidChargedCand and PidNeutralCand, find the used MC indices, and fill the map
  // with this index and all the mother indices

  Int_t nCands = 0;

  // First store primary MC tracks
  nCands = fInputArray->GetEntriesFast();
  for (Int_t imc = 0; imc < nCands; imc++) {
    PndMCTrack *mctrack = (PndMCTrack *)fInputArray->At(imc);
    if (mctrack->GetMotherID() == -1) {
      mapMCIndex[imc] = imc;
    } else
      break;
  }

  nCands = fPidChargedArray->GetEntriesFast();
  for (Int_t iPid = 0; iPid < nCands; iPid++) {
    PndPidCandidate *pidCand = (PndPidCandidate *)fPidChargedArray->At(iPid);
    Int_t mcIndex = pidCand->GetMcIndex();

    while (mcIndex != -1) {
      PndMCTrack *mctrack = (PndMCTrack *)fInputArray->At(mcIndex);
      if (mctrack == nullptr) {
        Error("FindUsedMCIndices", "PndMCTrack is not existing!");
      }
      mapMCIndex[mcIndex] = mcIndex;
      mcIndex = mctrack->GetMotherID();
    }
  } // Loop over PidChargedCand

  nCands = fPidNeutralArray->GetEntriesFast();
  for (Int_t iPid = 0; iPid < nCands; iPid++) {
    PndPidCandidate *pidCand = (PndPidCandidate *)fPidNeutralArray->At(iPid);
    Int_t mcIndex = pidCand->GetMcIndex();

    while (mcIndex != -1) {
      PndMCTrack *mctrack = (PndMCTrack *)fInputArray->At(mcIndex);
      if (mctrack == nullptr) {
        Error("FindUsedMCIndices", "PndMCTrack is not existing!");
      }
      mapMCIndex[mcIndex] = mcIndex;
      mcIndex = mctrack->GetMotherID();
    }
  } // Loop over PidNeutralCand
}
// -------------------------------------------------------------------------

// -----   Protected method FindUsedMcIndices   --------------------------------------------
void PndMcCloner::CloneAndCleanMCTrack()
{
  // Copy only the MCTracks which were used, and update the mother indices

  for (std::map<Int_t, Int_t>::iterator it = mapMCIndex.begin(); it != mapMCIndex.end(); ++it) {
    PndMCTrack *mctrack = (PndMCTrack *)fInputArray->At(it->first);
    TClonesArray &clref = *fOutputArray;
    Int_t size = clref.GetEntriesFast();
    new (clref[size]) PndMCTrack(*mctrack);
    mapMCIndex[it->first] = size;
  }
}
// -------------------------------------------------------------------------

// -----   Protected method CorrectMotherIndices   --------------------------------------------
void PndMcCloner::CorrectMotherIndices()
{
  // Loop over the new MCTrack TClonesArray and update the mother indices
  Int_t nmc = 0;
  nmc = fOutputArray->GetEntriesFast();
  for (Int_t imc = 0; imc < nmc; imc++) {
    PndMCTrack *mctrack = (PndMCTrack *)fOutputArray->At(imc);
    Int_t motherID = mctrack->GetMotherID();
    Int_t secondMotherID = mctrack->GetSecondMotherID();
    if (motherID != -1)
      mctrack->SetMotherID(mapMCIndex[motherID]);
    if (secondMotherID != -1)
      mctrack->SetSecondMotherID(mapMCIndex[secondMotherID]);
  }
}
// -----   Protected method CorrectMotherIndices   --------------------------------------------
void PndMcCloner::CorrectPidIndices()
{
  // Loop over Pid Candidates and set the mc indices with the new value

  Int_t nCands = 0;

  nCands = fPidChargedArray->GetEntriesFast();
  for (Int_t iPid = 0; iPid < nCands; iPid++) {
    PndPidCandidate *pidCand = (PndPidCandidate *)fPidChargedArray->At(iPid);
    Int_t mcIndex = pidCand->GetMcIndex();
    pidCand->SetMcIndex(mapMCIndex[mcIndex]);
  }

  nCands = fPidNeutralArray->GetEntriesFast();
  for (Int_t iPid = 0; iPid < nCands; iPid++) {
    PndPidCandidate *pidCand = (PndPidCandidate *)fPidNeutralArray->At(iPid);
    Int_t mcIndex = pidCand->GetMcIndex();
    pidCand->SetMcIndex(mapMCIndex[mcIndex]);
  }
}
ClassImp(PndMcCloner)
