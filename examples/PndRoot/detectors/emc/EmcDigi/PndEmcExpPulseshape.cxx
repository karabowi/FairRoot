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
//      Exponential pulseshape
//-----------------------------------------------------------

// This Class' Header ------------------
#include "PndEmcExpPulseshape.h"

// C/C++ Headers ----------------------
#include "assert.h"
#include "math.h"

// Collaborating Class Headers --------

// Class Member definitions -----------

PndEmcExpPulseshape::PndEmcExpPulseshape(double Tdif) : fTdif(Tdif) {}

double PndEmcExpPulseshape::operator()(const double t, const double amp, const double toffset) const
{
  return general_solution(t, amp, toffset);
}

double PndEmcExpPulseshape::general_solution(const double t, const double amp, const double toffset) const
{
  double dt = t - toffset;
  if (dt < 0)
    return 0; // piecewise definition!
  return amp * exp(-dt / fTdif);
}
