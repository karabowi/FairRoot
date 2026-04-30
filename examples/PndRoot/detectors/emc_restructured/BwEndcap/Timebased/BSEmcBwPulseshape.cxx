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

#include "BSEmcBwPulseshape.h"

#include <math.h>

#include "math.h"
//#include <iostream>
// using namespace std;

// Class Member definitions -----------
BSEmcBwPulseshape::BSEmcBwPulseshape() : fTau(0), fN(0) {}

BSEmcBwPulseshape::BSEmcBwPulseshape(Double_t t_tau, Double_t t_n) : fTau(t_tau), fN(t_n) {}

Double_t BSEmcBwPulseshape::operator()(const Double_t t_time, const Double_t t_amp, const Double_t t_offset) const
{
  if (t_time < t_offset) {
    return 0.0;
  }

  Double_t norm = (t_time - t_offset) / fTau;
  return t_amp / exp(-fN) * pow(norm, fN) * exp(-fN * norm);
}

ClassImp(BSEmcBwPulseshape);
