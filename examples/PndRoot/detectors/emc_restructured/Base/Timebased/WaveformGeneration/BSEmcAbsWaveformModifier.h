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

#ifndef BSEMCABSWAVEFORMMODIFIER_HH
#define BSEMCABSWAVEFORMMODIFIER_HH

#include "TObject.h"

#include "BSEmcMultiWaveform.h"
#include "BSEmcWaveform.h"

/**
 * @brief interface for waveform modifiers
 *
 * @author Ph. Mahlberg <mahlberg@hiskp.uni-bonn.de>
 * @ingroup EmcBase
 */
class BSEmcAbsWaveformModifier : public TObject {
 public:
  virtual void Modify(BSEmcWaveform *t_wf) = 0;
  virtual Double_t GetScale() { return 1.; };

  virtual ~BSEmcAbsWaveformModifier(){};

 protected:
  std::vector<Double_t> &GetWaveformReference(BSEmcWaveform *t_wf)
  {
    if (BSEmcMultiWaveform *multiWf = dynamic_cast<BSEmcMultiWaveform *>(t_wf)) {
      Int_t activeWf = multiWf->GetActiveWaveform();
      return multiWf->fSignals.at(activeWf);
    }
    return t_wf->fSignal;
  }

  ClassDef(BSEmcAbsWaveformModifier, 1);
};

#endif /*BSEMCABSWAVEFORMMODIFIER_HH*/
