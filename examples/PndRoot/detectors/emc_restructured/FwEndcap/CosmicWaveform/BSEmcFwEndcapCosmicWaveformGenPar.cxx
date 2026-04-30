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

#include "BSEmcFwEndcapCosmicWaveformGenPar.h"

#include "FairParamList.h"

ClassImp(BSEmcFwEndcapCosmicWaveformGenPar)

  const std::string BSEmcFwEndcapCosmicWaveformGenPar::fgParameterName = "EmcFwEndcapCosmicWaveformGenPar";

BSEmcFwEndcapCosmicWaveformGenPar::BSEmcFwEndcapCosmicWaveformGenPar(const char *t_name, const char *t_title, const char *t_context) : FairParGenericSet(t_name, t_title, t_context)
{
  clear();
}

void BSEmcFwEndcapCosmicWaveformGenPar::putParams(FairParamList *t_list)
{
  if (t_list == nullptr) {
    return;
  }

  t_list->add("TimeBeforeHit", fTimeBeforeHit);
  t_list->add("TimeAfterHit", fTimeAfterHit);
  t_list->add("SampleRate", fSampleRate);
  t_list->add("WfCutOffEnergy", fWfCutOffEnergy);
  t_list->add("NBits", fNBits);

  t_list->add("PulseRisetime", fPulseRisetime);
  t_list->add("PulseAmplitudeConversion", fPulseAmplitudeConversion);
  t_list->add("PulseFastDecaytime", fPulseFastDecaytime);

  t_list->add("EnergyRangeLow", fEnergyRangeLow);
  t_list->add("EnergyRangeHigh", fEnergyRangeHigh);
  t_list->add("NoiseWidthLow", fNoiseWidthLow);
  t_list->add("NoiseWidthHigh", fNoiseWidthHigh);

  t_list->add("BaselineMeanLow", fBaselineMeanLow);
  t_list->add("BaselineMeanHigh", fBaselineMeanHigh);
  t_list->add("BaselineSigmaLow", fBaselineSigmaLow);
  t_list->add("BaselineSigmaHigh", fBaselineSigmaHigh);
}

Bool_t BSEmcFwEndcapCosmicWaveformGenPar::getParams(FairParamList *t_list)
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
  if (!t_list->fill("SampleRate", &fSampleRate)) {
    return kFALSE;
  }
  if (!t_list->fill("WfCutOffEnergy", &fWfCutOffEnergy)) {
    return kFALSE;
  }

  if (!t_list->fill("NBits", &fNBits)) {
    return kFALSE;
  }

  if (!t_list->fill("PulseRisetime", &fPulseRisetime)) {
    return kFALSE;
  }
  if (!t_list->fill("PulseAmplitudeConversion", &fPulseAmplitudeConversion)) {
    return kFALSE;
  }
  if (!t_list->fill("PulseFastDecaytime", &fPulseFastDecaytime)) {
    return kFALSE;
  }

  if (!t_list->fill("EnergyRangeLow", &fEnergyRangeLow)) {
    return kFALSE;
  }
  if (!t_list->fill("EnergyRangeHigh", &fEnergyRangeHigh)) {
    return kFALSE;
  }

  if (!t_list->fill("NoiseWidthLow", &fNoiseWidthLow)) {
    return kFALSE;
  }
  if (!t_list->fill("NoiseWidthHigh", &fNoiseWidthHigh)) {
    return kFALSE;
  }

  if (!t_list->fill("BaselineMeanLow", &fBaselineMeanLow)) {
    return kFALSE;
  }
  if (!t_list->fill("BaselineMeanHigh", &fBaselineMeanHigh)) {
    return kFALSE;
  }
  if (!t_list->fill("BaselineSigmaLow", &fBaselineSigmaLow)) {
    return kFALSE;
  }
  if (!t_list->fill("BaselineSigmaHigh", &fBaselineSigmaHigh)) {
    return kFALSE;
  }

  return kTRUE;
}
