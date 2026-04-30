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

//==========================================================================
//	PndMdtWaveform.h
//
// 	Class to hold waveforms created from Mdt Digis
//
//	Hits will be converted to waveforms from semi-parameterized simulation
//
//	 Software developed for the BaBar Detector at the SLAC B-Factory.
// Adapted for the PANDA experiment at GSI
//
// Jifeng Hu, hu@to.infn.it, Torino University
//=======================================================================
//#pragma once
#ifndef PNDMDTWAVEFORM_H
#define PNDMDTWAVEFORM_H

#include <vector>
#include "TObject.h"
#include "FairTimeStamp.h"
#include <algorithm>
#include "TGraphErrors.h"

class PndMdtWaveform : public FairTimeStamp {

 public:
  //  Constructors
  PndMdtWaveform();
  PndMdtWaveform(Int_t trackId, Int_t detId, Double_t time, Bool_t isWire = kTRUE);
  // Destructor:
  virtual ~PndMdtWaveform();
  // Copy:
  PndMdtWaveform(const PndMdtWaveform &copy);
  // Operators

  virtual PndMdtWaveform &operator=(const PndMdtWaveform &copy);
  virtual bool operator<(const PndMdtWaveform &otherWave) const;
  PndMdtWaveform &operator+=(const PndMdtWaveform &otherWave);
  virtual bool equal(FairTimeStamp *data) const;
  virtual bool equal(FairTimeStamp *data);
  // Selectors
  Bool_t IsWire() const { return fIsWireSig; }
  Int_t GetDetectorID() const { return fDetectorId; }
  Int_t GetTrackId() const { return fTrackId; }
  Long_t GetWaveformLength() const { return fSignal.size(); }
  Double_t GetActiveTime() const { return GetTimeStamp() + 10 * GetWaveformLength(); } // nano seconds

  TGraphErrors *ToTGraph() const;

  const std::vector<Double_t> &GetSignal() const { return fSignal; }
  // Modifiers
  void SetSignal(const std::vector<Double_t> &v) { copy(v.begin(), v.end(), fSignal.begin()); }

  virtual std::ostream &Print(std::ostream &out = std::cout) const;

 protected:
  Int_t fTrackId;
  Int_t fDetectorId;
  Bool_t fIsWireSig;             // wire or strip
  std::vector<Double_t> fSignal; // induced current

  ClassDef(PndMdtWaveform, 5)
};
#endif
