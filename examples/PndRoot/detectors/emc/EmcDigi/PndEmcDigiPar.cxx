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
// PndEmcDigiPar
//
// Container class for Digitisation parameters
// class is inherited from FairParGenericSet
//
/////////////////////////////////////////////////////////////

#include "PndEmcDigiPar.h"
#include <iostream>

ClassImp(PndEmcDigiPar)

  PndEmcDigiPar::PndEmcDigiPar(const char *name, const char *title, const char *context)
  : FairParGenericSet(name, title, context), fEnergyHitThreshold(0), fDetectedPhotonsPerMeV(0), fDetectedPhotonsPerMeV_PMT(0), fSensitiveAreaAPD(0), fSensitiveAreaVPT(0),
    fQuantumEfficiencyAPD(0), fQuantumEfficiencyVPT(0), fQuantumEfficiencyPMT(0), fExcessNoiseFactorAPD(0), fExcessNoiseFactorVPT(0), fExcessNoiseFactorPMT(0),
    fIncoherent_elec_noise_width_GeV_APD(0), fIncoherent_elec_noise_width_GeV_VPT(0), fEnergyRange(0), fEnergyRangeBW(0), fFirstSamplePhase(0), fASIC_Shaping_int_time(0),
    fPMT_Shaping_int_time(0), fPMT_Shaping_diff_time(0), fCrystal_time_constant(0), fShashlyk_time_constant(0), fShashlykSamplingFactor(0), fSampleRate(0), fSampleRate_PMT(0),
    fEnergyDigiThreshold(0), fNBits(0), fNumber_of_samples_in_waveform(0), fNumber_of_samples_in_waveform_pmt(0), fUse_shaped_noise(0), fUse_photon_statistic(0),
    fUseDigiEffectiveSmearing(0), fNoiseAllChannels(0), fUse_nonuniformity(0)
{
  clear(); // WHY?
}

void PndEmcDigiPar::putParams(FairParamList *list)
{
  if (!list)
    return;
  list->add("EnergyHitThreshold", fEnergyHitThreshold);
  list->add("DetectedPhotonsPerMeV", fDetectedPhotonsPerMeV);
  list->add("DetectedPhotonsPerMeV_PMT", fDetectedPhotonsPerMeV_PMT);
  list->add("SensitiveAreaAPD", fSensitiveAreaAPD);
  list->add("SensitiveAreaVPT", fSensitiveAreaVPT);
  list->add("QuantumEfficiencyAPD", fQuantumEfficiencyAPD);
  list->add("QuantumEfficiencyVPT", fQuantumEfficiencyVPT);
  list->add("QuantumEfficiencyPMT", fQuantumEfficiencyPMT);
  list->add("ExcessNoiseFactorAPD", fExcessNoiseFactorAPD);
  list->add("ExcessNoiseFactorVPT", fExcessNoiseFactorVPT);
  list->add("ExcessNoiseFactorPMT", fExcessNoiseFactorPMT);
  list->add("Incoherent_elec_noise_width_GeV_APD", fIncoherent_elec_noise_width_GeV_APD);
  list->add("Incoherent_elec_noise_width_GeV_VPT", fIncoherent_elec_noise_width_GeV_VPT);
  list->add("EnergyRange", fEnergyRange);
  list->add("EnergyRangeBW", fEnergyRangeBW);
  list->add("FirstSamplePhase", fFirstSamplePhase);
  list->add("ASIC_Shaping_int_time", fASIC_Shaping_int_time);
  list->add("PMT_Shaping_int_time", fPMT_Shaping_int_time);
  list->add("PMT_Shaping_diff_time", fPMT_Shaping_diff_time);
  list->add("FWD_Shaping_int_time", fFWD_Shaping_int_time);
  list->add("FWD_time_constant", fFWD_time_constant);
  list->add("Crystal_time_constant", fCrystal_time_constant);
  list->add("Shashlyk_time_constant", fShashlyk_time_constant);
  list->add("ShashlykSamplingFactor", fShashlykSamplingFactor);
  list->add("SampleRate", fSampleRate);
  list->add("SampleRate_PMT", fSampleRate_PMT);
  list->add("SampleRate_FWD", fSampleRate_FWD);
  list->add("EnergyDigiThreshold", fEnergyDigiThreshold);
  list->add("NBits", fNBits);
  list->add("Number_of_samples_in_waveform", fNumber_of_samples_in_waveform);
  list->add("Number_of_samples_in_waveform_pmt", fNumber_of_samples_in_waveform_pmt);
  list->add("Number_of_samples_in_waveform_fwd", fNumber_of_samples_in_waveform_fwd);
  list->add("Use_shaped_noise", fUse_shaped_noise);
  list->add("Use_photon_statistic", fUse_photon_statistic);
  list->add("UseDigiEffectiveSmearing", fUseDigiEffectiveSmearing);
  list->add("NoiseAllChannels", fNoiseAllChannels);
  list->add("Use_nonuniformity", fUse_nonuniformity);
}

