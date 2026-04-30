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
 * Updated by h.xu@fz-juelich.de on Jun30.2010
 * To match the base class SdsStripHitProducer,the virtual functions SetParContainers() and
 * SetCalculators() were reloaded.
 */

#ifndef PNDLMDSTRIPHITPRODUCER_H
#define PNDLMDSTRIPHITPRODUCER_H

#include "PndSdsStripHitProducer.h"

class PndLmdStripHitProducer : public PndSdsStripHitProducer {
 public:
  /** Default constructor \n
   * creates object with parameters taken implicitly from DigiPar-File
   **/
  PndLmdStripHitProducer();

  /** Destructor **/
  virtual ~PndLmdStripHitProducer();

  virtual void SetBranchNames(TString inBranchname, TString outBranchname, TString folderName);
  virtual void SetBranchNames();
  virtual void SetCalculators();
  virtual void SetParContainers();

  ClassDef(PndLmdStripHitProducer, 65);
};

#endif
