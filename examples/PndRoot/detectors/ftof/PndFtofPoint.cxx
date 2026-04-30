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
//  CbmFtofPoint
//
//  Geant point for Forward tof  detector
//  created by A. Sanchez
//
///////////////////////////////////////////////////////////////

#include "PndFtofPoint.h"

#include <iostream>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndFtofPoint::PndFtofPoint() : PndMCPoint() {}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndFtofPoint::PndFtofPoint(Int_t trackID, Int_t evtID, Int_t detID, TString detName, TVector3 posin, TVector3 momin, TVector3 posout, TVector3 momout, Double_t tof,
                           Double_t length, Double_t eLoss, Double_t charge, Double_t mass, Int_t pdgCode, Double_t dist, Double_t PLin, Double_t PLout)
  : PndMCPoint(trackID, detID, posin, posout, momin, momout, tof, length, eLoss, evtID), fmass(mass), fcharge(charge), fdist(dist), fDetName(detName), fVolumeID(-999),
    fpdgCode(pdgCode)
{
}

// -----   Destructor   ----------------------------------------------------
PndFtofPoint::~PndFtofPoint() {}
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void PndFtofPoint::Print(const Option_t *) const
{
  cout << "-I- PndFtofPoint: forward tof Point for track "
       << " in detector " << endl;
  cout << "    Position (" << GetPosition().X() << ", " << GetPosition().Y() << ", " << GetPosition().Z() << ") cm" << endl;
  cout << "    Momentum (" << GetMomentum().X() << ", " << GetMomentum().Y() << ", " << GetMomentum().Z() << ") GeV" << endl;
}
// -------------------------------------------------------------------------

ClassImp(PndFtofPoint)
