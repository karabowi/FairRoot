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


#include <iostream>
#include "FairLogger.h"

using std::cout;
using std::endl;

#include "PndMdtPoint.h"
#include "PndDetectorList.h"

// -----   Default constructor   -------------------------------------------
PndMdtPoint::PndMdtPoint() : PndMCPoint() {}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndMdtPoint::PndMdtPoint(Int_t trackID, Int_t detID, TVector3 pos, TVector3 mom, Double_t tof, Double_t length, Double_t eLoss, TVector3 pos_in, TVector3 mom_in)
  : PndMCPoint(trackID, detID, pos_in, pos, mom_in, mom, tof, length, eLoss)
{
  SetLink(FairLink("MCTrack", trackID));
}

// -----   Destructor   ----------------------------------------------------
PndMdtPoint::~PndMdtPoint() {}
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void PndMdtPoint::Print(const Option_t *opt) const
{
  LOG(info) << " PndMdtPoint: MDT Point for track " << fTrackID << " in the detector " << fDetectorID;
  cout << " in event " << fEventId << " track length " << fLength << " time " << fTime << " eloss " << fELoss << endl;
  cout << "    Position In (" << GetPosition().X() << ", " << GetPosition().Y() << ", " << GetPosition().Z() << ") cm^3*s" << endl;
  cout << "    Momentum In (" << GetMomentum().X() << ", " << GetMomentum().Y() << ", " << GetMomentum().Z() << ") GeV^4" << endl;
  cout << "    Position Out (" << GetPositionOut().X() << ", " << GetPositionOut().Y() << ", " << GetPositionOut().Z() << ") cm^3*s" << endl;
  cout << "    Momentum Out(" << GetMomentumOut().X() << ", " << GetMomentumOut().Y() << ", " << GetMomentumOut().Z() << ") GeV^4" << endl;
  cout << " opt = " << opt << endl;
}
// -------------------------------------------------------------------------

ClassImp(PndMdtPoint)
