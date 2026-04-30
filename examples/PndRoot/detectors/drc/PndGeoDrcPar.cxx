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

// --------------------------------------------------------------------------
// -----                   PndGeoDrcPar source file                     -----
// -----                Created 14/08/06  by A Cecchi                   -----
// --------------------------------------------------------------------------
#include <iostream>
#include <iomanip>

using namespace std;

#include "PndGeoDrcPar.h"
#include "FairParamList.h"

ClassImp(PndGeoDrcPar)

  PndGeoDrcPar::PndGeoDrcPar(const char *name, const char *title, const char *context)
  : FairParGenericSet(name, title, context)
{

  fGeoSensNodes = new TObjArray();
  fGeoPassNodes = new TObjArray();
}

PndGeoDrcPar::~PndGeoDrcPar(void) {}

void PndGeoDrcPar::clear(void)
{
  if (fGeoSensNodes)
    delete fGeoSensNodes;
  if (fGeoPassNodes)
    delete fGeoPassNodes;
}

void PndGeoDrcPar::putParams(FairParamList *l)
{
  if (!l)
    return;
  //   l->addObject("FairGeoNodes Sensitive List", fGeoSensNodes);
  //  l->addObject("FairGeoNodes Passive List", fGeoPassNodes);
}

Bool_t PndGeoDrcPar::getParams(FairParamList *l)
{
  if (!l)
    return kFALSE;
  //   if (!l->fillObject("FairGeoNodes Sensitive List", fGeoSensNodes)) return kFALSE;
  //   if (!l->fillObject("FairGeoNodes Passive List", fGeoPassNodes)) return kFALSE;

  return kTRUE;
}
