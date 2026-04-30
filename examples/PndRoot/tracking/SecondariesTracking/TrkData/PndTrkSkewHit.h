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

/** PndTrkSkewHit
 ** Class for pattern recognition hit
 ** @author Lia Lavezzi
 **
 ** fSensorID: identifies the specific sensor of the MVD,
               the specific tube of the STT
 **/

#ifndef PNDTRKSKEWHIT_H
#define PNDTRKSKEWHIT_H 1

#include "TVector3.h"
#include "PndTrkParameters.h"
#include "PndTrkHit.h"

class PndTrkSkewHit : public PndTrkHit {

 public:
  PndTrkSkewHit();
  PndTrkSkewHit(Int_t hitID, Int_t detID, Bool_t used, Int_t iregion, Int_t sensorID, TVector3 &pos, Double_t isochrone, Double_t sortvar, Int_t trackID, TVector3 center1,
                TVector3 intersection1, double phi1, TVector3 center2, TVector3 intersection2, double phi2, double a, double b, int rightone, double beta);
  PndTrkSkewHit(PndTrkHit &hit, Int_t trackID, TVector3 center1, TVector3 intersection1, double phi1, TVector3 center2, TVector3 intersection2, double phi2, double a, double b,
                int rightone, double beta);
  PndTrkSkewHit(PndTrkHit &hit);
  PndTrkSkewHit(const PndTrkSkewHit &hit);

  PndTrkSkewHit &operator=(const PndTrkSkewHit &hit);

  ~PndTrkSkewHit();

  TVector3 GetIntersection1() { return fIntersection1; }
  TVector3 GetIntersection2() { return fIntersection2; }

  Double_t GetPhi1() { return fPhi1; }
  Double_t GetPhi2() { return fPhi2; }

  void SetPhi1(Double_t phi1) { fPhi1 = phi1; }
  void SetPhi2(Double_t phi2) { fPhi2 = phi2; }

  // info
  void Draw(Color_t color);
  void Print();

  // variables
  Int_t fTrackID, fRightIntersection;
  TVector3 fCenter1, fCenter2, fIntersection1, fIntersection2;
  Double_t fa, fb, fPhi1, fPhi2, fBeta;

  ClassDef(PndTrkSkewHit, 1);
};

#endif
