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

/*
 * PndMCPoint.cxx
 *
 *  Created on: 15.11.2022
 *      Author: tstockmanns
 */

#include "PndMCPoint.h"

ClassImp(PndMCPoint);

PndMCPoint::PndMCPoint() : FairMCPoint() {}

PndMCPoint::~PndMCPoint() {}

PndMCPoint::PndMCPoint(Int_t trackID, Int_t detID, TVector3 pos, TVector3 posOut, TVector3 mom, TVector3 momOut, Double_t tof, Double_t length, Double_t eLoss, UInt_t EventId)
  : FairMCPoint(trackID, detID, pos, mom, tof, length, eLoss, EventId), fXOut(posOut.X()), fYOut(posOut.Y()), fZOut(posOut.Z()), fPxOut(momOut.X()), fPyOut(momOut.Y()),
    fPzOut(momOut.Z())
{
}
