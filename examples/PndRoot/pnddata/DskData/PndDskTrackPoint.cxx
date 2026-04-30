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
// -----                  PndDskTrackPoint source file                    -----
// -----                 Created 27/03/09  by P. Koch                     -----
// ----------------------------------------------------------------------------

#include <iostream>
#include "FairLogger.h"
using std::cout;
using std::endl;

#include "PndDskTrackPoint.h"

// -----   Default constructor   ----------------------------------------------
PndDskTrackPoint::PndDskTrackPoint() : FairMCPoint() {}
// ----------------------------------------------------------------------------

// -----   Standard constructor   ---------------------------------------------
PndDskTrackPoint::PndDskTrackPoint(Int_t trackID, Int_t detectorID, TVector3 position, TVector3 momentum, Double_t time, Double_t length, Double_t eLoss)
  : FairMCPoint(trackID, detectorID, position, momentum, time, length, eLoss)
{
}
// ----------------------------------------------------------------------------

// -----   Destructor   -------------------------------------------------------
PndDskTrackPoint::~PndDskTrackPoint() {}
// ----------------------------------------------------------------------------

// -----   Public method Print   ----------------------------------------------
void PndDskTrackPoint::Print(const Option_t *opt) const
{
  (void)opt; // to remove "unused" warnings
  LOG(info) << " PndDskTrackPoint: track" << fTrackID << " created in detector " << fDetectorID;
  cout << "    at Position (" << fX << ", " << fY << ", " << fZ << ") cm" << endl;
  cout << "    with Momentum (" << fPx << ", " << fPy << ", " << fPz << ") eV" << endl;
  cout << "    at Time " << fTime << " ns" << endl;
}
// ----------------------------------------------------------------------------

ClassImp(PndDskTrackPoint)
