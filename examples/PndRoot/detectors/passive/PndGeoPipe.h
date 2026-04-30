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

#ifndef PNDGEOPIPE_H
#define PNDGEOPIPE_H

#include "FairGeoSet.h"

class PndGeoPipe : public FairGeoSet {
 protected:
  char modName[2]; // name of module
  char eleName[2]; // substring for elements in module
 public:
  PndGeoPipe();
  virtual ~PndGeoPipe();
  const char *getModuleName(Int_t) { return modName; }
  const char *getEleName(Int_t) { return eleName; }
  Bool_t create(FairGeoBuilder *);
  ClassDef(PndGeoPipe, 0) // Class for geometry of beam pipe
};

#endif /* !PNDGEOPIPE_H */
