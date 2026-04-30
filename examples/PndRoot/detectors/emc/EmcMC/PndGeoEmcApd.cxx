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
// PndGeoEmcApd
//
// Class for geometry of EMCAPD
//
// Created 14/08/06  by S.Spataro
//
/////////////////////////////////////////////////////////////

#include "PndGeoEmcApd.h"
#include "FairGeoNode.h"

ClassImp(PndGeoEmcApd)

  // -----   Default constructor   -------------------------------------------
  PndGeoEmcApd::PndGeoEmcApd()
{
  // Constructor
  fName = "apd";
  maxSectors = 0;
  maxModules = 20;
}
// -------------------------------------------------------------------------

const char *PndGeoEmcApd::getModuleName(Int_t m)
{
  // Returns the module name of muo number m

  sprintf(modName, "apd%i", m + 1);
  return modName;
}

const char *PndGeoEmcApd::getEleName(Int_t m)
{
  // Returns the element name of muo number m

  sprintf(eleName, "apd%i", m + 1);
  return eleName;
}
