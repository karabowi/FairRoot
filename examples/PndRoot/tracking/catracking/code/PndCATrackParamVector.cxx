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

// $Id: PndCATrackParamVector.cxx,v 1.1.1.1 2010/07/26 20:55:38 ikulakov Exp $
// **************************************************************************
// This file is property of and copyright by the ALICE HLT Project          *
// ALICE Experiment at CERN, All rights reserved.                           *
//                                                                          *
// Primary Authors: Sergey Gorbunov <sergey.gorbunov@kip.uni-heidelberg.de> *
//                  Ivan Kisel <kisel@kip.uni-heidelberg.de>                *
//                  for The ALICE HLT Project.                              *
//                                                                          *
// Developed by:   Igor Kulakov <I.Kulakov@gsi.de>                          *
//                 Maksym Zyzak <M.Zyzak@gsi.de>                            *
//                                                                          *
// Permission to use, copy, modify and distribute this software and its     *
// documentation strictly for non-commercial purposes is hereby granted     *
// without fee, provided that the above copyright notice appears in all     *
// copies and that both the copyright notice and this permission notice     *
// appear in the supporting documentation. The authors make no claims       *
// about the suitability of this software for any purpose. It is            *
// provided "as is" without express or implied warranty.                    *
//                                                                          *
//***************************************************************************

#include "PndCATrackParamVector.h"
#include "PndCAMath.h"
#include "PndCATrackLinearisationVector.h"
#include "PndCAParam.h"
#include "PndCAHitsV.h"
#include "PndCATarget.h"

#include <iostream>
#include <iomanip>
// #ifndef NVALGRIND
// #include <valgrind/memcheck.h>
// #endif
#include <assert.h>
//#include "debug.h"

//
// Circle in XY:
//
// kCLight = 0.000299792458;
// Kappa = Bz*kCLight*QPt;
// R  = 1/CAMath::Abs(Kappa);
// Xc = X - sin(Phi)/Kappa;
// Yc = Y + cos(Phi)/Kappa;
//

void PndCATrackParamVector::InitCovMatrix(float_v d2QMom)
{
  SetCov(0.f, 100.f);
  SetCov(1, 0.f);
  SetCov(2, 100.f);
  SetCov(3, 0.f);
  SetCov(4, 0.f);
  SetCov(5, 1.f);
  SetCov(6, 0.f);
  SetCov(7, 0.f);
  SetCov(8, 0.f);
  SetCov(9, 1.f);
  SetCov(10, 0.f);
  SetCov(11, 0.f);
  SetCov(12, 0.f);
  SetCov(13, 0.f);
  SetCov(14, d2QMom);

  SetChi2(0.f);

  SetNDF(-5);
}

void PndCATrackParamVector::InitByTarget(const PndCATarget &t)
{
  SetX(t.X0());
  SetY(t.X1());
  SetZ(t.X2());
  SetSinPhi(1.f);
  SetSignCosPhi(0.f);
  SetDzDs(0.f);
  SetQPt(0.f);
  SetCov(0.f, t.Err2X1());
  SetCov(1, 0.f);
  SetCov(2, t.Err2X2());
  SetCov(3, 0.f);
  SetCov(4, 0.f);
  SetCov(5, 1.f);
  SetCov(6, 0.f);
  SetCov(7, 0.f);
  SetCov(8, 0.f);
  SetCov(9, 1.f);
  SetCov(10, 0.f);
  SetCov(11, 0.f);
  SetCov(12, 0.f);
  SetCov(13, 0.f);
  SetCov(14, t.Err2QMom());

  SetChi2(0.f);

  SetNDF(-5 + t.NDF());

  // SetField( 0, t.B(), t.X0() );
}

void PndCATrackParamVector::InitByHit(const PndCAHitV &hit, const PndCAParam &param, const float_v &dQMom)
{
  SetX(hit.X0());
  SetY(hit.X1());
  SetZ(hit.X2());
  SetSinPhi(0.f);
  SetSignCosPhi(1.f);
  SetDzDs(0.f);
  SetQPt(0.f);
  SetAngle(hit.Angle());

  SetCov(0, hit.Err2X1());
  SetCov(1, 0.f);
  SetCov(2, hit.Err2X2());
  SetCov(3, 0.f);
  SetCov(4, 0.f);
  SetCov(5, 1.f);
  SetCov(6, 0.f);
  SetCov(7, 0.f);
  SetCov(8, 0.f);
  SetCov(9, 1.f);
  SetCov(10, 0.f);
  SetCov(11, 0.f);
  SetCov(12, 0.f);
  SetCov(13, 0.f);
  SetCov(14, dQMom * dQMom);

  SetChi2(0.f);

  SetNDF(-3);

  UNUSED_PARAM1(param);
  // const int ista0 = hit.IStation();
  // const PndCAStation sta0 = param.Station(ista0);

  // L1FieldValue B;
  // sta0.fieldSlice.GetFieldValue( hit.X1(), hit.X2(), B );
  // SetField( 1, B, sta0.z );

  // if ( ista0 > 0 ) {
  //   const L1Station staC = param.Station(ista0/2); // The field will be used to propaget to the target. Therefore take station in a middle between target and hit.
  //   const float_v tx1 = hit.X1()/hit.X0(); // suppose target is at 0
  //   const float_v tx2 = hit.X2()/hit.X0(); // suppose target is at 0
  //   staC.fieldSlice.GetFieldValue( tx1*staC.z, tx2*staC.z, B );
  //   SetField( 0, B, staC.z );
  // }
}

float_v PndCATrackParamVector::GetDist2(const PndCATrackParamVector &t) const
{
  // get squared distance between tracks

  const float_v &dx = GetX() - t.GetX();
  const float_v &dy = GetY() - t.GetY();
  const float_v &dz = GetZ() - t.GetZ();
  return dx * dx + dy * dy + dz * dz;
}

