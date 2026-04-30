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
#ifndef PNDEMCShashlykPULSESHAPE_H
#define PNDEMCShashlykPULSESHAPE_H

#include "PndEmcAbsPulseshape.h"

/**
 * @brief Experimentally derived Pulseshape 
 * 
 * of the form f(x) = A * (t')^ N * exp(-N t')
 * @ingroup PndEmc
 */
class PndEmcShashlykPulseshape  : public PndEmcAbsPulseshape {

	public:

	  PndEmcShashlykPulseshape();
	  PndEmcShashlykPulseshape(Double_t mu, Double_t sigma);
	  virtual ~PndEmcShashlykPulseshape(){};

	  virtual Double_t operator() (const Double_t t, const Double_t amp, const Double_t toffset) const;

	private:

	  Double_t fMu;
	  Double_t fSigma;

	ClassDef(PndEmcShashlykPulseshape, 1)
};

#endif

