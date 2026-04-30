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

#ifndef PNDEMCFPGAPILEUPANALYSER_HH
#define PNDEMCFPGAPILEUPANALYSER_HH

#include "PndEmcPSAFPGASampleAnalyser.h"
#include <string>
#include "TRandom.h"
#include <fstream>

class PndEmcWaveform;
class PndEmcPSAFPGAFilterCF;
class PndEmcPSAFPGAFilterMA;
class PndEmcPSAFPGAFilterMWD;
class PndEmcPSAFPGAFilterMWDsmooth;
class PndEmcPSAFPGAFilterDelay;
class PndEmcPSAFPGALinFitter;
class TF1;
class TGraph;

#define MAX_NUMBER_OF_HITS 1000

class PndEmcPSAFPGAPileupAnalyser : public PndEmcPSAFPGASampleAnalyser {

 public:
  PndEmcPSAFPGAPileupAnalyser();
  virtual ~PndEmcPSAFPGAPileupAnalyser();
  virtual void SetVerbose(Int_t verbose=0) { fVerbose = verbose; }

  enum pileup_t {kSingle=0, kPileup1=1, kPileup2=2, kInvalid=99};

  virtual void initFromFile() {};
  virtual void Init(const std::vector<Double_t> &params, TF1* R_thres, TF1* R_mean, float extBaselineValue=0);
  virtual void Init(const std::vector<Double_t> &params, TF1* R_thres, TF1* R_mean, unsigned int baselineStartSample, unsigned int baselineStopSample);

  virtual void GetHit(Int_t i, Double_t &Energy, Double_t &Time);
 //Collect Hits with pile-up flags
  virtual void GetHit2(Int_t i, Double_t &Energy, Double_t &Time, Int_t &PileupType);

  void GetEnergyMeasures(Int_t i, Double_t &Amplitude, Double_t &Integral);
  virtual void reset();
  virtual void put(float valueToStore);
  virtual float baseline() {return baseline_value;};
  
  

protected:
  enum {kWindow, kExtern, kFollow} baselineMode;

  virtual void setBaseline(float newBaseline);
  virtual void setBaselineWindow(unsigned int startSample, unsigned int stopSample);
  virtual void InitParameters(const std::vector<Double_t> &params);
  virtual void Init2(const std::vector<Double_t> &params, TF1* R_thres, TF1* R_mean);
 
  Int_t fVerbose;
  TF1* Int_thres;  
  TF1* Int_mean; 
  TGraph* grxy;
  unsigned int BaselineStartSample;
  unsigned int BaselineStopSample;
  float BaselineSum;

  PndEmcPSAFPGAFilterMWD* MWD_filter1;
  PndEmcPSAFPGAFilterMA* MA_filter1;
  PndEmcPSAFPGAFilterMWD* MWD_filter2;
    PndEmcPSAFPGAFilterMWD* MWD_filter3;
  PndEmcPSAFPGAFilterDelay* CF_prev;
  double CF_prev_val;
  double CF_prev_val2;
  std::ofstream Ratio,MWD,MWD2,Ratio_int;
  bool energy_finished;
  bool energy_finished_mwd;
  bool energy_finished_mwd2;
  bool pulse_detected_mwd;
  bool pulse_detected_mwd2;
  bool timing_finished;
   // bool timing_finished_mwd;
  bool func_defined;
  unsigned int cf_crossing;
  bool in_cfRise;
  float savedValuetoStore;
  float sav_mwd;
   float sav_mwd2;
   float Square_mwd2;
      float Square;
      float t;
      float t_mwd;
      float t_mwd2[MAX_NUMBER_OF_HITS];
      float max;
float max_mwd2[100];
float max_mwd;
float R_mwd;
  int saved_local_time[MAX_NUMBER_OF_HITS];
    int saved_local_time_mwd[MAX_NUMBER_OF_HITS];
    int saved_local_time_mwd2[MAX_NUMBER_OF_HITS];
  int Number_of_puls;
  int Number_of_puls_mwd;
  int Number_of_puls_mwd2;
  int Number_of_sample_mwd;
   int Number_of_sample_mwd2;
  int Number_of_sample;
  int z[MAX_NUMBER_OF_HITS];
  int z_mwd[MAX_NUMBER_OF_HITS];
  int z_mwd2[MAX_NUMBER_OF_HITS];
  float cfZero[MAX_NUMBER_OF_HITS];
  float cfZero2[MAX_NUMBER_OF_HITS];
  float cfRise[MAX_NUMBER_OF_HITS];
  float signal_buffer[MAX_NUMBER_OF_HITS][MAX_NUMBER_OF_HITS];
  float signal_buffer_mwd[MAX_NUMBER_OF_HITS][MAX_NUMBER_OF_HITS];
  float signal_buffer_mwd2[MAX_NUMBER_OF_HITS][MAX_NUMBER_OF_HITS];


  ClassDef(PndEmcPSAFPGAPileupAnalyser,2);
};





#endif
