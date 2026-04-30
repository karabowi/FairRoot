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

//*-- AUTHOR : Denis Bertini
//*-- Created : 21/06/2005
#include "PndGeoPassivePar.h"
#include "FairParamList.h"
#include <iostream>
#include <iomanip>
#include "TObjArray.h"
using namespace std;
ClassImp(PndGeoPassivePar)

  PndGeoPassivePar::PndGeoPassivePar(const char *name, const char *title, const char *context)
  : FairParGenericSet(name, title, context), fGeoSensNodes(), fGeoPassNodes()
{

  fGeoSensNodes = new TObjArray();
  fGeoPassNodes = new TObjArray();
}

PndGeoPassivePar::~PndGeoPassivePar(void) {}

void PndGeoPassivePar::clear(void)
{
  if (fGeoSensNodes)
    delete fGeoSensNodes;
  if (fGeoPassNodes)
    delete fGeoPassNodes;
}

void PndGeoPassivePar::putParams(FairParamList *l)
{
  if (!l)
    return;
  l->addObject("FairGeoNodes Sensitive List", fGeoSensNodes);
  l->addObject("FairGeoNodes Passive List", fGeoPassNodes);
}

Bool_t PndGeoPassivePar::getParams(FairParamList *l)
{
  if (!l)
    return kFALSE;
  if (!l->fillObject("FairGeoNodes Sensitive List", fGeoSensNodes))
    return kFALSE;
  if (!l->fillObject("FairGeoNodes Passive List", fGeoPassNodes))
    return kFALSE;

  return kTRUE;
}
