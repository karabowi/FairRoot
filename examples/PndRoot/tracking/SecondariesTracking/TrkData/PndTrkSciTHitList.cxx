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

//
// PndTrkSciTHitList.cxx
//
//
// authors: Lia Lavezzi - INFN Pavia (2012)
//

#include "PndTrkSciTHitList.h"

#include "PndDetectorList.h"

#include <iostream>

using namespace std;

// ------------------ instance --------------------------
PndTrkSciTHitList *PndTrkSciTHitList::fscitInstance = nullptr;

PndTrkSciTHitList *PndTrkSciTHitList::Instance()
{
  if (!fscitInstance)
    cout << "NO PndTrkSciTHitList: you must fill sciT hit list BEFORE doing what you are doing!" << endl;
  return fscitInstance;
}

PndTrkSciTHitList *PndTrkSciTHitList::Instanciate()
{
  fscitInstance = this;
  return fscitInstance;
}

// ------------------ instance ---------------------------

PndTrkSciTHitList::PndTrkSciTHitList() : PndTrkHitList(), fIRegion(-1) {}

PndTrkSciTHitList::PndTrkSciTHitList(Int_t iregion) : PndTrkHitList(), fIRegion(iregion) {}

PndTrkSciTHitList::~PndTrkSciTHitList()
{
  fscitInstance = nullptr;
}

void PndTrkSciTHitList::AddHit(Int_t hitid, Int_t detid, FairHit *hit)
{
  TVector3 position;
  hit->Position(position);

  PndTrkHitList::AddHit(hitid, detid, 0, SCIT, position); // CHECK iregion
}

ClassImp(PndTrkSciTHitList)
