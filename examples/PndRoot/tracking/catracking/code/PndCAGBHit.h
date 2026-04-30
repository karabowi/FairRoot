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
// $Id: PndCAHit.h,v 1.2 2010/09/01 10:38:27 ikulakov Exp $
// ************************************************************************
// This file is property of and copyright by the ALICE HLT Project        *
// ALICE Experiment at CERN, All rights reserved.                         *
// See cxx source for full Copyright notice                               *
//                                                                        *
//*************************************************************************

#ifndef PNDCAGBHIT_H
#define PNDCAGBHIT_H

//#include "PndCADef.h"
//#include "PndCAMath.h"
#include "TMatrix.h"
#include <iostream>
// using std::ostream;
// using std::istream;

/**
 * @class PndCAGBHit
 *
 * The PndCAGBHit class is the internal representation
 * of the TPC clusters for the PndCAGBTracker algorithm.
 *
 */
class PndCAGBHit {
 public:
  PndCAGBHit()
    : fX(0), fY(0), fZ(0), fErr2X0(0), fErr2X1(0), fErrX12(0), fErr2X2(0), fIRow(0), fID(0), fPhi(0), fR(0), fErr2R(0), fIsLeft(false), fXW(0), fYW(0), fZW(0), fTubeR(0),
      fHalfLength(0),
      // fEX( 0 ), fEY( 0 ), fEZ( 0 ),
      fPndDetID(0), fPndHitID(0), fIsUsed(0)
  {
  }

  // float X() const { return fX; }
  // float Y() const { return fY; }
  float GlobalX() const { return fX; }
  float GlobalY() const { return fY; }
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

  float R() const { return fR; }
  float Err2R() const { return fErr2R; }
  float Err2A() const { return (fHalfLength / 1.5) * (fHalfLength / 1.5); }
  // float Beta() const { return -0.5*atan(2*ErrX12()/(Err2X2() - Err2X1())); }

  bool IsLeft() const { return fIsLeft; }

  float XW() const { return fXW; }
  float YW() const { return fYW; }
  float ZW() const { return fZW; }

  float C(int i1, int i2) const { return fC[i1][i2]; }

  int PndDetID() const { return fPndDetID; }
  int PndHitID() const { return fPndHitID; }

  void SetGlobalX(float v) { fX = v; }
  void SetGlobalY(float v) { fY = v; }
  void SetZ(float v) { fZ = v; }
  void SetErr2X(float v) { fErr2X0 = v; }
  void SetErr2Y(float v) { fErr2X1 = v; }
  void SetErr2Z(float v) { fErr2X2 = v; }
  void SetIRow(int v) { fIRow = v; }
  void SetID(int v) { fID = v; }

  void SetErr2X0(float v) { fErr2X0 = v; }
  void SetErr2X1(float v) { fErr2X1 = v; }
  void SetErrX12(float v) { fErrX12 = v; }
  void SetErr2X2(float v) { fErr2X2 = v; }

  void SetAngle(float v) { fPhi = v; }
  float Angle() const { return fPhi; }

  void SetR(float v) { fR = v; }
  void SetErr2R(float v) { fErr2R = v; }
  void SetIsLeft(bool v) { fIsLeft = v; }

  void SetXW(float v) { fXW = v; }
  void SetYW(float v) { fYW = v; }
  void SetZW(float v) { fZW = v; }

  void SetTubeR(float v) { fTubeR = v; }
  void SetTubeHalfLength(float v) { fHalfLength = v; }

  void SetC(float v, int i1, int i2) { fC[i1][i2] = v; }
  void SetC(const TMatrixT<Double_t> c)
  {
    for (int i = 0; i < 3; i++)
      for (int j = 0; j < 3; j++)
        fC[i][j] = c[i][j];
  }

  void SetPndDetID(int v) { fPndDetID = v; }
  void SetPndHitID(int v) { fPndHitID = v; }

  static bool Compare(const PndCAGBHit &a, const PndCAGBHit &b);

  /// \brief Hits reordering  in accordance with the geometry and the track-finder needs:
  /// Hits are sorted by sector number at first, than by row number and at last by z-coordinate

  static bool CompareRowDown(const PndCAGBHit &a, const PndCAGBHit &b)
  {
    return (a.fIRow > b.fIRow);

    /// Hits are sorted by  row number
  }
  static bool ComparePRowDown(const PndCAGBHit *a, const PndCAGBHit *b)
  {
    return (a->fIRow > b->fIRow);
    /// Hits are sorted by  row number
  }

  bool IsUsed() const { return fIsUsed; }
  void SetAsUsed() { fIsUsed = true; }

  friend std::ostream &operator<<(std::ostream &out, const PndCAGBHit &a);
  friend std::istream &operator>>(std::istream &in, PndCAGBHit &a);

 protected:
  double fX; //* X position
  double fY; //* Y position
  double fZ; //* Z position

  float fErr2X0; //* position error^2 in local c.s. (x0 is perpendicular to station)
  float fErr2X1;
  float fErrX12; // covariance
  float fErr2X2;

  double fC[3][3]; // cov matrix

  int fIRow; //* row number
  int fID;   //* external ID (id of AliTPCcluster)

  double fPhi;

  double fR, fErr2R;
  bool fIsLeft;
  double fXW; //* X wire position
  double fYW; //* Y wire position
  double fZW; //* Z wire position

  double fTubeR, fHalfLength;
  // double fEX, fEY, fEZ; // wire direction
  int fPndDetID;
  int fPndHitID;
  bool fIsUsed;
};

inline bool PndCAGBHit::Compare(const PndCAGBHit &a, const PndCAGBHit &b)

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
