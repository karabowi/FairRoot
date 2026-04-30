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

// $Id: PndFTSCAGBHit.cxx,v 1.8 2016/12/11 11:54:24 mpugach Exp $
//***************************************************************************
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
//***************************************************************************

#include "PndFTSCAGBHit.h"
#include "PndFTSCAMath.h"
#include "PndFTSCAParameters.h"
#include "PndFTSCADef.h"

void PndFTSCAGBHit::GetLocalX0X1X2(float &x0, float &x1, float &x2) const
{
  double x0_, x1_, x2_;
  PndFTSCAParameters::GlobalToCALocal(fX, fY, fZ, fPhi, x0_, x1_, x2_);
  x0 = x0_;
  x1 = x1_;
  x2 = x2_;
}

ostream &operator<<(ostream &out, const PndFTSCAGBHit &a)
{
#ifdef STAR_HFT
  out << a.fX << " " << a.fY << " " << a.fZ << std::endl
      << a.fErr2X1 << " " << 0 << " " << a.fErr2X2 << std::endl
#elif defined(PANDA_STT) || defined(PANDA_FTS)
  out << a.fX << " " << a.fY << " " << a.fZ << " " << a.fR << std::endl;
  out << a.fC[0][0] << " " << a.fC[0][1] << " " << a.fC[0][2] << std::endl; // in global c.s.!
  out << a.fC[1][0] << " " << a.fC[1][1] << " " << a.fC[1][2] << std::endl;
  out << a.fC[2][0] << " " << a.fC[2][1] << " " << a.fC[2][2] << std::endl;
  out << a.fErr2R << std::endl;
  out << a.fTubeR << " " << a.fHalfLength << std::endl;
  out << a.fEX << " " << a.fEY << " " << a.fEZ << std::endl
#else
  out << a.fX << " " << a.fY << " " << a.fZ << std::endl
      << a.fErr2X1 << " " << a.fErrX12 << " " << a.fErr2X2 << std::endl
#endif
      << a.fIRow << " " << a.fID << " " << -a.fPhi << std::endl;
  return out;
}

