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

#pragma once
#ifndef PNDEMCABSWAVEFORMMODIFIER_H_
#define PNDEMCABSWAVEFORMMODIFIER_H_

#include "PndEmcWaveform.h"
#include "PndEmcMultiWaveform.h"

#include "TObject.h"

/**
 * @brief interface for waveform modifiers
 *
 * @author Ph. Mahlberg <mahlberg@hiskp.uni-bonn.de>
 * @ingroup PndEmc
 */
class PndEmcAbsWaveformModifier : public TObject {
 public:
  virtual void Modify(PndEmcWaveform *wf) = 0;
  virtual Double_t GetScale() { return 1.; };

  virtual ~PndEmcAbsWaveformModifier(){};

 protected:
  std::vector<Double_t> &GetWaveformReference(PndEmcWaveform *wf)
  {
    if (PndEmcMultiWaveform *multiWf = dynamic_cast<PndEmcMultiWaveform *>(wf)) {
      Int_t activeWf = multiWf->GetActiveWaveform();
      return multiWf->fSignals.at(activeWf);
    }
    return wf->fSignal;
  }

  ClassDef(PndEmcAbsWaveformModifier, 1);
};

#endif
