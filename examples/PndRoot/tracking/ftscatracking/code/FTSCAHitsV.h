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

#ifndef FTSCAHITSV_H
#define FTSCAHITSV_H

#include <vector>
using std::vector;

// #include "FTSCAStationArray.h"
#include "PndFTSCAGBHit.h"
#include "FTSCATES.h"
#include "FTSCAHits.h"
#include "PndFTSCAParameters.h"
#include "PndFTSCATrackParamVector.h"

class FTSCAHitV {
 public:
  FTSCAHitV()
    : fIStation(-1), fId(Vc::Zero), fX0(Vc::Zero), fX1(Vc::Zero), fX2(Vc::Zero), fErr2X1(Vc::Zero), fErrX12(Vc::Zero), fErr2X2(Vc::Zero),
#ifdef DRIFT_TUBES
      fR(Vc::Zero), fErr2R(Vc::Zero), fErr2A(Vc::Zero), fBeta(Vc::Zero), fIsLeft(true),
#endif
      fAngle(Vc::Zero)
  {
  }

  FTSCAHitV(const PndFTSCAGBHit **hits, const uint_v &ids, const float_m &valid);

  FTSCAHitV(const FTSCAHit *hits, const float_m &valid);

  FTSCAHitV(const FTSCAHit *hits, const float_m &valid, bool OneHit);

  float_m IsValid() const { return fIStation >= 0; }

  char IStation() const
  {
    const float_m v = IsValid();
    assert(!v.isEmpty());
    return fIStation[v.firstOne()];
  }
  int_v IStations() const { return fIStation; } // is not needed by tracker

  uint_v Id() const { return fId; };

  float_v X0() const { return fX0; }
  float_v X1() const { return fX1; }
  float_v X2() const { return fX2; }

  float_v FStrip() const { return GetStripsValue(fFStripP); };
  float_v BStrip() const { return GetStripsValue(fBStripP); };

  float_v Err2X1() const { return fErr2X1; }
  float_v ErrX12() const { return fErrX12; }
  float_v Err2X2() const { return fErr2X2; }

#ifdef DRIFT_TUBES
  float_v XWire0() const { return fX0; }
  float_v XWire1() const { return fX1; }
  float_v XWire2() const { return fX2; }

  float_v R() const { return fR; }
  float_v RSigned() const
  {
    float_v r = fR;
    r(fIsLeft) = -r;
    return r;
  }
  float_v Err2R() const { return fErr2R; }
  float_v Err2A() const { return fErr2A; }
  float_v Beta() const { return fBeta; }
  float_m IsLeft() const { return fIsLeft; }
#endif

  float_v Angle() const { return fAngle; }

  void GetGlobalCoor(int iV, float &x, float &y, float &z) const
  {
    PndFTSCAParameters::CALocalToGlobal(float(X0()[iV]), float(X1()[iV]), float(X2()[iV]), float(Angle()[iV]), x, y, z);
  }

  void InitValuesByZeros();

 private:
  float_v GetStripsValue(FTSCAStrip *const strip[float_v::Size]) const
  {
    float_v::Memory r;
    foreach_bit(unsigned short iV, IsValid()) { r[iV] = *(strip[iV]); }
    return float_v(r);
  };

  FTSCAStrip *fFStripP[float_v::Size]; // TODO simdize
  FTSCAStrip *fBStripP[float_v::Size];

  int_v fIStation;
  uint_v fId; // index of hits in an input array

  float_v fX0, fX1, fX2;             // local coordinates. X0 is normal to module, X2 is paralel to magnetic field
  float_v fErr2X1, fErrX12, fErr2X2; // cov matrix

#ifdef DRIFT_TUBES
  float_v fR;
  float_v fErr2R;
  float_v fErr2A;
  float_v fBeta;
  float_m fIsLeft;
#endif

  float_v fAngle; // direction of hit station. Angle between normal and vertical axis. This angle defines local CS of the hit
};

template <typename T>
class FTSCAElementsOnStation;

template <>
class FTSCAElementsOnStation<FTSCAHitV> : public vector<FTSCAHitV> {
 public:
  char &IStation() { return fISta; }
  const char &IStation() const { return fISta; }

 private:
  char fISta;
};

class FTSCAHitsV { // same as in FTSCAStationArray
 public:
  typedef FTSCAHitV T;

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

  FTSCAHitsV(const FTSCAHits &hits);

  FTSCAHitsV(int nSta)
  {
    fElement.resize(nSta);
    for (int i = 0; i < nSta; ++i)
      fElement[i].IStation() = i;
  }

