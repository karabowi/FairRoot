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


#include "BSEmcDigiPar.h"

#include "FairParamList.h"

ClassImp(BSEmcDigiPar);

const std::string BSEmcDigiPar::fgParameterName = "EmcDigiPar";
BSEmcDigiPar::BSEmcDigiPar(const char *name, const char *title, const char *context) : FairParGenericSet(name, title, context) {}

BSEmcDigiPar::~BSEmcDigiPar(void) {}

void BSEmcDigiPar::clear(void) {}

void BSEmcDigiPar::putParams(FairParamList *t_list)
{
  if (t_list == nullptr) {
    return;
  }
  t_list->add("EnergyHitThreshold", fEnergyHitThreshold);
  t_list->add("DetectedPhotonsPerMeV", fDetectedPhotonsPerMeV);
  t_list->add("SensitiveArea", fSensitiveArea);
  t_list->add("QuantumEfficiency", fQuantumEfficiency);
  t_list->add("ExcessNoiseFactor", fExcessNoiseFactor);
  t_list->add("Incoherent_elec_noise_width_GeV", fIncoherentElecNoiseWidthGeV);
  t_list->add("EnergyDigiThreshold", fEnergyDigiThreshold);
  t_list->add("UseDigiEffectiveSmearing", fUseDigiEffectiveSmearing);
  t_list->add("RearCrystalSurfaceArea", fRearCrystalSurfaceArea);
  t_list->add("OverlapTime", fOverlapTime);
}

Bool_t BSEmcDigiPar::getParams(FairParamList *t_list)
{
  if (t_list == nullptr) {
    return kFALSE;
  }
  if (!t_list->fill("EnergyHitThreshold", &fEnergyHitThreshold)) {
    return kFALSE;
  }
  if (!t_list->fill("DetectedPhotonsPerMeV", &fDetectedPhotonsPerMeV)) {
    return kFALSE;
  }
  if (!t_list->fill("SensitiveArea", &fSensitiveArea)) {
    return kFALSE;
  }
  if (!t_list->fill("QuantumEfficiency", &fQuantumEfficiency)) {
    return kFALSE;
  }
  if (!t_list->fill("ExcessNoiseFactor", &fExcessNoiseFactor)) {
    return kFALSE;
  }
  if (!t_list->fill("Incoherent_elec_noise_width_GeV", &fIncoherentElecNoiseWidthGeV)) {
    return kFALSE;
  }
  if (!t_list->fill("EnergyDigiThreshold", &fEnergyDigiThreshold)) {
    return kFALSE;
  }
  if (!t_list->fill("UseDigiEffectiveSmearing", &fUseDigiEffectiveSmearing)) {
    return kFALSE;
  }
  if (!t_list->fill("RearCrystalSurfaceArea", &fRearCrystalSurfaceArea)) {
    return kFALSE;
  }
  if (!t_list->fill("OverlapTime", &fOverlapTime)) {
    return kFALSE;
  }
  return kTRUE;
}
