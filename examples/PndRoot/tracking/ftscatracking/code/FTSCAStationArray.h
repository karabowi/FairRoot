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

#ifndef FTSCASTATIONARRAY_H
#define FTSCASTATIONARRAY_H

#include <iostream>
using std::cout;

#include <vector>
using std::vector;

#include "FTSCAHits.h"
#include "FTSCATES.h"

template <typename T>
class FTSCAElementsOnStation : public vector<T> {

  //* Properties of elements on station

 public:
  FTSCAElementsOnStation() : fHitsRef(nullptr), fISta(-1), fFirstElementIByHit0() {}
  FTSCAElementsOnStation(const FTSCAHits *hits) : fHitsRef(hits), fFirstElementIByHit0() {}

  void SetStation(char s)
  {
    fISta = s;
    fFirstElementIByHit0.resize((*fHitsRef)[s].size() + 1, -1);
  }

  const char &IStation() const { return fISta; }

  const FTSCAHit &GetHit(int iV, int IH, int i) const { return (*fHitsRef)[(*this)[i].IHit(IH)[iV]]; }

  const FTSCAHit &GetHit(int IH, int i) const { return (*fHitsRef)[(*this)[i].IHit(IH)]; }

  int FirstElementIByHit0(int iH) const { return fFirstElementIByHit0[iH]; }
  vector<int> &FirstElementIByHit0() { return fFirstElementIByHit0; }

  const FTSCAHits *HitsRef() const { return fHitsRef; }

  FTSCAElementsOnStation<T> &operator=(const FTSCAElementsOnStation<T> &a)
  { // do not copy fHitsRef
    vector<T>::operator=(a);
    fHitsRef = (a.fHitsRef == 0) ? fHitsRef : a.fHitsRef; // copy only of set
    fISta = (a.fISta == -1) ? fISta : a.fISta;
    fFirstElementIByHit0 = a.fFirstElementIByHit0;
    return *this;
  }

  FTSCAElementsOnStation<T> operator+(const FTSCAElementsOnStation<T> &a)
  {
    if (a.size() <= 0)
      return *this;
    FTSCAElementsOnStation<T> r(fHitsRef);
    const FTSCAElementsOnStation<T> &b = *this;
    const vector<int> &bf = fFirstElementIByHit0;

    assert(a.fFirstElementIByHit0.size() == bf.size());
    r.SetStation(fISta);

    // find size
    unsigned int n = 0;
    for (unsigned int i = 0; i < a.fFirstElementIByHit0.size() - 1; i++) {
      if (a.fFirstElementIByHit0[i] >= 0)
        for (int e = a.fFirstElementIByHit0[i]; e < a.fFirstElementIByHit0[i + 1]; e++) {
          n++;
        }
      if (bf[i] >= 0)
        for (int e = bf[i]; e < bf[i + 1]; e++) {
          n++;
        }
    }
    const T sample(a[0].N());
    r.resize((n - 1) / float_v::Size + 1, sample);

    // copy by hits
    n = 0;
    for (unsigned int i = 0; i < a.fFirstElementIByHit0.size() - 1; i++) {
      r.fFirstElementIByHit0[i] = n;
      r.fFirstElementIByHit0[i + 1] = n;
      if (a.fFirstElementIByHit0[i] >= 0)
        for (int e = a.fFirstElementIByHit0[i]; e < a.fFirstElementIByHit0[i + 1]; e++) {
          const int nV = e / float_v::Size;
          const int iV = e % float_v::Size;
          if (!a[nV].IsValid()[iV])
            continue;

          const int nVt = n / float_v::Size;
          const int iVt = n % float_v::Size;
          r[nVt].CopyOne(iVt, a[nV], iV);
          n++;
        }
      if (bf[i] >= 0)
        for (int e = bf[i]; e < bf[i + 1]; e++) {
          const int nV = e / float_v::Size;
          const int iV = e % float_v::Size;
          if (!b[nV].IsValid()[iV])
            continue;

          const int nVt = n / float_v::Size;
          const int iVt = n % float_v::Size;
          r[nVt].CopyOne(iVt, b[nV], iV);
          n++;
        }
      r.fFirstElementIByHit0[i + 1] = n;
    }

    return r;
  }

  // private:
  const FTSCAHits *fHitsRef;
  char fISta;
  vector<int> fFirstElementIByHit0;
};

template <typename T>
class FTSCAStationArray {
 public:
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

  FTSCAStationArray() {}

  FTSCAStationArray(int nSta, const FTSCAHits *hits) { Renew(nSta, hits); }

  void Renew(int nSta, const FTSCAHits *hits)
  {
    fElement.clear();
    fElement.resize(nSta, FTSCAElementsOnStation<T>(hits));
    for (int i = 0; i < nSta; ++i)
      fElement[i].SetStation(i);
  }

  char NStations() const { return fElement.size(); }

  const FTSCAHits *HitsRef() const
  {
    if (fElement.size())
      return fElement[0].fHitsRef;
    else
      return nullptr;
  }

  unsigned int Size() const
  {
    unsigned int s = 0;
    for (int i = 0; i < NStations(); ++i)
      s += fElement[i].size();
    return s;
  }

  T &operator[](TES i) { return fElement[i.s][i.e]; }
  const T &operator[](TES i) const { return fElement[i.s][i.e]; }

  void Add(const T &hit)
  {
    const int iSta = hit.Station();
    fElement[iSta].push_back(hit);
  }

 protected:
  vector<FTSCAElementsOnStation<T>> fElement; // hits on stations
};

#endif
