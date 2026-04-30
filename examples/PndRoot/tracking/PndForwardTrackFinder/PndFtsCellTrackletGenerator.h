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
 * PndFtsCellTrackletGenerator.h
 *
 *  Created on: May 24, 2016
 *      Author: kibellus
 */
#include <vector>
#include <map>
#include <algorithm>

#include "PndFtsHit.h"
#include "PndTrackCand.h"

#include "TMath.h"
#include "TVector3.h"
#include "TLine.h"

#ifndef PNDTOOLS_PNDFORWARDTRACKFINDER_PNDFTSCELLTRACKLETGENERATOR_H_
#define PNDTOOLS_PNDFORWARDTRACKFINDER_PNDFTSCELLTRACKLETGENERATOR_H_
using namespace std;
class PndFtsCellTrackletGenerator {
 public:
  PndFtsCellTrackletGenerator();
  virtual ~PndFtsCellTrackletGenerator();
  void reset();
  void setHits(std::vector<PndFtsHit *> hits);
  void findTracks();
  map<Int_t, vector<PndTrackCand>> getTracklets() { return fTracklets; }

 private:
  std::vector<PndFtsHit *> fHits;
  map<Int_t, vector<PndTrackCand>> fTracklets;

  void findTracks(map<Int_t, vector<PndFtsHit *>>, Int_t layer);
  map<Int_t, vector<PndFtsHit *>> splitLayers(vector<PndFtsHit *> hits);
  map<Int_t, PndTrackCand> FindTracklets(vector<PndFtsHit *> hits);
  map<Int_t, std::vector<Int_t>> getNeighbors(vector<PndFtsHit *> hits);

  void transform(Bool_t transToNewSystem, PndFtsHit *hit);
  TMatrix getRotationMatrix(Double_t angle);
  // ClassDef(PndFtsCellTrackletGenerator,1);
};

#endif /* PNDTOOLS_PNDFORWARDTRACKFINDER_PNDFTSCELLTRACKLETGENERATOR_H_ */
