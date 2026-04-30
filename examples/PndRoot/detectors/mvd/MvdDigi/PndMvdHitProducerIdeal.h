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
// -----                 CbmMvdHitProducerIdel header file             -----
// -------------------------------------------------------------------------

/** CbmMvdHitProducerIdeal.h
 *@author R.Kliemt
 ** Inherit from Sds Ideal hitproducer
 **/

#ifndef PNDMVDHITPRODUCERIDEAL_H
#define PNDMVDHITPRODUCERIDEAL_H 1

#include "PndSdsHitProducerIdeal.h"

class PndMvdHitProducerIdeal : public PndSdsHitProducerIdeal {
 public:
  /** Default constructor **/
  PndMvdHitProducerIdeal();

  /** Destructor **/
  virtual ~PndMvdHitProducerIdeal();

  void SetBranchNames(TString inBranchname, TString outBranchname, TString folderName);
  virtual void SetBranchNames();

  ClassDef(PndMvdHitProducerIdeal, 4);
};

#endif
