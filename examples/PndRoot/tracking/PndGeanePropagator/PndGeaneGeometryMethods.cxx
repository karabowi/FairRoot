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

#include <iostream>

#include "TNamed.h"
#include "TMath.h"
#include "TMathBase.h"
#include "TVector3.h"
#include "TString.h"
#include "TGeoTorus.h"

#include "FairLogger.h"

#include "PndPropagator.h"
#include "PndGeaneGeometryMethods.h"

PndProp::PCAOutputStruct PndGeaneGeometryMethods::Track2ToLine(TVector3 X1,
                                                               TVector3 X2,
                                                               TVector3 wire1,
                                                               TVector3 wire2)
{
  PndProp::PCAOutputStruct pcastruct;
  // Double_t a1, b1, c1, d1, e1;

  // line-line distance
  TVector3 x21 = X2 - X1;
  TVector3 w21 = wire2 - wire1;

  TVector3 xw1 = X1 - wire1;
  TVector3 xw2 = X2 - wire1;

  // a1 = x21.Mag2();
  // b1 = x21.Dot(w21);
  // c1 = w21.Mag2();
  // d1 = xw1.Dot(x21);
  // e1 = xw1.Dot(w21);

  Double_t Delta1 = x21.Mag2() * w21.Mag2() - x21.Dot(w21) * x21.Dot(w21);

  if (Delta1 > 1.E-08) {
    Double_t t1 = (x21.Mag2() * xw1.Dot(w21) - x21.Dot(w21) * xw1.Dot(x21)) / Delta1;
    Double_t s1 = (x21.Dot(w21) * xw1.Dot(w21) - w21.Mag2() * xw1.Dot(x21)) / Delta1;

    pcastruct.OnTrackPCA = (X1 + x21 * s1);
    pcastruct.OnWirePCA = (wire1 + w21 * t1);
    pcastruct.TrackLength = s1 * x21.Mag();
    pcastruct.Distance = (pcastruct.OnTrackPCA - pcastruct.OnWirePCA).Mag();
    pcastruct.PCAStatusFlag = 0;
  } else {
    // lines are parallel, no solution does exist
    pcastruct.OnTrackPCA = TVector3(0., 0., 0.);
    pcastruct.OnWirePCA = TVector3(0., 0., 0.);
    pcastruct.Distance = 0.;
    pcastruct.TrackLength = 0.;
    pcastruct.PCAStatusFlag = 2;
    return pcastruct;
  }
  TVector3 Pwire = pcastruct.OnWirePCA;
  // flag when the point on the wire is outside (wire1,wire2)
  if ((((Pwire[0] < wire1[0] && Pwire[0] < wire2[0]) || (wire2[0] < Pwire[0] && wire1[0] < Pwire[0]))
       && (fabs(Pwire[0] - wire1[0]) > 1e-11 && fabs(Pwire[0] - wire2[0]) > 1e-11))
      || (((Pwire[1] < wire1[1] && Pwire[1] < wire2[1]) || (wire2[1] < Pwire[1] && wire1[1] < Pwire[1]))
        && (fabs(Pwire[1] - wire1[1]) > 1e-11 && fabs(Pwire[1] - wire2[1]) > 1e-11))
      || (((Pwire[2] < wire1[2] && Pwire[2] < wire2[2]) || (wire2[2] < Pwire[2] && wire1[2] < Pwire[2]))
        && (fabs(Pwire[2] - wire1[2]) > 1e-11 && fabs(Pwire[2] - wire2[2]) > 1e-11))) {
      pcastruct.PCAStatusFlag = 1;
  }
  return pcastruct;
}

PndProp::PCAOutputStruct PndGeaneGeometryMethods::Track2ToPoint(TVector3 X1,
                                                                TVector3 X2,
                                                                TVector3 point)
{
  PndProp::PCAOutputStruct pcastruct;

  // point - x-line distance
  Double_t dist = (X2 - X1).Mag();
  if (fabs(dist) < 1.E-8) {
    pcastruct.PCAStatusFlag = 1;
    return pcastruct;
  }

  TVector3 u21 = (X2 - X1).Unit();

  pcastruct.Distance = ((point - X1).Cross(u21)).Mag();
  Double_t t1 = 1. / dist * (point - X1).Dot(u21);
  pcastruct.OnTrackPCA = (X2 - X1) * t1 + X1;
  pcastruct.TrackLength = (X2 - X1).Mag() * t1;
  pcastruct.PCAStatusFlag = 0;

  return pcastruct;
}

