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
// PndEmcBWDigiPar
//
// Container class for Digitisation parameters
// class is inherited from FairParGenericSet
//
/////////////////////////////////////////////////////////////

#include "PndEmcBWEndcapDigiPar.h"


PndEmcBWEndcapDigiPar::PndEmcBWEndcapDigiPar(const char* name,const char* title,
			     const char* context)
: FairParGenericSet(name,title,context)
{
	fUse_photon_statistic = 0;
	fQuantumEfficiencyAPD = 0.;
	fExcessNoiseFactorAPD = 0.;
	fSensitiveAreaAPD = 0.;
	fDetectedPhotonsPerMeV = 0.;

	fTimeBeforeHit = 0.;
	fTimeAfterHit = 0.;
	fActiveTimeIncrement = 0.;
	fWfCutOffEnergy = 0.;
	fSampleRate = 0.;
	fNBits = 0;
	fPulseshapeTau = 0.;
	fPulseshapeN = 0.;
	fHGLGRatio = 0.;
	fADCNoiseHigh = 0.;
	fADCNoiseLow = 0.;
	fFENoiseHigh = 0.;
	fFENoiseLow = 0.;
	fPsSigmaHigh = 0.;
	fPsSigmaLow = 0.;
	fGesSigmaHigh = 0.;
	fGesSigmaLow = 0.;

	fEnCalib = 0.;
	fSignalOverflowHigh = 0.;
	fTmaxTaps = 0;
	fTmaxGap = 0;
	fTmaxHitThresholdHigh = 0.;
	fTmaxHitThresholdLow = 0.;
	fTmaxTutPeak = 0.;
	fTmaxHitVal = 0.;
}

void PndEmcBWEndcapDigiPar::putParams(FairParamList* list)
{
	if (!list) return;
	list->add("Use_photon_statistic",  fUse_photon_statistic);
	list->add("QuantumEfficiencyAPD",  fQuantumEfficiencyAPD);
	list->add("ExcessNoiseFactorAPD",  fExcessNoiseFactorAPD);
	list->add("DetectedPhotonsPerMeV", fDetectedPhotonsPerMeV);
	list->add("SensitiveAreaAPD",      fSensitiveAreaAPD);

	list->add("TimeBeforeHit",         fTimeBeforeHit);
	list->add("TimeAfterHit",          fTimeAfterHit);
	list->add("ActiveTimeIncrement",   fActiveTimeIncrement);
	list->add("WfCutOffEnergy",        fWfCutOffEnergy);
	list->add("SampleRate",            fSampleRate);
	list->add("NBits",                 fNBits);
	list->add("EnCalib",               fEnCalib);
	list->add("PulseshapeTau",         fPulseshapeTau);
	list->add("PulseshapeN",           fPulseshapeN);
	list->add("HGLGRatio",             fHGLGRatio);
	list->add("ADCNoiseHigh",          fADCNoiseHigh);
	list->add("ADCNoiseLow",           fADCNoiseLow);
	list->add("FENoiseHigh",           fFENoiseHigh);
	list->add("FENoiseLow",            fFENoiseLow);
	list->add("PsSigmaHigh",           fPsSigmaHigh);
	list->add("PsSigmaLow",            fPsSigmaLow);
	list->add("GesSigmaHigh",          fGesSigmaHigh);
	list->add("GesSigmaLow",           fGesSigmaLow);
	list->add("PSFreq",                fPSFreq);
	list->add("PSPowerHigh",           fPSPowerHigh);
	list->add("PSPowerLow",            fPSPowerLow);

	list->add("SignalOverflowHigh",    fSignalOverflowHigh);
	list->add("FIRCoeff",              fFIRCoeff);
	list->add("TmaxTaps",              fTmaxTaps);
	list->add("TmaxGap",               fTmaxGap);
	list->add("TmaxHitThresholdHigh",  fTmaxHitThresholdHigh);
	list->add("TmaxHitThresholdLow",   fTmaxHitThresholdLow);
	list->add("TmaxTutPeak",           fTmaxTutPeak);
	list->add("TmaxHitVal",            fTmaxHitVal);
	list->add("EnergyDigiThreshold",   fEnergyDigiThreshold);
}

