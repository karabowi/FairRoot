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

#include "PndRichPoint.h"
#include "FairLogger.h"

#include <iostream>
using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndRichPoint::PndRichPoint() : FairMCPoint() {}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndRichPoint::PndRichPoint(Int_t trackID, Int_t detID, TVector3 pos, TVector3 mom, Double_t tof, Double_t length, Double_t eLoss)
  : FairMCPoint(trackID, detID, pos, mom, tof, length, eLoss)
{
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndRichPoint::~PndRichPoint() {}
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void PndRichPoint::Print(const Option_t *opt) const
{
  LOG(info) << " PndRichPoint: PndRich point for track " << fTrackID << " in detector " << fDetectorID;
  cout << "    Position (" << fX << ", " << fY << ", " << fZ << ") cm" << endl;
  cout << "    Momentum (" << fPx << ", " << fPy << ", " << fPz << ") GeV" << endl;
  cout << "    Time " << fTime << " ns,  Length " << fLength << " cm,  Energy loss " << fELoss * 1.0e06 << " keV" << endl;
}
// -------------------------------------------------------------------------

ClassImp(PndRichPoint)
