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
// -----                       PndDrcHit source file                   -----
// -----               Created 12/11/07  by Annalisa Cecchi            -----
// -----                                                               -----
// -----                                                               -----
// -------------------------------------------------------------------------
#include <iostream>

using std::cout;
using std::endl;

#include "PndDrcHit.h"

// -----   Default constructor   -------------------------------------------
PndDrcHit::PndDrcHit() : FairHit(), fSensorId(-999), fThetaC(-999.), fErrThetaC(-999.) {}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------

PndDrcHit::PndDrcHit(Int_t detID, Int_t sensorId, TVector3 &pos, TVector3 &dpos, Double_t thetaC, Double_t errThetaC, Int_t index)
  : FairHit(detID, pos, dpos, index), fSensorId(sensorId), fThetaC(thetaC), fErrThetaC(errThetaC)
{
}

// -----   Destructor   ----------------------------------------------------
PndDrcHit::~PndDrcHit() {}
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void PndDrcHit::Print(const Option_t *opt) const
{
  (void)opt; // to remove "unused" warnings
  cout << "DIRC hit: ThetaC = " << fThetaC << endl;
}
// -------------------------------------------------------------------------

ClassImp(PndDrcHit)
