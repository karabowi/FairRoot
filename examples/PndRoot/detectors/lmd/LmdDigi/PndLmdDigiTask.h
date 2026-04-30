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


/** PndLmdDigiTask.h
 **
 **/

#ifndef PNDLMDDIGITASK_H
#define PNDLMDDIGITASK_H

#include "FairTask.h"

class TClonesArray;

class PndLmdDigiTask : public FairTask {
 public:
  /** Default constructor **/
  PndLmdDigiTask();

  /** Destructor **/
  virtual ~PndLmdDigiTask();

  /** Virtual method Init **/
  //   virtual void SetParContainers();
  //   virtual InitStatus Init();
  //   virtual InitStatus ReInit();

  /** Virtual method Exec **/
  //   virtual void Exec(Option_t* opt);

 private:
  //  void Register();
  //  void Reset();
  //  void ProduceHits();
  ClassDef(PndLmdDigiTask, 2);
};

#endif /* LMDDIGITASK_H */
