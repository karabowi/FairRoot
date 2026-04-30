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

#include "BSEmcAbsWaveformSimulator.h"
#include "BSEmcSamplingNoiseAdder.h"
#include "BSEmcWaveform.h"
#include "FairLogger.h"
#include "TMath.h"
#include "TRandom.h"

BSEmcSamplingNoiseAdder::BSEmcSamplingNoiseAdder() : fNoiseWidth(0.) {}

BSEmcSamplingNoiseAdder::BSEmcSamplingNoiseAdder(Double_t t_noiseWidth) : fNoiseWidth(t_noiseWidth){}

void BSEmcSamplingNoiseAdder::Modify(BSEmcWaveform *t_wf)
{
  for (std::vector<Double_t>::iterator it = GetWaveformReference(t_wf).begin(); it != GetWaveformReference(t_wf).end(); ++it) {
    Double_t sampledValue = gRandom->Gaus(0, fNoiseWidth);

    (*it) += sampledValue;
  }
}
