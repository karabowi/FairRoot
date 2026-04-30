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
#ifndef BSEMCWAVEFORMDATA_HH
#define BSEMCWAVEFORMDATA_HH

#include <map>
#include <ostream>
#include <string>

#include "Rtypes.h"
#include "RtypesCore.h"
#include "TObject.h"
#include "TRef.h"

#include "FairLink.h"
#include "FairTimeStamp.h"

#include "BSEmcAbsWaveformSimulator.h"

class BSEmcWaveform;
class FairLink;
class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @brief represents a simulated waveform in an emc crystal, used by BSEmcFwEndcapTimebasedWaveforms
 * @author Ph. Mahlberg <mahlberg@hiskp.uni-bonn.de>
 * @ingroup EmcBase
 */
class BSEmcWaveformData : public FairTimeStamp {
  friend std::ostream &operator<<(std::ostream &t_out, BSEmcWaveformData &t_wfData)
  {
    t_out << "waveform links to " << t_wfData.fDepMap.size() << " BSEmcMCDeps:" << std::endl;

    Int_t count = 0;
    for (auto &it : t_wfData.fDepMap) {
      t_out << "\t#" << count++ << "\t time: " << it.first << "\t energy: " << it.second << std::endl;
    }

    return t_out;
  }

 public:
  BSEmcWaveformData();
  BSEmcWaveformData(Int_t t_detId, BSEmcAbsWaveformSimulator *t_wfSimulator);

  virtual ~BSEmcWaveformData(){};

  virtual Bool_t operator<(const BSEmcWaveformData &t_wfData) const;

  virtual BSEmcWaveformData &operator+=(const BSEmcWaveformData &t_toAdd);

  virtual Bool_t equal(FairTimeStamp *t_data) /*override*/;

  virtual void AddDeposit(const FairLink &t_linkToDeposit, Double_t t_absEventTime, Double_t t_energy);

  Int_t GetDetectorId() { return fDetectorId; };
  Int_t GetNDeposits() { return fDepMap.size(); };
  const std::map<Double_t, Double_t> &GetDepositMap() { return fDepMap; };
  BSEmcAbsWaveformSimulator *GetWaveformSimulator() { return dynamic_cast<BSEmcAbsWaveformSimulator *>(fWfSimulator.GetObject()); };
  Double_t GetTimeOfLastSample() { return fTimeOfLastSample; };
  void GetDepositParameter(Int_t t_depositNo, Double_t &t_time, Double_t &t_energy);

  void SetWaveformSimulator(BSEmcAbsWaveformSimulator *t_wfSimulator) { fWfSimulator.SetObject(t_wfSimulator); };
  void SetTimeOfLastSample(Double_t t_time) { fTimeOfLastSample = t_time; };
  void SetOverlapTime(const Double_t t_overlap) { fOverlapTime = t_overlap; }
  Double_t GetOverlapTime() const { return fOverlapTime; }

 protected:
  Int_t fDetectorId{-1};
  Double_t fTimeOfLastSample{-1};
  Double_t fOverlapTime{-1};

  TRef fWfSimulator{};
  std::map<Double_t, Double_t> fDepMap{};

  ClassDef(BSEmcWaveformData, 1)
};

#endif /*BSEMCWAVEFORMDATA_HH*/
