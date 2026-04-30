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

#include "BSEmcBwEndcapPulseAmplifier.h"

#include <vector>

using namespace std;

BSEmcBwEndcapPulseAmplifier::BSEmcBwEndcapPulseAmplifier() : fScale(0.), fInputScale(0.) {}

BSEmcBwEndcapPulseAmplifier::BSEmcBwEndcapPulseAmplifier(Double_t t_gain, Double_t t_inputScale) : fScale(t_gain), fInputScale(t_inputScale) {}

void BSEmcBwEndcapPulseAmplifier::Modify(BSEmcWaveform *t_wf)
{
  std::vector<Double_t>::iterator it;
  for (it = GetWaveformReference(t_wf).begin(); it != GetWaveformReference(t_wf).end(); ++it) {
    (*it) *= fScale;
  }
}
