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

#include "PndEmcBWEndcapPulseAmplifier.h"
#include "PndEmcAbsWaveformSimulator.h"

#include "PndEmcWaveform.h"
#include "TRandom.h"
#include "TMath.h"
#include "TVirtualFFT.h"

#include <vector>
#include <iostream>
using namespace std;

PndEmcBWEndcapPulseAmplifier::PndEmcBWEndcapPulseAmplifier() : fScale(0.), fInputScale(0.) {
}

PndEmcBWEndcapPulseAmplifier::PndEmcBWEndcapPulseAmplifier(Double_t gain, Double_t inputScale) : fScale(gain), fInputScale(inputScale)
{
}

void PndEmcBWEndcapPulseAmplifier::Modify(PndEmcWaveform* wf) {
    std::vector<Double_t>::iterator it;
	for (it = GetWaveformReference(wf).begin(); it  != GetWaveformReference(wf).end(); ++it) {
        (*it) *= fScale;
    }
}
