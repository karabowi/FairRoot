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

#include "BSEmcPSATmaxAnalyser.h"

#include <cmath>
#include <string.h>

#include "RtypesCore.h"

#include "fairlogger/Logger.h"

#include "BSEmcWaveform.h"

BSEmcPSATmaxAnalyser::BSEmcPSATmaxAnalyser(const Double_t *t_coeff, Int_t t_taps, Int_t t_gap, Int_t t_samplingrate, Double_t t_hit_thr, Double_t t_tut_peak, Double_t t_hit_val,
                                           Int_t t_verbose)
  : BSEmcAbsPSA(), fTaps(t_taps), fGap(t_gap), fHitThr(t_hit_thr), fTutPeak(t_tut_peak), fHitVal(t_hit_val), fFIRCoeff(t_coeff),
    fTimeStep(1. / t_samplingrate * 1e3), // time step in ns
    fVerbose(t_verbose)
{
}

BSEmcPSATmaxAnalyser::~BSEmcPSATmaxAnalyser() {}

void BSEmcPSATmaxAnalyser::Reset()
{
  fEnergyList.clear();
  fTimeList.clear();
}

Int_t BSEmcPSATmaxAnalyser::Process(const BSEmcWaveform *t_waveform)
{
  // convert vector to array
  Int_t size = t_waveform->GetWaveformLength();
  Double_t *input = new Double_t[size];
  Double_t time_stamp = t_waveform->GetTimeStamp();         // ns
  Double_t sample_rate = t_waveform->GetSampleRate() / 1E9; // GHz

  static Int_t iWf = 0;
  if (fVerbose > 2) {
    LOG(info) << "[BSEmcPSATmaxAnalyser]:[TMAX] "
              << "waveform_" << iWf << " " << size;
  }

  std::vector<Double_t> wf = t_waveform->GetSignal();
  for (Int_t i = 0; i < size; i++) {
    input[i] = wf[i];
    if (fVerbose > 2) {
      LOG(info) << "[BSEmcPSATmaxAnalyser]:[TMAX] " << time_stamp + (Double_t)i / sample_rate << " " << input[i];
    }
  }

  // FIR smoothing
  Double_t *signal = fir(input, size);
  if (fVerbose > 2) {
    LOG(info) << "[BSEmcPSATmaxAnalyser]:[TMAX] "
              << "waveform_fir_" << iWf << " " << size;
    for (Int_t i = 0; i < size; i++) {
      LOG(info) << "[BSEmcPSATmaxAnalyser]:[TMAX] " << time_stamp + (Double_t)i / sample_rate << " " << signal[i];
    }
  }

  // Derivative
  std::vector<Double_t> deri_n(size, 0.);
  Double_t deri_p = NAN;
  std::vector<Double_t> deri(size, 0.);
  std::vector<Double_t> deri_time(size, 0.);
  std::vector<Double_t> deri2_time(size, 0.);

  for (Int_t n = fTaps + fGap; n < size; n++) {
    deri_p = -signal[n - fGap] + signal[n];
    deri_time[n] = -signal[n] + signal[n - fGap];
    if (deri_time[n] < 0) {
      deri_n[n] = 0;
    } else {
      deri_n[n] = deri_time[n];
    }
    deri[n] = deri_p + deri_n[n];
  }

  // Second derivative
  for (Int_t n = fTaps + 2 * fGap; n < size; n++) {
    deri2_time[n] = -deri_time[n] + deri_time[n - fGap];
  }

  delete[] signal;

  if (fVerbose > 2) {
    LOG(info) << "[BSEmcPSATmaxAnalyser]:[TMAX] "
              << "waveform_D_" << iWf << " " << size;
    for (Int_t i = 0; i < size; i++) {
      LOG(info) << "[BSEmcPSATmaxAnalyser]:[TMAX] " << time_stamp + (Double_t)i / sample_rate << " " << -deri_time[i];
    }
    LOG(info) << "[BSEmcPSATmaxAnalyser]:[TMAX] "
              << "waveform_Dinv_" << iWf << " " << size;
    for (Int_t i = 0; i < size; i++) {
      LOG(info) << "[BSEmcPSATmaxAnalyser]:[TMAX] " << time_stamp + (Double_t)i / sample_rate << " " << deri[i];
    }
    LOG(info) << "[BSEmcPSATmaxAnalyser]:[TMAX] "
              << "waveform_D2_" << iWf << " " << size;
    for (Int_t i = 0; i < size; i++) {
      LOG(info) << "[BSEmcPSATmaxAnalyser]:[TMAX] " << time_stamp + (Double_t)i / sample_rate << " " << -deri2_time[i];
    }
  }

  // Integral
  Double_t sum = 0;
  Double_t temp = 0;
  Double_t d_last = 0;
  Double_t zp = 0;
  Double_t time = 0;
  Int_t tut_int = 0;
  Double_t hit_conv = 0;
  Bool_t hit = kFALSE;
  std::vector<Double_t> vamp;
  std::vector<Double_t> vtime;
  std::vector<Double_t> ds(size, 0.);
  std::vector<Double_t> hc(size, 0.);

  // Events
  for (Int_t n = 0; n < size; n++) {
    // Time extraction
    // if ((d_last < 0) && (deri_time[n] > 0)) {
    //	zp = -d_last / (deri_time[n] - d_last);
    //	time = n - 1 + zp;
    //}
    if (d_last <= 0 && deri2_time[n] > 0) {
      zp = -d_last / (deri2_time[n] - d_last);
      time = n - 1 + zp; // use 2nd derivative to extract time
    }
    d_last = deri_time[n];
    // Amplitude
    if ((sum + deri[n]) > temp) {
      tut_int += 1;
      sum += deri[n] / fGap;
      hit_conv = sum * hit_det(tut_int);
      if (hit_conv >= fHitThr) {
        hit = kTRUE;
      }
    }
    // Build event
    else {
      if (hit) {
        time = time_stamp + time / sample_rate;
        fEnergyList.push_back(temp);
        fTimeList.push_back(time);

        if (fVerbose > 2) {
          vamp.push_back(temp);
          vtime.push_back(time);
        }
      }
      sum = 0;
      tut_int = 0;
      hit_conv = 0;
      hit = kFALSE;
    }
    temp = sum;

    if (fVerbose > 2) {
      ds[n] = temp;
      hc[n] = hit_conv;
    }
  }

  delete[] input;

  if (fVerbose > 2) {
    LOG(info) << "[BSEmcPSATmaxAnalyser]:[TMAX] "
              << "waveform_Ds_" << iWf << " " << size;
    for (Int_t i = 0; i < size; i++) {
      LOG(info) << "[BSEmcPSATmaxAnalyser]:[TMAX] " << time_stamp + (Double_t)i / sample_rate << " " << ds[i];
    }
    LOG(info) << "[BSEmcPSATmaxAnalyser]:[TMAX] "
              << "waveform_hitConv_" << iWf << " " << size;
    for (Int_t i = 0; i < size; i++) {
      LOG(info) << "[BSEmcPSATmaxAnalyser]:[TMAX] " << time_stamp + (Double_t)i / sample_rate << " " << hc[i];
    }
    LOG(info) << "[BSEmcPSATmaxAnalyser]:[TMAX] "
              << "result " << vamp.size();
    for (UInt_t i = 0; i < vamp.size(); i++) {
      LOG(info) << "[BSEmcPSATmaxAnalyser]:[TMAX] " << vamp[i] << " " << vtime[i];
    }
    iWf++;
  }

  return fEnergyList.size();
}

