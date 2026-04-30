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
// -----                     PndGeoEmc header file                     -----
// -----               Created 14/08/06  by S.Spataro                  -----
// -------------------------------------------------------------------------
//#pragma once
#ifndef PNDGEOEMC_H
#define PNDGEOEMC_H

#include "FairGeoSet.h"

class PndGeoEmc : public FairGeoSet {
 protected:
  char modName[20]; // name of module
  char eleName[20]; // substring for elements in module

 public:
  PndGeoEmc();
  ~PndGeoEmc() {}
  const char *getModuleName(Int_t);
  const char *getEleName(Int_t);

  ClassDef(PndGeoEmc, 0) // Class for Emc
};

#endif /* !PNDGEOEMC_H */
