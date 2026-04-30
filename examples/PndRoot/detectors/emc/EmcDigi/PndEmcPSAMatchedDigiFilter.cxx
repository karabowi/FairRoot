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
//      Matched digital filter
//      Dima Melnychuk
//
//-----------------------------------------------------------

// This Class' Header ------------------
#include "PndEmcPSAMatchedDigiFilter.h"
#include "PndEmcWaveform.h"
#include "PndEmcFadcFilter.h"
#include "PndEmcAbsPulseshape.h"

// Class Member definitions -----------

PndEmcPSAMatchedDigiFilter::PndEmcPSAMatchedDigiFilter(const std::vector<Double_t> params, PndEmcAbsPulseshape *pulseshape) : fParams(params), fPulseshape(pulseshape) {}

Int_t PndEmcPSAMatchedDigiFilter::Process(const PndEmcWaveform *waveform)
{
  Process(waveform, lAmplitude, lTime);
  if (lAmplitude > 0) {
    return 1;
  } else {
    return 0;
  }
}
void PndEmcPSAMatchedDigiFilter::GetHit(Int_t, Double_t &energy, Double_t &time)
{ // i //[R.K.03/2017] unused variable(s)
  energy = lAmplitude;
  time = lTime;
}

void PndEmcPSAMatchedDigiFilter::Process(const PndEmcWaveform *waveform, Double_t &amplitude, Double_t &time)
{
  const std::vector<Double_t> signal = waveform->GetSignal();
  std::vector<Double_t> output;

  Int_t width = (Int_t)fParams[0];            // width of the filter
  Double_t sampleRate = (Double_t)fParams[1]; // sample rate of ADC

  PndEmcFadcFilter *flt = new PndEmcFadcFilter();
  flt->SetupMatchedFilter(width, fPulseshape, sampleRate);
  flt->Filter(signal, output);
  delete flt;

  // Extraction of energy and time information
  std::vector<Double_t>::iterator p;
  p = max_element(output.begin(), output.end());
  Int_t pPosition = distance(output.begin(), p);

  amplitude = *p;
  time = pPosition;

  return;
}
