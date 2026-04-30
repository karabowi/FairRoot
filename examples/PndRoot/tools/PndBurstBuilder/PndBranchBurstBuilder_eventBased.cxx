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

/*
 * PndEventCombinerTask_eb.cxx
 *
 *  Created on: July 13, 2017
 *      Author: Steinschaden
 *
 */

#include "PndBranchBurstBuilder_eventBased.h"
#include "TClonesArray.h"

// framework includes
#include "FairRootManager.h"

PndBranchBurstBuilder_eventBased::PndBranchBurstBuilder_eventBased() : PndBranchBurstBuilder()
{
  fNEvents = 3;
  fOutputPrefix = "Burst_eb";
}

PndBranchBurstBuilder_eventBased::~PndBranchBurstBuilder_eventBased() {}

// -----   Public method Init   --------------------------------------------
InitStatus PndBranchBurstBuilder_eventBased::Init()
{

  InitStatus status = PndBranchBurstBuilder::Init();

  for (size_t i = 0; i < fInBranchNames.size(); i++) {
    fTempArrays.push_back(new TClonesArray(fInArrays[i]->GetClass()->GetName()));
  }

  return status;
}

TClonesArray *PndBranchBurstBuilder_eventBased::GetBurstData(size_t branchNum)
{

  FairRootManager *ioman = FairRootManager::Instance();
  // generate empty dummy array to retrun when not enough data is collected for a burst
  TClonesArray *emptyArray = new TClonesArray(fInArrays[branchNum]->GetClass()->GetName());

  fTempArrays[branchNum]->AbsorbObjects(fInArrays[branchNum]); // fill the temp array with the data of the new events
  if (ioman->GetEntryNr() % fNEvents == fNEvents - 1) {        // if enough data is collected :
    return fTempArrays[branchNum];                             // the absorbtion of the returned TClonesArray will reset the TempArray
  }

  return emptyArray;
}

void PndBranchBurstBuilder_eventBased::FinishTask()
{
  // write out the data collected for a maybe last and uncomplete Burst

  for (size_t i = 0; i < fInBranchNames.size(); i++) {
    fOutArrays[i]->Delete(); // make sure data written out from old events are deleted
    fOutArrays[i]->AbsorbObjects(fTempArrays[i]);
  }
  FairRootManager::Instance()->SetLastFill();
}

ClassImp(PndBranchBurstBuilder_eventBased);
