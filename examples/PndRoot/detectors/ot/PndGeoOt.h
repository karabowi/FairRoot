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

////////////////////////////////////////////////////////////////////////////
// PndGeoOt header file
//
// Class for geometry of OT
//
// authors: Radoslaw Karabowicz, GSI, 2024
//
// modified from PndGeoFts by Nafija Ibrišimović in 2023
////////////////////////////////////////////////////////////////////////////

#ifndef PNDGEOOT_H
#define PNDGEOOT_H

// from FairRoot
#include <FairGeoSet.h>

class PndGeoOt : public FairGeoSet {
 protected:
  std::string modName; // name of module
  std::string eleName; // substring for elements in module
 public:
  PndGeoOt();
  ~PndGeoOt() {}
  const char *getModuleName(Int_t);
  const char *getEleName(Int_t);
  inline Int_t getModNumInMod(const TString &);
  ClassDef(PndGeoOt, 0) // Class for Ot
};

inline Int_t PndGeoOt::getModNumInMod(const TString &name)
{
  // returns the module index from module name

  return (Int_t)(name[2] - '0') - 1;
}

#endif /* !PNDGEOOT_H */
