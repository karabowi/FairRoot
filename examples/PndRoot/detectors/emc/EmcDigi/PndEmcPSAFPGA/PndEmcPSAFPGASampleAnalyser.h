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

#ifndef PNDEMCFPGASAMPLEANALYSER_HH
#define PNDEMCFPGASAMPLEANALYSER_HH

#include <string>
#include <fstream>
class PndEmcPSAFPGAFilterCF;
class PndEmcPSAFPGAFilterMA;
class PndEmcPSAFPGAFilterDelay;
class PndEmcPSAFPGALinFitter;

#include "PndEmcAbsPSA.h"

#define MAX_NUMBER_OF_HITS 100

class PndEmcPSAFPGASampleAnalyser : public PndEmcAbsPSA{

 public:
  PndEmcPSAFPGASampleAnalyser();
  PndEmcPSAFPGASampleAnalyser(const std::string &fname);
  virtual ~PndEmcPSAFPGASampleAnalyser();

  virtual void initFromFile(const std::string &fname);
  virtual void reset() = 0;
  virtual void put(float valueToStore) = 0;
  virtual void setBaseline(float newBaseline){baseline_value = newBaseline;};

  virtual float baseline() = 0;
  int nHits(){return Number_of_hits;};
  float sampleTime(int index);
  float sampleAmplitude(int index);
  float sampleIntegral(int index);
  Int_t samplePileup(int index);		 
  virtual Int_t Process(const PndEmcWaveform *waveform);
  virtual void Reset(){this->reset();};
  virtual void GetHit(Int_t i,Double_t &energy, Double_t &time);
  virtual void GetHit2(Int_t i, Double_t &energy, Double_t &time, Int_t &PileupType);
  virtual void Init(const std::vector<Double_t> &params);

  struct SampleAnalyserParams {
	  int ma_trig_M;
	  float hit_threshold;
	  int cf_delay;
	  float cf_ratio;
	  int cf_fitter_length;
	  int cf_fit_offset;
	  int mwd_length;
	  float mwd_tau;
	  int mwd2_length;
	  float mwd2_tau;
	  int mwd3_length;
	  float mwd3_tau;
	  int sig_delay;
	  float rough_timing_corr;
	  float clock_unit;//unit time per point
	  float iafactor;//integral vs ampltitue
  } ;

  enum status_t {kBaseline, kInPulse, kPulseDetected, kPulseFinished, kPileupFinished, kUndefined=99};
  status_t GetStatus() { return status; }

 protected:
  virtual void init(SampleAnalyserParams &params);

  SampleAnalyserParams analyserParams;

  float baseline_value;
  unsigned int local_time;
  bool pulse_detected;
  int rough_pulse_timing;
  int cf_zero_crossing_points;
  std::ofstream Ratio;

  PndEmcPSAFPGAFilterCF     *CF_filter;
  PndEmcPSAFPGAFilterCF     *CF_filter2;
  PndEmcPSAFPGAFilterMA     *MA_triggering;
  PndEmcPSAFPGAFilterDelay  *MA_delay;
  PndEmcPSAFPGAFilterDelay  *CF_delay;
  PndEmcPSAFPGAFilterDelay  *Signal_delay;
  PndEmcPSAFPGAFilterDelay  *Signal_buffer;
  PndEmcPSAFPGALinFitter    *CF_Fitter;

  // features of the detected pulses
  int Number_of_hits;
  float time[MAX_NUMBER_OF_HITS];
  float integral[MAX_NUMBER_OF_HITS];
  float amplitude[MAX_NUMBER_OF_HITS];
  Int_t pileups[MAX_NUMBER_OF_HITS];
  status_t status;
 
  ClassDef(PndEmcPSAFPGASampleAnalyser,2);
};

#endif
