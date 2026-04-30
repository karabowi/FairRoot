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

/**
 * @class PndEventCounterTask
 * @brief Task to print the event number, the time needed per event and the expected remaining time
 *
 * @author Ralf Kliemt <r.kliemt@gsi.de>
 */

#ifndef PNDEVENTCOUNTERTASK_H
#define PNDEVENTCOUNTERTASK_H

#include "FairTask.h"
#include "TStopwatch.h"

class TClonesArray;

class PndEventCounterTask : public FairTask {
 public:
  PndEventCounterTask(const char *name = "Event Counter", Int_t nev = 1, Int_t talk = 1);
  virtual ~PndEventCounterTask();
  virtual void SetParContainers() { return; }
  virtual InitStatus Init();
  virtual InitStatus ReInit() { return kSUCCESS; }
  virtual void Exec(Option_t *opt);
  void SetVerbose(Int_t iVerbose) { fVerbose = iVerbose; }
  void SetAnnounce(Int_t iann) { fEvtTalk = iann; }
  void SetNEvts(Int_t n) { fNEvts = n; }
  Int_t GetEventCount() const { return fEvtCounter; }
  Double_t GetTime()
  {
    return fTimer.RealTime();
    fTimer.Continue();
  }

 private:
  void StartTimer();
  Bool_t fInitialiezed;
  Int_t fEvtCounter;
  Int_t fEvtTalk;
  Int_t fNEvts;
  Double_t fTimeOffset;
  TStopwatch fTimer;
  ClassDef(PndEventCounterTask, 1);
};

#endif
