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

#include "BSEmcSplitOffPar.h"

#include "FairParamList.h"

ClassImp(BSEmcSplitOffPar);
const std::string BSEmcSplitOffPar::fgParameterName = "EmcSplitOffPar";
BSEmcSplitOffPar::BSEmcSplitOffPar(const char *t_name, const char *t_title, const char *t_context) : FairParGenericSet(t_name, t_title, t_context) {}

BSEmcSplitOffPar::~BSEmcSplitOffPar(void) {}

void BSEmcSplitOffPar::clear(void) {}

void BSEmcSplitOffPar::putParams(FairParamList *t_list)
{
  if (t_list == nullptr) {
    return;
  }

  t_list->add("1PEDFunction", f1PEDFunction);
  t_list->add("2PEDFunction", f2PEDFunction);
  t_list->add("HighPEDFunction", fHighPEDFunction);
}

Bool_t BSEmcSplitOffPar::getParams(FairParamList *t_list)
{
  if (t_list == nullptr) {
    return kFALSE;
  }

if (!t_list->fill("1PEDFunction", f1PEDFunction,30)) {
  return kFALSE;
}
if (!t_list->fill("2PEDFunction", f2PEDFunction,30)) {
  return kFALSE;
}
if (!t_list->fill("HighPEDFunction", fHighPEDFunction,30)) {
  return kFALSE;
}

  return kTRUE;
}
