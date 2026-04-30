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
#ifndef PNDEMCBWPULSESHAPE_H
#define PNDEMCBWPULSESHAPE_H

#include "PndEmcAbsPulseshape.h"

/**
 * @brief Experimentally derived Pulseshape 
 * 
 * of the form f(x) = A * (t')^ N * exp(-N t')
 * @ingroup PndEmc
 */
class PndEmcBWPulseshape  : public PndEmcAbsPulseshape {

	public:

	  PndEmcBWPulseshape();
	  PndEmcBWPulseshape(Double_t tau, Double_t N);
	  virtual ~PndEmcBWPulseshape(){};

	  virtual Double_t operator() (const Double_t t, const Double_t amp, const Double_t toffset) const;

	private:

	  Double_t fTau;
	  Double_t fN;

	ClassDef(PndEmcBWPulseshape, 1)
};

#endif

