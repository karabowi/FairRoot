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

// -----------------------------------------
// PndDrcPhotonInfo.h
//
// Created on: 18.10.2013
// Author  : R.Dzhygadlo at gsi.de
// -----------------------------------------

#include "PndDrcPhotonInfo.h"

ClassImp(PndDrcPhotonInfo)

  // -----   Default constructor   -------------------------------------------
  PndDrcPhotonInfo::PndDrcPhotonInfo()
  : TObject(), fAmbiguityArray(), fAmbiguitySize(0), fHitTime(0), fReflected(kFALSE), fEvReflections(0), fMcPrimeMomentumInBar(TVector3()), fMcCherenkovInBar(0)
{
}

PndDrcPhotonInfo::~PndDrcPhotonInfo()
{
  // for(std::vector<PndDrcAmbiguityInfo*>::iterator it = fAmbiguityArray.begin(); it != fAmbiguityArray.end(); it++){
  //   delete *it;
  // }
  // fAmbiguityArray.clear();
}

void PndDrcPhotonInfo::AddAmbiguity(PndDrcAmbiguityInfo ambiguity)
{
  fAmbiguityArray.push_back(ambiguity);
  fAmbiguitySize++;
}
