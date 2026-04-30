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

#ifndef PNDTRKGEMCOMBINATORIAL_H
#define PNDTRKGEMCOMBINATORIAL_H

// fairroot
#include "FairTask.h"

// ROOT
#include "TH2F.h"
#include "TCanvas.h"
#include "PndTrkGemCombinatorial.h"

class TClonesArray;

class PndTrkGemCombiTask : public FairTask {

 public:
  /** Default constructor **/
  PndTrkGemCombiTask();
  PndTrkGemCombiTask(int verbose);

  /** Destructor **/
  ~PndTrkGemCombiTask();

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  void Reset();

  inline void SwitchOnDisplay() { fDisplayOn = kTRUE; }
  inline void Evaluate() { fMCEval = kTRUE; }

 private:
  PndTrkGemCombinatorial *fCombiFinder;
  TClonesArray *fGemHitArray;
  TClonesArray *fGemPointArray;

  Bool_t fDisplayOn, fMCEval;

  ClassDef(PndTrkGemCombiTask, 1);
};

#endif
