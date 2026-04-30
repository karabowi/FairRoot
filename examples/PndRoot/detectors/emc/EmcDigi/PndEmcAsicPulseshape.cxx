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
// Description:
//      Pulseshape from an APFEL ASIC preamplifier shaper
//      Circuit is described in EMC TDR and consists of charge-sensitive preamplifier,
//      with the following pole-zero cancelation of its feedback ad 3rd order integrator.
//		  The pulseshape is determined by the integration time of single intagrator stage
//      and scintillation time of the crystal, with exponential input convoluted with
//      response of the circuit
//
//-----------------------------------------------------------

// This Class' Header ------------------
#include "PndEmcAsicPulseshape.h"

// C/C++ Headers ----------------------
#include "assert.h"
#include "math.h"

// Collaborating Class Headers --------

// Class Member definitions -----------
PndEmcAsicPulseshape::PndEmcAsicPulseshape() : fTint(0), fTsig(0) {}

PndEmcAsicPulseshape::PndEmcAsicPulseshape(double Tint, double Tsig) : fTint(Tint), fTsig(Tsig)
{
  assert(fTint != fTsig);
}

double PndEmcAsicPulseshape::operator()(const double t, const double amp, const double toffset) const
{
  if (fTsig == 0)
    return degenerate_solution(t, amp, toffset);

  return general_solution(t, amp, toffset);
}

double PndEmcAsicPulseshape::general_solution(const double t, const double amp, const double toffset) const
{
  double dt = t - toffset;
  if (dt < 0)
    return 0; // piecewise definition!

  double l_int = 1.0 / fTint;
  double l_sig = 1.0 / fTsig;

  double term1 = exp(-dt * l_int) * dt * dt * dt / (6 * (l_sig - l_int));
  double term2 = exp(-dt * l_int) * dt * dt / (2 * pow(l_sig - l_int, 2));
  double term3 = exp(-dt * l_int) * dt / (pow(l_sig - l_int, 3));
  double term4 = exp(-dt * l_int) / (pow(l_sig - l_int, 4));
  double term5 = exp(-dt * l_sig) / (pow(l_sig - l_int, 4));

  return amp * (term1 - term2 + term3 - term4 + term5);
}

double PndEmcAsicPulseshape::degenerate_solution(const double t, // for Tsig=0
                                                 const double amp, const double toffset) const
{
  double dt = t - toffset;
  if (dt < 0)
    return 0; // piecewise definition!

  double l_shaper = 1.0 / fTint;

  double term1 = exp(-l_shaper * dt) * pow(dt, 3);

  return amp * term1;
}

ClassImp(PndEmcAsicPulseshape);
