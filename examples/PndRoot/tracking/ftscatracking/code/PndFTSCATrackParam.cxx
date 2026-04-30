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

// $Id: AliHLTTPCCATrackParam.cxx,v 1.6 2011/05/20 16:11:22 fisyak Exp $
// **************************************************************************
// This file is property of and copyright by the ALICE HLT Project          *
// ALICE Experiment at CERN, All rights reserved.                           *
//                                                                          *
// Primary Authors: Sergey Gorbunov <sergey.gorbunov@kip.uni-heidelberg.de> *
//                  Ivan Kisel <kisel@kip.uni-heidelberg.de>                *
//                  for The ALICE HLT Project.                              *
//                                                                          *
// Developed by:   Igor Kulakov <I.Kulakov@gsi.de>                          *
//                 Mykhailo Pugach <M.Pugach@gsi.de>                        *
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

#include "PndFTSCATrackParam.h"
#include "PndFTSCAMath.h"
#include <iostream>

#ifdef PANDA_FTS

std::istream &operator>>(std::istream &in, PndFTSCATrackParam &t)
{ // TODO
  UNUSED_PARAM1(t);
  // in >> t.z;
  // for ( int i = 0; i < 5; i++ ) in >> t.Par(i);
  // for ( int i = 0; i < 15; i++ ) in >> t.Cov(i);
  return in;
}

std::ostream &operator<<(std::ostream &out, const PndFTSCATrackParam &t)
{ // TODO
  UNUSED_PARAM1(t);
  // out << t.X() << " "
  // << t.SignCosPhi() << " "
  // << t.Chi2() << " "
  // << t.NDF()
  // << '\n';
  // for ( int i = 0; i < 5; i++ ) out << t.Par()[i] << " ";
  // out << '\n';
  // for ( int i = 0; i < 15; i++ ) out << t.Cov()[i] << " ";
  // out << '\n';
  return out;
}

#else // PANDA_FTS
/*
float PndFTSCATrackParam::GetDist2( const PndFTSCATrackParam &t ) const
{
  // get squared distance between tracks

  float dx = X() - t.X();
  float dy = Y() - t.Y();
  float dz = Z() - t.Z();
  return dx*dx + dy*dy + dz*dz;
}

float PndFTSCATrackParam::GetDistXZ2( const PndFTSCATrackParam &t ) const
{
  // get squared distance between tracks in X&Z

  float dx = X() - t.X();
  float dz = Z() - t.Z();
  return dx*dx + dz*dz;
}
*/
#include "PndFTSCAParam.h"

float PndFTSCATrackParam::S(float x, float y, float Bz) const
{
  //* Get XY path length to the given point

  float k = Kappa(Bz);
  float ex = CosPhi();
  float ey = SinPhi();
  x -= X();
  y -= Y();
  float dS = x * ex + y * ey;
  if (CAMath::Abs(k) > 1.e-4)
    dS = CAMath::ATan2(k * dS, 1 + k * (x * ey - y * ex)) / k;
  return dS;
}

void PndFTSCATrackParam::GetDCAPoint(float x, float y, float z, float &xp, float &yp, float &zp, float Bz) const
{
  //* Get the track point closest to the (x,y,z)

  float x0 = X();
  float y0 = Y();
  float k = Kappa(Bz);
  float ex = CosPhi();
  float ey = SinPhi();
  float dx = x - x0;
  float dy = y - y0;
  float ax = dx * k + ey;
  float ay = dy * k - ex;
  float a = sqrt(ax * ax + ay * ay);
  xp = x0 + (dx - ey * ((dx * dx + dy * dy) * k - 2 * (-dx * ey + dy * ex)) / (a + 1)) / a;
  yp = y0 + (dy + ex * ((dx * dx + dy * dy) * k - 2 * (-dx * ey + dy * ex)) / (a + 1)) / a;
  float s = S(x, y, Bz);
  zp = Z() + DzDs() * s;
  if (CAMath::Abs(k) > 1.e-2) {
    float dZ = CAMath::Abs(DzDs() * CAMath::TwoPi() / k);
    if (dZ > .1) {
      zp += CAMath::Nint((z - zp) / dZ) * dZ;
    }
  }
}

