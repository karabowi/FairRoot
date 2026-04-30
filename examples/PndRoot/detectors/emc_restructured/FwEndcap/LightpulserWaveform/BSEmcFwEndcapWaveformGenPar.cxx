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

#include "BSEmcFwEndcapWaveformGenPar.h"

#include "FairParamList.h"

ClassImp(BSEmcFwEndcapWaveformGenPar)

  const std::string BSEmcFwEndcapWaveformGenPar::fgParameterName = "EmcFwEndcapWaveformGenPar";

BSEmcFwEndcapWaveformGenPar::BSEmcFwEndcapWaveformGenPar(const char *t_name, const char *t_title, const char *t_context)
  : FairParGenericSet(t_name, t_title, t_context), fNBits(0), fPulseshapeTau(0.), fPulseshapeN(0.), fEnergyRangeHigh(0.), fEnergyRangeLow(0.), fNoiseWidthHigh(0.),
    fNoiseWidthLow(0.)
{
  clear();
}

void BSEmcFwEndcapWaveformGenPar::putParams(FairParamList *t_list)
{
  if (t_list == nullptr) {
    return;
  }

  t_list->add("TimeBeforeHit", fTimeBeforeHit);
  t_list->add("TimeAfterHit", fTimeAfterHit);
  t_list->add("WfCutOffEnergy", fWfCutOffEnergy);
  t_list->add("SampleRate", fSampleRate);
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
  t_list->add("BaselineMeanLow", fBaselineMeanLow);
  t_list->add("BaselineMeanHigh", fBaselineMeanHigh);
  t_list->add("BaselineSigmaLow", fBaselineSigmaLow);
  t_list->add("BaselineSigmaHigh", fBaselineSigmaHigh);
}

Bool_t BSEmcFwEndcapWaveformGenPar::getParams(FairParamList *t_list)
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
