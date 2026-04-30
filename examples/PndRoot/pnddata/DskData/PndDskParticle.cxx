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
// -----                   PndDskParticle source file                     -----
// -----                  Created 21/03/08  by P. Koch                    -----
// ----------------------------------------------------------------------------

#include <iostream>
#include "FairLogger.h"
using std::cout;
using std::endl;

#include "PndDskParticle.h"

// -----   Default constructor   ----------------------------------------------
PndDskParticle::PndDskParticle() : PndMCPoint() {}
// ----------------------------------------------------------------------------

// -----   Standard constructor   ---------------------------------------------
PndDskParticle::PndDskParticle(Int_t trackID, Int_t detectorID, TVector3 position, TVector3 momentum, Double_t time, Int_t pdgCode, TString pdgName, Double_t energy,
                               Int_t motherTrackID, Int_t motherPdgCode, TString motherPdgName, Double_t mass, Double_t angIn, Double_t thetaC, Int_t nPhot)
  : PndMCPoint(trackID, detectorID, position, TVector3(), momentum, TVector3(), time, 0., 0.), fPdgCode(pdgCode), fEnergy(energy), fMass(mass),
    fMotherTrackID(motherTrackID), fMotherPdgCode(motherPdgCode), fExitTime(0.), fExitEnergy(0.), fThetaC(thetaC), fAngIn(angIn), fNPhot(nPhot)
{
}
// ----------------------------------------------------------------------------

PndDskParticle::PndDskParticle(const PndDskParticle &particle)
  : PndMCPoint(particle.fTrackID, particle.fDetectorID, particle.GetPosition(), particle.GetPositionOut(), particle.GetMomentum(), particle.GetMomentumOut(), particle.fTime,
               particle.fLength, particle.fELoss, particle.fEventId),
    fPdgCode(particle.fPdgCode), fEnergy(particle.fEnergy), fMass(particle.fMass), fMotherTrackID(particle.fMotherTrackID),
    fMotherPdgCode(particle.fMotherPdgCode), fExitTime(particle.fExitTime), fExitEnergy(particle.fExitEnergy), fThetaC(particle.fThetaC),
    fAngIn(particle.fAngIn), fNPhot(particle.fNPhot)
{
}

// -----   Destructor   -------------------------------------------------------
PndDskParticle::~PndDskParticle() {}
// ----------------------------------------------------------------------------

// -----   Public method Print   ----------------------------------------------
void PndDskParticle::Print(const Option_t *opt) const
{
  (void)opt; // to remove "unused" warnings
  LOG(info) << " PndDskParticle: track" << fTrackID << " created in detector " << fDetectorID;
  cout << "    at Position (" << fX << ", " << fY << ", " << fZ << ") cm" << endl;
  cout << "    with Momentum (" << fPx << ", " << fPy << ", " << fPz << ") eV" << endl;
  cout << "    at Time " << fTime << " ns" << endl;
}
// ----------------------------------------------------------------------------

// -----   Public method SetFinalValues   -------------------------------------
void PndDskParticle::SetFinalValues(TVector3 exitPosition, TVector3 exitMomentum, Double_t exitTime, Double_t exitEnergy)
{
  SetPositionOut(exitPosition);
  SetMomentumOut(exitMomentum);
  fExitTime = exitTime;
  fExitEnergy = exitEnergy;
}
// ----------------------------------------------------------------------------

ClassImp(PndDskParticle)
