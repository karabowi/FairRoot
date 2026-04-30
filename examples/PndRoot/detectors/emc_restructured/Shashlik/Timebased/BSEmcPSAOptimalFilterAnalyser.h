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

#ifndef BSEMCOPTIMALFILTERANALYSER_HH
#define BSEMCOPTIMALFILTERANALYSER_HH

#include "BSEmcAbsPSA.h"
#include <string>
#include <vector>

#include "BSEmcShashlykPulseshape.h"

//#define MAX_NUMBER_OptimalFilter_HITS 100

class BSEmcPSAOptimalFilterAnalyser : public BSEmcAbsPSA {

 public:
  BSEmcPSAOptimalFilterAnalyser(Double_t cf_frac, Double_t cf_tap, const Double_t *cf_tcorr, Int_t cf_n, const Double_t *of_a, const Double_t *of_b, Int_t of_m, Int_t of_b0,
                                BSEmcAbsPulseshape *shape, Double_t threshold, Double_t baseline = 0., Int_t verbose = 1);
  BSEmcPSAOptimalFilterAnalyser();
  ~BSEmcPSAOptimalFilterAnalyser();
  void setBaseline(Double_t baseline) { fBaseline = baseline; }
  void setVerbose(Int_t verbose) { fVerbose = verbose; }

  Int_t Process(const BSEmcWaveform *waveform);
  void GetHit(Int_t i, Double_t &energy, Double_t &time);
  void Reset();

private:
  Double_t fBaseline;

  Double_t fCF_fraction;
  Int_t fCF_tap;
  Int_t fCF_nsamples;
  Double_t* fCF_tcorr;

  Int_t fOF_b0;
  Int_t fOF_m;
  Double_t* fOF_a;
  Double_t* fOF_b;

  Double_t fThreshold;
  std::vector<Double_t> fDigiAmplitude;
  std::vector<Double_t> fDigiTime;

  BSEmcAbsPulseshape *fPulse;
  Double_t fSampleRate;
  Double_t fTimeStamp;

  Int_t fVerbose;

  void analyse(std::vector<Double_t>& signal, std::vector<Double_t>& baseline, Int_t& start_position); // return position for the next signal processing
  void zero_crossing(const std::vector<Double_t>& wf, Int_t& start_position, Double_t& time, Int_t& index);
  void generate_baseline(std::vector<Double_t>& baseline, Double_t amplitude, Double_t time);
  void subtract_baseline(std::vector<Double_t>& signal, const std::vector<Double_t>& baseline);

  ClassDef(BSEmcPSAOptimalFilterAnalyser, 0);
};

#endif /*BSEMCOPTIMALFILTERANALYSER_HH*/
