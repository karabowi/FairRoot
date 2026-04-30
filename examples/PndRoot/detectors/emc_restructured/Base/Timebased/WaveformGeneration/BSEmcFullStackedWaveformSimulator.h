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
#ifndef BSEMCFULLSTACKEDWAVEFORMSIMULATOR_HH
#define BSEMCFULLSTACKEDWAVEFORMSIMULATOR_HH

#include <iostream>
#include <vector>

#include "Rtypes.h"
#include "RtypesCore.h"

#include "BSEmcAbsWaveformSimulator.h"

class BSEmcWaveformData;
class BSEmcWaveform;
class BSEmcAbsPulseshape;
class TClonesArray;
class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @brief Waveform simulator
 *
 * @author Ph. Mahlberg <mahlberg@hiskp.uni-bonn.de>
 * @ingroup EmcBase
 */
class BSEmcFullStackedWaveformSimulator : public BSEmcAbsWaveformSimulator {
 public:
  BSEmcFullStackedWaveformSimulator();
  BSEmcFullStackedWaveformSimulator(Double_t t_sampleRate, BSEmcAbsPulseshape *t_pulseShape);
  virtual ~BSEmcFullStackedWaveformSimulator();

  virtual void Init(Double_t t_samplingBeforeFirstPulse, Double_t t_samplingAfterLastPulse, Double_t t_cutoff, Double_t t_activeTimeIncrement);

  virtual void GetAbsoluteTimeInterval(BSEmcWaveformData *t_wfData, Double_t &t_startTime, Double_t &t_activeTime) /*override*/; // in ns

  virtual Double_t GetTimeBeforeFirstHit(BSEmcWaveformData * /*unused*/) /*override*/ { return fSamplingBeforeFirstPulse; } // in ns  //wfData //[R.K.03/2017] unused variable(s)

  virtual Double_t GetScale() /*override*/ { return fScale; }
  virtual BSEmcWaveform *Get1GevWaveform() /*override*/ { return f1GeVWaveform; };

  virtual Double_t GetPulseRiseTime() { return fPulseRiseTime; };

 protected:
  virtual BSEmcWaveform *MakeWaveform(BSEmcWaveformData *t_wfData, TClonesArray *t_arrayToStore = nullptr);
  virtual BSEmcWaveform *
  MakeSingleWaveform(Double_t t_depositEnergy, Double_t t_depositTime, TClonesArray *t_arrayToStore = nullptr, Int_t t_detId = -1, Int_t t_trackId = -1, Int_t t_depositIndex = -1);

  virtual Double_t CalcWaveForTime(Double_t t_absoluteTime, BSEmcWaveformData *t_wfData);
  Double_t CalcSingleWaveForTime(Double_t t_absoluteTime, Double_t t_energy, Double_t t_pulseTime);

 private:
  // don't allow copying (-Weffc++)
  BSEmcFullStackedWaveformSimulator(const BSEmcFullStackedWaveformSimulator &);            // no implementation
  BSEmcFullStackedWaveformSimulator &operator=(const BSEmcFullStackedWaveformSimulator &); // no implementation

 private:
  Double_t fSamplingBeforeFirstPulse{-1}; //!< in ns. additional time interval before waveform starts
  Double_t fSamplingAfterLastPulse{-1};   //!< in ns. additional time interval after waveform falls below cutOff value
  Double_t fCutoff{-1}; //<! minimum pulse after which waveform simulation is terminated. Given as corresponding energy/GeV when cutoff value treated as pulse amplitude.

  BSEmcAbsPulseshape *fPulseshape{nullptr};

  Double_t fScale{-1}; //!< Height of 1GeV pulse
  BSEmcWaveform *f1GeVWaveform{nullptr};

  Double_t fPulseRiseTime{-1}; //!< total rising time of pulse in ns

  Double_t fActiveTimeIncrement{-1};

  ClassDef(BSEmcFullStackedWaveformSimulator, 1);
};

#endif /*BSEMCFULLSTACKEDWAVEFORMSIMULATOR_HH*/
