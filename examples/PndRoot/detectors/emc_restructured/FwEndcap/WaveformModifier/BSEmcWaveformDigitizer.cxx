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

#include "BSEmcWaveformDigitizer.h"

#include <vector>

#include "TMath.h"

class BSEmcWaveform;

BSEmcWaveformDigitizer::BSEmcWaveformDigitizer() : fInputScale(0.), fOneBitResolution(0.), fTotalChannels(0) {}

BSEmcWaveformDigitizer::BSEmcWaveformDigitizer(Double_t t_onebitresolution, Double_t t_inputScale, Int_t t_totalChannels)
  : fInputScale(t_inputScale), fOneBitResolution(t_onebitresolution), fTotalChannels(t_totalChannels)
{
  fOneBitResolution = fOneBitResolution * fInputScale;
}

void BSEmcWaveformDigitizer::Modify(BSEmcWaveform *t_wf)
{
  for (double &sample : GetWaveformReference(t_wf)) {
    sample = TMath::Floor(sample / fOneBitResolution); // digitize
  }
}
