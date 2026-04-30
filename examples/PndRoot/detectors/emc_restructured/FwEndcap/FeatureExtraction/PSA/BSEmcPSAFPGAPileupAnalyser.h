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

#ifndef BSEMCPSAFPGAPILEUPANALYSER_HH
#define BSEMCPSAFPGAPILEUPANALYSER_HH

#include <fstream>
#include <string>
#include <vector>

#include "Rtypes.h"
#include "RtypesCore.h"

#include "BSEmcPSAFPGASampleAnalyser.h"

class BSEmcWaveform;
class BSEmcPSAFPGAFilterCF;
class BSEmcPSAFPGAFilterMA;
class BSEmcPSAFPGAFilterMWD;
class BSEmcPSAFPGAFilterDelay;
class BSEmcPSAFPGALinFitter;
class TF1;
class TBuffer;
class TClass;
class TMemberInspector;

class BSEmcPSAFPGAPileupAnalyser : public BSEmcPSAFPGASampleAnalyser {
  constexpr static Int_t fgMaxNumberOfHits{1000};

 public:
  BSEmcPSAFPGAPileupAnalyser();
  virtual ~BSEmcPSAFPGAPileupAnalyser();
  virtual void SetVerbose(Int_t t_verbose = 0) { fVerbose = t_verbose; }

  virtual void initFromFile() /*override*/ {};
  virtual void Init(const std::vector<Double_t> &t_params, TF1 *t_r_thres, TF1 *t_r_mean, Float_t t_extBaselineValue = 0);
  virtual void Init(const std::vector<Double_t> &t_params, TF1 *t_r_thres, TF1 *t_r_mean, UInt_t t_baselineStartSample, UInt_t t_baselineStopSample);

  virtual void GetHit(Int_t t_idx, Double_t &t_energy, Double_t &t_time);
  virtual void GetHit(Int_t t_idx, Double_t &t_energy, Double_t &t_time, Int_t &t_pileupType);

  void GetEnergyMeasures(Int_t t_idx, Double_t &t_amplitude, Double_t &t_integral);
  virtual void reset() /*override*/;
  virtual void put(Float_t t_valueToStore) /*override*/;

  virtual Float_t baseline() /*override*/ { return fBaseline_value; };
  virtual void setBaselineWindow(UInt_t t_startSample, UInt_t t_stopSample);

 protected:
  enum { kWindow, kExtern, kFollow } fBaselineMode = kWindow;

  virtual void setBaseline(Float_t t_newBaseline) /*override*/;
  virtual void InitParameters(const std::vector<Double_t> &t_params);
  virtual void Init2(const std::vector<Double_t> &t_params, TF1 *t_r_thres, TF1 *t_r_mean);
  Int_t fVerbose{0};
  TF1 *fInt_thres{nullptr};
  TF1 *fInt_mean{nullptr};

  Int_t fBaselineStartSample{0};
  Int_t fBaselineStopSample{0};
  Float_t fBaselineSum{0};

  BSEmcPSAFPGAFilterMWD *fMWD_filter1{nullptr};
  BSEmcPSAFPGAFilterMA *fMA_filter1{nullptr};

  BSEmcPSAFPGAFilterMWD *fMWD_filter2{nullptr};
  BSEmcPSAFPGAFilterMWD *fMWD_filter3{nullptr};
  Double_t fCF_prev_val{0};
  Double_t fCF_prev_val2{0};
  std::ofstream fRatio, fMWD, fMWD2;
  Bool_t fEnergy_finished{kFALSE};
  Bool_t fEnergy_finished_mwd{kFALSE};
  Bool_t fEnergy_finished_mwd2{kFALSE};
  Bool_t fPulse_detected_mwd{kFALSE};
  Bool_t fPulse_detected_mwd2{kFALSE};
  Bool_t fTiming_finished{kFALSE};
  Bool_t fFunc_defined{kFALSE};

  UInt_t fCF_crossing{0};
  Bool_t fIn_cfRise{kFALSE};
  Float_t fCFZero[fgMaxNumberOfHits]{};
  Float_t fCFRise[fgMaxNumberOfHits]{};
  Float_t fSavedValuetoStore{0};
  Float_t fSav_mwd{0};
  Float_t fSav_mwd2{0};
  Float_t fSquare_mwd2{0};
  Float_t fSquare{0};
  Float_t fT{0};
  Float_t fT_mwd{0};
  Float_t fT_mwd2[fgMaxNumberOfHits];
  Float_t fMax{0};
  Float_t fMax_mwd2[100];
  Float_t fMax_mwd{0};
  Float_t fR_mwd{0};
  Int_t fSaved_local_time[fgMaxNumberOfHits];
  Int_t fSaved_local_time_mwd[fgMaxNumberOfHits];
  Int_t fSaved_local_time_mwd2[fgMaxNumberOfHits];
  Int_t fNumber_of_puls{0};
  Int_t fNumber_of_puls_mwd{0};
  Int_t fNumber_of_puls_mwd2{0};
  Int_t fNumber_of_sample_mwd{0};
  Int_t fNumber_of_sample_mwd2{0};
  Int_t fNumber_of_sample{0};
  Int_t fZ[fgMaxNumberOfHits];
  Int_t fZ_mwd[fgMaxNumberOfHits];
  Int_t fZ_mwd2[fgMaxNumberOfHits];
  Float_t fCfZero[fgMaxNumberOfHits];
  Float_t fCfZero2[fgMaxNumberOfHits];
  Float_t fCfRise[fgMaxNumberOfHits];
  Float_t fSignal_buffer_mwd[fgMaxNumberOfHits][fgMaxNumberOfHits];
  Float_t fSignal_buffer_mwd2[fgMaxNumberOfHits][fgMaxNumberOfHits];

  ClassDef(BSEmcPSAFPGAPileupAnalyser, 2);
};

#endif /*BSEMCPSAFPGAPILEUPANALYSER_HH*/
