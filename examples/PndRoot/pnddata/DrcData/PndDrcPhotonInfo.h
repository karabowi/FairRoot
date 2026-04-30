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

// -----------------------------------------
// PndDrcLutInfo.h
//
// Created on: 18.10.2013
// Author: R.Dzhygadlo at gsi.de
// -----------------------------------------
// Container for DrcTrackInfo

#ifndef PNDDRCPHOTONINFO_H
#define PNDDRCPHOTONINFO_H

#include "PndDrcAmbiguityInfo.h"

#include "TObject.h"
#include "TClonesArray.h"
#include "TVector3.h"
#include <vector>

class PndDrcPhotonInfo : public TObject {

 public:
  // Default constructor
  PndDrcPhotonInfo();

  // Default destructor
  ~PndDrcPhotonInfo();

  // Copy constructor
  // PndDrcPhotonInfo (const PndDrcPhotonInfo& val) { *this = val; }  // not needed

  // Mutators
  void SetHitTime(Double_t val) { fHitTime = val; }
  void SetReflected(Bool_t val) { fReflected = val; }
  void SetEvReflections(Int_t val) { fEvReflections = val; }
  void SetMcPrimeMomentumInBar(TVector3 val) { fMcPrimeMomentumInBar = val; }
  void SetMcCherenkovInBar(Double_t val) { fMcCherenkovInBar = val; }

  void AddAmbiguity(PndDrcAmbiguityInfo ambiguity);

  // Accessors
  Double_t GetHitTime() { return fHitTime; }
  Bool_t GetReflected() { return fReflected; }
  Int_t GetEvReflections() { return fEvReflections; }

  Int_t GetAmbiguitySize() { return fAmbiguitySize; }
  PndDrcAmbiguityInfo GetAmbiguity(Int_t id) { return fAmbiguityArray[id]; }
  TVector3 GetMcPrimeMomentumInBar() { return fMcPrimeMomentumInBar; }
  Double_t GetMcCherenkovInBar() { return fMcCherenkovInBar; }

 protected:
  std::vector<PndDrcAmbiguityInfo> fAmbiguityArray;
  Int_t fAmbiguitySize;

  Double_t fHitTime;
  Bool_t fReflected;
  Int_t fEvReflections;
  TVector3 fMcPrimeMomentumInBar;
  Double_t fMcCherenkovInBar;

  ClassDef(PndDrcPhotonInfo, 1)
};

#endif
