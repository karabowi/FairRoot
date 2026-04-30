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

#ifndef PNDDRCAMBIGUITYINFO_H
#define PNDDRCAMBIGUITYINFO_H

#include "PndDrcAmbiguityInfo.h"

#include "TObject.h"
#include "TClonesArray.h"
#include "TVector3.h"
#include <vector>

class PndDrcAmbiguityInfo : public TObject {

 public:
  // Default constructor
  PndDrcAmbiguityInfo();

  ~PndDrcAmbiguityInfo(){};

  // Copy constructor
  PndDrcAmbiguityInfo(const PndDrcAmbiguityInfo &val) : TObject(), fCherenkov(0.), fBarTime(0.), fEvTime(0.) { *this = val; }

  // Mutators
  void SetCherencov(Double_t val) { fCherenkov = val; }
  void SetBarTime(Double_t val) { fBarTime = val; }
  void SetEvTime(Double_t val) { fEvTime = val; }

  // Accessors
  Double_t GetCherencov() { return fCherenkov; }
  Double_t GetBarTime() { return fBarTime; }
  Double_t GetEvTime() { return fEvTime; }

 protected:
  Double_t fCherenkov;
  Double_t fBarTime;
  Double_t fEvTime;

  ClassDef(PndDrcAmbiguityInfo, 1)
};

#endif
