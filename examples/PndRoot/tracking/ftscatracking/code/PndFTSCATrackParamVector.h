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
// $Id: PndFTSCATrackParamVector.h,v 1.3 2011/01/31 17:18:32 fisyak Exp $
// ************************************************************************
// This file is property of and copyright by the ALICE HLT Project        *
// ALICE Experiment at CERN, All rights reserved.                         *
// See cxx source for full Copyright notice                               *
//                                                                        *
//*************************************************************************

#ifndef PNDFTSCATRACKPARAMVECTOR_H
#define PNDFTSCATRACKPARAMVECTOR_H

class PndFTSCATrackParam;

#ifdef PANDA_FTS

#include "PndFTSCADef.h"
#include "PndFTSVector.h"
#include "PndFTSCAMath.h"
#include "L1XYMeasurementInfo.h"
#include "L1MaterialInfo.h"
#include "L1Field.h"

#include "FairField.h"
#include "FairRunAna.h"
// #include "CAX1X2MeasurementInfo.h"
#include "FTSCAStation.h"

class PndFTSCAParam;
class FTSCAHit;
class FTSCAHitV;
class FTSCATarget;

/**
 * @class PndFTSCATrackParamVector
 *
 * PndFTSCATrackParamVector class describes the track parametrisation
 * which is used by the PndFTSCATracker slice tracker.
 *
 */
class PndFTSCATrackParamVector {
  friend std::istream &operator>>(std::istream &, PndFTSCATrackParamVector &);
  friend std::ostream &operator<<(std::ostream &, const PndFTSCATrackParamVector &);

 public:
  PndFTSCATrackParamVector()
    : fX(0.f), fY(0.f), fTx(0.f), fTy(0.f), fQP(0.f), fZ(0.f), fC00(0.f), fC10(0.f), fC11(0.f), fC20(0.f), fC21(0.f), fC22(0.f), fC30(0.f), fC31(0.f), fC32(0.f), fC33(0.f),
      fC40(0.f), fC41(0.f), fC42(0.f), fC43(0.f), fC44(0.f), fChi2(0.f), fNDF(0.f)
  {
    fZB[0] = fZB[1] = 10e10;
    fDirection = true;
  }

  void SetTrackParam(const PndFTSCATrackParamVector &param, const float_m &m = float_m(true))
  {
    for (int i = 0; i < 5; i++)
      Par(i)(m) = param.Par(i);
    for (int i = 0; i < 15; i++)
      Cov(i)(m) = param.Cov(i);
    fX(m) = param.X();
    fChi2(m) = param.Chi2();
    fZ(m) = param.Z();
    fNDF(static_cast<int_m>(m)) = param.NDF();
    fAlpha(static_cast<int_m>(m)) = param.Angle();
  }

  void SetTrackParamOne(int iV, const PndFTSCATrackParamVector &param, int iVa)
  {
    for (int i = 0; i < 5; i++)
      Par(i)[iV] = param.Par(i)[iVa];
    for (int i = 0; i < 15; i++)
      Cov(i)[iV] = param.Cov(i)[iVa];
    fX[iV] = param.X()[iVa];
    fZ[iV] = param.Z()[iVa];
    fChi2[iV] = param.Chi2()[iVa];
    fNDF[iV] = param.NDF()[iVa];
    fAlpha[iV] = param.Angle()[iVa];
  }

  void ConvertTrackParamToVector(PndFTSCATrackParam t0[float_v::Size], int nTracksV);

  void PrintCovMat()
  {
    cout << "CovMat " << endl;
    // for ( int i = 0; i < 15; i++ )
    cout << Cov(0) << endl;
    cout << Cov(1) << " " << Cov(2) << endl;
    cout << Cov(3) << " " << Cov(4) << " " << Cov(5) << endl;
    cout << Cov(6) << " " << Cov(7) << " " << Cov(8) << " " << Cov(9) << endl;
    cout << Cov(10) << " " << Cov(11) << " " << Cov(12) << " " << Cov(13) << " " << Cov(14) << endl;
    /*cout << Cov(0)[0] << endl;
    cout << Cov(1)[0] << " "<< Cov(2)[0] << endl;
    cout << Cov(3)[0] << " "<< Cov(4)[0] <<" "<< Cov(5)[0] << endl;
    cout << Cov(6)[0] << " "<< Cov(7)[0] <<" "<< Cov(8)[0] <<" "<< Cov(9)[0] << endl;
    cout << Cov(10)[0] <<" "<< Cov(11)[0] <<" "<< Cov(12)[0] <<" "<< Cov(13)[0] <<" "<< Cov(14)[0];*/
  }

  void InitCovMatrix(float_v d2QMom = 0.f);
  void InitByTarget(const FTSCATarget &target);
  void InitByHit(const FTSCAHitV &hit, const PndFTSCAParam &param, const float_v &dQP);

  void InitDirection(float_v r0, float_v r1, float_v r2)
  { // initialize direction parameters according to a given tangent vector r = { r0, r1, r2 }
    float_m mask = (r0 > 0.f);
    float_v tx(Vc::Zero);
    tx(mask) = r1 / r0;
    float_v ty(Vc::Zero);
    ty(mask) = r2 / r0;
    SetTx(tx);
    SetTy(ty);
  }

  float_v X() const { return fX; }
  float_v Y() const { return fY; }
  float_v Z() const { return fZ; }
  float_v Tx() const { return fTx; }
  float_v Ty() const { return fTy; }
  float_v QP() const { return fQP; }

  float_v Bx() const { return fBx; }
  float_v By() const { return fBy; }

  float_v Chi2() const { return fChi2; }
  int_v NDF() const { return fNDF; }

  float_v Angle() const { return fAlpha; }

  float_v X0() const { return Z(); }
  float_v X1() const { return X(); }
  float_v X2() const { return Y(); }
  float_v Tx1() const { return Tx(); }
  float_v Tx2() const { return Ty(); }
  float_v QMomentum() const { return QP(); } // used for triplets comparison
  float_v SinPhi() const
  { // dx/dxz
    const float_v tx12 = Tx1() * Tx1();
    return sqrt(tx12 / (1.f + tx12));
  }

  const float_v &Par(int i) const
  {
    switch (i) {
    case 0: return fX;
    case 1: return fY;
    case 2: return fTx;
    case 3: return fTy;
    case 4: return fQP;
    case 5: return fZ;
    };
    assert(0);
    return fX;
  }

  const float_v &Cov(int i) const
  {
    switch (i) {
    case 0: return fC00;
    case 1: return fC10;
    case 2: return fC11;
    case 3: return fC20;
    case 4: return fC21;
    case 5: return fC22;
    case 6: return fC30;
    case 7: return fC31;
    case 8: return fC32;
    case 9: return fC33;
    case 10: return fC40;
    case 11: return fC41;
    case 12: return fC42;
    case 13: return fC43;
    case 14: return fC44;
    };
    assert(0);
    return fC00;
  }

  float_v Err2X1() const { return Err2X(); }
  float_v Err2X2() const { return Err2Y(); }

  float_v Err2X() const { return fC00; }
  float_v Err2Y() const { return fC11; }
  float_v Err2QP() const { return fC44; }
  float_v Err2QMomentum() const { return Err2QP(); }

  void SetX(const float_v &v) { fX = v; }
  void SetY(const float_v &v) { fY = v; }
  void SetZ(const float_v &v) { fZ = v; }
  void SetTx(const float_v &v) { fTx = v; }
  void SetTy(const float_v &v) { fTy = v; }
  void SetQP(const float_v &v) { fQP = v; }
  void SetQMomentum(const float_v &v) { fQP = v; }
  void SetBx(const float_v &v) { fBx = v; }
  void SetBy(const float_v &v) { fBy = v; }

  void SetChi2(const float_v &v) { fChi2 = v; }
  void SetNDF(int v) { fNDF = v; }
  void SetNDF(const int_v &v) { fNDF = v; }

  void SetAngle(const float_v &v) { fAlpha = v; }

  void SetPar(int i, float_v v)
  {
    switch (i) {
    case 0: fX = v; break;
    case 1: fY = v; break;
    case 2: fTx = v; break;
    case 3: fTy = v; break;
    case 4: fQP = v; break;
    }
  }
  void SetCov(int i, float_v v)
  {
    switch (i) {
    case 0: fC00 = v; break;
    case 1: fC10 = v; break;
    case 2: fC11 = v; break;
    case 3: fC20 = v; break;
    case 4: fC21 = v; break;
    case 5: fC22 = v; break;
    case 6: fC30 = v; break;
    case 7: fC31 = v; break;
    case 8: fC32 = v; break;
    case 9: fC33 = v; break;
    case 10: fC40 = v; break;
    case 11: fC41 = v; break;
    case 12: fC42 = v; break;
    case 13: fC43 = v; break;
    case 14: fC44 = v; break;
    }
  }

  void SetDirection(bool b) { fDirection = b; }

  float_v &Par(int i)
  {
    switch (i) {
    case 0: return fX;
    case 1: return fY;
    case 2: return fTx;
    case 3: return fTy;
    case 4: return fQP;
    };
    assert(0);
    return fX;
  }

  float_v &Cov(int i)
  {
    switch (i) {
    case 0: return fC00;
    case 1: return fC10;
    case 2: return fC11;
    case 3: return fC20;
    case 4: return fC21;
    case 5: return fC22;
    case 6: return fC30;
    case 7: return fC31;
    case 8: return fC32;
    case 9: return fC33;
    case 10: return fC40;
    case 11: return fC41;
    case 12: return fC42;
    case 13: return fC43;
    case 14: return fC44;
    };
    assert(0);
    return fC00;
  }

  void SetX(const float_v &v, const float_m &m) { fX(m) = v; }
  void SetY(const float_v &v, const float_m &m) { fY(m) = v; }
  void SetZ(const float_v &v, const float_m &m) { fZ(m) = v; }
  void SetTx(const float_v &v, const float_m &m) { fTx(m) = v; }
  void SetTy(const float_v &v, const float_m &m) { fTy(m) = v; }
  void SetQP(const float_v &v, const float_m &m) { fQP(m) = v; }
  void SetQMomentum(const float_v &v, const float_m &m) { fQP(m) = v; }

  void SetChi2(const float_v &v, const float_m &m) { fChi2(m) = v; }
  void SetNDF(const int_v &v, const int_m &m) { fNDF(m) = v; }

  void SetField(int i, const CAFieldValue &b, const float_v &zb)
  {
    fB[i] = b;
    fZB[i] = zb;
  }

  void UpdateFieldValues(const FTSCAHitV &hit, const PndFTSCAParam &param, L1FieldRegion &f, const float_m &mask);
  void UpdateFieldValues(const FTSCAHitV &hit, int_v &iVrt, float_v &zVirtualStation, const PndFTSCAParam &param, L1FieldRegion &f, const float_m &mask);

  void SetCovX12(float_v v00, float_v v10, float_v v11)
  {
    fC00 = v00;
    fC10 = v10;
    fC11 = v11;
  }

  //  float_m Transport(const int_v& ista, const int_v& iVSta, const PndFTSCAParam& param, const float_m&mask = float_m( true ) );
  // float_m Transport( const int_v& ista, const PndFTSCAParam& param, const float_m &mask = float_m( true ) );

