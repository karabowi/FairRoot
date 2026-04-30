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

#ifndef BSEMCPSAFPGASAMPLEANALYSER_HH
#define BSEMCPSAFPGASAMPLEANALYSER_HH

#include <string>
#include <vector>

#include "Rtypes.h"
#include "RtypesCore.h"

#include "BSEmcAbsPSA.h"

class BSEmcPSAFPGAFilterCF;
class BSEmcPSAFPGAFilterMA;
class BSEmcPSAFPGAFilterDelay;
class BSEmcPSAFPGALinFitter;
class BSEmcWaveform;
class TBuffer;
class TClass;
class TMemberInspector;

class BSEmcPSAFPGASampleAnalyser : public BSEmcAbsPSA {
  constexpr static Int_t fgMaxNumberOfHits{100};

 public:
  BSEmcPSAFPGASampleAnalyser();
  BSEmcPSAFPGASampleAnalyser(const std::string &t_fname);
  virtual ~BSEmcPSAFPGASampleAnalyser();

  virtual void initFromFile(const std::string &t_fname);
  virtual void reset() = 0;
  virtual void put(Float_t t_valueToStore) = 0;
  virtual void setBaseline(Float_t t_newBaseline) { fBaseline_value = t_newBaseline; };

  virtual Float_t baseline() = 0;
  Int_t nHits() { return fNumber_of_hits; };
  Float_t sampleTime(Int_t t_index);
  Float_t sampleAmplitude(Int_t t_index);
  Float_t sampleIntegral(Int_t t_index);
  Int_t samplePileup(Int_t t_index);

  virtual Int_t Process(const BSEmcWaveform *t_waveform);
  virtual void Reset() { this->reset(); };
  virtual void GetHit(Int_t t_i, Double_t &t_energy, Double_t &t_time);
  virtual void GetHit(Int_t t_i, Double_t &t_energy, Double_t &t_time, Int_t &t_PileupType);

  virtual void Init(const std::vector<Double_t> &t_params);

  struct SampleAnalyserParams {
    Int_t ma_trig_M;
    Float_t hit_threshold;
    Int_t cf_delay;
    Float_t cf_ratio;
    Int_t cf_fitter_length;
    Int_t cf_fit_offset;
    Int_t mwd_length;
    Float_t mwd_tau;
    Int_t mwd2_length;
    Float_t mwd2_tau;
    Int_t mwd3_length;
    Float_t mwd3_tau;
    Int_t sig_delay;
    Float_t rough_timing_corr;
    Float_t clock_unit; // unit time per point
    Float_t iafactor;   // integral vs ampltitue
  };

  enum pileup_t { kSingle = 0, kPileup1 = 1, kPileup2 = 2, kInvalid = 99 };
  enum status_t { kBaseline, kInPulse, kPulseDetected, kPulseFinished, kPileupFinished, kUndefined = 99 };
  status_t GetStatus() { return fStatus; }

 protected:
  virtual void init(SampleAnalyserParams &t_params);

  SampleAnalyserParams fAnalyserParams;

  Float_t fBaseline_value{0};
  Int_t fLocal_time{0};
  Bool_t fPulse_detected{kFALSE};
  Int_t fRough_pulse_timing{0};
  Int_t fCF_zero_crossing_points{0}; // Unused

  BSEmcPSAFPGAFilterCF *fCF_filter{nullptr};
  BSEmcPSAFPGAFilterCF *fCF_filter2{nullptr};
  BSEmcPSAFPGAFilterMA *fMA_triggering{nullptr};
  BSEmcPSAFPGAFilterDelay *fMA_delay{nullptr};
  BSEmcPSAFPGAFilterDelay *fCF_delay{nullptr};
  BSEmcPSAFPGAFilterDelay *fSignal_delay{nullptr};
  BSEmcPSAFPGAFilterDelay *fSignal_buffer{nullptr};

  BSEmcPSAFPGALinFitter *fCF_Fitter{nullptr};

  // features of the detected pulses
  Int_t fNumber_of_hits{0};
  Float_t fTime[fgMaxNumberOfHits]{};
  Float_t fIntegral[fgMaxNumberOfHits]{};
  Float_t fAmplitude[fgMaxNumberOfHits]{};

  status_t fStatus{kUndefined};
  Int_t fPileups[fgMaxNumberOfHits]{};

  ClassDef(BSEmcPSAFPGASampleAnalyser, 2);
};

#endif /*BSEMCPSAFPGASAMPLEANALYSER_HH*/
