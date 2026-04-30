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
 * PndLineApproximation.cxx
 *
 *  Created on: Jun 6, 2016
 *      Author: kibellus
 */

#include "PndLineApproximation.h"

#include <FairRootManager.h>

PndLineApproximation::~PndLineApproximation()
{
  // TODO Auto-generated destructor stub
}

PndLineApproximation::PndLineApproximation(PndLine lineApprox, vector<PndFtsHit *> ch1, vector<PndFtsHit *> ch2)
{
  fLineApprox = lineApprox;
  correctHits3DAndAdd(ch1);
  correctHits3DAndAdd(ch2);
}

void PndLineApproximation::correctHits3DAndAdd(vector<PndFtsHit *> ch)
{
  for (size_t i = 0; i < ch.size(); i++) {
    // check to duplicate hits
    Bool_t duplicated = kFALSE;
    for (size_t j = 0; j < fCorrectedHits.size(); j++) {
      Int_t id1 = fCorrectedHits[j]->GetTubeID();
      Int_t id2 = ch[i]->GetTubeID();
      if (id1 == id2) {
        duplicated = kTRUE;
        break;
      }
    }
    if (duplicated)
      continue;
    Int_t layer = (ch[i]->GetLayerID() - 1) / 2;
    TVector3 base2;
    ch[i]->Position(base2);
    TVector3 dir2;
    if (layer % 4 == 1) {
      dir2 = TVector3(TMath::Sin(-5 * TMath::DegToRad()), TMath::Cos(-5 * TMath::DegToRad()), 0);
    } else if (layer % 4 == 2) {
      dir2 = TVector3(TMath::Sin(5 * TMath::DegToRad()), TMath::Cos(5 * TMath::DegToRad()), 0);
    } else {
      dir2 = TVector3(0, 1, 0);
    }
    PndLine line2(base2, dir2);
    TVector3 newPos = line2.getPerpendicular(fLineApprox);
    PndFtsHit *newHit = copyHitWithNewPosition(ch[i], newPos);
    fCorrectedHits.push_back(newHit);
  }
}

PndLineApproximation PndLineApproximation::newApproximation(PndLineApproximation &approx2)
{
  vector<PndFtsHit *> hits;
  for (size_t i = 0; i < fCorrectedHits.size(); i++)
    hits.push_back(fCorrectedHits[i]);
  for (size_t i = 0; i < approx2.fCorrectedHits.size(); i++) {
    hits.push_back(approx2.fCorrectedHits[i]);
  }
  PndLine newLine = linearRegression(hits);
  newLine.setRating(fLineApprox.getRating() + approx2.fLineApprox.getRating());
  vector<PndFtsHit *> hits2;
  PndLineApproximation newApprox(newLine, hits2);
  newApprox.correctHits3DAndAdd(hits);
  return newApprox;
}

PndLine PndLineApproximation::linearRegression(vector<PndFtsHit *> hits)
{
  PndLine zxLine = linearRegressionZX(hits);
  PndLine zyLine = linearRegressionZY(hits);
  PndPlane p1(zxLine.getP1(), zxLine.getDir(), TVector3(0, 1, 0));
  PndPlane p2(zyLine.getP1(), zyLine.getDir(), TVector3(1, 0, 0));
  PndLine newLine = p1.getIntersection(p2);
  return newLine;
}

PndLine PndLineApproximation::linearRegressionXZ(vector<PndFtsHit *> hits)
{
  Double_t sumX = 0;
  Double_t sumX2 = 0;
  Double_t sumZ = 0;
  Double_t sumXZ = 0;
  for (size_t i = 0; i < hits.size(); i++) {
    cout.precision(10);
    sumX += hits[i]->GetX();
    sumX2 += hits[i]->GetX() * hits[i]->GetX();
    sumZ += hits[i]->GetZ();
    sumXZ += hits[i]->GetX() * hits[i]->GetZ();
  }
  Double_t matrix[2][3];
  matrix[0][0] = hits.size();
  matrix[1][0] = sumX;
  matrix[0][1] = sumX;
  matrix[1][1] = sumX2;
  matrix[0][2] = sumZ;
  matrix[1][2] = sumXZ;
  TVector2 result = linearRegression(matrix);
  TVector3 p1(20, 0, result.X() + 20 * result.Y());
  TVector3 p2(40, 0, result.X() + 40 * result.Y());
  // TVector3 p1(hits[0]->GetX(),0,result.X()+hits[0]->GetX()*result.Y());
  // TVector3 p2(hits[0]->GetX()+20,0,result.X()+(hits[0]->GetX())+20*result.Y());
  TVector3 dir = p1 - p2;
  return PndLine(p1, dir);
}

