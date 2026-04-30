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

#include "PndHypDigiPar.h"

PndHypDigiPar::PndHypDigiPar(const char *name, const char *title, const char *context) : FairParGenericSet(name, title, context)
{
  clear();
}

void PndHypDigiPar::putParams(FairParamList *list)
{
  if (!list)
    return;
  list->add("dimX", dimX);
  list->add("dimY", dimY);
  // list->add("skew", skew);
  list->add("threshold", threshold);
  list->add("noise", noise);
  //   list->add("sensName", sensName);
  //   list->add("feName", feName);
}

Bool_t PndHypDigiPar::getParams(FairParamList *list)
{
  if (!list)
    return kFALSE;
  if (!list->fill("dimX", &dimX))
    return kFALSE;
  if (!list->fill("dimY", &dimY))
    return kFALSE;
  // if (!list->fill("skew",&skew)) return kFALSE;
  if (!list->fill("threshold", &threshold))
    return kFALSE;
  if (!list->fill("noise", &noise))
    return kFALSE;
  //   if (!list->fill("sensName",&sensName)) return kFALSE;
  //   if (!list->fill("feName",&feName)) return kFALSE;
  return kTRUE;
}

ClassImp(PndHypDigiPar);
