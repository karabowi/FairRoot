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

#include "PndRichGeoPar.h"

#include "FairParamList.h"

#include "TObjArray.h"

#include <iostream>

ClassImp(PndRichGeoPar)

  PndRichGeoPar ::PndRichGeoPar(const char *name, const char *title, const char *context)
  : FairParGenericSet(name, title, context), fGeoSensNodes(new TObjArray()), fGeoPassNodes(new TObjArray())
{
}

PndRichGeoPar::~PndRichGeoPar(void) {}

void PndRichGeoPar::clear(void)
{
  if (fGeoSensNodes) {
    delete fGeoSensNodes;
  }
  if (fGeoPassNodes) {
    delete fGeoPassNodes;
  }
}

void PndRichGeoPar::putParams(FairParamList *l)
{
  if (!l) {
    return;
  }
  l->addObject("FairGeoNodes Sensitive List", fGeoSensNodes);
  l->addObject("FairGeoNodes Passive List", fGeoPassNodes);
}

Bool_t PndRichGeoPar::getParams(FairParamList *l)
{
  if (!l) {
    return kFALSE;
  }
  if (!l->fillObject("FairGeoNodes Sensitive List", fGeoSensNodes)) {
    return kFALSE;
  }
  if (!l->fillObject("FairGeoNodes Passive List", fGeoPassNodes)) {
    return kFALSE;
  }
  return kTRUE;
}
