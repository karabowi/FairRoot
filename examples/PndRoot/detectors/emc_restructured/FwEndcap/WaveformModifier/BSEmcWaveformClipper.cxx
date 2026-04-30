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

#include "BSEmcWaveformClipper.h"

#include <vector>

class BSEmcWaveform;

BSEmcWaveformClipper::BSEmcWaveformClipper() : fTotalChannels(0) {}

BSEmcWaveformClipper::BSEmcWaveformClipper(Int_t t_totalNumberOfChannels) : fTotalChannels(t_totalNumberOfChannels) {}

void BSEmcWaveformClipper::Modify(BSEmcWaveform *t_wf)
{
  for (double &sample : GetWaveformReference(t_wf)) {
    sample = (sample > fTotalChannels) ? fTotalChannels : sample; // clipping
  }
}