Bool_t PndEmcBWEndcapDigiPar::getParams(FairParamList* list)
{
	if (!list) return kFALSE;
	if (!list->fill("Use_photon_statistic", &fUse_photon_statistic)) return kFALSE;
	if (!list->fill("QuantumEfficiencyAPD", &fQuantumEfficiencyAPD)) return kFALSE;
	if (!list->fill("ExcessNoiseFactorAPD", &fExcessNoiseFactorAPD)) return kFALSE;
	if (!list->fill("SensitiveAreaAPD",     &fSensitiveAreaAPD))     return kFALSE;
	if (!list->fill("DetectedPhotonsPerMeV",&fDetectedPhotonsPerMeV)) return kFALSE;

	if (!list->fill("TimeBeforeHit",        &fTimeBeforeHit))        return kFALSE;
	if (!list->fill("TimeAfterHit",         &fTimeAfterHit))         return kFALSE;
	if (!list->fill("ActiveTimeIncrement",  &fActiveTimeIncrement))  return kFALSE;
	if (!list->fill("WfCutOffEnergy",       &fWfCutOffEnergy))       return kFALSE;
	if (!list->fill("SampleRate",           &fSampleRate))           return kFALSE;
	if (!list->fill("NBits",                &fNBits))                return kFALSE;
	if (!list->fill("EnCalib",              &fEnCalib))              return kFALSE;
	if (!list->fill("PulseshapeTau",        &fPulseshapeTau))        return kFALSE;
	if (!list->fill("PulseshapeN",          &fPulseshapeN))          return kFALSE;
	if (!list->fill("HGLGRatio",            &fHGLGRatio))            return kFALSE;
	if (!list->fill("ADCNoiseHigh",         &fADCNoiseHigh))         return kFALSE;
	if (!list->fill("ADCNoiseLow",          &fADCNoiseLow))          return kFALSE;
	if (!list->fill("FENoiseHigh",          &fFENoiseHigh))          return kFALSE;
	if (!list->fill("FENoiseLow",           &fFENoiseLow))           return kFALSE;
	if (!list->fill("PsSigmaHigh",          &fPsSigmaHigh))          return kFALSE;
	if (!list->fill("PsSigmaLow",           &fPsSigmaLow))           return kFALSE;
	if (!list->fill("GesSigmaHigh",         &fGesSigmaHigh))         return kFALSE;
	if (!list->fill("GesSigmaLow",          &fGesSigmaLow))          return kFALSE;
	if (!list->fill("PSFreq",               &fPSFreq))               return kFALSE;
	if (!list->fill("PSPowerHigh",          &fPSPowerHigh))          return kFALSE;
	if (!list->fill("PSPowerLow",           &fPSPowerLow))           return kFALSE;

	if (!list->fill("SignalOverflowHigh",   &fSignalOverflowHigh))   return kFALSE;
	if (!list->fill("FIRCoeff",             &fFIRCoeff))             return kFALSE;
	if (!list->fill("TmaxTaps",             &fTmaxTaps))             return kFALSE;
	if (!list->fill("TmaxGap",              &fTmaxGap))              return kFALSE;
	if (!list->fill("TmaxHitThresholdHigh", &fTmaxHitThresholdHigh)) return kFALSE;
	if (!list->fill("TmaxHitThresholdLow",  &fTmaxHitThresholdLow))  return kFALSE;
	if (!list->fill("TmaxTutPeak",          &fTmaxTutPeak))          return kFALSE;
	if (!list->fill("TmaxHitVal",           &fTmaxHitVal))           return kFALSE;
	if (!list->fill("EnergyDigiThreshold",  &fEnergyDigiThreshold))  return kFALSE;
	
	return kTRUE;
}


ClassImp(PndEmcBWEndcapDigiPar)
