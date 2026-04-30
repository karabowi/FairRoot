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
 * PndTrackCombiner.h
 *
 *  Created on: Jun 7, 2016
 *      Author: kibellus
 */

#ifndef PNDTOOLS_PNDFORWARDTRACKFINDER_PNDTRACKCOMBINER_H_
#define PNDTOOLS_PNDFORWARDTRACKFINDER_PNDTRACKCOMBINER_H_

#include <map>
#include <vector>
#include <iostream>

#include "PndFtsExpandedTrackCand.h"
#include "PndLineApproximation.h"
#include "PndFtsLineComparator.h"
#include "PndPlane.h"
#include "PndTrack.h"
#include "FairTrackParP.h"
#include "PndTrackCand.h"
#include "PndLineCombination.h"

#include "TLine.h"
#include "TMath.h"
#include "TClonesArray.h"

using namespace std;

class PndTrackCombiner {
 public:
  static Double_t compDist;
  static Double_t compAngle;
  static Double_t compQuali;

  PndTrackCombiner(TClonesArray *a, TClonesArray *b) : fComparator(compDist, compAngle), planes(a), corrHits(b), firstRun(0), angleLimit(20) {}
  virtual ~PndTrackCombiner();
  void init(map<Int_t, vector<PndFtsExpandedTrackCand>> c)
  {
    fExpandedTrackCands = c;
    trackNum = 0;
    firstRun = 0;
    ftsPos[0] = fts1ZPos;
    ftsPos[1] = fts2ZPos;
    ftsPos[2] = fts3ZPos;
    ftsPos[3] = fts4ZPos;
    ftsPos[4] = fts5ZPos;
    ftsPos[5] = fts6ZPos;
  }
  vector<PndLineApproximation> combine(Int_t bigLayer);
  vector<PndLineApproximation> combine(vector<PndFtsExpandedTrackCand> cands1, vector<PndFtsExpandedTrackCand> cands2, Int_t layer1, Int_t layer2);
  vector<PndLineApproximation> combine(PndFtsExpandedTrackCand c1, PndFtsExpandedTrackCand c2, Int_t layer1, Int_t layer2);

 private:
  map<Int_t, vector<PndFtsExpandedTrackCand>> fExpandedTrackCands;
  PndFtsLineComparator fComparator;
  void addLine(vector<PndLineCombination> &lines, PndLineApproximation &l, Int_t max);
  void addLineOld(vector<PndLineApproximation> &lines, PndLineApproximation &l, Int_t end);
  void refitLines(vector<PndLineCombination> &lines);
  vector<PndFtsExpandedTrackCand> compareAndRefit(Int_t layer1, Int_t layer2);
  vector<PndLineApproximation> compareAndRefit(vector<PndLineApproximation> l1, vector<PndLineApproximation> l2);
  TClonesArray *planes;
  TClonesArray *corrHits;
  Int_t firstRun = 0;
  Int_t trackNum;
  Double_t angleLimit;

  Double_t fts1ZPos = 295.4;
  Double_t fts2ZPos = 327.4;
  Double_t fts3ZPos = 394.5;
  Double_t fts4ZPos = 438.5;
  Double_t fts5ZPos = 607.5;
  Double_t fts6ZPos = 747.5;
  Double_t ftsPos[6];
};

#endif /* PNDTOOLS_PNDFORWARDTRACKFINDER_PNDTRACKCOMBINER_H_ */
