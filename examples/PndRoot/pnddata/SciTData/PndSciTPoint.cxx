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
//  PndSciTPoint
//
//  MC point for Barrel tof  detector
//  created by A. Sanchez
//  modified by D. Steinschaden
//  last update  04.2015/
///////////////////////////////////////////////////////////////

#include "PndSciTPoint.h"
#include "FairEventHeader.h"
#include "FairRun.h"

#include <iostream>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndSciTPoint::PndSciTPoint() : PndMCPoint(), fDetName("") {}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndSciTPoint::PndSciTPoint(Int_t eventID, Int_t trackID, Int_t detectorID, TString detName, TVector3 posin, TVector3 momin, TVector3 posout, TVector3 momout, Double_t tof,
                           Double_t length, Double_t eLoss)
  : PndMCPoint(trackID, detectorID, posin, posout, momin, momout, tof, length, eLoss, (UInt_t)eventID), fDetName(detName)
{
  // FIXME: Do we need to call the event header here?
  // FairEventHeader* evtHeader = FairRun::Instance()->GetEventHeader();
  SetLink(FairLink(-1, eventID, "MCTrack", trackID));
}

// -----   Destructor   ----------------------------------------------------
PndSciTPoint::~PndSciTPoint() {}
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void PndSciTPoint::Print(const Option_t *opt) const
{
  cout << "-I- PndSciTPoint: forward tof Point for track "
       << " in detector " << endl;
  cout << "    Position (" << GetPosition().X() << ", " << GetPosition().Y() << ", " << GetPosition().Z() << ") cm" << endl;
  cout << "    Momentum (" << GetMomentum().X() << ", " << GetMomentum().Y() << ", " << GetMomentum().Z() << ") GeV"
       << " opt=" << opt << endl;
}
// -------------------------------------------------------------------------

ClassImp(PndSciTPoint)
