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

#ifndef PNDMVDRECOCHARGE_H
#define PNDMVDRECOCHARGE_H

#include <iostream>
#include <cmath>
#include "PndSdsDigi.h"

class PndMvdRecoCharge // calculates the charge of a digipixel for the given tot
{

 public:
  PndMvdRecoCharge();
  PndMvdRecoCharge(Double_t tr, Double_t a, Double_t threshold);
  //~PndMvdPixelCharge();
  Double_t GetCharge(PndSdsDigi &digi);
  Double_t GetCharge(Double_t tot);

 private:
  Double_t fthreshold; // threshold [e]
  Double_t ftr;        // raising time [ns]
  Double_t fa;         // falling ration [e/ns]
};
#endif /*PNDMVDRECOCHARGE_H*/
