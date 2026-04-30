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
// $Id: PndCATrackParamVector.h,v 1.3 2011/01/31 17:18:32 fisyak Exp $
// ************************************************************************
// This file is property of and copyright by the ALICE HLT Project        *
// ALICE Experiment at CERN, All rights reserved.                         *
// See cxx source for full Copyright notice                               *
//                                                                        *
//*************************************************************************

#ifndef PNDCATRACKPARAMVECTOR_H
#define PNDCATRACKPARAMVECTOR_H

#include "PndCADef.h"
#include "PndCAVector.h"
#include "PndCAMath.h"
#include "PndCAParam.h"
#include "PndCAX1X2MeasurementInfo.h"
#include "PndCAStation.h"
#include "PndCAHits.h"

// class PndCAParam;

class PndCATrackLinearisationVector;
// class PndCAParam;
// class PndCAHit;
class PndCAHitV;
class PndCATarget;

/**
 * @class PndCATrackParamVector
 *
 * PndCATrackParamVector class describes the track parametrisation
 * which is used by the PndCATracker slice tracker.
 *
 */
class PndCATrackParamVector {
  //     friend std::istream &operator>>( std::istream &, PndCATrackParamVector & );
  //     friend std::ostream &operator<<( std::ostream &, const PndCATrackParamVector & );
 public:
  PndCATrackParamVector() : fX(Vc::Zero), fSignCosPhi(Vc::Zero), fChi2(Vc::Zero), fNDF(Vc::Zero), fISec(Vc::Zero)
  {
    for (int i = 0; i < 5; ++i)
      fP[i].setZero();
    for (int i = 0; i < 15; ++i)
      fC[i].setZero();
  }

  void Reset()
  {
    fX.setZero();
    fSignCosPhi = float_v(1.f);
    fChi2.setZero();
    fNDF.setZero();
    // fISec.setZero();
    {
      for (int i = 0; i < 5; ++i)
        fP[i].setZero();
      for (int i = 0; i < 15; ++i)
        fC[i].setZero();
    }
  }
  void InitCovMatrix(float_v d2QMom = 0.f);
  void InitByTarget(const PndCATarget &target);
  void InitByHit(const PndCAHitV &hit, const PndCAParam &param, const float_v &dQMom);

  void InitDirection(float_v r0, float_v r1, float_v r2) // initialize direction parameters according to a given tangent vector
  {
    const float_v r = sqrt(r0 * r0 + r1 * r1);
    SetSinPhi(r1 / r);
    SetSignCosPhi(1.f); // r0/abs(r0) );
    SetDzDs(r2 / r);
  }

  struct PndCATrackFitParam {
    float_v fBethe;
    float_v fE;
    float_v fTheta2;
    float_v fEP2;
    float_v fSigmadE2;
    float_v fK22;
    float_v fK33;
    float_v fK43;
    float_v fK44;
  };

  float_v X() const { return fX; }
  float_v Y() const { return fP[0]; }
  float_v Z() const { return fP[1]; }
  float_v SinPhi() const { return fP[2]; }
  float_v DzDs() const { return fP[3]; }
  float_v QPt() const { return fP[4]; }

  float_v Angle() const { return fAlpha; }
  int_v ISec() const { return fISec; }

  float_v X0() const { return X(); }
  float_v X1() const { return Y(); }
  float_v X2() const { return Z(); }
  float_v Tx1() const { return SinPhi() / (SignCosPhi() * sqrt(1 - SinPhi() * SinPhi())); } // CHECKME
  float_v Tx2() const { return DzDs() / (SignCosPhi() * sqrt(1 - SinPhi() * SinPhi())); }   // dx2/dx0 = dz/dx
  float_v QMomentum() const { return QPt(); }                                               // used for triplets comparison

  /**
   * The sign of cos phi is always positive in the slice tracker. Only after coordinate
   * transformation can the sign change to negative.
   */
  float_v SignCosPhi() const { return fSignCosPhi; }
  float_v Chi2() const { return fChi2; }
  int_v NDF() const { return fNDF; }

