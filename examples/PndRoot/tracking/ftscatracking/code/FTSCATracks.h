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

#ifndef FTSCATRACKS_H
#define FTSCATRACKS_H

#include "FTSCAHits.h"
#include "CAFunctionality.h"

class FTSCATrack {

  //* Track's parameters

 public:
  FTSCATrack() : fChi2(0), fNDF(-5), fIHits(), fLevel(0) { fIHits.reserve(PndFTSCAParameters::MaxNStations); };

  int NHits() const { return fIHits.size(); }

  vector<TES> &IHits() { return fIHits; }
  const vector<TES> &IHits() const { return fIHits; }

  void RemoveHit(int iH)
  {
    fIHits.erase(IHits().begin() + iH);
#ifdef DRIFT_TUBES
    fNDF--;
#else
    fNDF -= 2;
#endif
  }

  void AddHit(char iS, int iH)
  {
    fIHits.push_back(TES(iS, iH));
#ifdef DRIFT_TUBES
    fNDF++;
#else
    fNDF += 2;
#endif
  }
  void AddHit(const TES &iH)
  {
    fIHits.push_back(iH);
#ifdef DRIFT_TUBES
    fNDF++;
#else
    fNDF += 2;
#endif
  }

  void AddHitsToTheBeginning(vector<TES> &hits)
  {
    fIHits.insert(IHits().begin(), hits.begin(), hits.end());
#ifdef DRIFT_TUBES
    fNDF += hits.size();
#else
    fNDF += 2 * hits.size();
#endif
  }

  float &Chi2() { return fChi2; }
  const float &Chi2() const { return fChi2; }
  char &NDF() { return fNDF; }
  const char &NDF() const { return fNDF; }
  char &Level() { return fLevel; }
  const char &Level() const { return fLevel; }

  static bool Compare(const FTSCATrack *a, const FTSCATrack *b)
  {
    // return (a->fIHits.size() > b->fIHits.size()) ||
    //        ( (a->fIHits.size() == b->fIHits.size()) &&
    //          ( (a->fIHits[0].s < b->fIHits[0].s) ||
    //            ( (a->fIHits[0].s == b->fIHits[0].s) && (a->fChi2 < b->fChi2)
    //            )
    //          )
    //        );
    return (a->fIHits.size() > b->fIHits.size()) || ((a->fIHits.size() == b->fIHits.size()) && (a->fChi2 < b->fChi2));
  }

  int NUsedHits(const FTSCAHits &hits) const
  {
    int nused = 0;
    for (unsigned int iH = 0; iH < fIHits.size(); ++iH) {
      const FTSCAHit &h = hits[fIHits[iH]];
      if (h.IsUsed())
        nused++;
    }
    return nused;
  }

  void SetHitsAsUsed(FTSCAHits &hits) const
  {
    for (unsigned int iH = 0; iH < fIHits.size(); ++iH) {
      FTSCAHit &h = hits[fIHits[iH]];
      h.SetAsUsed();
    }
  }
  // const PndFTSCATrackParam GetPar() { return fPar; }
  const PndFTSCATrackParam Fit(const FTSCAHits &hits, const FTSCATarget &target, const PndFTSCAParam &caParam, bool dir = true, bool usePar = false,
                               PndFTSCATrackParam outerParam = PndFTSCATrackParam()); // dir = true - get outer parameters, dir = false - get inner ones

  const PndFTSCATrackParam Fit2Times(const FTSCAHits &hits, const FTSCATarget &target0, const PndFTSCAParam &caParam, bool dir);

 private:
  float fChi2;
  char fNDF;
  vector<TES> fIHits; // index in FTSCATracks::fHits array

  char fLevel; // n of track segments
  // PndFTSCATrackParam fPar;
};

// typedef vector<FTSCATrack> FTSCATracks;

class FTSCATracks;

class FTSCATracks : public vector<FTSCATrack> {
 public:
  FTSCATracks(FTSCAHits *hits) : fHitsRef(hits){};

  const FTSCAHit &Hit(int iH, int iT) const { return (*fHitsRef)[(*this)[iT].IHits()[iH]]; }

  // selects tracks and marks all hits as used
  void SelectAndSaveTracks(FTSCATracks &tracks);

  /*
  static bool CompareByZ(const FTSCATrack *a, const FTSCATrack *b)
  {
    return (fHitsRef[(*a).IHits()[0]].X0() < fHitsRef[(*b).IHits()[0]].X0());
  }
  */

  FTSCAHits *HitsRef() { return fHitsRef; }
  void SortTracksByZ();

 private:
  FTSCAHits *fHitsRef;
};

