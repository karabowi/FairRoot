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
// BSEmcMultiWaveform.h
//
// Class to hold multiple waveforms from one Emc Hit / ADC readout
//
// Author: Christian Hammmann
//         chammann@hiskp.uni-bonn.de
//
//=============================================================================

//#pragma once
#ifndef BSEMCMULTIWAVEFORM_HH
#define BSEMCMULTIWAVEFORM_HH

#include <memory>
#include <string>
#include <vector>

#include "Rtypes.h"
#include "RtypesCore.h"

#include "BSEmcDataBranchNames.h"
#include "BSEmcWaveform.h"

class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @brief Class to hold multiple waveforms from one Emc Hit / ADC readout
 * @author Ch. Hammann <chammann@hiskp.uni-bonn.de>
 * @ingroup EmcData
 */
class BSEmcMultiWaveform : public BSEmcWaveform {

  friend class BSEmcAbsWaveformModifier;

 public:
  BSEmcMultiWaveform() : BSEmcWaveform(), fSignals(1, std::vector<Double_t>(0, 0.)), fActiveWaveform(1) {}

  BSEmcMultiWaveform(Int_t t_trackid, Long_t t_detId, Long_t t_waveform_length = 128, Int_t t_hitIndex = -1);

  virtual ~BSEmcMultiWaveform()
  {
    fSignal.clear();
    fSignals.clear();
  };

  void SetWaveform(const std::vector<Double_t> &t_signal, Int_t t_length, Int_t t_waveform);

  virtual void clearAndReset() /*override*/;

  virtual void Clear(Option_t * /*unused*/) /*override*/
  {
    fSignal.clear();
    fSignals.clear();
  };

  Int_t GetNumberOfWaveforms() const { return fSignals.size(); };
  Int_t GetActiveWaveform() const { return fActiveWaveform; };
  void SetActiveWaveform(Int_t t_active = 1) { fActiveWaveform = t_active; };
  Int_t GetWaveformLength() const { return fSignals.at(fActiveWaveform).size(); }

  virtual std::vector<Double_t> GetSignal() const /*override*/;
  const std::vector<Double_t> &GetSignal(Int_t t_waveform) const { return fSignals.at(t_waveform); }

 private:
  std::vector<std::vector<Double_t>> fSignals{};
  Int_t fActiveWaveform{-1};

  ClassDef(BSEmcMultiWaveform, 2);
};

#endif /*BSEMCMULTIWAVEFORM_HH*/
