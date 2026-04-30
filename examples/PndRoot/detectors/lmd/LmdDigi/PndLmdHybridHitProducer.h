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


/** PndLmdHybridHitProducer.h
 **
 **/

#ifndef PNDLMDHYBRIDHITPRODUCER_H
#define PNDLMDHYBRIDHITPRODUCER_H 1

#include "PndSdsHybridHitProducer.h"
#include "PndSdsTotDigiPar.h"
#include "TString.h"

class PndLmdHybridHitProducer : public PndSdsHybridHitProducer {
 public:
  /** Default constructor **/
  PndLmdHybridHitProducer();

  /** Destructor **/
  virtual ~PndLmdHybridHitProducer();

  /** Specific Parameter set loading **/
  virtual void SetParContainers();

  virtual InitStatus Init();

  /** Specify the branch & folder names for the I/O **/
  void SetBranchNames(TString inBranchname, TString outBranchname, TString folderName);

  /** Take default naming **/
  virtual void SetBranchNames();

  ClassDef(PndLmdHybridHitProducer, 9);
};

#endif
