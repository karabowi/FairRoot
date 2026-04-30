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

#include "BSEmcPSAFPGAPileupAnalyser.h"

#include <iostream>

#include "TF1.h"
#include "TString.h"

#include "BSEmcPSAFPGAFilterCF.h"
#include "BSEmcPSAFPGAFilterDelay.h"
#include "BSEmcPSAFPGAFilterMA.h"
#include "BSEmcPSAFPGAFilterMWD.h"
#include "BSEmcPSAFPGALinFitter.h"

BSEmcPSAFPGAPileupAnalyser::BSEmcPSAFPGAPileupAnalyser()
  : BSEmcPSAFPGASampleAnalyser(), fVerbose(0), fInt_thres(nullptr), fInt_mean(nullptr), fMWD_filter1(nullptr), fMA_filter1(nullptr), fMWD_filter2(nullptr), fFunc_defined(kFALSE)
{
}

BSEmcPSAFPGAPileupAnalyser::~BSEmcPSAFPGAPileupAnalyser()
{
  if (fInt_thres != nullptr) {
    delete fInt_thres;
  }
  if (fInt_mean != nullptr) {
    delete fInt_mean;
  }
  if (fMWD_filter1 != nullptr) {
    delete fMWD_filter1;
  }
  if (fMA_filter1 != nullptr) {
    delete fMA_filter1;
  }
  if (fMWD_filter2 != nullptr) {
    delete fMWD_filter2;
  }
}

void BSEmcPSAFPGAPileupAnalyser::reset()
{ // reset for every waveform to process
  fLocal_time = 0;
  fRough_pulse_timing = 0;
  fNumber_of_hits = 0;
  fSquare = 0;
  fT = 0;
  fMax = 0;

  fNumber_of_puls = 0;
  fNumber_of_sample = 0;
  fZ[fNumber_of_puls] = 1;
  fNumber_of_puls_mwd = 0;
  fNumber_of_sample_mwd = 0;
  fZ_mwd[fNumber_of_puls_mwd] = 1;
  fNumber_of_puls_mwd2 = 0;
  fNumber_of_sample_mwd2 = 0;
  fMax_mwd2[fNumber_of_puls_mwd2] = 0;
  fT_mwd = 0;
  fT_mwd2[fNumber_of_puls_mwd2] = 0;
  fZ_mwd2[fNumber_of_puls_mwd2] = 1;
  fSaved_local_time[fNumber_of_puls] = 0;
  fSaved_local_time_mwd[fNumber_of_puls_mwd] = 0;
  fSaved_local_time_mwd2[fNumber_of_puls_mwd2] = 0;
  fCF_filter->resetToZero();
  fCF_filter2->resetToZero();
  fMA_triggering->resetToZero();
  fMWD_filter1->resetToZero();
  fMWD_filter2->resetToZero();
  fMA_filter1->resetToZero();
  fCF_delay->resetToZero();
  fPulse_detected = kFALSE;
  fPulse_detected_mwd = kFALSE;
  fPulse_detected_mwd2 = kFALSE;
  fBaselineSum = 0;
  return;
}

void BSEmcPSAFPGAPileupAnalyser::InitParameters(const std::vector<Double_t> &t_params)
{

  // define parameter initialization, otherwise conflict with clock_unit and iafactor parameters, which have been added to BSEmcPSAFPGASampleAnalyser
  //
  const Int_t size = t_params.size();
  if (size != 14) {
    std::cerr << "-W BSEmcPSAFPGAPileupAnalyser::InitParameters Mismatch in number of parameters (" << size << "!=14). Filling missing parameter values up with zeros..."
              << std::endl;
  }

  SampleAnalyserParams newParams;
  newParams.ma_trig_M = (size > 0) ? (Int_t)t_params.at(0) : 0;
  newParams.hit_threshold = (size > 1) ? t_params.at(1) : 0;
  newParams.cf_delay = (size > 2) ? (Int_t)t_params.at(2) : 0;
  newParams.cf_ratio = (size > 3) ? t_params.at(3) : 0;
  // newParams.cf_ratio = 1 / 4;
  newParams.cf_fitter_length = (size > 4) ? (Int_t)t_params.at(4) : 0;
  newParams.cf_fit_offset = (size > 5) ? (Int_t)t_params.at(5) : 0;
  newParams.mwd_length = (size > 6) ? (Int_t)t_params.at(6) : 0;
  newParams.mwd_tau = (size > 7) ? t_params.at(7) : 0;
  // newParams.mwd_length = 32;
  // newParams.mwd_tau = 25;
  newParams.mwd2_length = 10;
  newParams.mwd2_tau = 24.5;
  // newParams.mwd2_length = (size>8) ? (Int_t) t_params.at(8): 0;
  // newParams.mwd2_tau = (size>9) ? t_params.at(9): 0;
  newParams.mwd3_length = (size > 10) ? (Int_t)t_params.at(10) : 0;
  newParams.mwd3_tau = (size > 11) ? t_params.at(11) : 0;
  newParams.sig_delay = (size > 12) ? t_params.at(12) : 0;
  newParams.rough_timing_corr = (size > 13) ? t_params.at(13) : 0;

  BSEmcPSAFPGASampleAnalyser::init(newParams);
}

