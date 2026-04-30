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

// -------------------------------------------------------------------------
// -----                     PndRichBarPoint  source file                   -----
// -----                  Created 09/04/2008  by E. Fioravanti & A. Cecchi               -----
// -------------------------------------------------------------------------

#include "PndRichBarPoint.h"
#include <iostream>
#include "FairLogger.h"

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndRichBarPoint::PndRichBarPoint()
  : FairMCPoint(), fPdgCode(0), fThetaC(0.), fMass(0.), fTrackStatus(0), fTrackPos0(0., 0., 0.), fTrackMom0(0., 0., 0.), fTime0(-1.), fAxisX(0., 0., 0.), fAxisY(0., 0., 0.),
    fAxisZ(0., 0., 0.)
{
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndRichBarPoint::PndRichBarPoint(Int_t trackID, Int_t detID, TVector3 pos, TVector3 mom, Double_t tof, Double_t length, Int_t pdgCode, Double_t thetaC, Int_t eventID,
                                 Double_t mass)
  : FairMCPoint(trackID, detID, pos, mom, tof, length, 0.0, (UInt_t)eventID), fPdgCode(pdgCode), fThetaC(thetaC), fMass(mass), fTrackStatus(0), fTrackPos0(0., 0., 0.),
    fTrackMom0(0., 0., 0.), fTime0(-1.), fAxisX(0., 0., 0.), fAxisY(0., 0., 0.), fAxisZ(0., 0., 0.)
{
}
// -------------------------------------------------------------------------

// -----   Standard constructor  2 ------------------------------------------
PndRichBarPoint::PndRichBarPoint(Int_t trackID, Int_t detID, TVector3 pos, TVector3 mom, Double_t tof, Double_t length, Int_t pdgCode, Double_t thetaC, Int_t eventID,
                                 Double_t mass, TVector3 pos0, TVector3 mom0)
  : FairMCPoint(trackID, detID, pos, mom, tof, length, 0.0, (UInt_t)eventID), fPdgCode(pdgCode), fThetaC(thetaC), fMass(mass), fTrackStatus(0), fTrackPos0(pos0), fTrackMom0(mom0),
    fTime0(-1.), fAxisX(0., 0., 0.), fAxisY(0., 0., 0.), fAxisZ(0., 0., 0.)
{
}
// -------------------------------------------------------------------------

PndRichBarPoint::PndRichBarPoint(TVector3 pos, TVector3 dir, Double_t time)
  : FairMCPoint(), fPdgCode(0), fThetaC(0.), fMass(0.), fTrackStatus(0), fTrackPos0(pos), fTrackMom0(0., 0., 0.), fTime0(time), fAxisX(0., 0., 0.), fAxisY(0., 0., 0.),
    fAxisZ(0., 0., 0.)
{
  SetMomentum0(dir);
}

void PndRichBarPoint::SetMomentum0(TVector3 dir)
{
  if (fTrackMom0 != dir) {
    fTrackMom0 = dir;
    TVector3 axis = TVector3(0, 0, 1);
    fAxisZ = dir.Unit();
    fAxisX = axis - fAxisZ * (fAxisZ * axis);
    if (fAxisX.Mag())
      fAxisX = fAxisX.Unit();
    else
      fAxisX = TVector3(1, 0, 0);
    fAxisY = (fAxisZ.Cross(fAxisX)).Unit();
  }
}

// -----   Destructor   ----------------------------------------------------
PndRichBarPoint::~PndRichBarPoint() {}
// -------------------------------------------------------------------------

void PndRichBarPoint::Print(const Option_t *opt) const
{
  (void)opt; // to remove "unused" warnings
  LOG(info) << " PndRichPoint: DRC Point for trackID---> " << fTrackID << " in detector " << fDetectorID;

  cout << "    Position (" << fX << ", " << fY << ", " << fZ << ") cm" << endl;
  cout << "    Momentum (" << fPx << ", " << fPy << ", " << fPz << ") GeV" << endl;
  cout << "    Time " << fTime << " ns,  Length " << fLength << " cm " << endl;
}
ClassImp(PndRichBarPoint)
