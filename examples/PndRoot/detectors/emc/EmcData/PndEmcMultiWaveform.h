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

//=============================================================================
// PndEmcMultiWaveform.h
//
// Class to hold multiple waveforms from one Emc Hit / ADC readout
//
// Author: Christian Hammmann
//         chammann@hiskp.uni-bonn.de
//
//=============================================================================

//#pragma once
#ifndef PNDEMCMULTIWAVEFORM_H
#define PNDEMCMULTIWAVEFORM_H

#include "PndEmcWaveform.h"

/**
 * @brief Class to hold multiple waveforms from one Emc Hit / ADC readout
 * @author Ch. Hammann <chammann@hiskp.uni-bonn.de>
 * @ingroup PndEmc
 */
class PndEmcMultiWaveform : public PndEmcWaveform {

  friend class PndEmcAbsWaveformModifier;

 public:
  PndEmcMultiWaveform() : PndEmcWaveform(), fSignals(1, std::vector<Double_t>(0, 0.)), fActiveWaveform(1){};

  PndEmcMultiWaveform(Int_t trackid, Long_t detId, Long_t waveform_length = 128, Int_t hitIndex = -1);

  virtual ~PndEmcMultiWaveform()
  {
    fSignal.clear();
    fSignals.clear();
  };

  //        PndEmcMultiWaveform(const PndEmcMultiWaveform& copy);

  void SetWaveform(const std::vector<Double_t> &signal, Int_t length, Int_t Waveform);

  virtual void clearAndReset();
  virtual void Clear(Option_t *)
  {
    fSignal.clear();
    fSignals.clear();
  };
  Int_t GetNumberOfWaveforms() const { return fSignals.size(); };
  Int_t GetActiveWaveform() const { return fActiveWaveform; };
  void SetActiveWaveform(Int_t active = 1) { fActiveWaveform = active; };

  virtual std::vector<Double_t> GetSignal() const;

 private:
  std::vector<std::vector<Double_t>> fSignals;
  Int_t fActiveWaveform;

  ClassDef(PndEmcMultiWaveform, 2);
};

#endif
