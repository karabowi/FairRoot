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
// $Id: PndFTSCATrackParam.h,v 1.5 2011/05/20 16:28:05 fisyak Exp $
// ************************************************************************
// This file is property of and copyright by the ALICE HLT Project        *
// ALICE Experiment at CERN, All rights reserved.                         *
// See cxx source for full Copyright notice                               *
//                                                                        *
//*************************************************************************

#ifndef PNDFTSCATRACKPARAM_H
#define PNDFTSCATRACKPARAM_H
#include <string.h>
#include "PndFTSCAMath.h"
#include "PndFTSCATrackParamVector.h"
#include "PndFTSCADef.h"
#include "FTSCAHits.h"

#ifdef PANDA_FTS
/**
 * @class PndFTSCATrackParam
 *
 * PndFTSCATrackParam class describes the track parametrisation
 * which is used by the PndFTSCATracker slice tracker.
 *
 */

class PndFTSCATrackParam {
  friend std::istream &operator>>(std::istream &, PndFTSCATrackParam &);
  friend std::ostream &operator<<(std::ostream &, const PndFTSCATrackParam &);

 public:
  PndFTSCATrackParam() { Reset(); }
  PndFTSCATrackParam(const TrackParamVector &v, int i)
    : x(v.X()[i]), y(v.Y()[i]), tx(v.Tx()[i]), ty(v.Ty()[i]), qp(v.QP()[i]), z(v.Z()[i]), C00(v.Cov(0)[i]), C10(v.Cov(1)[i]), C11(v.Cov(2)[i]), C20(v.Cov(3)[i]), C21(v.Cov(4)[i]),
      C22(v.Cov(5)[i]), C30(v.Cov(6)[i]), C31(v.Cov(7)[i]), C32(v.Cov(8)[i]), C33(v.Cov(9)[i]), C40(v.Cov(10)[i]), C41(v.Cov(11)[i]), C42(v.Cov(12)[i]), C43(v.Cov(13)[i]),
      C44(v.Cov(14)[i]), chi2(v.Chi2()[i]), ndf(v.NDF()[i]), fAlpha(v.Angle()[i])
  {
  }

  float X0() const { return z; }
  float X1() const { return x; }
  float X2() const { return y; }
  float Tx1() const { return tx; }
  float Tx2() const { return ty; }

  float X() const { return x; }
  float Y() const { return y; }
  float Z() const { return z; }
  float Tx() const { return tx; }
  float Ty() const { return ty; }
  float QP() const { return qp; }

  float Chi2() const { return chi2; }
  int NDF() const { return ndf; }

  float QMomentum() const { return QP(); } // used for triplets comparison
  float Angle() const { return fAlpha; }

  float Err2X1() const { return Err2X(); }
  float Err2X2() const { return Err2Y(); }
  float Err2Tx1() const { return Err2Tx(); }
  float Err2Tx2() const { return Err2Ty(); }

  float Err2X() const { return C00; }
  float Err2Y() const { return C11; }
  float Err2Tx() const { return C22; }
  float Err2Ty() const { return C33; }
  float Err2QP() const { return C44; }
  float Err2QMomentum() const { return Err2QP(); }

  float *Par() { return &x; }
  float *Cov() { return &C00; }

  float Par(int i) const
  {
    switch (i) {
    case 0: return x;
    case 1: return y;
    case 2: return tx;
    case 3: return ty;
    case 4: return qp;
    }
    assert(0);
    return -1;
  }

  float Cov(int i) const
  {
    switch (i) {
    case 0: return C00;
    case 1: return C10;
    case 2: return C11;
    case 3: return C20;
    case 4: return C21;
    case 5: return C22;
    case 6: return C30;
    case 7: return C31;
    case 8: return C32;
    case 9: return C33;
    case 10: return C40;
    case 11: return C41;
    case 12: return C42;
    case 13: return C43;
    case 14: return C44;
    }
    assert(0);
    return -1;
  }

