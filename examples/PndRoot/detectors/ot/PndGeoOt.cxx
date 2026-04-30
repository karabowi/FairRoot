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
// PndGeoOt source file
//
// Class for geometry of OT
//
// authors: Radoslaw Karabowicz, GSI, 2024
//
// modified from PndGeoFts by Nafija Ibrišimović in 2023
////////////////////////////////////////////////////////////////////////////

#include "PndGeoOt.h"
// from FairRoot
#include <FairGeoNode.h>
// from fmt
#include <fmt/core.h>

ClassImp(PndGeoOt)

  PndGeoOt::PndGeoOt()
{
  // Constructor
  fName = "ot";
  maxSectors = 0;
  maxModules = 40;
}

const char *PndGeoOt::getModuleName(Int_t m)
{
  // Returns the module name of ot number m
  modName = fmt::format("ot{:02d}", m + 1);

  return modName.c_str();
}

const char *PndGeoOt::getEleName(Int_t m)
{
  // Returns the element name of ot number m
  eleName = fmt::format("ot{:02d}", m + 1);

  return eleName.c_str();
}
