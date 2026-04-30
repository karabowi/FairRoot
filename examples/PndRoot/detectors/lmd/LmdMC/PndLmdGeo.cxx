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

/*
 * PndLmdGeo.cxx *
 *  Created on: Jun 30, 2010
 *      Author: huagen
 */

#include "PndLmdGeo.h"
#include "PndSdsGeo.h"
#include "FairLogger.h"

#include <iostream>

ClassImp(PndLmdGeo)

  // constructor
  PndLmdGeo::PndLmdGeo()
{
  fName = "lmd";
  maxSectors = 99;
  maxModules = 99;
}

PndLmdGeo::~PndLmdGeo() {}

const char *PndLmdGeo::getModuleName(Int_t m)
{
  //  PndSdsGeo::getModuleName(m);
  if (m < 0) {
    LOG(error) << " CbmGeoSts::getModuleName:: Module number " << m << " not known!";
    return "";
  }
  if (m < 9)
    sprintf(fModName, "lmdTrapStrip0%i", m + 1);
  else
    sprintf(fModName, "lmdTrapStrip%i", m + 1);
  return fModName;
}
const char *PndLmdGeo::getEleName(Int_t m)
{
  // PndSdsGeo::getEleName(m);
  // Returns the element name of Lmd number m
  if (m < 9)
    sprintf(fEleName, "lmd0%i", m + 1);
  else
    sprintf(fEleName, "lmd%i", m + 1);

  return fEleName;
}