  void SetCov(int i, float v)
  {
    switch (i) {
    case 0: C00 = v; break;
    case 1: C10 = v; break;
    case 2: C11 = v; break;
    case 3: C20 = v; break;
    case 4: C21 = v; break;
    case 5: C22 = v; break;
    case 6: C30 = v; break;
    case 7: C31 = v; break;
    case 8: C32 = v; break;
    case 9: C33 = v; break;
    case 10: C40 = v; break;
    case 11: C41 = v; break;
    case 12: C42 = v; break;
    case 13: C43 = v; break;
    case 14: C44 = v; break;
    }
  }

  void SetX(float v) { x = v; }
  void SetY(float v) { y = v; }
  void SetZ(float v) { z = v; }

  void SetTX(float v) { tx = v; }
  void SetTY(float v) { ty = v; }
  void SetQP(float v) { qp = v; }

  void SetChi2(float v) { chi2 = v; }
  void SetNDF(int v) { ndf = v; }

  void Print() const;

  bool TransportToX0(const float &x_, const float &Bz)
  {
    UNUSED_PARAM1(Bz);
    return TransportToX0Line(x_);
  };

  bool Transport(const FTSCAHit &hit, const PndFTSCAParam &param)
  {
    PndFTSCATrackParamVector v;
    v.ConvertTrackParamToVector(this, 1);
    const float_m &mask = v.Transport(hit, param);
    *this = PndFTSCATrackParam(v, 0);
    return mask[0];
  };

  bool Filter(const FTSCAHit &hit, const PndFTSCAParam &param)
  {
    PndFTSCATrackParamVector v;
    v.ConvertTrackParamToVector(this, 1);
    const float_m &mask = v.Filter(hit, param);
    *this = PndFTSCATrackParam(v, 0);
    return mask[0];
  };

  void Reset()
  {
    x = y = tx = ty = qp = z = C00 = C10 = C11 = C20 = C21 = C22 = C30 = C31 = C32 = C33 = C40 = C41 = C42 = C43 = C44 = 0.f;
    ndf = 0;
    chi2 = -1;
  }

  bool TransportToX0Line(float x0);

  bool IsValid() const { return chi2 != -1; }
  void SetAsInvalid() { chi2 = -1; }

  bool Rotate(float) { return 1; }; // don't need rotation in CBM

 private:
  float x, y, tx, ty, qp, z, C00, C10, C11, C20, C21, C22, C30, C31, C32, C33, C40, C41, C42, C43, C44;

  float chi2; // the chi^2 value
  short ndf;  // the Number of Degrees of Freedom

  float fAlpha; // coor system
};

inline bool PndFTSCATrackParam::TransportToX0Line(float x0_out)
{
  float dz = (x0_out - z);

  x += tx * dz;
  y += ty * dz;
  z += dz;

  const float dzC32_in = dz * C32;

  C21 += dzC32_in;
  C10 += dz * (C21 + C30);

  const float C20_in = C20;

  C20 += dz * C22;
  C00 += dz * (C20 + C20_in);

  const float C31_in = C31;

  C31 += dz * C33;
  C11 += dz * (C31 + C31_in);
  C30 += dzC32_in;

  C40 += dz * C42;
  C41 += dz * C43;

  return 1;
}

#else // PANDA_FTS

class PndFTSCATrackParam {
  friend std::istream &operator>>(std::istream &, PndFTSCATrackParam &);
  friend std::ostream &operator<<(std::ostream &, const PndFTSCATrackParam &);

 public:
  PndFTSCATrackParam() { Reset(); }
  PndFTSCATrackParam(const TrackParamVector &v, int i) : fX(v.X()[i]), fSignCosPhi(v.SignCosPhi()[i]), fChi2(v.Chi2()[i]), fNDF(v.NDF()[i]), fAlpha(v.Angle()[i])
  {
    for (int j = 0; j < 5; ++j)
      fP[j] = v.Par()[j][i];
    for (int j = 0; j < 15; ++j)
      fC[j] = v.Cov()[j][i];
  }

  float X0() const { return fX; }
  float X1() const { return Y(); }
  float X2() const { return Z(); }

  float X() const { return fX; }
  float Y() const { return fP[0]; }
  float Z() const { return fP[1]; }
  float SinPhi() const { return fP[2]; }
  float DzDs() const { return fP[3]; }
  float QPt() const { return fP[4]; }
  float QP() const { return QPt() / sqrt(DzDs() * DzDs() + 1); }
  float QMomentum() const { return QPt(); }

