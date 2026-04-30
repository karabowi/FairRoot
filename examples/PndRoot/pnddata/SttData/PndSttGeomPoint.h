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

// -------------------------------------------------------------------------
// -----                  PndSttGeomPoint header file                  -----
// -----                  Created 30/03/06  by R. Castelijns           -----
// -------------------------------------------------------------------------

#ifndef PNDSTTGEOMPOINT_H
#define PNDSTTGEOMPOINT_H 1

#include "math.h"

#include "TObject.h"
#include "TCanvas.h"

// class PndSttGeomHelix;
// class PndSttGeomLine;
class PndSttGeomCircle;

class PndSttGeomPoint : public TObject {
 public:
  /** Default constructor **/
  PndSttGeomPoint();

  /** Standard constructor **/
  PndSttGeomPoint(Double_t x, Double_t y, Double_t z, Double_t dx = 0., Double_t dy = 0., Double_t dz = 0.);

  /** Copy constructor **/
  PndSttGeomPoint(PndSttGeomPoint const &other);

  /** Destructor **/
  virtual ~PndSttGeomPoint();

  Double_t GetX() const { return fX; }
  Double_t GetY() const { return fY; }
  Double_t GetZ() const { return fZ; }

  void SetX(Double_t newX) { fX = newX; }
  void SetY(Double_t newY) { fY = newY; }
  void SetZ(Double_t newZ) { fZ = newZ; }

  Double_t GetErrorX() const { return fErrorX; }
  Double_t GetErrorY() const { return fErrorY; }
  Double_t GetErrorZ() const { return fErrorZ; }

  void SetErrorX(Double_t newErrorX) { fErrorX = newErrorX; }
  void SetErrorY(Double_t newErrorY) { fErrorY = newErrorY; }
  void SetErrorZ(Double_t newErrorZ) { fErrorZ = newErrorZ; }

  Bool_t Is2D() const;
  Bool_t IsBetween(PndSttGeomPoint thisCenter, PndSttGeomPoint otherCenter) const;
  void Transform(Double_t x, Double_t y, Double_t z);

  //    Double_t DistanceTo(PndSttGeomHelix myHelix) const;
  Double_t DistanceTo(PndSttGeomPoint myPoint) const;
  //    Double_t DistanceTo(PndSttGeomLine myLine) const;

  // void Transform();  // not implemented
  void Draw(Double_t size = 0.5, Int_t color = 1) const;

  // void operator=(PndSttGeomPoint const &other);
  PndSttGeomPoint &operator=(const PndSttGeomPoint &) { return *this; };

 private:
  Double_t fX;
  Double_t fY;
  Double_t fZ;

  Double_t fErrorX;
  Double_t fErrorY;
  Double_t fErrorZ;

  void Copy(PndSttGeomPoint const &other);
  void Destroy();

  ClassDef(PndSttGeomPoint, 1);
};

#endif
