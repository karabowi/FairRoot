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

#include "PndEmcShashlykDigiPar.h"


PndEmcShashlykDigiPar::PndEmcShashlykDigiPar(const char* name,const char* title,
                 const char* context)
: FairParGenericSet(name,title,context)
{
    fUse_photon_statistic = 0;
    fQuantumEfficiency = 0.;
    fExcessNoiseFactor = 0.;
    fDetectedPhotonsPerMeV = 0.;
	fSamplingFactor = 0.;

    fTimeBeforeHit = 0.;
    fTimeAfterHit = 0.;
    fActiveTimeIncrement = 0.;
    fWfCutOffEnergy = 0.;
    fSampleRate = 0.;
    fNBits = 0;
    fPulseshapeMu = 0.;
    fPulseshapeSigma = 0.;
    fNoiseWidth = 0.;

    fCF_fraction = 0.;
    fCF_tap = 0;
    //fCF_tcorr = NULL;
    fCF_nsamples = 0;
    //fOF_a = NULL;
    //fOF_b = NULL;
    fOF_m = 0;
    fOF_b0 = 0;
    fADCHitThreshold = 0.;
    fEnergyDigiThreshold = 0.;
}

void PndEmcShashlykDigiPar::putParams(FairParamList* list)
{
    if (!list) return;
    list->add("Use_photon_statistic",  fUse_photon_statistic);
    list->add("QuantumEfficiency",  fQuantumEfficiency);
    list->add("ExcessNoiseFactor",  fExcessNoiseFactor);
    list->add("DetectedPhotonsPerMeV", fDetectedPhotonsPerMeV);
	list->add("SamplingFactor", fSamplingFactor);

    list->add("TimeBeforeHit",         fTimeBeforeHit);
    list->add("TimeAfterHit",          fTimeAfterHit);
    list->add("ActiveTimeIncrement",   fActiveTimeIncrement);
    list->add("WfCutOffEnergy",        fWfCutOffEnergy);
    list->add("SampleRate",            fSampleRate);
    list->add("NBits",                 fNBits);
    list->add("EnCalib",               fEnCalib);
    list->add("PulseshapeMu",         fPulseshapeMu);
    list->add("PulseshapeSigma",           fPulseshapeSigma);
    list->add("NoiseWidth",            fNoiseWidth);

    list->add("CF_fraction", fCF_fraction);
    list->add("CF_tap", fCF_tap);
    list->add("CF_nsamples", fCF_nsamples);
    list->add("CF_tcorr", fCF_tcorr);
    list->add("OF_b0", fOF_b0);
    list->add("OF_m", fOF_m);
    list->add("OF_a", fOF_a);
    list->add("OF_b", fOF_b);
    list->add("ADCHitThreshold", fADCHitThreshold);
    list->add("EnergyDigiThreshold", fEnergyDigiThreshold);
}

Bool_t PndEmcShashlykDigiPar::getParams(FairParamList* list)
{
    if (!list) return kFALSE;
    if (!list->fill("Use_photon_statistic", &fUse_photon_statistic)) return kFALSE;
    if (!list->fill("QuantumEfficiency", &fQuantumEfficiency)) return kFALSE;
    if (!list->fill("ExcessNoiseFactor", &fExcessNoiseFactor)) return kFALSE;
    if (!list->fill("DetectedPhotonsPerMeV",&fDetectedPhotonsPerMeV)) return kFALSE;
	if (!list->fill("SamplingFactor", &fSamplingFactor)) return kFALSE;

    if (!list->fill("TimeBeforeHit",        &fTimeBeforeHit))        return kFALSE;
    if (!list->fill("TimeAfterHit",         &fTimeAfterHit))         return kFALSE;
    if (!list->fill("ActiveTimeIncrement",  &fActiveTimeIncrement))  return kFALSE;
    if (!list->fill("WfCutOffEnergy",       &fWfCutOffEnergy))       return kFALSE;
    if (!list->fill("SampleRate",           &fSampleRate))           return kFALSE;
    if (!list->fill("NBits",                &fNBits))                return kFALSE;
    if (!list->fill("EnCalib",              &fEnCalib))              return kFALSE;
    if (!list->fill("PulseshapeMu",        &fPulseshapeMu))        return kFALSE;
    if (!list->fill("PulseshapeSigma",          &fPulseshapeSigma))          return kFALSE;
    if (!list->fill("NoiseWidth",           &fNoiseWidth))           return kFALSE;
    
    if (!list->fill("CF_fraction", &fCF_fraction)) return kFALSE;
    if (!list->fill("CF_tap", &fCF_tap)) return kFALSE;
    if (!list->fill("CF_nsamples", &fCF_nsamples)) return kFALSE;
    if (!list->fill("CF_tcorr", &fCF_tcorr)) return kFALSE;
    if (!list->fill("OF_b0", &fOF_b0)) return kFALSE;
    if (!list->fill("OF_m", &fOF_m)) return kFALSE;
    if (!list->fill("OF_a", &fOF_a)) return kFALSE;
    if (!list->fill("OF_b", &fOF_b)) return kFALSE;
    if (!list->fill("ADCHitThreshold", &fADCHitThreshold)) return kFALSE;
    if (!list->fill("EnergyDigiThreshold", &fEnergyDigiThreshold)) return kFALSE;

    return kTRUE;
}


ClassImp(PndEmcShashlykDigiPar)
