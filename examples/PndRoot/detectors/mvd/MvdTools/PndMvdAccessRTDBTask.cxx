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

#include "PndMvdAccessRTDBTask.h"

#include "FairRuntimeDb.h"
#include "FairRun.h"

ClassImp(PndMvdAccessRTDBTask);

void PndMvdAccessRTDBTask::SetParContainers()
{
  FairRun *ana = FairRun::Instance();
  FairRuntimeDb *rtdb = ana->GetRuntimeDb();
  fPixelDigiPar = (PndSdsPixelDigiPar *)(rtdb->getContainer("MVDPixelDigiPar"));
  fStripDigiParRect = (PndSdsStripDigiPar *)(rtdb->getContainer("MVDStripDigiParRect"));
  fStripDigiParTrap = (PndSdsStripDigiPar *)(rtdb->getContainer("MVDStripDigiParTrap"));
}

InitStatus PndMvdAccessRTDBTask::Init()
{
  return kSUCCESS;
}

InitStatus PndMvdAccessRTDBTask::ReInit()
{
  SetParContainers();
  return kSUCCESS;
}

void PndMvdAccessRTDBTask::Exec(Option_t *)
{
  return;
}

void PndMvdAccessRTDBTask::Finish()
{
  fPixelDigiPar->Print();
  fStripDigiParRect->Print();
  fStripDigiParTrap->Print();
  return;
}
