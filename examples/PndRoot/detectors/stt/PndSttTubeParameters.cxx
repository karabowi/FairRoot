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

/////////////////////////////////////////////////////////////
// PndSttTubeParameters
//
// Class for tube parameters (object)
//
// authors: Lia Lavezzi - INFN Pavia (2013)
//
/////////////////////////////////////////////////////////////

#include "PndSttTubeParameters.h"
#include "TGeoManager.h"
#include <iostream>
using std::cout;
using std::endl;

PndSttTubeParameters::PndSttTubeParameters() : fTubeID(-1), fHalfLength(0) {}

PndSttTubeParameters::PndSttTubeParameters(PndSttTubeParameters &tubs) : TObject(tubs), fTubeID(tubs.GetTubeID()), fHalfLength(tubs.GetHalfLength()) {}

PndSttTubeParameters::PndSttTubeParameters(Int_t tubeID, Double_t hl) : fTubeID(tubeID), fHalfLength(hl) {}

PndSttTubeParameters::~PndSttTubeParameters() {}

Double_t PndSttTubeParameters::GetHalfLength()
{
  return fHalfLength;
}

Int_t PndSttTubeParameters::GetTubeID()
{
  return fTubeID;
}

ClassImp(PndSttTubeParameters)