PndProp::PCAOutputStruct PndGeaneGeometryMethods::Track3ToLine(TVector3 X1,
                                                               TVector3 X2,
                                                               TVector3 X3,
                                                               TVector3 wire1,
                                                               TVector3 wire2)
{
  PndProp::PCAOutputStruct pcastruct;
  TVector3 Pfinal, Pwire;
  Double_t Dist, Length, Radius;

  TVector3 xp1, xp2, xp3, xp32;
  TVector3 x21, x31;
  TVector3 e1, e2, e3, aperp;
  TVector3 Ppfinal, Ppwire;
  TVector3 wp1, wp2, wpt, xR, xpR;
  TVector3 xw1, xw2;
  TVector3 px;

  Double_t T[3][3], TM1[3][3];

  TVector3 N, M, D, B, Pointw;
  Double_t a0, a1, b0, b1, c0, c1, c2;
  Double_t d0, d1, d2, d3, d4, sol4[4], dmin;
  Double_t Angle;
  Double_t dx;
  Int_t it, imin;

  // go to the circle plane: matrix of director cosines
  x21 = X2 - X1;
  e1 = x21.Unit();
  T[0][0] = e1.X();
  T[0][1] = e1.Y();
  T[0][2] = e1.Z();

  x31 = X3 - X1;
  e3 = e1.Cross(x31);
  // if the points are on the same line
  if (e3.Mag() < 1e-8) {
    pcastruct.PCAStatusFlag = 1;
    return pcastruct;
  }
  e3 = e3.Unit();

  T[2][0] = e3.X();
  T[2][1] = e3.Y();
  T[2][2] = e3.Z();

  e2 = e3.Cross(e1);
  T[1][0] = e2.X();
  T[1][1] = e2.Y();
  T[1][2] = e2.Z();

  // new coordinates
  for (Int_t i = 0; i < 3; i++) {
    xp1[i] = 0.;
    xp2[i] = 0.;
    xp3[i] = 0.;
    wp1[i] = 0.;
    wp2[i] = 0.;
  }
  for (Int_t i = 0; i < 3; i++) {
    xp1[i] = 0.;
    for (Int_t j = 0; j < 3; j++) {
      TM1[i][j] = T[j][i];
      xp2[i] += T[i][j] * (X2[j] - X1[j]);
      xp3[i] += T[i][j] * (X3[j] - X1[j]);
      wp1[i] += T[i][j] * (wire1[j] - X1[j]);
      wp2[i] += T[i][j] * (wire2[j] - X1[j]);
    }
  }

  // radius and center
  xp32 = xp3 - xp2;
  xpR[0] = 0.5 * xp2[0];
  if (fabs(xp3[1]) < 1.E-8) {
    pcastruct.PCAStatusFlag = 4;
    return pcastruct;
  }
  xpR[1] = 0.5 * (xp32[0] * xp3[0] / xp3[1] + xp3[1]);
  xpR[2] = 0.;
  Radius = sqrt(pow(xpR[0] - xp1[0], 2) + pow(xpR[1] - xp1[1], 2));

  // Eberly's method
  B = wp1;
  M = wp2 - wp1;
  D = wp1 - xpR;
  N.SetXYZ(0., 0., 1.);

  a0 = M.Dot(D);
  a1 = M.Dot(M);
  b0 = M.Dot(D) - (N.Dot(M)) * (N.Dot(D));
  b1 = M.Dot(M) - (N.Dot(M)) * (N.Dot(M));
  c0 = D.Dot(D) - (N.Dot(D)) * (N.Dot(D));
  c1 = b0;
  c2 = b1;

  d0 = a0 * a0 * c0 - b0 * b0 * Radius * Radius;
  d1 = 2. * (a0 * a1 * c0 + a0 * a0 * c1 - b0 * b1 * Radius * Radius);
  d2 = a1 * a1 * c0 + 4. * a0 * a1 * c1 + a0 * a0 * c2 - b1 * b1 * Radius * Radius;
  d3 = 2. * (a1 * a1 * c1 + a0 * a1 * c2);
  d4 = a1 * a1 * c2;

  // solve the quartic equation
  for (Int_t k = 0; k < 4; k++) {
    sol4[k] = 0.;
  }
  if (fabs(d4) < 1.E-12) {
    pcastruct.PCAStatusFlag = 4;
    return pcastruct;
  }

  TGeoTorus t;
  it = t.SolveQuartic(d3 / d4, d2 / d4, d1 / d4, d0 / d4, sol4);

  if (it == 0) {
    pcastruct.PCAStatusFlag = 4;
    return pcastruct;
  }

  // select the right solution
  dmin = 1.e+08;
  imin = 0;

  for (Int_t j = 0; j < it; j++) {
    Pointw[0] = B[0] + sol4[j] * M[0];
    Pointw[1] = B[1] + sol4[j] * M[1];
    Pointw[2] = B[2] + sol4[j] * M[2];
    pcastruct = Track3ToPoint(xp1, xp2, xp3, Pointw);
    if (pcastruct.PCAStatusFlag == 2) {
      pcastruct.PCAStatusFlag = 4;
      return pcastruct;
    }
    if (pcastruct.Distance < dmin) {
      dmin = pcastruct.Distance;
      imin = j;
    }
  }

  // final solution
  Ppwire[0] = B[0] + sol4[imin] * M[0];
  Ppwire[1] = B[1] + sol4[imin] * M[1];
  Ppwire[2] = B[2] + sol4[imin] * M[2];
  pcastruct = Track3ToPoint(xp1, xp2, xp3, Ppwire);
  if (pcastruct.PCAStatusFlag == 2) {
    pcastruct.PCAStatusFlag = 4;
    return pcastruct;
  }

  // output: distance and points in the circle plane reference
  Ppfinal = pcastruct.OnTrackPCA;
  Radius = pcastruct.Radius;

  //
  // back to lab coordinates
  //
  xR[0] = 0.;
  xR[1] = 0.;
  xR[2] = 0.;
  Pfinal[0] = 0.;
  Pfinal[1] = 0.;
  Pfinal[2] = 0.;
  Pwire[0] = 0.;
  Pwire[1] = 0.;
  Pwire[2] = 0.;

  for (Int_t i = 0; i < 3; i++) {
    for (Int_t j = 0; j < 3; j++) {
      Pfinal[i] += TM1[i][j] * Ppfinal[j];
      Pwire[i] += TM1[i][j] * Ppwire[j];
      xR[i] += TM1[i][j] * xpR[j];
    }
  }
  Pfinal = Pfinal + X1;
  Pwire = Pwire + X1;
  xR = xR + X1;

  Double_t dx1 = (X1 - xR).Mag();
  Double_t dx2 = (Pfinal - xR).Mag();
  Double_t dx12 = dx1 * dx2;
  if (fabs(dx12) < 1.E-8) {
    pcastruct.PCAStatusFlag = 4;
    return pcastruct;
  }

  // now find the length
  Angle = TMath::ACos((X1 - xR).Dot(Pfinal - xR) / (dx12));
  Length = Radius * Angle;
  if ((X2 - X1).Dot(Pfinal - X1) < 0.) {
    Length = -Length;
  }

  pcastruct.PCAStatusFlag = 0;
  pcastruct.OnTrackPCA = Pfinal;
  pcastruct.OnWirePCA = Pwire;
  pcastruct.TrackLength = Length;

  // flag straight points within 20 microns
  Double_t epsi = 0;
  if (Radius > 1E-8) {
    epsi = Radius * (1. - TMath::Cos(0.5 * (X3 - X1).Mag() / Radius));
  }
  if (epsi < 0.0020) {
    pcastruct.PCAStatusFlag = 1;
  }

  // flag when the point on the wire is outside (wire1,wire2)
  if ((((Pwire[0] < wire1[0] && Pwire[0] < wire2[0]) || (wire2[0] < Pwire[0] && wire1[0] < Pwire[0]))
       && (fabs(Pwire[0] - wire1[0]) > 1e-11 && fabs(Pwire[0] - wire2[0]) > 1e-11))
      || (((Pwire[1] < wire1[1] && Pwire[1] < wire2[1]) || (wire2[1] < Pwire[1] && wire1[1] < Pwire[1]))
        && (fabs(Pwire[1] - wire1[1]) > 1e-11 && fabs(Pwire[1] - wire2[1]) > 1e-11))
      || (((Pwire[2] < wire1[2] && Pwire[2] < wire2[2]) || (wire2[2] < Pwire[2] && wire1[2] < Pwire[2]))
        && (fabs(Pwire[2] - wire1[2]) > 1e-11 && fabs(Pwire[2] - wire2[2]) > 1e-11))) {
      pcastruct.PCAStatusFlag = 2;
  }

  return pcastruct;
}

