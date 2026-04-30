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
#ifndef PNDEMCWAVEFORMDIGITIZER_H_
#define PNDEMCWAVEFORMDIGITIZER_H_

/**
 * @brief waveform digitzer
 * @author Ph. Mahlberg <mahlberg@hiskp.uni-bonn.de>
 * @ingroup PndEmc
 */
class PndEmcWaveformDigitizer : public PndEmcAbsWaveformModifier {
 public:
  PndEmcWaveformDigitizer();
  PndEmcWaveformDigitizer(Int_t nBits, Double_t energyRange, Double_t inputScale);

  virtual void Modify(PndEmcWaveform *wf);
  virtual Double_t GetScale() { return fTotalChannels / fInputScale; }

 private:
  Int_t fNBits;
  Double_t fEnergyRange; // in GeV;
  Double_t fInputScale;

  Int_t fTotalChannels;
  Double_t fOneBitResolution;

  ClassDef(PndEmcWaveformDigitizer, 0);
};

#endif
