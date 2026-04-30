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

/*
 * PndLine.cpp
 *
 *  Created on: Jun 14, 2016
 *      Author: kibellus
 */

#include "PndLine.h"

PndLine::PndLine(FairHit *h1, FairHit *h2)
{
  h1->Position(fP1);
  h2->Position(fP2);
  fRating = 1;
}

PndLine::PndLine(TVector3 &base, TVector3 &dir)
{
  fP1 = base;
  fP2 = base + dir;
  fRating = 1;
}

PndLine::PndLine(TVector3 h1, Int_t layer)
{
  fP1 = h1;
  if (layer % 4 == 1) {
    fP2 = fP1 + TVector3(TMath::Sin(-5 * TMath::DegToRad()), TMath::Cos(-5 * TMath::DegToRad()), 0);
  } else if (layer % 4 == 2) {
    fP2 = fP1 + TVector3(TMath::Sin(5 * TMath::DegToRad()), TMath::Cos(5 * TMath::DegToRad()), 0);
  } else {
    fP2 = fP1 + TVector3(0, 1, 0);
  }
  fRating = 1;
}

PndLine::~PndLine()
{
  // TODO Auto-generated destructor stub
}

void PndLine::Print(Bool_t withDirVec)
{
  cout << "Line:";
  cout << "(" << fP1[0] << "," << fP1[1] << "," << fP1[2] << ")";
  if (!withDirVec)
    cout << ",(" << fP2[0] << "," << fP2[1] << "," << fP2[2] << ")";
  else {
    TVector3 d = fP2 - fP1;
    cout << "+l*(" << d[0] << "," << d[1] << "," << d[2] << ")";
  }
}

FairTrackParP PndLine::plot(Double_t zVal1, Double_t zVal2)
{
  TVector3 dir = getDir();
  Double_t lam1 = (zVal1 - fP1[2]) / dir[2];
  Double_t lam2 = (zVal2 - fP1[2]) / dir[2];
  TVector3 pointA = fP1 + lam1 * dir;
  TVector3 pointB = fP1 + lam2 * dir;
  TVector3 dir2 = pointB - pointA;
  TVector3 v(1, 1, 1);
  FairTrackParP tp(pointA, 2 * dir2.Unit(), v, v, 1, v, v, v);
  return tp;
}
/*
PndLine PndLine::approx(PndLine &l2){
  TVector3 v1 = getDir().Unit();
  TVector3 v2 = l2.getDir().Unit();
  v1*=rating;
  v2*=l2.rating;
  TVector3 newDir = v1+v2;
  TVector3 newBase = (p1*rating + l2.p1*l2.rating);
  newBase*=1/(rating + l2.rating);
  TVector3 newBase2 = p1+newDir;
  PndLine newLine(p1,newBase2);
  newLine.rating = rating + l2.rating;
  return newLine;
}*/

TVector3 PndLine::getPerpendicular(PndLine l2)
{ // returns point on this line
  PndPlane plane(l2.fP1, l2.getDir(), getDir().Cross(l2.getDir()));
  return plane.getIntersection(*this);
}

Double_t PndLine::getDistTo(TVector3 p)
{
  TVector3 p3 = (p - fP1).Cross(getDir());
  Double_t dist = p3.Mag() / getDir().Mag();
  return dist;
}
