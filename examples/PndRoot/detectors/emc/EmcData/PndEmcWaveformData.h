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
#ifndef PNDEMCWAVEFORMDATA_H
#define PNDEMCWAVEFORMDATA_H

#include "FairTimeStamp.h"
#include "FairLink.h"
#include "PndEmcAbsWaveformSimulator.h"

#include "TRef.h"
#include "TObject.h"

#include <ostream>
#include <map>

class PndEmcWaveform;

/**
 * @brief represents a simulated waveform in an emc crystal, used by PndEmcFWEndcapTimebasedWaveforms
 * @author Ph. Mahlberg <mahlberg@hiskp.uni-bonn.de>
 * @ingroup PndEmc
 */
class PndEmcWaveformData : public FairTimeStamp {

 public:
  PndEmcWaveformData();
  PndEmcWaveformData(Int_t detId, PndEmcAbsWaveformSimulator *wfSimulator);

  virtual ~PndEmcWaveformData(){};

  virtual bool operator<(const PndEmcWaveformData &wfData) const;
  friend std::ostream &operator<<(std::ostream &out, PndEmcWaveformData &wfData);
  virtual PndEmcWaveformData &operator+=(const PndEmcWaveformData &toAdd);

  virtual bool equal(FairTimeStamp *data);

  virtual void AddHit(const FairLink &linkToHit, Double_t absEventTime, Double_t energy);

  Int_t GetDetectorId() { return fDetectorId; };
  Int_t GetNHits() { return fHitMap.size(); };
  const std::map<Double_t, Double_t> &GetHitMap() { return fHitMap; };
  PndEmcAbsWaveformSimulator *GetWaveformSimulator() { return dynamic_cast<PndEmcAbsWaveformSimulator *>(fWfSimulator.GetObject()); };
  Double_t GetTimeOfLastSample() { return fTimeOfLastSample; };
  void GetHitParameter(Int_t hitNo, Double_t &time, Double_t &energy);

  void SetWaveformSimulator(PndEmcAbsWaveformSimulator *wfSimulator) { fWfSimulator.SetObject(wfSimulator); };
  void SetTimeOfLastSample(Double_t time) { fTimeOfLastSample = time; };

 protected:
  Int_t fDetectorId;
  Double_t fTimeOfLastSample;

  TRef fWfSimulator;
  std::map<Double_t, Double_t> fHitMap;

  ClassDef(PndEmcWaveformData, 1)
};

#endif
