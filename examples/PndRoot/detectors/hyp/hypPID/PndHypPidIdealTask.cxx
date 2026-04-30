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
// -----            modified for hyp prupose by A. Sanchez             -----
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
#include "PndHypPidIdealTask.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "PndMCTrack.h"

// PndHyp includes
#include "PndHypPoint.h"
#include "PndHypPidCand.h"
// #include "PndHypHit.h"

// #include "PndHypIdealPidAlgo.h"
// #include "HypPid/PndHypSimplePidAlgo.h"
#include "PndHypAdvancedPidAlgo.h"

// -----   Default constructor   -------------------------------------------
PndHypPidIdealTask::PndHypPidIdealTask(TString algoName) : FairTask("Digitization task for PANDA PndHyp")
{
  fBranchName = "HypPoint";
  fAlgoName = algoName;
}

// -----   Public method Init   --------------------------------------------
InitStatus PndHypPidIdealTask::Init()
{
  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    std::cout << "-E- PndHypPidIdealTask::Init: "
              << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  // Get input array
  fPointArray = (TClonesArray *)ioman->GetObject(fBranchName);
  if (!fPointArray) {
    std::cout << "-W- PndHypPidIdealTask::Init: "
              << "No " << fBranchName << " array!" << std::endl;
    return kERROR;
  }

  // Get MCTruth collection
  fMcArray = (TClonesArray *)ioman->GetObject("MCTrack");
  if (fMcArray == nullptr) {
    std::cout << "-W- PndHypPidIdealTask::Init: "
              << "No McTruth array!" << std::endl;
    return kERROR;
  }

  // Create and register output array
  fTrackOutputArray = new TClonesArray("PndHypPidCand");
  ioman->Register("PndHypPidCand", "PndHyp ideal tracklets", fTrackOutputArray, kTRUE);

  if (fAlgoName != "ideal" && fAlgoName != "simple" && fAlgoName != "advanced") {
    std::cout << "-W- PndHypPidIdealTask::Init: "
              << "No pid algorithm named '" << fAlgoName << "'! Names are 'ideal', 'simple', 'advanced'" << std::endl;
    return kERROR;
  }
  return kSUCCESS;
}
// -------------------------------------------------------------------------
void PndHypPidIdealTask::SetParContainers()
{
  // Get Base Container
  // FairRunAna* ana = FairRunAna::Instance(); //[R.K. 01/2017] unused variable?
  // FairRuntimeDb* rtdb=ana->GetRuntimeDb(); //[R.K. 01/2017] unused variable
}

//----------------Exec----------------
void PndHypPidIdealTask::Exec(Option_t *)
{
  if (!fTrackOutputArray)
    Fatal("Exec", "No fTrackOutputArray");
  fTrackOutputArray->Clear();

  // the dE/dx information of PndmvdPidCand is obtained by PndHypMCPoints
  std::map<int, PndHypPidCand *> pidcand;
  TVector3 Out, In;

  for (int i = 0; i < fPointArray->GetEntriesFast(); i++) {
    PndHypPoint *mvdpoint = (PndHypPoint *)fPointArray->At(i);
    mvdpoint->PositionOut(Out);
    mvdpoint->PositionIn(In);

    int track = mvdpoint->GetTrackID();
    if (!pidcand[track])
      pidcand[track] = new PndHypPidCand();
    double dx = (Out - In).Mag();
    double dE = mvdpoint->GetEnergyLoss();
    TVector3 momentum;
    mvdpoint->Momentum(momentum);

    std::cout << dE << " " << dx << " " << momentum.Mag() << std::endl;

    if (dx > 0)
      pidcand[track]->AddHypHit(dE, dx, momentum.Mag());
  }

  // invoke likelohood calculation, TClonesArray output
  for (std::map<int, PndHypPidCand *>::iterator it = pidcand.begin(); it != pidcand.end(); it++) {
    int track = it->first;
    if (fAlgoName == "advanced")
      PndHypAdvancedPidAlgo::CalcLikelihood(pidcand[track]);
    std::cout << " track to be piditized " << std::endl;

    int size = fTrackOutputArray->GetEntriesFast();
    new ((*fTrackOutputArray)[size]) PndHypPidCand(*(pidcand[track]));
  }
}

// -------------------------------------------------------------------------

ClassImp(PndHypPidIdealTask)