  float_m Transport(const FTSCAHitV &hit, const PndFTSCAParam &p, float_v &qp0, const float_m &mask = float_m(true));
  float_m Transport(const FTSCAHitV &hit, const PndFTSCAParam &p, const float_m &mask = float_m(true));

  float_m TransportByLine(const FTSCAHitV &hit, const PndFTSCAParam &param, const float_m &mask = float_m(true));

  float_m Filter(const FTSCAHitV &hit, const PndFTSCAParam &param, const float_m &mask = float_m(true), const float_v &chi2Cut = 10e10f);

  float_m Transport(const FTSCAHit &hit, const PndFTSCAParam &p, const float_m &mask = float_m(true));
  float_m Filter(const FTSCAHit &hit, const PndFTSCAParam &param, const float_m &mask = float_m(true), const float_v &chi2Cut = 10e10f);

  float_m AddTarget(const FTSCATarget &t, const float_m &mask = float_m(true));

 private:
  //  float_m Transport( const FTSCAHitV& hit, const L1FieldRegion& F, const PndFTSCAParam& p, const float_m &mask = float_m( true ) ); // dbg TODO delme

  float_m TransportToX0WithMaterial(const float_v &x0, const L1FieldRegion &F, const L1MaterialInfo &material, float_v &qp0, const float_m &mask = float_m(true));
  float_m TransportToX0(const float_v &x0, const L1FieldRegion &F, const float_v &qp0, const float_m &mask = float_m(true));
  float_m RK4TransportToX0(const float_v &x0_out, const L1FieldRegion &F, const float_v &qp0, const float_m &mask = float_m(true));
  float_m TransportToX0Line(const float_v &x0, const float_m &mask = float_m(true));
  float_m PassMaterial(const L1MaterialInfo &info, const float_v &qp0, const float_m &mask = float_m(true));
  void EnergyLossCorrection(const float_v &mass2, const float_v &radThick, float_v &qp0, float_v direction, const float_m &mask);
  float_v ApproximateBetheBloch(const float_v &bg2);

  float_m Filter(const float_v &y0, const float_v &z0, const float_v &r, const FTSCAStripInfoVector &info, float_v err2, const float_m &active, const float_v &chi2Cut = 10e10f);

  float_m FilterVtx(const float_v &xV, const float_v &yV, const L1XYMeasurementInfo &info, float_v &extrDx, float_v &extrDy, float_v J[], const float_m &active = float_m(true));

  float_m TransportJXY0ToX0(const float_v &x0, const L1FieldRegion &F, float_v &extrDx, float_v &extrDy, float_v &J04, float_v &J14, const float_m &active = float_m(true));

  float_v fX, fY, fTx, fTy, fQP, fZ, fC00, fC10, fC11, fC20, fC21, fC22, fC30, fC31, fC32, fC33, fC40, fC41, fC42, fC43, fC44;

  float_v fChi2; // the chi^2 value
  int_v fNDF;    // the Number of Degrees of Freedom

  float_v fBx;
  float_v fBy;
  CAFieldValue fB[2]; // field at two previous points, which track passed
  float_v fZB[2];     // fZ position of the filld point
  bool fDirection;
  float_v fAlpha; // coor system
  float_v fDelta;
};

#include "debug.h"

inline float_m PndFTSCATrackParamVector::TransportToX0Line(const float_v &x0_out, const float_m &mask)
{
  float_v dz = (x0_out - fZ);
  // cout<<"dz "<<dz<<endl;

  fX(mask) += fTx * dz;
  fY(mask) += fTy * dz;
  fZ(mask) += dz;

  const float_v dzC32_in = dz * fC32;

  fC21(mask) += dzC32_in;
  fC10(mask) += dz * (fC21 + fC30);

  const float_v C20_in = fC20;

  fC20(mask) += dz * fC22;
  fC00(mask) += dz * (fC20 + C20_in);

  const float_v C31_in = fC31;

  fC31(mask) += dz * fC33;
  fC11(mask) += dz * (fC31 + C31_in);
  fC30(mask) += dzC32_in;

  fC40(mask) += dz * fC42;
  fC41(mask) += dz * fC43;

  return mask;
}

inline float_m PndFTSCATrackParamVector::Filter(
  const float_v &x0, const float_v &y0, const float_v &r, const FTSCAStripInfoVector &info, float_v err2, const float_m &active,
  const float_v & /*chi2Cut*/) //[R.K. 9/2018] unused
                               // Adds the tube measurement with the Kalman filter
                               // @beta is angle between the strip and z-axis, clockwise. The wire equation is: {x,y,z} - {x0,y0,z0} = t*e_s, where ort e_s = { 0, -sinB, cos B }
{
  // linearize track in current point, which must be x == x0
  // distance between wire and track are : r = - ({x,y,z} - {x0,y0,z0}) * e_o / |e_o|, where ort e_o = |[e_t x e_s]|

  float_m success = active;

  success &= (err2 > 1.e-8f);

  // const float_v& h0 = info.cos;
  // const float_v& h1 = info.sin;
  float_v h0(Vc::Zero);
  float_v h1(Vc::Zero);
  h0(active) = info.cos;
  h1(active) = info.sin;
  /*foreach_bit(unsigned short iV, active) {
    h0[iV] = info.cos;
    h1[iV] = info.sin;
  }*/

  const float_v tx = h0 * fTx + h1 * fTy;

  float_v zeta;

  // float_v Sign_1 = float_v(0.0);

  float_v rCorrection = sqrt(1.f + tx * tx);

  float_v Diff = h0 * (fX - x0) + h1 * (fY - y0);
  float_v zeta_1, zeta_2;
  zeta_1 = h0 * (fX - x0) + h1 * (fY - y0) - r * rCorrection;
  zeta_2 = h0 * (fX - x0) + h1 * (fY - y0) + r * rCorrection;

  for (int iDiff = 0; iDiff < 4; iDiff++) {
    float Diff_f = (float)Diff[iDiff];
    if (Diff_f > 0.f) {
      // zeta = h0*(fX - x0) + h1*(fY - y0) - r*rCorrection;
      zeta[iDiff] = zeta_1[iDiff];
    } else {
      zeta[iDiff] = zeta_2[iDiff];
    }

  } // iDiff

  // const float_v zeta2 = h0*(fX - x0) + h1*(fY - y0) + r*rCorrection;

  if (err2[0] < 0.01f)
    err2[0] = err2[0] + r[0] * r[0];
  err2 *= rCorrection * rCorrection;

  float_v wi, zetawi, HCH;
  float_v F0, F1, F2, F3, F4;
  float_v K1, K2, K3, K4;

  if (fC00[0] < 0.f)
    fC00[0] = 0.f;

  // F = CH'
  F0 = h0 * fC00 + h1 * fC10;
  F1 = h0 * fC10 + h1 * fC11;

  // HCH = ( F0*h0 + F1*h1 );
  HCH = (h0 * h0 * fC00 + 2.f * h0 * h1 * fC10) + h1 * h1 * fC11;

  F2 = h0 * fC20 + h1 * fC21;
  F3 = h0 * fC30 + h1 * fC31;
  F4 = h0 * fC40 + h1 * fC41;

  float_v dChi2(Vc::Zero);
#if 0 // use mask
  cout<<"we don't have to be here for now \n";
  const float_m mask = success && (HCH > err2 * 16.f);
  HCH(mask) += err2;
  wi(mask) = 1.f/HCH ;
  zetawi(mask) = zeta *wi;
  dChi2(mask) += (zeta * zetawi);
  fChi2 += dChi2;
#else
  wi = 1.f / (err2 + HCH);
  zetawi = zeta * wi;
  dChi2(success) += zeta * zetawi;
  // success &= dChi2 < chi2Cut;
  fChi2 += dChi2;
  // fChi2(success) +=  zeta * zetawi;
#endif // 0

  // success &= fChi2 < chi2Cut;

  K1 = F1 * wi;
  K2 = F2 * wi;
  K3 = F3 * wi;
  K4 = F4 * wi;

  fX(success) -= F0 * zetawi;
  fY(success) -= F1 * zetawi;
  fTx(success) -= F2 * zetawi;
  fTy(success) -= F3 * zetawi;
  fQP(success) -= F4 * zetawi;

  fC00(success) -= F0 * F0 * wi;
  fC10(success) -= K1 * F0;
  fC11(success) -= K1 * F1;
  fC20(success) -= K2 * F0;
  fC21(success) -= K2 * F1;
  fC22(success) -= K2 * F2;
  fC30(success) -= K3 * F0;
  fC31(success) -= K3 * F1;
  fC32(success) -= K3 * F2;
  fC33(success) -= K3 * F3;
  fC40(success) -= K4 * F0;
  fC41(success) -= K4 * F1;
  fC42(success) -= K4 * F2;
  fC43(success) -= K4 * F3;
  fC44(success) -= K4 * F4;

  fNDF(static_cast<int_m>(success)) += 1;

  return success;
}

inline float_m PndFTSCATrackParamVector::TransportToX0WithMaterial(const float_v &x0, const L1FieldRegion &F, const L1MaterialInfo &material, float_v &qp0, const float_m &mask)
{ // TODO material
  float_m active = mask;
  active &= TransportToX0(x0, F, qp0, active);
  // active &= TransportToX0Line(x0, active);
  // active &= RK4TransportToX0( x0, F, qp0, active );
  active &= PassMaterial(material, qp0, active);
  const float_v mass2 = 0.13957f * 0.13957f;
  float_v direction = -1.f;
  if (fDirection)
    direction = 1.f;
  // std::cout << "qp " << qp0[0] << " " << " fQP " << fQP[0]<<"     ";
  EnergyLossCorrection(mass2, material.RadThick, qp0, direction, active);
  // std::cout << qp0[0] << " " << fQP[0] << std::endl;
  return active;
}

