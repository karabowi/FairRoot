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
//      Exponential Pulseshape
//-----------------------------------------------------------
//#pragma once
#ifndef PndEmcExpPulseshape_H
#define PndEmcExpPulseshape_H

// Base Class Headers ----------------
#include "PndEmcAbsPulseshape.h"

// Collaborating Class Headers -------
//#include <ostream>

/**
 * @brief Exponential Pulseshape
 * @ingroup PndEmc
 */
class PndEmcExpPulseshape : public PndEmcAbsPulseshape {
 public:
  // Constructors/Destructors ---------
  PndEmcExpPulseshape() : fTdif(0) { ; }
  PndEmcExpPulseshape(double Tdif);
  virtual ~PndEmcExpPulseshape() { ; }

  // Operations ----------------------
  virtual double operator()(const double t, const double amp, const double toffset) const;

 private:
  // Private Data Members ------------
  double fTdif;

  // Private Methods -----------------

  double general_solution(const double t, const double amp, const double toffset) const;
};

#endif
