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
// @Autors: I.Kulakov; M.Pugach; M.Zyzak; I.Kisel                                        *
// @e-mail: I.Kulakov@gsi.de; M.Pugach@gsi.de; M.Zyzak@gsi.de; I.Kisel@compeng.uni-frankfurt.de *
//                                                                             *
// *****************************************************************************

#ifndef FTSCATARGET_H
#define FTSCATARGET_H

#include "PndFTSCATrackParam.h"
#include "PndFTSCAParam.h"
#include "CAFieldValue.h"

class FTSCATarget {

  /**
   * @class FTSCATarget
   *
   * The FTSCATarget class is the representation of target parameters
   */

 public:
  FTSCATarget() : fX0(0), fX1(0), fX2(0), fDX1(-1), fDX2(-1), fDQMom(-1), fB(), fNDF(-1){};
  FTSCATarget(const float &x0, const float &x1, const float &x2, const float &dx1, const float &dx2, const float &dqmom, const PndFTSCAParam &param, const int &ndf)
    : fX0(x0), fX1(x1), fX2(x2), fDX1(dx1), fDX2(dx2), fDQMom(dqmom), fNDF(ndf)
  {
    fB[0] = param.VtxFieldValue(0);
    fZB[0] = param.ZVtxFieldValue(0);
    fB[1] = param.VtxFieldValue(1);
    fZB[1] = param.ZVtxFieldValue(1);
  };

  float SetErrQMom(float v) { return fDQMom = v; }

  float X0() const { return fX0; }
  float X1() const { return fX1; }
  float X2() const { return fX2; }

  float Err2X1() const { return fDX1 * fDX1; }
  float Err2X2() const { return fDX2 * fDX2; }
  float Err2QMom() const { return fDQMom * fDQMom; }
  float DQMom() const { return fDQMom * fDQMom; }
  const CAFieldValue &B(int i = 0) const { return fB[i]; }
  const float_v &ZB(int i) const { return fZB[i]; }
  int NDF() const { return fNDF; }

 private:
  float fX0, fX1, fX2, fDX1, fDX2, fDQMom;

  CAFieldValue fB[2];
  float_v fZB[2]; // fZ position of the field point

  int fNDF; // 0 or 2, depends whether we consider target as a measurement
};

#endif
