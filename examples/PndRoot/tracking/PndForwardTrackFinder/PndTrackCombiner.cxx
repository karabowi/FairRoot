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
 * PndTrackCombiner.cxx
 *
 *  Created on: Jun 7, 2016
 *      Author: kibellus
 */

#include "PndTrackCombiner.h"

Double_t PndTrackCombiner::compQuali = 600;
Double_t PndTrackCombiner::compDist = 0.7;
Double_t PndTrackCombiner::compAngle = 2.5;

PndTrackCombiner::~PndTrackCombiner()
{
  // TODO Auto-generated destructor stubPndTrack*
}

vector<PndLineApproximation> PndTrackCombiner::combine(Int_t bigLayer)
{
  Int_t l1 = 4 * bigLayer + 0;
  Int_t l2 = 4 * bigLayer + 1;
  Int_t l3 = 4 * bigLayer + 2;
  Int_t l4 = 4 * bigLayer + 3;

  vector<PndFtsExpandedTrackCand> layer1a4 = compareAndRefit(l1, l4);
  vector<PndLineApproximation> arr[3];
  arr[0] = combine(layer1a4, fExpandedTrackCands[l2], 0, 1);
  arr[1] = combine(layer1a4, fExpandedTrackCands[l3], 0, 2);
  arr[2] = combine(fExpandedTrackCands[l2], fExpandedTrackCands[l3], 1, 2);

  //	cout << "firstAndLast=" << layer1a4.size() << endl;
  //	cout << "arr[0]=" << arr[0].size() << endl;
  //	cout << "arr[1]=" << arr[1].size() << endl;
  //	cout << "arr[2]=" << arr[2].size() << endl;

  // new implementation
  vector<PndLineCombination> combindedLines;
  // add the tracks of the first layer
  for (size_t i = 0; i < arr[0].size(); i++) {
    PndLineApproximation newApprox(arr[0][i]);
    combindedLines.push_back(newApprox);
  }

  // firstRun = combindedLines.size();
  for (int i = 1; i < 3; i++) {
    vector<PndLineApproximation> lines = arr[i];
    fComparator.setZValue(ftsPos[bigLayer] + 5 * i);
    Int_t maxVal = combindedLines.size(); // tracks in layers before -> no combination in same layer
    for (size_t j = 0; j < lines.size(); j++) {
      addLine(combindedLines, lines[j], maxVal);
    }
    refitLines(combindedLines);
  }
  // get all lines with a high quality
  vector<PndLineApproximation> result;
  for (size_t i = 0; i < combindedLines.size(); i++) {
    PndLineCombination combi = combindedLines[i];
    PndLineApproximation approx = combi.getApprox1();
    if (approx.getLine().getRating() > 2) {
      result.push_back(approx);
    }
  }
  // combine similar tracks
  vector<PndLineApproximation> result2;
  fComparator.setZValue(ftsPos[bigLayer]);
  Bool_t used[result.size()];
  for (size_t i = 0; i < result.size(); i++)
    used[i] = kFALSE;
  for (size_t i = 0; i < result.size(); i++) {
    if (used[i])
      continue;
    PndLineApproximation a = result[i];
    for (size_t j = i + 1; j < result.size(); j++) {
      if (used[j])
        continue;
      PndLineApproximation b = result[j];
      if (fComparator.areEqual2D(a.getLine(), b.getLine())) {
        a = a.newApproximation(b);
        used[j] = kTRUE;
      }
    }
    used[i] = kTRUE;
    result2.push_back(a);
  }
  return result2;
}

vector<PndLineApproximation> PndTrackCombiner::combine(vector<PndFtsExpandedTrackCand> cands1, vector<PndFtsExpandedTrackCand> cands2, Int_t layer1, Int_t layer2)
{
  vector<PndLineApproximation> result;
  for (size_t i = 0; i < cands1.size(); i++) {
    size_t resultSizeBefore = result.size();
    PndFtsExpandedTrackCand cand1 = cands1[i];
    for (size_t j = 0; j < cands2.size(); j++) {
      PndFtsExpandedTrackCand cand2 = cands2[j];
      vector<PndLineApproximation> lines = combine(cand1, cand2, layer1, layer2);
      result.insert(result.begin(), lines.begin(), lines.end());
    }
    // layer1 track not used?
    if (layer1 == 0 && result.size() == resultSizeBefore) {
      PndLineApproximation la = cand1.getLineApproximations()[0];
      la.getLine().setRating(3);
      la.setYInfo(kFALSE);
      result.push_back(la);
    }
  }
  return result;
}

