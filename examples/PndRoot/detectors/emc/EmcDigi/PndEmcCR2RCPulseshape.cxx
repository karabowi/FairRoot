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

//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndEmcCR2RCPulseshape
//      see PndEmcCR2RCPulseshape.h for details
//
//-----------------------------------------------------------

// This Class' Header ------------------
#include "PndEmcCR2RCPulseshape.h"

// C/C++ Headers ----------------------
#include "assert.h"
#include "math.h"

// Collaborating Class Headers --------

// Class Member definitions -----------
PndEmcCR2RCPulseshape::PndEmcCR2RCPulseshape() : fTint(0), fTdif(0), fTsig(0) {}

PndEmcCR2RCPulseshape::PndEmcCR2RCPulseshape(double Tint, double Tdif, double Tsig) : fTint(Tint), fTdif(Tdif), fTsig(Tsig)
{
  assert(fTint != fTsig && fTdif != fTsig);
}

double PndEmcCR2RCPulseshape::operator()(const double t, const double amp, const double toffset) const
{
  if (fTint == fTdif)
    return degenerate_solution(t, amp, toffset);
  return general_solution(t, amp, toffset);
}

double PndEmcCR2RCPulseshape::general_solution(const double t, const double amp, const double toffset) const
{
  double dt = t - toffset;
  if (dt < 0)
    return 0; // piecewise definition!

  double l_int = 1.0 / fTint;
  double l_dif = 1.0 / fTdif;
  double l_sig = 1.0 / fTsig;

  double term1 = exp(-dt * l_int) * (l_sig - l_dif) * (dt - 1 / (l_sig - l_int) - 1 / (l_dif - l_int));
  double term2 = exp(-dt * l_dif) * (l_sig - l_int) / (l_dif - l_int);
  double term3 = exp(-dt * l_sig) * (l_dif - l_int) / (l_sig - l_int);

  return amp / ((l_sig - l_dif) * (l_sig - l_int) * (l_dif - l_int)) * (term1 + term2 - term3);
}

double PndEmcCR2RCPulseshape::degenerate_solution(const double t, // for Tdif=Tint
                                                  const double amp, const double toffset) const
{
  double dt = t - toffset;
  if (dt < 0)
    return 0; // piecewise definition!

  double l_shaper = 1.0 / fTdif;
  double l_sig = 1.0 / fTsig;

  double dl = l_sig - l_shaper;

  double term1 = (exp(-l_shaper * dt) * (dl * dl * dt * dt / 2.0 - dl * dt + 1.0) - exp(-l_sig * dt));

  return amp * term1;
}