inline float_m PndFTSCATrackParamVector::RK4TransportToX0(const float_v &x0_out, const L1FieldRegion & /*F*/, const float_v & /*qp0*/, const float_m &mask) //[R.K. 9/2018] 2 unused
{
  // Forth-order Runge-Kutta method for solution of the equation
  // of motion of a particle with parameter qp = Q /P
  //              in the magnetic field B()
  //
  //        | x |          tx
  //        | y |          ty
  // d/dz = | tx| = ft * ( ty * ( B(3)+tx*b(1) ) - (1+tx**2)*B(2) )
  //        | ty|   ft * (-tx * ( B(3)+ty+b(2)   - (1+ty**2)*B(1) )  ,
  //
  //   where  ft = c_light*qp*sqrt ( 1 + tx**2 + ty**2 ) .
  //
  //  In the following for RK step  :
  //
  //     x=x[0], y=x[1], tx=x[3], ty=x[4].
  //
  //========================================================================

  //   const float ZERO = 0.0, ONE = 1.;

  // RK4ORDER
  float_v xOut[5];
  float_v Fmat[25];
  const float_v fC = 0.000299792458f;

  float_v coef[4];
  coef[0] = 0.f;
  coef[1] = 0.5f;
  coef[2] = 0.5f;
  coef[3] = 1.f;

  float_v xIn[4];
  xIn[0] = fX;
  xIn[2] = fTx;
  xIn[1] = fY;
  xIn[3] = fTy;
  xIn[4] = fQP;

  float_v Ax[4], Ay[4];
  float_v dAx_dtx[4], dAy_dtx[4], dAx_dty[4], dAy_dty[4];
  float_v k[4][4];

  float_v h = x0_out - fZ;
  // cout<<"Step h: "<<h<<endl;
  float_v hC = h * fC;
  float_v hCqp = h * fC * xIn[4];
  float_v x0[4];

  float_v x[4];
  x[0] = fX;
  x[2] = fTx;
  x[1] = fY;
  x[3] = fTy; // x[4] = fQP;

  for (unsigned int iStep = 0; iStep < 4; iStep++) { // 1
    if (iStep > 0) {

      x[0] = xIn[0] + coef[iStep] * k[0][iStep - 1];
      x[1] = xIn[1] + coef[iStep] * k[1][iStep - 1];
      x[2] = xIn[2] + coef[iStep] * k[2][iStep - 1];
      x[3] = xIn[3] + coef[iStep] * k[3][iStep - 1];
    }

    float_v bx(0.f), by(0.f), bz(0.f);
    double p[3];
    double b[3];
    for (int xxx = 0; xxx < float_v::Size; xxx++) {
      //        if (fabs(x[0][xxx])>0.001)
      {
        // double p[3] = {fX[ctr], fY[ctr], fZ[ctr]};
        // double b[3] = {0., 0., 0.};
        p[0] = x[0][xxx];
        p[1] = x[1][xxx];
        p[2] = fZ[xxx] + coef[iStep][xxx] * h[xxx];
        b[0] = 0;
        b[1] = 0;
        b[2] = 0;
        FairRunAna::Instance()->GetField()->Field(p, b); // CbmKF::Instance()->GetMagneticField()->GetFieldValue(p,b);
        bx[xxx] = b[0];
        by[xxx] = b[1];
        bz[xxx] = b[2];
      }
    }
    // fField->GetFieldValue(x[0], x[1], zIn + coef[iStep] * h, Bx, By, Bz);
    float_v tx = x[2];
    float_v ty = x[3];
    float_v txtx = tx * tx;
    float_v tyty = ty * ty;
    float_v txty = tx * ty;
    float_v txtxtyty1 = 1.f + txtx + tyty;
    float_v t1 = sqrt(txtxtyty1);
    float_v t2 = 1.f / txtxtyty1;

    Ax[iStep] = (txty * bx + ty * bz - (1.f + txtx) * by) * t1;
    Ay[iStep] = (-txty * by - tx * bz + (1.f + tyty) * bx) * t1;

    dAx_dtx[iStep] = Ax[iStep] * tx * t2 + (ty * bx - 2.f * tx * by) * t1;
    dAx_dty[iStep] = Ax[iStep] * ty * t2 + (tx * bx + bz) * t1;
    dAy_dtx[iStep] = Ay[iStep] * tx * t2 + (-ty * by - bz) * t1;
    dAy_dty[iStep] = Ay[iStep] * ty * t2 + (-tx * by + 2.f * ty * bx) * t1;

    k[0][iStep] = tx * h;
    k[1][iStep] = ty * h;
    k[2][iStep] = Ax[iStep] * hCqp;
    k[3][iStep] = Ay[iStep] * hCqp;

  } // 1

  // in + k[0]/6.f + k[1]/3.f + k[2]/3.f + k[3]/6.f;
  // for (unsigned int i = 0; i < 4; i++) { xOut[i] = CalcOut(xIn[i], k[i]); }
  xOut[0] = xIn[0] + k[0][0] / 6.f + k[0][1] / 3.f + k[0][2] / 3.f + k[0][3] / 6.f;
  xOut[1] = xIn[1] + k[1][0] / 6.f + k[1][1] / 3.f + k[1][2] / 3.f + k[1][3] / 6.f;
  xOut[2] = xIn[2] + k[2][0] / 6.f + k[2][1] / 3.f + k[2][2] / 3.f + k[2][3] / 6.f;
  xOut[3] = xIn[3] + k[3][0] / 6.f + k[3][1] / 3.f + k[3][2] / 3.f + k[3][3] / 6.f;
  xOut[4] = xIn[4];

  fX(mask) = xOut[0];
  fY(mask) = xOut[1];
  fTx(mask) = xOut[2];
  fTy(mask) = xOut[3];
  fQP(mask) = xOut[4];
  fZ(mask) += h;
  // Calculation of the derivatives

  // derivatives dx/dx and dx/dy
  // dx/dx
  Fmat[0] = 1.f;
  Fmat[5] = 0.f;
  Fmat[10] = 0.f;
  Fmat[15] = 0.f;
  Fmat[20] = 0.f;
  // dx/dy
  Fmat[1] = 0.f;
  Fmat[6] = 1.f;
  Fmat[11] = 0.f;
  Fmat[16] = 0.f;
  Fmat[21] = 0.f;
  // end of derivatives dx/dx and dx/dy

  // Derivatives dx/tx
  x[0] = x0[0] = 0.f;
  x[1] = x0[1] = 0.f;
  x[2] = x0[2] = 1.f;
  x[3] = x0[3] = 0.f;
  for (unsigned int iStep = 0; iStep < 4; iStep++) { // 2
    if (iStep > 0) {
      x[0] = xIn[0] + coef[iStep] * k[0][iStep - 1];
      x[1] = xIn[1] + coef[iStep] * k[1][iStep - 1];
      x[3] = xIn[3] + coef[iStep] * k[3][iStep - 1];
    }

    k[0][iStep] = x[2] * h;
    k[1][iStep] = x[3] * h;
    // k[2][iStep] = (dAx_dtx[iStep] * x[2] + dAx_dty[iStep] * x[3]) * hCqp;
    k[3][iStep] = (dAy_dtx[iStep] * x[2] + dAy_dty[iStep] * x[3]) * hCqp;
  } // 2

  Fmat[2] = x0[0] + k[0][0] / 6.f + k[0][1] / 3.f + k[0][2] / 3.f + k[0][3] / 6.f;

  Fmat[7] = x0[1] + k[1][0] / 6.f + k[1][1] / 3.f + k[1][2] / 3.f + k[1][3] / 6.f;
  Fmat[12] = 1.f;

  Fmat[17] = x0[3] + k[3][0] / 6.f + k[3][1] / 3.f + k[3][2] / 3.f + k[3][3] / 6.f;
  Fmat[22] = 0.f;
  // end of derivatives dx/dtx

  // Derivatives    dx/ty
  x[0] = x0[0] = 0.f;
  x[1] = x0[1] = 0.f;
  x[2] = x0[2] = 0.f;
  x[3] = x0[3] = 1.f;
  for (unsigned int iStep = 0; iStep < 4; iStep++) { // 4
    if (iStep > 0) {
      x[0] = xIn[0] + coef[iStep] * k[0][iStep - 1];
      x[1] = xIn[1] + coef[iStep] * k[1][iStep - 1];
      x[2] = xIn[2] + coef[iStep] * k[2][iStep - 1];
    }

    k[0][iStep] = x[2] * h;
    k[1][iStep] = x[3] * h;
    k[2][iStep] = (dAx_dtx[iStep] * x[2] + dAx_dty[iStep] * x[3]) * hCqp;
    // k[3][iStep] = (dAy_dtx[iStep] * x[2] + dAy_dty[iStep] * x[3]) * hCqp;
  } // 4

  Fmat[3] = x0[0] + k[0][0] / 6.f + k[0][1] / 3.f + k[0][2] / 3.f + k[0][3] / 6.f;

  Fmat[8] = x0[1] + k[1][0] / 6.f + k[1][1] / 3.f + k[1][2] / 3.f + k[1][3] / 6.f;

  Fmat[13] = x0[2] + k[2][0] / 6.f + k[2][1] / 3.f + k[2][2] / 3.f + k[2][3] / 6.f;
  Fmat[18] = 1.f;
  Fmat[23] = 0.f;
  // end of derivatives dx/dty

  // Derivatives dx/dqp
  x[0] = x0[0] = 0.f;
  x[1] = x0[1] = 0.f;
  x[2] = x0[2] = 0.f;
  x[3] = x0[3] = 0.f;
  for (unsigned int iStep = 0; iStep < 4; iStep++) { // 4
    if (iStep > 0) {
      x[0] = xIn[0] + coef[iStep] * k[0][iStep - 1];
      x[1] = xIn[1] + coef[iStep] * k[1][iStep - 1];
      x[2] = xIn[2] + coef[iStep] * k[2][iStep - 1];
      x[3] = xIn[3] + coef[iStep] * k[3][iStep - 1];
    }

    k[0][iStep] = x[2] * h;
    k[1][iStep] = x[3] * h;
    k[2][iStep] = Ax[iStep] * hC + hCqp * (dAx_dtx[iStep] * x[2] + dAx_dty[iStep] * x[3]);
    k[3][iStep] = Ay[iStep] * hC + hCqp * (dAy_dtx[iStep] * x[2] + dAy_dty[iStep] * x[3]);
  } // 4

  Fmat[4] = x0[0] + k[0][0] / 6.f + k[0][1] / 3.f + k[0][2] / 3.f + k[0][3] / 6.f;

  Fmat[9] = x0[1] + k[1][0] / 6.f + k[1][1] / 3.f + k[1][2] / 3.f + k[1][3] / 6.f;

  Fmat[14] = x0[2] + k[2][0] / 6.f + k[2][1] / 3.f + k[2][2] / 3.f + k[2][3] / 6.f;

  Fmat[19] = x[3] + k[3][0] / 6.f + k[3][1] / 3.f + k[3][2] / 3.f + k[3][3] / 6.f;
  Fmat[24] = 1.f;
  // end of derivatives dx/dqp

  // end calculation of the derivatives
  // F*C*Ft
  float_v A = fC20 + Fmat[2] * fC22 + Fmat[3] * fC32 + Fmat[4] * fC42;
  float_v B = fC30 + Fmat[2] * fC32 + Fmat[3] * fC33 + Fmat[4] * fC43;
  float_v C = fC40 + Fmat[2] * fC42 + Fmat[3] * fC43 + Fmat[4] * fC44;

  float_v D = fC21 + Fmat[7] * fC22 + Fmat[8] * fC32 + Fmat[9] * fC42;
  float_v E = fC31 + Fmat[7] * fC32 + Fmat[8] * fC33 + Fmat[9] * fC43;
  float_v G = fC41 + Fmat[7] * fC42 + Fmat[8] * fC43 + Fmat[9] * fC44;

  float_v H = fC22 + Fmat[13] * fC32 + Fmat[14] * fC42;
  float_v I = fC32 + Fmat[13] * fC33 + Fmat[14] * fC43;
  float_v J = fC42 + Fmat[13] * fC43 + Fmat[14] * fC44;

  float_v K = fC43 + Fmat[17] * fC42 + Fmat[19] * fC44;

  fC00(mask) = fC00 + Fmat[2] * fC20 + Fmat[3] * fC30 + Fmat[4] * fC40 + A * Fmat[2] + B * Fmat[3] + C * Fmat[4];
  fC10(mask) = fC10 + Fmat[2] * fC21 + Fmat[3] * fC31 + Fmat[4] * fC41 + A * Fmat[7] + B * Fmat[8] + C * Fmat[9];
  fC20(mask) = A + B * Fmat[13] + C * Fmat[14];
  fC30(mask) = B + A * Fmat[17] + C * Fmat[19];
  fC40(mask) = C;

  fC11(mask) = fC11 + Fmat[7] * fC21 + Fmat[8] * fC31 + Fmat[9] * fC41 + D * Fmat[7] + E * Fmat[8] + G * Fmat[9];
  fC21(mask) = D + E * Fmat[13] + G * Fmat[14];
  fC31(mask) = E + D * Fmat[17] + G * Fmat[19];
  fC41(mask) = G;

  fC22(mask) = H + I * Fmat[13] + J * Fmat[14];
  fC32(mask) = I + H * Fmat[17] + J * Fmat[19];
  fC42(mask) = J;

  fC33(mask) = fC33 + Fmat[17] * fC32 + Fmat[19] * fC43 + (Fmat[17] * fC22 + fC32 + Fmat[19] * fC42) * Fmat[17] + K * Fmat[19];
  fC43(mask) = K;

  fC44(mask) = fC44;

  return mask;
}

