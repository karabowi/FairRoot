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


//
// PndTrkTools.cxx
//
// authors: Lia Lavezzi - INFN Pavia (2012)
//

#include "PndTrkTools.h"

#include "TMath.h"

#include <iostream>

using namespace std;

PndTrkTools::PndTrkTools() {}
PndTrkTools::~PndTrkTools() {}

// given a slope, where this line is tangent
// to a given circle? There are two points
// where the line is tangent
void PndTrkTools::ComputeLinePointOfTangenceOnACircle(double m, double xc, double yc, double R, double &xi1, double &yi1, double &xi2, double &yi2)
{
  // there are two points where the line is tangent
  xi1 = xc + m * R / TMath::Sqrt(m * m + 1);
  yi1 = yc - R / TMath::Sqrt(m * m + 1);

  xi2 = xc - m * R / TMath::Sqrt(m * m + 1);
  yi2 = yc + R / TMath::Sqrt(m * m + 1);
}

// if I have a point P(x, y) which is not
// exactly on a circle(xc, yc, R) and I need
// the point ON the circle closest to P
TVector2 PndTrkTools::ComputePocaToPointOnCircle2(double x, double y, double xc, double yc, double R)
{
  // line which pass in P and C(xc, yc)
  double m = (y - yc) / (x - xc);
  // its perpendicular --> tangent in the poca I am searching
  double mperp = -1. / m;

  double xi1, yi1, xi2, yi2;
  ComputeLinePointOfTangenceOnACircle(mperp, xc, yc, R, xi1, yi1, xi2, yi2);

  // I want the nearest to P
  double xp = 999, yp = -999;

  sqrt((xi1 - x) * (xi1 - x) + (yi1 - y) * (yi1 - y)) < sqrt((xi2 - x) * (xi2 - x) + (yi2 - y) * (yi2 - y)) ? (xp = xi1, yp = yi1) : (xp = xi2, yp = yi2);
  return TVector2(xp, yp);
}

TVector3 PndTrkTools::ComputePocaToPointOnCircle3(double x, double y, double xc, double yc, double R)
{
  TVector2 vec = ComputePocaToPointOnCircle2(x, y, xc, yc, R);
  return TVector3(vec.X(), vec.Y(), 0.0);
}

// I have a circle (xc, yc, R) and a
// straight line (y = mx + q) and I
// want the point ON the circle closest to the line
TVector2 PndTrkTools::ComputePocaToLineOnCircle(double m, double q, double xc, double yc, double R)
{
  // there are two points where the line is tangent
  // to the circle
  double xi1, yi1, xi2, yi2;
  ComputeLinePointOfTangenceOnACircle(m, xc, yc, R, xi1, yi1, xi2, yi2);

  // I want the nearest to the line
  double xp = 999, yp = -999;
  fabs(yi1 - (m * xi1 + q)) < fabs(yi2 - (m * xi2 + q)) ? (xp = xi1, yp = yi1) : (xp = xi2, yp = yi2);

  return TVector2(xp, yp);
}

// I have a circle (xc, yc, R) and a
// segment of a straight line (x1, y1) - (x2, y2)
// and I want to know whether they are
// intersecting and where
Int_t PndTrkTools::ComputeSegmentCircleIntersection(TVector2 ex1, TVector2 ex2, double xc, double yc, double R, TVector2 &intersection1, TVector2 &intersection2)
{

  double xa, ya, xb, yb;
  double delta;
  //   ex1.Print();
  //   ex2.Print();
  //  cout << xc << " " << yc << " " << R << endl;

  if ((ex2.X() - ex1.X()) != 0) {
    double m = (ex2.Y() - ex1.Y()) / (ex2.X() - ex1.X());
    double p = ex2.Y() - m * ex2.X();
    ;

    delta = -(m * xc + p - yc) * (m * xc + p - yc) + R * R * (m * m + 1);
    //   cout << "delta " << delta << endl;
    if (delta < 0 || TMath::IsNaN(delta))
      return 0;
    xa = (-(m * (p - yc) - xc) + sqrt(delta)) / (m * m + 1);
    ya = m * xa + p;
    xb = (-(m * (p - yc) - xc) - sqrt(delta)) / (m * m + 1);
    yb = m * xb + p;
  } else {
    // (x - xc)**2 + (y -yc)**2 = R**2
    // y**2 - 2yc y + x**2 + xc**2 - 2xc x + yc**2 - R**2 = 0;
    // y = yc +/- sqrt(R**2 -(x - xc)**2)
    delta = R * R - (ex1.X() - xc) * (ex1.X() - xc);
    //  cout << "// to y, delta " << delta << endl;
    if (delta < 0 || TMath::IsNaN(delta))
      return 0;

    xa = ex1.X();
    ya = yc + sqrt(delta);
    xb = ex1.X();
    yb = yc - sqrt(delta);
  }

  TVector2 int1(xa, ya);
  TVector2 int2(xb, yb);

  TVector2 distance11 = int1 - ex1;
  TVector2 distance12 = int1 - ex2;

  TVector2 distance21 = int2 - ex1;
  TVector2 distance22 = int2 - ex2;

  TVector2 length = ex1 - ex2;
  Int_t found = 0;
  //   cout << distance11.Mod() << " " << distance12.Mod() << " " << length.Mod() << endl;
  //   cout << distance21.Mod() << " " << distance22.Mod() << " " << length.Mod() << endl;
  if (distance11.Mod() < length.Mod() && distance12.Mod() < length.Mod()) {
    // cout << "intersection 1" << endl;
    //    TMarker *mrka = new TMarker(xa, ya, 20);
    //    mrka->Draw("SAME");
    found++;
    intersection1 = int1;
  }

  if (distance22.Mod() < length.Mod() && distance21.Mod() < length.Mod()) {
    // cout << "intersection 2" << endl;
    //    TMarker *mrkb = new TMarker(xb, yb, 22);
    //    mrkb->Draw("SAME");
    if (found == 1)
      intersection2 = int2;
    else
      intersection1 = int2;
    found++;
  }

  // if delta = 0 we have two identical intersections
  if (delta == 0)
    found--; // CHECK
  return found;
}

// I have a circle (xc, yc, R) and a
// point on the circle.
// I want the tangent to the circle in that point
TVector2 PndTrkTools::ComputeTangentInPoint(double xc, double yc, TVector2 point)
{
  TVector2 center(xc, yc);
  TVector2 fromcentertoint = point - center;
  TVector2 tangent(fromcentertoint.Y(), -fromcentertoint.X()); // CHECK
  return tangent;
}

void PndTrkTools::ComputeTangentInPoint(double xc, double yc, TVector2 point, double &m, double &p)
{
  TVector2 tangent = ComputeTangentInPoint(xc, yc, point);
  m = TMath::Tan(tangent.Phi()); // CHECK this is [0, 2pi] by default
  p = point.Y() - m * point.X();
}

ClassImp(PndTrkTools)
