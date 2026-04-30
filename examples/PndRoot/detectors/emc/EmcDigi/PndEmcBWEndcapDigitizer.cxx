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

#include "PndEmcAbsWaveformSimulator.h"
#include "PndEmcBWEndcapDigitizer.h"

#include "PndEmcWaveform.h"
#include "TMath.h"


PndEmcBWEndcapDigitizer::PndEmcBWEndcapDigitizer() :
	fInputScale(0.)
{
}

PndEmcBWEndcapDigitizer::PndEmcBWEndcapDigitizer(Double_t inputScale, Double_t nbits) :
	fInputScale(inputScale), fNbits(nbits) {
}

void PndEmcBWEndcapDigitizer::Modify(PndEmcWaveform* wf) {
	for(std::vector<Double_t>::iterator it=GetWaveformReference(wf).begin(); it!=GetWaveformReference(wf).end(); ++it) { 
		Double_t channel = (Int_t)(*it);
		Int_t max_adc = TMath::Power(2, fNbits);
		(*it) = (channel > max_adc) ? max_adc : channel;
	}
}

