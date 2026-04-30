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
// -----                    PndMasterRecoTask source file              -----
// -----                   Created 31/01/16  by S. Spataro             -----
// -----                   Wrapper for  recotizing tasks               -----
// -------------------------------------------------------------------------

#include <PndIdealTrackFinder.h>
#include "PndMasterRecoTask.h"
#include "PndMasterTask.h"

#include "PndTrkTracking2.h"
#include "PndSttMvdGemTracking.h"
#ifdef VCSUPPORT
#include "PndFtsCATracking.h"
#endif // VCSUPPORT
#include "PndRecoKalmanTask.h"
#include "PndRecoKalmanTask2.h"
#include "PndRecoMultiKalmanTask.h"
#include "PndRecoMultiKalmanTask2.h"
#include "PndBarrelTrackFinder.h"
#include "PndMissingPzCleanerTask.h"
//#include "PndFtsTrackerIdeal.h"
#include "PndTrackSmearTask.h"
#include "PndGFRecoTask.h"
#include "PndFtsTrackFinderTask.h"

#include <array>

/**
 * @brief Default Constructor
 * @details # Reconstruction task list
 * Here all the reconstruction tasks are added to the task, with the standard settings. A check is done after each task if the tasklist enum is broken or not. At the end the event
 *counter is added (each 100 events), and the verbosity is set to 0 to all the tasks (it can be changed afterwards with SetVerbosity() functions.
 **/
