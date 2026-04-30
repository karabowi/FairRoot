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

//--------------------------------------------------------------------------
// Description:
//	Class Template
//------------------------------------------------------------------------

#ifndef PNDEMCFULLDIGITASK_H
#define PNDEMCFULLDIGITASK_H

#include <PndPersistencyTask.h>

/**
 * @brief combines the tasks PndEmcHitsToWaveform and PndEmcWaveformToDigi
 * @ingroup PndEmc
 */
class PndEmcFullDigiTask : public PndPersistencyTask {
 public:
  // Constructors
  PndEmcFullDigiTask(Int_t verbose = 0, Bool_t storedigis = kTRUE);

  // Destructor
  virtual ~PndEmcFullDigiTask();

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  void StoreDigi(Bool_t val);      // Method to specify whether digis are stored.
  void StoreWaveforms(Bool_t val); // Method to specify whether wavefors are stored.

 protected:
 private:
  /** Get parameter containers **/
  virtual void SetParContainers();

  /** Verbosity level **/
  Int_t fVerbose;

  static Int_t fEventCounter;
 
  ClassDef(PndEmcFullDigiTask, 1);
};
#endif // PndEmcFullDigiTask_HH
