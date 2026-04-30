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

//-*- Mode: C++ -*-
// $Id: PndFTSCAGBHit.h,v 1.2 2016/11/21 14:22:47 mpugach Exp $
// ************************************************************************
// This file is property of and copyright by the ALICE HLT Project        *
// ALICE Experiment at CERN, All rights reserved.                         *
// See cxx source for full Copyright notice                               *
//                                                                        *
//*************************************************************************

#ifndef PNDFTSCAGBHIT_H
#define PNDFTSCAGBHIT_H

//#include "PndFTSCADef.h"
//#include "PndFTSCAMath.h"
#include "FTSCAStrip.h"
#include "TMatrix.h"

#include <iostream>
using std::istream;
using std::ostream;

/**
 * @class PndFTSCAGBHit
 *
 * The PndFTSCAGBHit class is the internal representation
 * of the FTS clusters for the PndFTSCAGBTracker algorithm.
 *
 */
class PndFTSCAGBHit {
 public:
  PndFTSCAGBHit()
    : fX(0), fY(0), fZ(0), fErr2X0(0), fErr2X1(0), fErrX12(0), fErr2X2(0), fIRow(0), fID(0), fPhi(0)
#if defined(DRIFT_TUBES)
      ,
      fR(0), fErr2R(0), fIsLeft(false), fIsPileuped(false), fXW(0), fYW(0), fZW(0), fTubeR(0), fHalfLength(0), fEX(0), fEY(0), fEZ(0)
#endif
  {
  }

  float X() const { return fX; }
  float Y() const { return fY; }
  float Z() const { return fZ; }

  float Err2X() const { return fErr2X0; }
  float Err2Y() const { return fErr2X1; }
  float Err2Z() const { return fErr2X2; }

  void GetLocalX0X1X2(float &x0, float &x1, float &x2) const;

  float Err2X0() const { return fErr2X0; }
  float Err2X1() const { return fErr2X1; }
  float ErrX12() const { return fErrX12; }
  float Err2X2() const { return fErr2X2; }

  int IRow() const { return fIRow; }
  int ID() const { return fID; }

#if defined(DRIFT_TUBES)
  float R() const { return fR; }
  float Err2R() const { return fErr2R; }
  float Err2A() const { return (fHalfLength / 1.5) * (fHalfLength / 1.5); }
  float Beta() const { return -0.5 * atan(2 * ErrX12() / (Err2X2() - Err2X1())); }
  float DistanceFromWireToPoint(float x, float y, float z) const
  {
    // distance from line to point is |dr x e| / |e|
    const float dx = x - fXW;
    const float dy = y - fYW;
    const float dz = z - fZW;
    const float productX = dy * fEZ - dz * fEY;
    const float productY = dz * fEX - dx * fEZ;
    const float productZ = dx * fEY - dy * fEX;
    return sqrt(productX * productX + productY * productY + productZ * productZ) / sqrt(fEX * fEX + fEY * fEY + fEZ * fEZ);
  }

  bool IsLeft() const { return fIsLeft; }
  bool IsPileduped() const { return fIsPileuped; }

  float XW() const { return fXW; }
  float YW() const { return fYW; }
  float ZW() const { return fZW; }

  float C(int i1, int i2) const { return fC[i1][i2]; }
#endif

  FTSCAStrip *BStripP() const { return fBStripP; }
  FTSCAStrip *FStripP() const { return fFStripP; }

  void SetX(float v) { fX = v; }
  void SetY(float v) { fY = v; }
  void SetZ(float v) { fZ = v; }
  void SetErr2X(float v) { fErr2X0 = v; }
  void SetErr2Y(float v) { fErr2X1 = v; }
  void SetErr2Z(float v) { fErr2X2 = v; }
  void SetErrYZ(float v) { fErrX12 = v; }
  void SetIRow(int v) { fIRow = v; }
  void SetID(int v) { fID = v; }