float_v PndCATrackParamVector::GetDistXZ2(const PndCATrackParamVector &t) const
{
  // get squared distance between tracks in X&Z

  const float_v &dx = GetX() - t.GetX();
  const float_v &dz = GetZ() - t.GetZ();
  return dx * dx + dz * dz;
}

float_v PndCATrackParamVector::GetS(const float_v &_x, const float_v &_y, const float_v &Bz) const
{
  //* Get XY path length to the given point

  const float_v &k = GetKappa(Bz);
  const float_v &ex = GetCosPhi();
  const float_v &ey = GetSinPhi();
  const float_v &x = _x - GetX();
  const float_v &y = _y - GetY();
  float_v dS = x * ex + y * ey;
  const float_m &mask = CAMath::Abs(k) > 1.e-4f;
  if (!mask.isEmpty()) {
    dS(mask) = CAMath::ATan2(k * dS, 1 + k * (x * ey - y * ex)) / k;
  }
  return dS;
}

void PndCATrackParamVector::GetDCAPoint(const float_v &x, const float_v &y, const float_v &z, float_v *xp, float_v *yp, float_v *zp, const float_v &Bz) const
{
  //* Get the track point closest to the (x,y,z)

  const float_v &x0 = GetX();
  const float_v &y0 = GetY();
  const float_v &k = GetKappa(Bz);
  const float_v &ex = GetCosPhi();
  const float_v &ey = GetSinPhi();
  const float_v &dx = x - x0;
  const float_v &dy = y - y0;
  const float_v &ax = dx * k + ey;
  const float_v &ay = dy * k - ex;
  const float_v &a = sqrt(ax * ax + ay * ay);
  *xp = x0 + (dx - ey * ((dx * dx + dy * dy) * k - 2.f * (-dx * ey + dy * ex)) / (a + 1.f)) / a;
  *yp = y0 + (dy + ex * ((dx * dx + dy * dy) * k - 2.f * (-dx * ey + dy * ex)) / (a + 1.f)) / a;
  const float_v s = GetS(x, y, Bz);
  *zp = GetZ() + GetDzDs() * s;
  const float_v dZ = CAMath::Abs(GetDzDs() * CAMath::TwoPi() / k);
  const float_m mask = CAMath::Abs(k) > 1.e-2f && dZ > .1f;
  (*zp)(mask) += CAMath::Round((z - *zp) / dZ) * dZ;
}

//*
//* Transport routines
//*

float_m PndCATrackParamVector::TransportToX(const float_v &x, PndCATrackLinearisationVector &t0, const float_v &Bz, const float maxSinPhi, float_v *DL, const float_m &_mask)
{
  //* Transport the track parameters to X=x, using linearization at t0, and the field value Bz
  //* maxSinPhi is the max. allowed value for |t0.SinPhi()|
  //* linearisation of trajectory t0 is also transported to X=x,
  //* returns 1 if OK
  //*
  // std::cout << t0.QPt()[0] << " " << fP[4][0] << std::endl;
  const float_v ex = t0.CosPhi();
  const float_v ey = t0.SinPhi();
  const float_v k = t0.QPt() * Bz;
  const float_v dx = x - X();

  float_v ey1 = k * dx + ey;

  // check for intersection with X=x

  float_v ex1 = CAMath::Sqrt(1.f - ey1 * ey1);
  ex1(ex < Vc::Zero) = -ex1;

  const float_v dx2 = dx * dx;
  const float_v ss = ey + ey1;
  const float_v cc = ex + ex1;

  const float_v cci = 1.f / cc;
  const float_v exi = 1.f / ex;
  const float_v ex1i = 1.f / ex1;

  float_m mask = _mask && CAMath::Abs(ey1) <= maxSinPhi && CAMath::Abs(cc) >= 1.e-4f && CAMath::Abs(ex) >= 1.e-4f && CAMath::Abs(ex1) >= 1.e-4f;

  const float_v tg = ss * cci; // tan((phi1+phi)/2)

  const float_v dy = dx * tg;
  float_v dl = dx * CAMath::Sqrt(1.f + tg * tg);

  dl(cc < Vc::Zero) = -dl;
  const float_v dSin = CAMath::Max(float_v(-1.f), CAMath::Min(float_v(Vc::One), dl * k * 0.5f));
  const float_v dS = (CAMath::Abs(k) > 1.e-4f) ? (2 * CAMath::ASin(dSin) / k) : dl;
  const float_v dz = dS * t0.DzDs();

  if (DL) {
    (*DL)(mask) = -dS * CAMath::Sqrt(1.f + t0.DzDs() * t0.DzDs());
  }

  const float_v d[3] = {fP[2] - t0.SinPhi(), fP[3] - t0.DzDs(), fP[4] - t0.QPt()};

  // float H0[5] = { 1,0, h2,  0, h4 };
  // float H1[5] = { 0, 1, 0, dS,  0 };
  // float H2[5] = { 0, 0, 1,  0, dxBz };
  // float H3[5] = { 0, 0, 0,  1,  0 };
  // float H4[5] = { 0, 0, 0,  0,  1 };

  const float_v h2 = dx * (1.f + ey * ey1 + ex * ex1) * exi * ex1i * cci;
  const float_v h4 = dx2 * (cc + ss * ey1 * ex1i) * cci * cci * Bz;
  const float_v dxBz = dx * Bz;

  ex1(!mask) = ex;
  ey1(!mask) = ey;

  // std::cout << "QPt  " << t0.QPt()[0] << " " << fP[4][0] <<
  //              "  CosPhi " << t0.CosPhi()[0] << " " << ex1[0] << std::endl;

  t0.SetCosPhi(ex1);
  t0.SetSinPhi(ey1);

  fX(mask) = X() + dx;
  fP[0](mask) = Y() + dy + h2 * d[0] + h4 * d[2];
  fP[1](mask) = Z() + dz + dS * d[1];
  fP[2](mask) = t0.SinPhi() + d[0] + dxBz * d[2];
  fP[2](CAMath::Abs(fP[2]) > maxSinPhi) = t0.SinPhi();

  const float_v c00 = fC[0];
  const float_v c10 = fC[1];
  const float_v c11 = fC[2];
  const float_v c20 = fC[3];
  const float_v c21 = fC[4];
  const float_v c22 = fC[5];
  const float_v c30 = fC[6];
  const float_v c31 = fC[7];
  const float_v c32 = fC[8];
  const float_v c33 = fC[9];
  const float_v c40 = fC[10];
  const float_v c41 = fC[11];
  const float_v c42 = fC[12];
  const float_v c43 = fC[13];
  const float_v c44 = fC[14];

  fC[0](mask) = (c00 + h2 * h2 * c22 + h4 * h4 * c44 + 2.f * (h2 * c20 + h4 * c40 + h2 * h4 * c42));

  fC[1](mask) = c10 + h2 * c21 + h4 * c41 + dS * (c30 + h2 * c32 + h4 * c43);
  fC[2](mask) = c11 + 2.f * dS * c31 + dS * dS * c33;

  fC[3](mask) = c20 + h2 * c22 + h4 * c42 + dxBz * (c40 + h2 * c42 + h4 * c44);
  fC[4](mask) = c21 + dS * c32 + dxBz * (c41 + dS * c43);
  fC[5](mask) = c22 + 2.f * dxBz * c42 + dxBz * dxBz * c44;

  fC[6](mask) = c30 + h2 * c32 + h4 * c43;
  fC[7](mask) = c31 + dS * c33;
  fC[8](mask) = c32 + dxBz * c43;
  fC[9](mask) = c33;

  fC[10](mask) = c40 + h2 * c42 + h4 * c44;
  fC[11](mask) = c41 + dS * c43;
  fC[12](mask) = c42 + dxBz * c44;
  fC[13](mask) = c43;
  fC[14](mask) = c44;
  // std::cout << fC[10] << " " << fC[11] <<" " << h2 << " " << h4 << " " << c44 << " " << mask << " " << Bz << std::endl;
  // debugKF() << mask << "\n" << *this << std::endl;
  return mask;
}

