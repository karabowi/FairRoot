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

#include "BSEmcPositionPar.h"

#include "FairParamList.h"

const std::string BSEmcPositionPar::fgParameterName = "EmcPositionPar";

BSEmcPositionPar::BSEmcPositionPar(const char *t_name, const char *t_title, const char *t_context) : FairParGenericSet(t_name, t_title, t_context) {}

BSEmcPositionPar::~BSEmcPositionPar(void){};

void BSEmcPositionPar::putParams(FairParamList *t_list) /*override*/
{
  if (t_list == nullptr) {
    return;
  }
  t_list->add("EmcDigiPositionDepth", fEmcDigiPositionDepth);
  t_list->add("DigiPosMethod", fDigiPosMethod);
}

Bool_t BSEmcPositionPar::getParams(FairParamList *t_list) /*override*/
{
  if (t_list == nullptr) {
    return kFALSE;
  }
  if (!t_list->fill("EmcDigiPositionDepth", &fEmcDigiPositionDepth)) {
    return kFALSE;
  }
  if (!t_list->fill("DigiPosMethod", fDigiPosMethod, 100)) {
    return kFALSE;
  }

  return kTRUE;
}
