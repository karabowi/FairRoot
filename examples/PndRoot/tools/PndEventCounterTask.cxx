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
// -----                   PndEventCounterTask source file             -----
// -----                  Created 24/09/10  by R. Kliemt               -----
// -------------------------------------------------------------------------

#include "PndEventCounterTask.h"

#include "FairRootManager.h"

#include <iostream>

PndEventCounterTask::PndEventCounterTask(const char *name, Int_t nev, Int_t talk)
  : FairTask(name), fInitialiezed(kFALSE), fEvtCounter(0), fEvtTalk(talk), fNEvts(nev), fTimeOffset(0.), fTimer()
{
  fTimer.Start();
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndEventCounterTask::~PndEventCounterTask() {}
// -------------------------------------------------------------------------

// -----   Init         ----------------------------------------------------
InitStatus PndEventCounterTask::Init()
{
  if (fNEvts == 0) {
    fNEvts = FairRootManager::Instance()->CheckMaxEventNo(0);
  }

  return kSUCCESS;
}
// -------------------------------------------------------------------------

void PndEventCounterTask::StartTimer()
{
  fTimeOffset = fTimer.RealTime();
  fTimer.Continue();
  fEvtCounter = 0;
  fInitialiezed = kTRUE;
}

// -----   Public method Exec   --------------------------------------------
void PndEventCounterTask::Exec(Option_t *)
{
  if (!fInitialiezed)
    StartTimer();
  fEvtCounter++;
  if (fEvtCounter % fEvtTalk == 0 || fVerbose > 1) {
    Double_t t = fTimer.RealTime();
    fTimer.Continue();
    printf("Event %i/%i : time %6.1f sec, (%6.0f sec remaining)\n", fEvtCounter, fNEvts, t, (t - fTimeOffset) * (fNEvts - fEvtCounter) / fEvtCounter);
  }
  return;
}

ClassImp(PndEventCounterTask);
