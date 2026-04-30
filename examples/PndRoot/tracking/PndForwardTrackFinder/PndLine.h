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
 * PndLine.h
 *
 *  Created on: Jun 14, 2016
 *      Author: kibellus
 */

#ifndef PNDTOOLS_PNDFORWARDTRACKFINDER_PNDLINE_H_
#define PNDTOOLS_PNDFORWARDTRACKFINDER_PNDLINE_H_

#include "TVector3.h"
#include "TMath.h"

#include "FairHit.h"
#include "PndFtsHit.h"
#include "PndPlane.h"
#include "FairTrackParP.h"
#include <iostream>
#include <vector>

using namespace std;

class PndLine {
 public:
  PndLine() : fP1(), fP2(), fRating(1) {}
  PndLine(FairHit *h1, FairHit *h2);
  PndLine(TVector3 &base, TVector3 &dir);
  PndLine(TVector3 h1, Int_t layer);
  virtual ~PndLine();
  void Print(Bool_t withDirVec = kFALSE);
  FairTrackParP plot(Double_t zVal1, Double_t zVal2);
  TVector3 getP1() { return fP1; }
  TVector3 getP2() { return fP2; }
  TVector3 getDir() { return fP1 - fP2; }
  bool operator<(const PndLine &) const // rhs //[R.K.03/2017] unused variable(s)
  {
    return kTRUE; // dummy sorting operator for map
  }
  Double_t getZAngle() { return TMath::Abs(TMath::ASin(getDir()[2] / getDir().Mag()) * TMath::RadToDeg()); }
  Int_t getRating() { return fRating; }
  void incrementRating() { fRating++; }
  void setRating(Int_t r) { fRating = r; }
  // PndLine approx(PndLine &l2);
  TVector3 getPerpendicular(PndLine l2); // the point on this line
  Double_t getDistTo(TVector3 p);

 private:
  TVector3 fP1;
  TVector3 fP2;
  Int_t fRating;
};

#endif /* PNDTOOLS_PNDFORWARDTRACKFINDER_PNDLINE_H_ */