inline float_m PndFTSCATrackParamVector::TransportToX0(const float_v &x0_out, const L1FieldRegion &F, const float_v &qp0, const float_m &mask)
{

  // cout<<"Extrapolation..."<<endl;
  //
  //  Part of the analytic extrapolation formula with error (c_light*B*dz)^4/4!
  //
  /*cout<<"before extrp fX fY fTx fTy fQP "<<fX[0]<<" "<<fY[0]<<" "<<fTx[0]<<" "<<fTy[0]<<" "<<fQP[0] << "   z " << fZ[0] <<endl;
  cout<<"transport to x0 "<<x0_out[0]<<endl;*/
  const float_v c_light = 0.000299792458f;
  //   std::cout <<"Extrapolate 0 " <<std::endl;
  //   std::cout << "fX " << fX << "   fY " << fY << "   fZ " << fZ << std::endl;
  //   std::cout << "fTx " << fTx << "   fTy " << fTy << "   Qp " << fQP << std::endl;

  const float_v c1 = 1.f, c2 = 2.f, c3 = 3.f, c4 = 4.f, c6 = 6.f, c9 = 9.f, c15 = 15.f, c18 = 18.f, c45 = 45.f, c2i = 1.f / 2.f, c3i = 1.f / 3.f, c6i = 1.f / 6.f,
                c12i = 1.f / 12.f;

  float_v dz(Vc::Zero);
  dz(mask) = (x0_out - fZ);
  // const float_v dz = (x0_out - fZ);
  const float_v dz2 = dz * dz;
  const float_v dz3 = dz2 * dz;
  // construct coefficients

  const float_v x = fTx;
  const float_v y = fTy;
  const float_v xx = x * x;
  const float_v xy = x * y;
  const float_v yy = y * y;
  const float_v y2 = y * c2;
  const float_v x2 = x * c2;
  const float_v x4 = x * c4;
  const float_v xx31 = xx * c3 + c1;
  const float_v xx159 = xx * c15 + c9;

  const float_v Ay = -xx - c1;
  const float_v Ayy = x * (xx * c3 + c3);
  const float_v Ayz = -c2 * xy;
  const float_v Ayyy = -(c15 * xx * xx + c18 * xx + c3);

  const float_v Ayy_fX = c3 * xx31;
  const float_v Ayyy_fX = -x4 * xx159;

  const float_v bx = yy + c1;
  const float_v Byy = y * xx31;
  const float_v Byz = c2 * xx + c1;
  const float_v Byyy = -xy * xx159;

  const float_v Byy_fX = c6 * xy;
  const float_v Byyy_fX = -y * (c45 * xx + c9);
  const float_v Byyy_fY = -x * xx159;

  // end of coefficients calculation

  const float_v t2 = c1 + xx + yy;
  const float_v t = sqrt(t2);
  const float_v h = qp0 * c_light;
  const float_v ht = h * t;

  // get field integrals
  const float_v ddz = fZ - F.z0;
  float_v Fx0 = F.cx0 + F.cx1 * ddz + F.cx2 * ddz * ddz;
  float_v Fx1 = (F.cx1 + c2 * F.cx2 * ddz) * dz;
  float_v Fx2 = F.cx2 * dz2;
  float_v Fy0 = F.cy0 + F.cy1 * ddz + F.cy2 * ddz * ddz;
  float_v Fy1 = (F.cy1 + c2 * F.cy2 * ddz) * dz;
  float_v Fy2 = F.cy2 * dz2;
  float_v Fz0 = F.cz0 + F.cz1 * ddz + F.cz2 * ddz * ddz;
  float_v Fz1 = (F.cz1 + c2 * F.cz2 * ddz) * dz;
  float_v Fz2 = F.cz2 * dz2;

  const float_v sx = (Fx0 + Fx1 * c2i + Fx2 * c3i);
  const float_v sy = (Fy0 + Fy1 * c2i + Fy2 * c3i);
  const float_v sz = (Fz0 + Fz1 * c2i + Fz2 * c3i);

  const float_v Sx = (Fx0 * c2i + Fx1 * c6i + Fx2 * c12i);
  const float_v Sy = (Fy0 * c2i + Fy1 * c6i + Fy2 * c12i);
  const float_v Sz = (Fz0 * c2i + Fz1 * c6i + Fz2 * c12i);

  float_v syz;
  {
    const float_v d = 1.f / 360.f, c00 = 30.f * 6.f * d, c01 = 30.f * 2.f * d, c02 = 30.f * d, c10 = 3.f * 40.f * d, c11 = 3.f * 15.f * d, c12 = 3.f * 8.f * d,
                  c20 = 2.f * 45.f * d, c21 = 2.f * 2.f * 9.f * d, c22 = 2.f * 2.f * 5.f * d;
    syz = Fy0 * (c00 * Fz0 + c01 * Fz1 + c02 * Fz2) + Fy1 * (c10 * Fz0 + c11 * Fz1 + c12 * Fz2) + Fy2 * (c20 * Fz0 + c21 * Fz1 + c22 * Fz2);
  }

  float_v Syz;
  {
    const float_v d = 1.f / 2520.f, c00 = 21.f * 20.f * d, c01 = 21.f * 5.f * d, c02 = 21.f * 2.f * d, c10 = 7.f * 30.f * d, c11 = 7.f * 9.f * d, c12 = 7.f * 4.f * d,
                  c20 = 2.f * 63.f * d, c21 = 2.f * 21.f * d, c22 = 2.f * 10.f * d;
    Syz = Fy0 * (c00 * Fz0 + c01 * Fz1 + c02 * Fz2) + Fy1 * (c10 * Fz0 + c11 * Fz1 + c12 * Fz2) + Fy2 * (c20 * Fz0 + c21 * Fz1 + c22 * Fz2);
  }

  const float_v syy = sy * sy * c2i;
  const float_v syyy = syy * sy * c3i;

  float_v Syy;
  {
    const float_v d = 1.f / 2520.f, c00 = 420.f * d, c01 = 21.f * 15.f * d, c02 = 21.f * 8.f * d, c03 = 63.f * d, c04 = 70.f * d, c05 = 20.f * d;
    Syy = Fy0 * (c00 * Fy0 + c01 * Fy1 + c02 * Fy2) + Fy1 * (c03 * Fy1 + c04 * Fy2) + c05 * Fy2 * Fy2;
  }

  float_v Syyy;
  {
    const float_v d = 1.f / 181440.f, c000 = 7560 * d, c001 = 9 * 1008 * d, c002 = 5 * 1008 * d, c011 = 21 * 180 * d, c012 = 24 * 180 * d, c022 = 7 * 180 * d, c111 = 540 * d,
                  c112 = 945 * d, c122 = 560 * d, c222 = 112 * d;
    const float_v Fy22 = Fy2 * Fy2;
    Syyy = Fy0 * (Fy0 * (c000 * Fy0 + c001 * Fy1 + c002 * Fy2) + Fy1 * (c011 * Fy1 + c012 * Fy2) + c022 * Fy22) + Fy1 * (Fy1 * (c111 * Fy1 + c112 * Fy2) + c122 * Fy22) +
           c222 * Fy22 * Fy2;
  }

  const float_v sA1 = sx * xy + sy * Ay + sz * y;
  const float_v sA1_fX = sx * y - sy * x2;
  const float_v sA1_fY = sx * x + sz;

  const float_v sB1 = sx * bx - sy * xy - sz * x;
  const float_v sB1_fX = -sy * y - sz;
  const float_v sB1_fY = sx * y2 - sy * x;

  const float_v SA1 = Sx * xy + Sy * Ay + Sz * y;
  const float_v SA1_fX = Sx * y - Sy * x2;
  const float_v SA1_fY = Sx * x + Sz;

  const float_v SB1 = Sx * bx - Sy * xy - Sz * x;
  const float_v SB1_fX = -Sy * y - Sz;
  const float_v SB1_fY = Sx * y2 - Sy * x;

  const float_v sA2 = syy * Ayy + syz * Ayz;
  const float_v sA2_fX = syy * Ayy_fX - syz * y2;
  const float_v sA2_fY = -syz * x2;
  const float_v sB2 = syy * Byy + syz * Byz;
  const float_v sB2_fX = syy * Byy_fX + syz * x4;
  const float_v sB2_fY = syy * xx31;

  const float_v SA2 = Syy * Ayy + Syz * Ayz;
  const float_v SA2_fX = Syy * Ayy_fX - Syz * y2;
  const float_v SA2_fY = -Syz * x2;
  const float_v SB2 = Syy * Byy + Syz * Byz;
  const float_v SB2_fX = Syy * Byy_fX + Syz * x4;
  const float_v SB2_fY = Syy * xx31;

  const float_v sA3 = syyy * Ayyy;
  const float_v sA3_fX = syyy * Ayyy_fX;
  const float_v sB3 = syyy * Byyy;
  const float_v sB3_fX = syyy * Byyy_fX;
  const float_v sB3_fY = syyy * Byyy_fY;

  const float_v SA3 = Syyy * Ayyy;
  const float_v SA3_fX = Syyy * Ayyy_fX;
  const float_v SB3 = Syyy * Byyy;
  const float_v SB3_fX = Syyy * Byyy_fX;
  const float_v SB3_fY = Syyy * Byyy_fY;

  const float_v ht1 = ht * dz;
  const float_v ht2 = ht * ht * dz2;
  const float_v ht3 = ht * ht * ht * dz3;
  const float_v ht1sA1 = ht1 * sA1;
  const float_v ht1sB1 = ht1 * sB1;
  const float_v ht1SA1 = ht1 * SA1;
  const float_v ht1SB1 = ht1 * SB1;
  const float_v ht2sA2 = ht2 * sA2;
  const float_v ht2SA2 = ht2 * SA2;
  const float_v ht2sB2 = ht2 * sB2;
  const float_v ht2SB2 = ht2 * SB2;
  const float_v ht3sA3 = ht3 * sA3;
  const float_v ht3sB3 = ht3 * sB3;
  const float_v ht3SA3 = ht3 * SA3;
  const float_v ht3SB3 = ht3 * SB3;

  fX(mask) += ((x + ht1SA1 + ht2SA2 + ht3SA3) * dz);
  fY(mask) += ((y + ht1SB1 + ht2SB2 + ht3SB3) * dz);
  fTx(mask) += (ht1sA1 + ht2sA2 + ht3sA3);
  fTy(mask) += (ht1sB1 + ht2sB2 + ht3sB3);
  fZ(mask) += (dz);

  //   std::cout <<"Extrapolate 1 " <<std::endl;
  //   std::cout << "fX " << fX << "   fY " << fY << "   fZ " << fZ << std::endl;
  //   std::cout << "fTx " << fTx << "   fTy " << fTy << "   Qp " << fQP << std::endl;

  const float_v ctdz = c_light * t * dz;
  const float_v ctdz2 = c_light * t * dz2;

  const float_v dqp = fQP - qp0;
  const float_v t2i = c1 * rcp(t2); // /t2;
  const float_v xt2i = x * t2i;
  const float_v yt2i = y * t2i;
  const float_v tmp0 = ht1SA1 + c2 * ht2SA2 + c3 * ht3SA3;
  const float_v tmp1 = ht1SB1 + c2 * ht2SB2 + c3 * ht3SB3;
  const float_v tmp2 = ht1sA1 + c2 * ht2sA2 + c3 * ht3sA3;
  const float_v tmp3 = ht1sB1 + c2 * ht2sB2 + c3 * ht3sB3;

  const float_v j02 = dz * (c1 + xt2i * tmp0 + ht1 * SA1_fX + ht2 * SA2_fX + ht3 * SA3_fX);
  const float_v j12 = dz * (xt2i * tmp1 + ht1 * SB1_fX + ht2 * SB2_fX + ht3 * SB3_fX);
  const float_v j22 = c1 + xt2i * tmp2 + ht1 * sA1_fX + ht2 * sA2_fX + ht3 * sA3_fX;
  const float_v j32 = xt2i * tmp3 + ht1 * sB1_fX + ht2 * sB2_fX + ht3 * sB3_fX;

  const float_v j03 = dz * (yt2i * tmp0 + ht1 * SA1_fY + ht2 * SA2_fY);
  const float_v j13 = dz * (c1 + yt2i * tmp1 + ht1 * SB1_fY + ht2 * SB2_fY + ht3 * SB3_fY);
  const float_v j23 = yt2i * tmp2 + ht1 * sA1_fY + ht2 * sA2_fY;
  const float_v j33 = c1 + yt2i * tmp3 + ht1 * sB1_fY + ht2 * sB2_fY + ht3 * sB3_fY;

  const float_v j04 = ctdz2 * (SA1 + c2 * ht1 * SA2 + c3 * ht2 * SA3);
  const float_v j14 = ctdz2 * (SB1 + c2 * ht1 * SB2 + c3 * ht2 * SB3);
  const float_v j24 = ctdz * (sA1 + c2 * ht1 * sA2 + c3 * ht2 * sA3);
  const float_v j34 = ctdz * (sB1 + c2 * ht1 * sB2 + c3 * ht2 * sB3);

  // extrapolate inverse momentum
  fX(mask) += j04 * dqp;
  fY(mask) += j14 * dqp;
  fTx(mask) += j24 * dqp;
  fTy(mask) += j34 * dqp;

  //   std::cout <<"Extrapolate 2 " <<std::endl;
  //   std::cout << "fX " << fX << "   fY " << fY << "   fZ " << fZ << std::endl;
  //   std::cout << "fTx " << fTx << "   fTy " << fTy << "   Qp " << fQP << std::endl;
  //          covariance matrix transport

  const float_v c42 = fC42, c43 = fC43;

  const float_v cj00 = fC00 + fC20 * j02 + fC30 * j03 + fC40 * j04;
  //  const float_v cj10 = fC10 + fC21*j02 + fC31*j03 + fC41*j04;
  const float_v cj20 = fC20 + fC22 * j02 + fC32 * j03 + c42 * j04;
  const float_v cj30 = fC30 + fC32 * j02 + fC33 * j03 + c43 * j04;

  const float_v cj01 = fC10 + fC20 * j12 + fC30 * j13 + fC40 * j14;
  const float_v cj11 = fC11 + fC21 * j12 + fC31 * j13 + fC41 * j14;
  const float_v cj21 = fC21 + fC22 * j12 + fC32 * j13 + c42 * j14;
  const float_v cj31 = fC31 + fC32 * j12 + fC33 * j13 + c43 * j14;

  //  const float_v cj02 = fC20*j22 + fC30*j23 + fC40*j24;
  //  const float_v cj12 = fC21*j22 + fC31*j23 + fC41*j24;
  const float_v cj22 = fC22 * j22 + fC32 * j23 + c42 * j24;
  const float_v cj32 = fC32 * j22 + fC33 * j23 + c43 * j24;

  //  const float_v cj03 = fC20*j32 + fC30*j33 + fC40*j34;
  //  const float_v cj13 = fC21*j32 + fC31*j33 + fC41*j34;
  const float_v cj23 = fC22 * j32 + fC32 * j33 + c42 * j34;
  const float_v cj33 = fC32 * j32 + fC33 * j33 + c43 * j34;

  fC40(mask) += (c42 * j02 + c43 * j03 + fC44 * j04); // cj40
  fC41(mask) += (c42 * j12 + c43 * j13 + fC44 * j14); // cj41
  fC42(mask) = (c42 * j22 + c43 * j23 + fC44 * j24);  // cj42
  fC43(mask) = (c42 * j32 + c43 * j33 + fC44 * j34);  // cj43

  fC00(mask) = (cj00 + j02 * cj20 + j03 * cj30 + j04 * fC40);
  fC10(mask) = (cj01 + j02 * cj21 + j03 * cj31 + j04 * fC41);
  fC11(mask) = (cj11 + j12 * cj21 + j13 * cj31 + j14 * fC41);

  // cout<<"transport \n";
  // cout<<" C00 " << fC00 << " C10 " << fC10 << " C11 " << fC11 << endl;
  fC20(mask) = (j22 * cj20 + j23 * cj30 + j24 * fC40);
  fC30(mask) = (j32 * cj20 + j33 * cj30 + j34 * fC40);
  fC21(mask) = (j22 * cj21 + j23 * cj31 + j24 * fC41);
  fC31(mask) = (j32 * cj21 + j33 * cj31 + j34 * fC41);
  fC22(mask) = (j22 * cj22 + j23 * cj32 + j24 * fC42);
  fC32(mask) = (j32 * cj22 + j33 * cj32 + j34 * fC42);
  fC33(mask) = (j32 * cj23 + j33 * cj33 + j34 * fC43);

  return mask;
}

