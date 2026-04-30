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
#ifndef PNDEMCFpgaPAR_H
#define PNDEMCFpgaPAR_H

#include "FairParGenericSet.h"
#include "FairParamList.h"

class PndEmcFpgaPar : public FairParGenericSet {
 public:
  Int_t GetBarrelMAFilterLength() { return fMAFilterLengthBarrel; }
  Int_t GetBarrelCFFilterLength() { return fCFFilterLengthBarrel; }
  Double_t GetBarrelCFFilterRatio() { return fCFFilterRatioBarrel; }
  Int_t GetBarrelCFDelayLength() { return fCFDelayLengthBarrel; }
  Int_t GetBarrelCFFitterLength() { return fCFFitterLengthBarrel; }
  Int_t GetBarrelMWDFilterUsed() { return fMWDFilterUsedBarrel; }
  Int_t GetBarrelMWDFilterLength() { return fMWDFilterLengthBarrel; }
  Double_t GetBarrelMWDFilterLifeT() { return fMWDDecayConstantBarrel; } // unit relative to ns
  Double_t GetBarrelPulseThreshold() { return fPulseThresholdBarrel; }   // times over 1MeV noise level

  Int_t GetForwardMAFilterLength() { return fMAFilterLengthForward; };
  Int_t GetForwardCFFilterLength() { return fCFFilterLengthForward; }
  Double_t GetForwardCFFilterRatio() { return fCFFilterRatioForward; }
  Int_t GetForwardCFFitterLength() { return fCFFitterLengthForward; }
  Int_t GetForwardCFDelayLength() { return fCFDelayLengthForward; }
  Int_t GetForwardMWDFilterUsed() { return fMWDFilterUsedForward; }
  Int_t GetForwardMWDFilterLength() { return fMWDFilterLengthForward; }
  Double_t GetForwardMWDFilterLifeT() { return fMWDDecayConstantForward; } // unit relative to ns
  Double_t GetForwardPulseThreshold() { return fPulseThresholdForward; }   // times over 1MeV noise level

  Int_t GetShashylikMAFilterLength() { return fMAFilterLengthShashylik; };
  Int_t GetShashylikCFFilterLength() { return fCFFilterLengthShashylik; }
  Double_t GetShashylikCFFilterRatio() { return fCFFilterRatioShashylik; }
  Int_t GetShashylikCFFitterLength() { return fCFFitterLengthShashylik; }
  Int_t GetShashylikCFDelayLength() { return fCFDelayLengthShashylik; }
  Int_t GetShashylikMWDFilterUsed() { return fMWDFilterUsedShashylik; }
  Int_t GetShashylikMWDFilterLength() { return fMWDFilterLengthShashylik; }
  Double_t GetShashylikMWDFilterLifeT() { return fMWDDecayConstantShashylik; } // unit relative to ns
  Double_t GetShashylikPulseThreshold() { return fPulseThresholdShashylik; }   // times over 1MeV noise level

  PndEmcFpgaPar(const char *name = "PndEmcFpgaPar", const char *title = "Emc Fpga module parameter", const char *context = "TestDefaultContext");
  ~PndEmcFpgaPar(void){};

  // Empty method, WHY is this Called in constructor(FIXME)
  void clear(void){};

  void putParams(FairParamList *list);
  Bool_t getParams(FairParamList *list);

 private:
  Int_t fMAFilterLengthBarrel;
  Int_t fCFFilterLengthBarrel;
  Double_t fCFFilterRatioBarrel;
  Int_t fCFDelayLengthBarrel;
  Int_t fCFFitterLengthBarrel;
  Int_t fMWDFilterUsedBarrel;
  Int_t fMWDFilterLengthBarrel;
  Double_t fMWDDecayConstantBarrel; // unit relative to ns
  Double_t fPulseThresholdBarrel;   // times over 1MeV noise level

  Int_t fMAFilterLengthForward;
  Int_t fCFFilterLengthForward;
  Double_t fCFFilterRatioForward;
  Int_t fCFDelayLengthForward;
  Int_t fCFFitterLengthForward;
  Int_t fMWDFilterUsedForward;
  Int_t fMWDFilterLengthForward;
  Double_t fMWDDecayConstantForward; // unit relative to ns
  Double_t fPulseThresholdForward;   // times over 1MeV noise level

  Int_t fMAFilterLengthShashylik;
  Int_t fCFFilterLengthShashylik;
  Double_t fCFFilterRatioShashylik;
  Int_t fCFDelayLengthShashylik;
  Int_t fCFFitterLengthShashylik;
  Int_t fMWDFilterUsedShashylik;
  Int_t fMWDFilterLengthShashylik;
  Double_t fMWDDecayConstantShashylik; // unit relative to ns
  Double_t fPulseThresholdShashylik;   // times over 1MeV noise level

  ClassDef(PndEmcFpgaPar, 2);
};

#endif
