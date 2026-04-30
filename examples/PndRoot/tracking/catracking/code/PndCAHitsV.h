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

#ifndef PNDCAHITSV_H
#define PNDCAHITSV_H

#include <vector>
using std::vector;

#include "PndCAHits.h"
#include "PndCAParameters.h"
#include "PndCATrackParamVector.h"

class PndCAHitV {
 public:
  PndCAHitV()
    : fIStation(-1), fId(Vc::Zero), fX0(Vc::Zero), fX1(Vc::Zero), fX2(Vc::Zero), fErr2X1(Vc::Zero), fErrX12(Vc::Zero), fErr2X2(Vc::Zero), fR(Vc::Zero), fErr2R(Vc::Zero),
      fErr2A(Vc::Zero), fU(Vc::Zero), fDR(Vc::Zero), fIsLeft(true), fISec(Vc::Zero), fAngle(Vc::Zero)
  {
  }

  PndCAHitV(const PndCAHit *hits, const float_m &valid);

  float_m IsValid() const { return fIStation >= 0; }

  char IStation() const
  {
    const float_m v = IsValid();
    assert(!v.isEmpty());
    return fIStation[v.firstOne()];
  }
  int_v IStations() const { return fIStation; } // is not needed by tracker

  uint_v Id() const { return fId; };

  float_v X1Corrected(const PndCATrackParamVector &p) const;

  float_v X0() const { return fX0; }
  float_v X1() const { return fX1; }
  float_v X2() const { return fX2; }

  float_v Err2X1() const { return fErr2X1; }
  float_v ErrX12() const { return fErrX12; }
  float_v Err2X2() const { return fErr2X2; }

  float_v U() const { return fU; }
  float_v R() const { return fR; }
  float_v DR() const { return fDR; }
  int_v ISec() const { return fISec; }
  float_v Err2R() const { return fErr2R; }
  float_v Err2A() const { return fErr2A; }
  float_m IsLeft() const { return fIsLeft; }

  float_v Angle() const { return fAngle; }

  void GetGlobalCoor(int iV, float &x, float &y, float &z) const
  {
    PndCAParameters::CALocalToGlobal(float(X0()[iV]), float(X1()[iV]), float(X2()[iV]), float(Angle()[iV]), x, y, z);
  }

 private:
  int_v fIStation;
  uint_v fId; // index of hits in an input array

  float_v fX0, fX1, fX2;             // local coordinates. X0 is normal to module, X2 is paralel to magnetic field
  float_v fErr2X1, fErrX12, fErr2X2; // cov matrix

  float_v fR;
  float_v fErr2R;
  float_v fErr2A;
  float_v fU, fDR;
  float_m fIsLeft;
  int_v fISec;
  float_v fAngle; // direction of hit station. Angle between normal and vertical axis. This angle defines local CS of the hit
};

inline float_v PndCAHitV::X1Corrected(const PndCATrackParamVector &p) const
{
  float_v x1 = fX1;
  const float_v xCorr = fR - fR * rsqrt(1 - p.SinPhi() * p.SinPhi());
  //  xCorr /= cos(3.f/180.f*3.141592); // currently neglect stereo angle
  x1(fIsLeft) += xCorr;
  x1(!fIsLeft) -= xCorr;
  return x1;
}

inline PndCAHitV::PndCAHitV(const PndCAHit *hits, const float_m &valid)
{
  int_v::Memory mIStation;
  mIStation = int_v(-1);

  uint_v::Memory mId;
  float_v::Memory mX1, mX2, mX0;
  float_v::Memory mErr2X1, mErrX12, mErr2X2;
  float_v::Memory mR, mU, mDR, mErr2R, mErr2A, mIsLeft;
  float_v::Memory mAlpha;
  float_v::Memory mIsUsed;
  int_v::Memory mISec;

  foreach_bit(unsigned short iV, valid)
  {
    const PndCAHit &h = hits[iV];

    mId[iV] = h.Id();
    mIStation[iV] = h.IStation();
    mX1[iV] = h.X1();
    mX2[iV] = h.X2();
    mX0[iV] = h.X0();

    mErr2X1[iV] = h.Err2X1();
    mErrX12[iV] = h.ErrX12();
    mErr2X2[iV] = h.Err2X2();
    mR[iV] = h.R();
    mU[iV] = h.U();
    mDR[iV] = h.DR();
    mErr2R[iV] = h.Err2R();
    mErr2A[iV] = h.Err2A();
    mIsLeft[iV] = h.IsLeft() ? 1.f : 0.f;
    mISec[iV] = h.ISec();
    mAlpha[iV] = h.Angle();
    mIsUsed[iV] = h.IsUsed() ? 1.f : 0.f;
  }
  fId = uint_v(mId);
  fIStation = int_v(mIStation);
  fX1 = float_v(mX1);
  fX2 = float_v(mX2);
  fX0 = float_v(mX0);
  fErr2X1 = float_v(mErr2X1);
  fErrX12 = float_v(mErrX12);
  fErr2X2 = float_v(mErr2X2);
  fR = float_v(mR);
  fErr2R = float_v(mErr2R);
  fErr2A = float_v(mErr2A);
  fIsLeft = (float_v(mIsLeft) == 1.f);
  fU = float_v(mU);
  fDR = float_v(mDR);
  fISec = int_v(mISec);
  fAngle = float_v(mAlpha);
}

#endif
