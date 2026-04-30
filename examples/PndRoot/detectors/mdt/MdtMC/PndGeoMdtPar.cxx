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


#include <iostream>
#include <iomanip>

using namespace std;

#include "PndGeoMdtPar.h"
#include "FairParamList.h"

ClassImp(PndGeoMdtPar)

  PndGeoMdtPar::PndGeoMdtPar(const char *name, const char *title, const char *context)
  : FairParGenericSet(name, title, context)
{

  fSensNodes = new TObjArray();
  fPassNodes = new TObjArray();
}

PndGeoMdtPar::~PndGeoMdtPar(void) {}

void PndGeoMdtPar::clear(void)
{
  if (fSensNodes)
    delete fSensNodes;
  if (fPassNodes)
    delete fPassNodes;
}

void PndGeoMdtPar::putParams(FairParamList *l)
{
  if (!l)
    return;
  l->addObject("FairGeoNodes Sensitive List", fSensNodes);
  l->addObject("FairGeoNodes Passive List", fPassNodes);
}

Bool_t PndGeoMdtPar::getParams(FairParamList *l)
{
  if (!l)
    return kFALSE;
  if (!l->fillObject("FairGeoNodes Sensitive List", fSensNodes))
    return kFALSE;
  if (!l->fillObject("FairGeoNodes Passive List", fPassNodes))
    return kFALSE;

  return kTRUE;
}
