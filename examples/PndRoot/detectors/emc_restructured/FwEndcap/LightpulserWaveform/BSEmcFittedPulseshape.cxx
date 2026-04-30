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

#include "BSEmcFittedPulseshape.h"

#include <math.h>

#include "math.h"

// Class Member definitions -----------
BSEmcFittedPulseshape::BSEmcFittedPulseshape() : fTau(0), fN(0) {}

BSEmcFittedPulseshape::BSEmcFittedPulseshape(Double_t t_tau, Double_t t_n) : fTau(t_tau), fN(t_n) {}

Double_t BSEmcFittedPulseshape::operator()(const Double_t t_t, const Double_t t_amp, const Double_t t_toffset) const
{
  if (t_t < t_toffset) {
    return 0.0;
  }

  Double_t norm = (t_t - t_toffset) / fTau;
  return t_amp * pow(norm, fN) * exp(-fN * norm);
}

ClassImp(BSEmcFittedPulseshape);
