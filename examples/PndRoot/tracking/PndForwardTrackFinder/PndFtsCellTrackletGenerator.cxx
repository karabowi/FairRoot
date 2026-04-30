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
 * PndFtsCellTrackletGenerator.cxx
 *
 *  Created on: May 24, 2016
 *      Author: kibellus
 */

#include "PndFtsCellTrackletGenerator.h"

// ClassImp(PndFtsCellTrackletGenerator);

PndFtsCellTrackletGenerator::PndFtsCellTrackletGenerator()
{
  // TODO Auto-generated constructor stub
}

PndFtsCellTrackletGenerator::~PndFtsCellTrackletGenerator()
{
  // TODO Auto-generated destructor stub
}

void PndFtsCellTrackletGenerator::reset()
{
  fTracklets.clear();
}

void PndFtsCellTrackletGenerator::setHits(std::vector<PndFtsHit *> hits)
{
  fHits = hits;
}

void PndFtsCellTrackletGenerator::findTracks()
{
  std::cout << fHits.size() << " hits Found. " << std::endl;
  map<Int_t, vector<PndFtsHit *>> splittedHits = splitLayers(fHits);
  // first module
  for (int i = 0; i < 4; i++)
    findTracks(splittedHits, i);
  // second module
  for (int i = 4; i < 8; i++)
    findTracks(splittedHits, i);
  // third module
  for (int i = 8; i < 12; i++)
    findTracks(splittedHits, i);
  // fourth module
  for (int i = 12; i < 16; i++)
    findTracks(splittedHits, i);
  // fifth modules
  for (int i = 16; i < 20; i++)
    findTracks(splittedHits, i);
  // sixth module
  for (int i = 20; i < 24; i++)
    findTracks(splittedHits, i);
}

void PndFtsCellTrackletGenerator::findTracks(map<Int_t, vector<PndFtsHit *>> splittedHits, Int_t layer)
{
  map<Int_t, PndTrackCand> tracks = FindTracklets(splittedHits[layer]);
  typedef map<Int_t, PndTrackCand>::iterator iterator;
  for (iterator it = tracks.begin(); it != tracks.end(); it++) {
    fTracklets[layer].push_back(it->second);
  }
}

map<Int_t, vector<PndFtsHit *>> PndFtsCellTrackletGenerator::splitLayers(vector<PndFtsHit *> hits)
{
  map<Int_t, vector<PndFtsHit *>> map;
  for (size_t i = 0; i < hits.size(); i++) {
    PndFtsHit *hit = fHits[i];
    map[(hit->GetLayerID() - 1) / 2].push_back(hit);
  }
  return map;
}

map<Int_t, PndTrackCand> PndFtsCellTrackletGenerator::FindTracklets(vector<PndFtsHit *> hits)
{
  // typedef std::map<Int_t, vector<Int_t> >::iterator it_type; //[R.K.03/2017] unused typedef
  typedef std::map<Int_t, Int_t>::iterator it_type2;
  map<Int_t, Int_t> states;
  map<Int_t, PndFtsHit *> hitMap;
  map<Int_t, vector<Int_t>> neighbors = getNeighbors(hits);
  // create state Map
  for (size_t i = 0; i < hits.size(); i++) {
    PndFtsHit *hit = hits[i];
    states[hit->GetTubeID()] = hit->GetTubeID();
    hitMap[hit->GetTubeID()] = hit;
  }

  // run the cellular automaton
  Bool_t changes = kTRUE;
  while (changes) {
    changes = kFALSE;
    // for all hits
    for (it_type2 it = states.begin(); it != states.end(); it++) {
      // for all neighbors
      vector<int> hitNeighbors = neighbors[it->second];
      for (size_t i = 0; i < hitNeighbors.size(); i++) {
        Int_t state1 = states[it->first];
        Int_t state2 = states[hitNeighbors[i]];
        if (state1 != state2) {
          changes = kTRUE;
          Int_t min = std::min(state1, state2);
          states[it->first] = min;
          states[hitNeighbors[i]] = min;
        }
      }
    }
  }

  // create the TrackCands
  map<Int_t, PndTrackCand> result;
  for (it_type2 it = states.begin(); it != states.end(); it++) {
    Int_t hitNumber = result[it->second].GetNHits() + 1;
    result[it->second].AddHit(hitMap[it->first]->GetEntryNr(), hitNumber);
  }

  return result;
}

map<Int_t, vector<Int_t>> PndFtsCellTrackletGenerator::getNeighbors(vector<PndFtsHit *> hits)
{
  map<Int_t, vector<Int_t>> neighbors;
  for (size_t i = 0; i < hits.size(); i++) {
    PndFtsHit *hit1 = hits[i];
    transform(kTRUE, hit1);
    for (size_t j = i + 1; j < hits.size(); j++) {
      PndFtsHit *hit2 = hits[j];
      transform(kTRUE, hit2);
      TVector3 p1;
      hit1->Position(p1);
      TVector3 p2;
      hit2->Position(p2);
      TVector3 diff = p1 - p2;
      Double_t norm = TMath::Sqrt(diff.X() * diff.X() + diff.Z() * diff.Z());
      Double_t limit = 1.2;
      // if(hit1->GetSkewed()) limit=1.5;
      if (norm < limit) { // are neighbors?
        neighbors[hit1->GetTubeID()].push_back(hit2->GetTubeID());
        neighbors[hit2->GetTubeID()].push_back(hit1->GetTubeID());
      }
      transform(kTRUE, hit2);
    }
    transform(kFALSE, hit1);
  }
  return neighbors;
}

TMatrix PndFtsCellTrackletGenerator::getRotationMatrix(Double_t angle)
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

void PndFtsCellTrackletGenerator::transform(Bool_t transToNewSystem, PndFtsHit *hit)
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
