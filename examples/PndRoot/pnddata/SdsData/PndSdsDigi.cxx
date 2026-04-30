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

#include "PndSdsDigi.h"

PndSdsDigi::PndSdsDigi() : fIndex(), fDetID(-1), fSensorID(-1), fFE(-1), fCharge(-1.) {}

PndSdsDigi::PndSdsDigi(std::vector<Int_t> index, Int_t detID, Int_t sensorID, Int_t fe, Double_t charge, Double_t timestamp)
  : FairTimeStamp(timestamp), fIndex(), fDetID(detID), fSensorID(sensorID), fFE(fe), fCharge(charge)
{
  AddIndex(index);
}

PndSdsDigi::PndSdsDigi(Int_t index, Int_t detID, Int_t sensorID, Int_t fe, Double_t charge, Double_t timestamp)
  : FairTimeStamp(timestamp), fIndex(), fDetID(detID), fSensorID(sensorID), fFE(fe), fCharge(charge)
{
  AddIndex(index);
}

ClassImp(PndSdsDigi);
