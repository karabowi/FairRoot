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

#ifndef __PndRadMapPlane_HH__
#define __PndRadMapPlane_HH__ 1
#include <TH2D.h>
#include <TROOT.h>
#include <TFormula.h>
#include <TMatrixD.h>
#include <FairRadMapPoint.h>

enum axis { Xx = 1, Yy = 2, Zz = 3 };
enum orientation { XY = 1, YX = 2, XZ = 3, ZX = 4, YZ = 5, ZY = 6 };

class PndRadMapPlane {
 public:
  PndRadMapPlane();
  PndRadMapPlane(TVector3 _corner1, TVector3 _corner2, TVector3 _corner3, double dist = 0);
  PndRadMapPlane(Double_t dist, Double_t rot, orientation plane, axis ax);
  TVector3 LineIntersection(TVector3 begline, TVector3 endline);
  void SetNormal(TVector3 n) { normal = n; };
  void SetDistance(double d) { distance = d; };
  TVector3 GetCorner(int i);
  TVector3 Corner1() { return corner1; };
  TVector3 Corner2() { return corner2; };
  TVector3 Corner3() { return corner3; };
  double Distance() { return distance; };
  TVector3 Normal() { return normal; };

 private:
  TVector3 normal;
  TVector3 corner1;
  TVector3 corner2;
  TVector3 corner3;
  TVector3 _axis;
  double distance;
  // ClassDef(PndRadMapPlane,1);
};

#endif
