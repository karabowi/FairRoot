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
 * PndMvdRadDamIonizingTask.h
 *
 *  Created on: Dec 16, 2008
 *      Author: stockman
 */

#ifndef PndMvdRadDamIonizingTask_H
#define PndMvdRadDamIonizingTask_H

#include "FairTask.h"
#include "PndGeoHandling.h"

#include "TClonesArray.h"
#include "TProfile2D.h"

#include <map>
#include <string>

class PndMvdRadDamIonizingTask : public FairTask {
 public:
  PndMvdRadDamIonizingTask();
  ~PndMvdRadDamIonizingTask();
  PndMvdRadDamIonizingTask(const PndMvdRadDamIonizingTask &) = delete;
  PndMvdRadDamIonizingTask &operator=(const PndMvdRadDamIonizingTask &) = delete;

  virtual void SetParContainers();
  virtual InitStatus Init();
  virtual InitStatus ReInit();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);
  virtual void FinishEvent();
  virtual void FinishTask();

  void SetPersistance(Bool_t p = kTRUE) { fPersistance = p; };
  Bool_t GetPersistance() { return fPersistance; };

 private:
  Bool_t fPersistance; // switch to turn on/off storing the arrays to a file
  TClonesArray *fMCHits;
  TClonesArray *fRadDamHits;

  PndGeoHandling *fGeoH;

  std::map<std::string, TProfile2D *> fMapDetHistos;
  TH1D *fRadDamHisto;

  ClassDef(PndMvdRadDamIonizingTask, 1);
};

#endif /* PndMvdRadDamIonizingTask_H */