istream &operator>>(istream &in, PndFTSCAGBHit &a)
{
  double tmp;
  UNUSED_PARAM1(tmp);
#ifdef STAR_HFT
  in >> a.fX >> a.fY >> a.fZ >> a.fErr2X1 >> tmp >> a.fErr2X2 >> a.fIRow >> a.fID >> a.fPhi;
#elif defined(PANDA_STT) || defined(PANDA_FTS)
  in >> a.fX >> a.fY >> a.fZ >> a.fR >> a.fC[0][0] >> a.fC[0][1] >> a.fC[0][2] // in global c.s.!
    >> a.fC[1][0] >> a.fC[1][1] >> a.fC[1][2] >> a.fC[2][0] >> a.fC[2][1] >> a.fC[2][2] >> a.fErr2R >> a.fTubeR >> a.fHalfLength >> a.fEX >> a.fEY >> a.fEZ >> a.fIRow >> a.fID >>
    a.fPhi;
  a.fPhi = -a.fPhi; // TODO: fix input files
  a.fXW = a.fX;
  a.fYW = a.fY;
  a.fZW = a.fZ;

  // cout << " T0 " << endl;
  // cout << a.fC[0][0] << " " << a.fC[0][1] << " " << a.fC[0][2] << endl;
  // cout << a.fC[1][0] << " " << a.fC[1][1] << " " << a.fC[1][2] << endl;
  // cout << a.fC[2][0] << " " << a.fC[2][1] << " " << a.fC[2][2] << endl;

#if defined(PANDA_STT)

  const double s = sin(a.fPhi);
  const double c = cos(a.fPhi);
  // double T[3][3];
  // T[0][0] = -s;   T[0][1] = c;   T[0][2] = 0;
  // T[1][0] =  c;   T[1][1] = s;   T[1][2] = 0;
  // T[2][0] =  0;   T[2][1] = 0;   T[2][2] = -1;

  // double TC[3][3];
  // TC[0][0] = T[0][0]*a.fC[0][0] + T[0][1]*a.fC[1][0] + T[0][2]*a.fC[2][0];
  // TC[0][1] = T[0][0]*a.fC[0][1] + T[0][1]*a.fC[1][1] + T[0][2]*a.fC[2][1];
  // TC[0][2] = T[0][0]*a.fC[0][2] + T[0][1]*a.fC[1][2] + T[0][2]*a.fC[2][2];
  // TC[1][0] = T[1][0]*a.fC[0][0] + T[1][1]*a.fC[1][0] + T[1][2]*a.fC[2][0];
  // TC[1][1] = T[1][0]*a.fC[0][1] + T[1][1]*a.fC[1][1] + T[1][2]*a.fC[2][1];
  // TC[1][2] = T[1][0]*a.fC[0][2] + T[1][1]*a.fC[1][2] + T[1][2]*a.fC[2][2];
  // TC[2][0] = T[2][0]*a.fC[0][0] + T[2][1]*a.fC[1][0] + T[2][2]*a.fC[2][0];
  // TC[2][1] = T[2][0]*a.fC[0][1] + T[2][1]*a.fC[1][1] + T[2][2]*a.fC[2][1];
  // TC[2][2] = T[2][0]*a.fC[0][2] + T[2][1]*a.fC[1][2] + T[2][2]*a.fC[2][2];

  // double TCT[3][3]; // local C
  //   // TCT'
  // TCT[0][0] = TC[0][0]*T[0][0] + TC[0][1]*T[0][1] + TC[0][2]*T[0][2];
  // TCT[0][1] = TC[0][0]*T[1][0] + TC[0][1]*T[1][1] + TC[0][2]*T[1][2];
  // TCT[0][2] = TC[0][0]*T[2][0] + TC[0][1]*T[2][1] + TC[0][2]*T[2][2];
  // TCT[1][0] = TC[1][0]*T[0][0] + TC[1][1]*T[0][1] + TC[1][2]*T[0][2];
  // TCT[1][1] = TC[1][0]*T[1][0] + TC[1][1]*T[1][1] + TC[1][2]*T[1][2];
  // TCT[1][2] = TC[1][0]*T[2][0] + TC[1][1]*T[2][1] + TC[1][2]*T[2][2];
  // TCT[2][0] = TC[2][0]*T[0][0] + TC[2][1]*T[0][1] + TC[2][2]*T[0][2];
  // TCT[2][1] = TC[2][0]*T[1][0] + TC[2][1]*T[1][1] + TC[2][2]*T[1][2];
  // TCT[2][2] = TC[2][0]*T[2][0] + TC[2][1]*T[2][1] + TC[2][2]*T[2][2];

  // // cout << " T1 " << endl;
  // // cout << TCT[0][0] << " " << TCT[0][1] << " " << TCT[0][2] << endl;
  // // cout << TCT[1][0] << " " << TCT[1][1] << " " << TCT[1][2] << endl;
  // // cout << TCT[2][0] << " " << TCT[2][1] << " " << TCT[2][2] << endl;

  // double x,y,z;
  // PndFTSCAParameters::GlobalToCALocal( a.fEX, a.fEY, a.fEZ, a.fPhi, x, y, z );
  // // cout << " A " << x << " " << y << " " << z << endl;
  // double T2[3][3];
  // T2[0][0] = 1;   T2[0][1] = 0;   T2[0][2] = 0;
  // T2[1][0] = 0;   T2[1][1] = z;   T2[1][2] = -y;
  // T2[2][0] = 0;   T2[2][1] = y;   T2[2][2] = z;

  // TC[0][0] = T2[0][0]*TCT[0][0] + T2[0][1]*TCT[1][0] + T2[0][2]*TCT[2][0];
  // TC[0][1] = T2[0][0]*TCT[0][1] + T2[0][1]*TCT[1][1] + T2[0][2]*TCT[2][1];
  // TC[0][2] = T2[0][0]*TCT[0][2] + T2[0][1]*TCT[1][2] + T2[0][2]*TCT[2][2];
  // TC[1][0] = T2[1][0]*TCT[0][0] + T2[1][1]*TCT[1][0] + T2[1][2]*TCT[2][0];
  // TC[1][1] = T2[1][0]*TCT[0][1] + T2[1][1]*TCT[1][1] + T2[1][2]*TCT[2][1];
  // TC[1][2] = T2[1][0]*TCT[0][2] + T2[1][1]*TCT[1][2] + T2[1][2]*TCT[2][2];
  // TC[2][0] = T2[2][0]*TCT[0][0] + T2[2][1]*TCT[1][0] + T2[2][2]*TCT[2][0];
  // TC[2][1] = T2[2][0]*TCT[0][1] + T2[2][1]*TCT[1][1] + T2[2][2]*TCT[2][1];
  // TC[2][2] = T2[2][0]*TCT[0][2] + T2[2][1]*TCT[1][2] + T2[2][2]*TCT[2][2];

  // TCT[0][0] = TC[0][0]*T2[0][0] + TC[0][1]*T2[0][1] + TC[0][2]*T2[0][2];
  // TCT[0][1] = TC[0][0]*T2[1][0] + TC[0][1]*T2[1][1] + TC[0][2]*T2[1][2];
  // TCT[0][2] = TC[0][0]*T2[2][0] + TC[0][1]*T2[2][1] + TC[0][2]*T2[2][2];
  // TCT[1][0] = TC[1][0]*T2[0][0] + TC[1][1]*T2[0][1] + TC[1][2]*T2[0][2];
  // TCT[1][1] = TC[1][0]*T2[1][0] + TC[1][1]*T2[1][1] + TC[1][2]*T2[1][2];
  // TCT[1][2] = TC[1][0]*T2[2][0] + TC[1][1]*T2[2][1] + TC[1][2]*T2[2][2];
  // TCT[2][0] = TC[2][0]*T2[0][0] + TC[2][1]*T2[0][1] + TC[2][2]*T2[0][2];
  // TCT[2][1] = TC[2][0]*T2[1][0] + TC[2][1]*T2[1][1] + TC[2][2]*T2[1][2];
  // TCT[2][2] = TC[2][0]*T2[2][0] + TC[2][1]*T2[2][1] + TC[2][2]*T2[2][2];

  // cout << " T2 " << endl;
  // cout << TCT[0][0] << " " << TCT[0][1] << " " << TCT[0][2] << endl;
  // cout << TCT[1][0] << " " << TCT[1][1] << " " << TCT[1][2] << endl;
  // cout << TCT[2][0] << " " << TCT[2][1] << " " << TCT[2][2] << endl;

  // const float s = y;
  // const float c = z;

  // const float s = sin( a.fPhi );
  // const float c = cos( a.fPhi );
  // T =  sA  cA
  //      cA -sA
  a.fErr2X0 = s * s * a.fC[0][0] + 2 * s * c * a.fC[0][1] + c * c * a.fC[1][1];
  // a.fErrX01 = c*s*(a.fC[1][1] - a.fC[0][0]) + (c*c - s*s)*a.fC[0][1];
  a.fErr2X1 = c * c * a.fC[0][0] - 2 * s * c * a.fC[0][1] + s * s * a.fC[1][1];
  a.fErrX12 = -(c * a.fC[0][2] - s * a.fC[1][2]);

  a.fErr2X2 = a.fC[2][2];
#else // FTS
  a.fErr2X0 = a.fC[2][2];
  a.fErr2X1 = a.fC[0][0];
  a.fErrX12 = a.fC[0][1];

  a.fErr2X2 = a.fC[1][1];
#endif
#elif defined(ALICE_ITS)
  in >> a.fX >> a.fY >> a.fZ >> a.fErr2X1 >> a.fErrX12 >> a.fErr2X2 >> a.fIRow >> a.fID >> a.fPhi;

#else
  in >> a.fX >> a.fY >> a.fZ >> a.fErr2X1 >> a.fErrX12 >> a.fErr2X2 >> a.fIRow >> a.fID >> a.fPhi;
#endif
  return in;
}

// ClassImp(PndFTSCAGBHit)
