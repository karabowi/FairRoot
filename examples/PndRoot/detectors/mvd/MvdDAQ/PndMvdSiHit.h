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

#include <TObject.h>
#include <TClonesArray.h>

class PndMvdSiHit : public TObject {
 public:
  PndMvdSiHit();
  PndMvdSiHit(UShort_t box, UShort_t channel, Int_t adc, UShort_t numFrames);
  virtual ~PndMvdSiHit();

 public:
  UShort_t fBox;       // Box number (0..3)
  UShort_t fChannel;   // Channel of strip sensor (0..767)
  Int_t fAdc;          // raw ADC value
  UShort_t fNumFrames; // number of frames with signal above threshold

  ClassDef(PndMvdSiHit, 1);
};
