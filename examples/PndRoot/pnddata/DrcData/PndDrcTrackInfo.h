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

#ifndef PNDDRCTRACKINFO_H
#define PNDDRCTRACKINFO_H

#include "PndDrcPhotonInfo.h"

#include "TObject.h"
#include "TClonesArray.h"
#include "TVector3.h"
#include <vector>

class PndDrcTrackInfo : public TObject {

 public:
  // Default constructor
  PndDrcTrackInfo();

  // Default destructor
  ~PndDrcTrackInfo();

  // Copy constructor
  // PndDrcTrackInfo (const PndDrcTrackInfo& val): TObject(val)  { *this = val; }   // Most probably not needed

  // Mutators
  void SetMcPdg(Int_t val) { fMcPdg = val; }
  void SetMcMomentum(TVector3 val) { fMcMomentum = val; }
  void SetMcMomentumInBar(TVector3 val) { fMcMomentumInBar = val; }
  void SetMcPositionInBar(TVector3 val) { fMcPositionInBar = val; }
  void SetMcCherenkov(Double_t val) { fMcCherenkov = val; }
  void SetMcTimeInBar(Double_t val) { fMcTimeInBar = val; }

  void SetPdg(Int_t val) { fPdg = val; }
  void SetMomentum(TVector3 val) { fMomentum = val; }
  void SetCherenkov(Double_t val) { fCherenkov = val; }

  void AddPhoton(PndDrcPhotonInfo photon);

  // Accessors
  Int_t GetMcPdg() { return fMcPdg; }
  TVector3 GetMcMomentum() { return fMcMomentum; }
  TVector3 GetMcMomentumInBar() { return fMcMomentumInBar; }
  TVector3 GetMcPositionInBar() { return fMcPositionInBar; }
  Double_t GetMcCherenkov() { return fMcCherenkov; }
  Double_t GetMcTimeInBar() { return fMcTimeInBar; }

  Int_t GetPdg() { return fPdg; }
  TVector3 GetMomentum() { return fMomentum; }
  Double_t GetCherenkov() { return fCherenkov; }

  Int_t GetPhotonSize() { return fPhotonSize; }
  PndDrcPhotonInfo GetPhoton(Int_t id) { return fPhotonArray[id]; }

 protected:
  std::vector<PndDrcPhotonInfo> fPhotonArray;
  Int_t fPhotonSize;

  Int_t fMcPdg;
  TVector3 fMcMomentum;
  TVector3 fMcMomentumInBar;
  TVector3 fMcPositionInBar;
  Double_t fMcCherenkov;
  Double_t fMcTimeInBar;

  Int_t fPdg;
  TVector3 fMomentum;
  Double_t fCherenkov;

  ClassDef(PndDrcTrackInfo, 1)
};

#endif
