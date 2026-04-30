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

/////////////////////////////////////////////////////////////
// BSEmcBwDigiPar
//
// Container class for Digitisation parameters
// class is inherited from FairParGenericSet
//
/////////////////////////////////////////////////////////////

#include "BSEmcBwEndcapDigiPar.h"

#include "FairParamList.h"

ClassImp(BSEmcBwEndcapDigiPar) std::string BSEmcBwEndcapDigiPar::fgParameterName = "EmcBWEndcapDigiPar";
BSEmcBwEndcapDigiPar::BSEmcBwEndcapDigiPar(const char *t_name, const char *t_title, const char *t_context) : FairParGenericSet(t_name, t_title, t_context) {}

void BSEmcBwEndcapDigiPar::putParams(FairParamList *t_list)
{
  if (t_list == nullptr) {
    return;
  }
  t_list->add("Use_photon_statistic", fUse_photon_statistic);
  t_list->add("QuantumEfficiencyAPD", fQuantumEfficiencyAPD);
  t_list->add("ExcessNoiseFactorAPD", fExcessNoiseFactorAPD);
  t_list->add("DetectedPhotonsPerMeV", fDetectedPhotonsPerMeV);
  t_list->add("SensitiveAreaAPD", fSensitiveAreaAPD);
  t_list->add("RearCrystalSurfaceArea", fRearCrystalSurfaceArea);

  t_list->add("TimeBeforeHit", fTimeBeforeHit);
  t_list->add("TimeAfterHit", fTimeAfterHit);
  t_list->add("ActiveTimeIncrement", fActiveTimeIncrement);
  t_list->add("WfCutOffEnergy", fWfCutOffEnergy);
  t_list->add("SampleRate", fSampleRate);
  t_list->add("NBits", fNBits);
  t_list->add("EnCalib", fEnCalib);
  t_list->add("PulseshapeTau", fPulseshapeTau);
  t_list->add("PulseshapeN", fPulseshapeN);
  t_list->add("HGLGRatio", fHGLGRatio);
  t_list->add("ADCNoiseHigh", fADCNoiseHigh);
  t_list->add("ADCNoiseLow", fADCNoiseLow);
  t_list->add("FENoiseHigh", fFENoiseHigh);
  t_list->add("FENoiseLow", fFENoiseLow);
  t_list->add("PsSigmaHigh", fPsSigmaHigh);
  t_list->add("PsSigmaLow", fPsSigmaLow);
  t_list->add("GesSigmaHigh", fGesSigmaHigh);
  t_list->add("GesSigmaLow", fGesSigmaLow);
  t_list->add("PSFreq", fPSFreq);
  t_list->add("PSPowerHigh", fPSPowerHigh);
  t_list->add("PSPowerLow", fPSPowerLow);

  t_list->add("SignalOverflowHigh", fSignalOverflowHigh);
  t_list->add("FIRCoeff", fFIRCoeff);
  t_list->add("TmaxTaps", fTmaxTaps);
  t_list->add("TmaxGap", fTmaxGap);
  t_list->add("TmaxHitThresholdHigh", fTmaxHitThresholdHigh);
  t_list->add("TmaxHitThresholdLow", fTmaxHitThresholdLow);
  t_list->add("TmaxTutPeak", fTmaxTutPeak);
  t_list->add("TmaxHitVal", fTmaxHitVal);
  t_list->add("EnergyDigiThreshold", fEnergyDigiThreshold);
}

Bool_t BSEmcBwEndcapDigiPar::getParams(FairParamList *t_list)
{
  if (t_list == nullptr) {
    return kFALSE;
  }
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

  if (!t_list->fill("TimeBeforeHit", &fTimeBeforeHit)) {
    return kFALSE;
  }
  if (!t_list->fill("TimeAfterHit", &fTimeAfterHit)) {
    return kFALSE;
  }
  if (!t_list->fill("ActiveTimeIncrement", &fActiveTimeIncrement)) {
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
  if (!t_list->fill("EnCalib", &fEnCalib)) {
    return kFALSE;
  }
  if (!t_list->fill("PulseshapeTau", &fPulseshapeTau)) {
    return kFALSE;
  }
  if (!t_list->fill("PulseshapeN", &fPulseshapeN)) {
    return kFALSE;
  }
  if (!t_list->fill("HGLGRatio", &fHGLGRatio)) {
    return kFALSE;
  }
  if (!t_list->fill("ADCNoiseHigh", &fADCNoiseHigh)) {
    return kFALSE;
  }
  if (!t_list->fill("ADCNoiseLow", &fADCNoiseLow)) {
    return kFALSE;
  }
  if (!t_list->fill("FENoiseHigh", &fFENoiseHigh)) {
    return kFALSE;
  }
  if (!t_list->fill("FENoiseLow", &fFENoiseLow)) {
    return kFALSE;
  }
  if (!t_list->fill("PsSigmaHigh", &fPsSigmaHigh)) {
    return kFALSE;
  }
  if (!t_list->fill("PsSigmaLow", &fPsSigmaLow)) {
    return kFALSE;
  }
  if (!t_list->fill("GesSigmaHigh", &fGesSigmaHigh)) {
    return kFALSE;
  }
  if (!t_list->fill("GesSigmaLow", &fGesSigmaLow)) {
    return kFALSE;
  }
  if (!t_list->fill("PSFreq", &fPSFreq)) {
    return kFALSE;
  }
  if (!t_list->fill("PSPowerHigh", &fPSPowerHigh)) {
    return kFALSE;
  }
  if (!t_list->fill("PSPowerLow", &fPSPowerLow)) {
    return kFALSE;
  }

  if (!t_list->fill("SignalOverflowHigh", &fSignalOverflowHigh)) {
    return kFALSE;
  }
  if (!t_list->fill("FIRCoeff", &fFIRCoeff)) {
    return kFALSE;
  }
  if (!t_list->fill("TmaxTaps", &fTmaxTaps)) {
    return kFALSE;
  }
  if (!t_list->fill("TmaxGap", &fTmaxGap)) {
    return kFALSE;
  }
  if (!t_list->fill("TmaxHitThresholdHigh", &fTmaxHitThresholdHigh)) {
    return kFALSE;
  }
  if (!t_list->fill("TmaxHitThresholdLow", &fTmaxHitThresholdLow)) {
    return kFALSE;
  }
  if (!t_list->fill("TmaxTutPeak", &fTmaxTutPeak)) {
    return kFALSE;
  }
  if (!t_list->fill("TmaxHitVal", &fTmaxHitVal)) {
    return kFALSE;
  }
  if (!t_list->fill("EnergyDigiThreshold", &fEnergyDigiThreshold)) {
    return kFALSE;
  }

  return kTRUE;
}
