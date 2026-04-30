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
#ifndef BSEMCBWDIGIPAR_HH
#define BSEMCBWDIGIPAR_HH
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
 * @brief parameter set of Emc digitisation
 * @ingroup BwEndcap
 */
class BSEmcBwEndcapDigiPar : public FairParGenericSet {
 public:
  static std::string fgParameterName;
  BSEmcBwEndcapDigiPar(const char *name = "BSEmcBwEndcapDigiPar", const char *title = "Emc digi parameter", const char *context = "TestDefaultContext");
  virtual ~BSEmcBwEndcapDigiPar(void){};

  /* signal generator */
  Int_t GetUseDigiEffectiveSmearingMode() { return fUse_photon_statistic; }
  Double_t GetQuantumEfficiency() { return fQuantumEfficiencyAPD; }
  Double_t GetExcessNoiseFactor() { return fExcessNoiseFactorAPD; }
  Double_t GetSensitiveArea() { return fSensitiveAreaAPD; }
  Double_t GetDetectedPhotonsPerMeV() { return fDetectedPhotonsPerMeV; }
  Double_t GetRearCrystalSurfaceArea() { return fRearCrystalSurfaceArea; }
  Double_t GetTimeBeforeHit() { return fTimeBeforeHit; }
  Double_t GetTimeAfterHit() { return fTimeAfterHit; }
  Double_t GetActiveTimeIncrement() { return fActiveTimeIncrement; }
  Double_t GetWfCutOffEnergy() { return fWfCutOffEnergy; } // GeV  //0.001
  Double_t GetSampleRate() { return fSampleRate; }         // ns^-1 <-> GHz    //0.08
  Int_t GetNBits() { return fNBits; }                      // 1    //14
  Double_t GetEnCalib() { return fEnCalib; }               // GeV/channel
  Double_t GetPulseshapeTau() { return fPulseshapeTau; }   // ns   //68.7
  Double_t GetPulseshapeN() { return fPulseshapeN; }       // N    //1.667
  Double_t GetHGLGRatio() { return fHGLGRatio; }           // 10.5
  Double_t GetADCNoiseHigh() { return fADCNoiseHigh; }
  Double_t GetADCNoiseLow() { return fADCNoiseLow; }
  Double_t GetFENoiseHigh() { return fFENoiseHigh; }
  Double_t GetFENoiseLow() { return fFENoiseLow; }
  Double_t GetPsSigmaHigh() { return fPsSigmaHigh; }
  Double_t GetPsSigmaLow() { return fPsSigmaLow; }
  Double_t GetGesSigmaHigh() { return fGesSigmaHigh; }
  Double_t GetGesSigmaLow() { return fGesSigmaLow; }
  const TArrayD &GetPSFreq() { return fPSFreq; }
  const TArrayD &GetPSPowerHigh() { return fPSPowerHigh; }
  const TArrayD &GetPSPowerLow() { return fPSPowerLow; }

  /* feature extraction */
  Double_t GetSignalOverflowHigh() { return fSignalOverflowHigh; }
  const TArrayD &GetFIRCoeff() { return fFIRCoeff; }
  Int_t GetTmaxTaps() { return fTmaxTaps; }
  Int_t GetTmaxGap() { return fTmaxGap; }
  Double_t GetTmaxHitThresholdHigh() { return fTmaxHitThresholdHigh; }
  Double_t GetTmaxHitThresholdLow() { return fTmaxHitThresholdLow; }
  Double_t GetTmaxTutPeak() { return fTmaxTutPeak; }
  Double_t GetTmaxHitVal() { return fTmaxHitVal; }
  Double_t GetEnergyDigiThreshold() { return fEnergyDigiThreshold; }

  virtual void putParams(FairParamList *t_list) /*override*/;
  virtual Bool_t getParams(FairParamList *t_list) /*override*/;

 private:
  // Empty method, WHY is this Called in constructor(FIXME)
  // void clear(void){};

  Int_t fUse_photon_statistic{0};
  Double_t fQuantumEfficiencyAPD{0};
  Double_t fExcessNoiseFactorAPD{0};
  Double_t fSensitiveAreaAPD{0};
  Double_t fRearCrystalSurfaceArea{0};
  Double_t fDetectedPhotonsPerMeV{0};

  Double_t fTimeBeforeHit{0};
  Double_t fTimeAfterHit{0};
  Double_t fActiveTimeIncrement{0};
  Double_t fWfCutOffEnergy{0};
  Double_t fSampleRate{0};
  Int_t fNBits{0};
  Double_t fEnCalib{0};
  Double_t fPulseshapeTau{0};
  Double_t fPulseshapeN{0};
  Double_t fHGLGRatio{0};
  Double_t fADCNoiseHigh{0};
  Double_t fADCNoiseLow{0};
  Double_t fFENoiseHigh{0};
  Double_t fFENoiseLow{0};
  Double_t fPsSigmaHigh{0};
  Double_t fPsSigmaLow{0};
  Double_t fGesSigmaHigh{0};
  Double_t fGesSigmaLow{0};
  TArrayD fPSFreq{};
  TArrayD fPSPowerHigh{};
  TArrayD fPSPowerLow{};

  Double_t fSignalOverflowHigh{0};
  TArrayD fFIRCoeff{};
  Int_t fTmaxTaps{0};
  Int_t fTmaxGap{0};
  Double_t fTmaxHitThresholdHigh{0};
  Double_t fTmaxHitThresholdLow{0};
  Double_t fTmaxTutPeak{0};
  Double_t fTmaxHitVal{0};
  Double_t fEnergyDigiThreshold{0};

  ClassDef(BSEmcBwEndcapDigiPar, 1)
};

#endif /*BSEMCBWDIGIPAR_HH*/
