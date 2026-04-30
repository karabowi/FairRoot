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
#ifndef BSEMCSHASHLYKNOISEADDER_H_
#define BSEMCSHASHLYKNOISEADDER_H_

#include "BSEmcAbsWaveformModifier.h"
#include "TString.h"
#include "TRandom3.h"

class BSEmcWaveform;

/**
 * @brief waveform modifier to add noise to waveform
 * @author Guang Zhao (zhaog@ihep.ac.cn)
 * @ingroup Emc
 */
class BSEmcShashlykNoiseAdder : public BSEmcAbsWaveformModifier {
 public:
  BSEmcShashlykNoiseAdder();
  ~BSEmcShashlykNoiseAdder();
  BSEmcShashlykNoiseAdder(Double_t sigma, Int_t seed);

  virtual void Modify(BSEmcWaveform *wf);

 private:
  TRandom3 *fRandom;
  Double_t fSigma;
  void get_rnd_noise(BSEmcWaveform *wf, Double_t noise);

  ClassDef(BSEmcShashlykNoiseAdder, 0)
};

#endif /*BSEMCSHASHLYKNOISEADDER_H_*/
