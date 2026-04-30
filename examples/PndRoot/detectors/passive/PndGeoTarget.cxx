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

//*-- AUTHOR : Ilse Koenig
//*-- Created : 10/11/2003

/////////////////////////////////////////////////////////////
// PndGeoTarget
//
// Class for geometry of Target
//
/////////////////////////////////////////////////////////////

#include "PndGeoTarget.h"

ClassImp(PndGeoTarget)

  PndGeoTarget::PndGeoTarget()
{
  // Constructor
  fName = "target";
  strcpy(modName, "t");
  strcpy(eleName, "t");
  maxSectors = 0;
  maxModules = 1;
}