  float SignCosPhi() const { return fSignCosPhi; }
  float Chi2() const { return fChi2; }
  int NDF() const { return fNDF; }

  float Err2X1() const { return Err2Y(); }
  float Err2X2() const { return Err2Z(); }

  float Err2Y() const { return fC[0]; }
  float Err2Z() const { return fC[2]; }
  float Err2SinPhi() const { return fC[5]; }
  float Err2DzDs() const { return fC[9]; }
  float Err2QPt() const { return fC[14]; }

  float Angle() const { return fAlpha; }

  float Kappa(float Bz) const { return fP[4] * Bz; }
  float CosPhi() const { return fSignCosPhi * CAMath::Sqrt(1 - SinPhi() * SinPhi()); }

  float Err2QMomentum() const { return fC[14]; }

  const float *Par() const { return fP; }
  const float *Cov() const { return fC; }

  bool GetXYZPxPyPzQ(float &x, float &y, float &z, float &px, float &py, float &pz, int &q, float cov[21]) const;

  void SetSinPhi(float v) { fP[2] = v; }

  void GetDCAPoint(float x, float y, float z, float &px, float &py, float &pz, float Bz) const;

  bool TransportToX0(float x, float Bz, float maxSinPhi = .999);

  bool Rotate(float alpha, float maxSinPhi = .999);

  bool Transport(const FTSCAHit &hit, const PndFTSCAParam &param);

  bool IsValid() const { return fChi2 != -1; }
  void SetAsInvalid() { fChi2 = -1; }

 private:
  void Reset()
  {
    fX = 0;
    fSignCosPhi = 0;
    for (int i = 0; i < 5; i++)
      fP[i] = 0;
    for (int i = 0; i < 15; i++)
      fC[i] = 0;
    fChi2 = 0;
    fNDF = 0;
  }
  float S(float x, float y, float Bz) const;

  float fX;          // x position
  float fSignCosPhi; // sign of cosPhi   // phi = arctg (Dy/Dx)
  float fP[5];       // 'active' track parameters: Y, Z, SinPhi = dy/sqrt(dx*dx + dy*dy), DzDs = dz/sqrt(dx*dx + dy*dy), q/Pt
  float fC[15];      // the covariance matrix for Y,Z,SinPhi,..
  float fChi2;       // the chi^2 value
  int fNDF;          // the Number of Degrees of Freedom

  float fAlpha; // coor system
};

