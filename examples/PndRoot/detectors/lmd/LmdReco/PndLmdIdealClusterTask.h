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

#ifndef PNDLMDIDEALCLUSTERTASK_H
#define PNDLMDIDEALCLUSTERTASK_H

#include "PndSdsIdealClusterTask.h"

class PndLmdIdealClusterTask : public PndSdsIdealClusterTask {
 public:
  /** Default constructor \n
   **/
  PndLmdIdealClusterTask();
  PndLmdIdealClusterTask(Double_t radius, Int_t FEcolumns, Int_t FErows, TString geoFile);

  /** Destructor **/
  virtual ~PndLmdIdealClusterTask();

  virtual void SetBranchNames(TString inBranchname, TString outHitBranchname, TString outClustBranchname, TString folderName);
  virtual void SetBranchNames();

  ClassDef(PndLmdIdealClusterTask, 2);
};

#endif
