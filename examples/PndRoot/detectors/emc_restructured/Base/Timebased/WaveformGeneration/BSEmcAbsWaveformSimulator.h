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

#ifndef BSEMCABSWAVEFORMSIMULATOR_HH
#define BSEMCABSWAVEFORMSIMULATOR_HH

#include "TClonesArray.h"
#include "TList.h"
#include "TMath.h"
#include "TObject.h"
#include "TROOT.h"

#include "BSEmcAbsWaveformModifier.h"

class TClonesArray;
class BSEmcWaveform;
class BSEmcWaveformData;

/**
 * @brief Abstract base class for waveform simulator
 *
 * @author Ph. Mahlberg <mahlberg@hiskp.uni-bonn.de>
 * @ingroup EmcBase
 */
class BSEmcAbsWaveformSimulator : public TObject {
 public:
  /*! Simulate Waveform based on wfData object
   *
   * The BSEmcWaveformData object is used by the emc timebased simulation */
  virtual BSEmcWaveform *Simulate(BSEmcWaveformData *t_wfData, TClonesArray *t_arrayToStore = nullptr) { return CallModifiers(MakeWaveform(t_wfData, t_arrayToStore)); }

  /*! Simulate waveform consisting of single pusle*/
  virtual BSEmcWaveform *
  SimulateSinglePulse(Double_t t_depositEnergy, Double_t t_depositTime, TClonesArray *t_arrayToStore = nullptr, Int_t t_detId = -1, Int_t t_trackId = -1, Int_t t_depositIndex = -1)
  {
    return CallModifiers(MakeSingleWaveform(t_depositEnergy, t_depositTime, t_arrayToStore, t_detId, t_trackId, t_depositIndex));
  }

  /*! returns 1GeV equivalent waveform
   *
   * might be used for calibration etc. */
  virtual BSEmcWaveform *Get1GevWaveform() = 0;

  /*! returns absolute livetime interval of waveform
   *
   * times in ns*/
  virtual void GetAbsoluteTimeInterval(BSEmcWaveformData *t_wfData, Double_t &t_startTime, Double_t &t_activeTime) = 0; // in ns

  /*! maximum timeinterval generated waveforms are ahead of causal detector deposits
   *
   * Information is needed by timebased simulation framework. E.g. provokeed by simulation of preceding baseline samples */
  virtual Double_t GetTimeBeforeFirstDeposit(BSEmcWaveformData * /*unused*/) { return 0; } // in ns  // wfData //[R.K.03/2017] unused variable(s)

  /*! Get 1GeV pulse height equivalient of simulator generated waveform*/
  virtual Double_t GetScale() = 0;

  /*! Get 1GeV pulse height equivalent of waveform at end of modification chain */
  virtual Double_t GetTotalScale()
  {
    Double_t totalScale = GetScale();

    TIter iterModifiers(&fListOfWaveformModifiers);
    while (BSEmcAbsWaveformModifier *wfmod = dynamic_cast<BSEmcAbsWaveformModifier *>(iterModifiers())) {
      totalScale *= wfmod->GetScale();
    }

    return totalScale;
  }

  virtual Double_t GetSampleRate() { return fSampleRate; }

  virtual void AddModifier(BSEmcAbsWaveformModifier *t_wfModifier) { fListOfWaveformModifiers.Add(t_wfModifier); }

  void SetMCDepositBranchName(const TString &t_branchname) { fMCDepositBranchName = t_branchname; }

 protected:
  BSEmcAbsWaveformSimulator() : fSampleRate(0){};
  BSEmcAbsWaveformSimulator(Double_t sampleRate) : fSampleRate(sampleRate){};

  virtual BSEmcWaveform *MakeWaveform(BSEmcWaveformData *t_wfData, TClonesArray *t_arrayToStore = nullptr) = 0;
  virtual BSEmcWaveform *MakeSingleWaveform(Double_t t_depositEnergy, Double_t t_depositTime, TClonesArray *t_arrayToStore = nullptr, Int_t t_detId = -1, Int_t t_trackId = -1,
                                            Int_t t_depositIndex = -1) = 0;

  /* sync adc clock with absolute time */
  inline void SyncWithADCClock(Double_t &t_time) { t_time = TMath::Floor(t_time * fSampleRate) / fSampleRate; };

  virtual BSEmcWaveform *CallModifiers(BSEmcWaveform *t_wf)
  {
    TIter iterModifiers(&fListOfWaveformModifiers);
    while (BSEmcAbsWaveformModifier *wfmod = dynamic_cast<BSEmcAbsWaveformModifier *>(iterModifiers())) {
      wfmod->Modify(t_wf);
    }
    return t_wf;
  }

  TList fListOfWaveformModifiers{};
  Double_t fSampleRate{-1}; //!< sampling rate of SADC. In 1/ns

  TString fMCDepositBranchName{""};
  ClassDef(BSEmcAbsWaveformSimulator, 3);
};

#endif /*BSEMCABSWAVEFORMSIMULATOR_HH*/
