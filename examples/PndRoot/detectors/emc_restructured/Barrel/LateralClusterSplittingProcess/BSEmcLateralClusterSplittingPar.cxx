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

#include "BSEmcLateralClusterSplittingPar.h"

#include "FairParamList.h"

ClassImp(BSEmcLateralClusterSplittingPar);
const std::string BSEmcLateralClusterSplittingPar::fgParameterName = "EmcLateralClusterSplittingPar";
BSEmcLateralClusterSplittingPar::BSEmcLateralClusterSplittingPar(const char *t_name, const char *t_title, const char *t_context) : FairParGenericSet(t_name, t_title, t_context) {}

BSEmcLateralClusterSplittingPar::~BSEmcLateralClusterSplittingPar(void) {}

void BSEmcLateralClusterSplittingPar::clear(void) {}

void BSEmcLateralClusterSplittingPar::putParams(FairParamList *t_list)
{
  if (t_list == nullptr) {
    return;
  }
  t_list->add("ParArray1", fParArray1);
  t_list->add("ParArray2", fParArray2);
  t_list->add("ParArray3", fParArray3);
  t_list->add("ParArray4", fParArray4);
}

Bool_t BSEmcLateralClusterSplittingPar::getParams(FairParamList *t_list)
{
  if (t_list == nullptr) {
    return kFALSE;
  }
  if (!t_list->fill("ParArray1", &fParArray1)) {
    return kFALSE;
  }
  if (!t_list->fill("ParArray2", &fParArray2)) {
    return kFALSE;
  }
  if (!t_list->fill("ParArray3", &fParArray3)) {
    return kFALSE;
  }
  if (!t_list->fill("ParArray4", &fParArray4)) {
    return kFALSE;
  }

  return kTRUE;
}
