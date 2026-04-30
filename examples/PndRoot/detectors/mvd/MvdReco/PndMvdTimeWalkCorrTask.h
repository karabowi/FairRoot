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
 * PndMvdTimeWalkCorrctionTask.h
 *
 *  Created on: Aug 27, 2010
 *      Author: Simone Esch
 */

#ifndef PNDMVDTIMEWALKCORRTASK_H_
#define PNDMVDTIMEWALKCORRTASK_H_

#include "PndSdsTimeWalkCorrTask.h"
#include "TString.h"
#include "FairRuntimeDb.h"
#include "FairRun.h"

class PndMvdTimeWalkCorrTask : public PndSdsTimeWalkCorrTask {
 public:
  PndMvdTimeWalkCorrTask();
  virtual ~PndMvdTimeWalkCorrTask();

  void SetBranchNames();

  void SetParContainers();

  ClassDef(PndMvdTimeWalkCorrTask, 1);
};

#endif /* PNDMVDTIMEWALKCORRTASK_H_ */
