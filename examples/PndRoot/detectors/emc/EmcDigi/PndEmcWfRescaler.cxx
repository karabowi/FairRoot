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

#include "PndEmcWfRescaler.h"

PndEmcWfRescaler::PndEmcWfRescaler() : fMultiplicator(1.) {}

PndEmcWfRescaler::PndEmcWfRescaler(Double_t inputScale, Double_t outputScale) : fMultiplicator(outputScale / inputScale) {}

PndEmcWfRescaler::PndEmcWfRescaler(Double_t multiplicator) : fMultiplicator(multiplicator) {}

void PndEmcWfRescaler::Modify(PndEmcWaveform *wf)
{
  for (std::vector<Double_t>::iterator it = GetWaveformReference(wf).begin(); it != GetWaveformReference(wf).end(); ++it) {
    (*it) *= fMultiplicator;
  }
}
