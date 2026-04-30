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
// *****************************************************************************
//                                                                             *
// @Autors: I.Kulakov; M.Pugach; M.Zyzak; I.Kisel                                        *
// @e-mail: I.Kulakov@gsi.de; M.Pugach@gsi.de; M.Zyzak@gsi.de; I.Kisel@compeng.uni-frankfurt.de *
//                                                                             *
// *****************************************************************************

#ifndef FTSCAHITS_H
#define FTSCAHITS_H

#include <vector>
using std::vector;

#include "PndFTSCADef.h"
#include "PndFTSCAMath.h"
#include "PndFTSCAGBHit.h"
#include "FTSCATES.h"
#include "FTSCAStrip.h"
#include "PndFTSCATrackParamVector.h"
#include "PndFTSCAParameters.h"

class FTSCAHit {
 public:
  FTSCAHit() {}
  FTSCAHit(const PndFTSCAGBHit &h, int id)
    : fIStation(h.IRow()), fId(id), fErr2X1(h.Err2X1()), fErrX12(h.ErrX12()), fErr2X2(h.Err2X2()),
#ifdef DRIFT_TUBES
      fR(h.R()), fErr2R(h.Err2R()), fErr2A(h.Err2A()), fBeta(h.Beta()), fIsLeft(h.IsLeft()),
#endif
      fAngle(h.Angle()), fFStripP(h.FStripP()), fBStripP(h.BStripP())
  {
    h.GetLocalX0X1X2(fX0, fX1, fX2);
  };
  char IStation() const { return fIStation; }
  void SetId(int id) { fId = id; }
  int Id() const { return fId; };

#ifdef DRIFT_TUBES
  float_v X1Corrected(const PndFTSCATrackParamVector &p) const;
#endif

  float X0() const { return fX0; }
  float X1() const { return fX1; }
  float X2() const { return fX2; }

  float FStrip() const { return *fFStripP; };
  float BStrip() const { return *fBStripP; };
  FTSCAStrip *FStripP() const { return fFStripP; };
  FTSCAStrip *BStripP() const { return fBStripP; };

  float Err2X1() const { return fErr2X1; }
  float ErrX12() const { return fErrX12; }
  float Err2X2() const { return fErr2X2; }

#ifdef DRIFT_TUBES
  float XWire0() const { return fX0; }
  float XWire1() const { return fX1; }
  float XWire2() const { return fX2; }

  float R() const { return fR; }
  float RSigned() const { return fIsLeft ? -fR : fR; }
  float Err2R() const { return fErr2R; }
  float Err2A() const { return fErr2A; }
  float Beta() const { return fBeta; }
  bool IsLeft() const { return fIsLeft; }

  void SetR(float v) { fR = v; }

#endif

  float Angle() const { return fAngle; }

  bool IsUsed() const { return fFStripP->IsUsed() || fBStripP->IsUsed(); }
  void SetAsUsed()
  {
    fFStripP->SetAsUsed();
    fBStripP->SetAsUsed();
  }

  // comparison of hits on one station, needed for std::sort, returns true if a must be before b.
  friend bool operator<(const FTSCAHit &a, const FTSCAHit &b)
  {
#ifdef DRIFT_TUBES
    if (a.fIStation >= PndFTSCAParameters::NMVDStations)
      return (a.Angle() < b.Angle()) || ((a.Angle() == b.Angle()) && (a.X1() < b.X1()));
    else
      return a.X2() / abs(a.X0()) < b.X2() / abs(b.X0()); // check why x0 < 0 is possible
#else
    return a.X2() / abs(a.X0()) < b.X2() / abs(b.X0()); // check why x0 < 0 is possible
#endif
  }

  // private:
  char fIStation; // index of the station
  int fId;        // index of hits in an input array

  float fX1, fX2, fX0;             // local coordinates. fX2 is vertical, along radial direction
  float fErr2X1, fErrX12, fErr2X2; // errors squared

#ifdef DRIFT_TUBES
  float fR;
  float fErr2R;
  float fErr2A; // error along the tube
  float fBeta;
  bool fIsLeft; // left side or right side
#endif

  float fAngle; // direction of hit station. Angle between normal and vertical axis. This angle defines local CS of the hit

  FTSCAStrip *fFStripP, *fBStripP;
};

#ifdef DRIFT_TUBES
inline float_v FTSCAHit::X1Corrected(const PndFTSCATrackParamVector &p) const
{
  const float_v xCorr = fR - fR * rsqrt(1 - p.SinPhi() * p.SinPhi());
  //  xCorr /= cos(3.f/180.f*3.141592); // currently neglect stereo angle
  return (fIsLeft) ? fX1 + xCorr : fX1 - xCorr;
}
#endif

template <typename T>
class FTSCAElementsOnStation;

template <>
class FTSCAElementsOnStation<FTSCAHit> : public vector<FTSCAHit> {
 public:
  char &IStation() { return fISta; }
  const char &IStation() const { return fISta; }

 private:
  char fISta;
};

class FTSCAHits { // same as in FTSCAStationArray
 public:
  typedef FTSCAHit T;

  const FTSCAElementsOnStation<T> &OnStationConst(char i) const
  {
    assert((unsigned char)i < fElement.size());
    return fElement[i];
  }
  FTSCAElementsOnStation<T> &OnStation(char i)
  {
    assert((unsigned char)i < fElement.size());
    return fElement[i];
  }
  const FTSCAElementsOnStation<T> &OnStation(char i) const
  {
    assert((unsigned char)i < fElement.size());
    return fElement[i];
  }
  FTSCAElementsOnStation<T> &operator[](char i)
  {
    assert((unsigned char)i < fElement.size());
    return fElement[i];
  }
  const FTSCAElementsOnStation<T> &operator[](char i) const
  {
    assert((unsigned char)i < fElement.size());
    return fElement[i];
  }

  FTSCAHits() {}
  FTSCAHits(int nSta, int nHitsPerStation = 1)
  {
    fElement.resize(nSta);
    for (int i = 0; i < nSta; ++i) {
      fElement[i].IStation() = i;
      fElement[i].reserve(nHitsPerStation);
    }
  }

  T &operator[](TES i) { return fElement[i.s][i.e]; }
  const T &operator[](TES i) const { return fElement[i.s][i.e]; }

  char NStations() const { return fElement.size(); }

  void Add(const T &hit)
  {
    const int iSta = hit.IStation();
    fElement[iSta].push_back(hit);
  }

  void Sort()
  {
    for (unsigned int i = 0; i < fElement.size(); ++i) {
      FTSCAElementsOnStation<T> &hits = fElement[i];
      std::sort(hits.begin(), hits.end());
    }
  }

  void Clean()
  { // remove used hits
    for (unsigned int i = 0; i < fElement.size(); ++i) {
      FTSCAElementsOnStation<T> &hits = fElement[i];
      FTSCAElementsOnStation<T> tmp;
      tmp.IStation() = i;
      // tmp.reserve( hits.size() );
      for (unsigned int iH = 0; iH < hits.size(); ++iH) {
        if (hits[iH].IsUsed())
          continue;
        tmp.push_back(hits[iH]);
      }
      hits.clear();
      hits = tmp;
      tmp.clear();
    }
  }

 protected:
  vector<FTSCAElementsOnStation<T>> fElement; // hits on stations
};

#endif
