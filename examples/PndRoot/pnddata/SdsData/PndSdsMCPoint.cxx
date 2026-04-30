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
// -----                      PndSdsMCPoint source file                  -----
// -------------------------------------------------------------------------

#include <iostream>
#include "PndSdsMCPoint.h"
#include "FairRun.h"
#include "FairEventHeader.h"

// -----   Default constructor   -------------------------------------------
PndSdsMCPoint::PndSdsMCPoint() : PndMCPoint(), fSensorID(-1) {}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndSdsMCPoint::PndSdsMCPoint(Int_t trackID, Int_t detID, Int_t sensorID, TVector3 posIn, TVector3 posOut, TVector3 momIn, TVector3 momOut, Double_t tof, Double_t length,
                             Double_t eLoss)
  : PndMCPoint(trackID, detID, posIn, posOut, momIn, momOut, tof, length, eLoss), fSensorID(sensorID)
{
  // FIXME: Do we really need the header request?
  // FairEventHeader* evtHeader = FairRun::Instance()->GetEventHeader();
  SetLink(FairLink("MCTrack", trackID));
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndSdsMCPoint::~PndSdsMCPoint() {}
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void PndSdsMCPoint::Print(const Option_t *opt) const
{
  std::cout << *this << "opt=" << opt << std::endl;
}
// -------------------------------------------------------------------------

ClassImp(PndSdsMCPoint);