inline float_m PndFTSCATrackParamVector::PassMaterial(const L1MaterialInfo &info, const float_v &qp0, const float_m &mask)
{
  const float_v mass2 = 0.1395679f * 0.1395679f;

  float_v txtx = fTx * fTx;
  float_v tyty = fTy * fTy;
  float_v txtx1 = txtx + 1.f;
  float_v h = txtx + tyty;
  float_v t = sqrt(txtx1 + tyty);
  float_v h2 = h * h;
  float_v qp0t = qp0 * t;

  const float_v c1 = 0.0136f, c2 = c1 * 0.038f, c3 = c2 * 0.5f, c4 = -c3 / 2.0f, c5 = c3 / 3.0f, c6 = -c3 / 4.0f;

  float_v s0 = (c1 + c2 * info.logRadThick + c3 * h + h2 * (c4 + c5 * h + c6 * h2)) * qp0t;
  float_v a = ((t + mass2 * qp0 * qp0t) * info.RadThick * s0 * s0);
  // std::cout<<"info.RadThick info.logRadThick "<<info.RadThick<<" "<<info.logRadThick<<std::endl;
  // std::cout <<" a " << a << std::endl;
  //  a=0.000005;
  fC22(mask) += txtx1 * a;
  fC32(mask) += fTx * fTy * a;
  fC33(mask) += (1.f + tyty) * a;

  return mask;
}

inline float_v PndFTSCATrackParamVector::ApproximateBetheBloch(const float_v &bg2)
{
  //
  // This is the parameterization of the Bethe-Bloch formula inspired by Geant.
  //
  // bg2  - (beta*gamma)^2
  // kp0 - density [g/cm^3]
  // kp1 - density effect first junction point
  // kp2 - density effect second junction point
  // kp3 - mean excitation energy [GeV]
  // kp4 - mean Z/A
  //
  // The default values for the kp* parameters are for silicon.
  // The returned value is in [GeV/(g/cm^2)].
  //

  const float_v &kp0 = 2.33f;
  const float_v &kp1 = 0.20f;
  const float_v &kp2 = 3.00f;
  const float_v &kp3 = 173e-9f;
  const float_v &kp4 = 0.49848f;

  const float mK = 0.307075e-3f; // [GeV*cm^2/g]
  const float _2me = 1.022e-3f;  // [GeV/c^2]
  const float_v &rho = kp0;
  const float_v &x0 = kp1 * 2.303f;
  const float_v &x1 = kp2 * 2.303f;
  const float_v &mI = kp3;
  const float_v &mZA = kp4;
  const float_v &maxT = _2me * bg2; // neglecting the electron mass

  //*** Density effect
  float_v d2(Vc::Zero);
  const float_v x = 0.5f * log(bg2);
  const float_v lhwI = log(28.816f * 1e-9f * sqrt(rho * mZA) / mI);

  float_m init = x > x1;

  d2(init) = lhwI + x - 0.5f;
  const float_v &r = (x1 - x) / (x1 - x0);
  init = (x > x0) & (x1 > x);
  d2(init) = (lhwI + x - 0.5f + (0.5f - lhwI - x0) * r * r * r);

  return mK * mZA * (1.f + bg2) / bg2 * (0.5f * log(_2me * bg2 * maxT / (mI * mI)) - bg2 / (1.f + bg2) - d2);
}

inline void PndFTSCATrackParamVector::EnergyLossCorrection(const float_v &mass2, const float_v &radThick, float_v &qp0, float_v direction, const float_m &mask)
{
  const float_v &p2 = 1.f / (qp0 * qp0);
  const float_v &E2 = mass2 + p2;

  const float_v &bethe = ApproximateBetheBloch(p2 / mass2);

  float_v tr = sqrt(1.f + fTx * fTx + fTy * fTy);

  const float_v &dE = bethe * radThick * tr * 2.33f * 9.34961f;

  const float_v &E2Corrected = (sqrt(E2) + direction * dE) * (sqrt(E2) + direction * dE);
  float_v corr = sqrt(p2 / (E2Corrected - mass2));
  // cout<<"corr "<<corr<<endl;
  float_m init = !(corr == corr) || !(mask);
  corr(init) = 1.f;
  qp0(mask) *= corr;
  fQP(mask) *= corr;
  fC40(mask) *= corr;
  fC41(mask) *= corr;
  fC42(mask) *= corr;
  fC43(mask) *= corr;
  fC44(mask) *= corr * corr;
}

