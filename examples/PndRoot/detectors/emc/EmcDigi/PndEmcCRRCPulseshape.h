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
//      Pulseshape from an CRRC-Shaper
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//-----------------------------------------------------------
//#pragma once
#ifndef PNDEMCCRRCPULSESHAPE_H
#define PNDEMCCRRCPULSESHAPE_H

// Base Class Headers ----------------
#include "PndEmcAbsPulseshape.h"

// Collaborating Class Headers -------
//#include <ostream>

/**
 * @brief Pulseshape from an CRRC-Shaper
 *
 */
class PndEmcCRRCPulseshape : public PndEmcAbsPulseshape {
 public:
  // Constructors/Destructors ---------
  PndEmcCRRCPulseshape();
  PndEmcCRRCPulseshape(double Tint, double Tdif, double Tsig);
  virtual ~PndEmcCRRCPulseshape() { ; }

  // Operators

  // Accessors -----------------------

  // Modifiers -----------------------

  // Operations ----------------------
  virtual double operator()(const double t, const double amp, const double toffset) const;

 private:
  // Private Data Members ------------
  double fTint;
  double fTdif;
  double fTsig;

  // Private Methods -----------------

  double general_solution(const double t, const double amp, const double toffset) const;
  double degenerate_solution(const double t, // for Tdif=Tint
                             const double amp, const double toffset) const;

  ClassDef(PndEmcCRRCPulseshape, 1)
};

#endif
