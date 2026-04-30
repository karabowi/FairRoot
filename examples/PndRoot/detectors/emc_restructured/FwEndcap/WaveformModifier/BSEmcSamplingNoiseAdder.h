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
#ifndef BSEMCSAMPLINGNOISEADDER_HH
#define BSEMCSAMPLINGNOISEADDER_HH

#include "BSEmcAbsWaveformModifier.h"

class BSEmcWaveform;
class BSEmcAbsWaveformSimulator;

/**
 * @brief waveform modifier to add noise to waveform in ADC domain
 * @author Celina Frenkel <frenkel@hiskp.uni-bonn.de>
 * @ingroup BSEmc
 */
class BSEmcSamplingNoiseAdder : public BSEmcAbsWaveformModifier {
 public:
  BSEmcSamplingNoiseAdder();
  BSEmcSamplingNoiseAdder(Double_t t_noiseWidth);

  virtual void Modify(BSEmcWaveform *t_wf) /*override*/;

 protected:
 private:
  Double_t fNoiseWidth{0};

  
  ClassDef(BSEmcSamplingNoiseAdder, 0)
};

#endif /*BSEMCSAMPLINGNOISEADDER_HH*/
