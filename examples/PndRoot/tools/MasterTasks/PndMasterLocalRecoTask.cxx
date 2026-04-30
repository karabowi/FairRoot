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
// -----                    PndMasterLocalRecoTask source file              -----
// -----                   Created 31/01/16  by S. Spataro             -----
// -----                   Wrapper for  digitizing tasks               -----
// -------------------------------------------------------------------------

#include "PndMasterLocalRecoTask.h"
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
PndMasterLocalRecoTask::PndMasterLocalRecoTask(TString options) : PndMasterTask("Master Local Reco Task"), fOptions(options)
{

  fBranchTasks.push_back(new PndSttHitProducerRealFast());
  fBranchTasks.push_back(new PndFtsHitProducerRealFast());
  fBranchTasks.push_back(new PndSciTDigiTask());
  fBranchTasks.push_back(new PndMdtHitProducerIdeal());
  fBranchTasks.push_back(new PndDrcHitProducerReal());
  fBranchTasks.push_back(new PndFtofHitProducerIdeal());
  if (!fOptions.Contains("day1") || !fOptions.Contains("phase1")) {
    fBranchTasks.push_back(new PndRichHitProducer());
  }

  fBranchTasks.push_back(new PndMvdClusterTask());

  fBranchTasks.push_back(new PndEmcMakeCluster());
  fBranchTasks.push_back(new PndEmcMakeBump());

  fBranchTasks.push_back(new PndMdtTrkProducer());

  // -----   GEM hit producers   ---------------------------------
  if ((!fOptions.Contains("nogem")) || (!fOptions.Contains("gem0"))) {
    fBranchTasks.push_back(new PndGemFindHits("GEM Hit Finder", 0));
  }

  std::for_each(fBranchTasks.begin(), fBranchTasks.end(), [this](const FairTask *task) { Add((TTask *)task); });

  SetVerbose(0);
}
// -------------------------------------------------------------------------

/** Set the Persistency of all the tasks in the same way **/
void PndMasterLocalRecoTask::SetPersistency(Bool_t pers)
{
  std::for_each(fBranchTasks.begin(), fBranchTasks.end(), [pers](PndPersistencyTask *task) { task->SetPersistency(pers); });
  std::for_each(fFixedPersistency.begin(), fFixedPersistency.end(), [](std::pair<PndPersistencyTask *, bool> task) { task.first->SetPersistency(task.second); });

  return;
}

// -----   Destructor   ----------------------------------------------------
PndMasterLocalRecoTask::~PndMasterLocalRecoTask() {}
// -------------------------------------------------------------------------

/** @cond CLASSIMP */
ClassImp(PndMasterLocalRecoTask);
/** @endcond */
