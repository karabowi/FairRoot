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
//      Experimentally derived Pulseshape of the form:
//	f(x) = A * (t')^ N * exp(-N t')
//-----------------------------------------------------------
//#pragma once
#ifndef PNDEMCFITTEDPULSESHAPE_H
#define PNDEMCFITTEDPULSESHAPE_H

#include "PndEmcAbsPulseshape.h"

/**
 * @brief Experimentally derived Pulseshape
 *
 * of the form f(x) = A * (t')^ N * exp(-N t')
 * @ingroup PndEmc
 */
class PndEmcFittedPulseshape : public PndEmcAbsPulseshape {

 public:
  PndEmcFittedPulseshape();
  PndEmcFittedPulseshape(double tau, double N);
  virtual ~PndEmcFittedPulseshape(){};
 
  virtual double operator()(const double t, const double amp, const double toffset) const;

 private:
  double fTau;
  double fN;

  ClassDef(PndEmcFittedPulseshape, 1)
};

#endif