float_m PndCATrackParamVector::TransportToX(const float_v &x, const float_v &Bz, const float maxSinPhi, const float_m &mask)
{
  //* Transport the track parameters to X=x

  //  assert( ( x == 0 && mask ).isEmpty() );

  PndCATrackLinearisationVector t0(*this);

  return TransportToX(x, t0, Bz, maxSinPhi, 0, mask);
}

float_m PndCATrackParamVector::TransportToXWithMaterial(const float_v &x, PndCATrackLinearisationVector &t0, PndCATrackFitParam &par, const float_v &XOverX0,
                                                        const float_v &XThimesRho, const float_v &Bz, const float maxSinPhi, const float_m &mask_)
{
  //* Transport the track parameters to X=x  taking into account material budget
  //   UNUSED_PARAM1(par);
  // const float kRho = 1.025e-3f;//0.9e-3;
  //  const float kRadLen = 29.532f;//28.94;
  //  const float kRhoOverRadLen = kRho / kRadLen;
  // const float kRhoOverRadLen = 7.68e-5;
  float_v dl;

  float_m mask = mask_ && TransportToX(x, t0, Bz, maxSinPhi, &dl, mask_);
  //   if ( !mask.isEmpty() ) {
  //     CorrectForMeanMaterial( dl * kRhoOverRadLen, dl * kRho, par, mask );
  //   }

  const float_v dzds2 = t0.DzDs() * t0.DzDs();
  const float_v sphi2 = t0.SinPhi() * t0.SinPhi();
  float_v koeff = sqrt((1 + dzds2) * (1 + sphi2)); // = dr/dx
  mask &= CorrectForMeanMaterial(XOverX0 * koeff, XThimesRho * koeff, par, mask);

  return mask;
}

float_m PndCATrackParamVector::TransportToXWithMaterial(const float_v &x, PndCATrackFitParam &par, const float_v &XOverX0, const float_v &XThimesRho, const float_v &Bz,
                                                        const float maxSinPhi)
{
  //* Transport the track parameters to X=x  taking into account material budget

  PndCATrackLinearisationVector t0(*this);
  return TransportToXWithMaterial(x, t0, par, XOverX0, XThimesRho, Bz, maxSinPhi);
}

float_m PndCATrackParamVector::TransportToXWithMaterial(const float_v &x, const float_v &XOverX0, const float_v &XThimesRho, const float_v &Bz, const float maxSinPhi)
{
  //* Transport the track parameters to X=x taking into account material budget

  PndCATrackFitParam par;
  CalculateFitParameters(par);
  return TransportToXWithMaterial(x, par, XOverX0, XThimesRho, Bz, maxSinPhi);
}

//*
//*  Multiple scattering and energy losses
//*

float_v PndCATrackParamVector::BetheBlochGeant(const float_v &bg2, const float_v &kp0, const float_v &kp1, const float_v &kp2, const float_v &kp3, const float_v &kp4)
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
  const float_v x = 0.5f * CAMath::Log(bg2);
  const float_v lhwI = CAMath::Log(28.816f * 1e-9f * CAMath::Sqrt(rho * mZA) / mI);
  d2(x > x1) = lhwI + x - 0.5f;
  const float_v &r = (x1 - x) / (x1 - x0);
  d2(x > x0 && x <= x1) = lhwI + x - 0.5f + (0.5f - lhwI - x0) * r * r * r;

  return mK * mZA * (float_v(Vc::One) + bg2) / bg2 * (0.5f * CAMath::Log(maxT * maxT / (mI * mI)) - bg2 / (float_v(Vc::One) + bg2) - d2);
}