PndLine PndLineApproximation::linearRegressionZX(vector<PndFtsHit *> hits)
{
  Double_t sumZ = 0;
  Double_t sumZ2 = 0;
  Double_t sumX = 0;
  Double_t sumZX = 0;
  for (size_t i = 0; i < hits.size(); i++) {
    sumZ += hits[i]->GetZ();
    sumZ2 += hits[i]->GetZ() * hits[i]->GetZ();
    sumX += hits[i]->GetX();
    sumZX += hits[i]->GetZ() * hits[i]->GetX();
  }
  Double_t matrix[2][3];
  matrix[0][0] = hits.size();
  matrix[1][0] = sumZ;
  matrix[0][1] = sumZ;
  matrix[1][1] = sumZ2;
  matrix[0][2] = sumX;
  matrix[1][2] = sumZX;
  TVector2 result = linearRegression(matrix);
  TVector3 p1(result.X() + hits[0]->GetZ() * result.Y(), 0, hits[0]->GetZ());
  TVector3 p2(result.X() + (hits[0]->GetZ() + 50) * result.Y(), 0, hits[0]->GetZ() + 50);
  TVector3 dir = p1 - p2;
  return PndLine(p1, dir);
}

PndLine PndLineApproximation::linearRegressionYZ(vector<PndFtsHit *> hits)
{
  Double_t sumY = 0;
  Double_t sumY2 = 0;
  Double_t sumZ = 0;
  Double_t sumYZ = 0;
  for (size_t i = 0; i < hits.size(); i++) {
    sumY += hits[i]->GetY();
    sumY2 += hits[i]->GetY() * hits[i]->GetY();
    sumZ += hits[i]->GetZ();
    sumYZ += hits[i]->GetY() * hits[i]->GetZ();
  }
  Double_t matrix[2][3];
  matrix[0][0] = hits.size();
  matrix[1][0] = sumY;
  matrix[0][1] = sumY;
  matrix[1][1] = sumY2;
  matrix[0][2] = sumZ;
  matrix[1][2] = sumYZ;
  TVector2 result = linearRegression(matrix);
  TVector3 p1(0, -20, result.X() - 20 * result.Y());
  TVector3 p2(0, 20, result.X() + 20 * result.Y());
  TVector3 dir = p1 - p2;
  return PndLine(p1, dir);
}

PndLine PndLineApproximation::linearRegressionZY(vector<PndFtsHit *> hits)
{
  Double_t sumZ = 0;
  Double_t sumZ2 = 0;
  Double_t sumY = 0;
  Double_t sumZY = 0;
  for (size_t i = 0; i < hits.size(); i++) {
    sumZ += hits[i]->GetZ();
    sumZ2 += hits[i]->GetZ() * hits[i]->GetZ();
    sumY += hits[i]->GetY();
    sumZY += hits[i]->GetZ() * hits[i]->GetY();
  }
  Double_t matrix[2][3];
  matrix[0][0] = hits.size();
  matrix[1][0] = sumZ;
  matrix[0][1] = sumZ;
  matrix[1][1] = sumZ2;
  matrix[0][2] = sumY;
  matrix[1][2] = sumZY;
  TVector2 result = linearRegression(matrix);
  TVector3 p1(0, result.X() + hits[0]->GetZ() * result.Y(), hits[0]->GetZ());
  TVector3 p2(0, result.X() + (hits[0]->GetZ() + 300) * result.Y(), hits[0]->GetZ() + 300);
  TVector3 dir = p1 - p2;
  return PndLine(p1, dir);
}

TVector2 PndLineApproximation::linearRegression(Double_t matrix[2][3])
{
  Double_t fac = -matrix[1][0] / matrix[0][0];
  for (int i = 0; i < 3; i++) {
    matrix[1][i] += (fac * matrix[0][i]);
  }
  Double_t b = matrix[1][2] / matrix[1][1];
  Double_t a = (matrix[0][2] - b * matrix[0][1]) / matrix[0][0];
  return TVector2(a, b);
}

PndFtsHit *PndLineApproximation::copyHitWithNewPosition(PndFtsHit *h, TVector3 pos)
{
  PndFtsHit *h2 = new PndFtsHit();
  h2->SetDetectorID(h->GetDetectorID());
  h2->SetTubeID(h->GetTubeID());
  h2->SetChamberID(h->GetChamberID());
  h2->SetLayerID(h->GetLayerID());
  h2->SetXYZ(pos[0], pos[1], pos[2]);
  h2->SetRefIndex(h->GetRefIndex());
  h2->SetEntryNr(h->GetEntryNr());
  return h2;
}

PndTrack PndLineApproximation::plot(Double_t zVal1, Double_t zVal2, TClonesArray *hitArr)
{
  FairTrackParP tp1 = fLineApprox.plot(zVal1, zVal2);
  TVector3 v(1, 1, 1);
  FairTrackParP tp2(v, 0 * v, v, v, 1, v, v, v);
  PndTrackCand cand;
  Int_t j = hitArr->GetEntries();
  for (size_t i = 0; i < fCorrectedHits.size(); i++) {
    fCorrectedHits[i]->SetEntryNr(FairLink(-1, FairRootManager::Instance()->GetEntryNr(), FairRootManager::Instance()->GetBranchId("CorrectedHits"), j));
    new ((*hitArr)[j++]) PndFtsHit(*fCorrectedHits[i]); // PndFtsHit* myHit =  //[R.K.03/2017] unused variable
    cand.AddHit(fCorrectedHits[i]->GetEntryNr(), i);
  }
  PndTrack t(tp1, tp2, cand);
  return t;
}
