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

//#pragma once
#ifndef PNDEMCFULLSTACKEDWAVEFORMSIMULATOR_H_
#define PNDEMCFULLSTACKEDWAVEFORMSIMULATOR_H_

#include "PndEmcAbsWaveformSimulator.h"
#include <vector>

class PndEmcWaveformData;
class PndEmcWaveform;
class PndEmcAbsPulseshape;
class TClonesArray;

#include <iostream>

/**
 * @brief Waveform simulator
 *
 * @author Ph. Mahlberg <mahlberg@hiskp.uni-bonn.de>
 * @ingroup PndEmc
 */
class PndEmcFullStackedWaveformSimulator : public PndEmcAbsWaveformSimulator {
 public:
  PndEmcFullStackedWaveformSimulator();
  PndEmcFullStackedWaveformSimulator(Double_t sampleRate, PndEmcAbsPulseshape *pulseShape);
  virtual ~PndEmcFullStackedWaveformSimulator();

  virtual void Init(Double_t samplingBeforeFirstPulse, Double_t samplingAfterLastPulse, Double_t cutoff, Double_t activeTimeIncrement);

  virtual void GetAbsoluteTimeInterval(PndEmcWaveformData *wfData, Double_t &startTime, Double_t &activeTime); // in ns

  virtual Double_t GetTimeBeforeFirstHit(PndEmcWaveformData *) { return fSamplingBeforeFirstPulse; } // in ns  //wfData //[R.K.03/2017] unused variable(s)

  virtual Double_t GetScale() { return fScale; }
  virtual PndEmcWaveform *Get1GevWaveform() { return f1GeVWaveform; };

  virtual Double_t GetPulseRiseTime() { return fPulseRiseTime; };

 protected:
  virtual PndEmcWaveform *MakeWaveform(PndEmcWaveformData *wfData, TClonesArray *arrayToStore = nullptr);
  virtual PndEmcWaveform *MakeSingleWaveform(Double_t hitEnergy, Double_t hitTime, TClonesArray *arrayToStore = nullptr, Int_t detId = -1, Int_t trackId = -1, Int_t hitIndex = -1);

  virtual Double_t CalcWaveForTime(Double_t absoluteTime, PndEmcWaveformData *wfData);
  Double_t CalcSingleWaveForTime(Double_t absoluteTime, Double_t energy, Double_t pulseTime);

 private:
  // don't allow copying (-Weffc++)
  PndEmcFullStackedWaveformSimulator(const PndEmcFullStackedWaveformSimulator &);            // no implementation
  PndEmcFullStackedWaveformSimulator &operator=(const PndEmcFullStackedWaveformSimulator &); // no implementation

 private:
  Double_t fSamplingBeforeFirstPulse; //!< in ns. additional time interval before waveform starts
  Double_t fSamplingAfterLastPulse;   //!< in ns. additional time interval after waveform falls below cutOff value
  Double_t fCutoff; //<! minimum pulse after which waveform simulation is terminated. Given as corresponding energy/GeV when cutoff value treated as pulse amplitude.

  PndEmcAbsPulseshape *fPulseshape;

  Double_t fScale; //!< Height of 1GeV pulse
  PndEmcWaveform *f1GeVWaveform;

  Double_t fPulseRiseTime; //!< total rising time of pulse in ns

  Double_t fActiveTimeIncrement;

  ClassDef(PndEmcFullStackedWaveformSimulator, 1);
};

#endif
