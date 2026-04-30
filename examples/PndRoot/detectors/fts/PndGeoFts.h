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

#ifndef PNDGEOFTS_H
#define PNDGEOFTS_H

#include "FairGeoSet.h"

class PndGeoFts : public FairGeoSet {
 protected:
  char modName[40]; // name of module
  char eleName[40]; // substring for elements in module
 public:
  PndGeoFts();
  ~PndGeoFts() {}
  const char *getModuleName(Int_t);
  const char *getEleName(Int_t);
  inline Int_t getModNumInMod(const TString &);
  ClassDef(PndGeoFts, 0) // Class for Fts
};

inline Int_t PndGeoFts::getModNumInMod(const TString &name)
{
  // returns the module index from module name

  return (Int_t)(name[3] - '0') - 1;
}

#endif /* !PNDGEOFTS_H */
