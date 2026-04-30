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

//
// PndTrkHit.cxx
//
// Class for pattern recognition skewed hit
//
// authors: Lia Lavezzi - INFN Pavia (2012)
//

#include "PndTrkSkewHit.h"

#include "PndDetectorList.h"

#include <iostream>

// ROOT
#include "TEllipse.h"

using namespace std;

PndTrkSkewHit::PndTrkSkewHit()
  : PndTrkHit(), fTrackID(-1), fRightIntersection(-1), fCenter1(0., 0., 0.), fCenter2(0., 0., 0.), fIntersection1(0., 0., 0.), fIntersection2(0., 0., 0.), fa(-1), fb(-1), fPhi1(0),
    fPhi2(0), fBeta(0)
{
}

PndTrkSkewHit::PndTrkSkewHit(Int_t hitID, Int_t detID, Bool_t used, Int_t iregion, Int_t sensorID, TVector3 &pos, Double_t isochrone, Double_t sortvar, Int_t trackID,
                             TVector3 center1, TVector3 intersection1, double phi1, TVector3 center2, TVector3 intersection2, double phi2, double a, double b, int rightone,
                             double beta)
  : PndTrkHit(hitID, detID, used, iregion, sensorID, pos, isochrone, sortvar), fTrackID(trackID), fRightIntersection(rightone), fCenter1(center1), fCenter2(center2),
    fIntersection1(intersection1), fIntersection2(intersection2), fa(a), fb(b), fPhi1(phi1), fPhi2(phi2), fBeta(beta)
{
}

PndTrkSkewHit::PndTrkSkewHit(PndTrkHit &hit, Int_t trackID, TVector3 center1, TVector3 intersection1, double phi1, TVector3 center2, TVector3 intersection2, double phi2, double a,
                             double b, int rightone, double beta)
  : PndTrkHit(hit), fTrackID(trackID), fRightIntersection(rightone), fCenter1(center1), fCenter2(center2), fIntersection1(intersection1), fIntersection2(intersection2), fa(a),
    fb(b), fPhi1(phi1), fPhi2(phi2), fBeta(beta)
{
}

PndTrkSkewHit::PndTrkSkewHit(PndTrkHit &hit)
  : PndTrkHit(hit), fTrackID(-1), fRightIntersection(-1), fCenter1(0., 0., 0.), fCenter2(0., 0., 0.), fIntersection1(0., 0., 0.), fIntersection2(0., 0., 0.), fa(-1), fb(-1),
    fPhi1(0), fPhi2(0), fBeta(0)
{
}

PndTrkSkewHit::PndTrkSkewHit(const PndTrkSkewHit &hit)
  : PndTrkHit(hit), fTrackID(-1), fRightIntersection(-1), fCenter1(0., 0., 0.), fCenter2(0., 0., 0.), fIntersection1(0., 0., 0.), fIntersection2(0., 0., 0.), fa(-1), fb(-1),
    fPhi1(0), fPhi2(0), fBeta(0)
{
  *this = hit;
}

PndTrkSkewHit::~PndTrkSkewHit() {}

// operator equals
PndTrkSkewHit &PndTrkSkewHit::operator=(const PndTrkSkewHit &skewhit)
{
  fTrackID = skewhit.fTrackID;
  fRightIntersection = skewhit.fRightIntersection;
  fCenter1 = skewhit.fCenter1;
  fCenter2 = skewhit.fCenter2;
  fIntersection1 = skewhit.fIntersection1;
  fIntersection2 = skewhit.fIntersection2;
  fa = skewhit.fa;
  fb = skewhit.fb;
  fPhi1 = skewhit.fPhi1;
  fPhi2 = skewhit.fPhi2;
  fBeta = skewhit.fBeta;
  return *this;
}

void PndTrkSkewHit::Draw(Color_t color)
{
  Int_t style = 1;
  if (fUsed && color == kBlack) {
    style = 0;
    color = kGray;
  }

  TEllipse *ell1 = new TEllipse(fCenter1.X(), fCenter1.Y(), fa, fb, 0, 360, -fBeta);
  ell1->SetFillStyle(style);
  ell1->SetLineColor(color);
  ell1->Draw("SAME");

  TEllipse *ell2 = new TEllipse(fCenter2.X(), fCenter2.Y(), fa, fb, 0, 360, -fBeta);
  ell2->SetFillStyle(style);
  ell2->SetLineColor(color - 5);
  ell2->Draw("SAME");
}

void PndTrkSkewHit::Print()
{
  cout << "####### SKEW ########################" << endl;
  cout << "hitID " << fHitID << " used " << fUsed << " iregion " << fIRegion << endl;
  cout << "isochrone " << fIsochrone << endl;
  fIntersection1.Print();
  fIntersection2.Print();
}

ClassImp(PndTrkSkewHit)
