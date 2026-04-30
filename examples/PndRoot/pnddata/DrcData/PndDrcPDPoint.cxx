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
// -----                     PndDrcPDPoint  source file                   -----
// -----                  Created 09/04/2008  by E. Fioravanti & A. Cecchi               -----
// -------------------------------------------------------------------------

#include "PndDrcPDPoint.h"
#include <iostream>
#include "FairLogger.h"

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndDrcPDPoint::PndDrcPDPoint() : FairMCPoint(), fPdgCode(0), fMcpId(0), fBarPointId(0), ftimeAtEV(0) {}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndDrcPDPoint::PndDrcPDPoint(Int_t trackID, Int_t detID, Int_t mcpId, Int_t barPointId, TVector3 pos, TVector3 mom, TVector3 momAtEV, Double_t timeAtEV, Double_t tof,
                             Double_t length, Int_t pdgCode, Int_t eventID)
    : FairMCPoint(trackID, detID, pos, mom, tof, length, 0.0, (UInt_t)eventID), fPdgCode(pdgCode), fMcpId(mcpId), fBarPointId(barPointId)
    , fmomAtEVX(momAtEV.X())
    , fmomAtEVY(momAtEV.Y())
    , fmomAtEVZ(momAtEV.Z())
    , ftimeAtEV(timeAtEV)
{
  SetLink(FairLink("MCTrack", trackID));
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndDrcPDPoint::~PndDrcPDPoint() {}
// -------------------------------------------------------------------------

void PndDrcPDPoint::Print(const Option_t *opt) const
{
  (void)opt; // to remove "unused" warnings
  LOG(info) << " PndDrcPDPoint: DRC Point for trackID---> " << fTrackID << " in detector " << fDetectorID;

  cout << "    Position (" << fX << ", " << fY << ", " << fZ << ") cm" << endl;
  cout << "    Momentum (" << fPx << ", " << fPy << ", " << fPz << ") GeV" << endl;
  cout << "    Time " << fTime << " ns,  Length " << fLength << " cm " << endl;
}
ClassImp(PndDrcPDPoint)