  float_v Err2Y() const { return fC[0]; }
  float_v Err2Z() const { return fC[2]; }
  float_v Err2SinPhi() const { return fC[5]; }
  float_v Err2DzDs() const { return fC[9]; }
  float_v Err2QPt() const { return fC[14]; }

  float_v GetX() const { return fX; }
  float_v GetY() const { return fP[0]; }
  float_v GetZ() const { return fP[1]; }
  float_v GetSinPhi() const { return fP[2]; }
  float_v GetDzDs() const { return fP[3]; }
  float_v GetQPt() const { return fP[4]; }
  float_v GetSignCosPhi() const { return fSignCosPhi; }
  float_v GetChi2() const { return fChi2; }
  int_v GetNDF() const { return fNDF; }

  float_v GetKappa(const float_v &Bz) const { return fP[4] * Bz; }
  float_v GetCosPhiPositive() const { return CAMath::Sqrt(float_v(Vc::One) - SinPhi() * SinPhi()); }
  float_v GetCosPhi() const { return fSignCosPhi * CAMath::Sqrt(float_v(Vc::One) - SinPhi() * SinPhi()); }

  float_v Err2X1() const { return fC[0]; }
  float_v Err2X2() const { return fC[2]; }
  // float_v GetErr2SinPhi() const { return fC[5]; }
  // float_v GetErr2DzDs()   const { return fC[9]; }
  float_v Err2QMomentum() const { return fC[14]; }

  const float_v &Par(int i) const { return fP[i]; }
  const float_v &Cov(int i) const { return fC[i]; }

 private:
  friend class PndCATrackParam;
  const float_v *Par() const { return fP; }
  const float_v *Cov() const { return fC; }
  float_v *Par() { return fP; }
  float_v *Cov() { return fC; }

 public:
  void SetTrackParam(const PndCATrackParamVector &param, const float_m &m = float_m(true))
  {
    for (int i = 0; i < 5; i++)
      fP[i](m) = param.Par()[i];
    for (int i = 0; i < 15; i++)
      fC[i](m) = param.Cov()[i];
    fX(m) = param.X();
    fSignCosPhi(m) = param.SignCosPhi();
    fChi2(m) = param.GetChi2();
    fNDF(static_cast<int_m>(m)) = param.GetNDF();
    fISec(m) = param.fISec;
  }

  void SetTrackParam(const PndCATrackParamVector &p, int k, int m)
  {
    fX[k] = p.fX[m];
    fSignCosPhi = float_v(1.f);
    for (int i = 0; i < 5; i++)
      fP[i][k] = p.Par()[i][m];
    for (int i = 0; i < 15; i++)
      fC[i][k] = p.Cov()[i][m];
    fChi2[k] = p.fChi2[m];
    fNDF[k] = p.fNDF[m];
    fAlpha[k] = p.fAlpha[m];
    fISec[k] = p.fISec[m];
  }

  void SetPar(int i, const float_v &v) { fP[i] = v; }
  void SetPar(int i, const float_v &v, const float_m &m) { fP[i](m) = v; }
  void SetCov(int i, const float_v &v) { fC[i] = v; }
  void SetCov(int i, const float_v &v, const float_m &m) { fC[i](m) = v; }

  void SetX(const float_v &v) { fX = v; }
  void SetY(const float_v &v) { fP[0] = v; }
  void SetZ(const float_v &v) { fP[1] = v; }
  void SetX(const float_v &v, const float_m &m) { fX(m) = v; }
  void SetY(const float_v &v, const float_m &m) { fP[0](m) = v; }
  void SetZ(const float_v &v, const float_m &m) { fP[1](m) = v; }
  void SetSinPhi(const float_v &v) { fP[2] = v; }
  void SetSinPhi(const float_v &v, const float_m &m) { fP[2](m) = v; }
  void SetDzDs(const float_v &v) { fP[3] = v; }
  void SetDzDs(const float_v &v, const float_m &m) { fP[3](m) = v; }
  void SetQPt(const float_v &v) { fP[4] = v; }
  void SetQMomentum(const float_v &v) { SetQPt(v); }
  void SetQPt(const float_v &v, const float_m &m) { fP[4](m) = v; }
  void SetSignCosPhi(const float_v &v) { fSignCosPhi = v; }
  void SetSignCosPhi(const float_v &v, const float_m &m) { fSignCosPhi(m) = v; }
  void SetChi2(const float_v &v) { fChi2 = v; }
  void SetChi2(const float_v &v, const float_m &m) { fChi2(m) = v; }
  void SetNDF(int v) { fNDF = v; }
  void SetNDF(const int_v &v) { fNDF = v; }
  void SetNDF(const int_v &v, const int_m &m) { fNDF(m) = v; }

