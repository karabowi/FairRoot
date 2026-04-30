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

//#pragma once
#ifndef PNDEMCWFRESCALER
#define PNDEMCWFRESCALER

#include "PndEmcAbsWaveformModifier.h"

class PndEmcWaveform;
class PndEmcAbsWaveformSimulator;

/**
 * @brief waveform rescaler
 * @author Ph. Mahlberg <mahlberg@hiskp.uni-bonn.de>
 * @ingroup PndEmc
 */
class PndEmcWfRescaler : public PndEmcAbsWaveformModifier {

 public:
  PndEmcWfRescaler();
  PndEmcWfRescaler(Double_t inputScale, Double_t outputScale);
  PndEmcWfRescaler(Double_t multiplicator);
  virtual void Modify(PndEmcWaveform *wf);
  virtual Double_t GetScale() { return fMultiplicator; }

 private:
  Double_t fMultiplicator;

  ClassDef(PndEmcWfRescaler, 1)
};

#endif
