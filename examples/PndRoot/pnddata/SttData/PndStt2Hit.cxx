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
 
#include "PndStt2Hit.h"

#include "PndDetectorList.h"

#include <iostream>
#include "TMath.h"

using namespace std;

/** Default constructor **/
PndStt2Hit::PndStt2Hit() : FairHit()
{
  Clear();
}

PndStt2Hit::PndStt2Hit(Int_t mcindex, Int_t detID, Int_t tubeID, TVector3 &pos, TVector3 &dpos, Double_t thit, Double_t dist2wire, Double_t isorad, Double_t isoraderrlo, Double_t isoraderrhi, Double_t chDep)
  : FairHit(detID, pos, dpos, mcindex), fTubeID(tubeID), fHitTime(thit), fDist2Wire(dist2wire), fIsoRad(isorad), fIsoRadErrLo(isoraderrlo), fIsoRadErrHi(isoraderrhi), fDepCharge(chDep)
{
  SetTimeStamp(thit);
  fIsoRadError = 0.5*(isoraderrlo+isoraderrhi);

  SetLink(FairLink("STTPoint", mcindex));
}

/** Public method Clear **/
void PndStt2Hit::Clear()
{
  //fMcIndex = -1;
  fTubeID = -1;
  fHitTime = 0;
  fDist2Wire = 0.;
  fIsoRad = 0.;
  fIsoRadError = 0.;
  fIsoRadErrLo = 0.;
  fIsoRadErrHi = 0.;
  fDepCharge = 0.;
}

/** Destructor **/
PndStt2Hit::~PndStt2Hit() {}

//Double_t PndStt2Hit::ComputedEdx(PndTrack *track, Double_t tuberadius)
//{

  //// cout << "tuberadius/isochrone " << tuberadius << " " << fIsochrone << endl;

  //Double_t distance = 2 * sqrt(tuberadius * tuberadius - fIsoRad * fIsoRad); // cm
  ////  cout << "tuberadius/isochrone/distance " << tuberadius << " " << fIsochrone << " " << distance << endl;

  //TVector3 momentum = track->GetParamFirst().GetMomentum();
  //// Double_t pt = momentum.Perp();
  //// Double_t pl = momentum.Z();

  //Double_t coslam = momentum.Perp() / momentum.Mag();

  //distance = distance / coslam;
  ////   cout << "depcharge " << fDepCharge << endl;;
  //Double_t dedx = 0.;
  //if (distance != 0)
    //dedx = fDepCharge / (1000000 * distance); // in arbitrary units
                                              ////   cout << "cosla/distance2/dedx " << coslam << " " << distance/coslam << " " << dedx << endl;

  //return dedx;
//}

ClassImp(PndStt2Hit)