  void SetAngle(const float_v &v) { fAlpha = v; }
  void SetAngle(const float_v &v, const float_m &m) { fAlpha(m) = v; }

  void SetISec(const int_v &v) { fISec = v; }
  void SetISec(const int_v &v, const int_m &m) { fISec(m) = v; }

  void SetErr2Y(float_v v) { fC[0] = v; }
  void SetErr2Z(float_v v) { fC[2] = v; }
  void SetErr2QPt(float_v v) { fC[14] = v; }

  float_v GetDist2(const PndCATrackParamVector &t) const;
  float_v GetDistXZ2(const PndCATrackParamVector &t) const;

  float_v GetS(const float_v &x, const float_v &y, const float_v &Bz) const;

  void GetDCAPoint(const float_v &x, const float_v &y, const float_v &z, float_v *px, float_v *py, float_v *pz, const float_v &Bz) const;

  float_m Transport0(const int_v &ista, const PndCAParam &param, const float_m &mask = float_m(true));
  float_m Transport0(const PndCAHitV &hit, const PndCAParam &param, const float_m &mask = float_m(true));
  float_m Transport0(const PndCAHit &hit, const PndCAParam &p, const float_m &mask = float_m(true));

  float_m TransportToXWithMaterial(const float_v &x, const float_v &XOverX0, const float_v &XThimesRho, const float_v &Bz, const float maxSinPhi = .999f);

  float_m TransportToX(const float_v &x, const float_v &Bz, const float maxSinPhi = .999f, const float_m &mask = float_m(true));

  float_m TransportToX(const float_v &x, PndCATrackLinearisationVector &t0, const float_v &Bz, const float maxSinPhi = .999f, float_v *DL = 0, const float_m &mask = float_m(true));

  float_m Transport0ToX(const float_v &x, const float_v &Bz, const float_m &mask);

  float_m TransportToX(const float_v &x, const float_v &sinPhi0, const float_v &Bz, const float_v maxSinPhi = .999f, const float_m &mask = float_m(true));

  float_m TransportToXWithMaterial(const float_v &x, PndCATrackLinearisationVector &t0, PndCATrackFitParam &par, const float_v &XOverX0, const float_v &XThimesRho,
                                   const float_v &Bz, const float maxSinPhi = .999f, const float_m &mask = float_m(true));

  float_m TransportToXWithMaterial(const float_v &x, PndCATrackFitParam &par, const float_v &XOverX0, const float_v &XThimesRho, const float_v &Bz, const float maxSinPhi = .999f);

  float_m Rotate(const float_v &alpha, PndCATrackLinearisationVector &t0, const float maxSinPhi = .999f, const float_m &mask = float_m(true));
  float_m Rotate(const float_v &alpha, const float maxSinPhi = .999f, const float_m &mask = float_m(true));
  void RotateXY(float_v alpha, float_v &x, float_v &y, float_v &sin, const float_m &mask = float_m(true)) const;

  float_m FilterWithMaterial(const float_v &y, const float_v &z, float_v err2Y, float_v errYZ, float_v err2Z, float maxSinPhi = 0.999f, const float_m &mask = float_m(true),
                             const int_v &hitNDF = int_v(2), const float_v &chi2Cut = 10e10f); // filters 2-D measurement

  float_m FilterWithMaterial(const float_v &y, const float_v &z, const PndCAStripInfo &info, float_v err2, float maxSinPhi = 0.999f, const float_m &mask = float_m(true),
                             const float_v &chi2Cut = 10e10f); // filters 1-D measurement

