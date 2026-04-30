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

#include "BSEmcBwEndcapDigitizer.h"

#include <vector>

#include "TMath.h"

class BSEmcWaveform;

BSEmcBwEndcapDigitizer::BSEmcBwEndcapDigitizer() : fInputScale(0.) {}

BSEmcBwEndcapDigitizer::BSEmcBwEndcapDigitizer(Double_t t_inputScale, Double_t t_nbits) : fInputScale(t_inputScale), fNbits(t_nbits) {}

void BSEmcBwEndcapDigitizer::Modify(BSEmcWaveform *t_wf)
{
  for (double &sample : GetWaveformReference(t_wf)) {
    Double_t channel = (Int_t)sample;
    Int_t max_adc = TMath::Power(2, fNbits);
    sample = (channel > max_adc) ? max_adc : channel;
  }
}
