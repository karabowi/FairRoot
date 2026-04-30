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
// -----                      PndGemPoint source file                  -----
// -----                  Created 27/10/08  by R. Kliemt               -----
// -------------------------------------------------------------------------

#include <iostream>
#include "PndGemMCPoint.h"

#include "PndDetectorList.h"

// -----   Default constructor   -------------------------------------------
PndGemMCPoint::PndGemMCPoint() : PndMCPoint() {}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndGemMCPoint::PndGemMCPoint(Int_t trackID, Int_t detID, Int_t sensID, TVector3 posIn, TVector3 posOut, TVector3 momIn, TVector3 momOut, Double_t tof, Double_t length,
                             Double_t eLoss)
  : PndMCPoint(trackID, detID, posIn, posOut, momIn, momOut, tof, length, eLoss), fSensorId(sensID)
{
  SetLink(FairLink("MCTrack", trackID));
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndGemMCPoint::~PndGemMCPoint() {}
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void PndGemMCPoint::Print(const Option_t *opt) const
{
  std::cout << *this << " opt = " << opt << std::endl;
}
// -------------------------------------------------------------------------

ClassImp(PndGemMCPoint);
