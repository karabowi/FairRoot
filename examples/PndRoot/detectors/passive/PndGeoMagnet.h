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

#ifndef PNDGEOMAGNET_H
#define PNDGEOMAGNET_H

#include "FairGeoSet.h"
#include "TString.h"

class PndGeoMagnet : public FairGeoSet {
 protected:
  char modName[2]; // name of module
  char eleName[2]; // substring for elements in module
 public:
  PndGeoMagnet();
  ~PndGeoMagnet() {}
  const char *getModuleName(Int_t) { return modName; }
  const char *getEleName(Int_t) { return eleName; }
  ClassDef(PndGeoMagnet, 0) // Class for the geometry of Magnet
};

#endif /* !CBMGEOMAGNET_H */
