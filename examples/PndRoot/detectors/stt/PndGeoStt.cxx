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
// CbmGeoTof
//
// Class for geometry of STT
//
/////////////////////////////////////////////////////////////

#include "PndGeoStt.h"
#include "FairGeoNode.h"

ClassImp(PndGeoStt)

  PndGeoStt::PndGeoStt()
{
  // Constructor
  fName = "stt";
  maxSectors = 0;
  maxModules = 20; // stt1
}

const char *PndGeoStt::getModuleName(Int_t m)
{
  // Returns the module name of stt number m
  // sprintf(modName,"stt%i",m+1);
  if (m < 9)
    sprintf(modName, "stt0%i", m + 1);
  else
    sprintf(modName, "stt%i", m + 1);

  return modName;
}

const char *PndGeoStt::getEleName(Int_t m)
{
  // Returns the element name of sts number m
  // sprintf(eleName,"s%i",m+1);
  if (m < 9)
    sprintf(eleName, "stt0%i", m + 1);
  else
    sprintf(eleName, "stt%i", m + 1);
  return eleName;
}
