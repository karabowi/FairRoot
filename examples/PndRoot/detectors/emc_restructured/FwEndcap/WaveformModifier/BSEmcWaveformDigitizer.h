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
#ifndef BSEMCWAVEFORMDIGITIZER_HH
#define BSEMCWAVEFORMDIGITIZER_HH
#include "Rtypes.h"
#include "RtypesCore.h"

#include "BSEmcAbsWaveformModifier.h"

class BSEmcWaveform;
class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @brief waveform digitzer
 * @author Ph. Mahlberg <mahlberg@hiskp.uni-bonn.de>
 * @ingroup FwEndcap
 */
class BSEmcWaveformDigitizer : public BSEmcAbsWaveformModifier {
 public:
  BSEmcWaveformDigitizer();
  BSEmcWaveformDigitizer(Double_t t_onebitresolution, Double_t t_inputScale, Int_t t_totalChannels);

  virtual void Modify(BSEmcWaveform *t_wf) /*override*/;
  virtual Double_t GetScale() /*override*/ { return fTotalChannels / fInputScale; }

 private:
  Double_t fInputScale{0};
  Double_t fOneBitResolution{0};
  Int_t fTotalChannels{0};

  ClassDef(BSEmcWaveformDigitizer, 0)
};

#endif /*BSEMCWAVEFORMDIGITIZER_HH*/
