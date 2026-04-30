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

/////////////////////////////////////////////////////////////
// PndGeoHyp
//
// Class for geometry of EMC
//

//
/////////////////////////////////////////////////////////////

#include "PndGeoHyp.h"
#include "FairGeoNode.h"

ClassImp(PndGeoHyp)

  // -----   Default constructor   -------------------------------------------
  PndGeoHyp::PndGeoHyp()
{
  // Constructor
  fName = "stg";
  maxSectors = 0;
  maxModules = 1;
  strcpy(modName, "s");
  strcpy(eleName, "s");
}
// -------------------------------------------------------------------------
/*
const char* PndGeoHyp::getModuleName(Int_t m) {
  // Returns the module name of muo number m

  sprintf(modName,"stg0%i",m+1);
  return modName;
 cout << "MODNAME: " << modName << endl;
}

const char* PndGeoHyp::getEleName(Int_t m) {
  // Returns the element name of muo number m

  sprintf(eleName,"stglay%i",m+1);
  return eleName;
 cout << "ELENAME: " << eleName << endl;
}
*/
