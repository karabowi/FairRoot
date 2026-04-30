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
// @Autors: I.Kulakov; M.Zyzak; I.Kisel                                        *
// @e-mail: I.Kulakov@gsi.de; M.Zyzak@gsi.de; I.Kisel@compeng.uni-frankfurt.de *
//                                                                             *
// *****************************************************************************

#ifndef PNDCAHITS_H
#define PNDCAHITS_H

#include <vector>
using std::vector;

#include "PndCADef.h"
#include "PndCAGBHit.h"
#include "PndCATES.h"
#include "PndCAParameters.h"

class PndCAHit {
 public:
  PndCAHit() {}
  PndCAHit(PndCAGBHit &h, int id)
    : fIStation(h.IRow()), fId(id), fErr2X1(h.Err2X1()), fErrX12(h.ErrX12()), fErr2X2(h.Err2X2()), fR(h.R()), fErr2R(h.Err2R()), fErr2A(h.Err2A()), fIsLeft(h.IsLeft()),
      fAngle(h.Angle()), fCAHit(&h)
  {
    h.GetLocalX0X1X2(fX0, fX1, fX2);
  };
  char IStation() const { return fIStation; }
  void SetId(int id) { fId = id; }
  int Id() const { return fId; };

  float_v X1Corrected(float_v sinPhi) const;

  float X0() const { return fX0; }
  float X1() const { return fX1; }
  float X2() const { return fX2; }

  float Err2X1() const { return fErr2X1; }
  float ErrX12() const { return fErrX12; }
  float Err2X2() const { return fErr2X2; }

  float R() const { return fR; }
  float DR() const { return fDR; }
  float U() const { return fU; }
  float Err2R() const { return fErr2R; }
  float Err2A() const { return fErr2A; }

  bool IsLeft() const { return fIsLeft; }
  int ISec() const { return fISec; }

  float Angle() const { return fAngle; }

  bool IsUsed() const { return fCAHit->IsUsed(); }
  void SetAsUsed() { fCAHit->SetAsUsed(); }

  // comparison of hits on one station, needed for std::sort, returns true if a must be before b.
  friend bool operator<(const PndCAHit &a, const PndCAHit &b)
  {
    if (a.fIStation >= PndCAParameters::NMVDStations)
      return (a.Angle() < b.Angle()) || ((a.Angle() == b.Angle()) && (a.X1() < b.X1()));
    else
      return a.X2() / abs(a.X0()) < b.X2() / abs(b.X0()); // check why x0 < 0 is possible
  }

  // private:
  char fIStation; // index of the station
  int fId;        // index of hits in an input array

  float fX1, fX2, fX0;             // local coordinates. fX2 is vertical, along radial direction
  float fErr2X1, fErrX12, fErr2X2; // errors squared

  float fR;
  float fErr2R;
  float fErr2A; // error along the tube
  float fU, fDR;
  bool fIsLeft; // left side or right side
  int fISec;

  float fAngle; // direction of hit station. Angle between normal and vertical axis. This angle defines local CS of the hit

  PndCAGBHit *fCAHit;
};

inline float_v PndCAHit::X1Corrected(float_v sinPhi) const
{
  const float_v xCorr = fR - fR * rsqrt(1 - sinPhi * sinPhi);
  //  xCorr /= cos(3.f/180.f*3.141592); // currently neglect stereo angle
  return (fIsLeft) ? fX1 + xCorr : fX1 - xCorr;
}

template <typename T>
class PndCAElementsOnStation;

template <>
class PndCAElementsOnStation<PndCAHit> : public vector<PndCAHit> {
 public:
  char &IStation() { return fISta; }
  const char &IStation() const { return fISta; }

 private:
  char fISta;
};

class PndCAHits { // same as in PndCAStationArray
 public:
  typedef PndCAHit T;

  PndCAElementsOnStation<T> &OnStation(char i)
  {
    assert((unsigned char)i < fElement.size());
    return fElement[i];
  }
  const PndCAElementsOnStation<T> &OnStation(char i) const
  {
    assert((unsigned char)i < fElement.size());
    return fElement[i];
  }
  const PndCAElementsOnStation<T> &OnStationConst(char i) const
  {
    assert((unsigned char)i < fElement.size());
    return fElement[i];
  }
  PndCAElementsOnStation<T> &operator[](char i)
  {
    assert((unsigned char)i < fElement.size());
    return fElement[i];
  }
  const PndCAElementsOnStation<T> &operator[](char i) const
  {
    assert((unsigned char)i < fElement.size());
    return fElement[i];
  }

  PndCAHits() {}
  PndCAHits(int nSta, int nHitsPerStation = 1)
  {
    fElement.resize(nSta);
    for (int i = 0; i < nSta; ++i) {
      fElement[i].IStation() = i;
      fElement[i].reserve(nHitsPerStation);
    }
  }

  T &operator[](PndCATES i) { return fElement[i.s][i.e]; }
  const T &operator[](PndCATES i) const { return fElement[i.s][i.e]; }

  char NStations() const { return fElement.size(); }

  void Add(const T &hit)
  {
    const int iSta = hit.IStation();
    fElement[iSta].push_back(hit);
  }

  void Sort()
  {
    for (unsigned int i = 0; i < fElement.size(); ++i) {
      PndCAElementsOnStation<T> &hits = fElement[i];
      std::sort(hits.begin(), hits.end());
    }
  }

  void Clean()
  { // remove used hits
    for (unsigned int i = 0; i < fElement.size(); ++i) {
      PndCAElementsOnStation<T> &hits = fElement[i];
      PndCAElementsOnStation<T> tmp;
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
  vector<PndCAElementsOnStation<T>> fElement; // hits on stations
};

#endif