bool PndFTSCATrackParam::Rotate(float alpha, float maxSinPhi)
{
  //* Rotate the coordinate system in XY on the angle alpha

  const float cA = CAMath::Cos(alpha);
  const float sA = CAMath::Sin(alpha);
  const float x = X(), y = Y(), sP = SinPhi(), cP = CosPhi();
  const float cosPhi = cP * cA + sP * sA;
  const float sinPhi = -cP * sA + sP * cA;

  if (CAMath::Abs(sinPhi) > maxSinPhi || CAMath::Abs(cosPhi) < 1.e-2 || CAMath::Abs(cP) < 1.e-2)
    return 0;

  if (fabs(alpha) > 3.1415 * 0.25)
    return 0; // allow turn by 45 degree only

  const float j0 = cP / cosPhi;
  const float j2 = cosPhi / cP;

  fX = x * cA + y * sA;
  fP[0] = -x * sA + y * cA;
  fSignCosPhi = CAMath::Abs(cosPhi) / cosPhi;
  fP[2] = sinPhi;

  //  float J[5][5] = { { j0, 0, 0,  0,  0 }, // Y
  //                    {  0, 1, 0,  0,  0 }, // Z
  //                    {  0, 0, j2, 0,  0 }, // SinPhi
  //                    {  0, 0, 0,  1,  0 }, // DzDs
  //                    {  0, 0, 0,  0,  1 } }; // Kappa
  fC[0] *= j0 * j0;
  fC[1] *= j0;
  fC[3] *= j0;
  fC[6] *= j0;
  fC[10] *= j0;

  fC[3] *= j2;
  fC[4] *= j2;
  fC[5] *= j2 * j2;
  fC[8] *= j2;
  fC[12] *= j2;

  fAlpha += alpha;

  return 1;
}

bool PndFTSCATrackParam::Transport(const FTSCAHit &hit, const PndFTSCAParam &param)
{
  const bool rotated = Rotate(-fAlpha + hit.Angle());
  const bool transported = TransportToX0(hit.X0(), param.cBz());
  return rotated & transported;
}

std::istream &operator>>(std::istream &in, PndFTSCATrackParam &t)
{
  in >> t.fX;
  in >> t.fSignCosPhi;
  in >> t.fChi2;
  in >> t.fNDF;
  for (int i = 0; i < 5; i++)
    in >> t.fP[i];
  for (int i = 0; i < 15; i++)
    in >> t.fC[i];
  return in;
}

std::ostream &operator<<(std::ostream &out, const PndFTSCATrackParam &t)
{
  out << t.X() << " " << t.SignCosPhi() << " " << t.Chi2() << " " << t.NDF() << '\n';
  for (int i = 0; i < 5; i++)
    out << t.Par()[i] << " ";
  out << '\n';
  for (int i = 0; i < 15; i++)
    out << t.Cov()[i] << " ";
  out << '\n';
  return out;
}