inline bool PndFTSCATrackParam::TransportToX0(float x, float Bz, float maxSinPhi)
{
  //* Transport the track parameters to X=x, using linearization at t0, and the field value Bz
  //* maxSinPhi is the max. allowed value for |t0.SinPhi()|
  //* linearisation of trajectory t0 is also transported to X=x,
  //* returns 1 if OK
  //*

  const float ex = CosPhi();
  const float ey = SinPhi();
  const float k = QPt() * Bz;
  const float dx = x - X();

  const float ey1 = k * dx + ey;

  // check for intersection with X=x

  if (CAMath::Abs(ey1) > maxSinPhi)
    return 0;

  float ex1 = CAMath::Sqrt(1.f - ey1 * ey1);
  if (ex < 0)
    ex1 = -ex1;

  const float dx2 = dx * dx;
  const float ss = ey + ey1;
  const float cc = ex + ex1;

  if ((CAMath::Abs(cc) < 1.e-4 || CAMath::Abs(ex) < 1.e-4 || CAMath::Abs(ex1) < 1.e-4))
    return 0;

  const float cci = 1.f / cc;
  const float exi = 1.f / ex;
  const float ex1i = 1.f / ex1;

  const float tg = ss * cci; // tan((phi1+phi)/2)

  const float dy = dx * tg;
  float dl = dx * CAMath::Sqrt(1.f + tg * tg);

  if (cc < 0)
    dl = -dl;
  float dSin = dl * k * 0.5;
  if (dSin > 1.f)
    dSin = 1.f;
  if (dSin < -1.f)
    dSin = -1.f;
  const float dS = (CAMath::Abs(k) > 1.e-4) ? (2 * CAMath::ASin(dSin) / k) : dl;
  const float dz = dS * DzDs();

  // float H0[5] = { 1,0, h2,  0, h4 };
  // float H1[5] = { 0, 1, 0, dS,  0 };
  // float H2[5] = { 0, 0, 1,  0, dxBz };
  // float H3[5] = { 0, 0, 0,  1,  0 };
  // float H4[5] = { 0, 0, 0,  0,  1 };

  const float h2 = dx * (1.f + ey * ey1 + ex * ex1) * exi * ex1i * cci;
  const float h4 = dx2 * (cc + ss * ey1 * ex1i) * cci * cci * Bz;
  const float dxBz = dx * Bz;

  fX = X() + dx;
  fP[0] = Y() + dy;
  fP[1] = Z() + dz;
  fP[2] = ey1;

#if 1
  const float c00 = fC[0];
  const float c10 = fC[1];
  const float c11 = fC[2];
  const float c20 = fC[3];
  const float c21 = fC[4];
  const float c22 = fC[5];
  const float c30 = fC[6];
  const float c31 = fC[7];
  const float c32 = fC[8];
  const float c33 = fC[9];
  const float c40 = fC[10];
  const float c41 = fC[11];
  const float c42 = fC[12];
  const float c43 = fC[13];
  const float c44 = fC[14];

  fC[0] = (c00 + h2 * h2 * c22 + h4 * h4 * c44 + 2.f * (h2 * c20 + h4 * c40 + h2 * h4 * c42));

  fC[1] = c10 + h2 * c21 + h4 * c41 + dS * (c30 + h2 * c32 + h4 * c43);
  fC[2] = c11 + 2.f * dS * c31 + dS * dS * c33;

  fC[3] = c20 + h2 * c22 + h4 * c42 + dxBz * (c40 + h2 * c42 + h4 * c44);
  fC[4] = c21 + dS * c32 + dxBz * (c41 + dS * c43);
  fC[5] = c22 + 2.f * dxBz * c42 + dxBz * dxBz * c44;

  fC[6] = c30 + h2 * c32 + h4 * c43;
  fC[7] = c31 + dS * c33;
  fC[8] = c32 + dxBz * c43;
  fC[9] = c33;

  fC[10] = c40 + h2 * c42 + h4 * c44;
  fC[11] = c41 + dS * c43;
  fC[12] = c42 + dxBz * c44;
  fC[13] = c43;
  fC[14] = c44;
#else
  fC[0] = (fC[0] + h2 * h2 * fC[5] + h4 * h4 * fC[14] + 2 * (h2 * fC[3] + h4 * fC[10] + h2 * h4 * fC[12]));

  fC[1] = fC[1] + h2 * fC[4] + h4 * fC[11] + dS * (fC[6] + h2 * fC[8] + h4 * fC[13]);
  fC[2] = fC[2] + 2 * dS * fC[7] + dS * dS * fC[9];

  fC[3] = fC[3] + h2 * fC[5] + h4 * fC[12] + dxBz * (fC[10] + h2 * fC[12] + h4 * fC[14]);
  fC[4] = fC[4] + dS * fC[8] + dxBz * (fC[11] + dS * fC[13]);
  fC[5] = fC[5] + 2 * dxBz * fC[12] + dxBz * dxBz * fC[14];

  fC[6] = fC[6] + h2 * fC[8] + h4 * fC[13];
  fC[7] = fC[7] + dS * fC[9];
  fC[8] = fC[8] + dxBz * fC[13];
  fC[9] = fC[9];

  fC[10] = fC[10] + h2 * fC[12] + h4 * fC[14];
  fC[11] = fC[11] + dS * fC[13];
  fC[12] = fC[12] + dxBz * fC[14];
  fC[13] = fC[13];
  fC[14] = fC[14];
#endif

  return 1;
}

#endif // PANDA_FTS

typedef PndFTSCATrackParam TrackParam;

std::istream &operator>>(std::istream &in, PndFTSCATrackParam &ot);
std::ostream &operator<<(std::ostream &out, const PndFTSCATrackParam &ot);

#endif
