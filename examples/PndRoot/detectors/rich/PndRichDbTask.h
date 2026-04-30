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

#ifndef PNDRICHDBTASK_H
#define PNDRICHDBTASK_H 1

#include "FairTask.h"
#include "PndRichReco.h"
#include "TVector3.h"
#include "TH1F.h"
#include "TF1.h"
#include <vector>

class TClonesArray;

class PndRichDbTask : public FairTask {

 public:
  /** Default constructor **/
  PndRichDbTask();

  /** Destructor **/
  ~PndRichDbTask();

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  /** set persistence flag **/
  void SetPersistence(Bool_t persistence) { fPersistence = persistence; }

  void FinishEvent();
  void FinishTask();

 private:
  /** object persistence **/
  Bool_t fPersistence; //!

  ClassDef(PndRichDbTask, 1);
};

#endif