vector<PndLineApproximation> PndTrackCombiner::combine(PndFtsExpandedTrackCand c1, PndFtsExpandedTrackCand c2, Int_t layer1, Int_t layer2)
{
  vector<PndLineApproximation> approx1 = c1.getLineApproximations();
  vector<PndLineApproximation> approx2 = c2.getLineApproximations();
  vector<PndLineApproximation> result;
  for (size_t i = 0; i < approx1.size(); i++) {
    for (size_t j = 0; j < approx2.size(); j++) {
      PndLine line1 = approx1[i].getLine();
      PndLine line2 = approx2[j].getLine();
      PndPlane p1(line1, layer1);
      PndPlane p2(line2, layer2);
      PndLine l = p1.getIntersection(p2);

      Double_t angle = l.getZAngle();
      if (TMath::Min(angle, 180 - angle) > angleLimit) {
        vector<PndFtsHit *> cHits1 = approx1[i].getHits();
        vector<PndFtsHit *> cHits2 = approx2[j].getHits();
        l.setRating(TMath::Max(line1.getRating(), line2.getRating()));
        PndLineApproximation newApprox(l, cHits1, cHits2);
        result.push_back(newApprox);
      }
    }
  }
  return result;
}

void PndTrackCombiner::addLine(vector<PndLineCombination> &lines, PndLineApproximation &la, Int_t max)
{
  PndLine &l = la.getLine();
  Bool_t inUse = kFALSE;
  for (int i = 0; i < max; i++) {
    PndLine &l2 = lines[i].getApprox1().getLine();
    // compare with the line of other layer
    Double_t quality = fComparator.getQuality(l, l2);
    if (quality > compQuali)
      continue;
    // is this line better as the current best line
    if (lines[i].getQuality() > quality) {
      // better line found
      lines[i].setApprox2(&la);
      lines[i].setQuality(quality);
    }
    inUse = kTRUE;
  }
  if (!inUse) {
    PndLineCombination newCom(la);
    lines.push_back(newCom);
  }
}

void PndTrackCombiner::refitLines(vector<PndLineCombination> &lines)
{
  for (size_t i = 0; i < lines.size(); i++) {
    PndLineApproximation *a2 = lines[i].getApprox2();
    if (a2 != 0) {
      PndLineApproximation a1 = lines[i].getApprox1();
      PndLineApproximation newApprox = a1.newApproximation(*a2);
      lines[i].setApprox1(newApprox);
      lines[i].setApprox2(0);
    }
  }
}

void PndTrackCombiner::addLineOld(vector<PndLineApproximation> &lines, PndLineApproximation &la, Int_t end)
{
  PndLine &l = la.getLine();
  for (int i = 0; i < end; i++) {
    PndLine &l2 = lines[i].getLine();
    if (fComparator.areEqual2D(l, l2)) {
      l.incrementRating();
      l2.incrementRating();
    }
  }
  lines.push_back(la);
}

vector<PndFtsExpandedTrackCand> PndTrackCombiner::compareAndRefit(Int_t layer1, Int_t layer2)
{
  vector<PndFtsExpandedTrackCand> cands1 = fExpandedTrackCands[layer1];
  vector<PndFtsExpandedTrackCand> cands2 = fExpandedTrackCands[layer2];
  vector<PndFtsExpandedTrackCand> result;
  Bool_t used2[cands2.size()]; // second cand is used
  for (size_t j = 0; j < cands2.size(); j++)
    used2[j] = kFALSE;

  for (size_t i = 0; i < cands1.size(); i++) {
    Bool_t used = kFALSE;
    for (size_t j = 0; j < cands2.size(); j++) {
      PndFtsExpandedTrackCand cand1 = cands1[i];
      PndFtsExpandedTrackCand cand2 = cands2[j];
      vector<PndLineApproximation> a1 = cand1.getLineApproximations();
      vector<PndLineApproximation> a2 = cand2.getLineApproximations();
      vector<PndLineApproximation> lines = compareAndRefit(a1, a2);
      if (lines.size() == 0)
        continue;
      used = kTRUE;
      used2[j] = kTRUE;
      PndFtsExpandedTrackCand etc(cand1.getSourceHits(), cand2.getSourceHits(), lines);
      result.push_back(etc);
    }
    if (!used)
      result.push_back(cands1[i]);
  }
  for (size_t j = 0; j < cands2.size(); j++) {
    if (!used2[j])
      result.push_back(cands2[j]);
  }
  return result;
}

vector<PndLineApproximation> PndTrackCombiner::compareAndRefit(vector<PndLineApproximation> l1, vector<PndLineApproximation> l2)
{
  vector<PndLineApproximation> result;
  for (size_t i = 0; i < l1.size(); i++) {
    PndLineApproximation approx1 = l1[i];
    PndLine line1 = approx1.getLine();
    fComparator.setZValue(line1.getP1()[2]);
    for (size_t j = 0; j < l2.size(); j++) {
      PndLineApproximation approx2 = l2[j];
      PndLine line2 = approx2.getLine();
      if (fComparator.areEqual2D(line1, line2)) {
        PndLineApproximation newApprox = approx1.newApproximation(approx2);
        result.push_back(newApprox);
        return result;
      }
    }
  }
  return result;
}
