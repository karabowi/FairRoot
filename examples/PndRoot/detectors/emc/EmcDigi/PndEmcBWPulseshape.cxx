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

#include "PndEmcBWPulseshape.h"
#include "math.h"
//#include <iostream>
//using namespace std;

// Class Member definitions -----------
PndEmcBWPulseshape::PndEmcBWPulseshape() : fTau(0), fN(0) {
}

PndEmcBWPulseshape::PndEmcBWPulseshape(Double_t tau, Double_t N) : fTau(tau), fN(N) {
}

Double_t PndEmcBWPulseshape::operator() (const Double_t t, const Double_t amp, const Double_t toffset) const {
	if (t < toffset) {
		return 0.0;
	} 

	Double_t t_norm = (t-toffset) / fTau;
	return amp/exp(-fN) * pow(t_norm, fN) * exp(-fN * t_norm);
}

ClassImp(PndEmcBWPulseshape);
