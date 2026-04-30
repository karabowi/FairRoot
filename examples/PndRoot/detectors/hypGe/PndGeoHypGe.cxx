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

#include "PndGeoHypGe.h"
#include "FairGeoNode.h"

ClassImp(PndGeoHypGe)

  // -----   Default constructor   -------------------------------------------
  PndGeoHypGe::PndGeoHypGe()
{
  // Constructor
  fName = "Ge_Crystal";
  maxSectors = 0;
  maxModules = 20;
}
// -------------------------------------------------------------------------

const char *PndGeoHypGe::getModuleName(Int_t m)
{
  // Returns the module name of muo number m

  sprintf(modName, "Ge_Crystal%i", m + 1);
  return modName;
}

const char *PndGeoHypGe::getEleName(Int_t m)
{
  // Returns the element name of muo number m

  sprintf(eleName, "Ge_Crystal%i", m + 1);
  return eleName;
}
