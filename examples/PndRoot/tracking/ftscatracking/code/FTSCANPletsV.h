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

#ifndef FTSCANPletsV_H
#define FTSCANPletsV_H

#include <vector>
using std::vector;
#include "FTSCAStationArray.h"
#include "PndFTSCATrackParamVector.h"

class FTSCANPletV {

  //* Parameters of N-plet in vectors

 public:
  FTSCANPletV(int n = 0) : fParam(), fIsValid(false) { fIHit.resize(n); }

  FTSCANPletV(const FTSCANPletV &s1, const PndFTSCATrackParamVector &param, const float_m isValid) : fParam(param), fIsValid(isValid) { fNHits = s1.N() + 1; }

  FTSCANPletV(const FTSCANPletV &s1, int ista, unsigned int ihit, const PndFTSCATrackParamVector &param, const float_m isValid) : fParam(param), fIsValid(isValid)
  {
    fNHits = s1.N() + 1;
    fIHit.resize(s1.N() + 1);
    for (int i = 0; i < N() - 1; i++)
      fIHit[i] = s1.IHit(i);
    fIHit[N() - 1] = TESV(ista, ihit);
  }

  FTSCANPletV(const uint_v &ihit, const int_v &ista, const PndFTSCATrackParamVector &param, const float_m &isValid) : fParam(param), fIsValid(isValid)
  {
    fIHit.resize(1, TESV(ista, ihit));
  }
  FTSCANPletV(const FTSCANPletV &s1, const FTSCANPletV &s2, const int iV, const PndFTSCATrackParamVector &param, const float_m isValid) : fParam(param), fIsValid(isValid)
  {
    fIHit.resize(s1.N() + 1);
    assert(s1.N() == s2.N());

    for (int i = 0; i < N() - 1; i++)
      fIHit[i] = s1.IHit(i);
    fIHit[N() - 1] = s2.IHit(N() - 2)[iV];
  }

  int N() const { return fNHits; /*fIHit.size();*/ }

  const TESV &IHit(int IH) const { return fIHit[IH]; }
  const vector<TESV> &IHit() const { return fIHit; }
  const PndFTSCATrackParamVector &Param() const { return fParam; }
  PndFTSCATrackParamVector &Param() { return fParam; }
  PndFTSCATrackParamVector &ParamRef() { return fParam; } // dbg

  float_m IsValid() const { return fIsValid; }

  // check wether a is neighbour from the right to this
  float_m IsRightNeighbour(const FTSCANPletV &a, unsigned int iV) const
  {
    float_m active = fIsValid;
    assert(a.N() >= N() - 1);
    for (int i = 0; i < N() - 1; i++) {
      active &= IHit(i + 1) == a.IHit(i)[iV];
    }
    return active;
  }

  void CopyOne(int iV, FTSCANPletV a, int iVa)
  {
    for (int i = 0; i < N(); i++) {
      fIHit[i].s[iV] = a.fIHit[i].s[iVa];
      fIHit[i].e[iV] = a.fIHit[i].e[iVa];
    }
    fParam.SetTrackParamOne(iV, a.fParam, iVa);
    fIsValid = (float_m(uint_v(Vc::IndexesFromZero) == uint_v(iV)) && float_m(a.fIsValid[iVa])) || (!float_m(uint_v(Vc::IndexesFromZero) == uint_v(iV)) && fIsValid);
  }

  int_v fLastHit;
  int fNHits;
  // private:
  vector<TESV> fIHit; // index of hit on station
  PndFTSCATrackParamVector fParam;

  float_m fIsValid;
};

class FTSCANPletsV : public FTSCAStationArray<FTSCANPletV> {
 public:
  FTSCANPletsV(){};
  FTSCANPletsV(int nSta, const FTSCAHits *hits) : FTSCAStationArray<FTSCANPletV>(nSta, hits){};
};

#endif
