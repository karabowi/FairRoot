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

#include "BSEmcShashlykPulseshape.h"
#include "TMath.h"
//#include <iostream>
//using namespace std;

// Class Member definitions -----------
BSEmcShashlykPulseshape::BSEmcShashlykPulseshape() : fMu(0), fSigma(0) {}

BSEmcShashlykPulseshape::BSEmcShashlykPulseshape(Double_t mu, Double_t sigma) : fMu(mu), fSigma(sigma) {}

Double_t BSEmcShashlykPulseshape::operator()(const Double_t t, const Double_t amp, const Double_t toffset) const
{
  if (t <= toffset) {
		return 0.0;
	} 

	return amp*(TMath::Exp(fMu - TMath::Power(fSigma, 2)/2.)/(t - toffset))*TMath::Exp(-TMath::Power(TMath::Log(t-toffset)-fMu, 2)/(2.*fSigma*fSigma));
}

ClassImp(BSEmcShashlykPulseshape);
