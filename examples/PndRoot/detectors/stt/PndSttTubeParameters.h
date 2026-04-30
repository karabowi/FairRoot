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

#ifndef PNDSTTTUBEPARAMETERS_H
#define PNDSTTTUBEPARAMETERS_H

#include "TObject.h"

class PndSttTubeParameters : public TObject {

 public:
  /** Default constructor **/
  PndSttTubeParameters();
  PndSttTubeParameters(PndSttTubeParameters &parms);

  ~PndSttTubeParameters();
  PndSttTubeParameters(Int_t tubeID, Double_t hl);

  Int_t GetTubeID();
  Double_t GetHalfLength();

 private:
  Int_t fTubeID;
  Double_t fHalfLength;

  ClassDef(PndSttTubeParameters, 1);
};

#endif
