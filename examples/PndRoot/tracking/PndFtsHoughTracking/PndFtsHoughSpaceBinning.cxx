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

#include "PndFtsHoughSpaceBinning.h"

ClassImp(PndFtsHoughSpaceBinning)
  // -----   Default constructor   -------------------------------------------

  PndFtsHoughSpaceBinning::PndFtsHoughSpaceBinning()
  : fNBinsTheta(0), fThetaRadLow(0.), fThetaRadHigh(0.),

    fNBinsY(0), fYLow(0.), fYHigh(0.)
{
}

PndFtsHoughSpaceBinning::PndFtsHoughSpaceBinning(Int_t nBinsThetaVal, Double_t thetaRadLowVal, Double_t thetaRadHighVal,

                                                 Int_t nBinsYVal, Double_t yLowVal, Double_t yHighVal)
  : fNBinsTheta(nBinsThetaVal), fThetaRadLow(thetaRadLowVal), fThetaRadHigh(thetaRadHighVal),

    fNBinsY(nBinsYVal), fYLow(yLowVal), fYHigh(yHighVal)
{
}

PndFtsHoughSpaceBinning::PndFtsHoughSpaceBinning(Int_t stepsPerThetaDeg, Double_t thetaDegLowHighVal,

                                                 Int_t nBinsYVal, Double_t yLowHighVal)
  : fNBinsTheta(stepsPerThetaDeg * 2 * thetaDegLowHighVal), fThetaRadLow(-1 * thetaDegLowHighVal * TMath::DegToRad()), fThetaRadHigh(thetaDegLowHighVal * TMath::DegToRad()),

    fNBinsY(nBinsYVal), fYLow(-1 * yLowHighVal), fYHigh(yLowHighVal)
{
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndFtsHoughSpaceBinning::~PndFtsHoughSpaceBinning() {}
// -------------------------------------------------------------------------
