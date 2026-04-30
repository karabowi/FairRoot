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

#ifndef PNDCATRACKS_H
#define PNDCATRACKS_H

#include "PndCAHits.h"
#include "PndCAFunctionality.h"

class PndCATrack {
 public:
  PndCATrack() : fChi2(0), fNDF(-5), fIHits(), fLevel(0) { fIHits.reserve(PndCAParameters::MaxNStations); };

  int NHits() const { return fIHits.size(); }

  vector<PndCATES> &IHits() { return fIHits; }
  const vector<PndCATES> &IHits() const { return fIHits; }

  void AddHit(char iS, int iH)
  {
    fIHits.push_back(PndCATES(iS, iH));
    fNDF++;
  }
  void AddHit(const PndCATES &iH)
  {
    fIHits.push_back(iH);
    fNDF++;
  }

  float &Chi2() { return fChi2; }
  const float &Chi2() const { return fChi2; }
  char &NDF() { return fNDF; }
  const char &NDF() const { return fNDF; }
  char &Level() { return fLevel; }
  const char &Level() const { return fLevel; }

  static bool Compare(const PndCATrack *a, const PndCATrack *b)
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

  int NUsedHits(const PndCAHits &hits) const
  {
    int nused = 0;
    for (unsigned int iH = 0; iH < fIHits.size(); ++iH) {
      const PndCAHit &h = hits[fIHits[iH]];
      if (h.IsUsed())
        nused++;
    }
    return nused;
  }

  void SetHitsAsUsed(PndCAHits &hits) const
  {
    for (unsigned int iH = 0; iH < fIHits.size(); ++iH) {
      PndCAHit &h = hits[fIHits[iH]];
      h.SetAsUsed();
    }
  }

  const PndCATrackParam
  Fit(const PndCAHits &hits, const PndCATarget &target, const PndCAParam &caParam, bool dir = true); // dir = true - get outer parameters, dir = false - get inner ones

 private:
  float fChi2;
  char fNDF;
  vector<PndCATES> fIHits; // index in PndCATracks::fHits array

  char fLevel; // n of track segments
};

// typedef vector<PndCATrack> PndCATracks;

class PndCATracks;

class PndCATracks : public vector<PndCATrack> {
 public:
  PndCATracks(PndCAHits *hits) : fHitsRef(hits){};

  const PndCAHit &Hit(int iH, int iT) const { return (*fHitsRef)[(*this)[iT].IHits()[iH]]; }

  // selects tracks and marks all hits as used
  void SelectAndSaveTracks(PndCATracks &tracks);

  PndCAHits *HitsRef() { return fHitsRef; }

 private:
  PndCAHits *fHitsRef;
};

inline void PndCATracks::SelectAndSaveTracks(PndCATracks &tracks)
{
  vector<PndCATrack *> vptrackcandidate; // vptrackcandidate - array of pointers to vtrackcandidate
  vptrackcandidate.resize(size());

  for (unsigned int iC = 0; iC < size(); ++iC) {
    vptrackcandidate[iC] = &((*this)[iC]);
  }

  sort(vptrackcandidate.begin(), vptrackcandidate.end(), PndCATrack::Compare);

  for (vector<PndCATrack *>::iterator trIt = vptrackcandidate.begin(); trIt != vptrackcandidate.end(); ++trIt) {
    PndCATrack *tr = *trIt;

    if (tr->NUsedHits(*fHitsRef) > 0)
      continue; // don't allow tracks have shared hits. Track will be shorter, leave it for the next iteration

    tr->SetHitsAsUsed(*fHitsRef);

    tracks.push_back(*tr);
  } // i_trackCandidate
}

inline const PndCATrackParam PndCATrack::Fit(const PndCAHits &hits, const PndCATarget &target0, const PndCAParam &caParam, bool dir)
{

  vector<PndCATESV> iHits(NHits());
  const int NTHits = iHits.size();

  for (unsigned short ihit = 0; ihit < NTHits; ihit++) {
    iHits[ihit] = IHits()[ihit];
  }
  PndCATrackParamVector param;
  PndCAFunctionality::Fit(caParam, hits, param, iHits, target0, dir);
  PndCATrackParam paramS(param, 0);
  fChi2 = paramS.Chi2();
  fNDF = paramS.NDF();
  return paramS;
}

#endif
