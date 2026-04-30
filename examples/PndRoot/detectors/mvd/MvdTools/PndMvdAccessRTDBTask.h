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

#ifndef PNDMVDACCESSRTDBTASK_H
#define PNDMVDACCESSRTDBTASK_H

#include "FairTask.h"
#include "PndSdsPixelDigiPar.h"
#include "PndSdsStripDigiPar.h"

#include <string>
#include <vector>

class TClonesArray;

class PndMvdAccessRTDBTask : public FairTask {
 public:
  /** Default constructor **/
  PndMvdAccessRTDBTask() : fPixelDigiPar(nullptr), fStripDigiParRect(nullptr), fStripDigiParTrap(nullptr){};

  /** Destructor **/
  virtual ~PndMvdAccessRTDBTask(){};

  PndMvdAccessRTDBTask(const PndMvdAccessRTDBTask &) = delete;

  PndMvdAccessRTDBTask &operator=(const PndMvdAccessRTDBTask &) = delete;

  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();
  virtual InitStatus ReInit();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);
  /** Virtual method Finish **/
  virtual void Finish();

 private:
  PndSdsPixelDigiPar *fPixelDigiPar;
  PndSdsStripDigiPar *fStripDigiParRect;
  PndSdsStripDigiPar *fStripDigiParTrap;

  ClassDef(PndMvdAccessRTDBTask, 1);
};

#endif
