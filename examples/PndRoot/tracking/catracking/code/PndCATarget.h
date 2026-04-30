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

#ifndef PNDCATARGET_H
#define PNDCATARGET_H

#include "PndCATrackParam.h"
#include "PndCAFieldValue.h"

class PndCATarget {
 public:
  PndCATarget() : fX0(0), fX1(0), fX2(0), fDX1(-1), fDX2(-1), fDQMom(-1), fB(), fNDF(-1){};
  PndCATarget(float x0, float x1, float x2, float dx1, float dx2, float dqmom, PndCAFieldValue b, int ndf)
    : fX0(x0), fX1(x1), fX2(x2), fDX1(dx1), fDX2(dx2), fDQMom(dqmom), fB(b), fNDF(ndf){};

  float SetErrQMom(float v) { return fDQMom = v; }

  float X0() const { return fX0; }
  float X1() const { return fX1; }
  float X2() const { return fX2; }

  float Err2X1() const { return fDX1 * fDX1; }
  float Err2X2() const { return fDX2 * fDX2; }
  float Err2QMom() const { return fDQMom * fDQMom; }
  float DQMom() const { return fDQMom * fDQMom; }
  const PndCAFieldValue &B() const { return fB; }
  int NDF() const { return fNDF; }

 private:
  float fX0, fX1, fX2, fDX1, fDX2, fDQMom;
  PndCAFieldValue fB;
  int fNDF; // 0 or 2, depends whether we consider target as a measurement
};

#endif
