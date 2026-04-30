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
 * PndModuleCombiner.cpp
 *
 *  Created on: Jul 18, 2016
 *      Author: kibellus
 */

#include "PndModuleCombiner.h"

Double_t PndModuleCombiner::compDist = 0.5;
Double_t PndModuleCombiner::compAngle = 3;
Double_t PndModuleCombiner::hitAddT = 1;

PndModuleCombiner::~PndModuleCombiner()
{
  // TODO Auto-generated destructor stub
}

vector<PndLineApproximation> PndModuleCombiner::combineModules(vector<PndLineApproximation> m1, vector<PndLineApproximation> m2)
{
  vector<PndLineApproximation> result;
  Bool_t usedM1[m1.size()];
  Bool_t usedM2[m2.size()];
  // init
  for (size_t i = 0; i < m1.size(); i++)
    usedM1[i] = kFALSE;
  for (size_t i = 0; i < m2.size(); i++)
    usedM2[i] = kFALSE;
  // combine
  for (size_t i = 0; i < m1.size(); i++) {
    PndLineApproximation a1 = m1[i];
    PndLine l1 = a1.getLine();
    for (size_t j = 0; j < m2.size(); j++) {
      PndLineApproximation a2 = m2[j];
      PndLine l2 = a2.getLine();
      Double_t zVal = (l1.getP1()[2] + l2.getP1()[2]) / 2;
      fLineComparator.setZValue(zVal);
      if (fLineComparator.areEqual2D(l1, l2)) {
        PndLineApproximation newApprox = a1.newApproximation(a2);
        result.push_back(newApprox);
        usedM1[i] = kTRUE;
        usedM2[j] = kTRUE;
      }
    }
  }
  // add not used tracks
  for (size_t i = 0; i < m1.size(); i++) {
    if (!usedM1[i])
      result.push_back(m1[i]);
  }
  for (size_t i = 0; i < m2.size(); i++) {
    if (!usedM2[i])
      result.push_back(m2[i]);
  }
  return result;
}

void PndModuleCombiner::combineModules(vector<PndTrackCollection> &c, vector<PndLineApproximation> a)
{
  // Bool_t used[a.size()]; //unused?
  for (size_t i = 0; i < c.size(); i++) {
    PndTrackCollection coll = c[i];
    PndLine l1 = coll.getLastLine();

    PndLineApproximation best;
    best.getLine().setRating(-1);
    Double_t bestVal = 999999999999;
    // Int_t index; //unused?
    for (size_t j = 0; j < a.size(); j++) {
      PndLineApproximation approx = a[j];
      PndLine l2 = approx.getLine();
      Double_t zVal = (l1.getP1()[2] + l2.getP1()[2]) / 2;
      fLineComparator.setZValue(zVal);
      Double_t quali = fLineComparator.getQuality(l1, l2);
      if (quali < bestVal) {
        bestVal = quali;
        best = approx;
        // index = j; //unused?
      }
    }
    /*PndLine l2 = best.getLine();
    Double_t dist = fLineComparator.getDist2D(l1.getP1(),l2.getP1(),l1.getDir(),l2.getDir());
    Double_t angle = fLineComparator.getAngle(l1.getDir(),l2.getDir());
    if(dist < 5 || angle < 30)
      coll.add(best,best.HasYInfo());*/
    if (bestVal < 8000) {
      coll.add(best, best.HasYInfo());
      // used[index] = kTRUE; //unused?
    }
  }
  // add unused
  /*for(int i=0;i<a.size();i++){
    if(!used[i]){
      PndTrackCollection newColl;
      newColl.add(a[i],a[i].HasYInfo());
      c.push_back(newColl);
    }
  }*/
}

void PndModuleCombiner::addUnusedHits(vector<PndFtsHit *> hits, vector<PndTrackCollection> &c)
{
  for (size_t i = 0; i < c.size(); i++) {
    // get all unused hits
    vector<PndFtsHit *> unused;
    vector<PndFtsHit *> collectionHits = c[i].getHits();
    for (size_t j = 0; j < hits.size(); j++) {
      Bool_t inUse = kFALSE;
      for (size_t k = 0; k < collectionHits.size(); k++) {
        if (collectionHits[k]->GetTubeID() == hits[j]->GetTubeID())
          inUse = kTRUE;
      }
      if (!inUse)
        unused.push_back(hits[j]);
    }
    // check all unused hits
    for (size_t j = 0; j < unused.size(); j++) {
      Int_t layer = (unused[j]->GetLayerID() - 1) / 2;
      TVector3 pos;
      unused[j]->Position(pos);
      PndLine l(pos, layer);
      Double_t dist = c[i].getDistTo(l, layer);
      if (dist - hitAddT < unused[j]->GetIsochrone())
        c[i].addHit(unused[j]);
    }
  }
}
