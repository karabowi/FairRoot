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

#pragma once

#ifndef PNDEMCABSWAVEFORMSIMULATOR_H_
#define PNDEMCABSWAVEFORMSIMULATOR_H_

#include "TObject.h"
#include "TMath.h"
#include "TList.h"
#include "PndEmcAbsWaveformModifier.h"

class TClonesArray;
class PndEmcWaveform;
class PndEmcWaveformData;

#include "TROOT.h"
#include "TClonesArray.h"

/**
 * @brief Abstract base class for waveform simulator
 *
 * @author Ph. Mahlberg <mahlberg@hiskp.uni-bonn.de>
 * @ingroup PndEmc
 */
class PndEmcAbsWaveformSimulator : public TObject {
 public:
  /*! Simulate Waveform based on wfData object
   *
   * The PndEmcWaveformData object is used by the emc timebased simulation */
  virtual PndEmcWaveform *Simulate(PndEmcWaveformData *wfData, TClonesArray *arrayToStore = nullptr) { return CallModifiers(MakeWaveform(wfData, arrayToStore)); }

  /*! Simulate waveform consisting of single pusle*/
  virtual PndEmcWaveform *SimulateSinglePulse(Double_t hitEnergy, Double_t hitTime, TClonesArray *arrayToStore = nullptr, Int_t detId = -1, Int_t trackId = -1, Int_t hitIndex = -1)
  {
    return CallModifiers(MakeSingleWaveform(hitEnergy, hitTime, arrayToStore, detId, trackId, hitIndex));
  }

  /*! returns 1GeV equivalent waveform
   *
   * might be used for calibration etc. */
  virtual PndEmcWaveform *Get1GevWaveform() = 0;

  /*! returns absolute livetime interval of waveform
   *
   * times in ns*/
  virtual void GetAbsoluteTimeInterval(PndEmcWaveformData *wfData, Double_t &startTime, Double_t &activeTime) = 0; // in ns

  /*! maximum timeinterval generated waveforms are ahead of causal detector hits
   *
   * Information is needed by timebased simulation framework. E.g. provokeed by simulation of preceding baseline samples */
  virtual Double_t GetTimeBeforeFirstHit(PndEmcWaveformData *) { return 0; } // in ns  // wfData //[R.K.03/2017] unused variable(s)

  /*! Get 1GeV pulse height equivalient of simulator generated waveform*/
  virtual Double_t GetScale() = 0;

  /*! Get 1GeV pulse height equivalent of waveform at end of modification chain */
  virtual Double_t GetTotalScale()
  {
    Double_t totalScale = GetScale();

    TIter iterModifiers(&fListOfWaveformModifiers);
    while (PndEmcAbsWaveformModifier *wfmod = (PndEmcAbsWaveformModifier *)iterModifiers()) {
      totalScale *= wfmod->GetScale();
    }

    return totalScale;
  }

  virtual Double_t GetSampleRate() { return fSampleRate; }

  virtual void AddModifier(PndEmcAbsWaveformModifier *wfModifier) { fListOfWaveformModifiers.Add(wfModifier); }

 protected:
  PndEmcAbsWaveformSimulator() : fSampleRate(0){};
  PndEmcAbsWaveformSimulator(Double_t sampleRate) : fSampleRate(sampleRate){};

  virtual PndEmcWaveform *MakeWaveform(PndEmcWaveformData *wfData, TClonesArray *arrayToStore = nullptr) = 0;
  virtual PndEmcWaveform *
  MakeSingleWaveform(Double_t hitEnergy, Double_t hitTime, TClonesArray *arrayToStore = nullptr, Int_t detId = -1, Int_t trackId = -1, Int_t hitIndex = -1) = 0;

  /* sync adc clock with absolute time */
  inline void SyncWithADCClock(Double_t &time) { time = TMath::Floor(time * fSampleRate) / fSampleRate; };
 
  virtual PndEmcWaveform *CallModifiers(PndEmcWaveform *wf)
  {
    TIter iterModifiers(&fListOfWaveformModifiers);
    while (PndEmcAbsWaveformModifier *wfmod = (PndEmcAbsWaveformModifier *)iterModifiers()) {
      wfmod->Modify(wf);
    }
    return wf;
  }

  TList fListOfWaveformModifiers;
  Double_t fSampleRate; //!< sampling rate of SADC. In 1/ns

  ClassDef(PndEmcAbsWaveformSimulator, 3);
};

#endif