  static float_v ApproximateBetheBloch(const float_v &beta2);
  static float_v BetheBlochGeant(const float_v &bg, const float_v &kp0 = 2.33f, const float_v &kp1 = 0.20f, const float_v &kp2 = 3.00f, const float_v &kp3 = 173e-9f,
                                 const float_v &kp4 = 0.49848f);
  static float_v BetheBlochSolid(const float_v &bg);
  static float_v BetheBlochGas(const float_v &bg);

  void CalculateFitParameters(PndCATrackFitParam &par, const float_v &mass = 0.13957f);
  float_m CorrectForMeanMaterial(const float_v &xOverX0, const float_v &xTimesRho, const PndCATrackFitParam &par, const float_m &_mask);

  // float_m FilterDelta( const float_m &mask, const float_v &dy, const float_v &dz,
  //     float_v err2Y, float_v err2Z, const float maxSinPhi = .999f );
  // float_m Filter( const float_m &mask, const float_v &y, const float_v &z,
  //     float_v err2Y, float_v err2Z, const float maxSinPhi = .999f );

  float_m
  FilterVtx(const float_v &xV, const float_v &yV, const PndCAX1X2MeasurementInfo &info, float_v &extrDx, float_v &extrDy, float_v J[], const float_m &active = float_m(true));
  float_m TransportJ0ToX0(const float_v &x0, const float_v &cBz, float_v &extrDy, float_v &extrDz, float_v J[], const float_m &active = float_m(true));

  float_m Transport(const int_v &ista, const PndCAParam &param, const float_m &mask = float_m(true));
  float_m Transport(const PndCAHitV &hit, const PndCAParam &p, const float_m &mask = float_m(true));
  float_m Filter(const PndCAHitV &hit, const PndCAParam &param, const float_m &mask = float_m(true), const float_v &chi2Cut = 10e10f);

  float_m Transport(const PndCAHit &hit, const PndCAParam &p, const float_m &mask = float_m(true));
  float_m Filter(const PndCAHit &hit, const PndCAParam &param, const float_m &mask = float_m(true), const float_v &chi2Cut = 10e10f);

  float_m Accept(const PndCAHit &hit, const PndCAParam &param, const float_m &mask = float_m(true), const float_v &chi2Cut = 10e10f) const;
  float_m AcceptWithMaterial(const float_v &y, const float_v &z, float_v err2Y, float_v errYZ, float_v err2Z, float maxSinPhi = 0.999f, const float_m &mask = float_m(true),
                             const int_v &hitNDF = int_v(2), const float_v &chi2Cut = 10e10f) const; // filters 2-D measurement

  float_m AcceptWithMaterial(const float_v &y, const float_v &z, const PndCAStripInfo &info, float_v err2, float maxSinPhi = 0.999f, const float_m &mask = float_m(true),
                             const float_v &chi2Cut = 10e10f) const; // filters 1-D measurement

  float_m AddTarget(const PndCATarget &target, const float_m &mask = float_m(true));

 public:
  float_v fX;          // x position
  float_v fSignCosPhi; // sign of cosPhi
  float_v fP[5];       // 'active' track parameters: Y, Z, SinPhi, DzDs, q/Pt
  float_v fC[15];      // the covariance matrix for Y,Z,SinPhi,..
  float_v fChi2;       // the chi^2 value
  int_v fNDF;          // the Number of Degrees of Freedom

  float_v fAlpha; // coor system
  int_v fISec;
};

//#include "debug.h"

