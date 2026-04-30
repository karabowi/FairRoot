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

#include "BSEmcBwEndcapNoiseAdder.h"

#include <math.h>
#include <vector>

#include "TRandom.h"
#include "TVirtualFFT.h"

#include "fairlogger/Logger.h"

#include "BSEmcWaveform.h"

#include "math.h"

BSEmcBwEndcapNoiseAdder::BSEmcBwEndcapNoiseAdder()
{
  fTraceLength = 0.;
  fPowerSpecSample = 0.;
  fSamplingRate = 0.;
  fAdcNoise = 0.;
  fFENoise = 0.;
  fSigmaPs = 0.;
  fSigmaGes = 0.;
  fTraceLength = 128;
  fPSNoiseBuffer = nullptr;
  fBandNoiseBuffer = nullptr;
  fFFTFreqBuffer = nullptr;
  fFFTReCoeffBuffer = nullptr;
  fFFTImCoeffBuffer = nullptr;
  fft = nullptr;
  fBufferIsEmpty = kTRUE;
  fBufferSize = 0;
  fBufferUseRate = 0.;
  fPSNoiseMode = 1;
  fVerbose = 0;
}

BSEmcBwEndcapNoiseAdder::BSEmcBwEndcapNoiseAdder(Int_t t_mode, Double_t t_sampling_rate, const Double_t *t_ps_freq, const Double_t *t_ps_power, Int_t t_ps_size,
                                                 Double_t t_adc_noise, Double_t t_fe_noise, Double_t t_sigma_ps, Double_t t_sigma_ges, Int_t /*t_seed*/, Int_t t_verbose)
{
  fPSNoiseMode = t_mode;
  fSamplingRate = t_sampling_rate * 1E3; // GHz->MHz
  fAdcNoise = t_adc_noise;
  fFENoise = t_fe_noise;
  fSigmaPs = t_sigma_ps;
  fSigmaGes = t_sigma_ges;
  fPSFreq = t_ps_freq;
  fPSPower = t_ps_power;
  fPowerSpecSample = 2 * t_ps_size + 1;
  fTraceLength = 1024;

  fBufferIsEmpty = kTRUE;
  fBufferSize = 16 * fTraceLength;
  fBufferUseRate = 0.;
  fBufferPosition = 0;
  fPSNoiseBuffer = new Double_t[fBufferSize];
  fBandNoiseBuffer = new Double_t[fTraceLength];
  fFFTFreqBuffer = new Double_t[fTraceLength];
  fFFTReCoeffBuffer = new Double_t[fTraceLength];
  fFFTImCoeffBuffer = new Double_t[fTraceLength];

  fft = TVirtualFFT::FFT(1, &fTraceLength, "C2CBACKWARD ES K");

  fVerbose = t_verbose;
}

BSEmcBwEndcapNoiseAdder::~BSEmcBwEndcapNoiseAdder()
{
  if (fPSNoiseBuffer != nullptr) {
    delete[] fPSNoiseBuffer;
  }
  if (fBandNoiseBuffer != nullptr) {
    delete[] fBandNoiseBuffer;
  }
  if (fFFTFreqBuffer != nullptr) {
    delete[] fFFTFreqBuffer;
  }
  if (fFFTReCoeffBuffer != nullptr) {
    delete[] fFFTReCoeffBuffer;
  }
  if (fFFTImCoeffBuffer != nullptr) {
    delete[] fFFTImCoeffBuffer;
  }
  if (fft != nullptr) {
    delete fft;
  }
}

void BSEmcBwEndcapNoiseAdder::Modify(BSEmcWaveform *t_wf)
{
  fWaveformLength = t_wf->GetWaveformLength();

  get_ps_noise(t_wf);
  get_rnd_noise(t_wf, fAdcNoise);
  get_rnd_noise(t_wf, fFENoise);
}

void BSEmcBwEndcapNoiseAdder::get_ps_noise(BSEmcWaveform *t_wf)
{
  if (fWaveformLength > fBufferSize) {
    expand_buffer(fWaveformLength);
  }

  switch (fPSNoiseMode) {
  case 0: // precision mode
    fBufferPosition += fWaveformLength;
    if (fBufferIsEmpty || fBufferPosition + fWaveformLength > fBufferSize) {
      generate_ps_noise();
      fBufferPosition = 0;
      fBufferIsEmpty = kFALSE;
    }
    break;

  case 1: // speed mode
    fBufferUseRate += (Double_t)fWaveformLength / fBufferSize;
    LOG(debug) << "use rate = " << fBufferUseRate;
    if (fBufferIsEmpty || fBufferUseRate > 1000.) {
      generate_ps_noise();
      fBufferUseRate = 0.;
      fBufferIsEmpty = kFALSE;
    }
    do {
      fBufferPosition = (Int_t)(gRandom->Rndm() * fBufferSize);
    } while (fBufferPosition + fWaveformLength > fBufferSize);
    break;

  default: return;
  }

  std::vector<Double_t>::iterator it;
  Int_t idx = 0;
  for (it = GetWaveformReference(t_wf).begin(); it != GetWaveformReference(t_wf).end(); ++it) {
    (*it) += fPSNoiseBuffer[fBufferPosition + idx];
    idx++;
  }

  if (fVerbose > 2) {
    LOG(info) << "[BSEmcBwEndcapNoiseAdder]:[ps noise] " << fWaveformLength;
    for (Int_t i = 0; i < fWaveformLength; i++) {
      LOG(info) << "[BSEmcBwEndcapNoiseAdder]:[ps noise] " << fPSNoiseBuffer[fBufferPosition + i];
    }
  }
}