void BSEmcPSAFPGAPileupAnalyser::Init(const std::vector<Double_t> &t_params, TF1 *t_r_thres, TF1 *t_r_mean, Float_t t_extBaselineValue)
{
  Init2(t_params, t_r_thres, t_r_mean);
  setBaseline(t_extBaselineValue);
}

void BSEmcPSAFPGAPileupAnalyser::Init(const std::vector<Double_t> &t_params, TF1 *t_r_thres, TF1 *t_r_mean, UInt_t t_baselineStartSample, UInt_t t_baselineStopSample)
{
  Init2(t_params, t_r_thres, t_r_mean);
  setBaselineWindow(t_baselineStartSample, t_baselineStopSample);
}

void BSEmcPSAFPGAPileupAnalyser::Init2(const std::vector<Double_t> &t_params, TF1 *t_r_thres, TF1 *t_r_mean)
{

  InitParameters(t_params);
  fSignal_delay->set(fAnalyserParams.sig_delay);

  if (fMWD_filter1 != nullptr) {
    delete fMWD_filter1;
  }
  fMWD_filter1 = new BSEmcPSAFPGAFilterMWD();
  if (fMA_filter1 != nullptr) {
    delete fMA_filter1;
  }
  fMA_filter1 = new BSEmcPSAFPGAFilterMA();
  if (fMWD_filter2 != nullptr) {
    delete fMWD_filter2;
  }
  fMWD_filter2 = new BSEmcPSAFPGAFilterMWD();

  fMWD_filter1->set(fAnalyserParams.mwd_tau, fAnalyserParams.mwd_length);
  fMA_filter1->set(fAnalyserParams.mwd_length / 2);
  fMWD_filter2->set(fAnalyserParams.mwd2_tau, fAnalyserParams.mwd2_length);

  if (t_r_mean != nullptr && t_r_thres != nullptr) {
    fFunc_defined = kTRUE;

    if (fInt_thres != nullptr) {
      delete fInt_thres;
    }
    fInt_thres = new TF1("intThres", "(" + t_r_thres->GetExpFormula() + ")*x", t_r_thres->GetXmin(), t_r_thres->GetXmax());
    fInt_thres->SetParameters(t_r_thres->GetParameters());
    fInt_thres->SetNpx(1000);

    if (fInt_mean != nullptr) {
      delete fInt_mean;
    }
    fInt_mean = new TF1("intMean", "(" + t_r_mean->GetExpFormula() + ")*x", t_r_mean->GetXmin(), t_r_mean->GetXmax());
    fInt_mean->SetParameters(t_r_mean->GetParameters());
    fInt_mean->SetNpx(1000);

  } else {
    std::cerr << "-W BSEmcPSAFPGAPileupAnalyser:Init2 no pileup separation function defined. All hits will be treated as single pulses" << std::endl;
  }
}

// TODO: Baseline follower (fBaselineMode = kFollow)

void BSEmcPSAFPGAPileupAnalyser::setBaseline(Float_t t_newBaseline)
{
  BSEmcPSAFPGASampleAnalyser::setBaseline(t_newBaseline);
  fBaselineMode = kExtern;
}

void BSEmcPSAFPGAPileupAnalyser::setBaselineWindow(UInt_t t_startSample, UInt_t t_stopSample)
{
  fBaselineStartSample = t_startSample;
  fBaselineStopSample = t_stopSample;
  fBaselineMode = kWindow;
}

void BSEmcPSAFPGAPileupAnalyser::GetHit(Int_t t_idx, Double_t &t_energy, Double_t &t_time)
{
  t_energy = sampleAmplitude(t_idx);
  t_time = sampleTime(t_idx);
}

// void BSEmcPSAFPGAPileupAnalyser::GetHit(Int_t t_idx, Double_t &t_energy, Double_t &t_time, pileup_t &t_pileupType)
// {
//   GetHit(t_idx, t_energy, t_time);
//   if (t_idx < nHits() && t_idx >= 0) {
//     t_pileupType = fPileups[t_idx];
//   } else {
//     t_pileupType = kInvalid;
//   }
// }