  int NStations() const { return fElement.size(); }

  T &operator[](TES i) { return fElement[i.s][i.e]; }
  const T &operator[](TES i) const { return fElement[i.s][i.e]; }

  void Add(const T &hit)
  {
    const int iSta = hit.IStation();
    fElement[iSta].push_back(hit);
  }

  void Clean();

 protected:
  vector<FTSCAElementsOnStation<T>> fElement; // hits on stations
};

inline FTSCAHitV::FTSCAHitV(const PndFTSCAGBHit **hits, const uint_v &ids, const float_m &valid) : fId(ids)
{
  int_v::Memory mIStation;
  mIStation = int_v(-1);

  float_v::Memory mX1, mX2, mX0;
  float_v::Memory mErr2X1, mErrX12, mErr2X2;

#ifdef DRIFT_TUBES
  float_v::Memory mR, mErr2R, mErr2A, mBeta, mIsLeft;
#endif
  float_v::Memory mAlpha;

  foreach_bit(unsigned short iV, valid)
  {
    const PndFTSCAGBHit &h = *(hits[iV]);

    mIStation[iV] = h.IRow();
    h.GetLocalX0X1X2(mX0[iV], mX1[iV], mX2[iV]);

    mErr2X1[iV] = h.Err2X1();
    mErrX12[iV] = h.ErrX12();
    mErr2X2[iV] = h.Err2X2();
#ifdef DRIFT_TUBES
    mR[iV] = h.R();
    mErr2R[iV] = h.Err2R();
    mErr2A[iV] = h.Err2A();
    mBeta[iV] = h.Beta();
    mIsLeft[iV] = h.IsLeft() ? 1.f : 0.f;
#endif
    mAlpha[iV] = h.Angle();
    fFStripP[iV] = h.FStripP();
    fBStripP[iV] = h.BStripP();
  }

  fIStation = int_v(mIStation);
  fX1 = float_v(mX1);
  fX2 = float_v(mX2);
  fX0 = float_v(mX0);
  fErr2X1 = float_v(mErr2X1);
  fErrX12 = float_v(mErrX12);
  fErr2X2 = float_v(mErr2X2);
#ifdef DRIFT_TUBES
  fR = float_v(mR);
  fErr2R = float_v(mErr2R);
  fErr2A = float_v(mErr2A);
  fBeta = float_v(mBeta);
  fIsLeft = (float_v(mIsLeft) == 1.f);
#endif
  fAngle = float_v(mAlpha);
}

inline FTSCAHitV::FTSCAHitV(const FTSCAHit *hits, const float_m &valid, bool /*OneHit*/) //[R.K. 9/2018] unused
{
  InitValuesByZeros();
  float_v::Memory mIsLeft;
  foreach_bit(unsigned short iV, valid)
  {
    const FTSCAHit &h = hits[0];

    fId[iV] = h.Id();
    fIStation[iV] = h.IStation();
    fX1[iV] = h.X1();
    fX2[iV] = h.X2();
    fX0[iV] = h.X0();

    fErr2X1[iV] = h.Err2X1();
    fErrX12[iV] = h.ErrX12();
    fErr2X2[iV] = h.Err2X2();

    fR[iV] = h.R();
    fErr2R[iV] = h.Err2R();
    fErr2A[iV] = h.Err2A();
    fBeta[iV] = h.Beta();
    mIsLeft[iV] = h.IsLeft() ? 1.f : 0.f;

    fAngle[iV] = h.Angle();
    fFStripP[iV] = h.FStripP();
    fBStripP[iV] = h.BStripP();
    // mIsUsed[iV] = h.IsUsed() ? 1.f : 0.f;
  }
  fIsLeft = (float_v(mIsLeft) == 1.f);
  // cout<<"FTSCAHitV constructor\n";
  // cout<<"fX0 "<<fX0<<" fX1 "<<fX1<<endl;
}

