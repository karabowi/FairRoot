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
 * PndEventCombinerTask_tb.cxx
 *
 *  Created on: July 13, 2017
 *      Author: Steinschaden
 *
 */

#include "PndBranchBurstBuilder_timeCut.h"
#include "TClonesArray.h"

// framework includes
#include "FairRootManager.h"
#include "FairTimeStamp.h"

PndBranchBurstBuilder_timeCut::PndBranchBurstBuilder_timeCut() : PndBranchBurstBuilder()
{
  fOutputPrefix = "Burst_tb";
  fTimePeriod = 2000.0; // in nano seconds
}
PndBranchBurstBuilder_timeCut::~PndBranchBurstBuilder_timeCut() {}

// -----   Public method Init   --------------------------------------------
InitStatus PndBranchBurstBuilder_timeCut::Init()
{

  InitStatus status = PndBranchBurstBuilder::Init();

  fStopFunctor = new StopTime();

  return status;
}

TClonesArray *PndBranchBurstBuilder_timeCut::GetBurstData(size_t branchNum)
{
  FairRootManager *ioman = FairRootManager::Instance();

  // the absorbtion of the Array to the Writeout Array will "clear" the memmory of this Array so that in the next loop only new data are present
  return ioman->GetData(fInBranchNames[branchNum], fStopFunctor, ioman->GetEntryNr() * fTimePeriod + fTimePeriod);
}

ClassImp(PndBranchBurstBuilder_timeCut);
