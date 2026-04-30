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

////////////////////////////////////////////////////////////
//
// PndTrkConformalTransform
//
// Class to perform the Conformal Transformation
//
// authors: Lia Lavezzi - INFN Pavia (2012)
//
////////////////////////////////////////////////////////////

#include "PndTrkConformalTransform.h"

#include "PndTrkConformalHit.h"
#include <iostream>

using namespace std;

// -----   Default constructor   -------------------------------------------
PndTrkConformalTransform::PndTrkConformalTransform() : fTrasl(0., 0.), fAngle(0.) {}

// -------------------------------------------------------------------------
PndTrkConformalTransform::PndTrkConformalTransform(double x, double y, double delta) : fTrasl(x, y), fAngle(delta) {}

// -----   Destructor   ----------------------------------------------------
PndTrkConformalTransform::~PndTrkConformalTransform() {}
// -------------------------------------------------------------------------

void PndTrkConformalTransform::SetOrigin(double x, double y, double delta)
{
  SetTranslation(x, y);
  SetRotation(delta);
}

void PndTrkConformalTransform::SetTranslation(double x, double y)
{
  fTrasl.Set(x, y);
}
void PndTrkConformalTransform::SetRotation(double delta)
{
  fAngle = delta;
}

void PndTrkConformalTransform::PerformConformalTransformation(double x, double y, double rd, double &u, double &v, double &rc)
{
  Double_t xtrasl, ytrasl;
  // traslation
  xtrasl = x - fTrasl.X();
  ytrasl = y - fTrasl.Y();

  Double_t xrot, yrot;
  xrot = TMath::Cos(fAngle) * xtrasl + TMath::Sin(fAngle) * ytrasl;
  yrot = -TMath::Sin(fAngle) * xtrasl + TMath::Cos(fAngle) * ytrasl;

  // change coordinate of the center
  u = GetXConf(xrot, yrot, rd);
  v = GetYConf(xrot, yrot, rd);
  rc = GetRConf(xrot, yrot, rd);
}

void PndTrkConformalTransform::PerformRealTransformation(double u, double v, double rc, double &x, double &y, double &rd)
{
  // conf ->> real
  double xrot, yrot;
  xrot = GetXConf(u, v, rc);
  yrot = GetYConf(u, v, rc);
  rd = GetRConf(u, v, rc);

  // re-rotation
  Double_t xtrasl, ytrasl;
  xtrasl = TMath::Cos(fAngle) * xrot - TMath::Sin(fAngle) * yrot;
  ytrasl = TMath::Sin(fAngle) * xrot + TMath::Cos(fAngle) * yrot;

  // re-traslation
  x = xtrasl + fTrasl.X();
  y = ytrasl + fTrasl.Y();
}

PndTrkConformalHit PndTrkConformalTransform::GetConformalSttHit(PndTrkHit *hit)
{
  TVector3 position = hit->GetPosition();
  Double_t rd = hit->GetIsochrone();
  double u, v, rc;
  PerformConformalTransformation(position.X(), position.Y(), rd, u, v, rc);
  return PndTrkConformalHit(hit, u, v, rc);
}

PndTrkConformalHit PndTrkConformalTransform::GetConformalHit(PndTrkHit *hit)
{
  TVector3 position = hit->GetPosition();
  Double_t rd = 0.;
  double u, v, rc;
  PerformConformalTransformation(position.X(), position.Y(), rd, u, v, rc);
  return PndTrkConformalHit(hit, u, v);
}

double PndTrkConformalTransform::GetXConf(double x, double y, double rd)
{
  return x / (x * x + y * y - rd * rd);
}

double PndTrkConformalTransform::GetYConf(double x, double y, double rd)
{
  return y / (x * x + y * y - rd * rd);
}

double PndTrkConformalTransform::GetRConf(double x, double y, double rd)
{
  return rd / (x * x + y * y - rd * rd);
}

ClassImp(PndTrkConformalTransform)
