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

// --------------------------------------------------------------------------------
// -----                    PndMasterMultiPidTask source file                 -----
// -----                   Created 07/05/18  by W. Ikegami Andersson          -----
// -----                       Wrapper for pid tasks when running multikalman -----
// --------------------------------------------------------------------------------

#include "FairLogger.h"
#include "PndMasterMultiPidTask.h"
#include "PndMasterTask.h"

#include "PndPidCorrelator.h"
#include "PndPidBremCorrector.h"
#include "PndMcCloner.h"
#include "PndMcClonerMulti.h"
#include "PndPidIdealAssociatorTask.h"
#include "PndPidMvdAssociatorTask.h"
#include "PndPidMdtHCAssociatorTask.h"
#include "PndPidDrcAssociatorTask.h"
#include "PndPidDiscAssociatorTask.h"
#include "PndPidSttAssociatorTask.h"
#include "PndPidEmcBayesAssociatorTask.h"
#include "PndPidSciTAssociatorTask.h"
#include "PndPidFtofAssociatorTask.h"
#include "PndPidRichAssociatorTask.h"
#include "PndMatchBarrelForward.h"
#include "PndHadronicSplitOffCut.h"

#include <array>
/**
 * @brief Default Constructor
 * @details # Pid task list
 * Here all the pid tasks are added to the task, with the standard settings. A check is done after each task if the tasklist enum is broken or not. At the end the event counter is
 *added (each 100 events), and the verbosity is set to 0 to all the tasks (it can be changed afterwards with SetVerbosity() functions.
 **/
