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
#include "PndEmcWaveformDigitizer.h"

#include "PndEmcWaveform.h"
#include "TMath.h"


PndEmcWaveformDigitizer::PndEmcWaveformDigitizer() :
	fNBits(0),
	fEnergyRange(0.),
	fInputScale(0.),
	fTotalChannels(0),
	fOneBitResolution(0.) {
}

PndEmcWaveformDigitizer::PndEmcWaveformDigitizer(Int_t nBits, Double_t energyRange, Double_t inputScale) :
	fNBits(nBits),
	fEnergyRange(energyRange),
	fInputScale(inputScale) {
		fTotalChannels = (Int_t) (TMath::Power(2, nBits) + 0.5);
		fOneBitResolution = fEnergyRange/fTotalChannels * fInputScale;
}

void PndEmcWaveformDigitizer::Modify(PndEmcWaveform* wf) {
	for(std::vector<Double_t>::iterator it=GetWaveformReference(wf).begin(); it!=GetWaveformReference(wf).end(); ++it) { 
		Double_t channel = TMath::Floor((*it)/fOneBitResolution); 					//digitize
		(*it) = (channel > fTotalChannels) ? fTotalChannels : channel;				//clipping
		//cout << ((channel > fTotalChannels) ? fTotalChannels : channel) << " ";
	}
//cout << endl;
}

