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
 * PndFtsLineApproximator.h
 *
 *  Created on: Jun 2, 2016
 *      Author: kibellus
 */
#include <vector>
#include "PndLine.h"
#include "TMatrix.h"
#include "TMath.h"
#include "PndTrackCand.h"
#include "PndFtsHit.h"
#include "PndFtsExpandedTrackCand.h"
//#include "PndLineApproximation.h"
#include "PndFtsLineComparator.h"
#include "Tuple.h"

using namespace std;

#ifndef PNDTOOLS_PNDFORWARDTRACKFINDER_PNDFTSLINEAPPROXIMATOR_H_
#define PNDTOOLS_PNDFORWARDTRACKFINDER_PNDFTSLINEAPPROXIMATOR_H_

class PndFtsLineApproximator {
 public:
  static Double_t compDist;
  static Double_t compAngle;
  PndFtsLineApproximator() : fComparator(compDist, compAngle) {}
  virtual ~PndFtsLineApproximator();
  PndFtsExpandedTrackCand createExpandedTrackCand(PndTrackCand &cand);

 private:
  vector<PndLineApproximation> approxLines(PndFtsHit *h1, PndFtsHit *h2);
  vector<PndLineApproximation> approxInnerLine(PndFtsHit *h1, PndFtsHit *h2);
  vector<PndLineApproximation> approxOuterLine(PndFtsHit *h1, PndFtsHit *h2);
  void addLine(vector<vector<PndLineApproximation>> &lines, PndLineApproximation &l);
  // PndLineApproximation createLine(vector<PndLineApproximation> lines);

  PndFtsHit *copyHitWithNewPosition(PndFtsHit *h, Double_t x, Double_t z);

  void transform(Bool_t transToNewSystem, PndFtsHit *hit);
  TMatrix getRotationMatrix(Double_t angle);
  PndFtsLineComparator fComparator;
};

#endif /* PNDTOOLS_PNDFORWARDTRACKFINDER_PNDFTSLINEAPPROXIMATOR_H_ */
