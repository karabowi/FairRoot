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
// PndGeoPipe
//
// Class for geometry of support structure
//
/////////////////////////////////////////////////////////////

#include "PndGeoPipe.h"
#include "FairGeoLoader.h"
#include "FairGeoInterface.h"

ClassImp(PndGeoPipe)

  PndGeoPipe::PndGeoPipe()
{
  // Constructor
  fName = "pipe";
  strcpy(modName, "p");
  strcpy(eleName, "p");
  maxSectors = 0;
  maxModules = 1;
}
Bool_t PndGeoPipe::create(FairGeoBuilder *build)
{
  Bool_t rc = FairGeoSet::create(build);
  if (rc) {
    FairGeoLoader *loader = FairGeoLoader::Instance();
    FairGeoInterface *GeoInterface = loader->getGeoInterface();

    GeoInterface->getMasterNodes()->Add((TObject *)getVolume("pipeCentral"));
  }
  return rc;
}

PndGeoPipe::~PndGeoPipe() {}