inline FTSCAHitV::FTSCAHitV(const FTSCAHit *hits, const float_m &valid)
{
  InitValuesByZeros();

  float_v::Memory mIsLeft;
  foreach_bit(unsigned short iV, valid)
  {
    const FTSCAHit &h = hits[iV];

    fId[iV] = h.Id();
    fIStation[iV] = h.IStation();
    fX1[iV] = h.X1();
    fX2[iV] = h.X2();
    fX0[iV] = h.X0();

    fErr2X1[iV] = h.Err2X1();
    fErrX12[iV] = h.ErrX12();
    fErr2X2[iV] = h.Err2X2();

    fR[iV] = h.R();
    fErr2R[iV] = h.Err2R();
    fErr2A[iV] = h.Err2A();
    fBeta[iV] = h.Beta();
    mIsLeft[iV] = h.IsLeft() ? 1.f : 0.f;

    fAngle[iV] = h.Angle();
    fFStripP[iV] = h.FStripP();
    fBStripP[iV] = h.BStripP();
    // mIsUsed[iV] = h.IsUsed() ? 1.f : 0.f;
  }
  fIsLeft = (float_v(mIsLeft) == 1.f);

  /*int_v::Memory mIStation;
  mIStation = int_v( -1 );

  uint_v::Memory mId;
  float_v::Memory mX1, mX2, mX0;
  float_v::Memory mErr2X1, mErrX12, mErr2X2;
#ifdef DRIFT_TUBES
  float_v::Memory mR, mErr2R, mErr2A, mBeta, mIsLeft;
#endif
  float_v::Memory mAlpha;
  //float_v::Memory mIsUsed;

  foreach_bit(unsigned short iV, valid) {
    const FTSCAHit& h = hits[iV];

    mId[iV] = h.Id();
    mIStation[iV] = h.IStation();
    mX1[iV] = h.X1();
    mX2[iV] = h.X2();
    mX0[iV] = h.X0();

    mErr2X1[iV] = h.Err2X1();
    mErrX12[iV] = h.ErrX12();
    mErr2X2[iV] = h.Err2X2();
#ifdef DRIFT_TUBES
    mR[iV] = h.R();
    mErr2R[iV] = h.Err2R();
    mErr2A[iV] = h.Err2A();
    mBeta[iV] = h.Beta();
    mIsLeft[iV] = h.IsLeft() ? 1.f : 0.f;
#endif
    mAlpha[iV] = h.Angle();
    fFStripP[iV] = h.FStripP();
    fBStripP[iV] = h.BStripP();
    //mIsUsed[iV] = h.IsUsed() ? 1.f : 0.f;
  }
  fId = uint_v(mId);
  fIStation = int_v(mIStation);
  fX1 = float_v(mX1); fX2 = float_v(mX2); fX0 = float_v(mX0);
  fErr2X1 = float_v(mErr2X1); fErrX12 = float_v(mErrX12); fErr2X2 = float_v(mErr2X2);
#ifdef DRIFT_TUBES
  fR = float_v(mR); fErr2R = float_v(mErr2R);  fErr2A = float_v(mErr2A); fBeta = float_v(mBeta); fIsLeft = ( float_v(mIsLeft) == 1.f );
#endif
  fAngle = float_v(mAlpha);
  */
  // fIsUsed = ( float_v(mIsUsed) == 1.f ); // CHECKME what to do with isUsed?
}

inline void FTSCAHitV::InitValuesByZeros()
{
  fIStation = -1;
  fId = Vc::Zero;
  fX0 = Vc::Zero;
  fX1 = Vc::Zero;
  fX2 = Vc::Zero;
  fErr2X1 = Vc::Zero;
  fErrX12 = Vc::Zero;
  fErr2X2 = Vc::Zero;
  fR = Vc::Zero;
  fErr2R = Vc::Zero;
  fErr2A = Vc::Zero;
  fBeta = Vc::Zero;
  fIsLeft = static_cast<float_m>(true);
  fAngle = Vc::Zero;
}

inline FTSCAHitsV::FTSCAHitsV(const FTSCAHits &hits)
{
  fElement.resize(hits.NStations());
  for (int i = 0; i < hits.NStations(); ++i) {
    fElement[i].IStation() = i;
    const FTSCAElementsOnStation<FTSCAHit> &hs = hits.OnStation(i);
    for (unsigned int iH = 0; iH < hs.size(); iH += float_v::Size) {
      float_m valid = static_cast<float_m>(uint_v::IndexesFromZero() < uint_v(hs.size() - iH));
      fElement[i].push_back(FTSCAHitV(&(hs[iH]), valid));
    }
  }
}

inline void FTSCAHitsV::Clean()
{ // remove used hits TODO
  // for( unsigned int i = 0; i < fElement.size(); ++i ) {
  //   FTSCAElementsOnStation<T>& hits = fElement[i];
  //   FTSCAElementsOnStation<T> tmp;
  //   tmp.IStation() = i;
  //   tmp.resize( hits.size() );
  //   for( unsigned int iH = 0; iH < hits.size(); ++iH ) {
  //     if ( hits[iH].IsUsed() ) continue;
  //     tmp.push_back( hits[iH] );
  //   }
  //   hits = tmp;
  //   tmp.clear();
  // }
}

#endif
