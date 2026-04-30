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
#ifndef PNDEMCFWENDCAPDIGIPAR_H
#define PNDEMCFWENDCAPDIGIPAR_H

#include <iostream>

#include "FairParGenericSet.h"
#include "FairParamList.h"
#include "TArrayD.h"

/**
 * @brief parameter set for the FWEndcap variant of waveform simulation
 * @author Ph. Mahlberg <mahlberg@hiskp.uni-bonn.de>
 * @ingroup PndEmc
 */
class PndEmcFWEndcapDigiPar : public FairParGenericSet {

 public:
  PndEmcFWEndcapDigiPar(const char *name = "PndEmcFWEndcapDigiPar", const char *title = "Emc forward endcap digi parameters", const char *context = "TestDefaultContext");
  ~PndEmcFWEndcapDigiPar(void){};

  Int_t GetUse_photon_statistic() { return fUse_photon_statistic; };
  Double_t GetQuantumEfficiencyAPD() { return fQuantumEfficiencyAPD; };
  Double_t GetExcessNoiseFactorAPD() { return fExcessNoiseFactorAPD; };
  Double_t GetSensitiveAreaAPD() { return fSensitiveAreaAPD; };
  Double_t GetDetectedPhotonsPerMeV() { return fDetectedPhotonsPerMeV; };

  Int_t GetNBits() { return fNBits; };
  Double_t GetPulseshapeTau() { return fPulseshapeTau; };
  Double_t GetPulseshapeN() { return fPulseshapeN; };
  Double_t GetPulseshapeTau2() { return fPulseshapeTau2; };
  Double_t GetPulseshapeN2() { return fPulseshapeN2; };
  Double_t GetPulseshapev1() { return fPulseshapev1; };
  Double_t GetPulseshapetcr() { return fPulseshapetcr; };
  Double_t GetPulseshapetcf() { return fPulseshapetcf; };
  Double_t GetPulseshapet_r() { return fPulseshapet_r; };
  Double_t GetPulseshapeRes() { return fPulseshapeRes; };
  Double_t GetEnergyRangeHigh() { return fEnergyRangeHigh; };
  Double_t GetEnergyRangeLow() { return fEnergyRangeLow; };
  Double_t GetNoiseWidthHigh() { return fNoiseWidthHigh; };
  Double_t GetNoiseWidthLow() { return fNoiseWidthLow; };
  Double_t GetEnergyDigiThreshold() { return fEnergyDigiThreshold; };
  Double_t GetCalibHigh() { return fCalibHigh; };
  Double_t GetCalibLow() { return fCalibLow; };

  Double_t GetWfCutOffEnergy() { return fWfCutOffEnergy; };
  Double_t GetTimeBeforeHit() { return fTimeBeforeHit; };
  Double_t GetTimeAfterHit() { return fTimeAfterHit; };
  Double_t GetSampleRate() { return fSampleRate; };

  const TArrayD &GetPsaParLow() { return fPsaParLow; }
  const TArrayD &GetPsaParHigh() { return fPsaParHigh; }
  const TString &GetPsaTypeLow() { return fPsaTypeLow; }
  const TString &GetPsaTypeHigh() { return fPsaTypeHigh; }
  const TString &GetRValueParLow() { return fRValueParaLow; }
  const TString &GetRValueParHigh() { return fRValueParaHigh; }
  Double_t GetSignalOverflowHigh() { return fSignalOverflowHigh; }

  virtual void putParams(FairParamList *list);
  virtual Bool_t getParams(FairParamList *list);

 private:
  // Empty method, WHY is this Called in constructor(FIXME)
  void clear(void){};

  Int_t fUse_photon_statistic;
  Double_t fQuantumEfficiencyAPD;
  Double_t fExcessNoiseFactorAPD;
  Double_t fSensitiveAreaAPD;
  Double_t fDetectedPhotonsPerMeV;

  Int_t fNBits;
  Double_t fPulseshapeTau;
  Double_t fPulseshapeN;
  Double_t fPulseshapeTau2;
  Double_t fPulseshapeN2;
  Double_t fPulseshapev1;
  Double_t fPulseshapetcr;
  Double_t fPulseshapetcf;
  Double_t fPulseshapet_r;
  Double_t fPulseshapeRes;
  Double_t fEnergyRangeHigh;
  Double_t fEnergyRangeLow;
  Double_t fNoiseWidthHigh;
  Double_t fNoiseWidthLow;
  Double_t fEnergyDigiThreshold;
  Double_t fCalibHigh;
  Double_t fCalibLow;

  Double_t fTimeBeforeHit;
  Double_t fTimeAfterHit;
  Double_t fWfCutOffEnergy;
  Double_t fSampleRate;

  TArrayD fPsaParLow;
  TArrayD fPsaParHigh;
  TString fPsaTypeLow;
  TString fPsaTypeHigh;
  TString fRValueParaLow;
  TString fRValueParaHigh;
  Double_t fSignalOverflowHigh;

  ClassDef(PndEmcFWEndcapDigiPar, 2);
};

#endif
