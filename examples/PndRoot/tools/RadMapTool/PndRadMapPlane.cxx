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



#include "PndRadMapPlane.h"
#include <TMath.h>
#include <TFile.h>

#include <iostream>
#include <iomanip>
#include <utility>

//#include "G4UnitsTable.hh"

#define verbose true

// ClassImp(PndRadMapPlane);

PndRadMapPlane::PndRadMapPlane()
{
  SetNormal(TVector3(0, 0, 0));
  SetDistance(0);
  corner1 = TVector3(0, 0, 0);
  corner2 = TVector3(0, 0, 0);
  corner3 = TVector3(0, 0, 0);
}

PndRadMapPlane::PndRadMapPlane(TVector3 _corner1, TVector3 _corner2, TVector3 _corner3, double dist)
{
  corner1 = _corner1;
  corner2 = _corner2;
  corner3 = _corner3;
  TVector3 dpv1 = corner2 - corner1;
  TVector3 dpv2 = corner3 - corner1;
  TVector3 n = (dpv1.Cross(dpv2)).Unit();
  SetNormal(n);
  SetDistance(dist);
}

PndRadMapPlane::PndRadMapPlane(Double_t dist, Double_t rot, orientation plane, axis ax)
{

  // TVector3 corner1, corner2, corner3, axis;
  switch (plane) {
  case XY:
  case YX:
    corner1.SetXYZ(1, 1, dist);
    corner2.SetXYZ(-1, 1, dist);
    corner3.SetXYZ(-1, -1, dist);
    break;

  case XZ:
  case ZX:
    corner1.SetXYZ(1, dist, 1);
    corner2.SetXYZ(-1, dist, 1);
    corner3.SetXYZ(-1, dist, -1);
    break;

  case YZ:
  case ZY:
    corner1.SetXYZ(dist, 1, 1);
    corner2.SetXYZ(dist, -1, 1);
    corner3.SetXYZ(dist, -1, -1);
    break;
  };

  switch (ax) {
  case Xx: _axis.SetXYZ(1, 0, 0); break;
  case Yy: _axis.SetXYZ(0, 1, 0); break;
  case Zz: _axis.SetXYZ(0, 0, 1); break;
  };

  corner1.Rotate(rot, _axis);
  corner2.Rotate(rot, _axis);
  corner3.Rotate(rot, _axis);

  TVector3 dpv1 = corner2 - corner1;
  TVector3 dpv2 = corner3 - corner1;
  TVector3 n = (dpv1.Cross(dpv2)).Unit();
  SetNormal(n);
}

TVector3 PndRadMapPlane::GetCorner(int i)
{
  switch (i) {
  case 1: return Corner1();
  case 2: return Corner2();
  case 3: return Corner3();
  default: return TVector3(0, 0, 0);
  }
}

TVector3 PndRadMapPlane::LineIntersection(TVector3 begline, TVector3 endline)
{
  // plane: (p-corner1)*n = 0
  //    line: p=t*(end-beg)+beg
  //    (t*(end-beg) + beg - corner1)*n=0
  //    t*(end-beg)*n+(beg-corner1)*n=0
  //    n*(corner1-beg)/(n*(end-beg))=t
  TVector3 diffl = endline - begline;
  float nDotBeg = normal * (corner1 - begline);
  float nDotDiff = normal * diffl;
  float tt = nDotBeg / nDotDiff;

  TVector3 ixp = begline + tt * diffl;

  return ixp;
}