void BSEmcBwEndcapNoiseAdder::generate_ps_noise()
{
  if (fVerbose > 2) {
    LOG(info) << "[BSEmcBwEndcapNoiseAdder]: generating ps noise";
  }
  Double_t power_gain = sqrt((Double_t)fTraceLength / fPowerSpecSample);
  Double_t fac = sqrt(fSigmaGes * fSigmaGes - fAdcNoise * fAdcNoise - fFENoise * fFENoise) / fSigmaPs;

  Int_t nTraces = fBufferSize / fTraceLength;

  for (Int_t i = 0; i < nTraces; i++) { // generate nTraces ps noises
    for (Int_t j = 0; j < fPowerSpecSample / 2 - 1; j++) {
      Double_t low = NAN, high = NAN, pwr = NAN;
      low = ((j == 0) ? 0 : fPSFreq[j - 1]);
      high = fPSFreq[j];
      pwr = fPSPower[j];
      Double_t power_factor = pow(10, (pwr / 20));

      generate_band_noise(low, high);
      for (Int_t k = 0; k < fTraceLength; k++) {
        if (j == 0) {
          fPSNoiseBuffer[k + i * fTraceLength] = 0.;
        }
        fPSNoiseBuffer[k + i * fTraceLength] += fBandNoiseBuffer[k] * power_factor * power_gain * fac;
      }
    }
  }
}

void BSEmcBwEndcapNoiseAdder::generate_band_noise(Double_t t_min_freq, Double_t t_max_freq)
{
  Int_t samples = fTraceLength;
  Double_t sample_rate = fSamplingRate;

  // calculate frequences for ifft
  if (fBufferIsEmpty) {
    for (Int_t i = 0; i < samples; i++) {
      if (i < (samples + 1) / 2) {
        fFFTFreqBuffer[i] = 1. / samples * sample_rate * i;
      } else {
        fFFTFreqBuffer[i] = 1. / samples * sample_rate * (samples - i);
      }
    }
  }

  // randomly generate phases
  Int_t np = (samples - 1) / 2;
  const Double_t pi = 3.1415927;
  Int_t non_zero_term = 0;
  for (Int_t i = 0; i < samples; i++) {
    if (fFFTFreqBuffer[i] >= t_min_freq && fFFTFreqBuffer[i] <= t_max_freq) {
      if (i >= 1 && i < np + 1) {
        Double_t phase = 2 * pi * gRandom->Rndm();
        fFFTReCoeffBuffer[i] = cos(phase);
        fFFTImCoeffBuffer[i] = sin(phase);
      } else if (i >= samples - np && i < samples) {
        fFFTReCoeffBuffer[i] = fFFTReCoeffBuffer[samples - i];
        fFFTImCoeffBuffer[i] = -fFFTImCoeffBuffer[samples - i];
      } else {
        fFFTReCoeffBuffer[i] = 1.;
        fFFTImCoeffBuffer[i] = 0.;
      }
      non_zero_term++;
    } else {
      fFFTReCoeffBuffer[i] = 0.;
      fFFTImCoeffBuffer[i] = 0.;
    }
  }

  if (non_zero_term == 0) { // skip fft if all coefficiences are 0
    for (Int_t i = 0; i < samples; i++) {
      fBandNoiseBuffer[i] = 0.;
    }
    return;
  }

  // inverse-fft
  fft->SetPointsComplex(fFFTReCoeffBuffer, fFFTImCoeffBuffer);
  fft->Transform();
  fft->GetPointsComplex(fFFTReCoeffBuffer, fFFTImCoeffBuffer);

  for (Int_t i = 0; i < samples; i++) {
    fBandNoiseBuffer[i] = fFFTReCoeffBuffer[i] / samples;
  }
}

void BSEmcBwEndcapNoiseAdder::get_rnd_noise(BSEmcWaveform *t_wf, Double_t t_noise_width)
{
  if (fVerbose > 2) {
    LOG(info) << "[BSEmcBwEndcapNoiseAdder]:[rnd noise] " << fWaveformLength << " " << t_noise_width;
  }

  std::vector<Double_t>::iterator it;
  Int_t i = 0;
  for (it = GetWaveformReference(t_wf).begin(); it != GetWaveformReference(t_wf).end(); ++it) {
    Double_t noise = gRandom->Gaus(0, t_noise_width);
    (*it) += noise;
    i++;

    if (fVerbose > 2) {
      LOG(info) << "[BSEmcBwEndcapNoiseAdder]:[rnd noise] " << noise;
    }
  }
}

void BSEmcBwEndcapNoiseAdder::expand_buffer(Int_t t_wf_size)
{
  if (fPSNoiseBuffer != nullptr) {
    delete[] fPSNoiseBuffer;
  }
  while (fBufferSize < t_wf_size) {
    fBufferSize *= 2;
  }
  fPSNoiseBuffer = new Double_t[fBufferSize];
  LOG(debug) << "BSEmcBwEndcapNoiseAdder::buffer expanded: " << fBufferSize;
}
