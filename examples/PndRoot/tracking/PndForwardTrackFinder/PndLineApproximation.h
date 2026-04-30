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
 * PndLineApproximation.h
 *
 *  Created on: Jun 6, 2016
 *      Author: kibellus
 */

#ifndef PNDTOOLS_PNDFORWARDTRACKFINDER_PNDLINEAPPROXIMATION_H_
#define PNDTOOLS_PNDFORWARDTRACKFINDER_PNDLINEAPPROXIMATION_H_

#include "PndLine.h"
#include "PndFtsHit.h"
#include "PndTrack.h"
#include "PndTrackCand.h"
#include "FairTrackParP.h"
#include "TVector2.h"
#include "TVector3.h"
#include "TClonesArray.h"
#include <vector>

using namespace std;

class PndLineApproximation {
 public:
  PndLineApproximation(vector<PndFtsHit *> hits) : fLineApprox(linearRegression(hits)), fCorrectedHits(hits), hasYInfo(kTRUE) {}
  PndLineApproximation() {}
  PndLineApproximation(PndLine lineApprox, vector<PndFtsHit *> correctedHits) : fLineApprox(lineApprox), fCorrectedHits(correctedHits) {}
  PndLineApproximation(PndLine lineApprox, vector<PndFtsHit *> ch1, vector<PndFtsHit *> ch2);
  virtual ~PndLineApproximation();
  PndLine &getLine() { return fLineApprox; }
  void setLine(PndLine l) { fLineApprox = l; }
  vector<PndFtsHit *> getHits() { return fCorrectedHits; }
  void setHits(vector<PndFtsHit *> h) { fCorrectedHits = h; }
  PndLineApproximation newApproximation(PndLineApproximation &approx2);
  PndLine linearRegression(vector<PndFtsHit *> hits);
  PndLine linearRegressionXZ(vector<PndFtsHit *> hits);
  PndLine linearRegressionZX(vector<PndFtsHit *> hits);
  PndLine linearRegressionYZ(vector<PndFtsHit *> hits);
  PndLine linearRegressionZY(vector<PndFtsHit *> hits);
  PndTrack plot(Double_t zVal1, Double_t zVal2, TClonesArray *hitArr);
  void correctHits3DAndAdd(vector<PndFtsHit *> correctedHits);
  void setYInfo(Bool_t i) { hasYInfo = i; }
  Bool_t HasYInfo() { return hasYInfo; }

 private:
  TVector2 linearRegression(Double_t matrix[2][3]);
  PndLine fLineApprox;
  vector<PndFtsHit *> fCorrectedHits;
  PndFtsHit *copyHitWithNewPosition(PndFtsHit *h, TVector3 pos);
  Bool_t hasYInfo;
};

#endif /* PNDTOOLS_PNDFORWARDTRACKFINDER_PNDLINEAPPROXIMATION_H_ */
