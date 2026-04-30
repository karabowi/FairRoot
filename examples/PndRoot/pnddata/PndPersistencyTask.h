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
// -----                 PndPersistencyTask header file                    -----
// -----           Created 28/02/18  by T. Stockmanns		         -----
// -------------------------------------------------------------------------

/** PndPersistencyTask.h
 *@author T.Stockmanns <t.stockmanns@fz-juelich.de>
 **
 ** Version 28/02/18 by T.Stockmanns
 **
 ** This class handles the persistency parameter used to define if an output branch is stored in a root file
 ** or not.
 **/

#ifndef PndPersistencyTask_H
#define PndPersistencyTask_H

#include "FairTask.h"

class PndPersistencyTask : public FairTask {

 public:
  PndPersistencyTask();
  PndPersistencyTask(const char *name, Int_t iVerbose = 1);
  virtual ~PndPersistencyTask();

  void SetPersistency(Bool_t val = kTRUE) { fPersistency = val; }
  Bool_t GetPersistency() { return fPersistency; }

 private:
  Bool_t fPersistency; //!   ///< Persistency flag

  ClassDef(PndPersistencyTask, 1)
};

#endif
