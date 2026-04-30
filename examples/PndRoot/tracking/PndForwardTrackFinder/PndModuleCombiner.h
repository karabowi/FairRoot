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
 * PndModuleCombiner.h
 *
 *  Created on: Jul 18, 2016
 *      Author: kibellus
 */

#ifndef PNDTOOLS_PNDFORWARDTRACKFINDER_PNDMODULECOMBINER_H_
#define PNDTOOLS_PNDFORWARDTRACKFINDER_PNDMODULECOMBINER_H_

#include <vector>
#include <iostream>

#include "PndLineApproximation.h"
#include "PndLine.h"
#include "PndFtsLineComparator.h"
#include "PndTrackCollection.h"

using namespace std;

class PndModuleCombiner {
 public:
  static Double_t compDist;
  static Double_t compAngle;
  static Double_t hitAddT;
  PndModuleCombiner() : fLineComparator(compDist, compAngle) {}
  virtual ~PndModuleCombiner();
  vector<PndLineApproximation> combineModules(vector<PndLineApproximation> m1, vector<PndLineApproximation> m2);
  void combineModules(vector<PndTrackCollection> &c, vector<PndLineApproximation>);
  void addUnusedHits(vector<PndFtsHit *> hits, vector<PndTrackCollection> &c);

 private:
  PndFtsLineComparator fLineComparator;
};

#endif /* PNDTOOLS_PNDFORWARDTRACKFINDER_PNDMODULECOMBINER_H_ */
