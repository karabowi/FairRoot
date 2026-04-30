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
#ifndef BSEMCBWENDCAPPULSEAMPLIFIER_HH
#define BSEMCBWENDCAPPULSEAMPLIFIER_HH

#include "Rtypes.h"
#include "RtypesCore.h"

#include "BSEmcAbsWaveformModifier.h"

class BSEmcWaveform;
class BSEmcAbsWaveformSimulator;
class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @brief waveform modifier to amplify the amplitude
 * @author Guang Zhao (zhaog@ihep.ac.cn)
 * @ingroup BwEndcap
 */
class BSEmcBwEndcapPulseAmplifier : public BSEmcAbsWaveformModifier {
 public:
  BSEmcBwEndcapPulseAmplifier();
  BSEmcBwEndcapPulseAmplifier(Double_t t_gain, Double_t t_inputScale);

  virtual Double_t GetScale() /*override*/ { return fScale / fInputScale; }
  virtual void Modify(BSEmcWaveform *t_wf) /*override*/;

 protected:
 private:
  Double_t fScale{0};
  Double_t fInputScale{0};

  ClassDef(BSEmcBwEndcapPulseAmplifier, 1)
};

#endif /*BSEMCBWENDCAPPULSEAMPLIFIER_HH*/
