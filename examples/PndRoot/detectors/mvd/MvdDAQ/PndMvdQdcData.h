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

class PndMvdQdcData : public TObject {
 public:
  PndMvdQdcData();
  PndMvdQdcData(UShort_t channel, Int_t qdcValue);
  virtual ~PndMvdQdcData();

 public:
  UShort_t fChannel; // QDC channel
  Int_t fQdc;        // raw QDC value

  ClassDef(PndMvdQdcData, 1);
};
