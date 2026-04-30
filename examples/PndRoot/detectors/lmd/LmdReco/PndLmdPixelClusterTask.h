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
// -------------------------------------------------------------------------

#ifndef PNDLMDPIXELCLUSTERTASK_H
#define PNDLMDPIXELCLUSTERTASK_H

#include "PndSdsPixelClusterTask.h"
#include "TString.h"

class FairRun;
class FairRuntimeDb;

class PndLmdPixelClusterTask : public PndSdsPixelClusterTask {
  FairRun *ana;
  FairRuntimeDb *rtdb;

 public:
  /** Default constructor **/
  PndLmdPixelClusterTask();

  /** Destructor **/
  virtual ~PndLmdPixelClusterTask();

  virtual void SetParContainers();
  virtual void SetBranchNames();
  virtual void SetClusterFinder();
  virtual void SetBackMapping();
  virtual void SetBranchNames(TString inBranchname, TString outHitBranchname, TString outClustBranchname, TString folderName);
  /** Virtual method Exec **/
  void Exec(Option_t *opt);
  void FinishEvent();

  InitStatus Init();
  ClassDef(PndLmdPixelClusterTask, 3);
};

#endif /* LMDCLUSTERTASK_H */
