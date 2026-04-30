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

#include "PndEmcFittedPulseshape.h"
#include "math.h"

// Class Member definitions -----------
PndEmcFittedPulseshape::PndEmcFittedPulseshape() : fTau(0), fN(0) {}

PndEmcFittedPulseshape::PndEmcFittedPulseshape(double tau, double N) : fTau(tau), fN(N) {}

double PndEmcFittedPulseshape::operator()(const double t, const double amp, const double toffset) const
{
  if (t < toffset) {
    return 0.0;
  }
 
  double t_norm = (t - toffset) / fTau;
  return amp * pow(t_norm, fN) * exp(-fN * t_norm);
}

ClassImp(PndEmcFittedPulseshape);
