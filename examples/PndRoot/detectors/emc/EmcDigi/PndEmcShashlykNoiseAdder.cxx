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

#include "PndEmcShashlykNoiseAdder.h"
#include "PndEmcAbsWaveformSimulator.h"

#include "PndEmcWaveform.h"
#include "TRandom3.h"
#include "TMath.h"
#include "TVirtualFFT.h"

#include <vector>
#include <fstream>
#include <iostream>

using std::cout;
using std::endl;

PndEmcShashlykNoiseAdder::PndEmcShashlykNoiseAdder() {
    fSigma = 0.;
}

PndEmcShashlykNoiseAdder::PndEmcShashlykNoiseAdder(Double_t sigma, Int_t seed)
{
    fSigma = sigma;
    fRandom = new TRandom3(seed);
}

PndEmcShashlykNoiseAdder::~PndEmcShashlykNoiseAdder() {
}

void PndEmcShashlykNoiseAdder::Modify(PndEmcWaveform* wf) {
    get_rnd_noise(wf, fSigma);
}

void PndEmcShashlykNoiseAdder::get_rnd_noise(PndEmcWaveform* wf, Double_t noise_width) {
    std::vector<Double_t>::iterator it;
    Int_t i = 0;
	for (it = GetWaveformReference(wf).begin(); it != GetWaveformReference(wf).end(); ++it) { 
        Double_t noise = fRandom->Gaus(0, noise_width);
        (*it) += noise;
        i++;
    }
}