inline float_m
PndFTSCATrackParamVector::FilterVtx(const float_v &xV, const float_v &yV, const L1XYMeasurementInfo &info, float_v &extrDx, float_v &extrDy, float_v J[], const float_m &active)
{
  float_v zeta0, zeta1, S00, S10, S11, si;
  float_v F00, F10, F20, F30, F40, F01, F11, F21, F31, F41;
  float_v K00, K10, K20, K30, K40, K01, K11, K21, K31, K41;

  zeta0 = fX + extrDx - xV;
  zeta1 = fY + extrDy - yV;

  // H = 1 0 J[0] J[1] J[2]
  //     0 1 J[3] J[4] J[5]

  // F = CH'
  F00 = fC00;
  F01 = fC10;
  F10 = fC10;
  F11 = fC11;
  F20 = J[0] * fC22;
  F21 = J[3] * fC22;
  F30 = J[1] * fC33;
  F31 = J[4] * fC33;
  F40 = J[2] * fC44;
  F41 = J[5] * fC44;

  S00 = info.C00 + F00 + J[0] * F20 + J[1] * F30 + J[2] * F40;
  S10 = info.C10 + F10 + J[3] * F20 + J[4] * F30 + J[5] * F40;
  S11 = info.C11 + F11 + J[3] * F21 + J[4] * F31 + J[5] * F41;

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

  fX(active) -= K00 * zeta0 + K01 * zeta1;
  fY(active) -= K10 * zeta0 + K11 * zeta1;
  fTx(active) -= K20 * zeta0 + K21 * zeta1;
  fTy(active) -= K30 * zeta0 + K31 * zeta1;
  fQP(active) -= K40 * zeta0 + K41 * zeta1;

  fC00(active) -= (K00 * F00 + K01 * F01);
  fC10(active) -= (K10 * F00 + K11 * F01);
  fC11(active) -= (K10 * F10 + K11 * F11);
  fC20(active) = -(K20 * F00 + K21 * F01);
  fC21(active) = -(K20 * F10 + K21 * F11);
  fC22(active) -= (K20 * F20 + K21 * F21);
  fC30(active) = -(K30 * F00 + K31 * F01);
  fC31(active) = -(K30 * F10 + K31 * F11);
  fC32(active) = -(K30 * F20 + K31 * F21);
  fC33(active) -= (K30 * F30 + K31 * F31);
  fC40(active) = -(K40 * F00 + K41 * F01);
  fC41(active) = -(K40 * F10 + K41 * F11);
  fC42(active) = -(K40 * F20 + K41 * F21);
  fC43(active) = -(K40 * F30 + K41 * F31);
  fC44(active) -= (K40 * F40 + K41 * F41);

  return active;
}

inline float_m
PndFTSCATrackParamVector::TransportJXY0ToX0(const float_v &x0, const L1FieldRegion &F, float_v &extrDx, float_v &extrDy, float_v &J04, float_v &J14, const float_m &active)
{
  const float_v c_light = 0.000299792458f, c1 = 1.f, c2i = 0.5f, c6i = 1.f / 6.f, c12i = 1.f / 12.f;

  const float_v dz = x0 - fZ;
  float_v dz2 = dz * dz;
  float_v dzc6i = dz * c6i;
  float_v dz2c12i = dz2 * c12i;

  float_v xx = fTx * fTx;
  float_v yy = fTy * fTy;
  float_v xy = fTx * fTy;

  float_v Ay = -xx - c1;
  float_v bx = yy + c1;

  float_v ctdz2 = c_light * sqrt(c1 + xx + yy) * dz2;

  float_v Sx = F.cx0 * c2i + F.cx1 * dzc6i + F.cx2 * dz2c12i;
  float_v Sy = F.cy0 * c2i + F.cy1 * dzc6i + F.cy2 * dz2c12i;
  float_v Sz = F.cz0 * c2i + F.cz1 * dzc6i + F.cz2 * dz2c12i;

  extrDx(active) = (fTx)*dz;
  extrDy(active) = (fTy)*dz;
  J04(active) = ctdz2 * (Sx * xy + Sy * Ay + Sz * fTy);
  J14(active) = ctdz2 * (Sx * bx - Sy * xy - Sz * fTx);

  return active;
}

#else // PANDA_FTS

#include "PndFTSCADef.h"
#include "PndFTSVector.h"
#include "PndFTSCAMath.h"
#include "CAX1X2MeasurementInfo.h"
#include "FTSCAStation.h"

class PndFTSCATrackLinearisationVector;
class PndFTSCAParam;
class FTSCAHit;
class FTSCAHitV;
class FTSCATarget;

namespace std {
template <typename T>
struct char_traits;
template <typename _CharT, typename _Traits>
class basic_istream;
typedef basic_istream<char, char_traits<char>> istream;
template <typename _CharT, typename _Traits>
class basic_ostream;
typedef basic_ostream<char, char_traits<char>> ostream;
} // namespace std

/**
 * @class PndFTSCATrackParamVector
 *
 * PndFTSCATrackParamVector class describes the track parametrisation
 * which is used by the PndFTSCATracker slice tracker.
 *
 */
class PndFTSCATrackParamVector {
  friend std::istream &operator>>(std::istream &, PndFTSCATrackParamVector &);
  friend std::ostream &operator<<(std::ostream &, const PndFTSCATrackParamVector &);

 public:
  PndFTSCATrackParamVector() : fX(Vc::Zero), fSignCosPhi(Vc::Zero), fChi2(Vc::Zero), fNDF(Vc::Zero)
  {
    for (int i = 0; i < 5; ++i)
      fP[i].setZero();
    for (int i = 0; i < 15; ++i)
      fC[i].setZero();
  }

  void ConvertTrackParamToVector(PndFTSCATrackParam t0[float_v::Size], int nTracksV);

  void InitCovMatrix(float_v d2QMom = 0.f);
  void InitByTarget(const FTSCATarget &target);
  void InitByHit(const FTSCAHitV &hit, const PndFTSCAParam &param, const float_v &dQMom);

  void InitDirection(float_v r0, float_v r1, float_v r2) // initialize direction parameters according to a given tangent vector
  {
    const float_v r = sqrt(r0 * r0 + r1 * r1);
    SetSinPhi(r1 / r);
    SetSignCosPhi(r0 / abs(r0));
    SetDzDs(r2 / r);
  }

  struct PndFTSCATrackFitParam {
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
  friend class PndFTSCATrackParam;
  const float_v *Par() const { return fP; }
  const float_v *Cov() const { return fC; }
  float_v *Par() { return fP; }
  float_v *Cov() { return fC; }

 public:
  void SetTrackParam(const PndFTSCATrackParamVector &param, const float_m &m = float_m(true))
  {
    for (int i = 0; i < 5; i++)
      fP[i](m) = param.Par()[i];
    for (int i = 0; i < 15; i++)
      fC[i](m) = param.Cov()[i];
    fX(m) = param.X();
    fSignCosPhi(m) = param.SignCosPhi();
    fChi2(m) = param.GetChi2();
    fNDF(static_cast<int_m>(m)) = param.GetNDF();
    fAlpha(static_cast<int_m>(m)) = param.Angle();
  }

