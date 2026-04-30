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
// PndEmcMultiWaveform.cxx
//
// Class to hold multiple waveforms from one Emc Hit / ADC readout
//
// Author: Christian Hammmann
//         chammann@hiskp.uni-bonn.de
//
//=============================================================================

#include "PndEmcMultiWaveform.h"
#include "FairRootManager.h"

PndEmcMultiWaveform::PndEmcMultiWaveform(int trackId, long detId, long waveform_length, Int_t hitIndex)
  : PndEmcWaveform(trackId, detId, 0, waveform_length, hitIndex), // sample Rate=0
    fSignals(1, std::vector<Double_t>(waveform_length, 0.)), fActiveWaveform(1)
{
  if (FairRootManager::Instance() != nullptr) {
    SetLink(FairLink(-1, FairRootManager::Instance()->GetEntryNr(), "EmcHit", hitIndex));
  }
}

void PndEmcMultiWaveform::clearAndReset()
{
  fSignals.clear();
  fSignals.at(0) = std::vector<Double_t>(fWaveformLength, 0.);
}

std::vector<Double_t> PndEmcMultiWaveform::GetSignal() const
{
  if (fActiveWaveform < 1 || fActiveWaveform >= (int)fSignals.size()) {
    return fSignals.at(0);
  } else {
    return fSignals.at(fActiveWaveform);
  }
}

void PndEmcMultiWaveform::SetWaveform(const std::vector<Double_t> &signal, Int_t, Int_t Waveform)
{ // length   //[R.K.03/2017] unused variable(s)
  if (Waveform < 0) {
    return;
  }
  if (Waveform >= (int)fSignals.size()) {
    fSignals.insert(fSignals.end(), Waveform - fSignals.size() + 1, std::vector<Double_t>(fWaveformLength, 0.));
  }
  fSignals[Waveform] = signal;
}

ClassImp(PndEmcMultiWaveform);
