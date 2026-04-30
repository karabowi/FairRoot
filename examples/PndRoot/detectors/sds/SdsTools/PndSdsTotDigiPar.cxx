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
#include "PndSdsTotDigiPar.h"

PndSdsTotDigiPar::PndSdsTotDigiPar(const char *name, const char *title, const char *context)
  : FairParGenericSet(name, title, context), fChargingTime(0.), fConstCurrent(0.), fClockFrequency(0.)
{
  clear();
}

void PndSdsTotDigiPar::putParams(FairParamList *list)
{
  if (!list)
    return;
  list->add("charge_time", fChargingTime);
  list->add("const_current", fConstCurrent);
  list->add("clock_frequency", fClockFrequency);
}

Bool_t PndSdsTotDigiPar::getParams(FairParamList *list)
{
  if (!list)
    return kFALSE;
  if (!list->fill("charge_time", &fChargingTime))
    return kFALSE;
  if (!list->fill("const_current", &fConstCurrent))
    return kFALSE;
  if (!list->fill("clock_frequency", &fClockFrequency))
    return kFALSE;
  return kTRUE;
}

ClassImp(PndSdsTotDigiPar);

void PndSdsTotDigiPar::Print(std::ostream &out) const
{
  out << "SDS Mvd Tot Digitization Parameters:" << std::endl;
  out << "   charge time (ns)            = " << fChargingTime << std::endl;
  out << "   constant current (e/ns)         = " << fConstCurrent << std::endl;
  out << "   clock frequency (MHz)        = " << fClockFrequency << std::endl;
}
