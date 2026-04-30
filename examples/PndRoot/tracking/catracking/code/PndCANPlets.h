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

#ifndef PNDCANPLETS_H
#define PNDCANPLETS_H

#include <vector>
using std::pair;
using std::vector;
#include "PndCATrackParam.h"
#include "PndCAStationArray.h"
#include "PndCANPletsV.h"

class PndCANPlet {
 public:
  PndCANPlet() : fParam(), fLevel(-1), fChi2Level(0), fNeighbours() {}
  PndCANPlet(const PndCATrackParam &param) : fParam(param), fLevel(0), fChi2Level(0), fNeighbours() {}
  PndCANPlet(const vector<PndCATESV> &ih, const int iV, const PndCATrackParam &param) : fParam(param), fLevel(0), fChi2Level(0), fNeighbours()
  {
    fIHit.resize(ih.size());

    for (int i = 0; i < N(); i++)
      fIHit[i] = ih[i][iV];
  }

  int N() const { return fIHit.size(); }

  const PndCATES &IHit(int IH) const { return fIHit[IH]; }
  int ISta(int IH) const { return fIHit[IH].s; }

  const PndCATrackParam &Param() const { return fParam; }
  //  PndCATrackParam&       Param()       { return fParam; }

  float QMomentum() const { return fParam.QMomentum(); }
  float QMomentumErr() const { return sqrt(fParam.Err2QMomentum()); } // qp err
  float QMomentumErr2() const { return fParam.Err2QMomentum(); }      // qp err^2

  char &Level() { return fLevel; };
  const char &Level() const { return fLevel; };

  float &Chi2Level() { return fChi2Level; };
  const float &Chi2Level() const { return fChi2Level; };

  const unsigned int &INeighbours(int i) const { return fNeighbours[i].second; };
  const float &Chi2Neighbours(int i) const { return fNeighbours[i].first; };
  unsigned int NNeighbours() const { return fNeighbours.size(); };
  vector<pair<float, unsigned int>> &Neighbours() { return fNeighbours; }

  // check wether a is neighbour from the right to this
  bool IsRightNeighbour(float pick, const PndCANPlet &a, float &chi2)
  {
    int start = (N() - a.N() < 0) ? 0 : N() - a.N();
    for (int i = start; i < N() - 1; i++)
      if (IHit(i + 1) != a.IHit(i))
        return false;
    chi2 = fabs(QMomentum() - a.QMomentum()) / sqrt(QMomentumErr2() + a.QMomentumErr2());
    if (chi2 > pick)
      return false; // neighbours must have same qp
    chi2 *= chi2;
    return true;
  }

  static bool compare(const PndCANPlet &i, const PndCANPlet &j) { return (i.Level() > j.Level()) || (i.Level() == j.Level() && i.Chi2Level() < j.Chi2Level()); }

  vector<PndCATES> fIHit; // index of hit on station

 private:
  PndCATrackParam fParam;

  char fLevel;
  float fChi2Level;
  vector<pair<float, unsigned int>> fNeighbours; // index of neighbour triplets on their station
};

class PndCANPlets : public PndCAStationArray<PndCANPlet> {
 public:
  PndCANPlets(int nSta, const PndCAHits *hits) : PndCAStationArray<PndCANPlet>(nSta, hits){};

  PndCANPlets(const PndCANPletsV &p);
};

#endif
