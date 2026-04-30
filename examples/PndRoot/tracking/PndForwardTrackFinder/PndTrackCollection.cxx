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
 * PndTrackCollection.cxx
 *
 *  Created on: Jul 28, 2016
 *      Author: kibellus
 */

#include "PndTrackCollection.h"

PndTrackCollection::PndTrackCollection()
{
  fLines = new vector<PndLineApproximation>();
}

PndTrackCollection::~PndTrackCollection() {}

void PndTrackCollection::add(PndLineApproximation l, Bool_t skewed)
{
  if (l.getLine().getRating() == -1)
    return;
  // copy hits
  vector<PndFtsHit *> newHits;
  vector<PndFtsHit *> oldHits = l.getHits();
  for (size_t i = 0; i < oldHits.size(); i++) {
    newHits.push_back(copyHit(oldHits[i]));
  }
  l.setHits(newHits);
  // add new Aprrox
  fLines->push_back(l);
  // refit
  if (fLines->size() == 1)
    fCurrLine = (*fLines)[0].getLine();
  if (fLines->size() > 1 && skewed) {
    refitAllHits();
  } else if (fLines->size() > 1) {
    vector<PndFtsHit *> hits = l.getHits();
    for (size_t i = 0; i < hits.size(); i++) {
      refitHit(fCurrLine, hits[i]);
    }
  }
  refitAllTracks();
}

PndFtsHit *PndTrackCollection::copyHit(PndFtsHit *h)
{
  PndFtsHit *h2 = new PndFtsHit();
  h2->SetDetectorID(h->GetDetectorID());
  h2->SetTubeID(h->GetTubeID());
  h2->SetChamberID(h->GetChamberID());
  h2->SetLayerID(h->GetLayerID());
  h2->SetXYZ(h->GetX(), h->GetY(), h->GetZ());
  h2->SetIsochrone(h->GetIsochrone());
  h2->SetIsochroneError(h->GetIsochroneError());
  h2->SetRefIndex(h->GetRefIndex());
  h2->SetEntryNr(h->GetEntryNr());
  return h2;
}

void PndTrackCollection::refitAllHits()
{
  // get all hits
  vector<PndFtsHit *> allHits;
  for (size_t i = 0; i < fLines->size(); i++) {
    vector<PndFtsHit *> hits = (*fLines)[i].getHits();
    for (size_t j = 0; j < hits.size(); j++)
      allHits.push_back(hits[j]);
  }
  // refit with line
  PndLine zyLine = (*fLines)[0].linearRegressionZY(allHits);
  // set the z pos
  for (size_t i = 0; i < allHits.size(); i++) {
    refitHit(zyLine, allHits[i]);
  }
  fCurrLine = zyLine;
}

void PndTrackCollection::refitAllTracks()
{
  for (size_t i = 0; i < fLines->size(); i++) {
    PndLineApproximation a = (*fLines)[i];
    PndPlane p(a.getLine(), 0);
    PndPlane p2(fCurrLine.getP1(), fCurrLine.getDir(), TVector3(1, 0, 0));
    PndLine newLine = p.getIntersection(p2);
    (*fLines)[i].setLine(newLine);
  }
}

void PndTrackCollection::refitHit(PndLine &l, PndFtsHit *hit)
{
  TVector3 base = l.getP1();
  TVector3 dir = l.getDir();
  Double_t lamp = (hit->GetZ() - base[2]) / dir[2];
  TVector3 p = base + lamp * dir;
  hit->SetY(p[1]);
}

PndTrack PndTrackCollection::getPndTrack(map<Int_t, PndFtsHit *> orgHits)
{
  PndLine first = (*fLines)[0].getLine();
  PndLine last = (*fLines)[fLines->size() - 1].getLine();
  TVector3 v(1, 1, 1);
  FairTrackParP tp1(first.getP1(), 3 * first.getDir().Unit(), v, v, 1, v, v, v);
  FairTrackParP tp2(last.getP1(), 3 * last.getDir().Unit(), v, v, 1, v, v, v);
  PndTrackCand trackCand;
  Int_t hitCount;
  for (size_t i = 0; i < fLines->size(); i++) {
    PndLineApproximation a = (*fLines)[i];
    vector<PndFtsHit *> hits = a.getHits();
    hitCount = hits.size();
    for (size_t j = 0; j < hits.size(); j++) {
      trackCand.AddHit(orgHits[hits[j]->GetTubeID()]->GetEntryNr(), j);
    }
  }
  for (size_t i = 0; i < fHits.size(); i++)
    trackCand.AddHit(fHits[i]->GetEntryNr(), i + hitCount);
  return PndTrack(tp1, tp2, trackCand);
}

Double_t PndTrackCollection::getDistTo(PndLine l, Int_t layer)
{
  Double_t dist = 9999999999;
  for (size_t i = 0; i < fLines->size(); i++) {
    PndLineApproximation appr = (*fLines)[i];
    Int_t apprLayer = appr.getHits()[0]->GetLayerID();
    if (layer < 8 && apprLayer >= 8)
      continue;
    if (layer >= 16 && apprLayer < 16)
      continue;
    if (layer > 8 && layer < 16 && layer != apprLayer)
      continue;
    PndLine l2 = appr.getLine();
    TVector3 p1 = l.getPerpendicular(l2);
    TVector3 p2 = l2.getPerpendicular(l);
    Double_t dist2 = (p1 - p2).Mag();
    if (dist2 < dist)
      dist = dist2;
  }
  return dist;
}
