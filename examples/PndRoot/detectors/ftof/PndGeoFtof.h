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
// -----                     PndGeoFtof header file                     -----
// -----               Created 30.03.09 by A. Sanchez                -----
// -------------------------------------------------------------------------

#ifndef PNDGEOFTOF_H
#define PNDGEOFTOF_H

#include "FairGeoSet.h"

class PndGeoFtof : public FairGeoSet {
 protected:
  /*  char modName[20];  // name of module */
  /*   char eleName[20];  // substring for elements in module */
  char modName[4]; // name of module
  char eleName[4]; // substring for elements in module

 public:
  PndGeoFtof();
  ~PndGeoFtof() {}
  //  const char* getModuleName(Int_t);
  //  const char* getEleName(Int_t);

  ClassDef(PndGeoFtof, 0) // Class for Ftof
};

#endif /* !PNDGEOFTOF_H */
