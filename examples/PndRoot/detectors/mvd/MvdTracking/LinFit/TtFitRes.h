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

// Simone Bianco
// 15.07.2010

#ifndef TTFITRES_H
#define TTFITRES_H

#include "TVector3.h"
#include "TString.h"

#include <stdio.h>
#include <iostream>
#include <vector>

class TtFitRes : public TObject {

 public:
  TtFitRes();

  TtFitRes(Double_t p0, Double_t p1, Double_t p2, Double_t p3, Double_t eloss, Double_t chiX, Double_t chiY, Int_t numSens);

  virtual ~TtFitRes();

  void GetPar(Double_t *par) const
  {
    par[0] = fP0;
    par[1] = fP1;
    par[2] = fP2;
    par[3] = fP3;
  }
  TVector2 GetFirstPoint() const { return TVector2(fP0, fP2); }
  TVector2 GetDirectionVec() const { return TVector2(fP1, fP3); }
  Double_t GetChiSquareX() const { return fChiSqX; }
  Double_t GetChiSquareY() const { return fChiSqY; }
  Int_t GetNsens() const { return fNsens; }
  Double_t GetEloss() { return fEloss; }

 private:
  Double_t fP0, fP1, fP2, fP3; // fit-parameter
  Double_t fEloss;             // Total energy loss
  Double_t fChiSqX;            // Chi-Square of X Fit
  Double_t fChiSqY;            // Chi-Square of Y Fit
  Int_t fNsens;                // number of sensors used for the fit

  ClassDef(TtFitRes, 1);
};

#endif
