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
// @(#) $Id: PndFTSCAParam.h,v 1.5 2016/10/11 02:33:47 mpugach Exp $
// ************************************************************************
// This file is property of and copyright by the ALICE HLT Project        *
// ALICE Experiment at CERN, All rights reserved.                         *
// See cxx source for full Copyright notice                               *
//                                                                        *
//*************************************************************************

#ifndef PNDFTSCAPARAM_H
#define PNDFTSCAPARAM_H

#include "PndFTSCADef.h"
#include "PndFTSVector.h"
#include "PndFTSCAMath.h"
#include "PndFTSCATrackParam.h"
#include "PndFTSCAParameters.h"
#include "FTSCAStation.h"
#include "CAFieldValue.h"
#include <cstdio>

/**
 * @class ALIHLTTPCCAParam
 * parameters of the PndFTSCATracker, including geometry information
 * and some reconstructon constants.
 *
 * The class is under construction.
 *
 */
class L1FieldSlice;

class PndFTSCAParam {
  friend std::istream &operator>>(std::istream &, PndFTSCAParam &);

 public:
  PndFTSCAParam();
  ~PndFTSCAParam()
  {
    if (fStations)
      delete[] fStations;
  }
  void InitMagneticField();
  void CalculateFieldSlice(L1FieldSlice &fieldSlice, const float xMax, const float yMax, const float z);
  int NStations() const { return fNStations; }

  const FTSCAStation &Station(short i) const { return fStations[i]; }
  void GetStripInfo(FTSCAStripInfoVector &stripInfo, const int_v iStation, const float_m &mask) const;

  float Bz() const { return fBz; }
  float cBz() const { return fBz * 0.000299792458f; }

  void SetBz(float v) { fBz = v; }

  //  void SetBoundaries( float z, float r ){ fMaxZ = z; fMaxR = r; }

  float GetBz() const { return fBz; }
  float GetBz(float x, float y, float z) const;
  float_v GetBz(float_v x, float_v y, float_v z) const;
  float GetBz(const PndFTSCATrackParam &t) const;
  float_v GetBz(const PndFTSCATrackParamVector &t) const;

  float GetX0(short iSt) const { return fStations[iSt].x0; };
  float_v GetX0(int_v iSt, const float_m &mask) const
  {
    float_v r;
    r.gather(fStations, &FTSCAStation::x0, static_cast<uint_v>(iSt), mask);
    return r;
  }

#ifndef PANDA_FTS
  float GetXOverX0(short iSt) const { return fStations[iSt].xOverX0; };
  float GetXTimesRho(short iSt) const { return fStations[iSt].xTimesRho; };
  float_v GetXOverX0(int_v iSt, float_m mask) const
  {
    float_v r;
    r.gather(fStations, &FTSCAStation::xOverX0, static_cast<uint_v>(iSt), mask);
    return r;
  };
  float_v GetXTimesRho(int_v iSt, float_m mask) const
  {
    float_v r;
    r.gather(fStations, &FTSCAStation::xTimesRho, static_cast<uint_v>(iSt), mask);
    return r;
  };
#else
  CAFieldValue GetFieldValue(int_v iSt, float_v x1, float_v x2, float_m mask) const
  {
    CAFieldValue r;
    foreach_bit(int iV, mask)
    {
      CAFieldValue b;
      fStations[iSt[iV]].fieldSlice.GetFieldValue(x1, x2, b, mask);
      r.x[iV] = b.x[iV];
      r.y[iV] = b.y[iV];
      r.z[iV] = b.z[iV];
    }
    return r;
  }

  CAFieldValue GetFieldValue(int_v iSt, int_v iVSt, float_v x1, float_v x2, float_m mask) const
  {
    CAFieldValue r;
    foreach_bit(int iV, mask)
    {
      CAFieldValue b;
      fStations[iSt[iV]].fieldVirtualSlice[iVSt[iV]].GetFieldValue(x1, x2, b, mask);
      r.x[iV] = b.x[iV];
      r.y[iV] = b.y[iV];
      r.z[iV] = b.z[iV];
    }
    return r;
  }

  int_v GetNVirtualStations(int_v iSt, float_m mask) const
  {
    int_v r;
    foreach_bit(int iV, mask) { r[iV] = fStations[iSt[iV]].fieldVirtualSlice.size(); }
    return r;
  }
#endif

  float MinZ() const { return fMinZ; }
  float MaxZ() const { return fMaxZ; }
#ifdef PANDA_FTS
  float MinX() const { return fMinX; }
  float MaxX() const { return fMaxX; }
  float MinY() const { return fMinY; }
  float MaxY() const { return fMaxY; }
#else
  float MinR() const { return fMinR; }
  float MaxR() const { return fMaxR; }
#endif
  void StoreToFile(FILE *f) const;
  void RestoreFromFile(FILE *f);

  const CAFieldValue &VtxFieldValue(int i = 0) const { return fVtxFieldValue[i]; }
  const float_v &ZVtxFieldValue(int i) const { return fZVtxFieldValue[i]; }

 protected:
  void CheckFieldApproximation(); // check the approximation of the field at the station position

  int fNStations;
  FTSCAStation *fStations;
  float fBz;

  CAFieldValue fVtxFieldValue[2]; // field at fZVtxFieldValue position to transport from vertex to station.
  float_v fZVtxFieldValue[2];

  float fMinZ, fMaxZ;
#ifdef PANDA_FTS
  float fMinX, fMaxX;
  float fMinY, fMaxY;
#else
  float fMinR, fMaxR;
#endif
};

std::istream &operator>>(std::istream &, PndFTSCAParam &);
std::ostream &operator<<(std::ostream &, const PndFTSCAParam &);

#endif
