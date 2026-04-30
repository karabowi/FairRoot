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

#include "BSEmcShapingNoiseAdder.h"

#include "TMath.h"
#include "TRandom.h"

#include "fairlogger/Logger.h"

BSEmcShapingNoiseAdder::BSEmcShapingNoiseAdder() : fShapingTime(0), fNoiseWidth(0.), fInputScale(0.0), fSingleNoiseWidth(0.0), fMA_buffer(0) {}

BSEmcShapingNoiseAdder::BSEmcShapingNoiseAdder(Double_t t_shapingTime, Double_t t_sampleRate, Double_t t_noiseWidth, Double_t t_inputScale)
  : fShapingTime(t_shapingTime), fNoiseWidth(t_noiseWidth), fInputScale(t_inputScale),
    // MA buffer size as long as no of samples forming rising edge of wf
    fMA_buffer((Int_t)(t_shapingTime * t_sampleRate))
{
  Int_t bufferSize = fMA_buffer.GetSize();

  if (bufferSize == 0) {
    LOG(error) << "BSEmcShapedNoiseWaveformSimulator: pulse raises in less than one sample. Noise generation will fail";
  }

  // noise of individual sample is averaged over bufferSize samples --> fNoiseWidth² = bufferSize * fSingleNoiseWidth²
  fSingleNoiseWidth = fNoiseWidth / TMath::Sqrt(bufferSize); // still in GeV

  // Init buffer
  while ((bufferSize--) != 0) { // init buffer
    fMA_buffer(gRandom->Gaus(0, fSingleNoiseWidth * fInputScale));
  }
}

void BSEmcShapingNoiseAdder::Modify(BSEmcWaveform *t_wf)
{

  for (double &sample : GetWaveformReference(t_wf)) {
    Double_t sampledValue = gRandom->Gaus(0, fSingleNoiseWidth * fInputScale);

    Double_t result = fMA_buffer(sampledValue); // smear with shaping noise

    sample += result;
  }
}
