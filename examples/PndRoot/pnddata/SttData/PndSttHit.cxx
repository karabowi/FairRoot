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

/** CbmStsMapsHit
 *@author Michael Deveaux <m.deveaux@gsi.de>
 **Acknowledgements to M. Al-Turany, D. Bertini, G. Gaycken
 **Version beta 0.1 (02.02.2005)
 **/

#include "PndSttHit.h"

#include "PndDetectorList.h"

#include <iostream>
#include "TMath.h"

using namespace std;

/** Default constructor **/
PndSttHit::PndSttHit() : FairHit(), fTubeID(0), fHitTime(0), fPulse(0), fDist2Wire(0), fIsochrone(0), fIsochroneError(0), fIsochroneErrorLo(0), fIsochroneErrorHi(0), fDepCharge(0)
{
  Clear();
}

  Int_t fTubeID; // CHECK added
  /** hit time **/
  Double_t fHitTime;
  /** time pulse **/
  Double_t fPulse;
  /** This variable contains the true radial distance to the wire **/
  Double_t fDist2Wire;
  /** This variable contains the radial distance to the wire **/
  Double_t fIsochrone;
  /** This variable contains the error on the radial distance to the wire **/
  Double_t fIsochroneError;
  Double_t fIsochroneErrorLo;
  Double_t fIsochroneErrorHi;
  /**  deposit charge (arbitrary units) **/
  Double_t fDepCharge;


PndSttHit::PndSttHit(Int_t detID, Int_t tubeID, Int_t mcindex, TVector3 &pos, TVector3 &dpos, Double_t p, Double_t isochrone, Double_t isochroneError, Double_t chDep)
  : FairHit(detID, pos, dpos, mcindex), fTubeID(tubeID), fHitTime(0), fPulse(p), fDist2Wire(0), 
    fIsochrone(isochrone), fIsochroneError(isochroneError), fIsochroneErrorLo(isochroneError), fIsochroneErrorHi(isochroneError), fDepCharge(chDep)
{
  SetTimeStamp(p);

  SetLink(FairLink("STTPoint", mcindex));
}

/** Public method Clear **/
void PndSttHit::Clear()
{
  fTubeID = 0;
  fHitTime = 0;
  fPulse = 0;
  fDist2Wire = 0;
  fIsochrone = 0.;
  fIsochroneError = 0.;
  fIsochroneErrorLo = 0.;
  fIsochroneErrorHi = 0.;
  fDepCharge = 0.;
}

/** Destructor **/
PndSttHit::~PndSttHit() {}

Double_t PndSttHit::ComputedEdx(PndTrack *track, Double_t tuberadius)
{
  // cout << "tuberadius/isochrone " << tuberadius << " " << fIsochrone << endl;

  Double_t distance = 2 * sqrt(tuberadius * tuberadius - fIsochrone * fIsochrone); // cm
  //  cout << "tuberadius/isochrone/distance " << tuberadius << " " << fIsochrone << " " << distance << endl;

  TVector3 momentum = track->GetParamFirst().GetMomentum();
  // Double_t pt = momentum.Perp();
  // Double_t pl = momentum.Z();

  Double_t coslam = momentum.Perp() / momentum.Mag();

  distance = distance / coslam;
  //   cout << "depcharge " << fDepCharge << endl;;
  Double_t dedx = 0.;
  if (distance != 0)
    dedx = fDepCharge / (1000000 * distance); // in arbitrary units
                                              //   cout << "cosla/distance2/dedx " << coslam << " " << distance/coslam << " " << dedx << endl;

  return dedx;
}

ClassImp(PndSttHit)
