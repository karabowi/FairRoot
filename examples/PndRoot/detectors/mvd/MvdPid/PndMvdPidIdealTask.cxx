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
// -----                PndMvdPidIdealTask source file                 -----
// -----                Created 11/07/07  by T.Baldauf                 -----
// -------------------------------------------------------------------------
// libc includes
#include <iostream>
#include <vector>

// Root includes
#include "TROOT.h"
#include "TString.h"
#include "TClonesArray.h"
#include "TParticlePDG.h"
#include "TObject.h"
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"

// framework includes
#include "FairRootManager.h"
#include "PndMvdPidIdealTask.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "PndMCTrack.h"

// PndMvd includes
#include "PndSdsMCPoint.h"
#include "PndMvdPidCand.h"
// #include "PndSdsHit.h"

#include "PndMvdIdealPidAlgo.h"
#include "PndMvdSimplePidAlgo.h"
#include "PndMvdAdvancedPidAlgo.h"

// -----   Default constructor   -------------------------------------------
PndMvdPidIdealTask::PndMvdPidIdealTask(TString algoName)
  : FairTask("Digitization task for PANDA PndMvd"), fBranchName("MVDPoint"), fAlgoName(algoName), fPointArray(nullptr), fMcArray(nullptr), fTrackOutputArray(nullptr)
{
}

// -----   Public method Init   --------------------------------------------
InitStatus PndMvdPidIdealTask::Init()
{
  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    std::cout << "-E- PndMvdPidIdealTask::Init: "
              << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  // Get input array
  fPointArray = (TClonesArray *)ioman->GetObject(fBranchName);
  if (!fPointArray) {
    std::cout << "-W- PndMvdPidIdealTask::Init: "
              << "No " << fBranchName << " array!" << std::endl;
    return kERROR;
  }

  // Get MCTruth collection
  fMcArray = (TClonesArray *)ioman->GetObject("MCTrack");
  if (fMcArray == nullptr) {
    std::cout << "-W- PndMvdPidIdealTask::Init: "
              << "No McTruth array!" << std::endl;
    return kERROR;
  }

  // Create and register output array
  fTrackOutputArray = new TClonesArray("PndMvdPidCand");
  ioman->Register("PndMvdPidCand", "PndMvd ideal tracklets", fTrackOutputArray, kTRUE);

  if (fAlgoName != "ideal" && fAlgoName != "simple" && fAlgoName != "advanced") {
    std::cout << "-W- PndMvdPidIdealTask::Init: "
              << "No pid algorithm named '" << fAlgoName << "'! Names are 'ideal', 'simple', 'advanced'" << std::endl;
    return kERROR;
  }
  return kSUCCESS;
}
// -------------------------------------------------------------------------
void PndMvdPidIdealTask::SetParContainers()
{
  // Get Base Container
  //  FairRun* ana = FairRun::Instance();
  //  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
}

//----------------Exec----------------
void PndMvdPidIdealTask::Exec(Option_t *)
{
  if (!fTrackOutputArray)
    Fatal("Exec", "No fTrackOutputArray");
  fTrackOutputArray->Delete();

  // the dE/dx information of PndmvdPidCand is obtained by PndMvdMCPoints
  std::map<int, PndMvdPidCand *> pidcand;

  for (int i = 0; i < fPointArray->GetEntriesFast(); i++) {
    PndSdsMCPoint *mvdpoint = (PndSdsMCPoint *)fPointArray->At(i);
    int track = mvdpoint->GetTrackID();
    if (!pidcand[track])
      pidcand[track] = new PndMvdPidCand();
    double dx = (mvdpoint->GetPositionOut() - mvdpoint->GetPosition()).Mag();
    double dE = mvdpoint->GetEnergyLoss();
    TVector3 momentum;
    mvdpoint->Momentum(momentum);
    if (dx > 0)
      pidcand[track]->AddMvdHit(dE, dx, momentum.Mag());
  }

  // invoke likelohood calculation, TClonesArray output
  for (std::map<int, PndMvdPidCand *>::iterator it = pidcand.begin(); it != pidcand.end(); it++) {
    int track = it->first;
    if (fAlgoName == "ideal")
      PndMvdIdealPidAlgo::CalcLikelihood(pidcand[track]);
    else if (fAlgoName == "simple")
      PndMvdSimplePidAlgo::CalcLikelihood(pidcand[track]);
    else if (fAlgoName == "advanced")
      PndMvdAdvancedPidAlgo::CalcLikelihood(pidcand[track]);

    int size = fTrackOutputArray->GetEntriesFast();
    new ((*fTrackOutputArray)[size]) PndMvdPidCand(*(pidcand[track]));
  }
}

// -------------------------------------------------------------------------

ClassImp(PndMvdPidIdealTask)
