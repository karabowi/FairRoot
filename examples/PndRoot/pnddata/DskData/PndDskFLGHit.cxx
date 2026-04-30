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

// ----------------------------------------------------------------------------
// -----                   PndDskFLGHit source file                     -----
// -----                  Created 21/03/08  by P. Koch                    -----
// ----------------------------------------------------------------------------

#include <iostream>
#include "FairLogger.h"
using std::cout;
using std::endl;

#include "PndDskFLGHit.h"

// -----   Default constructor   ----------------------------------------------
PndDskFLGHit::PndDskFLGHit() : PndMCPoint() {}
// ----------------------------------------------------------------------------

// -----   Standard constructor   ---------------------------------------------
PndDskFLGHit::PndDskFLGHit(Int_t trackID, Int_t detectorID, TVector3 position, TVector3 momentum, Double_t time, Int_t pdgCode, TString pdgName, Double_t energy,
                           Int_t motherTrackID, Int_t motherPdgCode, TString motherPdgName, Double_t mass, Double_t angIn, Double_t thetaC, TVector3 mom_photon,
                           Int_t num_light_guide, Int_t num_pixel)
  : PndMCPoint(trackID, detectorID, position, TVector3(), momentum, TVector3(), time, 0., 0.), fPdgCode(pdgCode), fEnergy(energy), fMass(mass),
    fMotherTrackID(motherTrackID), fMotherPdgCode(motherPdgCode), fExitTime(0.), fExitEnergy(0.), fThetaC(thetaC), fAngIn(angIn),
    fMom_CP_X(mom_photon.X()), fMom_CP_Y(mom_photon.Y()), fMom_CP_Z(mom_photon.Z()), fNum_light_guide(num_light_guide), fNum_pixel(num_pixel)
{
}
// ----------------------------------------------------------------------------
PndDskFLGHit::PndDskFLGHit(Int_t trackID, Int_t detectorID, TVector3 position, TVector3 momentum, Double_t time, Double_t angIn, Double_t thetaC, TVector3 mom_photon,
                           Int_t num_light_guide, Int_t num_pixel)
  : PndMCPoint(trackID, detectorID, position, TVector3(), momentum, TVector3(), time, 0., 0.), fPdgCode(-1), fEnergy(0.), fMass(0.), fMotherTrackID(-1),
    fMotherPdgCode(-1), fExitTime(0.), fExitEnergy(0.), fThetaC(thetaC), fAngIn(angIn), fMom_CP_X(mom_photon.X()), fMom_CP_Y(mom_photon.Y()), fMom_CP_Z(mom_photon.Z()),
    fNum_light_guide(num_light_guide), fNum_pixel(num_pixel)
{
}

// -----   Destructor   -------------------------------------------------------
PndDskFLGHit::~PndDskFLGHit() {}
// ----------------------------------------------------------------------------

// -----   Public method Print   ----------------------------------------------
void PndDskFLGHit::Print(const Option_t *opt) const
{
  (void)opt; // to remove "unused" warnings
  LOG(info) << " PndDskFLGHit: track" << fTrackID << " created in detector " << fDetectorID;
  cout << "    at Position (" << fX << ", " << fY << ", " << fZ << ") cm" << endl;
  cout << "    with Momentum (" << fPx << ", " << fPy << ", " << fPz << ") eV" << endl;
  cout << "    at Time " << fTime << " ns" << endl;
}
// ----------------------------------------------------------------------------

// -----   Public method SetFinalValues   -------------------------------------
void PndDskFLGHit::SetFinalValues(TVector3 exitPosition, TVector3 exitMomentum, Double_t exitTime, Double_t exitEnergy)
{
  SetPositionOut(exitPosition);
  SetMomentumOut(exitMomentum);
  fExitTime = exitTime;
  fExitEnergy = exitEnergy;
}
// ----------------------------------------------------------------------------

ClassImp(PndDskFLGHit)
