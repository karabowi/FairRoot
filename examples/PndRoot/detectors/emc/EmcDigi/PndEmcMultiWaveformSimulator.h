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

#ifndef PNDEMCMULTIWAVEFORMSIMULATOR_H_
#define PNDEMCMULTIWAVEFORMSIMULATOR_H_

#include "PndEmcFullStackedWaveformSimulator.h"
#include <vector>

class PndEmcAbsPulseshape;
class PndEmcMultiWaveform;
class TClonesArray;

/**
 * @brief Simulator to create PndEmcMultiWaveform
 *
 * @author Ph. Mahlberg <mahlberg@hiskp.uni-bonn.de>
 * @ingroup PndEmc
 */
class PndEmcMultiWaveformSimulator : public PndEmcFullStackedWaveformSimulator {
 public:
  PndEmcMultiWaveformSimulator();
  PndEmcMultiWaveformSimulator(Double_t sampleRate, PndEmcAbsPulseshape *pulseshape, Int_t wfMultiplicity);

  /*! wfIndex characterizes wf, the modifier will apllied on*
   *  If the modifier itself will work on the multiwaveform itself, pass a -1. In this case, the Modifier is considered to scale all waveforms equally.
   */
  virtual void AddModifier(PndEmcAbsWaveformModifier *wfModifier, Int_t wfIndex);

  virtual Double_t GetTotalScale(Int_t wfIndex);

 protected:
  virtual PndEmcWaveform *CallModifiers(PndEmcWaveform *wf);
  virtual PndEmcWaveform *MakeWaveform(PndEmcWaveformData *wfData, TClonesArray *arrayToStore = nullptr);
  virtual PndEmcWaveform *MakeSingleWaveform(Double_t hitEnergy, Double_t hitTime, TClonesArray *arrayToStore = nullptr, Int_t detId = -1, Int_t trackId = -1, Int_t hitIndex = -1);

 private:
  Int_t fWfMultiplicity;
  std::vector<Int_t> fIndexList;

  PndEmcMultiWaveform *MultiplyWaveform(const PndEmcWaveform *singleWaveform, TClonesArray *arrayToStore);

  ClassDef(PndEmcMultiWaveformSimulator, 1)
};

#endif