// -----   Default constructor   -------------------------------------------
PndMasterRecoTask::PndMasterRecoTask(TString options) : PndMasterTask("Master Reconstruction Task"), fOptions(options)
{

  std::array<TString, 5> hypoName = {"Electron", "Muon", "Pion", "Kaon", "Proton"};
  TString brltrkname = "", brltrkgenname = ""; // those will be set below

  // Set the track branches according to the options
  if (fOptions.Contains("stttracking")) {
    // -----  MVD + STT Pattern Recognition -----------------------------------
    //  use the constructor with input :
    //      printout flag (int) , plotting flag (bool), MC comparison flag (bool), SciTil.
    PndTrkTracking2 *tracking = nullptr;
    fBranchTasks.push_back(tracking = new PndTrkTracking2(0, false, false, true));
    tracking->SetInputBranchName("STTHit", "MVDHitsPixel", "MVDHitsStrip");
    // tracking->SetInputBranchName("STTHitMix","MVDHitsPixelMix","MVDHitsStripMix");
    //  don't do the Pattern Recognition second part, starting from the Mvd;
    tracking->NoMvdAloneTracking();
    // do Cleanup only when there is Mixing;
    // tracking->Cleanup();
    fFixedPersistency[tracking] = kFALSE;

    brltrkname = "SttMvdTrack";
    brltrkgenname = "SttMvdGenTrack";

    // ----- MVD + STT + GEM Pattern Recognition --------------
    if ((!fOptions.Contains("nogem")) && (!fOptions.Contains("gem0"))) {
      PndSttMvdGemTracking *SttMvdGemTracking = nullptr;
      fBranchTasks.push_back(SttMvdGemTracking = new PndSttMvdGemTracking(0));
      fFixedPersistency[SttMvdGemTracking] = kFALSE;
      // SttMvdGemTracking->SetDefaultPdg(211); how to incorporate??  default is -13
      brltrkname = "SttMvdGemTrack";
      brltrkgenname = "SttMvdGemGenTrack";
    }
  } else {
    // Default
    // Use Radeks Tracking code
    PndBarrelTrackFinder *tracker = NULL;
    fBranchTasks.push_back(tracker = new PndBarrelTrackFinder());
    bool useMvd = true;
    bool useStt = true;
    bool useGem = true;
    if (fOptions.Contains("nogem") && fOptions.Contains("gem0")) {
      useGem = false;
    }
    tracker->UseMvdSttGem(useMvd, useStt, useGem);
    brltrkname = "BarrelTrack";
    brltrkgenname = "BarrelGenTrack";
  }

  // ----- Barrel Kalman Task     ----------------------------
  if (!fOptions.Contains("multikalman")) { // no multikalman
    if (fOptions.Contains("genfit2")) {
      PndRecoKalmanTask2 *recoKalman = nullptr;
      fBranchTasks.push_back(recoKalman = new PndRecoKalmanTask2()); // 2
      recoKalman->SetTrackInBranchName(brltrkname);
      recoKalman->SetTrackOutBranchName(brltrkgenname);
      recoKalman->SetPropagateToIP(kFALSE);
      // recoKalman->SetIdealHyp(kTRUE);
    } else if (fOptions.Contains("newGF2")) {
      PndGFRecoTask *recoKalman = nullptr;
      fBranchTasks.push_back(recoKalman = new PndGFRecoTask()); // 2
      recoKalman->SetTrackInBranchName(brltrkname);
      recoKalman->SetTrackOutBranchName(brltrkgenname);
    } else {
      PndRecoKalmanTask *recoKalman = nullptr;
      fBranchTasks.push_back(recoKalman = new PndRecoKalmanTask()); // 2
      recoKalman->SetTrackInBranchName(brltrkname);
      recoKalman->SetTrackOutBranchName(brltrkgenname);
      recoKalman->SetPropagateToIP(kFALSE);
      // recoKalman->SetPropagateToIP(kFALSE);
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
    } else if (fOptions.Contains("newGF2")) {
      PndGFRecoTask *recoKalman = nullptr;
      fBranchTasks.push_back(recoKalman = new PndGFRecoTask()); // 2
      recoKalman->SetTrackInBranchName(brltrkname);
      recoKalman->SetTrackOutBranchName(brltrkgenname);
      recoKalman->SetMultiPID();
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

  if (fOptions.Contains("filtered")) {
    PndMissingPzCleanerTask *cleaner = nullptr;
    fBranchTasks.push_back(cleaner = new PndMissingPzCleanerTask());
    cleaner->SetInputTrackBranch(brltrkgenname); // _filtered appendix follows
    cleaner->SetRemoveTrack(kTRUE);
  }

  // -----  FTS Tracking    ----------------------------
  TString ftstrkname = "FtsTrack", ftstrkgenname = "FtsGenTrack";
#ifdef VCSUPPORT
  if (fOptions.Contains("ftsca")) { // CA Tracker without Geane (yet)
    ftstrkname = "FtsCaTrack";
    ftstrkgenname = "FtsCaGenTrack";
    // if (!fOptions.Contains("multikalman")) { // no multikalman
    PndFtsCATracking *trackFts = nullptr;
    fBranchTasks.push_back(trackFts = new PndFtsCATracking("FtsCATracking", 2));
    trackFts->SetFtsTracksBranchName(ftstrkname);
    //} else {//yes, multikalman!
    // ftstrkgenname=ftstrkname;
    // for(int k=0; k<5; ++k) {
    // PndFtsCATracking *trackFts = nullptr;
    // TString hyp=hypoName[k];
    // hyp.ToLower();
    // if(!fOptions.Contains(hyp)) continue;
    // fBranchTasks.push_back(trackFts = new PndFtsCATracking("FtsCATracking",2));
    // trackFts->SetFtsTracksBranchName( Form("%s%s",ftstrkgenname.Data(),hypoName[k].Data()));
    //}
    //}
  } else
#endif // VCSUPPORT
    if (fOptions.Contains("ftstrack")) {
      ftstrkname = "FtsTrack";
      ftstrkgenname = "FtsGenTrack";
      PndFtsTrackFinderTask *trackFts = nullptr;
      fBranchTasks.push_back(trackFts = new PndFtsTrackFinderTask());
      trackFts->SetOutputBranchName(ftstrkname);

    } else {
      ftstrkname = "FtsIdealTrack";
      ftstrkgenname = "FtsIdealGenTrack";
      PndIdealTrackFinder *trackFts = nullptr;
      fBranchTasks.push_back(trackFts = new PndIdealTrackFinder("Ideal FWD Tracker"));
      trackFts->SetTrackSelector("FtsTrackFunctor");
      trackFts->AddBranchName("FTSHit");
      trackFts->AddBranchName("MVDHitsPixel");
      trackFts->AddBranchName("MVDHitsStrip");
      trackFts->SetRelativeMomentumSmearing(0.05);
      trackFts->SetVertexSmearing(0.05, 0.05, 0.05);
      trackFts->SetTrackingEfficiency(1.);
      trackFts->SetOutputBranchName(ftstrkname);
      fFixedPersistency[trackFts] = kFALSE;
    } // ftsca with geane

  // ----- Forward Kalman Task     ---------------------------
  if (!fOptions.Contains("multikalman")) { // no multikalman

    if (fOptions.Contains("genfit2")) {
      PndRecoKalmanTask2 *recoKalman = nullptr;
      fBranchTasks.push_back(recoKalman = new PndRecoKalmanTask2("FwdKalman")); // 2
      recoKalman->SetTrackInBranchName(ftstrkname);
      recoKalman->SetTrackOutBranchName(ftstrkgenname);
      recoKalman->SetPropagateToIP(kFALSE);
      // recoKalman->SetIdealHyp(kTRUE);
    } else if (fOptions.Contains("newGF2")) {
      PndGFRecoTask *recoKalman = nullptr;
      fBranchTasks.push_back(recoKalman = new PndGFRecoTask()); // 2
      recoKalman->SetTrackInBranchName(ftstrkname);
      recoKalman->SetTrackOutBranchName(ftstrkgenname);
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
    } else if (fOptions.Contains("newGF2")) {
      PndGFRecoTask *recoKalman = nullptr;
      fBranchTasks.push_back(recoKalman = new PndGFRecoTask()); // 2
      recoKalman->SetTrackInBranchName(ftstrkname);
      recoKalman->SetTrackOutBranchName(ftstrkgenname);
      recoKalman->SetMultiPID();
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
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndMasterRecoTask::~PndMasterRecoTask() {}
// -------------------------------------------------------------------------

/** @cond CLASSIMP */
ClassImp(PndMasterRecoTask);
/** @endcond */