inline float_m PndCATrackParamVector::TransportToX(const float_v &x, const float_v &sinPhi0, const float_v &Bz, const float_v maxSinPhi, const float_m &_mask)
{
  //* Transport the track parameters to X=x, using linearization at phi0 with 0 curvature,
  //* and the field value Bz
  //* maxSinPhi is the max. allowed value for |t0.SinPhi()|
  //* linearisation of trajectory t0 is also transported to X=x,
  //* returns 1 if OK
  //*

  // debugKF() << "Start TransportToX(" << x << ", " << _mask << ")\n" << *this << std::endl;

  const float_v &ey = sinPhi0;
  const float_v &dx = x - X();
  const float_v &exi = float_v(Vc::One) * CAMath::RSqrt(float_v(Vc::One) - ey * ey); // RSqrt

  const float_v &dxBz = dx * Bz;
  const float_v &dS = dx * exi;
  const float_v &h2 = dS * exi * exi;
  const float_v &h4 = .5f * h2 * dxBz;
  /// mvz start 23.01.2010
  //  const float_v &sinPhi = SinPhi() * (float_v( Vc::One ) - 0.5f * dxBz * QPt() *dxBz * QPt()/ ( float_v( Vc::One ) - SinPhi()*SinPhi() )) + dxBz * QPt();
  const float_v &sinPhi = SinPhi() + dxBz * QPt();
  /// mvz end 23.01.2010

  float_m mask = _mask && CAMath::Abs(exi) <= 1.e4f;
  mask &= ((CAMath::Abs(sinPhi) <= maxSinPhi) || (maxSinPhi <= 0.f));

  fX(mask) += dx;
  fP[0](mask) += dS * ey + h2 * (SinPhi() - ey) + h4 * QPt();
  fP[1](mask) += dS * DzDs();
  fP[2](mask) = sinPhi;

  // const float_v c00 = fC[0];
  // const float_v c11 = fC[2];
  const float_v c20 = fC[3];
  // const float_v c21 = fC[4];
  const float_v c22 = fC[5];
  // const float_v c30 = fC[6];
  const float_v c31 = fC[7];
  // const float_v c32 = fC[8];
  const float_v c33 = fC[9];
  const float_v c40 = fC[10];
  // const float_v c41 = fC[11];
  const float_v c42 = fC[12];
  // const float_v c43 = fC[13];
  const float_v c44 = fC[14];

  const float_v two(2.f);

  fC[0](mask) += h2 * h2 * c22 + h4 * h4 * c44 + two * (h2 * c20 + h4 * (c40 + h2 * c42));

  // fC[1] ( mask ) += h2 * c21 + h4 * c41 + dS * ( c30 + h2 * c32 + h4 * c43 );
  fC[2](mask) += dS * (two * c31 + dS * c33);

  fC[3](mask) += h2 * c22 + h4 * c42 + dxBz * (c40 + h2 * c42 + h4 * c44);
  // fC[4] ( mask ) += dS * c32 + dxBz * ( c41 + dS * c43 );
  const float_v &dxBz_c44 = dxBz * c44;
  fC[12](mask) += dxBz_c44;
  fC[5](mask) += dxBz * (two * c42 + dxBz_c44);

  // fC[6] ( mask ) += h2 * c32 + h4 * c43;
  fC[7](mask) += dS * c33;
  // fC[8] ( mask ) += dxBz * c43;
  // fC[9] ( mask ) = c33;

  fC[10](mask) += h2 * c42 + h4 * c44;
  // fC[11]( mask ) += dS * c43;
  // fC[13]( mask ) = c43;
  // fC[14]( mask ) = c44;

  // debugKF() << mask << "\n" << *this << std::endl;
  return mask;
}

inline float_m PndCATrackParamVector::Rotate(const float_v &alpha, const float maxSinPhi, const float_m &mask)
{
  // Rotate the coordinate system in XY on the angle alpha
  if ((CAMath::Abs(alpha) < 1e-6f || !mask).isFull())
    return mask;

  const float_v cA = CAMath::Cos(alpha);
  const float_v sA = CAMath::Sin(alpha);
  const float_v x0 = X(), y0 = Y(), sP = SinPhi(), cP = GetCosPhi();
  const float_v cosPhi = cP * cA + sP * sA;
  const float_v sinPhi = -cP * sA + sP * cA;

  float_m ReturnMask(mask);
  ReturnMask &= (!(CAMath::Abs(sinPhi) > maxSinPhi || CAMath::Abs(cosPhi) < 1.e-4f || CAMath::Abs(cP) < 1.e-4f));

  float_v tmp = alpha * 0.15915494f;            // 1/(2.f*3.1415f);
  ReturnMask &= abs(tmp - round(tmp)) < 0.167f; // allow turn by 60 degree only TODO scalar

  // float J[5][5] = { { j0, 0, 0,  0,  0 }, // Y
  //                  {  0, 1, 0,  0,  0 }, // Z
  //                  {  0, 0, j2, 0,  0 }, // SinPhi
  //                  {  0, 0, 0,  1,  0 }, // DzDs
  //                  {  0, 0, 0,  0,  1 } }; // Kappa

  const float_v j0 = cP / cosPhi;
  const float_v j2 = cosPhi / cP;
  const float_v d = SinPhi() - sP;

  SetX(x0 * cA + y0 * sA, ReturnMask);
  SetY(-x0 * sA + y0 * cA, ReturnMask);

  SetSinPhi(sinPhi + j2 * d, ReturnMask);

  fC[0](ReturnMask) *= j0 * j0;
  fC[1](ReturnMask) *= j0;
  fC[3](ReturnMask) *= j0;
  fC[6](ReturnMask) *= j0;
  fC[10](ReturnMask) *= j0;

  fC[3](ReturnMask) *= j2;
  fC[4](ReturnMask) *= j2;
  fC[5](ReturnMask) *= j2 * j2;
  fC[8](ReturnMask) *= j2;
  fC[12](ReturnMask) *= j2;

  fAlpha(ReturnMask) += alpha;

  return ReturnMask;
}

