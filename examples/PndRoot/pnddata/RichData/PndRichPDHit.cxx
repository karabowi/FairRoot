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
// -----                       PndRichPDHit source file                -----
// -----               Created 01/11/14  by Beloborodov Konstantin     -----
// -----                                                               -----
// -----                                                               -----
// -------------------------------------------------------------------------
#include <iostream>

using std::cout;
using std::endl;

#include "PndRichPDHit.h"

// -----   Default constructor   -------------------------------------------
PndRichPDHit::PndRichPDHit() : FairHit(), fIndex(-1), fSensorId(-1), fTime(-999.), fTimeThreshold(-999.) {}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------

PndRichPDHit::PndRichPDHit(Int_t index, Int_t detID, Int_t sensorId, TVector3 &pos, TVector3 &dpos, Double_t time, Double_t timeThreshold)
  : FairHit(detID, pos, dpos, index), fIndex(index), fSensorId(sensorId), fTime(time), fTimeThreshold(timeThreshold)
{
  fX = pos.X();
  fY = pos.Y();
  fZ = pos.Z();
}

// -----   Destructor   ----------------------------------------------------
PndRichPDHit::~PndRichPDHit() {}
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void PndRichPDHit::Print(const Option_t *opt) const
{
  cout << "RICH Photon hit: Time = " << fTime << " opt=" << opt << endl;
}
// -------------------------------------------------------------------------

ClassImp(PndRichPDHit)