  void SetTrackParamOne(int iV, const PndFTSCATrackParamVector &param, int iVa)
  {
    for (int i = 0; i < 5; i++)
      fP[i][iV] = param.Par()[i][iVa];
    for (int i = 0; i < 15; i++)
      fC[i][iV] = param.Cov()[i][iVa];
    fX[iV] = param.X()[iVa];
    fSignCosPhi[iV] = param.SignCosPhi()[iVa];
    fChi2[iV] = param.GetChi2()[iVa];
    fNDF[iV] = param.GetNDF()[iVa];
    fAlpha[iV] = param.Angle()[iVa];
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

  void SetErr2Y(float_v v) { fC[0] = v; }
  void SetErr2Z(float_v v) { fC[2] = v; }
  void SetErr2QPt(float_v v) { fC[14] = v; }

  float_v GetDist2(const PndFTSCATrackParamVector &t) const;
  float_v GetDistXZ2(const PndFTSCATrackParamVector &t) const;

  float_v GetS(const float_v &x, const float_v &y, const float_v &Bz) const;

  void GetDCAPoint(const float_v &x, const float_v &y, const float_v &z, float_v *px, float_v *py, float_v *pz, const float_v &Bz) const;

  float_m TransportToX0WithMaterial(const float_v &x, const float_v &XOverX0, const float_v &XThimesRho, const float_v &Bz, const float maxSinPhi = .999f);

  float_m TransportToX0(const float_v &x, const float_v &Bz, const float maxSinPhi = .999f, const float_m &mask = float_m(true));

  float_m
  TransportToX0(const float_v &x, PndFTSCATrackLinearisationVector &t0, const float_v &Bz, const float maxSinPhi = .999f, float_v *DL = 0, const float_m &mask = float_m(true));

  float_m TransportToX0(const float_v &x, const float_v &sinPhi0, const float_v &Bz, const float_v maxSinPhi = .999f, const float_m &mask = float_m(true));

  float_m TransportToX0WithMaterial(const float_v &x, PndFTSCATrackLinearisationVector &t0, PndFTSCATrackFitParam &par, const float_v &XOverX0, const float_v &XThimesRho,
                                    const float_v &Bz, const float maxSinPhi = .999f, const float_m &mask = float_m(true));

  float_m
  TransportToX0WithMaterial(const float_v &x, PndFTSCATrackFitParam &par, const float_v &XOverX0, const float_v &XThimesRho, const float_v &Bz, const float maxSinPhi = .999f);

  float_m Rotate(const float_v &alpha, PndFTSCATrackLinearisationVector &t0, const float maxSinPhi = .999f, const float_m &mask = float_m(true));
  float_m Rotate(const float_v &alpha, const float maxSinPhi = .999f, const float_m &mask = float_m(true));
  void RotateXY(float_v alpha, float_v &x, float_v &y, float_v &sin, const float_m &mask = float_m(true)) const;

  float_m FilterWithMaterial(const float_v &y, const float_v &z, float_v err2Y, float_v errYZ, float_v err2Z, float maxSinPhi = 0.999f, const float_m &mask = float_m(true),
                             const int_v &hitNDF = int_v(2), const float_v &chi2Cut = 10e10f); // filters 2-D measurement

  float_m FilterWithMaterial(const float_v &y, const float_v &z, const FTSCAStripInfo &info, float_v err2, float maxSinPhi = 0.999f, const float_m &mask = float_m(true),
                             const float_v &chi2Cut = 10e10f); // filters 1-D measurement
  float_m FilterWithMaterial(const float_v &y, const float_v &z, const float_v &r, const FTSCAStripInfo &info, float_v err2, float maxSinPhi = 0.999f,
                             const float_m &mask = float_m(true), const float_v &chi2Cut = 10e10f); // filters tube measurement

  static float_v ApproximateBetheBloch(const float_v &beta2);
  static float_v BetheBlochGeant(const float_v &bg, const float_v &kp0 = 2.33f, const float_v &kp1 = 0.20f, const float_v &kp2 = 3.00f, const float_v &kp3 = 173e-9f,
                                 const float_v &kp4 = 0.49848f);
  static float_v BetheBlochSolid(const float_v &bg);
  static float_v BetheBlochGas(const float_v &bg);

  void CalculateFitParameters(PndFTSCATrackFitParam &par, const float_v &mass = 0.13957f);
  float_m CorrectForMeanMaterial(const float_v &xOverX0, const float_v &xTimesRho, const PndFTSCATrackFitParam &par, const float_m &_mask);

  // float_m FilterDelta( const float_m &mask, const float_v &dy, const float_v &dz,
  //     float_v err2Y, float_v err2Z, const float maxSinPhi = .999f );
  // float_m Filter( const float_m &mask, const float_v &y, const float_v &z,
  //     float_v err2Y, float_v err2Z, const float maxSinPhi = .999f );

  float_m FilterVtx(const float_v &xV, const float_v &yV, const CAX1X2MeasurementInfo &info, float_v &extrDx, float_v &extrDy, float_v J[], const float_m &active = float_m(true));
  float_m TransportJ0ToX0(const float_v &x0, const float_v &cBz, float_v &extrDy, float_v &extrDz, float_v J[], const float_m &active = float_m(true));

  float_m Transport(const int_v &ista, const PndFTSCAParam &param, const float_m &mask = float_m(true));
  float_m Transport(const FTSCAHitV &hit, const PndFTSCAParam &p, const float_m &mask = float_m(true));
  float_m Filter(const FTSCAHitV &hit, const PndFTSCAParam &param, const float_m &mask = float_m(true), const float_v &chi2Cut = 10e10f);

  float_m Transport(const FTSCAHit &hit, const PndFTSCAParam &p, const float_m &mask = float_m(true));
  float_m Filter(const FTSCAHit &hit, const PndFTSCAParam &param, const float_m &mask = float_m(true), const float_v &chi2Cut = 10e10f);

  float_m AddTarget(const FTSCATarget &target, const float_m &mask = float_m(true));

 private:
  float_v fX;          // x position
  float_v fSignCosPhi; // sign of cosPhi
  float_v fP[5];  // 'active' track parameters: Y, Z, SinPhi = dy/sqrt(dx*dx + dy*dy), DzDs = dz/sqrt(dx*dx + dy*dy), q/Pt // if q/pt = 0 then track equation is [dr x e] = 0, where
                  // e = { sqrt( ( 1 - SinPhi^2 )/( 1 + DzDs^2 ) ), sqrt( SinPhi^2/( 1 + DzDs^2 ) ), sqrt( 1 /( 1 + DzDs^2 ) ) }
  float_v fC[15]; // the covariance matrix for Y,Z,SinPhi,..
  float_v fChi2;  // the chi^2 value
  int_v fNDF;     // the Number of Degrees of Freedom

  float_v fAlpha; // coor system
};

#include "debug.h"

inline float_m PndFTSCATrackParamVector::TransportToX0(const float_v &x, const float_v &sinPhi0, const float_v &Bz, const float_v maxSinPhi, const float_m &_mask)
{
  //* Transport the track parameters to X=x, using linearization at phi0 with 0 curvature,
  //* and the field value Bz
  //* maxSinPhi is the max. allowed value for |t0.SinPhi()|
  //* linearisation of trajectory t0 is also transported to X=x,
  //* returns 1 if OK
  //*

  debugKF() << "Start TransportToX0(" << x << ", " << _mask << ")\n" << *this << std::endl;

  const float_v &ey = sinPhi0;
  const float_v &dx = x - X();
  const float_v &exi = float_v(Vc::One) * CAMath::RSqrt(float_v(Vc::One) - ey * ey); // RSqrt

  const float_v &dxBz = dx * Bz;
  const float_v &dS = dx * exi;
  const float_v &h2 = dS * exi * exi;
  const float_v &h4 = .5f * h2 * dxBz;
//#define LOSE_DEBUG
#ifdef LOSE_DEBUG
  std::cout << " TrTo-sinPhi0 = " << sinPhi0 << std::endl;
#endif
  /// mvz start 23.01.2010
  //  const float_v &sinPhi = SinPhi() * (float_v( Vc::One ) - 0.5f * dxBz * QPt() *dxBz * QPt()/ ( float_v( Vc::One ) - SinPhi()*SinPhi() )) + dxBz * QPt();
  const float_v &sinPhi = SinPhi() + dxBz * QPt();
/// mvz end 23.01.2010
#ifdef LOSE_DEBUG
  std::cout << " TrTo-sinPhi = " << sinPhi << std::endl;
#endif
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

  debugKF() << mask << "\n" << *this << std::endl;
  return mask;
}

#include <assert.h>

#ifdef NVALGRIND
#define VALGRIND_CHECK_VALUE_IS_DEFINED(mask)
#define VALGRIND_CHECK_MASKED_VECTOR_IS_DEFINED(v, k)
#define VALGRIND_CHECK_MEM_IS_DEFINED(v, k) ;
#define VALGRIND_CHECK_MEM_IS_ADDRESSABLE(v, k) ;
#else
#include <valgrind/memcheck.h>
#define VALGRIND_CHECK_MASKED_VECTOR_IS_DEFINED(v, k) \
  {                                                   \
    __typeof__(v + v) tmp(v);                         \
    tmp.setZero(!k);                                  \
    VALGRIND_CHECK_VALUE_IS_DEFINED(tmp);             \
  }
#endif

// inline float_m PndFTSCATrackParamVector::FilterDelta( const float_m &mask, const float_v &dy, const float_v &dz,
//     float_v err2Y, float_v err2Z, const float maxSinPhi )
// {
//   debugKF() << "Kalman filter( " << mask
//     << "\n  " << dy
//     << "\n  " << dz
//     << "\n  " << err2Y
//     << "\n  " << err2Z
//     << "\n):" << std::endl;

//   assert( err2Y > 0.f || !mask );
//   assert( err2Z > 0.f || !mask );
//   VALGRIND_CHECK_VALUE_IS_DEFINED( mask );
//   VALGRIND_CHECK_MASKED_VECTOR_IS_DEFINED( dy, mask );
//   VALGRIND_CHECK_MASKED_VECTOR_IS_DEFINED( dz, mask );
//   VALGRIND_CHECK_MASKED_VECTOR_IS_DEFINED( err2Y, mask );
//   VALGRIND_CHECK_MASKED_VECTOR_IS_DEFINED( err2Z, mask );
// #ifndef NVALGRIND
//   err2Y.setZero( !mask );
//   err2Z.setZero( !mask );
// #endif
//   VALGRIND_CHECK_VALUE_IS_DEFINED( maxSinPhi );

//   //* Add the y,z measurement with the Kalman filter

//   const float_v c00 = fC[ 0];
//   const float_v c11 = fC[ 2];
//   const float_v c20 = fC[ 3];
//   const float_v c31 = fC[ 7];
//   const float_v c40 = fC[10];

//   VALGRIND_CHECK_VALUE_IS_DEFINED( c00 );
//   VALGRIND_CHECK_VALUE_IS_DEFINED( c11 );
//   VALGRIND_CHECK_VALUE_IS_DEFINED( c20 );
//   VALGRIND_CHECK_VALUE_IS_DEFINED( c40 );
//   VALGRIND_CHECK_VALUE_IS_DEFINED( c31 );

//   err2Y += c00;
//   err2Z += c11;
// #ifndef NODEBUG
//   if ( !( err2Y > 0.f || !mask ).isFull() ) {
//     std::cerr << err2Y << mask << ( err2Y > 0.f || !mask ) << c00 << std::endl;
//   }
// #endif
//   assert( err2Y > 0.f || !mask );
//   assert( err2Z > 0.f || !mask );

//   VALGRIND_CHECK_VALUE_IS_DEFINED( fP[0] );
//   VALGRIND_CHECK_VALUE_IS_DEFINED( fP[1] );
//   VALGRIND_CHECK_VALUE_IS_DEFINED( fP[2] );

//   const float_v &z0 = dy;
//   const float_v &z1 = dz;

//   const float_v &mS0 = float_v( Vc::One ) / err2Y;
//   const float_v &mS2 = float_v( Vc::One ) / err2Z;
//   //const float_v &mS0 = CAMath::Reciprocal( err2Y );
//   //const float_v &mS2 = CAMath::Reciprocal( err2Z );
//   debugKF() << "delta(mS0): " << CAMath::Abs( float_v( Vc::One ) / err2Y - mS0 ) << std::endl;
//   debugKF() << "delta(mS2): " << CAMath::Abs( float_v( Vc::One ) / err2Z - mS2 ) << std::endl;
//   assert( mS0 > 0.f || !mask );
//   assert( mS2 > 0.f || !mask );

//   // K = CHtS

//   const float_v &k00 = c00 * mS0;
//   const float_v &k20 = c20 * mS0;
//   const float_v &k40 = c40 * mS0;

//   const float_v &k11 = c11 * mS2;
//   const float_v &k31 = c31 * mS2;

//   debugKF() << "delta(k00): " << ( c00 / err2Y - k00 ) << std::endl;
//   debugKF() << "delta(k20): " << ( c20 / err2Y - k20 ) << std::endl;
//   debugKF() << "delta(k40): " << ( c40 / err2Y - k40 ) << std::endl;

//   debugKF() << "delta(k11): " << ( c11 / err2Z - k11 ) << std::endl;
//   debugKF() << "delta(k31): " << ( c31 / err2Z - k31 ) << std::endl;

//   const float_v &sinPhi = fP[2] + k20 * z0  ;
//   debugKF() << "delta(sinPhi): " << ( z0 * c20 / err2Y + fP[2] - sinPhi ) << std::endl;

//   assert( maxSinPhi > 0.f );
//   VALGRIND_CHECK_MASKED_VECTOR_IS_DEFINED( sinPhi, mask );
//   const float_m &success = mask && err2Y >= 1.e-8f && err2Z >= 1.e-8f && CAMath::Abs( sinPhi ) < maxSinPhi;
//   VALGRIND_CHECK_VALUE_IS_DEFINED( success );

//   fNDF  ( static_cast<int_m>( success ) ) += 2;
//   fChi2 ( success ) += mS0 * z0 * z0 + mS2 * z1 * z1 ;

//   fP[ 0]( success ) += k00 * z0 ;
//   fP[ 1]( success ) += k11 * z1 ;
//   fP[ 2]( success ) = sinPhi ;
//   fP[ 3]( success ) += k31 * z1 ;
//   fP[ 4]( success ) += k40 * z0 ;

//   fC[ 0]( success ) -= k00 * c00 ;
//   fC[ 3]( success ) -= k20 * c00 ;
//   fC[ 5]( success ) -= k20 * c20 ;
//   fC[10]( success ) -= k40 * c00 ;
//   fC[12]( success ) -= k40 * c20 ;
//   fC[14]( success ) -= k40 * c40 ;

//   fC[ 2]( success ) -= k11 * c11 ;
//   fC[ 7]( success ) -= k31 * c11 ;
//   fC[ 9]( success ) -= k31 * c31 ;
// #if 1
//   const float_m check = ( fC[ 0] >= 0.f ) && ( fC[ 2] >= 0.f ) && ( fC[ 5] >= 0.f ) && ( fC[ 9] >= 0.f ) && ( fC[14] >= 0.f );
// #else
//   assert( fC[ 0] >= 0.f );
//   assert( fC[ 2] >= 0.f );
//   assert( fC[ 5] >= 0.f );
//   assert( fC[ 9] >= 0.f );
//   assert( fC[14] >= 0.f );
// #endif
//   return success && check;
// }

// inline float_m PndFTSCATrackParamVector::Filter( const float_m &mask, const float_v &y, const float_v &z,
//     float_v err2Y, float_v err2Z, const float maxSinPhi )
// {
//   debugKF() << "Kalman filter( " << mask
//     << "\n  " << y
//     << "\n  " << z
//     << "\n  " << err2Y
//     << "\n  " << err2Z
//     << "\n):" << std::endl;

//   assert( err2Y > 0.f || !mask );
//   assert( err2Z > 0.f || !mask );
//   VALGRIND_CHECK_VALUE_IS_DEFINED( mask );
//   VALGRIND_CHECK_MASKED_VECTOR_IS_DEFINED( y, mask );
//   VALGRIND_CHECK_MASKED_VECTOR_IS_DEFINED( z, mask );
//   VALGRIND_CHECK_MASKED_VECTOR_IS_DEFINED( err2Y, mask );
//   VALGRIND_CHECK_MASKED_VECTOR_IS_DEFINED( err2Z, mask );
// #ifndef NVALGRIND
//   err2Y.setZero( !mask );
//   err2Z.setZero( !mask );
// #endif
//   VALGRIND_CHECK_VALUE_IS_DEFINED( maxSinPhi );

//   //* Add the y,z measurement with the Kalman filter

//   const float_v c00 = fC[ 0];
//   const float_v c11 = fC[ 2];
//   const float_v c20 = fC[ 3];
//   const float_v c31 = fC[ 7];
//   const float_v c40 = fC[10];

//   VALGRIND_CHECK_VALUE_IS_DEFINED( c00 );
//   VALGRIND_CHECK_VALUE_IS_DEFINED( c11 );
//   VALGRIND_CHECK_VALUE_IS_DEFINED( c20 );
//   VALGRIND_CHECK_VALUE_IS_DEFINED( c40 );
//   VALGRIND_CHECK_VALUE_IS_DEFINED( c31 );

//   err2Y += c00;
//   err2Z += c11;
// #ifndef NODEBUG
//   if ( !( err2Y > 0.f || !mask ).isFull() ) {
//     std::cerr << err2Y << mask << ( err2Y > 0.f || !mask ) << c00 << std::endl;
//   }
// #endif
//   assert( err2Y > 0.f || !mask );
//   assert( err2Z > 0.f || !mask );

//   VALGRIND_CHECK_VALUE_IS_DEFINED( fP[0] );
//   VALGRIND_CHECK_VALUE_IS_DEFINED( fP[1] );
//   VALGRIND_CHECK_VALUE_IS_DEFINED( fP[2] );

//   const float_v &z0 = y - fP[0];
//   const float_v &z1 = z - fP[1];

//   const float_v &mS0 = float_v( Vc::One ) / err2Y;
//   const float_v &mS2 = float_v( Vc::One ) / err2Z;
//   //const float_v &mS0 = CAMath::Reciprocal( err2Y );
//   //const float_v &mS2 = CAMath::Reciprocal( err2Z );
//   debugKF() << "delta(mS0): " << CAMath::Abs( float_v( Vc::One ) / err2Y - mS0 ) << std::endl;
//   debugKF() << "delta(mS2): " << CAMath::Abs( float_v( Vc::One ) / err2Z - mS2 ) << std::endl;
//   assert( mS0 > 0.f || !mask );
//   assert( mS2 > 0.f || !mask );

//   // K = CHtS

//   const float_v &k00 = c00 * mS0;
//   const float_v &k20 = c20 * mS0;
//   const float_v &k40 = c40 * mS0;

//   const float_v &k11 = c11 * mS2;
//   const float_v &k31 = c31 * mS2;

//   debugKF() << "delta(k00): " << ( c00 / err2Y - k00 ) << std::endl;
//   debugKF() << "delta(k20): " << ( c20 / err2Y - k20 ) << std::endl;
//   debugKF() << "delta(k40): " << ( c40 / err2Y - k40 ) << std::endl;

//   debugKF() << "delta(k11): " << ( c11 / err2Z - k11 ) << std::endl;
//   debugKF() << "delta(k31): " << ( c31 / err2Z - k31 ) << std::endl;

//   const float_v &sinPhi = fP[2] + k20 * z0  ;
//   debugKF() << "delta(sinPhi): " << ( z0 * c20 / err2Y + fP[2] - sinPhi ) << std::endl;

//   assert( maxSinPhi > 0.f );
//   VALGRIND_CHECK_MASKED_VECTOR_IS_DEFINED( sinPhi, mask );
//   const float_m &success = mask && err2Y >= 1.e-8f && err2Z >= 1.e-8f && CAMath::Abs( sinPhi ) < maxSinPhi;
//   VALGRIND_CHECK_VALUE_IS_DEFINED( success );

//   fNDF  ( static_cast<int_m>( success ) ) += 2;
//   fChi2 ( success ) += mS0 * z0 * z0 + mS2 * z1 * z1 ;

//   fP[ 0]( success ) += k00 * z0 ;
//   fP[ 1]( success ) += k11 * z1 ;
//   fP[ 2]( success ) = sinPhi ;
//   fP[ 3]( success ) += k31 * z1 ;
//   fP[ 4]( success ) += k40 * z0 ;

//   fC[ 0]( success ) -= k00 * c00 ;
//   fC[ 3]( success ) -= k20 * c00 ;
//   fC[ 5]( success ) -= k20 * c20 ;
//   fC[10]( success ) -= k40 * c00 ;
//   fC[12]( success ) -= k40 * c20 ;
//   fC[14]( success ) -= k40 * c40 ;

//   fC[ 2]( success ) -= k11 * c11 ;
//   fC[ 7]( success ) -= k31 * c11 ;
//   fC[ 9]( success ) -= k31 * c31 ;
// #if 1
//   const float_m check = ( fC[ 0] >= 0.f ) && ( fC[ 2] >= 0.f ) && ( fC[ 5] >= 0.f ) && ( fC[ 9] >= 0.f ) && ( fC[14] >= 0.f );
// #else
//   assert( fC[ 0] >= 0.f );
//   assert( fC[ 2] >= 0.f );
//   assert( fC[ 5] >= 0.f );
//   assert( fC[ 9] >= 0.f );
//   assert( fC[14] >= 0.f );
// #endif
//   return success && check;
// }

inline float_m PndFTSCATrackParamVector::Rotate(const float_v &alpha, const float maxSinPhi, const float_m &mask)
{
  //* Rotate the coordinate system in XY on the angle alpha
  if ((abs(alpha) < 1e-6f || !mask).isFull())
    return mask;

  const float_v cA = CAMath::Cos(alpha);
  const float_v sA = CAMath::Sin(alpha);
  const float_v x = X(), y = Y(), sP = SinPhi(), cP = GetCosPhi();
  const float_v cosPhi = cP * cA + sP * sA;
  const float_v sinPhi = -cP * sA + sP * cA;

  float_m mReturn = mask && (CAMath::Abs(sinPhi) < maxSinPhi) && (CAMath::Abs(cosPhi) > 1.e-2f) && (CAMath::Abs(cP) > 1.e-2f);

  mReturn &= abs(alpha) < 3.1415f * 0.25f; // allow turn by 45 degree only

  const float_v j0 = cP / cosPhi;
  const float_v j2 = cosPhi / cP;

  SetX(x * cA + y * sA, mReturn);
  SetY(-x * sA + y * cA, mReturn);
  SetSignCosPhi(CAMath::Abs(cosPhi) / cosPhi, mReturn);
  SetSinPhi(sinPhi, mReturn);

  // float J[5][5] = { { j0, 0, 0,  0,  0 }, // Y
  //                      {  0, 1, 0,  0,  0 }, // Z
  //                      {  0, 0, j2, 0,  0 }, // SinPhi
  //                    {  0, 0, 0,  1,  0 }, // DzDs
  //                    {  0, 0, 0,  0,  1 } }; // Kappa
  // cout<<"alpha="<<alpha<<" "<<x<<" "<<y<<" "<<sP<<" "<<cP<<" "<<j0<<" "<<j2<<endl;
  // cout<<"      "<<fC[0]<<" "<<fC[1]<<" "<<fC[6]<<" "<<fC[10]<<" "<<fC[4]<<" "<<fC[5]<<" "<<fC[8]<<" "<<fC[12]<<endl;
  fC[0](mReturn) *= j0 * j0;
  fC[1](mReturn) *= j0;
  fC[3](mReturn) *= j0;
  fC[6](mReturn) *= j0;
  fC[10](mReturn) *= j0;

  fC[3](mReturn) *= j2;
  fC[4](mReturn) *= j2;
  fC[5](mReturn) *= j2 * j2;
  fC[8](mReturn) *= j2;
  fC[12](mReturn) *= j2;

  fAlpha(mReturn) += alpha;
  // cout<<"      "<<fC[0]<<" "<<fC[1]<<" "<<fC[6]<<" "<<fC[10]<<" "<<fC[4]<<" "<<fC[5]<<" "<<fC[8]<<" "<<fC[12]<<endl;
  return mReturn;
}

inline void PndFTSCATrackParamVector::RotateXY(float_v alpha, float_v &x, float_v &y, float_v &sin, const float_m &mask) const
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
PndFTSCATrackParamVector::FilterVtx(const float_v &yV, const float_v &zV, const CAX1X2MeasurementInfo &info, float_v &extrDy, float_v &extrDz, float_v J[], const float_m &active)
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

inline float_m PndFTSCATrackParamVector::TransportJ0ToX0(const float_v &x, const float_v &cBz, float_v &extrDy, float_v &extrDz, float_v J[], const float_m &active)
{
  const float_v &ey = SinPhi();
  const float_v &dx = x - X();
  const float_v &exi = CAMath::RSqrt(float_v(Vc::One) - ey * ey); // RSqrt

  const float_v &dxBz = dx * cBz;
  const float_v &dS = dx * exi;
  const float_v &h2 = dS * exi * exi;
  const float_v &h4 = .5f * h2 * dxBz;

  float_m mask = active && CAMath::Abs(exi) <= 1.e4f;

  extrDy(active) = dS * ey;
  extrDz(active) = dS * DzDs();
  J[0](active) = dS;
  J[1](active) = 0;
  J[2](active) = h4;
  J[3](active) = dS;
  J[4](active) = dS;
  J[5](active) = 0;
  return active;
}

std::istream &operator>>(std::istream &in, PndFTSCATrackParamVector &ot);
std::ostream &operator<<(std::ostream &out, const PndFTSCATrackParamVector &ot);

#endif // !PANDA_FTS

typedef PndFTSCATrackParamVector TrackParamVector;

#endif
