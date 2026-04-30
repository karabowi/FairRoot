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

#include "BSEmcFwEndcapDigiPar.h"

#include "FairParamList.h"

ClassImp(BSEmcFwEndcapDigiPar)

  const std::string BSEmcFwEndcapDigiPar::fgParameterName = "EmcFWEndcapDigiPar";

BSEmcFwEndcapDigiPar::BSEmcFwEndcapDigiPar(const char *t_name, const char *t_title, const char *t_context)
  : FairParGenericSet(t_name, t_title, t_context), fNBits(0), fPulseshapeTau(0.), fPulseshapeN(0.), fEnergyRangeHigh(0.), fEnergyRangeLow(0.), fNoiseWidthHigh(0.),
    fNoiseWidthLow(0.)
{
  clear();
}

void BSEmcFwEndcapDigiPar::putParams(FairParamList *t_list)
{
  if (t_list == nullptr) {
    return;
  }

  t_list->add("TimeBeforeHit", fTimeBeforeHit);
  t_list->add("TimeAfterHit", fTimeAfterHit);
  t_list->add("WfCutOffEnergy", fWfCutOffEnergy);
  t_list->add("SampleRate", fSampleRate);

  t_list->add("PsaParLow", fPsaParLow);
  t_list->add("PsaParHigh", fPsaParHigh);
  t_list->add("PsaTypeLow", fPsaTypeLow);
  t_list->add("PsaTypeHigh", fPsaTypeHigh);
  t_list->add("RValueParaLow", fRValueParaLow);
  t_list->add("RValueParaHigh", fRValueParaHigh);
  t_list->add("SignalOverflowHigh", fSignalOverflowHigh);

  t_list->add("Use_photon_statistic", fUse_photon_statistic);
  t_list->add("QuantumEfficiencyAPD", fQuantumEfficiencyAPD);
  t_list->add("ExcessNoiseFactorAPD", fExcessNoiseFactorAPD);
  t_list->add("DetectedPhotonsPerMeV", fDetectedPhotonsPerMeV);
  t_list->add("SensitiveAreaAPD", fSensitiveAreaAPD);
  t_list->add("RearCrystalSurfaceArea", fRearCrystalSurfaceArea);
  t_list->add("NBits", fNBits);
  t_list->add("PulseshapeTau", fPulseshapeTau);
  t_list->add("PulseshapeN", fPulseshapeN);
  t_list->add("Pulseshapev1", fPulseshapev1);
  t_list->add("Pulseshapetcr", fPulseshapetcr);
  t_list->add("Pulseshapetcf", fPulseshapetcf);
  t_list->add("Pulseshapet_r", fPulseshapet_r);
  t_list->add("PulseshapeRes", fPulseshapeRes);
  t_list->add("EnergyRangeHigh", fEnergyRangeHigh);
  t_list->add("EnergyRangeLow", fEnergyRangeLow);
  t_list->add("NoiseWidthLow", fNoiseWidthLow);
  t_list->add("NoiseWidthHigh", fNoiseWidthHigh);
  t_list->add("EnergyDigiThreshold", fEnergyDigiThreshold);
}

Bool_t BSEmcFwEndcapDigiPar::getParams(FairParamList *t_list)
{

  if (t_list == nullptr) {
    return kFALSE;
  }
  if (!t_list->fill("TimeBeforeHit", &fTimeBeforeHit)) {
    return kFALSE;
  }
  if (!t_list->fill("TimeAfterHit", &fTimeAfterHit)) {
    return kFALSE;
  }
  if (!t_list->fill("WfCutOffEnergy", &fWfCutOffEnergy)) {
    return kFALSE;
  }
  if (!t_list->fill("SampleRate", &fSampleRate)) {
    return kFALSE;
  }

  Text_t buf[512];
  if (!t_list->fill("PsaParLow", &fPsaParLow)) {
    return kFALSE;
  }
  if (!t_list->fill("PsaParHigh", &fPsaParHigh)) {
    return kFALSE;
  }
  if (!t_list->fill("PsaTypeLow", buf, 512)) {
    return kFALSE;
  }
  fPsaTypeLow = buf;
  if (!t_list->fill("PsaTypeHigh", buf, 512)) {
    return kFALSE;
  }
  fPsaTypeHigh = buf;
  if (!t_list->fill("RValueParaLow", buf, 512)) {
    return kFALSE;
  }
  fRValueParaLow = buf;
  if (!t_list->fill("RValueParaHigh", buf, 512)) {
    return kFALSE;
  }
  fRValueParaHigh = buf;
  if (!t_list->fill("SignalOverflowHigh", &fSignalOverflowHigh)) {
    return kFALSE;
  }

  /*other paras */
  if (!t_list->fill("Use_photon_statistic", &fUse_photon_statistic)) {
    return kFALSE;
  }
  if (!t_list->fill("QuantumEfficiencyAPD", &fQuantumEfficiencyAPD)) {
    return kFALSE;
  }
  if (!t_list->fill("ExcessNoiseFactorAPD", &fExcessNoiseFactorAPD)) {
    return kFALSE;
  }
  if (!t_list->fill("SensitiveAreaAPD", &fSensitiveAreaAPD)) {
    return kFALSE;
  }
  if (!t_list->fill("RearCrystalSurfaceArea", &fRearCrystalSurfaceArea)) {
    return kFALSE;
  }
  if (!t_list->fill("DetectedPhotonsPerMeV", &fDetectedPhotonsPerMeV)) {
    return kFALSE;
  }
  if (!t_list->fill("NBits", &fNBits)) {
    return kFALSE;
  }
  if (!t_list->fill("PulseshapeTau", &fPulseshapeTau)) {
    return kFALSE;
  }
  if (!t_list->fill("PulseshapeN", &fPulseshapeN)) {
    return kFALSE;
  }

  if (!t_list->fill("Pulseshapev1", &fPulseshapev1)) {
    return kFALSE;
  }
  if (!t_list->fill("Pulseshapetcr", &fPulseshapetcr)) {
    return kFALSE;
  }
  if (!t_list->fill("Pulseshapetcf", &fPulseshapetcf)) {
    return kFALSE;
  }
  if (!t_list->fill("Pulseshapet_r", &fPulseshapet_r)) {
    return kFALSE;
  }
  if (!t_list->fill("PulseshapeRes", &fPulseshapeRes)) {
    return kFALSE;
  }
  if (!t_list->fill("EnergyRangeHigh", &fEnergyRangeHigh)) {
    return kFALSE;
  }
  if (!t_list->fill("EnergyRangeLow", &fEnergyRangeLow)) {
    return kFALSE;
  }
  if (!t_list->fill("NoiseWidthLow", &fNoiseWidthLow)) {
    return kFALSE;
  }
  if (!t_list->fill("NoiseWidthHigh", &fNoiseWidthHigh)) {
    return kFALSE;
  }
  if (!t_list->fill("EnergyDigiThreshold", &fEnergyDigiThreshold)) {
    return kFALSE;
  }

  return kTRUE;
}