// -----   Default constructor   -------------------------------------------
PndMasterMultiPidTask::PndMasterMultiPidTask(TString options) : PndMasterTask("Master Pid Task"), fOptions(options)
{
  std::array<Int_t, 6> hypoPdg = {11, 13, 211, 321, 2212, 211}; // last entry is used without multikalman
  std::array<TString, 6> hypoName = {"Electron", "Muon", "Pion", "Kaon", "Proton", ""};
  for (int i = 0; i < 6; i++)
    fHypoFlag[i] = false;

  unsigned int nfits = 0;
  if (fOptions.Contains("multikalman")) {
    if (fOptions.Contains("electron")) {
      nfits++;
      fHypoFlag[0] = true;
    }
    if (fOptions.Contains("muon")) {
      nfits++;
      fHypoFlag[1] = true;
    }
    if (fOptions.Contains("pion")) {
      nfits++;
      fHypoFlag[2] = true;
    }
    if (fOptions.Contains("kaon")) {
      nfits++;
      fHypoFlag[3] = true;
    }
    if (fOptions.Contains("proton")) {
      nfits++;
      fHypoFlag[4] = true;
    }
    if (nfits == 0) {
      LOG(info) << "PndMasterMultiPidTask: No hypotheses given, running pid with all 5 hypothesis" << std::endl;
      fHypoFlag[0] = true;
      fHypoFlag[1] = true;
      fHypoFlag[2] = true;
      fHypoFlag[3] = true;
      fHypoFlag[4] = true;
    }
  } else {
    fHypoFlag[5] = true;
  }

  for (int iHyp = 0; iHyp < 6; iHyp++) {
    if (!fHypoFlag[iHyp])
      continue;

    // -----   Correlation   ---------------------------------
    PndPidCorrelator *correlator = new PndPidCorrelator("PndPidCorrelator_" + hypoName[iHyp]);
    correlator->SetOption(fOptions);
    correlator->SetPidHyp(hypoPdg[iHyp]);
    correlator->SetOutputBranch(hypoName[iHyp]); // suffixes for branches
    if (!fOptions.Contains("goodtracks"))
      correlator->SetFlagCut(kFALSE);

    // Set the track branch according to the options
    TString brlbranchname = "BarrelGenTrack"; // default
    if (fOptions.Contains("stttracking")) {
      brlbranchname = "SttMvdGemGenTrack";
    } else if (fOptions.Contains("nogem") || fOptions.Contains("gem0")) {
      brlbranchname = "SttMvdGenTrack";
    } else if (fOptions.Contains("finalgen")) {
      brlbranchname = "FinalGenTrack";
    }

    brlbranchname += hypoName[iHyp];
    if (fOptions.Contains("filtered"))
      brlbranchname += "_filtered";
    if (fOptions.Contains("fakeonline"))
      brlbranchname += "_fakeonline";

    TString fwdbranchname = "FtsIdealGenTrack";
    if (fOptions.Contains("ftsca"))
      fwdbranchname = "FtsCaGenTrack";
    fwdbranchname += hypoName[iHyp];
    if (fOptions.Contains("fakeonline"))
      fwdbranchname += "_fakeonline";

    PndMatchBarrelForward *TrackCombiner = new PndMatchBarrelForward();
    this->Add(TrackCombiner);

    TrackCombiner->SetBarrelTrackBranch(brlbranchname);
    TrackCombiner->SetForwardTrackBranch(fwdbranchname);

    correlator->SetTrackBranch("CombinedTracks");

    if (fOptions.Contains("piddebug"))
      correlator->SetDebugMode(kTRUE);
    if (fOptions.Contains("pidfast"))
      correlator->SetFast(kTRUE);
    if (fOptions.Contains("pidnoswim"))
      correlator->SetBackPropagate(kFALSE);

    // -----   Bremsstrahlung Correction ----------------------
    if (iHyp == 0 || iHyp == 5) {
      // Do Bremsstrahlung correction only for electrons or unknown hypothesis
      PndPidBremCorrector *PidBrem = new PndPidBremCorrector("PndPidBremCorrector_" + hypoName[iHyp]);
      PidBrem->SetOutputBranch(hypoName[iHyp]);
      correlator->Add(PidBrem); // 2
    }

    PndPidMvdAssociatorTask *PidMvd = new PndPidMvdAssociatorTask("PndPidMvdAssociatorTask_" + hypoName[iHyp]);
    PidMvd->SetOutputBranch(hypoName[iHyp]);
    correlator->Add(PidMvd); // 5

    PndPidMdtHCAssociatorTask *PidMdt = new PndPidMdtHCAssociatorTask("PndPidMdtHCAssociatorTask_" + hypoName[iHyp]);
    PidMdt->SetOutputBranch(hypoName[iHyp]);
    correlator->Add(PidMdt); // 6

    PndPidDrcAssociatorTask *PidDrc = new PndPidDrcAssociatorTask("PndPidDrcAssociatorTask_" + hypoName[iHyp]);
    PidDrc->SetOutputBranch(hypoName[iHyp]);
    correlator->Add(PidDrc); // 7

    if (!fOptions.Contains("day1") && !fOptions.Contains("phase1")) {
      PndPidDiscAssociatorTask *PidDisc = new PndPidDiscAssociatorTask("PndPidDiscAssociatorTask_" + hypoName[iHyp]);
      PidDisc->SetOutputBranch(hypoName[iHyp]);
      correlator->Add(PidDisc); // 8
    }

    PndPidSttAssociatorTask *PidStt = new PndPidSttAssociatorTask("PndPidSttAssociatorTask_" + hypoName[iHyp]);
    PidStt->SetOutputBranch(hypoName[iHyp]);
    correlator->Add(PidStt); // 9

    PndPidEmcBayesAssociatorTask *PidEmcBayes = new PndPidEmcBayesAssociatorTask("PndPidEmcBayesAssociatorTask_" + hypoName[iHyp]);
    PidEmcBayes->SetOutputBranch(hypoName[iHyp]);
    correlator->Add(PidEmcBayes); // 10

    PndPidSciTAssociatorTask *PidSciT = new PndPidSciTAssociatorTask("PndPidSciTAssociatorTask_" + hypoName[iHyp]);
    PidSciT->SetOutputBranch(hypoName[iHyp]);
    correlator->Add(PidSciT); // 11

    PndPidFtofAssociatorTask *PidFtof = new PndPidFtofAssociatorTask("PndPidFtofAssociatorTask_" + hypoName[iHyp]);
    PidFtof->SetOutputBranch(hypoName[iHyp]);
    correlator->Add(PidFtof); // 12

    if (!fOptions.Contains("day1") && !fOptions.Contains("phase1")) {
      PndPidRichAssociatorTask *PidRich = new PndPidRichAssociatorTask("PndPidRichAssociatorTask_" + hypoName[iHyp]);
      PidRich->SetOutputBranch(hypoName[iHyp]);
      correlator->Add(PidRich); // 13
    }

    // Now we're done setting up
    this->Add(correlator); // Adding to task list

    // Cut hadronic split-offs around charged candidates
    PndHadronicSplitOffCut *HadronCorrection = new  PndHadronicSplitOffCut();
    this->Add(HadronCorrection);
  }

  // -----   MC Cloner   ------------------------------------
  // cloner that goes through all candidates. Run only once, and after all correlators
  if (!fOptions.Contains("nomc")) {
    if (fOptions.Contains("multikalman")) {
      PndMcClonerMulti *clone = new PndMcClonerMulti();
      // Option to clean the MCTrack TClonesArray from particles which were not interacting with sensitive detectors
      clone->SetCleanMc();
      this->Add(clone); // 3
    } else {
      PndMcCloner *clone = new PndMcCloner();
      // Option to clean the MCTrack TClonesArray from particles which were not interacting with sensitive detectors
      clone->SetCleanMc();
      this->Add(clone); // 3
    }

    for (int iHyp = 0; iHyp < 6; iHyp++) {
      if (!fHypoFlag[iHyp])
        continue;
      PndPidIdealAssociatorTask *PidIdeal = new PndPidIdealAssociatorTask("IdealPid_" + hypoName[iHyp]);
      PidIdeal->SetOutputBranch(hypoName[iHyp]);
      this->Add(PidIdeal); // 4
    }
  }

  PrintTaskSetup();

  SetVerbose(0);
}
// -------------------------------------------------------------------------

/** Set the Persistency of all the tasks in the same way **/
void PndMasterMultiPidTask::SetPersistency(Bool_t pers)
{
  if (!pers)
    LOG(info) << "It makes no sense to have pid persistency switched OFF!";
  if (!pers)
    LOG(info) << "Or, if you prefer... this functionality has not been implemented yet";

  return;
}

/** Print the setup of this instance **/
void PndMasterMultiPidTask::PrintTaskSetup()
{
  LOG(info) << "PndMasterMultiPidTask  ###### " << GetName();
  Print();
  PrintTaskList();
}

// -----   Destructor   ----------------------------------------------------
PndMasterMultiPidTask::~PndMasterMultiPidTask() {}
// -------------------------------------------------------------------------

/** @cond CLASSIMP */
ClassImp(PndMasterMultiPidTask);
/** @endcond */
