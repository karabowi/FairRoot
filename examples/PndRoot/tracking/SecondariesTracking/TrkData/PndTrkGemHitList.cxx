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
// PndTrkGemHitList.cxx
//
//
// authors: Lia Lavezzi - INFN Pavia (2012)
//

#include "PndTrkGemHitList.h"

#include "PndDetectorList.h"

#include <iostream>

using namespace std;

// ------------------ instance --------------------------
PndTrkGemHitList *PndTrkGemHitList::fgemInstance = nullptr;

PndTrkGemHitList *PndTrkGemHitList::Instance()
{
  if (!fgemInstance)
    cout << "NO PndTrkGemHitList: you must fill gem hit list BEFORE doing what you are doing!" << endl;
  return fgemInstance;
}

PndTrkGemHitList *PndTrkGemHitList::Instanciate()
{
  fgemInstance = this;
  return fgemInstance;
}

// ------------------ instance ---------------------------

PndTrkGemHitList::PndTrkGemHitList() : PndTrkHitList(), fIRegion(-1) {}

PndTrkGemHitList::PndTrkGemHitList(Int_t iregion) : PndTrkHitList(), fIRegion(iregion) {}

PndTrkGemHitList::~PndTrkGemHitList()
{
  fgemInstance = nullptr;
}

void PndTrkGemHitList::AddHit(Int_t hitid, Int_t detid, FairHit *hit)
{
  TVector3 position;
  hit->Position(position);
  int station = ((PndGemHit *)hit)->GetStationNr();
  int sensor = ((PndGemHit *)hit)->GetSensorNr();

  // layerid = 0, 1, 2, ...
  int layerid = 2 * station + sensor - 3;

  PndTrkHitList::AddHit(hitid, detid, 0, GEM, layerid, position); // CHECK iregion
}

void PndTrkGemHitList::AddNonCombiHits(Int_t detID, TClonesArray *array, std::map<int, bool> hitTousable)
{
  for (int ihit = 0; ihit < array->GetEntriesFast(); ihit++) {
    PndGemHit *hit = (PndGemHit *)array->At(ihit);
    if (hitTousable[ihit] == false)
      continue;
    AddHit(ihit, detID, hit);
  }
}

ClassImp(PndTrkGemHitList)
