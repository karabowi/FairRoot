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

#include "BSEmcFittedPulseshape_nw.h"

#include <math.h>

#include "math.h"

// Class Member definitions -----------
BSEmcFittedPulseshape_nw::BSEmcFittedPulseshape_nw() : fTau(0), fN(0), fv1(0), ftcr(0), ftcf(0), ft_r(0), fres(0) {}

BSEmcFittedPulseshape_nw::BSEmcFittedPulseshape_nw(Double_t v1, Double_t tcr, Double_t tcf, Double_t t_r, Double_t res, Double_t tau, Double_t N)
  : fTau(tau), fN(N), fv1(v1), ftcr(tcr), ftcf(tcf), ft_r(t_r), fres(res)
{
}

Double_t BSEmcFittedPulseshape_nw::operator()(const Double_t t, const Double_t amp, const Double_t toffset) const
{
  if (t < toffset) {
    return 0.0;
  }

  Double_t tdf = toffset + ft_r;
  Double_t v2 = 1;
  if (t < toffset) {
    return fres;
  }
  if (t < tdf) {
    return fv1 + amp * (v2 - exp(-(t - toffset) / ftcr));
  } else {
    return fv1 + amp * exp(-(t - tdf) / ftcf);
  }
}

ClassImp(BSEmcFittedPulseshape_nw);
