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
// -----                    PndMasterRecoIdealTask source file              -----
// -----                   Created 31/01/16  by S. Spataro             -----
// -----                   Wrapper for  recotizing tasks               -----
// -------------------------------------------------------------------------

#include <PndIdealTrackFinder.h>
#include "PndMasterRecoIdealTask.h"
#include "PndMasterTask.h"

#include "PndRecoKalmanTask.h"
#include "PndRecoKalmanTask2.h"
#include "PndRecoMultiKalmanTask.h"
#include "PndRecoMultiKalmanTask2.h"

//#include "PndFtsTrackerIdeal.h"
#include "PndTrackSmearTask.h"

/**
 * @brief Default Constructor
 * @details # Reconstruction task list
 * Here all the reconstruction tasks are added to the task, with the standard settings. A check is done after each task if the tasklist enum is broken or not. At the end the event
 *counter is added (each 100 events), and the verbosity is set to 0 to all the tasks (it can be changed afterwards with SetVerbosity() functions.
 **/
// -----   Default constructor   -------------------------------------------
PndMasterRecoIdealTask::PndMasterRecoIdealTask(TString options) : PndMasterTask("Master Reconstruction Task"), fOptions(options)
{
  std::array<TString, 5> hypoName = {"Electron", "Muon", "Pion", "Kaon", "Proton"};
  // -----  MVD + STT Pattern Recognition -----------------------------------
  //  use the constructor with input :
  //      printout flag (int) , plotting flag (bool), MC comparison flag (bool), SciTil.

  TString brltrkname = "BarrelTrack", brltrkgenname = "BarrelGenTrack";
  if (fOptions.Contains("nogem") || fOptions.Contains("gem0")) {
    brltrkname = "SttMvdTrack";
    brltrkgenname = "SttMvdGenTrack";
  }

  PndIdealTrackFinder *tracking = nullptr;
  fBranchTasks.push_back(tracking = new PndIdealTrackFinder());
  if (fOptions.Contains("nogem") || fOptions.Contains("gem0")) {
    tracking->SetTrackSelector("MvdSttTrackFunctor");
  } else {
    tracking->SetTrackSelector("NoFtsTrackFunctor");
  }
  tracking->SetRelativeMomentumSmearing(0.05);
  tracking->SetVertexSmearing(0.05, 0.05, 0.05);
  tracking->SetTrackingEfficiency(1.);
  tracking->SetOutputBranchName(brltrkname);
  tracking->SetPersistence(kTRUE);

  // ----- Barrel Kalman Task     ----------------------------
  if (!fOptions.Contains("multikalman")) { // no multikalman
    if (fOptions.Contains("genfit2")) {
      PndRecoKalmanTask2 *recoKalman = nullptr;
      fBranchTasks.push_back(recoKalman = new PndRecoKalmanTask2()); // 2
      recoKalman->SetTrackInBranchName(brltrkname);
      recoKalman->SetTrackOutBranchName(brltrkgenname);
      recoKalman->SetPropagateToIP(kFALSE);
      // recoKalman->SetIdealHyp(kTRUE);
    } else {
      PndRecoKalmanTask *recoKalman = nullptr;
      fBranchTasks.push_back(recoKalman = new PndRecoKalmanTask()); // 2
      recoKalman->SetTrackInBranchName(brltrkname);
      recoKalman->SetTrackOutBranchName(brltrkgenname);
      recoKalman->SetPropagateToIP(kFALSE);
      // recoKalman->SetIdealHyp(kTRUE);
      // recoKalman->SetNumIterations(3);
    }
  } else { // yes, multikalman
    if (fOptions.Contains("genfit2")) {
      PndRecoMultiKalmanTask2 *recoKalman = nullptr;
      fBranchTasks.push_back(recoKalman = new PndRecoMultiKalmanTask2()); // 2
      recoKalman->SetFitHypotheses(fOptions + ";BRL");
      recoKalman->SetTrackInBranchName(brltrkname);
      recoKalman->SetTrackOutBranchName(brltrkgenname);
      recoKalman->SetVerbose(2);
    } else {
      PndRecoMultiKalmanTask *recoKalman = nullptr;
      fBranchTasks.push_back(recoKalman = new PndRecoMultiKalmanTask("BrlKalman", 6)); // 2
      recoKalman->SetFitHypotheses(fOptions + ";BRL");
      recoKalman->SetTrackInBranchName(brltrkname);
      recoKalman->SetTrackOutBranchName(brltrkgenname);
      recoKalman->SetBusyCut(50); // CHECK to be tuned
      recoKalman->SetTrackRep(0); // 0 Geane (default), 1 RK
      recoKalman->SetPropagateToIP(kFALSE);
      // recoKalman->SetNumIterations(3);
    }
  }

  // -----  FTS Ideal Tracking    ----------------------------
  TString ftstrkname = "FtsIdealTrack", ftstrkgenname = "FtsIdealGenTrack";
  PndIdealTrackFinder *trackFts = nullptr;
  fBranchTasks.push_back(trackFts = new PndIdealTrackFinder());
  trackFts->SetTrackSelector("FtsTrackFunctor");
  trackFts->AddBranchName("FTSHit");
  trackFts->AddBranchName("MVDHitsPixel");
  trackFts->AddBranchName("MVDHitsStrip");
  trackFts->SetRelativeMomentumSmearing(0.05);
  trackFts->SetVertexSmearing(0.05, 0.05, 0.05);
  trackFts->SetTrackingEfficiency(1.);
  trackFts->SetOutputBranchName(ftstrkname);
  fFixedPersistency[trackFts] = kFALSE;

  // ----- Forward Kalman Task     ----------------------------
  if (!fOptions.Contains("multikalman")) { // no multikalman

    if (fOptions.Contains("genfit2")) {
      PndRecoKalmanTask2 *recoKalman = nullptr;
      fBranchTasks.push_back(recoKalman = new PndRecoKalmanTask2("FwdKalman")); // 2
      recoKalman->SetTrackInBranchName(ftstrkname);
      recoKalman->SetTrackOutBranchName(ftstrkgenname);
      recoKalman->SetPropagateToIP(kFALSE);
      // recoKalman->SetIdealHyp(kTRUE);
    } else {
      PndRecoKalmanTask *recoKalman = nullptr;
      fBranchTasks.push_back(recoKalman = new PndRecoKalmanTask("FwdKalman"));
      recoKalman->SetTrackInBranchName(ftstrkname);
      recoKalman->SetTrackOutBranchName(ftstrkgenname);
      recoKalman->SetPropagateToIP(kFALSE);
      // recoKalman->SetIdealHyp(kTRUE);
    }
  } else { // yes, multikalman!
    if (fOptions.Contains("genfit2")) {
      PndRecoMultiKalmanTask2 *recoKalman = nullptr;
      fBranchTasks.push_back(recoKalman = new PndRecoMultiKalmanTask2("FwdKalman", 2)); // 2
      recoKalman->SetFitHypotheses(fOptions + ";FWD");
      recoKalman->SetTrackInBranchName(ftstrkname);
      recoKalman->SetTrackOutBranchName(ftstrkgenname);
      recoKalman->SetVerbose(2);
    } else {
      PndRecoMultiKalmanTask *recoKalman = nullptr;
      fBranchTasks.push_back(recoKalman = new PndRecoMultiKalmanTask("FwdKalman"));
      recoKalman->SetFitHypotheses(fOptions + ";FWD");
      recoKalman->SetTrackInBranchName(ftstrkname);
      recoKalman->SetTrackOutBranchName(ftstrkgenname);
      recoKalman->SetPropagateToIP(kFALSE);
    }
  }

  // --- Online Emulator smearing
  if (fOptions.Contains("fakeonline")) {
    PndTrackSmearTask *smearer = nullptr;
    fBranchTasks.push_back(smearer = new PndTrackSmearTask());
    if (fOptions.Contains("filtered"))
      brltrkgenname += "_filtered";
    if (fOptions.Contains("multikalman")) { // yes, multikalman
      for (int k = 0; k < 5; ++k) {
        smearer->AddInputTrackBranch(Form("%s%s", brltrkgenname.Data(), hypoName[k].Data()));
        smearer->AddInputTrackBranch(Form("%s%s", ftstrkgenname.Data(), hypoName[k].Data()));
      }
    } else { // no multikalman
      smearer->AddInputTrackBranch(brltrkgenname);
      smearer->AddInputTrackBranch(ftstrkgenname);
    }
  }

  // Now add all cached tasks to this one
  std::for_each(fBranchTasks.begin(), fBranchTasks.end(), [this](const FairTask *task) { Add((TTask *)task); });

  SetVerbose(0);
}

// -----   Destructor   ----------------------------------------------------
PndMasterRecoIdealTask::~PndMasterRecoIdealTask() {}
// -------------------------------------------------------------------------

/** @cond CLASSIMP */
ClassImp(PndMasterRecoIdealTask);
/** @endcond */