void BSEmcPSAFPGAPileupAnalyser::GetHit(Int_t t_idx, Double_t &t_energy, Double_t &t_time, Int_t &t_pileupType)
{
  GetHit(t_idx, t_energy, t_time);
  if (t_idx < nHits() && t_idx >= 0) {
    t_pileupType = fPileups[t_idx];
  } else {
    t_pileupType = kInvalid;
  }
}

void BSEmcPSAFPGAPileupAnalyser::GetEnergyMeasures(Int_t t_idx, Double_t &t_amplitude, Double_t &t_integral)
{
  t_amplitude = sampleAmplitude(t_idx);
  t_integral = sampleIntegral(t_idx);
}

void BSEmcPSAFPGAPileupAnalyser::put(Float_t t_valueToStore)
{

  if (fBaselineMode == kWindow && fLocal_time <= fBaselineStopSample) {
    if (fLocal_time >= fBaselineStartSample) {
      fBaselineSum += t_valueToStore;
    }
    if (fLocal_time == fBaselineStopSample) {
      fBaseline_value = fBaselineSum / (fBaselineStopSample - fBaselineStartSample + 1);
    }
  } else {
    Float_t signal = t_valueToStore - fBaseline_value;
    Float_t mwd_value = fMWD_filter1->put(signal);
    Float_t ma_value = fMA_filter1->put(mwd_value);
    Float_t mwd2_value = fMWD_filter2->put(signal);
    Float_t cf_value = fCF_filter->put(ma_value);
    Float_t cf_value2 = fCF_filter2->put(mwd2_value);
    Float_t cf_value_prev = fCF_prev_val;
    Float_t cf_value_prev2 = fCF_prev_val2;
    fStatus = kUndefined;

    //////...................fMWD2........................................////
    if (!fPulse_detected_mwd2) {
      if (mwd2_value > fAnalyserParams.hit_threshold) { // new pulse detected..reset pulse specific quantities
        fPulse_detected_mwd2 = kTRUE;
      } else {
        fSaved_local_time_mwd2[fNumber_of_puls_mwd2] = fLocal_time;
        fSav_mwd2 = mwd2_value;
      }
    }
    ///......................fMWD2........................
    if (fPulse_detected_mwd2) {
      if ((fLocal_time >= fAnalyserParams.cf_delay) && (fLocal_time < 5 * fAnalyserParams.cf_delay + fSaved_local_time_mwd2[fNumber_of_puls_mwd2])) {
        if ((cf_value_prev2 < 0.0) && (cf_value2 >= 0.0)) { // new zero crossing
          fCF_Fitter->reset();
          fCF_Fitter->putPoint(fLocal_time - 1, cf_value_prev2);
          fCF_Fitter->putPoint(fLocal_time, cf_value2);
          fCF_Fitter->fit();

          Double_t a = fCF_Fitter->offset();
          Double_t k = fCF_Fitter->slope();

          if (k > 0) {
            fCfZero2[fNumber_of_puls_mwd2] = -a / k;
          }
        }
      }
      if (mwd2_value < fAnalyserParams.hit_threshold / 4) {
        fMax_mwd2[fNumber_of_puls_mwd2] = 0;
        fPulse_detected_mwd2 = kFALSE;
        for (Int_t i = 0; i < fZ_mwd2[fNumber_of_puls_mwd2]; i++) {
          if (fMax_mwd2[fNumber_of_puls_mwd2] < fSignal_buffer_mwd2[i][fNumber_of_puls_mwd2]) {
            fMax_mwd2[fNumber_of_puls_mwd2] = fSignal_buffer_mwd2[i][fNumber_of_puls_mwd2];
          }
          fSignal_buffer_mwd2[i][fNumber_of_puls_mwd2] = 0;
        }
        fT_mwd2[fNumber_of_puls_mwd2] = fCfZero2[fNumber_of_puls_mwd2];
        if (fT_mwd2[fNumber_of_puls_mwd2] > 0) {
          fNumber_of_puls_mwd2++;
        }
        fZ_mwd2[fNumber_of_puls_mwd2] = 1;
      }

      fSignal_buffer_mwd2[0][fNumber_of_puls_mwd2] = fSav_mwd2;
      ////...............fMWD2 Waveform..........................
      if (mwd2_value > fAnalyserParams.hit_threshold / 4) {
        fSignal_buffer_mwd2[fZ_mwd2[fNumber_of_puls_mwd2]][fNumber_of_puls_mwd2] = mwd2_value;
        fZ_mwd2[fNumber_of_puls_mwd2]++;
      }
    }
    //////////////MWD2END/////////////////////
    //#ifdef KILLMWD
    //////...................fMWD........................................////
    if (!fPulse_detected_mwd) {
      if (ma_value > fAnalyserParams.hit_threshold) { // new pulse detected..reset pulse specific quantities
        fEnergy_finished = kFALSE;
        fTiming_finished = kFALSE;
        fPulse_detected_mwd = kTRUE;
        fPileups[fNumber_of_puls_mwd] = 0;
      } else {
        fSaved_local_time_mwd[fNumber_of_puls_mwd] = fLocal_time;
        fSav_mwd = ma_value;
        fStatus = kBaseline;
      }
    }
    ///......................fMWD........................
    if (fPulse_detected_mwd) {
      fStatus = kPulseDetected;
      if (((fLocal_time >= fAnalyserParams.cf_delay) && (fLocal_time < 5 * fAnalyserParams.cf_delay + fSaved_local_time_mwd[fNumber_of_puls_mwd])) && (!fTiming_finished)) {
        if ((cf_value_prev < 0.0) && (cf_value >= 0.0)) { // new zero crossing
          fCF_Fitter->reset();
          fCF_Fitter->putPoint(fLocal_time - 1, cf_value_prev);
          fCF_Fitter->putPoint(fLocal_time, cf_value);
          fCF_Fitter->fit();

          Double_t a = fCF_Fitter->offset();
          Double_t k = fCF_Fitter->slope();

          if (k > 0) {
            //	fTiming_finished = kTRUE;
            fCfZero[fNumber_of_puls_mwd] = -a / k;
          }
        }
      }
      if (ma_value < fAnalyserParams.hit_threshold / 4) {

        fMax_mwd = 0;
        fPulse_detected_mwd = kFALSE;
        fT_mwd = 0;
        Float_t Square_mwd = 0;
        for (Int_t i = 0; i < fZ_mwd[fNumber_of_puls_mwd]; i++) {
          Square_mwd = fSignal_buffer_mwd[i][fNumber_of_puls_mwd] + Square_mwd;

          if (fMax_mwd < fSignal_buffer_mwd[i][fNumber_of_puls_mwd]) {
            fMax_mwd = fSignal_buffer_mwd[i][fNumber_of_puls_mwd];
          }
        }
        fR_mwd = Square_mwd / fMax_mwd;
        fT_mwd = fCfZero[fNumber_of_puls_mwd];

        for (Int_t i = 0; i < fZ_mwd[fNumber_of_puls_mwd]; i++) {
          fSignal_buffer_mwd[i][fNumber_of_puls_mwd] = 0;
        }
        if (fT_mwd > 0) {

          if (fR_mwd > 24) {

            for (Int_t m = fNumber_of_puls_mwd; m < fNumber_of_puls_mwd2; m++) {
              /*fAmplitude*/ fAmplitude[m] = fMax_mwd2[m] * 0.903;
              /*fTime*/ fTime[m] = fT_mwd2[m];
              /*Pile-up index*/ fPileups[m] = 2;
              fStatus = kPulseFinished;
              fNumber_of_puls_mwd++;
            }
            fEnergy_finished = kTRUE;
            fTiming_finished = kTRUE;
          } else {
            /*INTEGRAL*/ fIntegral[fNumber_of_puls_mwd] = Square_mwd;
            /*fAmplitude*/ fAmplitude[fNumber_of_puls_mwd] = fMax_mwd * 0.903;
            /*fTime*/ fTime[fNumber_of_puls_mwd] = fT_mwd;
            /*Pile-up index*/ fPileups[fNumber_of_puls_mwd] = 1;
            fTiming_finished = kTRUE;
            fEnergy_finished = kTRUE;
            fNumber_of_puls_mwd++;
            fNumber_of_puls_mwd2 = fNumber_of_puls_mwd;
          }
        }

        fZ_mwd[fNumber_of_puls_mwd] = 1;
      }
      fSignal_buffer_mwd[0][fNumber_of_puls_mwd] = fSav_mwd;
      ////...............fMWD Waveform..........................
      if (ma_value > fAnalyserParams.hit_threshold / 4) {
        fSignal_buffer_mwd[fZ_mwd[fNumber_of_puls_mwd]][fNumber_of_puls_mwd] = ma_value;

        fZ_mwd[fNumber_of_puls_mwd]++;
      }
    }

    if (fEnergy_finished && fTiming_finished) {
      fNumber_of_hits = fNumber_of_puls_mwd;
      /*Just for print */ fIntegral[fNumber_of_puls_mwd] = fSquare;
      /*Just for print */ fAmplitude[fNumber_of_puls_mwd] = fMax_mwd;
      /*Just for print */ fTime[fNumber_of_puls_mwd] = fT_mwd;
      fEnergy_finished = kFALSE;
      fStatus = kPulseFinished;
    }
    fCF_prev_val = cf_value;
    fCF_prev_val2 = cf_value2;
  }
  fLocal_time++;
}

ClassImp(BSEmcPSAFPGAPileupAnalyser)
