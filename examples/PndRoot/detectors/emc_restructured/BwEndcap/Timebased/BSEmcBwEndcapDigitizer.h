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
#ifndef BSEMCBWENDCAPDIGITIZER_HH
#define BSEMCBWENDCAPDIGITIZER_HH

#include "Rtypes.h"
#include "RtypesCore.h"

#include "BSEmcAbsWaveformModifier.h"

class BSEmcWaveform;
class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @brief waveform digitzer
 * @author Guang Zhao (zhaog@ihep.ac.cn)
 * @ingroup BwEndcap
 */
class BSEmcBwEndcapDigitizer : public BSEmcAbsWaveformModifier {
 public:
  BSEmcBwEndcapDigitizer();
  BSEmcBwEndcapDigitizer(Double_t t_inputScale, Double_t t_nbits);

  virtual void Modify(BSEmcWaveform *t_wf) /*override*/;
  virtual Double_t GetScale() /*override*/ { return fInputScale; }

 private:
  Double_t fInputScale{0};
  Double_t fNbits{0};

  ClassDef(BSEmcBwEndcapDigitizer, 1)
};

#endif /*BSEMCBWENDCAPDIGITIZER_HH*/
