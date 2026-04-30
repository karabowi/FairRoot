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

/*
 * PndTrackCollection.h
 *
 *  Created on: Jul 28, 2016
 *      Author: kibellus
 */

#ifndef PNDTOOLS_PNDFORWARDTRACKFINDER_PNDTRACKCOLLECTION_H_
#define PNDTOOLS_PNDFORWARDTRACKFINDER_PNDTRACKCOLLECTION_H_

#include <vector>

#include "PndLineApproximation.h"

using namespace std;

class PndTrackCollection {
 public:
  PndTrackCollection();
  virtual ~PndTrackCollection();
  void add(PndLineApproximation l, Bool_t skewed);
  PndLine getCurrLine() { return fCurrLine; }
  PndLine getLastLine() { return (*fLines)[fLines->size() - 1].getLine(); }
  vector<PndLineApproximation> *getLines() { return fLines; }
  PndTrack getPndTrack(map<Int_t, PndFtsHit *> orgHits);
  vector<PndFtsHit *> getHits()
  {
    vector<PndFtsHit *> result;
    for (size_t i = 0; i < fLines->size(); i++) {
      for (size_t j = 0; j < (*fLines)[i].getHits().size(); j++) {
        result.push_back((*fLines)[i].getHits()[j]);
      }
    }
    return result;
  }
  vector<PndFtsHit *> fHits;
  void addHit(PndFtsHit *h) { fHits.push_back(h); }
  Double_t getDistTo(PndLine l, Int_t layer);

 private:
  std::vector<PndLineApproximation> *fLines;
  PndLine fCurrLine;
  PndFtsHit *copyHit(PndFtsHit *h);
  void refitAllHits();
  void refitAllTracks();
  void refitHit(PndLine &l, PndFtsHit *hit);
};

#endif /* PNDTOOLS_PNDFORWARDTRACKFINDER_PNDTRACKCOLLECTION_H_ */
