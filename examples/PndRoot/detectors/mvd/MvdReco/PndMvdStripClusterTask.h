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
// -----                 PndMvdStripClusterTask header file             -----
// -------------------------------------------------------------------------

#ifndef PNDMVDSTRIPCLUSTERTASK_H
#define PNDMVDSTRIPCLUSTERTASK_H 1

#include "PndSdsStripClusterTask.h"

class PndMvdStripClusterTask : public PndSdsStripClusterTask {
 public:
  /** Default constructor **/
  PndMvdStripClusterTask();

  /** Destructor **/
  virtual ~PndMvdStripClusterTask();

  virtual void GetParList(TList *tempList);
  virtual void SetParContainersMQ(TList *tempList);

  void SetBranchNames(TString inBranchname, TString outHitBranchname, TString outClustBranchname, TString folderName);
  virtual void SetBranchNames();

  virtual void SetParContainers();
  virtual void SetCalculators();

  ClassDef(PndMvdStripClusterTask, 3);
};

#endif /* MVDCLUSTERTASK_H */