PndProp::PCAOutputStruct PndGeaneGeometryMethods::Track3ToPoint(TVector3 X1,
                                                                TVector3 X2,
                                                                TVector3 X3,
                                                                TVector3 point)
{
  PndProp::PCAOutputStruct pcastruct;
  TVector3 Pfinal;
  Double_t Radius;
  
  TVector3 xp1, xp2, xp3, xp32;
  TVector3 x21, x31;
  TVector3 e1, e2, e3;
  TVector3 Ppfinal, x32;
  TVector3 wp1, wpt, xR, xpR;
  TVector3 xw1, xw2;
  TVector3 xc1, xc2, xc3, wc1;

  Double_t m1, m3, Rt;
  Double_t T[3][3], TM1[3][3];

  Double_t Angle;

  // go to the circle plane with origin in x1 prime (xp1):
  // matrix of director cosines
  x21 = X2 - X1;

  Double_t x21mag = x21.Mag();
  if (x21mag < 1.E-8) {
    pcastruct.PCAStatusFlag = 2;
    return pcastruct;
  }

  m1 = 1. / x21mag;
  e1 = m1 * x21;
  T[0][0] = e1.X();
  T[0][1] = e1.Y();
  T[0][2] = e1.Z();

  x31 = X3 - X1;
  e3 = e1.Cross(x31);

  // if the points are on the same line
  if (e3.Mag() < 1e-8) {
    pcastruct.PCAStatusFlag = 1;
    return pcastruct;
  }

  m3 = 1. / e3.Mag();
  e3 = m3 * e3;
  T[2][0] = e3.X();
  T[2][1] = e3.Y();
  T[2][2] = e3.Z();

  e2 = e3.Cross(e1);
  T[1][0] = e2.X();
  T[1][1] = e2.Y();
  T[1][2] = e2.Z();

  // new coordinates
  for (Int_t i = 0; i < 3; i++) {
    xp1[i] = 0.;
    xp2[i] = 0.;
    xp3[i] = 0.;
    wp1[i] = 0.;
  }
  for (Int_t i = 0; i < 3; i++) {
    for (Int_t j = 0; j < 3; j++) {
      TM1[i][j] = T[j][i];
      xp1[i] += 0.;
      xp2[i] += T[i][j] * (X2[j] - X1[j]);
      xp3[i] += T[i][j] * (X3[j] - X1[j]);
      wp1[i] += T[i][j] * (point[j] - X1[j]);
    }
  }

  // radius Radius and center xpR

  xp32 = xp3 - xp2;
  xpR[0] = 0.5 * xp2[0];
  if (fabs(xp3[1]) < 1.E-8) {
    pcastruct.PCAStatusFlag = 2;
    return pcastruct;
  }
  xpR[1] = 0.5 * (xp32[0] * xp3[0] / xp3[1] + xp3[1]);
  xpR[2] = 0.;

  Radius = sqrt(pow(xpR[0] - xp1[0], 2) + pow(xpR[1] - xp1[1], 2));

  // distance and points
  wpt = wp1;
  wpt[2] = 0.;   // point projection on the circle plane

  Double_t dwp = (wpt - xpR).Mag();
  if (fabs(dwp) < 1.E-8) {
    pcastruct.PCAStatusFlag = 2;
    return pcastruct;
  }
  Rt = Radius / dwp;
  Ppfinal = (wpt - xpR) * Rt + xpR;
  pcastruct.Distance = (wp1 - Ppfinal).Mag();

  // back to lab coordinates:
  // from Ppfinal to Pfinal and from xpR to xR

  xR[0] = 0.;
  xR[1] = 0.;
  xR[2] = 0.;
  Pfinal[0] = 0.;
  Pfinal[1] = 0.;
  Pfinal[2] = 0.;
  for (Int_t i = 0; i < 3; i++) {
    for (Int_t j = 0; j < 3; j++) {
      Pfinal[i] += TM1[i][j] * Ppfinal[j];
      xR[i] += TM1[i][j] * xpR[j];
    }
  }
  Pfinal = Pfinal + X1;
  xR = xR + X1;

  // now find the length
  Double_t dx1 = (X1 - xR).Mag();
  Double_t dx2 = (Pfinal - xR).Mag();
  Double_t dx12 = dx1 * dx2;
  if (fabs(dx12) < 1.E-8) {
    pcastruct.PCAStatusFlag = 2;
    return pcastruct;
  }
  // now find the length
  Angle = TMath::ACos((X1 - xR).Dot(Pfinal - xR) / (dx12));
  pcastruct.TrackLength = Radius * Angle;

  pcastruct.OnTrackPCA = Pfinal;
  pcastruct.Radius = Radius;
  pcastruct.PCAStatusFlag = 0;

  // flag straight points within 20 microns
  Double_t epsi = 0;
  if (Radius > 1E-8) {
    epsi = Radius * (1. - TMath::Cos(0.5 * (X3 - X1).Mag() / Radius));
  }
  if (epsi < 0.0020) {
    pcastruct.PCAStatusFlag = 1;
  }

  return pcastruct;
}