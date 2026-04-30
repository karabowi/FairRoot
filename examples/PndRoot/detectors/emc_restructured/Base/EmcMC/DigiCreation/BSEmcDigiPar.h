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

#ifndef BSEMCDIGIPAR_HH
#define BSEMCDIGIPAR_HH
#include <string>

#include "Rtypes.h"
#include "RtypesCore.h"

#include "FairParGenericSet.h"

class FairParamList;
class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @class BSEmcDigiPar
 * @brief Container for runtime parameters that are required for the
 * transformation from BSEmcMCDeposits to BSEmcDigis
 * @details Based on PndEmcDigiPar
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-01
 * @ingroup EmcBase
 */
class BSEmcDigiPar : public FairParGenericSet {
 public:
  static const std::string fgParameterName;
  BSEmcDigiPar(const char *t_name = "EmcDigiPar", const char *t_title = "Emc Geometry Version Flag", const char *t_context = "TestDefaultContext");
  ~BSEmcDigiPar(void);
  void clear(void) /*override*/;

  Int_t GetUseDigiEffectiveSmearingMode() const { return fUseDigiEffectiveSmearing; }
  Double_t GetEnergyHitThreshold() const { return fEnergyHitThreshold; }
  Double_t GetDetectedPhotonsPerMeV() const { return fDetectedPhotonsPerMeV; }
  Double_t GetSensitiveArea() const { return fSensitiveArea; }
  Double_t GetQuantumEfficiency() const { return fQuantumEfficiency; }
  Double_t GetExcessNoiseFactor() const { return fExcessNoiseFactor; }
  Double_t GetIncoherentElecNoiseWidthGeV() const { return fIncoherentElecNoiseWidthGeV; }
  Double_t GetRearCrystalSurfaceArea() const { return fRearCrystalSurfaceArea; }
  Double_t GetNPhotoElectronsPerMeV() const { return fDetectedPhotonsPerMeV * fSensitiveArea / fRearCrystalSurfaceArea * fQuantumEfficiency; }
  Double_t GetEnergyDigiThreshold() const { return fEnergyDigiThreshold; }
  Double_t GetOverlapTime() const { return fOverlapTime; }
  void putParams(FairParamList * /*unused*/) /*override*/;
  Bool_t getParams(FairParamList * /*unused*/) /*override*/;

 private:
  Int_t fUseDigiEffectiveSmearing{-1};
  Double_t fEnergyHitThreshold{-1};
  Double_t fDetectedPhotonsPerMeV{-1};
  Double_t fSensitiveArea{-1}; // mm^2
  Double_t fQuantumEfficiency{-1};
  Double_t fExcessNoiseFactor{-1};
  Double_t fIncoherentElecNoiseWidthGeV{-1}; // GeV
  Double_t fEnergyDigiThreshold{-1};
  Double_t fRearCrystalSurfaceArea{-1}; // These is not set and needs to be set manually;
  Double_t fNPhotoElectronsPerMeV{-1};
  Double_t fOverlapTime{-1};
  ClassDef(BSEmcDigiPar, 3)
};

#endif /*BSEMCDIGIPAR_HH*/
