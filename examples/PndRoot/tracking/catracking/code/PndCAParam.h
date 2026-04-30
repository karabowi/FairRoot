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
// @(#) $Id: PndCAParam.h,v 1.4 2011/10/01 00:23:44 perev Exp $
// ************************************************************************
// This file is property of and copyright by the ALICE HLT Project        *
// ALICE Experiment at CERN, All rights reserved.                         *
// See cxx source for full Copyright notice                               *
//                                                                        *
//*************************************************************************

#ifndef PNDCAPARAM_H
#define PNDCAPARAM_H

#include "PndCADef.h"
#include "PndCAVector.h"
#include "PndCAMath.h"
//#include "PndCATrackParam.h";
class PndCATrackParam;
class PndCATrackParamVector;
#include "PndCAParameters.h"
#include "PndCAStation.h"
#include "PndCAFieldValue.h"
#include <cstdio>
#include <sstream>

/**
 * @class PndCAParam
 * parameters of the PndCATracker, including geometry information
 * and some reconstructon constants.
 *
 * The class is under construction.
 *
 */
class PndCAParam {
  friend std::istringstream &operator>>(std::istringstream &, PndCAParam &);
  //   friend std::ostream &operator<<( std::ostream &, const PndCAParam & );
 public:
  PndCAParam();
  ~PndCAParam()
  {
    if (fStations)
      delete[] fStations;
  }
  int NStations() const { return fNStations; }

  const PndCAStation &Station(short i) const { return fStations[i]; }

  float Bz() const { return fBz; }
  float cBz() const { return fBz * 0.000299792458; }

  void SetBz(float v) { fBz = v; }

  void SetBoundaries(float z, float r)
  {
    fMaxZ = z;
    fMaxR = r;
  }

  float GetBz() const { return fBz; }
  float GetBz(float x, float y, float z) const;
  float_v GetBz(float_v x, float_v y, float_v z) const;
  float GetBz(const PndCATrackParam &t) const;
  float_v GetBz(const PndCATrackParamVector &t) const;

  float GetXOverX0(short iSt) const { return fStations[iSt].xOverX0; };
  float GetXTimesRho(short iSt) const { return fStations[iSt].xTimesRho; };
  float GetR(short iSt) const { return fStations[iSt].r; };
  float_v GetXOverX0(int_v iSt, float_m mask) const
  {
    float_v r;
    r.gather(fStations, &PndCAStation::xOverX0, static_cast<uint_v>(iSt), mask);
    return r;
  };
  float_v GetXTimesRho(int_v iSt, float_m mask) const
  {
    float_v r;
    r.gather(fStations, &PndCAStation::xTimesRho, static_cast<uint_v>(iSt), mask);
    return r;
  };
  float_v GetR(int_v iSt, const float_m &mask) const
  {
    float_v r;
    r.gather(fStations, &PndCAStation::r, static_cast<uint_v>(iSt), mask);
    return r;
  }

  float MinZ() const { return fMinZ; }
  float MaxZ() const { return fMaxZ; }
  float MinR() const { return fMinR; }
  float MaxR() const { return fMaxR; }

  const PndCAFieldValue &VtxFieldValue() const { return fVtxFieldValue; }

 protected:
  int fNStations;
  PndCAStation *fStations;
  float fBz;

  PndCAFieldValue fVtxFieldValue; // field at the vertex position.

  float fMinZ, fMaxZ, fMinR, fMaxR;
};

std::istringstream &operator>>(std::istringstream &, PndCAParam &);
// std::ostream &operator<<( std::ostream &, const PndCAParam & );

#endif
