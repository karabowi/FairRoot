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
// -----                     PndDrcEVPoint  source file                -----
// -----                  Created 16/06/2013  by H. Kumawat            -----
// -------------------------------------------------------------------------

#include "PndDrcEVPoint.h"
#include <iostream>
#include "FairLogger.h"

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndDrcEVPoint::PndDrcEVPoint() : FairMCPoint(), fPdgCode(0), fTimeStart(0.), fTimeAtEVEntrance(0.), fVeloPhoton(0.) {}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndDrcEVPoint::PndDrcEVPoint(Int_t trackID, Int_t detID, TVector3 pos, TVector3 mom, Double_t tof, Double_t length, Int_t pdgCode, Int_t eventID, Double_t timestart,
                             Double_t timeatEVentrance, Double_t VeloPhoton, TVector3 normal)
  : FairMCPoint(trackID, detID, pos, mom, tof, length, 0.0, (UInt_t)eventID), fPdgCode(pdgCode), fTimeStart(timestart), fTimeAtEVEntrance(timeatEVentrance),
    fVeloPhoton(VeloPhoton), fNormalX(normal.X()), fNormalY(normal.Y()), fNormalZ(normal.Z())
{
  SetLink(FairLink("MCTrack", trackID));
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndDrcEVPoint::~PndDrcEVPoint() {}
// -------------------------------------------------------------------------

void PndDrcEVPoint::Print(const Option_t *opt) const
{
  (void)opt; // to remove "unused" warnings
  LOG(info) << " PndDrcEVPoint: DRC Point for trackID---> " << fTrackID << " in detector " << fDetectorID;

  cout << "    Position (" << fX << ", " << fY << ", " << fZ << ") cm" << endl;
  cout << "    Momentum (" << fPx << ", " << fPy << ", " << fPz << ") GeV" << endl;
  cout << "    Time " << fTime << " ns,  Length " << fLength << " cm " << endl;
}
ClassImp(PndDrcEVPoint)
