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
 * PndMvdTimeWalkCorrectionTask.cxx
 *
 *  Created on: Aug 27, 2010
 *      Author: Simone Esch
 */

#include "PndMvdTimeWalkCorrTask.h"

// -----   Default constructor   -------------------------------------------

PndMvdTimeWalkCorrTask::PndMvdTimeWalkCorrTask() : PndSdsTimeWalkCorrTask("SDS TimeWalkCorrection Task")
{
  SetPersistency(true);
}

PndMvdTimeWalkCorrTask::~PndMvdTimeWalkCorrTask() {}

// -----   Default I/O folder/branches   ----------------------------------------------------
void PndMvdTimeWalkCorrTask::SetBranchNames()
{
  fInBranchName = "MVDPixelDigis"; // Name in digi_complete
  fOutBranchName = "MVDDigisCorr"; // Name in reco_complete
  fFolderName = "PndMvd";
}

void PndMvdTimeWalkCorrTask::SetParContainers()
{

  FairRun *ana = FairRun::Instance();
  FairRuntimeDb *rtdb = ana->GetRuntimeDb();
  fDigiPar = (PndSdsPixelDigiPar *)(rtdb->getContainer("MVDPixelDigiPar"));
  fDigiTotPar = (PndSdsTotDigiPar *)(rtdb->getContainer("MVDPixelTotDigiPar"));
  PndSdsTimeWalkCorrTask::SetParContainers();
}
