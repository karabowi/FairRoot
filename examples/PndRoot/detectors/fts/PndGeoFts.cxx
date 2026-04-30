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
// PndGeoFts
//
// Class for geometry of FTS
//
/////////////////////////////////////////////////////////////

#include "PndGeoFts.h"
#include "FairGeoNode.h"

ClassImp(PndGeoFts)

  PndGeoFts::PndGeoFts()
{
  // Constructor
  fName = "fts";
  maxSectors = 0;
  maxModules = 40;
}

const char *PndGeoFts::getModuleName(Int_t m)
{
  // Returns the module name of fts number m

  if (m < 9)
    sprintf(modName, "fts0%i", m + 1);
  else
    sprintf(modName, "fts%i", m + 1);
  // std::cout << "MODNAME: " << modName << std::endl;

  return modName;
}

const char *PndGeoFts::getEleName(Int_t m)
{
  // Returns the element name of fts number m

  if (m < 9)
    sprintf(eleName, "fts0%i", m + 1);
  else
    sprintf(eleName, "fts%i", m + 1);
  // std::cout << "ELENAME: " << eleName << std::endl;

  return eleName;
}
