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

/////////////////////////////////////////////////////////////
//
//  PndHypPoint
//
//  Geant point for Hyp detector
//
//
///////////////////////////////////////////////////////////////
#include "FairLogger.h"

#include <iostream>
using std::cout;
using std::endl;
#include "THParticle.h"

// -----   Default constructor   -------------------------------------------
THParticle::THParticle() : TParticle()
{

  fEventID = -1;
  fstatus = 0;
  fIndex = 0;

  fCalcMass = 0.0;
  fA = fZ = fH = fpdgCode = 0;
  fPx = fPy = fPz = fVx = fVy = fVz = 0.;
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
THParticle::THParticle(Int_t pdg, Int_t status, Int_t evtID, Int_t index, Double_t mass, Int_t A, Int_t Z, Int_t H, const TLorentzVector &p, const TLorentzVector &v)
  : TParticle(pdg, status, 0, 0, 0, 0, p, v)
{

  fstatus = status;
  fIndex = index;
  fEventID = evtID;
  fpdgCode = pdg;
  fPx = p.Px();
  fPy = p.Py();
  fPz = p.Pz();
  fVx = v.X();
  fVy = v.Y();
  fVz = v.Z();

  fA = A;
  fZ = Z;
  fH = H;
  fCalcMass = mass;
}

// -----   Destructor   ----------------------------------------------------
THParticle::~THParticle() {}
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void THParticle::Print(const Option_t *) const
{
  LOG(info) << " THParticle: HYP fragment for track " << fpdgCode << " in event " << fEventID;
}
// -------------------------------------------------------------------------

ClassImp(THParticle)
