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
 * PndWayFollower.cxx
 *
 *  Created on: Jul 26, 2016
 *      Author: kibellus
 */

#include "PndWayFollower.h"

Double_t PndWayFollower::compDist = 5;
Double_t PndWayFollower::compAngle = 10;
Double_t PndWayFollower::qualiT = 10;

vector<PndTrackCollection> PndWayFollower::followLines(Int_t begin)
{
  vector<PndTrackCollection> result;
  cout << "Start follow lines" << endl;

  /*
  vector<PndLineApproximation> allLines;
  for(map<Int_t, vector<PndFtsExpandedTrackCand>>::iterator i=cands.begin();i!=cands.end();i++){
    vector<PndFtsExpandedTrackCand> vec = i->second;
    for(int j=0;j<vec.size();j++){
      vector<PndLineApproximation> appr = vec[j].getLineApproximations();
      for(int k=0;k<appr.size();k++)allLines.push_back(appr[k]);
    }
  }*/

  for (size_t i = 0; i < fBefore.size(); i++) {
    cout << "Follow Track " << i << endl;
    PndTrackCollection trackCollection;
    PndLineApproximation a = fBefore[i];
    trackCollection.add(a, kTRUE);

    // FTS3
    PndLineApproximation best = getBestApproxTo(trackCollection.getLastLine(), begin);
    trackCollection.add(best, kFALSE);
    PndLineApproximation best2 = getBestSkewedApproxTo(trackCollection.getLastLine(), begin + 1);
    trackCollection.add(best2, kTRUE);
    PndLineApproximation best3 = getBestSkewedApproxTo(trackCollection.getLastLine(), begin + 2);
    trackCollection.add(best3, kTRUE);
    PndLineApproximation best4 = getBestApproxTo(trackCollection.getLastLine(), begin + 3);
    trackCollection.add(best4, kFALSE);

    // FTS4
    PndLineApproximation best5 = getBestApproxTo(trackCollection.getLastLine(), begin + 4);
    trackCollection.add(best5, kFALSE);
    /*if(asd==0 && i==1){
      TVector3 v(1,1,1);
      PndTrackCand trackCand;
      FairTrackParP tp1(trackCollection.getLastLine().getP1(),3 * trackCollection.getLastLine().getDir().Unit(), v, v, 1, v, v, v);
      FairTrackParP tp2(v,v.Unit(), v, v, 1, v, v, v);
      PndTrack* myCand = new ((*planes)[0]) PndTrack(tp1, tp2, trackCand);
      PndPlane p1(allLines[134].getLine(),0);
      PndPlane p2(allLines[144].getLine(),1);
      PndPlane p3(allLines[146].getLine(),1);
      PndLine l1 = p1.getIntersection(p2);
      PndLine l2 = p1.getIntersection(p3);
      fLineComp.setZValue(allLines[144].getHits()[0]->GetZ());
      cout << "Quali wrong line:" <<fLineComp.getQuality3D(trackCollection.getLastLine(),l1) << endl;
      cout << "Quali right line:" <<fLineComp.getQuality3D(trackCollection.getLastLine(),l2) << endl;
      asd++;
    }*/
    PndLineApproximation best6 = getBestSkewedApproxTo(trackCollection.getLastLine(), begin + 5);
    trackCollection.add(best6, kTRUE);
    PndLineApproximation best7 = getBestSkewedApproxTo(trackCollection.getLastLine(), begin + 6);
    trackCollection.add(best7, kTRUE);
    PndLineApproximation best8 = getBestApproxTo(trackCollection.getLastLine(), begin + 7);
    trackCollection.add(best8, kFALSE);

    result.push_back(trackCollection);
  }
  return result;
}

PndLineApproximation PndWayFollower::getBestApproxTo(PndLine l1, Int_t layer)
{
  vector<PndFtsExpandedTrackCand> layerCands = cands[layer];

  PndLineApproximation bestApprox;
  bestApprox.getLine().setRating(-1);
  Double_t bestDist = 99999999;

  for (size_t i = 0; i < layerCands.size(); i++) {
    vector<PndLineApproximation> approxs = layerCands[i].getLineApproximations();
    for (size_t j = 0; j < approxs.size(); j++) {
      PndLineApproximation approx = approxs[j];
      fLineComp.setZValue(approx.getHits()[0]->GetZ() - 20);
      PndLine l2 = approx.getLine();
      Double_t dist = fLineComp.getDist2D(l1.getP1(), l2.getP1(), l1.getDir(), l2.getDir());
      if (dist < bestDist) {
        bestDist = dist;
        bestApprox = approx;
      }
    }
  }
  if (bestDist > 5) {
    bestApprox = PndLineApproximation();
    bestApprox.getLine().setRating(-1);
  }
  // to bad?
  PndLine l2 = bestApprox.getLine();
  Double_t quali = fLineComp.getQuality(l1, l2);
  if (quali > qualiT) {
    PndLineApproximation apprWrong;
    apprWrong.getLine().setRating(-1);
    return apprWrong;
  }
  return bestApprox;
}

PndLineApproximation PndWayFollower::getBestSkewedApproxTo(PndLine l1, Int_t layer)
{
  PndPlane p1(l1, layer - 1);
  vector<PndFtsExpandedTrackCand> layerCands = cands[layer];

  PndLineApproximation bestApprox;
  bestApprox.getLine().setRating(-1);
  Double_t bestDist = 99999999;

  for (size_t i = 0; i < layerCands.size(); i++) {
    vector<PndLineApproximation> approxs = layerCands[i].getLineApproximations();
    for (size_t j = 0; j < approxs.size(); j++) {
      PndLineApproximation approx = approxs[j];
      fLineComp.setZValue(approx.getHits()[0]->GetZ());
      PndPlane p2(approx.getLine(), layer);
      PndLine l3 = p1.getIntersection(p2);

      Double_t dist = fLineComp.getQuality3D(l1, l3);
      if (dist < bestDist) {
        bestDist = dist;
        vector<PndFtsHit *> hits;
        bestApprox = PndLineApproximation(l3, hits);
        bestApprox.correctHits3DAndAdd(approx.getHits());
      }
    }
  }
  // to bad?
  PndLine l2 = bestApprox.getLine();
  Double_t quali = fLineComp.getQuality(l1, l2);
  if (quali > qualiT) {
    PndLineApproximation apprWrong;
    apprWrong.getLine().setRating(-1);
    return apprWrong;
  }
  return bestApprox;
}

PndWayFollower::~PndWayFollower()
{
  // TODO Auto-generated destructor stub
}
