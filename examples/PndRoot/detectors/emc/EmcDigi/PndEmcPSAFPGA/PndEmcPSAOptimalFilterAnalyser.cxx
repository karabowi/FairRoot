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

#include "PndEmcPSAOptimalFilterAnalyser.h"

#include "PndEmcWaveform.h"
#include "FairRootManager.h"
#include <iostream>
#include <fstream>
#include <string>

PndEmcPSAOptimalFilterAnalyser::PndEmcPSAOptimalFilterAnalyser(Double_t cf_frac, Double_t cf_tap, 
const Double_t* cf_tcorr, Int_t cf_n, const Double_t* of_a, const Double_t* of_b, Int_t of_m, 
Int_t of_b0, PndEmcAbsPulseshape* shape, Double_t threshold, Double_t baseline, Int_t verbose) : PndEmcAbsPSA()
{
  fCF_fraction = cf_frac;
  fCF_tap = cf_tap;
  fCF_nsamples = cf_n;
  fCF_tcorr = new Double_t[fCF_nsamples];
  for (Int_t i = 0; i < fCF_nsamples; i++) {
    fCF_tcorr[i] = cf_tcorr[i];
  }

  fOF_m = of_m;
  fOF_b0 = of_b0;
  fOF_a = new Double_t[fOF_m*fCF_nsamples];
  fOF_b = new Double_t[fOF_m*fCF_nsamples];
  for (Int_t i = 0; i < fOF_m*fCF_nsamples; i++) {
    fOF_a[i] = of_a[i];
    fOF_b[i] = of_b[i];
  }

  fPulse = shape;
  fThreshold = threshold;
  fBaseline = baseline;
  fVerbose = verbose;
}

PndEmcPSAOptimalFilterAnalyser::PndEmcPSAOptimalFilterAnalyser() : PndEmcAbsPSA()
{
  fBaseline = 0.;
  fCF_fraction = 0.5;
  fCF_tap = 2;
  fCF_tcorr = NULL;
  fCF_nsamples = 0;
  fOF_a = NULL;
  fOF_b = NULL;
  fOF_m = 0;
  fOF_b0 = 0;
  fThreshold = 0.001;
  fPulse = NULL;
  fVerbose = 1;
}

PndEmcPSAOptimalFilterAnalyser::~PndEmcPSAOptimalFilterAnalyser()
{
  delete[] fCF_tcorr;
  delete[] fOF_a;
  delete[] fOF_b;
}

Int_t PndEmcPSAOptimalFilterAnalyser::Process(const PndEmcWaveform *waveform)
{
  Reset();

  std::vector<Double_t> signal = waveform->GetSignal();
  std::vector<Double_t> baseline(signal.size(), fBaseline);
  fSampleRate = waveform->GetSampleRate();
  fTimeStamp = waveform->GetTimeStamp();
  Int_t start_position = 0;

  if (fVerbose > 2) {
    std::set<FairLink> links = waveform->GetLinks();
    std::set<FairLink>::iterator it = links.begin();
    cout << "[PndEmcPSAOptimalFilterAnalyser] fair_link " << links.size() << endl;
    for (; it != links.end(); it++) {
        if (FairRootManager::Instance()->GetBranchName(it->GetType()).CompareTo("EmcWaveformData") == 0) {
            cout << "[PndEmcPSAOptimalFilterAnalyser] " << it->GetEntry() << " " << it->GetIndex() << endl;
        }
    }
  }

  while (start_position >= 0) {
    analyse(signal, baseline, start_position);
  }

  return fDigiAmplitude.size();
}

void PndEmcPSAOptimalFilterAnalyser::GetHit(Int_t i, Double_t &energy, Double_t &time) {
  if (i >= 0 && i < (Int_t)fDigiAmplitude.size()) {
    energy = fDigiAmplitude[i];
    time = fDigiTime[i];
  }
  else {
    energy = -999.;
    time = -999.;
  }
}

void PndEmcPSAOptimalFilterAnalyser::Reset() {
  fDigiAmplitude.clear();
  fDigiTime.clear();
}