Bool_t PndEmcDigiPar::getParams(FairParamList *list)
{
  if (!list)
    return kFALSE;
  if (!list->fill("EnergyHitThreshold", &fEnergyHitThreshold))
    return kFALSE;
  if (!list->fill("DetectedPhotonsPerMeV", &fDetectedPhotonsPerMeV))
    return kFALSE;
  if (!list->fill("DetectedPhotonsPerMeV_PMT", &fDetectedPhotonsPerMeV_PMT))
    return kFALSE;
  if (!list->fill("SensitiveAreaAPD", &fSensitiveAreaAPD))
    return kFALSE;
  if (!list->fill("SensitiveAreaVPT", &fSensitiveAreaVPT))
    return kFALSE;
  if (!list->fill("QuantumEfficiencyAPD", &fQuantumEfficiencyAPD))
    return kFALSE;
  if (!list->fill("QuantumEfficiencyVPT", &fQuantumEfficiencyVPT))
    return kFALSE;
  if (!list->fill("QuantumEfficiencyPMT", &fQuantumEfficiencyPMT))
    return kFALSE;
  if (!list->fill("ExcessNoiseFactorAPD", &fExcessNoiseFactorAPD))
    return kFALSE;
  if (!list->fill("ExcessNoiseFactorVPT", &fExcessNoiseFactorVPT))
    return kFALSE;
  if (!list->fill("ExcessNoiseFactorPMT", &fExcessNoiseFactorPMT))
    return kFALSE;
  if (!list->fill("Incoherent_elec_noise_width_GeV_APD", &fIncoherent_elec_noise_width_GeV_APD))
    return kFALSE;
  if (!list->fill("Incoherent_elec_noise_width_GeV_VPT", &fIncoherent_elec_noise_width_GeV_VPT))
    return kFALSE;
  if (!list->fill("EnergyRange", &fEnergyRange))
    return kFALSE;
  if (!list->fill("EnergyRangeBW", &fEnergyRangeBW))
    return kFALSE;
  if (!list->fill("FirstSamplePhase", &fFirstSamplePhase))
    return kFALSE;
  if (!list->fill("ASIC_Shaping_int_time", &fASIC_Shaping_int_time))
    return kFALSE;
  if (!list->fill("PMT_Shaping_int_time", &fPMT_Shaping_int_time))
    return kFALSE;
  if (!list->fill("PMT_Shaping_diff_time", &fPMT_Shaping_diff_time))
    return kFALSE;
  if (!list->fill("FWD_Shaping_int_time", &fFWD_Shaping_int_time))
    return kFALSE;
  if (!list->fill("FWD_time_constant", &fFWD_time_constant))
    return kFALSE;
  if (!list->fill("Crystal_time_constant", &fCrystal_time_constant))
    return kFALSE;
  if (!list->fill("Shashlyk_time_constant", &fShashlyk_time_constant))
    return kFALSE;
  if (!list->fill("ShashlykSamplingFactor", &fShashlykSamplingFactor))
    return kFALSE;
  if (!list->fill("SampleRate", &fSampleRate))
    return kFALSE;
  if (!list->fill("SampleRate_PMT", &fSampleRate_PMT))
    return kFALSE;
  if (!list->fill("SampleRate_FWD", &fSampleRate_FWD))
    return kFALSE;
  if (!list->fill("EnergyDigiThreshold", &fEnergyDigiThreshold))
    return kFALSE;
  if (!list->fill("NBits", &fNBits))
    return kFALSE;
  if (!list->fill("Number_of_samples_in_waveform", &fNumber_of_samples_in_waveform))
    return kFALSE;
  if (!list->fill("Number_of_samples_in_waveform_pmt", &fNumber_of_samples_in_waveform_pmt))
    return kFALSE;
  if (!list->fill("Number_of_samples_in_waveform_fwd", &fNumber_of_samples_in_waveform_fwd))
    return kFALSE;
  if (!list->fill("Use_shaped_noise", &fUse_shaped_noise))
    return kFALSE;
  if (!list->fill("Use_photon_statistic", &fUse_photon_statistic))
    return kFALSE;
  if (!list->fill("UseDigiEffectiveSmearing", &fUseDigiEffectiveSmearing))
    return kFALSE;
  if (!list->fill("NoiseAllChannels", &fNoiseAllChannels))
    return kFALSE;
  if (!list->fill("Use_nonuniformity", &fUse_nonuniformity))
    return kFALSE;

  return kTRUE;
}