inline void PndCATrackParamVector::RotateXY(float_v alpha, float_v &x, float_v &y, float_v &sin, const float_m &mask) const
{
  //* Rotate the coordinate system in XY on the angle alpha
  if ((abs(alpha) < 1e-6f || !mask).isFull())
    return;

  const float_v cA = CAMath::Cos(alpha);
  const float_v sA = CAMath::Sin(alpha);

  x(mask) = (X() * cA + Y() * sA);
  y(mask) = (-X() * sA + Y() * cA);
  sin(mask) = -GetCosPhi() * sA + SinPhi() * cA;
}

inline float_m
PndCATrackParamVector::FilterVtx(const float_v &yV, const float_v &zV, const PndCAX1X2MeasurementInfo &info, float_v &extrDy, float_v &extrDz, float_v J[], const float_m &active)
{
  float_v zeta0, zeta1, S00, S10, S11, si;
  float_v F00, F10, F20, F30, F40, F01, F11, F21, F31, F41;
  float_v K00, K10, K20, K30, K40, K01, K11, K21, K31, K41;
  float_v &c00 = fC[0];
  float_v &c10 = fC[1];
  float_v &c11 = fC[2];
  float_v &c20 = fC[3];
  float_v &c21 = fC[4];
  float_v &c22 = fC[5];
  float_v &c30 = fC[6];
  float_v &c31 = fC[7];
  float_v &c32 = fC[8];
  float_v &c33 = fC[9];
  float_v &c40 = fC[10];
  float_v &c41 = fC[11];
  float_v &c42 = fC[12];
  float_v &c43 = fC[13];
  float_v &c44 = fC[14];

  zeta0 = Y() + extrDy - yV;
  zeta1 = Z() + extrDz - zV;

  // H = 1 0 J[0] J[1] J[2]
  //     0 1 J[3] J[4] J[5]

  // F = CH'
  F00 = c00;
  F01 = c10;
  F10 = c10;
  F11 = c11;
  F20 = J[0] * c22;
  F21 = J[3] * c22;
  F30 = J[1] * c33;
  F31 = J[4] * c33;
  F40 = J[2] * c44;
  F41 = J[5] * c44;

  S00 = info.C00() + F00 + J[0] * F20 + J[1] * F30 + J[2] * F40;
  S10 = info.C10() + F10 + J[3] * F20 + J[4] * F30 + J[5] * F40;
  S11 = info.C11() + F11 + J[3] * F21 + J[4] * F31 + J[5] * F41;

  si = 1.f / (S00 * S11 - S10 * S10);
  float_v S00tmp = S00;
  S00 = si * S11;
  S10 = -si * S10;
  S11 = si * S00tmp;

  fChi2(active) += zeta0 * zeta0 * S00 + 2.f * zeta0 * zeta1 * S10 + zeta1 * zeta1 * S11;

  K00 = F00 * S00 + F01 * S10;
  K01 = F00 * S10 + F01 * S11;
  K10 = F10 * S00 + F11 * S10;
  K11 = F10 * S10 + F11 * S11;
  K20 = F20 * S00 + F21 * S10;
  K21 = F20 * S10 + F21 * S11;
  K30 = F30 * S00 + F31 * S10;
  K31 = F30 * S10 + F31 * S11;
  K40 = F40 * S00 + F41 * S10;
  K41 = F40 * S10 + F41 * S11;

  fP[0](active) -= K00 * zeta0 + K01 * zeta1;
  fP[1](active) -= K10 * zeta0 + K11 * zeta1;
  fP[2](active) -= K20 * zeta0 + K21 * zeta1;
  fP[3](active) -= K30 * zeta0 + K31 * zeta1;
  fP[4](active) -= K40 * zeta0 + K41 * zeta1;

  c00(active) -= (K00 * F00 + K01 * F01);
  c10(active) -= (K10 * F00 + K11 * F01);
  c11(active) -= (K10 * F10 + K11 * F11);
  c20(active) = -(K20 * F00 + K21 * F01);
  c21(active) = -(K20 * F10 + K21 * F11);
  c22(active) -= (K20 * F20 + K21 * F21);
  c30(active) = -(K30 * F00 + K31 * F01);
  c31(active) = -(K30 * F10 + K31 * F11);
  c32(active) = -(K30 * F20 + K31 * F21);
  c33(active) -= (K30 * F30 + K31 * F31);
  c40(active) = -(K40 * F00 + K41 * F01);
  c41(active) = -(K40 * F10 + K41 * F11);
  c42(active) = -(K40 * F20 + K41 * F21);
  c43(active) = -(K40 * F30 + K41 * F31);
  c44(active) -= (K40 * F40 + K41 * F41);

  return active;
}

