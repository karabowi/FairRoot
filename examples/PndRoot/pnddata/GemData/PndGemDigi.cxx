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

//* $Id:

// -------------------------------------------------------------------------
// -----                      PndGemDigi source file                   -----
// -------------------------------------------------------------------------
#include "PndGemDigi.h"

#include <iostream>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndGemDigi::PndGemDigi() : fDetectorId(0), fChannelNr(0), fDigiCharge(0.), fDigiCor(0.0) {}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndGemDigi::PndGemDigi(Int_t iDetectorId, Double_t iChannel, Int_t index) : fDetectorId(iDetectorId), fChannelNr(iChannel), fDigiCharge(0.), fDigiCor(0.0)
{
  AddIndex(index);
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndGemDigi::PndGemDigi(Int_t iDetectorId, Double_t iChannel, Int_t index, Double_t signal, Double_t time)
  : fDetectorId(iDetectorId), fChannelNr(iChannel), fDigiCharge(signal), fDigiCor(0.0)
{
  AddIndex(index);
  SetTimeStamp(time);
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndGemDigi::~PndGemDigi() {}
// -------------------------------------------------------------------------

ClassImp(PndGemDigi)
