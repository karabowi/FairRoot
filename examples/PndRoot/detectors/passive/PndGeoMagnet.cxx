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
// CbmGeoMagnet
//
// Class for the geometry of detector part Magnet
//
/////////////////////////////////////////////////////////////

#include "PndGeoMagnet.h"

ClassImp(PndGeoMagnet)

  PndGeoMagnet::PndGeoMagnet()
{
  // Constructor
  fName = "magnet";
  maxSectors = 0;
  maxModules = 1;
  strcpy(modName, "m");
  strcpy(eleName, "m");
}
