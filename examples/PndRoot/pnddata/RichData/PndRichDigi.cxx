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
// -----                       PndRichDigi source file                -----
// -----               Created 01/11/14  by Beloborodov Konstantin     -----
// -----                                                               -----
// -----                                                               -----
// -------------------------------------------------------------------------
#include <iostream>

using std::cout;
using std::endl;

#include "PndRichDigi.h"

// -----   Default constructor   -------------------------------------------
PndRichDigi::PndRichDigi() : FairTimeStamp(0), fSensorId(-1), fTime(-999.), fTimeThreshold(-999.)
{
  SetTimeStamp(0);
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------

PndRichDigi::PndRichDigi(Int_t index,
                         Int_t, // detID, //[R.K. 04/2017] unused parameter
                         Int_t sensorId, TVector3 &pos,
                         TVector3 &, // dpos, //[R.K. 04/2017] unused parameter
                         Double_t time, Double_t timeThreshold, Double_t timeStamp)
  : FairTimeStamp(timeStamp), fSensorId(sensorId), fTime(time), fTimeThreshold(timeThreshold)
{
  AddIndex(index);
  fX = pos.X();
  fY = pos.Y();
  fZ = pos.Z();
}

PndRichDigi::PndRichDigi(std::vector<Int_t> index,
                         Int_t, // detID,  //[R.K. 04/2017] unused parameter
                         Int_t sensorId, TVector3 &pos,
                         TVector3 &, // dpos, //[R.K. 04/2017] unused parameter
                         Double_t time, Double_t timeThreshold, Double_t timeStamp)
  : FairTimeStamp(timeStamp), fSensorId(sensorId), fTime(time), fTimeThreshold(timeThreshold)
{
  AddIndex(index);
  fX = pos.X();
  fY = pos.Y();
  fZ = pos.Z();
}

// -----   Destructor   ----------------------------------------------------
PndRichDigi::~PndRichDigi() {}
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void PndRichDigi::Print(const Option_t *opt) const
{
  cout << "RICH Photon hit: Time = " << fTime << " opt=" << opt << endl;
}
// -------------------------------------------------------------------------

ClassImp(PndRichDigi)
