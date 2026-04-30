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
 * PndFtsLineComparator.h
 *
 *  Created on: Jun 2, 2016
 *      Author: kibellus
 */

#include "PndLine.h"
#include "TVector3.h"
#include "TMath.h"

#include <iostream>

#ifndef PNDTOOLS_PNDFORWARDTRACKFINDER_PNDFTSLINECOMPARATOR_H_
#define PNDTOOLS_PNDFORWARDTRACKFINDER_PNDFTSLINECOMPARATOR_H_

using namespace std;

class PndFtsLineComparator {
 public:
  PndFtsLineComparator(Double_t maxDistance, Double_t maxAngle, Double_t z = 0) : fMaxDistance(maxDistance), fMaxAngle(maxAngle), zValue(z) {}
  virtual ~PndFtsLineComparator();
  Bool_t areEqual(PndLine l1, PndLine l2);
  Bool_t areEqual2D(PndLine l1, PndLine l2);
  Double_t getQuality3D(PndLine l1, PndLine l2);
  Double_t getQuality(PndLine l1, PndLine l2);
  void setZValue(Double_t z) { zValue = z; }
  Double_t getAngle(TVector3 d1, TVector3 d2);
  Double_t getDist(TVector3 b1, TVector3 b2, TVector3 d1, TVector3 d2);
  Double_t getAngle2D(TVector3 d1, TVector3 d2);
  Double_t getDist2D(TVector3 b1, TVector3 b2, TVector3 d1, TVector3 d2);
  void setMaxDistance(Double_t d) { fMaxDistance = d; }
  void setMaxAngle(Double_t a) { fMaxAngle = a; }
  Double_t getMaxDist() { return fMaxDistance; }
  Double_t getMaxAngle() { return fMaxAngle; }

 private:
  Double_t fMaxDistance;
  Double_t fMaxAngle;
  Double_t zValue;
};

#endif /* PNDTOOLS_PNDFORWARDTRACKFINDER_PNDFTSLINECOMPARATOR_H_ */
