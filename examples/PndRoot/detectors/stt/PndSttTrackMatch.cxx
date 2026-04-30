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
// -----                    PndSttTrackMatch source file               -----
// -----                  Created 30/03/06  by R. Castelijns           -----
// -------------------------------------------------------------------------

#include "PndSttTrackMatch.h"

// -----   Default constructor   -------------------------------------------
PndSttTrackMatch::PndSttTrackMatch()
{
  fMCTrackID = -1;
  fNofTrueHits = 0;
  fNofWrongHits = 0;
  fNofFakeHits = 0;
  fNofMCTracks = 0;
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndSttTrackMatch::PndSttTrackMatch(Int_t mcTrackID, Int_t nTrue, Int_t nWrong, Int_t nFake, Int_t nTracks)
{
  fMCTrackID = mcTrackID;
  fNofTrueHits = nTrue;
  fNofWrongHits = nWrong;
  fNofFakeHits = nFake;
  fNofMCTracks = nTracks;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndSttTrackMatch::~PndSttTrackMatch() {}
// -------------------------------------------------------------------------

ClassImp(PndSttTrackMatch)
