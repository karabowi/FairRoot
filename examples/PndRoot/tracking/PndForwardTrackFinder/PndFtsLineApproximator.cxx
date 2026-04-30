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
 * PndFtsLineApproximator.cpp
 *
 *  Created on: Jun 2, 2016
 *      Author: kibellus
 */

#include "PndFtsLineApproximator.h"

#include <FairRootManager.h>

Double_t PndFtsLineApproximator::compDist = 0.2;
Double_t PndFtsLineApproximator::compAngle = 0.2;

PndFtsLineApproximator::~PndFtsLineApproximator()
{
  // TODO Auto-generated destructor stub
}

PndFtsExpandedTrackCand PndFtsLineApproximator::createExpandedTrackCand(PndTrackCand &cand)
{
  FairRootManager *ioman = FairRootManager::Instance();
  Int_t branchID = ioman->GetBranchId("FTSHit"); // right hits
  vector<PndFtsHit *> hits;
  set<FairLink> linksToHits = cand.GetLinksWithType(branchID).GetLinks();

  for (std::set<FairLink>::iterator it = linksToHits.begin(); it != linksToHits.end(); it++) {
    FairLink link = *it;
    PndFtsHit *hit = (PndFtsHit *)ioman->GetCloneOfLinkData(link);
    transform(kTRUE, hit);
    hit->SetY(0); // project to y=0
    hits.push_back(hit);
  }
  fComparator.setZValue(hits[0]->GetZ());

  // track with only one hit
  if (hits.size() == 1) {
    vector<PndLineApproximation> approx; // empty vector
    return PndFtsExpandedTrackCand(cand, hits, approx);
  } else if (hits.size() == 2) {
    // two hits => 4 lines
    vector<PndLineApproximation> approx = approxLines(hits[0], hits[1]);
    return PndFtsExpandedTrackCand(cand, hits, approx);
  } else {
    // more than 2 hits => search the best line
    vector<vector<PndLineApproximation>> lines;
    // create all combinations of possible lines
    for (size_t i = 0; i < hits.size(); i++) {
      for (size_t j = i + 1; j < hits.size(); j++) {
        vector<PndLineApproximation> lines2 = approxLines(hits[i], hits[j]);
        for (size_t k = 0; k < lines2.size(); k++)
          addLine(lines, lines2[k]);
      }
    }
    // find the max
    size_t max = 0;
    for (size_t i = 0; i < lines.size(); i++) {
      if (lines[i].size() > max)
        max = lines[i].size();
    }
    vector<PndLineApproximation> result;
    // find all approximations with size=max
    for (size_t i = 0; i < lines.size(); i++) {
      if (lines[i].size() == max) {
        PndLineApproximation a = lines[i][0];
        for (size_t j = 1; j < lines[i].size(); j++)
          a = a.newApproximation(lines[i][j]);
        a.getLine().setRating(1);
        result.push_back(a);
      }
    }
    return PndFtsExpandedTrackCand(cand, hits, result);
  }
}

vector<PndLineApproximation> PndFtsLineApproximator::approxLines(PndFtsHit *h1, PndFtsHit *h2)
{
  vector<PndLineApproximation> result;
  vector<PndLineApproximation> r1 = approxInnerLine(h1, h2);
  result.push_back(r1[0]);
  result.push_back(r1[1]);
  vector<PndLineApproximation> r2 = approxOuterLine(h1, h2);
  result.push_back(r2[0]);
  result.push_back(r2[1]);
  return result;
}

