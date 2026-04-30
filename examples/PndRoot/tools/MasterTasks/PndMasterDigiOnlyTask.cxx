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
// -----                    PndMasterDigiOnlyTask source file              -----
// -----                   Created 31/01/16  by S. Spataro             -----
// -----                   Wrapper for  digitizing tasks               -----
// -------------------------------------------------------------------------

#include "PndMasterDigiOnlyTask.h"
#include "PndMasterTask.h"

#include "PndSttHitProducerRealFast.h"
#include "PndMvdDigiTask.h"
#include "PndMvdClusterTask.h"
#include "PndEmcHitsToWaveform.h"
#include "PndEmcWaveformToDigi.h"
#include "PndEmcMakeCluster.h"
#include "PndEmcMakeBump.h"
#include "PndSciTHitProducerIdeal.h"
#include "PndSciTDigiTask.h"
#include "PndMdtHitProducerIdeal.h"
#include "PndMdtTrkProducer.h"
#include "PndDrcHitProducerReal.h"
#include "PndGemDigitize.h"
#include "PndGemFindHits.h"
#include "PndFtsHitProducerRealFast.h"
#include "PndFtofHitProducerIdeal.h"
#include "PndRichHitProducer.h"

#include <algorithm>

/**
 * @brief Default Constructor
 * @details # Digitization task list
 * Here all the digitization tasks are added to the task, with the standard settings. A check is done after each task if the tasklist enum is broken or not. At the end the event
 *counter is added (each 100 events), and the verbosity is set to 0 to all the tasks (it can be changed afterwards with SetVerbosity() functions.
 **/
// -----   Default constructor   -------------------------------------------
PndMasterDigiOnlyTask::PndMasterDigiOnlyTask(TString options) : PndMasterTask("Master Digitization Only Task"), fOptions(options)
{

  fBranchTasks.push_back(new PndSttHitProducerRealFast());
  fBranchTasks.push_back(new PndMvdDigiTask());

  fBranchTasks.push_back(new PndEmcHitsToWaveform());
  fFixedPersistency[fBranchTasks.back()] = false;
  fBranchTasks.push_back(new PndEmcWaveformToDigi());

  fBranchTasks.push_back(new PndSciTDigiTask());
  fBranchTasks.push_back(new PndMdtHitProducerIdeal());
  ((PndMdtHitProducerIdeal *)(fBranchTasks.back()))->SetPositionSmearing(.3);
  fBranchTasks.push_back(new PndDrcHitProducerReal());

  // -----   GEM hit producers   ---------------------------------
  if ((!fOptions.Contains("nogem")) || (!fOptions.Contains("gem0"))) {
    fBranchTasks.push_back(new PndGemDigitize("GEM Digitizer", 0));
  }
  fBranchTasks.push_back(new PndFtsHitProducerRealFast());
  fBranchTasks.push_back(new PndFtofHitProducerIdeal());

  // -----   Rich hit producers   ---------------------------
  if (!fOptions.Contains("day1") || !fOptions.Contains("phase1")) {
    fBranchTasks.push_back(new PndRichHitProducer());
  }

  std::for_each(fBranchTasks.begin(), fBranchTasks.end(), [this](const FairTask *task) { Add((TTask *)task); });

  SetVerbose(0);
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndMasterDigiOnlyTask::~PndMasterDigiOnlyTask() {}
// -------------------------------------------------------------------------

/** @cond CLASSIMP */
ClassImp(PndMasterDigiOnlyTask);
/** @endcond */
