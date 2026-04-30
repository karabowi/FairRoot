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

#ifndef FTSCANPlets_H
#define FTSCANPlets_H

#include <vector>
using std::pair;
using std::vector;
#include "PndFTSCATrackParam.h"
#include "FTSCAStationArray.h"
#include "FTSCANPletsV.h"

class FTSCANPlet {

  //* Parameters of N-plet

 public:
  FTSCANPlet() : fParam(), fLevel(-1), fChi2Level(0), fNeighbours() {}
  FTSCANPlet(const vector<TESV> &ih, const int iV, const PndFTSCATrackParam &param) : fParam(param), fLevel(0), fChi2Level(0), fNeighbours()
  {
    fIHit.resize(ih.size());

    for (int i = 0; i < N(); i++)
      fIHit[i] = ih[i][iV];
  }

  int N() const { return fIHit.size(); }

  const TES &IHit(int IH) const { return fIHit[IH]; }
  int ISta(int IH) const { return fIHit[IH].s; }

  const PndFTSCATrackParam &Param() const { return fParam; }
  //  PndFTSCATrackParam&       Param()       { return fParam; }

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
  bool IsRightNeighbour(float /*pick*/, const FTSCANPlet &a, float &chi2)
  { //[R.K. 9/2018] unused
    int start = (N() - a.N() < 0) ? 0 : N() - a.N();
    for (int i = start; i < N() - 1; i++)
      if (IHit(i + 1) != a.IHit(i))
        return false;
    chi2 = fabs(QMomentum() - a.QMomentum()) / sqrt(QMomentumErr2() + a.QMomentumErr2());
    /*if ( chi2 > pick )
      return false; // neighbours must have same qp*/
    chi2 *= chi2;
    return true;
  }

  static bool compare(const FTSCANPlet &i, const FTSCANPlet &j) { return (i.Level() > j.Level()) || (i.Level() == j.Level() && i.Chi2Level() < j.Chi2Level()); }

 private:
  vector<TES> fIHit; // index of hit on station
  PndFTSCATrackParam fParam;

  char fLevel;
  float fChi2Level;
  vector<pair<float, unsigned int>> fNeighbours; // index of neighbour triplets on their station
};

class FTSCANPlets : public FTSCAStationArray<FTSCANPlet> {
 public:
  FTSCANPlets(int nSta, const FTSCAHits *hits) : FTSCAStationArray<FTSCANPlet>(nSta, hits){};

  FTSCANPlets(const FTSCANPletsV &p) : FTSCAStationArray<FTSCANPlet>(p.NStations(), p.OnStation(0).HitsRef())
  {

    for (int i = 0; i < NStations(); ++i) {
      FTSCAElementsOnStation<FTSCANPlet> &tOnSta = OnStation(i);

      const FTSCAElementsOnStation<FTSCANPletV> &ts = p.OnStation(i);
      int n = 0;
      for (unsigned int iT = 0; iT < ts.size(); iT++) {
        const FTSCANPletV &t = ts[iT];
        n += t.IsValid().count();
      }

      tOnSta.resize(n);
      n = 0;
      for (unsigned int iT = 0; iT < ts.size(); iT++) {
        const FTSCANPletV &t = ts[iT];
        foreach_bit(unsigned int iV, t.IsValid()) { tOnSta[n++] = FTSCANPlet(t.IHit(), iV, PndFTSCATrackParam(t.Param(), iV)); }
      }
    }
  };
};

#endif