vector<PndLineApproximation> PndFtsLineApproximator::approxInnerLine(PndFtsHit *h1, PndFtsHit *h2)
{
  vector<PndLineApproximation> result;
  Double_t r0 = h1->GetIsochrone();
  Double_t r1 = h2->GetIsochrone();
  Double_t a = h1->GetX();
  Double_t b = h1->GetZ();
  Double_t c = h2->GetX();
  Double_t d = h2->GetZ();

  Double_t xp = (c * r0 - a * r1) / (r0 - r1);
  Double_t yp = (d * r0 - b * r1) / (r0 - r1);

  Double_t xt1Z = r0 * r0 * (xp - a) + r0 * (yp - b) * TMath::Sqrt((xp - a) * (xp - a) + (yp - b) * (yp - b) - r0 * r0);
  Double_t xt1N = (xp - a) * (xp - a) + (yp - b) * (yp - b);
  Double_t xt1 = xt1Z / xt1N + a;

  Double_t yt1Z = r0 * r0 * (yp - b) + r0 * (xp - a) * TMath::Sqrt((xp - a) * (xp - a) + (yp - b) * (yp - b) - r0 * r0);
  Double_t yt1N = (xp - a) * (xp - a) + (yp - b) * (yp - b);
  Double_t yt1 = yt1Z / yt1N + b;

  Double_t xt2Z = r0 * r0 * (xp - a) - r0 * (yp - b) * TMath::Sqrt((xp - a) * (xp - a) + (yp - b) * (yp - b) - r0 * r0);
  Double_t xt2N = (xp - a) * (xp - a) + (yp - b) * (yp - b);
  Double_t xt2 = xt2Z / xt2N + a;

  Double_t yt2Z = r0 * r0 * (yp - b) - r0 * (xp - a) * TMath::Sqrt((xp - a) * (xp - a) + (yp - b) * (yp - b) - r0 * r0);
  Double_t yt2N = (xp - a) * (xp - a) + (yp - b) * (yp - b);
  Double_t yt2 = yt2Z / yt2N + b;

  Double_t xt3Z = r1 * r1 * (xp - c) + r1 * (yp - d) * TMath::Sqrt((xp - c) * (xp - c) + (yp - d) * (yp - d) - r1 * r1);
  Double_t xt3N = (xp - c) * (xp - c) + (yp - d) * (yp - d);
  Double_t xt3 = xt3Z / xt3N + c;

  Double_t yt3Z = r1 * r1 * (yp - d) + r1 * (xp - c) * TMath::Sqrt((xp - c) * (xp - c) + (yp - d) * (yp - d) - r1 * r1);
  Double_t yt3N = (xp - c) * (xp - c) + (yp - d) * (yp - d);
  Double_t yt3 = yt3Z / yt3N + d;

  Double_t xt4Z = r1 * r1 * (xp - c) - r1 * (yp - d) * TMath::Sqrt((xp - c) * (xp - c) + (yp - d) * (yp - d) - r1 * r1);
  Double_t xt4N = (xp - c) * (xp - c) + (yp - d) * (yp - d);
  Double_t xt4 = xt4Z / xt4N + c;

  Double_t yt4Z = r1 * r1 * (yp - d) - r1 * (xp - c) * TMath::Sqrt((xp - c) * (xp - c) + (yp - d) * (yp - d) - r1 * r1);
  Double_t yt4N = (xp - c) * (xp - c) + (yp - d) * (yp - d);
  Double_t yt4 = yt4Z / yt4N + d;

  // outer lines
  Double_t s = TMath::Abs((b - yt1) * (yp - yt1) / ((xt1 - a) * (xt1 - xp)));
  Double_t p1x = xt1;
  Double_t p1y;
  Double_t p2x = xt2;
  Double_t p2y;
  if (TMath::Abs(s - 1) < 0.0000000001) {
    p1y = yt1;
    p2y = yt2;
  } else {
    p1y = yt2;
    p2y = yt1;
  }

  s = TMath::Abs((d - yt3) * (yp - yt3) / ((xt3 - c) * (xt3 - xp)));

  Double_t p3x = xt3;
  Double_t p3y;
  Double_t p4x = xt4;
  Double_t p4y;
  if (TMath::Abs(s - 1) < 0.0000000001) {
    p3y = yt3;
    p4y = yt4;
  } else {
    p3y = yt4;
    p4y = yt3;
  }

  // create the corrected hits
  PndFtsHit *newHit1 = copyHitWithNewPosition(h1, p1x, p1y);
  PndFtsHit *newHit2 = copyHitWithNewPosition(h1, p2x, p2y);
  PndFtsHit *newHit3 = copyHitWithNewPosition(h2, p3x, p3y);
  PndFtsHit *newHit4 = copyHitWithNewPosition(h2, p4x, p4y);

  // transform to normal coordinate-system
  transform(kFALSE, newHit1);
  transform(kFALSE, newHit2);
  transform(kFALSE, newHit3);
  transform(kFALSE, newHit4);

  // create 1st line
  vector<PndFtsHit *> hits1;
  hits1.push_back(newHit1);
  hits1.push_back(newHit3);
  PndLine l1(newHit1, newHit3);
  result.push_back(PndLineApproximation(l1, hits1));

  // create 2nd line
  vector<PndFtsHit *> hits2;
  hits2.push_back(newHit2);
  hits2.push_back(newHit4);
  PndLine l2(newHit2, newHit4);
  result.push_back(PndLineApproximation(l2, hits2));

  return result;
}

