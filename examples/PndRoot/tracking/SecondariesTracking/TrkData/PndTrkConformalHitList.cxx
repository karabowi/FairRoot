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
// PndTrkConformalHitList.cxx
//
// authors: Lia Lavezzi - INFN Pavia (2012)
//

#include "PndTrkConformalHitList.h"

#include <iostream>
#include "PndTrkConformalHit.h"

using namespace std;

PndTrkConformalHitList::PndTrkConformalHitList() : fConformal(new PndTrkConformalTransform()), fHitList(TClonesArray("PndTrkConformalHit", 10000)) {}

PndTrkConformalHitList::PndTrkConformalHitList(PndTrkConformalTransform *conformal) : fConformal(conformal), fHitList(TClonesArray("PndTrkConformalHit", 10000)) {}

PndTrkConformalHitList::PndTrkConformalHitList(const PndTrkConformalHitList &hlist)
  : TObject(hlist), fConformal(new PndTrkConformalTransform()), fHitList(TClonesArray("PndTrkConformalHit", 10000))
{
  *this = hlist;
}

PndTrkConformalHitList::~PndTrkConformalHitList()
{
  delete fConformal;
  fHitList.Delete();
}

PndTrkConformalHitList &PndTrkConformalHitList::operator=(const PndTrkConformalHitList &hlist)
{
  fHitList = TClonesArray(hlist.fHitList);
  fConformal = hlist.fConformal;
  return *this;
}

// ----------------------------------------------------

void PndTrkConformalHitList::Reset()
{
  fConformal = nullptr;
  fHitList.Clear();
}

void PndTrkConformalHitList::Clear(Option_t *opt)
{
  fConformal = nullptr;
  fHitList.Clear(opt);
}

void PndTrkConformalHitList::AddHit(PndTrkConformalHit *chit)
{
  int size = fHitList.GetEntriesFast();
  new (fHitList[size]) PndTrkConformalHit(*chit);
}

PndTrkConformalHit *PndTrkConformalHitList::GetHit(int index)
{
  return (PndTrkConformalHit *)fHitList.At(index);
}

void PndTrkConformalHitList::Print()
{

  cout << "###############################" << endl;
  for (int ihit = 0; ihit < GetNofHits(); ihit++) {
    PndTrkConformalHit *chit = (PndTrkConformalHit *)fHitList.At(ihit);
    chit->Print();
  }
}

void PndTrkConformalHitList::Draw(Color_t color)
{
  for (int ihit = 0; ihit < GetNofHits(); ihit++) {
    PndTrkConformalHit *chit = (PndTrkConformalHit *)fHitList.At(ihit);
    chit->Draw(color);
  }
}

ClassImp(PndTrkConformalHitList)
