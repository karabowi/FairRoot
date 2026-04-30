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
// PndTrkIndivisibleHit.cxx
//
// Class for pattern recognition combined hit:
// the hit is obtained combining indivisible
// hits and its position is obtained calculating
// the center of mass.
//
// authors: Lia Lavezzi - INFN Pavia (2012)
//

#include "PndTrkIndivisibleHit.h"

#include "PndDetectorList.h"

#include <iostream>

// ROOT
#include "TArc.h"
#include "TMarker.h"
#include "TMath.h"

using namespace std;

// Bool_t PndTrkIndivisibleHit::operator==(const PndTrkIndivisibleHit &hit1) {
//   return  fDetectorID == hit1.fDetectorID && fHitID == hit1.fHitID;
// }

// Bool_t PndTrkIndivisibleHit::operator<(const PndTrkIndivisibleHit &hit1) {
//   return  fSortVariable < hit1.fSortVariable;
// }

PndTrkIndivisibleHit::PndTrkIndivisibleHit() : PndTrkHit(), fHitIDs(TArrayI(0)) {}

PndTrkIndivisibleHit::PndTrkIndivisibleHit(TArrayI hitids, TVector3 &pos) : PndTrkHit(-1, -1, kFALSE, INDIVISIBLE, -1, pos, -1, pos.Mag()), fHitIDs(hitids) {}

PndTrkIndivisibleHit::PndTrkIndivisibleHit(const PndTrkIndivisibleHit &hit) : PndTrkHit(hit)
{
  fHitIDs = TArrayI(hit.fHitIDs);
}

PndTrkIndivisibleHit::~PndTrkIndivisibleHit()
{
  fHitIDs.Reset();
}

// Int_t PndTrkIndivisibleHit::Compare(const TObject *hit)  const {
//   //  Compare abstract method. Must be overridden if a class wants to be able
//   //  to compare itself with other objects. Must return -1 if this is smaller
//   //  than obj, 0 if objects are equal and 1 if this is larger than obj.
//   if(fSortVariable < ((PndTrkIndivisibleHit*) hit)->fSortVariable) return -1;
//   else if(fSortVariable == ((PndTrkIndivisibleHit*) hit)->fSortVariable) return 0;
//   else return 1;
// }

void PndTrkIndivisibleHit::Draw(Color_t color)
{

  TMarker *mrk = new TMarker(fPosition.X(), fPosition.Y(), 1);
  mrk->SetMarkerColor(color);
  mrk->SetMarkerStyle(20);
  mrk->Draw("SAME");
}

void PndTrkIndivisibleHit::Print()
{

  cout << "###############################" << endl;
  cout << "hitIDs: ";
  for (int ihit = 0; ihit < fHitIDs.GetSize(); ihit++)
    cout << " " << fHitIDs.At(ihit);
  cout << endl;
  fPosition.Print();
}

ClassImp(PndTrkIndivisibleHit)