vector<PndLineApproximation> PndFtsLineApproximator::approxOuterLine(PndFtsHit *h1, PndFtsHit *h2)
{
  vector<PndLineApproximation> result;
  Double_t r0 = h1->GetIsochrone();
  Double_t r1 = h2->GetIsochrone();

  Double_t a = h1->GetX();
  Double_t b = h1->GetZ();
  Double_t c = h2->GetX();
  Double_t d = h2->GetZ();

  Double_t xp = (c * r0 + a * r1) / (r0 + r1);
  Double_t yp = (d * r0 + b * r1) / (r0 + r1);

  Double_t xt1Z = r0 * r0 * (xp - a) + r0 * (yp - b) * TMath::Sqrt((xp - a) * (xp - a) + (yp - b) * (yp - b) - r0 * r0);
  Double_t xt1N = (xp - a) * (xp - a) + (yp - b) * (yp - b);
  Double_t xt1 = xt1Z / xt1N + a;

  Double_t yt1Z = r0 * r0 * (yp - b) + r0 * (xp - a) * TMath::Sqrt((xp - a) * (xp - a) + (yp - b) * (yp - b) - r0 * r0);
  Double_t yt1N = (xp - a) * (xp - a) + (yp - b) * (yp - b);
  Double_t yt1 = yt1Z / yt1N + b;

  Double_t xt2Z = r0 * r0 * (xp - a) - r0 * (yp - b) * TMath::Sqrt((xp - a) * (xp - a) + (yp - b) * (yp - b) - r0 * r0);
  Double_t xt2N = (xp - a) * (xp - a) + (yp - b) * (yp - b);
  Double_t xt2 = xt2Z / xt2N + a;

  Double_t yt2Z = r0 * r0 * (yp - b) - r0 * (xp - a) * TMath::Sqrt((xp - a) * (xp - a) + (yp - b) * (yp - b) - r0 * r0);
  Double_t yt2N = (xp - a) * (xp - a) + (yp - b) * (yp - b);
  Double_t yt2 = yt2Z / yt2N + b;

  Double_t xt3Z = r1 * r1 * (xp - c) + r1 * (yp - d) * TMath::Sqrt((xp - c) * (xp - c) + (yp - d) * (yp - d) - r1 * r1);
  Double_t xt3N = (xp - c) * (xp - c) + (yp - d) * (yp - d);
  Double_t xt3 = xt3Z / xt3N + c;

  Double_t yt3Z = r1 * r1 * (yp - d) + r1 * (xp - c) * TMath::Sqrt((xp - c) * (xp - c) + (yp - d) * (yp - d) - r1 * r1);
  Double_t yt3N = (xp - c) * (xp - c) + (yp - d) * (yp - d);
  Double_t yt3 = yt3Z / yt3N + d;

  Double_t xt4Z = r1 * r1 * (xp - c) - r1 * (yp - d) * TMath::Sqrt((xp - c) * (xp - c) + (yp - d) * (yp - d) - r1 * r1);
  Double_t xt4N = (xp - c) * (xp - c) + (yp - d) * (yp - d);
  Double_t xt4 = xt4Z / xt4N + c;

  Double_t yt4Z = r1 * r1 * (yp - d) - r1 * (xp - c) * TMath::Sqrt((xp - c) * (xp - c) + (yp - d) * (yp - d) - r1 * r1);
  Double_t yt4N = (xp - c) * (xp - c) + (yp - d) * (yp - d);
  Double_t yt4 = yt4Z / yt4N + d;

  // outer lines
  Double_t s = TMath::Abs((b - yt1) * (yp - yt1) / ((xt1 - a) * (xt1 - xp)));
  Double_t p1x = xt1;
  Double_t p1y;
  Double_t p2x = xt2;
  Double_t p2y;
  if (TMath::Abs(s - 1) < 0.0000000001) {
    p1y = yt1;
    p2y = yt2;
  } else {
    p1y = yt2;
    p2y = yt1;
  }

  s = TMath::Abs((d - yt3) * (yp - yt3) / ((xt3 - c) * (xt3 - xp)));

  Double_t p3x = xt3;
  Double_t p3y;
  Double_t p4x = xt4;
  Double_t p4y;
  if (TMath::Abs(s - 1) < 0.0000000001) {
    p3y = yt3;
    p4y = yt4;
  } else {
    p3y = yt4;
    p4y = yt3;
  }

  // create the corrected hits
  PndFtsHit *newHit1 = copyHitWithNewPosition(h1, p1x, p1y);
  PndFtsHit *newHit2 = copyHitWithNewPosition(h1, p2x, p2y);
  PndFtsHit *newHit3 = copyHitWithNewPosition(h2, p3x, p3y);
  PndFtsHit *newHit4 = copyHitWithNewPosition(h2, p4x, p4y);

  // transform to normal coordinate-system
  transform(kFALSE, newHit1);
  transform(kFALSE, newHit2);
  transform(kFALSE, newHit3);
  transform(kFALSE, newHit4);

  // create 1st line
  vector<PndFtsHit *> hits1;

  hits1.push_back(newHit1);
  hits1.push_back(newHit3);
  PndLine l1(newHit1, newHit3);
  // TLine l1(newHit1->GetX(),newHit1->GetZ(),newHit3->GetX(),newHit3->GetZ());
  result.push_back(PndLineApproximation(l1, hits1));

  // create 2nd line
  vector<PndFtsHit *> hits2;
  hits2.push_back(newHit2);
  hits2.push_back(newHit4);
  // TLine l2(newHit2->GetX(),newHit2->GetZ(),newHit4->GetX(),newHit4->GetZ());
  PndLine l2(newHit2, newHit4);
  result.push_back(PndLineApproximation(l2, hits2));

  return result;
}