void PndEmcPSAOptimalFilterAnalyser::analyse(std::vector<Double_t>& signal, std::vector<Double_t>& baseline, Int_t& start_position) {

  //static int iwf = -1;
  //if (fVerbose > 2 && start_position == 0) {
  //  iwf++;
  //}

  // cf waveform
  std::vector<Double_t> cf_signal(signal.size(), 0.);
  for (UInt_t i = 0; i < signal.size(); i++) {
    if (i >= (UInt_t)fCF_tap)
      cf_signal[i] = signal[i-fCF_tap] - fCF_fraction*signal[i];
  }

  // t0
  Double_t t_BCFD = -999.;
  Int_t index = 0;
  zero_crossing(cf_signal, start_position, t_BCFD, index);
  if (start_position < 0) return;
  Double_t t_OF_init = t_BCFD - fCF_tcorr[index];

  // OF filter
  Double_t alpha1 = 0.;
  Double_t alpha2 = 0.;
  for (Int_t i = 0; i < fOF_m; i++) {
    alpha1 += fOF_a[index*fCF_nsamples+i] * signal[(Int_t)t_BCFD+fOF_b0+i+1];
    alpha2 += fOF_b[index*fCF_nsamples+i] * signal[(Int_t)t_BCFD+fOF_b0+i+1];
  }
  Double_t amplitude = alpha1; // amplitude
  if (amplitude == 0) return;
  Double_t tau = alpha2 / amplitude;
  Double_t time = fTimeStamp + (t_OF_init + tau) / fSampleRate * 1E9; // time[ns]

  // check 1
  if (fVerbose > 2) {
    if (amplitude > fThreshold) {
      //cout << "[PndEmcPSAOptimalFilterAnalyser] raw_waveform " << iwf << " " << fTimeStamp << " " << fSampleRate << " " << signal.size() << endl;
      cout << "[PndEmcPSAOptimalFilterAnalyser] raw_waveform " << fTimeStamp << " " << fSampleRate << " " << signal.size() << endl;
      for (unsigned int i = 0; i < signal.size(); i++) {
        cout << "[PndEmcPSAOptimalFilterAnalyser] " << signal[i] << endl;
      }
      cout << "[PndEmcPSAOptimalFilterAnalyser] cf_waveform " << cf_signal.size() << endl;
      for (unsigned int i = 0; i < cf_signal.size(); i++) {
        cout << "[PndEmcPSAOptimalFilterAnalyser] " << cf_signal[i] << endl;
      }
      cout << "[PndEmcPSAOptimalFilterAnalyser] initial_t0 " << endl;
      cout << "[PndEmcPSAOptimalFilterAnalyser] " << t_BCFD << endl;
      cout << "[PndEmcPSAOptimalFilterAnalyser] of_results" << endl;
      cout << "[PndEmcPSAOptimalFilterAnalyser] " << amplitude << endl;
      cout << "[PndEmcPSAOptimalFilterAnalyser] " << time << endl;
    }
  }

  // hit detection
  if (amplitude > fThreshold) {
    fDigiAmplitude.push_back(amplitude);
    fDigiTime.push_back(time);
    generate_baseline(baseline, amplitude, time);
    subtract_baseline(signal, baseline);
  }

  // check 2
  if (fVerbose > 2) {
    if (amplitude > fThreshold) {
      cout << "[PndEmcPSAOptimalFilterAnalyser] baseline " << signal.size() << endl;
      for (unsigned int i = 0; i < signal.size(); i++) {
        cout << "[PndEmcPSAOptimalFilterAnalyser] " << baseline[i] << endl;
      }
      cout << "[PndEmcPSAOptimalFilterAnalyser] signal_sub " << signal.size() << endl;
      for (unsigned int i = 0; i < signal.size(); i++) {
        cout << "[PndEmcPSAOptimalFilterAnalyser] " << signal[i] << endl;
      }
    }
  }

}

void PndEmcPSAOptimalFilterAnalyser::zero_crossing(const std::vector<Double_t>& wf, Int_t& start_position, Double_t& time, Int_t& index) {
  Double_t last = 99.;
  for (unsigned int i = start_position; i < wf.size(); i++) {
    if (last < -0.5 && wf[i] > 0.5) { // set 0.5 to avoid ieee double precision error
      time = (i - 1) - last / (wf[i] - last); // zero crossing point

      index = (int)((time - (i - 1)) * fOF_m); // index in the 4 bins
      if (index == fOF_m) index--;

      time = (i - 1) + 1./fOF_m/2 + (Double_t)index/fOF_m; // alligned time

      start_position = (Int_t)time + 1;
      return;
    }
    last = wf[i];
  }
  start_position = -1;
}

void PndEmcPSAOptimalFilterAnalyser::generate_baseline(std::vector<Double_t>& baseline, Double_t amplitude, Double_t time) {
  for (UInt_t i = 0; i < baseline.size(); i++) {
    Double_t t = fTimeStamp + i / fSampleRate * 1E9; // ns
    baseline[i] = (*fPulse)(t, amplitude, time) + fBaseline;
  }
}

void PndEmcPSAOptimalFilterAnalyser::subtract_baseline(std::vector<Double_t>& signal, const std::vector<Double_t>& baseline) {
  for (UInt_t i = 0; i < signal.size(); i++) {
    signal[i] -= baseline[i];
  }
}


ClassImp(PndEmcPSAOptimalFilterAnalyser);
