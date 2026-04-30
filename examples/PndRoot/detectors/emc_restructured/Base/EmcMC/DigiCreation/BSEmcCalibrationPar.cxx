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

#include "BSEmcCalibrationPar.h"

#include "FairParamList.h"
#include "fairlogger/Logger.h"

const std::string BSEmcCalibrationPar::fgParameterName{"EmcCalibrationPar"};

BSEmcCalibrationPar::BSEmcCalibrationPar(const char *t_name, const char *t_title, const char *t_context) : FairParGenericSet(t_name, t_title, t_context) {}

BSEmcCalibrationPar::~BSEmcCalibrationPar() {}

void BSEmcCalibrationPar::putParams(FairParamList *t_list)
{
  LOG(debug) << "BSEmcCalibrationPar::putParams(FairParamList* t_list)";
  if (t_list == nullptr) {
    LOG(warn) << "BSEmcCalibrationPar::putParams(FairParamList* t_list) returning early as list is null";
    return;
  }
  LOG(debug) << "BSEmcCalibrationPar::putParams(FairParamList* t_list) adding fRlations";

  t_list->add("LowGain", fLowGain);
  t_list->add("HighGain", fHighGain);
  t_list->add("Default", fDefault);
}

Bool_t BSEmcCalibrationPar::getParams(FairParamList *t_list)
{
  LOG(debug) << "BSEmcCalibrationPar::getParams(FairParamList *t_list)";

  if (t_list == nullptr) {
    return kFALSE;
  }
  if (!t_list->fill("LowGain", &fLowGain)) {
    return kFALSE;
  }
  if (!t_list->fill("HighGain", &fHighGain)) {
    return kFALSE;
  }
  if (!t_list->fill("Default", &fDefault)) {
    return kFALSE;
  }
  return kTRUE;
}
