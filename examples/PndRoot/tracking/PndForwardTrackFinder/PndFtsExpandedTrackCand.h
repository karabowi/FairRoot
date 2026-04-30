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
 * PndFtsExpandedTrackCand.h
 *
 *  Created on: Jun 2, 2016
 *      Author: kibellus
 */

#ifndef PNDTOOLS_PNDFORWARDTRACKFINDER_PNDFTSEXPANDEDTRACKCAND_H_
#define PNDTOOLS_PNDFORWARDTRACKFINDER_PNDFTSEXPANDEDTRACKCAND_H_

#include "PndLine.h"
#include <vector>
#include "PndTrackCand.h"
#include "PndFtsHit.h"
#include "PndLineApproximation.h"

using namespace std;

class PndFtsExpandedTrackCand {
 public:
  PndFtsExpandedTrackCand(PndTrackCand pndTrackCand, vector<PndFtsHit *> sourceHits, vector<PndLineApproximation> approximations)
    : fPndTrackCand(pndTrackCand), fSourceHits(sourceHits), fApproximations(approximations)
  {
  }
  PndFtsExpandedTrackCand(vector<PndFtsHit *> h1, vector<PndFtsHit *> h2, vector<PndLineApproximation> l);
  virtual ~PndFtsExpandedTrackCand();
  vector<PndLineApproximation> getLineApproximations() { return fApproximations; }
  vector<PndFtsHit *> getSourceHits() { return fSourceHits; }
  PndTrackCand getTrackCand() { return fPndTrackCand; }

 private:
  PndTrackCand fPndTrackCand;
  vector<PndFtsHit *> fSourceHits;
  vector<PndLineApproximation> fApproximations;
};

#endif /* PNDTOOLS_PNDFORWARDTRACKFINDER_PNDFTSEXPANDEDTRACKCAND_H_ */
