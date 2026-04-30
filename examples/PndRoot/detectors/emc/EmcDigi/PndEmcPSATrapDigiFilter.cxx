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
//      Dima Melnychuk
//
//-----------------------------------------------------------

// This Class' Header ------------------
#include "PndEmcPSATrapDigiFilter.h"
#include "PndEmcWaveform.h"
#include "PndEmcFadcFilter.h"

#include "TH1D.h"
#include "TCanvas.h"

// Class Member definitions -----------

PndEmcPSATrapDigiFilter::PndEmcPSATrapDigiFilter(const std::vector<Double_t> params) : fParams(params) {}

Int_t PndEmcPSATrapDigiFilter::Process(const PndEmcWaveform *waveform)
{
  Process(waveform, lAmplitude, lTime);
  if (lAmplitude > 0) {
    return 1;
  } else {
    return 0;
  }
}
void PndEmcPSATrapDigiFilter::GetHit(Int_t, Double_t &energy, Double_t &time)
{ // i //[R.K.03/2017] unused variable(s)
  energy = lAmplitude;
  time = lTime;
}
void PndEmcPSATrapDigiFilter::Process(const PndEmcWaveform *waveform, Double_t &amplitude, Double_t &time)
{
  const std::vector<Double_t> signal = waveform->GetSignal();
  std::vector<Double_t> output;

  // Signal filtartion
  Int_t rise_time = (Int_t)fParams[0];   // rise time of trapezoid (in sampling periods)
  Int_t flat_length = (Int_t)fParams[1]; // length of the flat top period of trapezoid (in sampling periods)
  Int_t shift = (Int_t)fParams[2];       // shift of the point to determine energy

  Int_t energy_point = rise_time + flat_length + shift - 1;

  PndEmcFadcFilter *flt = new PndEmcFadcFilter();
  flt->SetupTrapez(rise_time, flat_length);
  flt->Filter(signal, output);
  delete flt;

  amplitude = output[energy_point];

  // Determination of the time is not yet implemented
  time = 0;

  return;
}
