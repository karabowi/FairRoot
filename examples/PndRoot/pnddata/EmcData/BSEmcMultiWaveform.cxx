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


//=============================================================================
// BSEmcMultiWaveform.cxx
//
// Class to hold multiple waveforms from one Emc Hit / ADC readout
//
// Author: Christian Hammmann
//         chammann@hiskp.uni-bonn.de
//
//=============================================================================

#include "BSEmcMultiWaveform.h"

#include "BSEmcWaveform.h"

BSEmcMultiWaveform::BSEmcMultiWaveform(Int_t t_trackId, long t_detId, long t_waveform_length, Int_t t_hitIndex)
  : BSEmcWaveform(), fSignals(1, std::vector<Double_t>(t_waveform_length, 0.)), fActiveWaveform(1)
{
  SetSampleRate(0);
  fTrackId = (t_trackId);
  fDetectorId = (t_detId);
  fWaveformLength = (t_waveform_length);
  fHitIndex = (t_hitIndex);
}

void BSEmcMultiWaveform::clearAndReset()
{
  fSignals.clear();
  fSignals.at(0) = std::vector<Double_t>(fWaveformLength, 0.);
}

std::vector<Double_t> BSEmcMultiWaveform::GetSignal() const
{
  if (fActiveWaveform < 1 || fActiveWaveform >= (Int_t)fSignals.size()) {
    return fSignals.at(0);
  } else {
    return fSignals.at(fActiveWaveform);
  }
}

void BSEmcMultiWaveform::SetWaveform(const std::vector<Double_t> &t_signal, Int_t /*unused*/, Int_t t_waveform)
{
  if (t_waveform < 0) {
    return;
  }
  if (t_waveform >= (Int_t)fSignals.size()) {
    fSignals.push_back(t_signal);
    // fSignals.insert(fSignals.end(), t_waveform - fSignals.size() + 1, std::vector<Double_t>(fWaveformLength, 0.));
  }
  fSignals[t_waveform] = t_signal;
}

ClassImp(BSEmcMultiWaveform);