void PndFtsLineApproximator::addLine(vector<vector<PndLineApproximation>> &lines, PndLineApproximation &l)
{
  Bool_t used = kFALSE;
  for (size_t i = 0; i < lines.size(); i++) {
    PndLine l2 = lines[i][0].getLine();
    if (fComparator.areEqual2D(l.getLine(), l2)) {
      lines[i].push_back(l);
      used = kTRUE;
    }
  }
  if (used)
    return;
  vector<PndLineApproximation> newLine;
  newLine.push_back(l);
  lines.push_back(newLine);
}
/*
PndLineApproximation PndFtsLineApproximator::createLine(vector<PndLineApproximation> lines){
  vector<PndFtsHit*> correctedHits;
  for(int i=0;i<lines.size();i++){
    vector<PndFtsHit*> hitsOfLine = lines[i].getHits();
    for(int j=0;j<hitsOfLine.size();j++){
      Bool_t exist = kFALSE;
      for(int k=0;k<correctedHits.size();k++){
        if(correctedHits[k]->GetTubeID()==hitsOfLine[j]->GetTubeID()){
          exist = kTRUE;
          break;
        }
      }
      if(!exist)
        correctedHits.push_back(hitsOfLine[j]);
    }
  }
  PndLineApproximation newApprox(lines[0].getLine(),correctedHits);
  return newApprox;
}*/

PndFtsHit *PndFtsLineApproximator::copyHitWithNewPosition(PndFtsHit *h, Double_t x, Double_t z)
{
  PndFtsHit *h2 = new PndFtsHit();
  h2->SetDetectorID(h->GetDetectorID());
  h2->SetTubeID(h->GetTubeID());
  h2->SetChamberID(h->GetChamberID());
  h2->SetLayerID(h->GetLayerID());
  h2->SetXYZ(x, h->GetY(), z);
  h2->SetIsochrone(h->GetIsochrone());
  h2->SetIsochroneError(h->GetIsochroneError());
  h2->SetRefIndex(h->GetRefIndex());
  h2->SetEntryNr(h->GetEntryNr());
  return h2;
}

TMatrix PndFtsLineApproximator::getRotationMatrix(Double_t angle)
{
  TMatrix a(3, 3);
  a[0][0] = TMath::Cos(angle);
  a[0][1] = -TMath::Sin(angle);
  a[0][2] = 0;
  a[1][0] = TMath::Sin(angle);
  a[1][1] = TMath::Cos(angle);
  a[1][2] = 0;
  a[2][0] = 0;
  a[2][1] = 0;
  a[2][2] = 1;
  return a;
}

void PndFtsLineApproximator::transform(Bool_t transToNewSystem, PndFtsHit *hit)
{
  Double_t angle = 0;
  if (((hit->GetLayerID() - 1) / 2) % 4 == 1)
    angle = -5;
  if (((hit->GetLayerID() - 1) / 2) % 4 == 2)
    angle = 5;
  if (angle == 0)
    return;
  if (!transToNewSystem)
    angle *= -1;
  TMatrix rotMat = getRotationMatrix(angle * TMath::DegToRad());
  TMatrix hitMat(3, 1);
  hitMat[0][0] = hit->GetX();
  hitMat[1][0] = hit->GetY();
  hitMat[2][0] = hit->GetZ();
  TMatrix newPos = rotMat * hitMat;
  hit->SetXYZ(newPos[0][0], newPos[1][0], newPos[2][0]);
}
