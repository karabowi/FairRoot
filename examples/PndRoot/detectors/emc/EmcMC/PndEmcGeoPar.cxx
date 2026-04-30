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

//*-- AUTHOR  :
//*-- Created : 20/05/2009

#include "PndEmcMapper.h"
#include "PndEmcGeoPar.h"
#include "FairParamList.h"
#include <iostream>
#include <iomanip>

ClassImp(PndEmcGeoPar)

  PndEmcGeoPar::PndEmcGeoPar(const char *name, const char *title, const char *context)
  : FairParGenericSet(name, title, context), fMapperVersion(0), fGeometryVersion(0)
{
}

PndEmcGeoPar::~PndEmcGeoPar(void) {}

void PndEmcGeoPar::clear(void) {}

void PndEmcGeoPar::putParams(FairParamList *l)
{
  if (!l)
    return;
  l->add("PndEmcMapperVersion", fMapperVersion);
  l->add("PndEmcGeometryVersion", fGeometryVersion);
}

Bool_t PndEmcGeoPar::getParams(FairParamList *l)
{
  if (!l)
    return kFALSE;
  if (!l->fill("PndEmcMapperVersion", &fMapperVersion))
    return kFALSE; // AB
  if (!l->fill("PndEmcGeometryVersion", &fGeometryVersion))
    return kFALSE;

  return kTRUE;
}

void PndEmcGeoPar::InitEmcMapper()
{
  PndEmcMapper::Init(fMapperVersion);
}
