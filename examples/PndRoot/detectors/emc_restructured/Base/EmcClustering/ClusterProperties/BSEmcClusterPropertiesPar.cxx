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

#include "BSEmcClusterPropertiesPar.h"

#include "FairParamList.h"

ClassImp(BSEmcClusterPropertiesPar);
const std::string BSEmcClusterPropertiesPar::fgParameterName = "EmcClusterPropertiesPar";
BSEmcClusterPropertiesPar::BSEmcClusterPropertiesPar(const char *t_name, const char *t_title, const char *t_context) : FairParGenericSet(t_name, t_title, t_context) {}

BSEmcClusterPropertiesPar::~BSEmcClusterPropertiesPar(void) {}

void BSEmcClusterPropertiesPar::clear(void) {}

void BSEmcClusterPropertiesPar::putParams(FairParamList *t_list)
{
  if (t_list == nullptr) {
    return;
  }
  t_list->add("EmcClusterPosMethod", fEmcClusterPosMethod);
  t_list->add("OffsetParmA", fOffsetParmA);
  t_list->add("OffsetParmB", fOffsetParmB);
  t_list->add("OffsetParmC", fOffsetParmC);
}

Bool_t BSEmcClusterPropertiesPar::getParams(FairParamList *t_list)
{
  if (t_list == nullptr) {
    return kFALSE;
  }
  if (!t_list->fill("EmcClusterPosMethod", fEmcClusterPosMethod, 10)) {
    return kFALSE;
  }
  if (!t_list->fill("OffsetParmA", &fOffsetParmA)) {
    return kFALSE;
  }
  if (!t_list->fill("OffsetParmB", &fOffsetParmB)) {
    return kFALSE;
  }
  if (!t_list->fill("OffsetParmC", &fOffsetParmC)) {
    return kFALSE;
  }

  return kTRUE;
}
