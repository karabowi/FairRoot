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

class PndMvdTdcData : public TObject {
 public:
  PndMvdTdcData();
  PndMvdTdcData(UShort_t channel, Int_t tdcValue, Bool_t trailing);
  virtual ~PndMvdTdcData();

 public:
  UShort_t fChannel; // TDC channel
  Int_t fTdc;        // raw TDC value
  Bool_t fTrailing;  // leading or trailing edge

  ClassDef(PndMvdTdcData, 1);
};