inline void FTSCATracks::SortTracksByZ()
{
  // sort (tracks.begin(), tracks.end(), CompareByZ);
  FTSCATrack tempTrack;
  // cout<<"before sort\n";
  for (int i = this->size() - 1; i > -1; i--) {
    for (int j = 1; j < i + 1; j++) {
      FTSCATrack &t1 = (*this)[j - 1];
      FTSCATrack &t2 = (*this)[j];

      // float diff11 = abs( (*(this->HitsRef()))[t1.IHits()[0]].X0() - 393.995) ;
      // float diff21 = abs(  (*(this->HitsRef()))[t2.IHits()[0]].X0() - 393.995) ;

      // float diff12 = abs(  (*(this->HitsRef()))[t1.IHits()[t1.NHits()-1]].X0() - 467.84) ;
      // float diff22 = abs(  (*(this->HitsRef()))[t2.IHits()[t2.NHits()-1]].X0() - 467.84) ;
      float diff12 = abs((*(this->HitsRef()))[t1.IHits()[0]].X0() - 467.84);
      float diff22 = abs((*(this->HitsRef()))[t2.IHits()[0]].X0() - 467.84);
      // sort tracks so that first come those which fit closer into the middle-region of FTS
      if /*((diff11>diff21) &&*/ (diff12 > diff22)
      // sort tracks so that first come those which are closer to PV
      // if ( ( (*(this->HitsRef()))[t1.IHits()[0]].X0() > (*(this->HitsRef()))[t2.IHits()[0]].X0() ) )
      {
        tempTrack = (*this)[j - 1];
        (*this)[j - 1] = (*this)[j];
        (*this)[j] = tempTrack;
      }
    }
  }
  // cout<<"save tracks\n";
}

inline void FTSCATracks::SelectAndSaveTracks(FTSCATracks &tracks)
{
  vector<FTSCATrack *> vptrackcandidate; // vptrackcandidate - array of pointers to vtrackcandidate
  vptrackcandidate.resize(size());

  for (unsigned int iC = 0; iC < size(); ++iC) {
    vptrackcandidate[iC] = &((*this)[iC]);
  }

  sort(vptrackcandidate.begin(), vptrackcandidate.end(), FTSCATrack::Compare);

  for (vector<FTSCATrack *>::iterator trIt = vptrackcandidate.begin(); trIt != vptrackcandidate.end(); ++trIt) {
    FTSCATrack *tr = *trIt;

    if ((tr->NUsedHits(*fHitsRef) > 0))
      continue; // don't allow tracks have shared hits. Track will be shorter, leave it for the next iteration
                // if(tr->NHits()<10) continue;
#ifndef SAVE_ALL_CANDIDATES_DBG
    tr->SetHitsAsUsed(*fHitsRef);
#endif

    tracks.push_back(*tr);
  } // i_trackCandidate
}

inline const PndFTSCATrackParam
FTSCATrack::Fit(const FTSCAHits &hits, const FTSCATarget &target0, const PndFTSCAParam &caParam, bool dir, bool usePar, PndFTSCATrackParam outerParam)
{
  vector<TESV> iHits(NHits());
  const int NTHits = iHits.size();
  // cout<<"NTHits "<<NTHits<<endl;
  for (unsigned short ihit = 0; ihit < NTHits; ihit++) {
    // cout<<"IHits()[ihit] ista "<<int(IHits()[ihit].s)<<" number "<<IHits()[ihit].e<<endl;
    iHits[ihit] = IHits()[ihit];
  }

  PndFTSCATrackParamVector param;
  // CAFunctionality::Fit( caParam, hits, param, iHits, target0, dir );
  if (usePar) {
    /*param.SetX(outerParam.X());
    param.SetY(outerParam.Y());
    param.SetZ(outerParam.Z());
    param.SetTx(outerParam.Tx());
    param.SetTy(outerParam.Ty());
    param.SetQP(outerParam.QP());
    for (int i=0; i<15; i++)
    {
      param.SetCov(i,outerParam.Cov(i));
    }*/
    param.SetQP(outerParam.QP());
    CAFunctionality::FitUseParam(caParam, hits, param, iHits, target0, dir, usePar);
  } else {
    CAFunctionality::Fit(caParam, hits, param, iHits, target0, dir);
  }

  PndFTSCATrackParam paramS(param, 0);
  fChi2 = paramS.Chi2();
  fNDF = paramS.NDF();
  // fPar = paramS;
  return paramS;
}

inline const PndFTSCATrackParam FTSCATrack::Fit2Times(const FTSCAHits &hits, const FTSCATarget &target0, const PndFTSCAParam &caParam, bool dir)
{
  vector<TESV> iHits(NHits());
  const int NTHits = iHits.size();

  for (unsigned short ihit = 0; ihit < NTHits; ihit++) {
    iHits[ihit] = IHits()[ihit];
  }

  PndFTSCATrackParamVector param;

  CAFunctionality::Fit(caParam, hits, param, iHits, target0, dir);
  param.SetNDF(-4);
  param.SetChi2(0.f);
  CAFunctionality::Fit(caParam, hits, param, iHits, target0, !dir);

  PndFTSCATrackParam paramS(param, 0);
  fChi2 = paramS.Chi2();
  fNDF = paramS.NDF();
  return paramS;
}

#endif