  void SetErr2X0(float v) { fErr2X0 = v; }
  void SetErr2X1(float v) { fErr2X1 = v; }
  void SetErrX12(float v) { fErrX12 = v; }
  void SetErr2X2(float v) { fErr2X2 = v; }

  void SetFStripP(FTSCAStrip *s) { fFStripP = s; }
  void SetBStripP(FTSCAStrip *s) { fBStripP = s; }

  void SetAngle(float v) { fPhi = v; }
  float Angle() const { return fPhi; }

  void SetC(const TMatrixT<Double_t> c)
  {
    for (int i = 0; i < 3; i++)
      for (int j = 0; j < 3; j++)
        fC[i][j] = c[i][j];
  }
#if defined(DRIFT_TUBES)
  void SetTubeR(float v) { fTubeR = v; }
  void SetTubeHalfLength(float v) { fHalfLength = v; }

  void SetR(float v) { fR = v; }
  void SetErr2R(float v) { fErr2R = v; }
  void SetIsLeft(bool v) { fIsLeft = v; }
  void SetIsPileuped(bool b) { fIsPileuped = b; }

  void SetXW(float v) { fXW = v; }
  void SetYW(float v) { fYW = v; }
  void SetZW(float v) { fZW = v; }

  void SetEX(float v) { fEX = v; }
  void SetEY(float v) { fEY = v; }
  void SetEZ(float v) { fEZ = v; }

  void SetC(float v, int i1, int i2) { fC[i1][i2] = v; }
#endif

  static bool Compare(const PndFTSCAGBHit &a, const PndFTSCAGBHit &b);

  /// \brief Hits reordering  in accordance with the geometry and the track-finder needs:
  /// Hits are sorted by sector number at first, than by row number and at last by z-coordinate

  static bool CompareRowDown(const PndFTSCAGBHit &a, const PndFTSCAGBHit &b)
  {
    return (a.fIRow > b.fIRow);

    /// Hits are sorted by  row number
  }
  static bool ComparePRowDown(const PndFTSCAGBHit *a, const PndFTSCAGBHit *b)
  {
    return (a->fIRow > b->fIRow);
    /// Hits are sorted by  row number
  }

  void SetPndDetID(int v) { fPndDetID = v; }
  void SetPndHitID(int v) { fPndHitID = v; }
  int PndDetID() const { return fPndDetID; }
  int PndHitID() const { return fPndHitID; }

  friend ostream &operator<<(ostream &out, const PndFTSCAGBHit &a);
  friend istream &operator>>(istream &in, PndFTSCAGBHit &a);

  float point_X;
  float point_Y;
  float point_Z;
  float point_Px;
  float point_Py;
  float point_Pz;
  float point_Qp;
  int Track_ID;

 protected:
  FTSCAStrip *fFStripP, *fBStripP;

  double fX; //* X position
  double fY; //* Y position
  double fZ; //* Z position

  float fErr2X0; //* position error^2 in local c.s. (x0 is perpendicular to station)
  float fErr2X1;
  float fErrX12; // covariance
  float fErr2X2;

  double fC[3][3]; // cov matrix in global c.s.

  int fIRow; //* row number
  int fID;   //* external ID (id of AliTPCcluster)

  double fPhi;

#if defined(DRIFT_TUBES)
  double fR, fErr2R;
  bool fIsLeft;
  bool fIsPileuped;
  double fXW; //* X wire position
  double fYW; //* Y wire position
  double fZW; //* Z wire position

  double fTubeR, fHalfLength;
  double fEX, fEY, fEZ; // wire direction
#endif
  int fPndDetID;
  int fPndHitID;
};

inline bool PndFTSCAGBHit::Compare(const PndFTSCAGBHit &a, const PndFTSCAGBHit &b)

/// \brief Hits reordering  in accordance with the geometry and the track-finder needs:
/// Hits are sorted by sector number at first, than by row number and at last by z-coordinate.

{
  //* Comparison function for sorting hits
  if (a.fIRow < b.fIRow)
    return 1;
  if (a.fIRow > b.fIRow)
    return 0;
  return (a.fZ < b.fZ);
}

#endif
