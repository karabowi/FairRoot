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
 * PndPlane.h
 *
 *  Created on: Jun 14, 2016
 *      Author: kibellus
 */

#ifndef PNDTOOLS_PNDFORWARDTRACKFINDER_PNDPLANE_H_
#define PNDTOOLS_PNDFORWARDTRACKFINDER_PNDPLANE_H_

#include "PndLine.h"
#include "TMath.h"
#include "PndLine.h"

#include <iostream>

using namespace std;

class PndLine;

class PndPlane {
 public:
  PndPlane(TVector3 b, TVector3 d1, TVector3 d2) : base(b), dir1(d1), dir2(d2) {}
  PndPlane(PndLine line, Int_t layer);
  virtual ~PndPlane();
  PndLine getIntersection(PndPlane &p);
  void Print();
  TVector3 getBase() { return base; }
  TVector3 getDir1() { return dir1; }
  TVector3 getDir2() { return dir2; }
  TVector3 getIntersection(PndLine l);

 private:
  TVector3 base;
  TVector3 dir1;
  TVector3 dir2;
};

#endif /* PNDTOOLS_PNDFORWARDTRACKFINDER_PNDPLANE_H_ */
