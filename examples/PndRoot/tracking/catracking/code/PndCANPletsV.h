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

//-*- Mode: C++ -*-
// *****************************************************************************
//                                                                             *
// @Autors: I.Kulakov; M.Zyzak; I.Kisel                                        *
// @e-mail: I.Kulakov@gsi.de; M.Zyzak@gsi.de; I.Kisel@compeng.uni-frankfurt.de *
//                                                                             *
// *****************************************************************************

#ifndef PNDCANPletsV_H
#define PNDCANPletsV_H

#include <vector>
using std::vector;
#include "PndCAStationArray.h"
#include "PndCATrackParamVector.h"

class PndCANPletV {
 public:
  PndCANPletV() : fParam(), fIsValid(false) {}

  PndCANPletV(const PndCANPletV &s1, const PndCATrackParamVector &param, const float_m isValid) : fParam(param), fIsValid(isValid) { fNHits = s1.N() + 1; }

  int N() const { return fNHits; }

  const PndCATrackParamVector &Param() const { return fParam; }
  PndCATrackParamVector &ParamRef() { return fParam; }

  float_m IsValid() const { return fIsValid; }
  void SetValid(float_m v) { fIsValid = v; }
  // check wether a is neighbour from the right to this

  int_v fLastHit;
  int fNHits;
  PndCATrackParamVector fParam;
  float_m fIsValid;
};

class PndCANPletsV : public PndCAStationArray<PndCANPletV> {
 public:
  PndCANPletsV(){};
  PndCANPletsV(int nSta, const PndCAHits *hits) : PndCAStationArray<PndCANPletV>(nSta, hits){};
};

#endif
