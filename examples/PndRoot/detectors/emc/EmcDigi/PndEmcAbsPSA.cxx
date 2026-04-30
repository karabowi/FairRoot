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

#include "PndEmcAbsPSA.h"
void PndEmcAbsPSA::Process(const PndEmcWaveform *waveform, Double_t &amplitude, Double_t &time)
{
  if (Process(waveform) > 0) {
    GetHit(0, amplitude, time);
  } else {
    amplitude = 0.;
    time = 0.;
  }
}; // deprecated do not use

ClassImp(PndEmcAbsPSA);
