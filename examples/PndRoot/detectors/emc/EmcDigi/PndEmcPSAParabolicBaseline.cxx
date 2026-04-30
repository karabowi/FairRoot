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

//-----------------------------------------------------------
//
// Description:
//      Pulseshape analysis for ADC waveforms
//      (see header file for more details)
//
//      Dima Melnychuk
//
//-----------------------------------------------------------

// This Class' Header ------------------
#include "PndEmcPSAParabolicBaseline.h"
#include "PndEmcWaveform.h"
#include <iostream>

// Class Member definitions -----------

void PndEmcPSAParabolicBaseline::Process(const PndEmcWaveform *waveform, Double_t &amplitude, Double_t &time)
{
  const std::vector<Double_t> signal = waveform->GetSignal();
  FitPeak(signal, amplitude, time);
  Double_t baseline;
  GetBaseline(signal, baseline);
  //    std::cout << "amplitude: " << amplitude << " baseline: " << baseline << std::endl;
  amplitude -= baseline;
}

Int_t PndEmcPSAParabolicBaseline::Process(const PndEmcWaveform *waveform)
{
  Process(waveform, lAmplitude, lTime);
  if (lAmplitude > 0) {
    return 1;
  } else {
    return 0;
  }
}
void PndEmcPSAParabolicBaseline::GetHit(Int_t, Double_t &energy, Double_t &time)
{ // i //[R.K.03/2017] unused variable(s)
  energy = lAmplitude;
  time = lTime;
}

void PndEmcPSAParabolicBaseline::FitPeak(const std::vector<Double_t> &signal, Double_t &ampl, Double_t &pos, Int_t peakBin) const
{
  Int_t waveformLength = signal.size();

  ampl = pos = -1.;
  if (peakBin > 0 && peakBin < waveformLength - 1) {
    long theBin(peakBin);

    Double_t pValue = signal[peakBin];
    Double_t pPosition = Double_t(peakBin);

    Double_t leftValue = signal[theBin - 1];
    Double_t rightValue = signal[theBin + 1];
    if (leftValue < pValue && rightValue < pValue) {
      Double_t d = 0.25 * (rightValue - leftValue);
      Double_t b = pValue - 0.5 * (leftValue + rightValue);
      pValue += d * d / b;
      pPosition += d / b;
    }
    ampl = pValue;
    pos = pPosition;
  }
}

void PndEmcPSAParabolicBaseline::FitPeak(const std::vector<Double_t> &signal, Double_t &ampl, Double_t &pos, Int_t start, Int_t end) const
{
  std::vector<Double_t>::const_iterator p;
  p = max_element(signal.begin() + start, signal.begin() + end);
  Int_t pPosition = distance(signal.begin(), p);
  FitPeak(signal, ampl, pos, pPosition);
}

void PndEmcPSAParabolicBaseline::FitPeak(const std::vector<Double_t> &signal, Double_t &ampl, Double_t &pos) const
{
  std::vector<Double_t>::const_iterator p;
  p = max_element(signal.begin(), signal.end());
  Int_t pPosition = distance(signal.begin(), p);
  FitPeak(signal, ampl, pos, pPosition);
}
void PndEmcPSAParabolicBaseline::GetBaseline(const std::vector<Double_t> &signal, Double_t &baseline) const
{
  std::vector<Double_t>::const_iterator p;
  p = signal.begin();
  baseline = 0;
  for (Int_t i = 0; i < lBaselength; i++) {
    baseline += *p++;
  }
  baseline /= lBaselength;
}

ClassImp(PndEmcPSAParabolicBaseline);
