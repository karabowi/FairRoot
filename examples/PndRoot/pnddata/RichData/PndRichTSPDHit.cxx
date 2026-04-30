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
// -----                       PndRichTSPDHit source file              -----
// -----               Created 12/03/17  by Beloborodov Konstantin     -----
// -----                                                               -----
// -----                                                               -----
// -------------------------------------------------------------------------
#include <iostream>

using std::cout;
using std::endl;

#include "PndRichTSPDHit.h"

// -----   Default constructor   -------------------------------------------
PndRichTSPDHit::PndRichTSPDHit() : FairTimeStamp(), fSensorId(-1), fTime(-999.), fTimeThreshold(-999.) {}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------

PndRichTSPDHit::PndRichTSPDHit(Int_t detID, Int_t sensorId, TVector3 pos, TVector3 dpos, Double_t time, Double_t timeThreshold)
  : FairTimeStamp(), fDetID(detID), fSensorId(sensorId), fPos(pos), fdPos(dpos), fTime(time), fTimeThreshold(timeThreshold)
{
}

// -----   Destructor   ----------------------------------------------------
PndRichTSPDHit::~PndRichTSPDHit() {}
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void PndRichTSPDHit::Print(const Option_t *opt) const
{
  cout << "RICH Photon hit: Time = " << fTime << " opt=" << opt << endl;
}
// -------------------------------------------------------------------------

ClassImp(PndRichTSPDHit)
