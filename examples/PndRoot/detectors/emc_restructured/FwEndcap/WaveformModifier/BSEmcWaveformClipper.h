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
#ifndef BSEMCWAVEFORMCLIPPER_HH
#define BSEMCWAVEFORMCLIPPER_HH
#include "Rtypes.h"
#include "RtypesCore.h"

#include "BSEmcAbsWaveformModifier.h"

class BSEmcWaveform;
class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @brief waveform clipper
 * @author Ph. Mahlberg <mahlberg@hiskp.uni-bonn.de>
 * @ingroup FwEndcap
 */
class BSEmcWaveformClipper : public BSEmcAbsWaveformModifier {
 public:
  BSEmcWaveformClipper();
  BSEmcWaveformClipper(Int_t t_totalNumberOfChannels);

  virtual void Modify(BSEmcWaveform *t_wf) /*override*/;

 private:
  Int_t fTotalChannels{0};

  ClassDef(BSEmcWaveformClipper, 0)
};

#endif /*BSEMCWAVEFORMCLIPPER_HH*/
