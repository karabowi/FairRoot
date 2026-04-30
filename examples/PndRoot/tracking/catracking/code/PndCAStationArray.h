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

#ifndef PNDCASTATIONARRAY_H
#define PNDCASTATIONARRAY_H

#include <iostream>
using std::cout;

#include <vector>
using std::vector;

#include "PndCAHits.h"
#include "PndCATES.h"

template <typename T>
class PndCAElementsOnStation : public vector<T> {
 public:
  PndCAElementsOnStation() : fHitsRef(nullptr), fISta(-1) {}
  PndCAElementsOnStation(const PndCAHits *hits) : fHitsRef(hits) {}

  void SetStation(char s) { fISta = s; }

  const char &IStation() const { return fISta; }

  const PndCAHit &GetHit(int iV, int IH, int i) const { return (*fHitsRef)[(*this)[i].IHit(IH)[iV]]; }

  const PndCAHit &GetHit(int IH, int i) const { return (*fHitsRef)[(*this)[i].IHit(IH)]; }

  const PndCAHits *HitsRef() const { return fHitsRef; }

  PndCAElementsOnStation<T> &operator=(const PndCAElementsOnStation<T> &a)
  { // do not copy fHitsRef
    vector<T>::operator=(a);
    fHitsRef = (a.fHitsRef == 0) ? fHitsRef : a.fHitsRef; // copy only of set
    fISta = (a.fISta == -1) ? fISta : a.fISta;
    return *this;
  }

  PndCAElementsOnStation<T> operator+(const PndCAElementsOnStation<T> &a)
  {
    PndCAElementsOnStation<T> r(fHitsRef);
    r.resize(this->size() + a.size());
    int iR = 0;
    for (unsigned int i = 0; i < this->size(); ++i) {
      r[iR++] = (*this)[i];
    }
    for (unsigned int i = 0; i < a.size(); ++i) {
      r[iR++] = a[i];
    }
    r.SetStation(fISta);
    assert(0); // TODO combine fFirstElementIByHit0
    return r;
  }

  // private:
  const PndCAHits *fHitsRef;
  char fISta;
};

template <typename T>
class PndCAStationArray {
 public:
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

  PndCAStationArray() {}

  PndCAStationArray(int nSta, const PndCAHits *hits) { Renew(nSta, hits); }

  void Renew(int nSta, const PndCAHits *hits)
  {
    fElement.clear();
    fElement.resize(nSta, PndCAElementsOnStation<T>(hits));
    for (int i = 0; i < nSta; ++i)
      fElement[i].SetStation(i);
  }

  char NStations() const { return fElement.size(); }

  const PndCAHits *HitsRef() const
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

  T &operator[](PndCATES i) { return fElement[i.s][i.e]; }
  const T &operator[](PndCATES i) const { return fElement[i.s][i.e]; }

  void Add(const T &hit)
  {
    const int iSta = hit.Station();
    fElement[iSta].push_back(hit);
  }

 protected:
  vector<PndCAElementsOnStation<T>> fElement; // hits on stations
};

#endif
