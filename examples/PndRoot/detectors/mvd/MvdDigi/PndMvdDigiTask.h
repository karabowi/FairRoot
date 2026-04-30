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


/** PndMvdDigiTask.h
 **
 **/

#ifndef PNDMVDDIGITASK_H
#define PNDMVDDIGITASK_H

#include "PndPersistencyTask.h"

class TClonesArray;

class PndMvdDigiTask : public PndPersistencyTask {
 public:
  /** Default constructor **/
  PndMvdDigiTask();

  /** Destructor **/
  virtual ~PndMvdDigiTask();

  void SetPersistance(Bool_t p = kTRUE);
  Bool_t GetPersistance() { return fPersistance; };
  void RunTimeBased();

 private:
  Bool_t fPersistance; // switch to turn on/off storing the arrays to a file
  void Register();
  void Reset();
  void ProduceHits();
  ClassDef(PndMvdDigiTask, 1);
};

#endif /* MVDDIGITASK_H */
