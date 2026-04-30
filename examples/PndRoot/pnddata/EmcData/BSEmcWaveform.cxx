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

//=====================================================================
//	BSEmcWaveform.cxx
//
// 	Class to hold waveforms created from Emc Hits
//
//	 Software developed for the BaBar Detector at the SLAC B-Factory.
// Adapted for the PANDA experiment at GSI
//
// 	P.D.Strother 	Imperial College
//      Naveen Gunawardane  Imperial College
// Dima Melnichuk - adaption for PANDA
//-----------------------

#include "BSEmcWaveform.h"

#include <FairTimeStamp.h>
#include <algorithm>
#include <math.h>
#include <stddef.h>

#include "TGraphErrors.h"

#include "fairlogger/Logger.h"

BSEmcWaveform::BSEmcWaveform()
  : FairTimeStamp(), fTrackId(-1), fDetectorId(-1), fWaveformLength(0), fSignal(0, 0.), fSignalError(0, 0.), fHitIndex(-1), fSampleRate(0.), fBaselineValue(0.)
{
}

BSEmcWaveform::BSEmcWaveform(Int_t t_trackId, long t_detId, const std::vector<Double_t> &t_signal, Int_t t_hitIndex)
  : fTrackId(t_trackId), fDetectorId(t_detId), fWaveformLength(t_signal.size()), fSignal(t_signal), fHitIndex(t_hitIndex)
{
}

BSEmcWaveform::~BSEmcWaveform()
{
  fSignal.clear();
  fSignalError.clear();
  fEvt.clear();
}

Double_t BSEmcWaveform::Max()
{
  Double_t max = *max_element(fSignal.begin(), fSignal.end());
  return max;
}

void BSEmcWaveform::clearAndReset()
{
  // reset values of fSignal to 0
  fill(fSignal.begin(), fSignal.end(), 0);
}

Bool_t BSEmcWaveform::operator==(const BSEmcWaveform &t_otherWave) const
{
  if (fDetectorId != t_otherWave.fDetectorId) {
    return kFALSE;
  }
  return kTRUE;
}

Bool_t BSEmcWaveform::operator<(const BSEmcWaveform &t_otherWave) const
{
  if (GetDetectorId() < t_otherWave.GetDetectorId()) {
    return kTRUE;
  } else if (GetDetectorId() == t_otherWave.GetDetectorId()) { // FIXME: Can we have two waveforms in the same crystal within the same Event?
    if (GetTimeStamp() < t_otherWave.GetTimeStamp()) {
      return t_otherWave.GetTimeStamp() > GetActiveTime() - fOverlapTime;
    } else {
      return GetTimeStamp() > t_otherWave.GetActiveTime() - fOverlapTime;
    }
  } else {
    return kFALSE;
  }
}

Bool_t BSEmcWaveform::operator!=(const BSEmcWaveform &t_otherWave) const
{
  if (fDetectorId != t_otherWave.fDetectorId) {
    return kTRUE;
  }
  return kFALSE;
}

Bool_t BSEmcWaveform::equal(FairTimeStamp *t_data)
{
  BSEmcWaveform *other = dynamic_cast<BSEmcWaveform *>(t_data);
  if (GetDetectorId() == other->GetDetectorId()) {
    return kTRUE;
  }
  return kFALSE;
}

BSEmcWaveform &BSEmcWaveform::operator+=(const BSEmcWaveform &t_otherWave)
{
  if (GetTimeStamp() > t_otherWave.GetTimeStamp()) // current wave earlier
  {
    LOG(warn) << "BSEmcWaveform::operator+= - Please make sure the eariler waveform += the later waveform";
    return *this;
  }
  //++ fPileupCount ;

  const std::vector<Int_t> &evtList = t_otherWave.GetEvtList();
  for (int i : evtList) {
    AddEvt(i);
  }

  Int_t k = 0;
  Int_t IDX = 0;
  for (; (IDX < fWaveformLength) && (k < t_otherWave.fWaveformLength); ++IDX) {
    if ((GetTimeStamp() + IDX / fSampleRate * 1.0e9) < t_otherWave.GetTimeStamp()) {
      continue;
    }
    fSignal[IDX] += t_otherWave.fSignal[k];
    fSignalError[IDX] = sqrt(fSignalError[IDX] * fSignalError[IDX] + t_otherWave.fSignalError[k] * t_otherWave.fSignalError[k]);
    ++k;
  }
  if (k < t_otherWave.fWaveformLength) {
    fWaveformLength += t_otherWave.fWaveformLength - k;
    for (; IDX < fWaveformLength; ++IDX, ++k) {
      fSignal.push_back(t_otherWave.fSignal[k]);
      fSignalError.push_back(t_otherWave.fSignalError[k]);
    }
  }

  return *this;
}

TGraphErrors *BSEmcWaveform::ToTGraph() const
{

  // free this object outside
  TGraphErrors *g = new TGraphErrors(fSignal.size());
  for (size_t i = 0; i < fSignal.size(); ++i) {
    g->SetPoint(i, GetTimeStamp() / 1.e9 + Double_t(i) / fSampleRate, fSignal[i]);
    g->SetPointError(i, 0, fSignalError[i]);
  }
  return g;
}

void BSEmcWaveform::SetWaveform(std::vector<Double_t> &t_signal, Int_t t_length)
{
  fSignal = t_signal;
  fWaveformLength = t_length;
}

Double_t BSEmcWaveform::Integral() const
{
  Double_t sum(0.);
  for (double i : fSignal) {
    sum += i;
  }
  return sum;
}

ClassImp(BSEmcWaveform)
