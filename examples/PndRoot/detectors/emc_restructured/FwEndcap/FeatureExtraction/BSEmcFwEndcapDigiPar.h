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
#ifndef BSEMCFWENDCAPDIGIPAR_HH
#define BSEMCFWENDCAPDIGIPAR_HH

#include <string>

#include "Rtypes.h"
#include "RtypesCore.h"
#include "TArrayD.h"
#include "TString.h"

#include "FairParGenericSet.h"
#include "FairParamList.h"

class FairParamList;
class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @brief parameter set for the FWEndcap variant of waveform simulation
 * @author Ph. Mahlberg <mahlberg@hiskp.uni-bonn.de>
 * @ingroup FwEndcap
 */
class BSEmcFwEndcapDigiPar : public FairParGenericSet {

 public:
  static const std::string fgParameterName;
  BSEmcFwEndcapDigiPar(const char *t_name = "BSEmcFwEndcapDigiPar", const char *t_title = "Emc forward endcap digi parameters", const char *t_context = "TestDefaultContext");
  virtual ~BSEmcFwEndcapDigiPar(void){};

  Int_t GetUseDigiEffectiveSmearingMode() { return fUse_photon_statistic; };
  Double_t GetQuantumEfficiency() { return fQuantumEfficiencyAPD; };
  Double_t GetExcessNoiseFactor() { return fExcessNoiseFactorAPD; };
  Double_t GetSensitiveArea() { return fSensitiveAreaAPD; };
  Double_t GetDetectedPhotonsPerMeV() { return fDetectedPhotonsPerMeV; };
  Double_t GetRearCrystalSurfaceArea() { return fRearCrystalSurfaceArea; }

  Int_t GetNBits() { return fNBits; };
  Double_t GetPulseshapeTau() { return fPulseshapeTau; };
  Double_t GetPulseshapeN() { return fPulseshapeN; };
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

  virtual void putParams(FairParamList *t_list) /*override*/;
  virtual Bool_t getParams(FairParamList *t_list) /*override*/;

 private:
  // Empty method, WHY is this Called in constructor(FIXME)
  virtual void clear(void) /*override*/ {};

  Int_t fUse_photon_statistic{0};
  Double_t fQuantumEfficiencyAPD{0};
  Double_t fExcessNoiseFactorAPD{0};
  Double_t fSensitiveAreaAPD{0};
  Double_t fRearCrystalSurfaceArea{0};
  Double_t fDetectedPhotonsPerMeV{0};

  Int_t fNBits{0};
  Double_t fPulseshapeTau{0};
  Double_t fPulseshapeN{0};

  Double_t fPulseshapev1{0};
  Double_t fPulseshapetcr{0};
  Double_t fPulseshapetcf{0};
  Double_t fPulseshapet_r{0};
  Double_t fPulseshapeRes{0};

  Double_t fEnergyRangeHigh{0};
  Double_t fEnergyRangeLow{0};
  Double_t fNoiseWidthHigh{0};
  Double_t fNoiseWidthLow{0};
  Double_t fEnergyDigiThreshold{0};

  Double_t fTimeBeforeHit{0};
  Double_t fTimeAfterHit{0};
  Double_t fWfCutOffEnergy{0};
  Double_t fSampleRate{0};

  TArrayD fPsaParLow{};
  TArrayD fPsaParHigh{};
  TString fPsaTypeLow{};
  TString fPsaTypeHigh{};
  TString fRValueParaLow{};
  TString fRValueParaHigh{};
  Double_t fSignalOverflowHigh{0};

  ClassDef(BSEmcFwEndcapDigiPar, 2);
};

#endif /*BSEMCFWENDCAPDIGIPAR_HH*/
