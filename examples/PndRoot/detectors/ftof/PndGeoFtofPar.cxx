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

/////////////////////////////////////////////////////////////
//
// PndGeoFTofPar
//
//
/////////////////////////////////////////////////////////////

#include "PndGeoFtofPar.h"
#include "FairParamList.h"
#include "iostream"
#include "iomanip"

ClassImp(PndGeoFtofPar)

  PndGeoFtofPar::PndGeoFtofPar(const char *name, const char *title, const char *context)
  : FairParGenericSet(name, title, context)
{

  fGeoSensNodes = new TObjArray();
  fGeoPassNodes = new TObjArray();
}

PndGeoFtofPar::~PndGeoFtofPar(void) {}

// probably the next funtions can be deleted

void PndGeoFtofPar::clear(void)
{
  if (fGeoSensNodes)
    delete fGeoSensNodes;
  if (fGeoPassNodes)
    delete fGeoPassNodes;
}

void PndGeoFtofPar::putParams(FairParamList *l)
{
  if (!l)
    return;
  // l->addObject("FairGeoNodes Sensitive List", fGeoSensNodes);
  //  l->addObject("FairGeoNodes Passive List", fGeoPassNodes);
}

Bool_t PndGeoFtofPar::getParams(FairParamList *l)
{
  if (!l)
    return kFALSE;
  //   if (!l->fillObject("FairGeoNodes Sensitive List", fGeoSensNodes)) return kFALSE;
  //   if (!l->fillObject("FairGeoNodes Passive List", fGeoPassNodes)) return kFALSE;

  return kTRUE;
}
