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

#include "PndSttGeomPoint.h"
// #include "PndSttGeomHelix.h"
#include "TPolyLine3D.h"

void PndSttGeomPoint::Copy(PndSttGeomPoint const &other)
{
  fX = other.fX;
  fY = other.fY;
  fZ = other.fZ;

  fErrorX = other.fErrorX;
  fErrorY = other.fErrorY;
  fErrorZ = other.fErrorZ;
}

void PndSttGeomPoint::Destroy() {}

PndSttGeomPoint::PndSttGeomPoint(PndSttGeomPoint const &other)
  : TObject(other), fX(other.fX), fY(other.fY), fZ(other.fZ), fErrorX(other.fErrorX), fErrorY(other.fErrorY), fErrorZ(other.fErrorZ)
{
}

/* ???
void PndSttGeomPoint::operator=(PndSttGeomPoint const &other)
{
  return *this;
}
*/

PndSttGeomPoint::PndSttGeomPoint() : TObject(), fX(0), fY(0), fZ(0), fErrorX(0), fErrorY(0), fErrorZ(0) {}

PndSttGeomPoint::PndSttGeomPoint(Double_t x, Double_t y, Double_t z, Double_t dx, Double_t dy, Double_t dz) : fX(x), fY(y), fZ(z), fErrorX(dx), fErrorY(dy), fErrorZ(dz) {}

void PndSttGeomPoint::Transform(Double_t x, Double_t y, Double_t z)
{
  fX += x;
  fY += y;
  fZ += z;
}

PndSttGeomPoint::~PndSttGeomPoint() {}

Bool_t PndSttGeomPoint::Is2D() const
{
  Bool_t retval = kTRUE;

  if (fabs(fZ) > 0.)
    retval = kFALSE;

  return retval;
}

void PndSttGeomPoint::Draw(Double_t size, Int_t color) const
{
  Double_t xstartx[2] = {fX - size, fX + size}, ystartx[2] = {fX, fX}, zstartx[2] = {fX, fX};

  Double_t xstarty[2] = {fY, fY}, ystarty[2] = {fY - size, fY + size}, zstarty[2] = {fY, fY};

  Double_t xstartz[2] = {fZ, fZ}, ystartz[2] = {fZ, fZ}, zstartz[2] = {fZ - size, fZ + size};

  TPolyLine3D *xstart = new TPolyLine3D(2, xstartx, xstarty, xstartz), *ystart = new TPolyLine3D(2, ystartx, ystarty, ystartz),
              *zstart = new TPolyLine3D(2, zstartx, zstarty, zstartz);

  xstart->SetLineColor(color);
  xstart->Draw();
  ystart->SetLineColor(color);
  ystart->Draw();
  zstart->SetLineColor(color);
  zstart->Draw();
}

Bool_t PndSttGeomPoint::IsBetween(PndSttGeomPoint firstPoint, PndSttGeomPoint secondPoint) const
{
  Bool_t betweenX = kFALSE, betweenY = kFALSE, betweenZ = kFALSE, retval = kFALSE;

  if (((firstPoint.GetX() < fX) && (fX < secondPoint.GetX())) || ((firstPoint.GetX() > fX) && (fX > secondPoint.GetX()))) {
    betweenX = kTRUE;
  }

  if (((firstPoint.GetY() < fY) && (fY < secondPoint.GetY())) || ((firstPoint.GetY() > fY) && (fY > secondPoint.GetY()))) {
    betweenY = kTRUE;
  }

  if (((firstPoint.GetZ() < fZ) && (fZ < secondPoint.GetZ())) || ((firstPoint.GetZ() > fZ) && (fZ > secondPoint.GetZ()))) {
    betweenZ = kTRUE;
  }

  if (betweenX && betweenY && betweenZ) {
    retval = kTRUE;
  }

  return retval;
}

// Double_t PndSttGeomPoint::DistanceTo(PndSttGeomHelix myHelix) const
// {
//     return myHelix.DistanceTo(*this);
// }

Double_t PndSttGeomPoint::DistanceTo(PndSttGeomPoint myPoint) const
{
  return sqrt(((myPoint.GetX() - fX) * (myPoint.GetX() - fX)) + ((myPoint.GetY() - fY) * (myPoint.GetY() - fY)) + ((myPoint.GetZ() - fZ) * (myPoint.GetZ() - fZ)));
}

// Double_t PndSttGeomPoint::DistanceTo(PndSttGeomLine myLine) const
// {
//     return myLine.DistanceTo(*this);
// }

ClassImp(PndSttGeomPoint)