bool PndFTSCATrackParam::GetXYZPxPyPzQ(float &x, float &y, float &z, float &px, float &py, float &pz, int &q, float cov[21]) const
{
  { // -- convert parameters

    x = X();
    y = Y();
    z = Z();

    const float pt = CAMath::Abs(1.f / QPt());
    q = -(QPt() >= 0 ? 1 : -1);

    const float cosL = DzDs();
    px = pt * CosPhi();
    py = pt * SinPhi();
    pz = pt * cosL;

    // -- convert cov matrix
    // get jacobian
    float J[6][6];
    for (int i = 0; i < 6; i++)
      for (int j = 0; j < 6; j++)
        J[i][j] = 0;
    J[0][0] = 1; // x -> x
    J[1][1] = 1; // y -> y
    J[2][2] = 1; // z -> z
    J[3][3] = pt * CosPhi() / SinPhi();
    J[3][5] = -q * pt * pt * CosPhi(); // q/pt -> px
    J[4][3] = pt;                      // sinPhi -> py
    J[4][5] = -q * pt * pt * SinPhi(); // q/pt -> py
    J[5][4] = pt;                      // dz/ds -> pz
    J[5][5] = -q * pt * pt * cosL;     // q/pt -> pz

    float CovIn[6][6]; // triangular -> symmetric matrix
    {
      CovIn[0][0] = .000001f * .00001f; // dx. From nowhere. TODO
      for (int i = 1; i < 6; i++) {
        CovIn[i][0] = 0;
        CovIn[0][i] = 0;
      }
      int k = 0;
      for (int i = 1; i < 6; i++) {
        for (int j = 1; j <= i; j++, k++) {
          CovIn[i][j] = Cov()[k];
          CovIn[j][i] = Cov()[k];
        }
      }
    }

    float CovInJ[6][6]; // CovInJ = CovIn * J^t
    for (int i = 0; i < 6; i++)
      for (int j = 0; j < 6; j++) {
        CovInJ[i][j] = 0;
        for (int k = 0; k < 6; k++) {
          CovInJ[i][j] += CovIn[i][k] * J[j][k];
        }
      }

    float CovOut[6][6]; // CovOut = J * CovInJ
    for (int i = 0; i < 6; i++)
      for (int j = 0; j < 6; j++) {
        CovOut[i][j] = 0;
        for (int k = 0; k < 6; k++) {
          CovOut[i][j] += J[i][k] * CovInJ[k][j];
        }
      }

    // symmetric matrix -> triangular
    {
      int k = 0;
      for (int i = 0; i < 6; i++) {
        for (int j = 0; j <= i; j++, k++) {
          cov[k] = CovOut[i][j];
          ASSERT(!finite(CovOut[i][j]) || CovOut[i][j] == 0 || fabs(1. - CovOut[j][i] / CovOut[i][j]) <= 0.05,
                 "CovOut[" << i << "][" << j << "] == CovOut[" << j << "][" << i << "] : " << CovOut[i][j] << " == " << CovOut[j][i]);
        }
      }
    }

    { // check cov matrix
      bool ok = true;
      int k = 0;
      for (int i = 0; i < 6; i++) {
        for (int j = 0; j <= i; j++, k++) {
          ok &= finite(cov[k]);
        }
        ok &= (cov[k - 1] > 0);
      }
      if (!ok)
        return false;
    }
  }

  // Rotate in XY plane to convert to global
  {
    const float alpha = Angle();

#if defined(PANDA_STT) || defined(PANDA_FTS)
    assert(0); // different coordinate transform for panda
#endif
    const float cA = cos(alpha);
    const float sA = sin(alpha);

    // float J[6][6] = { {  cA, sA, 0,  0,  0,  0 }, // X
    //                  { -sA, cA, 0,  0,  0,  0 }, // Y
    //                  {  0,  0,  1,  0,  0,  0 }, // Z
    //                  {  0,  0,  0,  cA, sA, 0 }, // Px
    //                  {  0,  0,  0, -sA, cA, 0 }, // Py
    //                  {  0,  0,  0,  0,  0,  1 } }; // Pz

    float J[2][2] = {{cA, sA}, {-sA, cA}};

    { // convert x, y TODO optimize
      const float xt = x, yt = y;
      x = xt * cA + yt * sA;
      y = -xt * sA + yt * cA;

      float Cov1[2][2]; // triangular -> symmetric matrix
      Cov1[0][0] = cov[0];
      Cov1[0][1] = cov[1];
      Cov1[1][0] = cov[1];
      Cov1[1][1] = cov[2];

      float Cov2[2][2]; // Cov2 = Cov1 * J^t
      for (int i = 0; i < 2; i++)
        for (int j = 0; j < 2; j++) {
          Cov2[i][j] = 0;
          for (int k = 0; k < 2; k++) {
            Cov2[i][j] += Cov1[i][k] * J[j][k];
          }
        }

      // Cov1 = J * Cov2
      for (int i = 0; i < 2; i++)
        for (int j = 0; j < 2; j++) {
          Cov1[i][j] = 0;
          for (int k = 0; k < 2; k++) {
            Cov1[i][j] += J[i][k] * Cov2[k][j];
          }
        }

      // symmetric matrix -> triangular
      cov[0] = Cov1[0][0];
      cov[1] = Cov1[0][1];
      cov[2] = Cov1[1][1];
    }

    { // convert Px, Py TODO optimize
      const float xt = px, yt = py;
      px = xt * cA + yt * sA;
      py = -xt * sA + yt * cA;

      float Cov1[2][2]; // triangular -> symmetric matrix
      Cov1[0][0] = cov[9];
      Cov1[0][1] = cov[13];
      Cov1[1][0] = cov[13];
      Cov1[1][1] = cov[14];

      float Cov2[2][2]; // Cov2 = Cov1 * J^t
      for (int i = 0; i < 2; i++)
        for (int j = 0; j < 2; j++) {
          Cov2[i][j] = 0;
          for (int k = 0; k < 2; k++) {
            Cov2[i][j] += Cov1[i][k] * J[j][k];
          }
        }

      // Cov1 = J * Cov2
      for (int i = 0; i < 2; i++)
        for (int j = 0; j < 2; j++) {
          Cov1[i][j] = 0;
          for (int k = 0; k < 2; k++) {
            Cov1[i][j] += J[i][k] * Cov2[k][j];
          }
        }

      // symmetric matrix -> triangular

      cov[9] = Cov1[0][0];
      cov[13] = Cov1[0][1];
      cov[14] = Cov1[1][1];
    }
  }

  return true;
}

#endif // PANDA_FTS