float_v PndCATrackParamVector::BetheBlochSolid(const float_v &bg)
{
  //------------------------------------------------------------------
  // This is an approximation of the Bethe-Bloch formula,
  // reasonable for solid materials.
  // All the parameters are, in fact, for Si.
  // The returned value is in [GeV]
  //------------------------------------------------------------------

  return BetheBlochGeant(bg);
}

float_v PndCATrackParamVector::BetheBlochGas(const float_v &bg)
{
  //------------------------------------------------------------------
  // This is an approximation of the Bethe-Bloch formula,
  // reasonable for gas materials.
  // All the parameters are, in fact, for Ne.
  // The returned value is in [GeV]
  //------------------------------------------------------------------

  const float_v rho = 0.9e-3f;
  const float_v x0 = 2.f;
  const float_v x1 = 4.f;
  const float_v mI = 140.e-9f;
  const float_v mZA = 0.49555f;

  return BetheBlochGeant(bg, rho, x0, x1, mI, mZA);
}

float_v PndCATrackParamVector::ApproximateBetheBloch(const float_v &beta2)
{
  //------------------------------------------------------------------
  // This is an approximation of the Bethe-Bloch formula with
  // the density effect taken into account at beta*gamma > 3.5
  // (the approximation is reasonable only for solid materials)
  //------------------------------------------------------------------

  const float_v &beta2_1subBeta2 = beta2 / (float_v(Vc::One) - beta2); // beta2 * CAMath::Reciprocal( float_v( Vc::One ) - beta2 );
  const float_v &_0p000153_beta2 = 0.153e-3f / beta2;
  const float log_3p5mul5940 = 9.942227380852058f; // log( 3.5 * 5940 )
  const float log_5940 = 8.68946441235669f;        // log( 5940 )
  const float_v log_beta2_1subBeta2 = CAMath::Log(beta2_1subBeta2);

  float_v ret = _0p000153_beta2 * (log_5940 + log_beta2_1subBeta2 - beta2);
  ret(beta2_1subBeta2 > 3.5f * 3.5f) = _0p000153_beta2 * (log_3p5mul5940 + 0.5f * log_beta2_1subBeta2 - beta2);
  ret.setZero(beta2 >= float_v(Vc::One));
  return ret;
}

void PndCATrackParamVector::CalculateFitParameters(PndCATrackFitParam &par, const float_v &mass)
{
  //*!

  const float_v p2 = (float_v(Vc::One) + fP[3] * fP[3]);
  const float_v k2 = fP[4] * fP[4];
  const float_v mass2 = mass * mass;
  const float_v beta2 = p2 / (p2 + mass2 * k2);

  float_v pp2 = 10000.f;
  pp2(k2 > 1.e-8f) = p2 / k2; // impuls 2

  // par.fBethe = BetheBlochGas( pp2/mass2);
  // par.fBethe = ApproximateBetheBloch( pp2 / mass2 ); Why pp2 / mass2?, should be beta2.
  par.fBethe = ApproximateBetheBloch(beta2);
  par.fE = CAMath::Sqrt(pp2 + mass2);
  par.fTheta2 = 14.1f * 14.1f / (beta2 * pp2 * 1.e6f);
  par.fEP2 = par.fE / pp2;

  // Approximate energy loss fluctuation (M.Ivanov)

  const float knst = 0.07f; // To be tuned.
  par.fSigmadE2 = knst * par.fEP2 * fP[4];
  par.fSigmadE2 = par.fSigmadE2 * par.fSigmadE2;

  par.fK22 = (float_v(Vc::One) + fP[3] * fP[3]);
  par.fK33 = par.fK22 * par.fK22;
  par.fK43 = fP[3] * fP[4] * par.fK22;
  par.fK44 = fP[3] * fP[3] * fP[4] * fP[4];
}

float_m PndCATrackParamVector::CorrectForMeanMaterial(const float_v &xOverX0, const float_v &xTimesRho, const PndCATrackFitParam &par, const float_m &_mask)
{
  //------------------------------------------------------------------
  // This function corrects the track parameters for the crossed material.
  // "xOverX0"   - X/X0, the thickness in units of the radiation length.
  // "xTimesRho" - is the product length*density (g/cm^2).
  //------------------------------------------------------------------

  float_v &fC22 = fC[5];
  float_v &fC33 = fC[9];
  float_v &fC40 = fC[10];
  float_v &fC41 = fC[11];
  float_v &fC42 = fC[12];
  float_v &fC43 = fC[13];
  float_v &fC44 = fC[14];

  // Energy losses************************

  const float_v &dE = par.fBethe * xTimesRho;
  assert(dE >= 0);
  float_m mask = _mask && dE <= 0.3f * par.fE; // 30% energy loss is too much!
  const float_v &corr = (float_v(Vc::One) - par.fEP2 * dE);
  mask &= corr >= 0.3f && corr <= 1.3f;

  fP[4](mask) *= corr;
  fC40(mask) *= corr;
  fC41(mask) *= corr;
  fC42(mask) *= corr;
  fC43(mask) *= corr;
  fC44(mask) *= corr * corr;
  fC44(mask) += par.fSigmadE2 * CAMath::Abs(dE);

  // Multiple scattering******************

  assert(xOverX0 >= 0);
  const float_v &theta2 = par.fTheta2 * xOverX0;
  fC22(mask) += theta2 * par.fK22 * (float_v(Vc::One) - fP[2] * fP[2]);
  fC33(mask) += theta2 * par.fK33;
  fC43(mask) += theta2 * par.fK43;
  fC44(mask) += theta2 * par.fK44;

  return mask;
}

