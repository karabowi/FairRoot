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

#ifndef PNDRICHRESOLUTION_H
#define PNDRICHRESOLUTION_H

#include "PndPidCandidate.h"
#include "TVector3.h"
#include "PndRichCalDb.h"

class PndRichResolution //: public FairGeoSet
{

 private:
  PndRichCalDb *caldb;

 protected:
 public:
  PndRichResolution();
  ~PndRichResolution();

  Double_t Sigma(dbpoint pnt);
  Double_t Shift(dbpoint pnt);
  Double_t Efficiency(dbpoint pnt);
};

#endif
