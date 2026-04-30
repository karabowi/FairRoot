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
// PndTrkSdsHitList.cxx
//
//
// authors: Lia Lavezzi - INFN Pavia (2012)
//

#include "PndTrkSdsHitList.h"

#include "PndDetectorList.h"

#include <iostream>

using namespace std;

// ------------------ instance --------------------------
PndTrkSdsHitList *PndTrkSdsHitList::fpixInstance = nullptr;
PndTrkSdsHitList *PndTrkSdsHitList::fstrInstance = nullptr;

PndTrkSdsHitList *PndTrkSdsHitList::Instance()
{
  if (!fpixInstance)
    cout << "NO PndTrkSdsHitList: you must fill pixel list it BEFORE doing what you are doing!" << endl;
  return fpixInstance;
}

PndTrkSdsHitList *PndTrkSdsHitList::Instanciate()
{
  fpixInstance = this;
  return fpixInstance;
}

PndTrkSdsHitList *PndTrkSdsHitList::InstancePixel()
{
  if (!fpixInstance)
    cout << "NO PndTrkSdsHitList: you must fill pixel list it BEFORE doing what you are doing!" << endl;
  return fpixInstance;
}

PndTrkSdsHitList *PndTrkSdsHitList::InstanciatePixel()
{
  if (fpixInstance)
    cout << "you are instanciating a pixel PndTrkSdsHitList, but you already did it, what are you doing?" << endl;
  fpixInstance = this;
  return fpixInstance;
}

PndTrkSdsHitList *PndTrkSdsHitList::InstanceStrip()
{
  if (!fstrInstance)
    cout << "NO PndTrkSdsHitList: you must fill strip list it BEFORE doing what you are doing!" << endl;
  return fstrInstance;
}

PndTrkSdsHitList *PndTrkSdsHitList::InstanciateStrip()
{
  if (fstrInstance)
    cout << "you are instanciating a strip PndTrkSdsHitList, but you already did it, what are you doing?" << endl;
  fstrInstance = this;
  return fstrInstance;
}

// ------------------ instance ---------------------------

PndTrkSdsHitList::PndTrkSdsHitList() : PndTrkHitList(), fIRegion(-1) {}

PndTrkSdsHitList::PndTrkSdsHitList(Int_t iregion) : PndTrkHitList(), fIRegion(iregion) {}

PndTrkSdsHitList::~PndTrkSdsHitList()
{
  fpixInstance = nullptr;
  fstrInstance = nullptr;
}

void PndTrkSdsHitList::AddHit(Int_t hitid, Int_t detid, FairHit *hit)
{
  TVector3 position;
  hit->Position(position);

  PndTrkHitList::AddHit(hitid, detid, 0, fIRegion, ((PndSdsHit *)hit)->GetSensorID(), position); // CHECK iregion
}

ClassImp(PndTrkSdsHitList)
