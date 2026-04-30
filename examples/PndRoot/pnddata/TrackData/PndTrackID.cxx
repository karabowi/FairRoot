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

//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTrackID
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      01/07/09 - Stefano Spataro (Torino)
//
//
//-----------------------------------------------------------

// Panda Headers ----------------------
#include <iostream>

// This Class' Header ------------------
#include "PndTrackID.h"

using std::cout;
using std::endl;

ClassImp(PndTrackID);

PndTrackID::PndTrackID() : fTrackID(-1), fCorrTrackIds(0), fMultTrackIds(0) {}

PndTrackID::PndTrackID(Int_t id, TArrayI track, TArrayI mult) : fTrackID(id), fCorrTrackIds(track), fMultTrackIds(mult) {}

PndTrackID::~PndTrackID() {}

void PndTrackID::Reset()
{
  fCorrTrackIds.Reset();
  fMultTrackIds.Reset();
}

void PndTrackID::Print()
{
  std::cout << "PndTrackID::Print() - PndTrackID: " << fTrackID << "\tNumber of correlated MCTrack ids: " << GetNCorrTrackId() << std::endl;
  for (Int_t ii = 0; ii < GetNCorrTrackId(); ++ii) {
    std::cout << " *** At: " << ii << "\t MCTrack ID: " << fCorrTrackIds[ii] << "\t Multiplicity: " << fMultTrackIds[ii] << std::endl;
  }
}