inline float_m PndCATrackParamVector::TransportJ0ToX0(const float_v &x, const float_v &cBz, float_v &extrDy, float_v &extrDz, float_v J[], const float_m &active)
{
  const float_v &ey = SinPhi();
  const float_v &dx = x - X();
  const float_v &exi = CAMath::RSqrt(float_v(Vc::One) - ey * ey); // RSqrt

  const float_v &dxBz = dx * cBz;
  const float_v &dS = dx * exi;
  const float_v &h2 = dS * exi * exi;
  const float_v &h4 = .5f * h2 * dxBz;

  float_m mask = active && CAMath::Abs(exi) <= 1.e4f;

  extrDy(mask) = dS * ey;
  extrDz(mask) = dS * DzDs();
  J[0](mask) = dS;
  J[1](mask) = 0;
  J[2](mask) = h4;
  J[3](mask) = dS;
  J[4](mask) = dS;
  J[5](mask) = 0;
  return mask;
}

typedef PndCATrackParamVector TrackParamVector;

// std::istream &operator>>( std::istream &in, PndCATrackParamVector &ot );
// std::ostream &operator<<( std::ostream &out, const PndCATrackParamVector &ot );

inline float_m PndCATrackParamVector::Transport0(const int_v &ista, const PndCAParam &param, const float_m &mask)
{
  float_m active = mask;
  // PndCATrackLinearisationVector tE( *this );
  // active &= TransportToX( param.GetR( ista, active ), tE, param.cBz(), 0.999f, nullptr, active );
  active &= Transport0ToX(param.GetR(ista, active), param.cBz(), active);
  return active;
}

inline float_m PndCATrackParamVector::Transport0(const PndCAHit &hit, const PndCAParam &param, const float_m &mask)
{
  if (((CAMath::Abs(-fAlpha + hit.Angle()) < 1e-6f && CAMath::Abs(-fX + hit.X0()) < 2e-4f) || !mask).isFull())
    return mask;
  float_m active = mask;
  // PndCATrackLinearisationVector tR( *this );
  active &= Rotate(-fAlpha + hit.Angle(), .999f, active);
  // active &= Rotate(  -fAlpha + hit.Angle(), tR, .999f, active );
  // active &= TransportToX( hit.X0(), tR,  param.cBz(), 0.999f, nullptr, active );
  active &= Transport0ToX(hit.X0(), param.cBz(), active);
  return active;
}

#endif
