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

#include "BSEmcAbsPSA.h"

void BSEmcAbsPSA::Process(const BSEmcWaveform *t_waveform, Double_t &t_amplitude, Double_t &t_time)
{
  if (Process(t_waveform) > 0) {
    GetHit(0, t_amplitude, t_time);
  } else {
    t_amplitude = 0.;
    t_time = 0.;
  }
}; // deprecated do not use

ClassImp(BSEmcAbsPSA);
