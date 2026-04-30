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
//	PndMdtWaveform.cxx
//
// 	Class to hold waveforms created from Mdt Digis
//
//	 Software developed for the BaBar Detector at the SLAC B-Factory.
// Adapted for the PANDA experiment at GSI
//
// Jifeng Hu, hu@to.infn.it, Torino University
//-----------------------

//---------------
// C++ Headers --
//---------------

#include "PndMdtWaveform.h"
#include <iostream>
#include <algorithm>
#include "assert.h"

using std::cout;
using std::endl;

//----------------
// Constructors --
//----------------
PndMdtWaveform::PndMdtWaveform() : FairTimeStamp(), fTrackId(-1), fDetectorId(-1), fSignal(40, 0.) {}

PndMdtWaveform::PndMdtWaveform(Int_t trackId, Int_t detId, Double_t time, Bool_t isWire) : fTrackId(trackId), fDetectorId(detId), fIsWireSig(isWire), fSignal(40, 0.)
{
  SetTimeStamp(time);
}

PndMdtWaveform::PndMdtWaveform(const PndMdtWaveform &copy)
  : FairTimeStamp(copy), fTrackId(copy.fTrackId), fDetectorId(copy.fDetectorId), fIsWireSig(copy.fIsWireSig), fSignal(copy.fSignal)
{
  SetTimeStamp(copy.GetTimeStamp());
  SetTimeStampError(copy.GetTimeStampError());
}

PndMdtWaveform &PndMdtWaveform::operator=(const PndMdtWaveform &copy)
{
  if (this != &copy) {
    fTrackId = copy.fTrackId;
    fDetectorId = copy.fDetectorId;
    fSignal = copy.fSignal;
    SetTimeStamp(copy.GetTimeStamp());
    SetTimeStampError(copy.GetTimeStampError());
  }
  return *this;
}

//--------------
// Destructor --
//--------------

PndMdtWaveform::~PndMdtWaveform()
{
  fSignal.clear();
}

//-------------
// Selectors --
//-------------
bool PndMdtWaveform::operator<(const PndMdtWaveform &otherWave) const
{
  if (GetDetectorID() < otherWave.GetDetectorID())
    return true;
  if (GetDetectorID() > otherWave.GetDetectorID())
    return false;
  if (GetTimeStamp() < otherWave.GetTimeStamp())
    return GetActiveTime() < otherWave.GetTimeStamp();
  return false;
}
PndMdtWaveform &PndMdtWaveform::operator+=(const PndMdtWaveform &otherWave)
{
  if (this != &otherWave) {
    Int_t k = 0;
    Int_t IDX = 0;
    for (; (IDX < GetWaveformLength()) && (k < otherWave.GetWaveformLength()); ++IDX) {
      if ((GetTimeStamp() + IDX * 10.) < otherWave.GetTimeStamp()) {
        continue;
      }
      fSignal[IDX] += otherWave.fSignal[k];
      ++k;
    }
    if (k < otherWave.GetWaveformLength()) {
      fSignal.reserve(GetWaveformLength() + otherWave.GetWaveformLength() - (k + 1));
      for (; IDX < GetWaveformLength(); ++IDX, ++k) {
        fSignal.push_back(otherWave.fSignal[k]);
      }
    }
  }
  return *this;
}

TGraphErrors *PndMdtWaveform::ToTGraph() const
{
  // free this object outside
  TGraphErrors *g = new TGraphErrors(fSignal.size());
  for (size_t i = 0; i < fSignal.size(); ++i) {
    g->SetPoint(i, i * 10, fSignal[i]); // every 10 nano seconds
    g->SetPointError(i, 0, 0);
  }
  return g;
}
std::ostream &PndMdtWaveform::Print(std::ostream &out) const
{
  std::vector<Double_t>::const_iterator it = fSignal.begin();
  std::vector<Double_t>::const_iterator it_end = fSignal.end();
  cout << "PndMdtWaveform::" << fSignal.size() << " :[";
  for (; it != it_end; ++it)
    out << *it << ", ";
  out << "]" << endl;
  return out;
}
bool PndMdtWaveform::equal(FairTimeStamp *data) const
{
  PndMdtWaveform *other = (PndMdtWaveform *)(data);
  if (GetDetectorID() == other->GetDetectorID())
    return true;
  return false;
}
bool PndMdtWaveform::equal(FairTimeStamp *data)
{
  PndMdtWaveform *other = (PndMdtWaveform *)(data);
  if (GetDetectorID() == other->GetDetectorID())
    return true;
  return false;
}

ClassImp(PndMdtWaveform)
