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
// PndTrkGemCombiTask
//
// Class for GEM combinatorial suppression
//
// authors: Lia Lavezzi - University of Torino (2015)
//
////////////////////////////////////////////////////////////

#include "PndTrkGemCombiTask.h"

#include "PndTrkTools.h"

// fairroot
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
// ROOT
#include "TClonesArray.h"
#include "TVector3.h"
#include "TMarker.h"

#include "PndGemHit.h"
#include "PndGemMCPoint.h"

#include <iostream>

using namespace std;

// -----   Default constructor   -------------------------------------------
PndTrkGemCombiTask::PndTrkGemCombiTask() : FairTask("GEM combinatorial suppression", 0), fDisplayOn(kFALSE), fMCEval(kFALSE) {}

PndTrkGemCombiTask::PndTrkGemCombiTask(int verbose) : FairTask("GEM combinatorial suppression", verbose), fDisplayOn(kFALSE), fMCEval(kFALSE) {}

// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndTrkGemCombiTask::~PndTrkGemCombiTask()
{

  delete fGemHitArray;
  delete fGemPointArray;
}
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
InitStatus PndTrkGemCombiTask::Init()
{
  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    cout << "-E- PndTrkGemCombiTask::Init: "
         << "RootManager not instantiated, return!" << endl;
    return kFATAL;
  }

  fGemHitArray = (TClonesArray *)ioman->GetObject("GEMHit");
  if (!fGemHitArray) {
    std::cout << "-W- PndTrkGemCombiTask::Init: "
              << "No GEM hitArray, return!" << std::endl;
    return kERROR;
  }

  fCombiFinder = new PndTrkGemCombinatorial(fGemHitArray, fVerbose);
  if (fDisplayOn)
    fCombiFinder->SwitchOnDisplay();
  if (fMCEval) {
    fGemPointArray = (TClonesArray *)ioman->GetObject("GEMPoint"); // to evaluate performances
    fCombiFinder->Evaluate();
    fCombiFinder->SetMCPointTCA(fGemPointArray);
  }

  return kSUCCESS;
}

// -------------------------------------------------------------------------
void PndTrkGemCombiTask::Exec(Option_t *)
{

  std::map<int, bool> hitidTousability = fCombiFinder->CombinatorialSuppression();
}

ClassImp(PndTrkGemCombiTask)
