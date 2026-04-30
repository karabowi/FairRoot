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

// -------------------------------------------------------------------------
// -----                PndGemGeoPar source file                       -----
// -----                Created 15/02/2009  by R.Karabowicz            -----
// -------------------------------------------------------------------------

/** \class PndGemGeoPar
 *  \author R. Karabowicz
 *  \date 15/02/2009
 *  \Storing list of GEM volumes
 **/

#include "PndGemGeoPar.h"
#include "FairParamList.h"
#include <iostream>
#include <iomanip>
#include "TObjArray.h"

ClassImp(PndGemGeoPar)

  PndGemGeoPar::PndGemGeoPar(const char *name, const char *title, const char *context)
  : FairParGenericSet(name, title, context)
{
  fGeoSensNodes = new TObjArray();
  fGeoPassNodes = new TObjArray();
}

PndGemGeoPar::~PndGemGeoPar(void) {}

void PndGemGeoPar::clear(void)
{
  if (fGeoSensNodes)
    delete fGeoSensNodes;
  if (fGeoPassNodes)
    delete fGeoPassNodes;
}

void PndGemGeoPar::putParams(FairParamList *l)
{
  if (!l)
    return;

  //   l->add("FairGeoNodes Sensitive List", fGeoSensNodes);
  //   l->add("FairGeoNodes Passive List", fGeoPassNodes);
  l->addObject("FairGeoNodes Sensitive List", fGeoSensNodes);
  l->addObject("FairGeoNodes Passive List", fGeoPassNodes);

}

Bool_t PndGemGeoPar::getParams(FairParamList *l)
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