void BSEmcPSATmaxAnalyser::GetHit(Int_t t_idx, Double_t &t_energy, Double_t &t_time)
{
  t_energy = fEnergyList[t_idx];
  t_time = fTimeList[t_idx];
}

//
// 20-coeff FIR filting to the raw waveform
//
Double_t *BSEmcPSATmaxAnalyser::fir(Double_t *t_input, Int_t t_size)
{

  Double_t *output = new Double_t[t_size];

  Double_t insamp[fTaps - 1 + t_size];
  memset(insamp, 0, sizeof(insamp));

  memcpy(&insamp[fTaps - 1], t_input, t_size * sizeof(Double_t));

  Double_t acc = NAN;
  const Double_t *coeffp = nullptr;
  Double_t *inputp = nullptr;

  for (Int_t n = 0; n < t_size; n++) {
    coeffp = fFIRCoeff;
    inputp = &insamp[fTaps - 1 + n];
    acc = 0;
    for (Int_t k = 0; k < fTaps; k++) {
      acc += (*coeffp++) * (*inputp--);
    }
    output[n] = acc;
  }

  memmove(&insamp[0], &insamp[t_size], (fTaps - 1) * sizeof(Double_t));

  return output;
}

Double_t BSEmcPSATmaxAnalyser::hit_det(Int_t t_tut_int)
{
  Double_t a = log(fHitVal) / fTutPeak;
  if (t_tut_int <= fTutPeak) {
    return exp(a * t_tut_int);
  } else {
    return fHitVal * exp(-a * (t_tut_int - fTutPeak));
  }
}

ClassImp(BSEmcPSATmaxAnalyser)
