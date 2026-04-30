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

//#pragma once
#ifndef BSEMCBWENDCAPNOISEADDER_HH
#define BSEMCBWENDCAPNOISEADDER_HH

#include "Rtypes.h"
#include "RtypesCore.h"
#include "TString.h"

#include "BSEmcAbsWaveformModifier.h"

class BSEmcWaveform;
class TVirtualFFT;
class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @brief waveform modifier to add noise to waveform
 * @author Guang Zhao (zhaog@ihep.ac.cn)
 * @ingroup BwEndcap
 */
class BSEmcBwEndcapNoiseAdder : public BSEmcAbsWaveformModifier {
 public:
  BSEmcBwEndcapNoiseAdder();
  virtual ~BSEmcBwEndcapNoiseAdder();
  BSEmcBwEndcapNoiseAdder(Int_t t_mode, Double_t t_sampling_rate, const Double_t *t_ps_freq, const Double_t *t_ps_power, Int_t t_ps_size, Double_t t_adc_noise, Double_t t_fe_noise,
                          Double_t t_sigma_ps, Double_t t_sigma_ges, Int_t t_seed, Int_t t_verbose); // mode: 0 - full, 1 - reduced, 2 - last

  virtual void Modify(BSEmcWaveform *t_wf) /*override*/;

 private:
  Int_t fTraceLength{128};
  Int_t fBufferPosition{0};
  Int_t fWaveformLength{0};
  Int_t fPowerSpecSample{0};
  Double_t fSamplingRate{0};
  Double_t fAdcNoise{0};
  Double_t fFENoise{0};
  Double_t fSigmaPs{0};
  Double_t fSigmaGes{0};
  Double_t fInputScale{0};
  const Double_t *fPSFreq{nullptr};
  const Double_t *fPSPower{nullptr};
  TVirtualFFT *fft{nullptr};
  Int_t fVerbose{0};

  Int_t fBufferSize{0};
  Int_t fPSNoiseMode{1}; // 0 for precision; 1 for speed; 2 for last
  Double_t fBufferUseRate{0};
  Bool_t fBufferIsEmpty{kTRUE};
  Double_t *fPSNoiseBuffer{nullptr};
  Double_t *fBandNoiseBuffer{nullptr};
  Double_t *fFFTFreqBuffer{nullptr};
  Double_t *fFFTReCoeffBuffer{nullptr};
  Double_t *fFFTImCoeffBuffer{nullptr};

  void expand_buffer(Int_t t_wf_size);

  void get_ps_noise(BSEmcWaveform *t_wf);
  void generate_ps_noise();
  void generate_band_noise(Double_t t_min_freq, Double_t t_max_freq);
  void get_rnd_noise(BSEmcWaveform *t_wf, Double_t t_noise);

  ClassDef(BSEmcBwEndcapNoiseAdder, 1)
};

#endif /*BSEMCBWENDCAPNOISEADDER_HH*/