float_m PndCATrackParamVector::Rotate(const float_v &alpha, PndCATrackLinearisationVector &t0, const float maxSinPhi, const float_m &mask)
{
  //* Rotate the coordinate system in XY on the angle alpha
  if ((CAMath::Abs(alpha) < 1e-6f || !mask).isFull())
    return mask;

  const float_v cA = CAMath::Cos(alpha);
  const float_v sA = CAMath::Sin(alpha);
  const float_v x0 = X(), y0 = Y(), sP = t0.SinPhi(), cP = t0.CosPhi();
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
  t0.SetCosPhi(cosPhi);
  t0.SetSinPhi(sinPhi);

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

float_m PndCATrackParamVector::FilterWithMaterial(const float_v &y, const float_v &z, float_v err2Y, float_v errYZ, float_v err2Z, float maxSinPhi, const float_m &mask,
                                                  const int_v &hitNDF, const float_v &chi2Cut)
{
  assert(maxSinPhi > 0.f);
  //* Add the y,z measurement with the Kalman filter

  const float_v c00 = fC[0];
  const float_v c10 = fC[1];
  const float_v c11 = fC[2];
  const float_v c20 = fC[3];
  const float_v c21 = fC[4];
  //  float c22 = fC[5];
  const float_v c30 = fC[6];
  const float_v c31 = fC[7];
  //  float c32 = fC[8];
  //  float c33 = fC[9];
  const float_v c40 = fC[10];
  const float_v c41 = fC[11];
  //  float c42 = fC[12];
  //  float c43 = fC[13];
  //  float c44 = fC[14];

  const float_v z0 = y - fP[0], z1 = z - fP[1];

  // foreach_bit( int i, mask ) {
  //   cout << i << " e = " << err2Y[i] << " " << errYZ[i] << " " << err2Z[i] << " c = " << c00[i] << " " << c10[i] << " " << c11[i] <<  " z = " << z0[i] <<  " " << z1[i]  << "
  //   chi2 = " << fChi2[i]<< endl;
  // }

  err2Y += c00;
  err2Z += c11;
  errYZ += c10;
  float_v d = float_v(Vc::One) / (err2Y * err2Z - errYZ * errYZ);

  float_m success = mask; //  if ( ISUNLIKELY( err2Y < 1.e-8f ) || ISUNLIKELY( err2Z < 1.e-8f ) ) return 0;
  success &= (err2Y > 1.e-8f) && (err2Z > 1.e-8f);

  const float_v mS0 = err2Z * d;
  const float_v mS1 = -errYZ * d;
  const float_v mS2 = err2Y * d;

  // K = CHtS

  const float_v k00 = c00 * mS0 + c10 * mS1, k01 = c00 * mS1 + c10 * mS2, k10 = c10 * mS0 + c11 * mS1, k11 = c10 * mS1 + c11 * mS2, k20 = c20 * mS0 + c21 * mS1,
                k21 = c20 * mS1 + c21 * mS2, k30 = c30 * mS0 + c31 * mS1, k31 = c30 * mS1 + c31 * mS2, k40 = c40 * mS0 + c41 * mS1, k41 = c40 * mS1 + c41 * mS2;

  const float_v sinPhi = fP[2] + k20 * z0 + k21 * z1;

  success &= CAMath::Abs(sinPhi) < maxSinPhi;

  fNDF(static_cast<int_m>(success)) += hitNDF;
  fChi2(success) += mS0 * z0 * z0 + 2 * z0 * z1 * mS1 + mS2 * z1 * z1;
  success &= fChi2 < chi2Cut;
  if (success.isEmpty())
    return success; // TODO move upper

  // foreach_bit( int i, success ) {
  //   cout << i << " " << mS0[i] << " " << mS1[i] << " " << mS2[i] << " " << (mS0 * z0 * z0)[i] << " " << (2 * z0 * z1 * mS1)[i] << " " << (mS2 * z1 * z1)[i] << " " << fChi2[i] <<
  //   endl;
  // }
  // std::cout << success << std::endl;
  fP[0](success) += k00 * z0 + k01 * z1;
  fP[1](success) += k10 * z0 + k11 * z1;
  fP[2](success) = sinPhi;
  fP[3](success) += k30 * z0 + k31 * z1;
  fP[4](success) += k40 * z0 + k41 * z1;
  fC[0](success) -= (k00 * c00 + k01 * c10); // c00

  fC[1](success) -= (k10 * c00 + k11 * c10); // c10
  fC[2](success) -= (k10 * c10 + k11 * c11); // c11

  fC[3](success) -= (k20 * c00 + k21 * c10); // c20
  fC[4](success) -= (k20 * c10 + k21 * c11); // c21
  fC[5](success) -= (k20 * c20 + k21 * c21); // c22

  fC[6](success) -= (k30 * c00 + k31 * c10); // c30
  fC[7](success) -= (k30 * c10 + k31 * c11); // c31
  fC[8](success) -= (k30 * c20 + k31 * c21); // c32
  fC[9](success) -= (k30 * c30 + k31 * c31); // c33

  fC[10](success) -= (k40 * c00 + k41 * c10); // c40
  fC[11](success) -= (k40 * c10 + k41 * c11); // c41
  // std::cout << fC[11] << " " << fC[10] <<  std::endl;

  fC[12](success) -= (k40 * c20 + k41 * c21); // c42
  fC[13](success) -= (k40 * c30 + k41 * c31); // c43
  fC[14](success) -= (k40 * c40 + k41 * c41); // c44

  return success;
}

float_m PndCATrackParamVector::FilterWithMaterial(const float_v &y, const float_v &z, const PndCAStripInfo &info, float_v err2, float maxSinPhi, const float_m &mask,
                                                  const float_v &chi2Cut)
//* Adds the 1-D measurement with the Kalman filter
//* @beta is angle between the strip and z-axis, clockwise
{
  assert(maxSinPhi > 0.f);
  float_m success = mask;
  success &= (err2 > 1.e-8f);

  const float_v &c00 = fC[0];
  const float_v &c10 = fC[1];
  const float_v &c11 = fC[2];

  const float_v &sb = info.sin;
  const float_v &cb = info.cos;

  const float_v u = cb * y + sb * z;
  const float_v zeta = cb * fP[0] + sb * fP[1] - u;

  // F = CH'
  const float_v F0 = cb * c00 + sb * c10;
  const float_v F1 = cb * c10 + sb * c11;

  const float_v HCH = (F0 * cb + F1 * sb);

  const float_v wi = 1 / (err2 + HCH);
  const float_v zetawi = zeta * wi;

  fChi2(success) += zeta * zetawi;
  success &= fChi2 < chi2Cut;
  if (success.isEmpty())
    return success;

  const float_v &c20 = fC[3];
  const float_v &c21 = fC[4];
  const float_v &c30 = fC[6];
  const float_v &c31 = fC[7];
  const float_v &c40 = fC[10];
  const float_v &c41 = fC[11];

  const float_v F2 = cb * c20 + sb * c21;
  const float_v F3 = cb * c30 + sb * c31;
  const float_v F4 = cb * c40 + sb * c41;

  const float_v K1 = F1 * wi;
  const float_v K2 = F2 * wi;
  const float_v K3 = F3 * wi;
  const float_v K4 = F4 * wi;

  const float_v sinPhi = fP[2] - F2 * zetawi;

  success &= CAMath::Abs(sinPhi) < maxSinPhi;

  fNDF(static_cast<int_m>(success)) += 1; // TODO

  fP[0](success) -= F0 * zetawi;
  fP[1](success) -= F1 * zetawi;
  fP[2](success) = sinPhi;
  fP[3](success) -= F3 * zetawi;
  fP[4](success) -= F4 * zetawi;

  fC[0](success) -= F0 * F0 * wi;

  fC[1](success) -= K1 * F0;
  fC[2](success) -= K1 * F1;

  fC[3](success) -= K2 * F0;
  fC[4](success) -= K2 * F1;
  fC[5](success) -= K2 * F2;

  fC[6](success) -= K3 * F0;
  fC[7](success) -= K3 * F1;
  fC[8](success) -= K3 * F2;
  fC[9](success) -= K3 * F3;

  fC[10](success) -= K4 * F0;
  fC[11](success) -= K4 * F1;

  fC[12](success) -= K4 * F2;
  fC[13](success) -= K4 * F3;
  fC[14](success) -= K4 * F4;

  return success;
}

float_m PndCATrackParamVector::Transport(const int_v &ista, const PndCAParam &param, const float_m &mask)
{
  float_m active = mask;
  PndCATrackLinearisationVector tE(*this);
  active &= TransportToX(param.GetR(ista, active), tE, param.cBz(), 0.999f, nullptr, active);
  // SG!!
  /*
  PndCATrackFitParam fitPar;
  CalculateFitParameters( fitPar );
  PndCATrackLinearisationVector tE( *this );
  active &= TransportToXWithMaterial( param.GetR( ista, active ), tE, fitPar, param.GetXOverX0(int_v(ista),active), param.GetXTimesRho(int_v(ista),active), param.cBz(), 0.999f,
  active );
  */
  return active;
}

float_m PndCATrackParamVector::Transport(const PndCAHitV &hit, const PndCAParam &param, const float_m &mask)
{
  if (((CAMath::Abs(-fAlpha + hit.Angle()) < 1e-6f && CAMath::Abs(-fX + hit.X0()) < 2e-4f) || !mask).isFull())
    return mask;
  float_m active = mask & hit.IsValid();
  PndCATrackLinearisationVector tR(*this);
  active &= Rotate(-fAlpha + hit.Angle(), tR, .999f, active);
  active &= TransportToX(hit.X0(), tR, param.cBz(), 0.999f, nullptr, active);

  // SG!!
  /*
  PndCATrackFitParam fitPar;
  CalculateFitParameters( fitPar );
  active &= TransportToXWithMaterial( hit.X0(), tR, fitPar, param.GetXOverX0(hit.IStations(),active), param.GetXTimesRho(hit.IStations(),active), param.cBz(), 0.999f, active );
  */
  return active;
}

float_m PndCATrackParamVector::Filter(const PndCAHitV &hit, const PndCAParam &param, const float_m &mask, const float_v &chi2Cut)
{
  const int iSta = hit.IStations()[hit.IsValid().firstOne()];
  if (param.Station(iSta).NDF == 1) {
    float_m m = FilterWithMaterial(hit.X1Corrected(*this), hit.X2(), param.Station(iSta).f, hit.Err2R(), 0.999f, mask, chi2Cut);
    return m;
  }
  // SG!!
  float_m m = FilterWithMaterial(hit.X1(), hit.X2(), float_v(3.f) * hit.Err2X1(), hit.ErrX12(), float_v(3.f) * hit.Err2X2(), 0.999f, mask,
                                 int_v(param.Station(hit.IStations()[0]).NDF), chi2Cut);
  return m;
}

float_m PndCATrackParamVector::Transport(const PndCAHit &hit, const PndCAParam &param, const float_m &mask)
{
  if (((CAMath::Abs(-fAlpha + hit.Angle()) < 1e-6f && CAMath::Abs(-fX + hit.X0()) < 2e-4f) || !mask).isFull())
    return mask;

  float_m active = mask;

  PndCATrackLinearisationVector tR(*this);
  active &= Rotate(-fAlpha + hit.Angle(), tR, .999f, active);

  active &= TransportToX(hit.X0(), tR, param.cBz(), 0.999f, nullptr, active);
  /*
  PndCATrackFitParam fitPar;
  CalculateFitParameters( fitPar );
  active &= TransportToXWithMaterial( hit.X0(), tR, fitPar, param.GetXOverX0(hit.IStation()), param.GetXTimesRho(hit.IStation()), param.cBz(), 0.999f, active );
  */
  return active;
}

float_m PndCATrackParamVector::Filter(const PndCAHit &hit, const PndCAParam &param, const float_m &mask, const float_v &chi2Cut)
{
  if (param.Station(hit.IStation()).NDF == 1) {
    return FilterWithMaterial(hit.X1Corrected(this->SinPhi()), hit.X2(), param.Station(hit.IStation()).f, hit.Err2R(), 0.999f, mask, chi2Cut);
  }
  return FilterWithMaterial(hit.X1(), hit.X2(), hit.Err2X1(), hit.ErrX12(), hit.Err2X2(), 0.999f, mask, int_v(param.Station(hit.IStation()).NDF), chi2Cut);
}

float_m PndCATrackParamVector::AddTarget(const PndCATarget &target, const float_m &mask)
{
  float_m active = mask;

  float_v eY, eZ;
  // float_v dz = target.X0() - X0();
  float_v J[6];
  // H = 1 0 J[0] J[1] J[2]
  //     0 1 J[3] J[4] J[5]
  active &= TransportJ0ToX0(target.X0(), static_cast<float_v>(target.B()), eY, eZ, J, active);
  active &= FilterVtx(target.X1(), target.X2(), PndCAX1X2MeasurementInfo(target.Err2X1(), 0.f, target.Err2X2()), eY, eZ, J, active);
  fNDF(static_cast<int_m>(active)) += target.NDF();

  return active;
}

#include <iostream>

// std::istream &operator>>( std::istream &in, PndCATrackParamVector &t )
// {
//   float_v::Memory x, s, p[5], c[15], chi2;
//   int_v::Memory ndf;
//   for ( int j = 0; j < uint_v::Size; ++j ) {
//     in >> x[j];
//     in >> s[j];
//     for ( int i = 0; i < 5; i++ ) in >> p[i][j];
//     for ( int i = 0; i < 15; i++ ) in >> c[i][j];
//     in >> chi2[j];
//     in >> ndf[j];
//   }
//   t.fX.load( x );
//   t.fSignCosPhi.load( s );
//   for ( int i = 0; i < 5; i++ ) t.fP[i].load( p[i] );
//   for ( int i = 0; i < 5; i++ ) t.fC[i].load( c[i] );
//   t.fChi2.load( chi2 );
//   t.fNDF.load( ndf );
//   return in;
// }
//
// std::ostream &operator<<( std::ostream &out, const PndCATrackParamVector &t )
// {
//   if ( out == std::cerr ) {
//     out << "------------------------------ Track Param ------------------------------"
//         << "\n             X: " << t.X()
//         << "\n    SignCosPhi: " << t.SignCosPhi()
//         << "\n          Chi2: " << t.Chi2()
//         << "\n           NDF: " << t.NDF()
//         << "\n             Y: " << t.Par()[0]
//         << "\n             Z: " << t.Par()[1]
//         << "\n        SinPhi: " << t.Par()[2]
//         << "\n          DzDs: " << t.Par()[3]
//         << "\n          q/Pt: " << t.Par()[4]
//         << "\nCovariance Matrix\n";
//     int i = 0;
//     out << std::setprecision( 2 );
//     for ( int step = 1; step <= 5; ++step ) {
//       int end = i + step;
//       for ( ; i < end; ++i ) {
//         out << t.Cov()[i] << '\t';
//       }
//       out << "\n";
//     }
//     out << std::setprecision( 6 );
//     return out << std::endl;
//   }
//   for ( int j = 0; j < uint_v::Size; ++j ) {
//     out << t.X()[j] << " "
//         << t.SignCosPhi()[j] << " "
//         << t.Chi2()[j] << " "
//         << t.NDF()[j]
//         << std::endl;
//     for ( int i = 0; i < 5; i++ ) out << t.Par()[i][j] << " ";
//     out << std::endl;
//     for ( int i = 0; i < 15; i++ ) out << t.Cov()[i][j] << " ";
//     out << std::endl;
//   }
//   return out;
// }

float_m PndCATrackParamVector::Transport0ToX(const float_v &x, const float_v &Bz, const float_m &_mask)
{
  //* Transport the track parameters to X=x, using linearization at t0, and the field value Bz
  //* maxSinPhi is the max. allowed value for |t0.SinPhi()|
  //* linearisation of trajectory t0 is also transported to X=x,
  //* returns 1 if OK
  //*

  const float_v ey = SinPhi();
  const float_v ex = CAMath::Sqrt(float_v(Vc::One) - ey * ey);
  const float_v dx = x - X();

  float_m mask = _mask && CAMath::Abs(ey) <= .999f;

  const float_v exi = 1.f / ex;
  const float_v dS = dx * exi;
  const float_v h2 = dx * exi * exi * exi;
  const float_v h4 = 0.5f * dx * Bz * h2;
  const float_v dxBz = dx * Bz;

  fX += dx;
  fP[0] += dS * ey + h4 * fP[4];
  fP[1] += dS * DzDs();
  fP[2] += dxBz * fP[4];

  const float_v c00 = fC[0];
  const float_v c10 = fC[1];
  const float_v c11 = fC[2];
  const float_v c20 = fC[3];
  const float_v c21 = fC[4];
  const float_v c22 = fC[5];
  const float_v c30 = fC[6];
  const float_v c31 = fC[7];
  const float_v c32 = fC[8];
  const float_v c33 = fC[9];
  const float_v c40 = fC[10];
  const float_v c41 = fC[11];
  const float_v c42 = fC[12];
  const float_v c43 = fC[13];
  const float_v c44 = fC[14];

  fC[0] = (c00 + h2 * h2 * c22 + h4 * h4 * c44 + 2.f * (h2 * c20 + h4 * c40 + h2 * h4 * c42));

  fC[1] = c10 + h2 * c21 + h4 * c41 + dS * (c30 + h2 * c32 + h4 * c43);
  fC[2] = c11 + 2.f * dS * c31 + dS * dS * c33;

  fC[3] = c20 + h2 * c22 + h4 * c42 + dxBz * (c40 + h2 * c42 + h4 * c44);
  fC[4] = c21 + dS * c32 + dxBz * (c41 + dS * c43);
  fC[5] = c22 + 2.f * dxBz * c42 + dxBz * dxBz * c44;

  fC[6] = c30 + h2 * c32 + h4 * c43;
  fC[7] = c31 + dS * c33;
  fC[8] = c32 + dxBz * c43;

  fC[10] = c40 + h2 * c42 + h4 * c44;
  fC[11] = c41 + dS * c43;
  fC[12] = c42 + dxBz * c44;
  return mask;
}

float_m PndCATrackParamVector::Transport0(const PndCAHitV &hit, const PndCAParam &param, const float_m &mask)
{
  // if ( ((CAMath::Abs(-fAlpha + hit.Angle()) < 1e-6f && CAMath::Abs(-fX + hit.X0()) < 2e-4f) || !mask).isFull() ) return mask;
  float_m active = mask & hit.IsValid();
  active &= Rotate(-fAlpha + hit.Angle(), .999f, active);
  active &= Transport0ToX(hit.X0(), param.cBz(), active);
  return active;
}

float_m PndCATrackParamVector::Accept(const PndCAHit &hit, const PndCAParam &param, const float_m &mask, const float_v &chi2Cut) const
{
  if (param.Station(hit.IStation()).NDF == 1) {
    return AcceptWithMaterial(hit.X1Corrected(this->SinPhi()), hit.X2(), param.Station(hit.IStation()).f, hit.Err2R(), 0.999f, mask, chi2Cut);
  }
  return AcceptWithMaterial(hit.X1(), hit.X2(), hit.Err2X1(), hit.ErrX12(), hit.Err2X2(), 0.999f, mask, int_v(param.Station(hit.IStation()).NDF), chi2Cut);
}

float_m PndCATrackParamVector::AcceptWithMaterial(const float_v &y, const float_v &z, float_v err2Y, float_v errYZ, float_v err2Z, float maxSinPhi, const float_m &mask,
                                                  const int_v & /*hitNDF*/, const float_v &chi2Cut) const
{
  assert(maxSinPhi > 0.f);
  //* Add the y,z measurement with the Kalman filter

  const float_v c00 = fC[0];
  const float_v c10 = fC[1];
  const float_v c11 = fC[2];
  const float_v c20 = fC[3];
  const float_v c21 = fC[4];
  const float_v z0 = y - fP[0], z1 = z - fP[1];

  err2Y += c00;
  err2Z += c11;
  errYZ += c10;
  float_v d1 = (err2Y * err2Z - errYZ * errYZ);

  float_m success = mask; //  if ( ISUNLIKELY( err2Y < 1.e-8f ) || ISUNLIKELY( err2Z < 1.e-8f ) ) return 0;
  success &= (err2Y > 1.e-8f) && (err2Z > 1.e-8f);

  const float_v mS0 = err2Z;
  const float_v mS1 = -errYZ;
  const float_v mS2 = err2Y;

  // K = CHtS

  const float_v k20 = (c20 * mS0 + c21 * mS1), k21 = (c20 * mS1 + c21 * mS2);
  const float_v sinPhi = d1 * fP[2] + (k20 * z0 + k21 * z1);
  success &= CAMath::Abs(sinPhi) < maxSinPhi * d1;
  float_v chi2 = (mS0 * z0 * z0 + 2 * z0 * z1 * mS1 + mS2 * z1 * z1);
  success &= chi2 < d1 * (chi2Cut - fChi2);
  return success;
}

float_m PndCATrackParamVector::AcceptWithMaterial(const float_v &y, const float_v &z, const PndCAStripInfo &info, float_v err2, float maxSinPhi, const float_m &mask,
                                                  const float_v &chi2Cut) const
//* Adds the 1-D measurement with the Kalman filter
//* @beta is angle between the strip and z-axis, clockwise
{
  assert(maxSinPhi > 0.f);
  float_m success = mask;
  success &= (err2 > 1.e-8f);

  const float_v &c00 = fC[0];
  const float_v &c10 = fC[1];
  const float_v &c11 = fC[2];

  const float_v &sb = info.sin;
  const float_v &cb = info.cos;

  const float_v u = cb * y + sb * z;
  const float_v zeta = cb * fP[0] + sb * fP[1] - u;

  // F = CH'
  const float_v F0 = cb * c00 + sb * c10;
  const float_v F1 = cb * c10 + sb * c11;

  const float_v HCH = (F0 * cb + F1 * sb);

  const float_v w = (err2 + HCH);
  success &= zeta * zeta < w * (chi2Cut - fChi2);
  return success;
}
