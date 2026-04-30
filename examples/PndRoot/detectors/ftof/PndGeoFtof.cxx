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
// CbmGeoFtof
//
// Class for geometry of FTOF
//

//
/////////////////////////////////////////////////////////////

#include "PndGeoFtof.h"
#include "FairGeoNode.h"

ClassImp(PndGeoFtof)

  // -----   Default constructor   -------------------------------------------
  PndGeoFtof::PndGeoFtof()
{
  // Constructor

  fName = "ft";
  maxSectors = 0;
  maxModules = 1;
  strcpy(modName, "ft");
  strcpy(eleName, "ft");
}
// -------------------------------------------------------------------------
/*
const char* PndGeoFtof::getModuleName(Int_t m) {
  // Returns the module name of muo number m

  sprintf(modName,"stg0%i",m+1);
  return modName;
 cout << "MODNAME: " << modName << endl;
}

const char* PndGeoFtof::getEleName(Int_t m) {
  // Returns the element name of muo number m

  sprintf(eleName,"stglay%i",m+1);
  return eleName;
 cout << "ELENAME: " << eleName << endl;
}
*/
