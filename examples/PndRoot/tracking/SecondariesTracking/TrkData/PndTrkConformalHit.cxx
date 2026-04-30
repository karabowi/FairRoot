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
// PndTrkConformalHit.cxx
//
// Class for pattern recognition conformal hit
//
// authors: Lia Lavezzi - INFN Pavia (2012)
//

#include "PndTrkConformalHit.h"

#include <iostream>

// ROOT
#include "TArc.h"
#include "TMarker.h"
#include "TMath.h"

using namespace std;

PndTrkConformalHit::PndTrkConformalHit() : fHit(nullptr), fPosition(0., 0.), fIsochrone(-1) {}

PndTrkConformalHit::PndTrkConformalHit(PndTrkHit *hit, double u, double v, double rd) : fHit(hit), fPosition(u, v), fIsochrone(rd) {}

PndTrkConformalHit::PndTrkConformalHit(PndTrkHit *hit, double u, double v) : fHit(hit), fPosition(u, v), fIsochrone(-1) {}

PndTrkConformalHit::~PndTrkConformalHit() {}

double PndTrkConformalHit::GetDistanceFromTrack(double fitm, double fitp)
{
  return TMath::Abs(fPosition.Y() - fitm * fPosition.X() - fitp) / TMath::Sqrt(fitm * fitm + 1);
}

void PndTrkConformalHit::Draw(Color_t color)
{

  TMarker *mrk = nullptr;
  TArc *arc = nullptr;

  switch (fHit->GetIRegion()) {
  case 0: {
    mrk = new TMarker(fPosition.X(), fPosition.Y(), 21);
    break;
  }
  case 1: {
    mrk = new TMarker(fPosition.X(), fPosition.Y(), 25);
    break;
  }
  case 2:
  case 6:
  case 3:
  case 7: {
    arc = new TArc(fPosition.X(), fPosition.Y(), fIsochrone); // TUBERADIUS);
    break;
  }
  case 4:
  case 5: {
    //      arc = new TArc(fPosition.X(), fPosition.Y(), TUBERADIUS); // CHECK skew
    mrk = new TMarker(fPosition.X(), fPosition.Y(), 3);
    break;
  }
  case 8: {
    mrk = new TMarker(fPosition.X(), fPosition.Y(), 29);
    break;
  }
  case 9: {
    mrk = new TMarker(fPosition.X(), fPosition.Y(), 24);
    break;
  }
  case 10: {
    mrk = new TMarker(fPosition.X(), fPosition.Y(), 25);
    break;
  }
  }

  if (mrk) {
    mrk->SetMarkerColor(color);
    mrk->Draw("SAME");
  }
  if (arc) {
    arc->SetFillStyle(0);
    arc->SetLineColor(color);
    arc->Draw("SAME");
  }
}

void PndTrkConformalHit::Print()
{

  cout << "conformal hit ###############################" << endl;
  cout << "hitID " << fHit->GetHitID() << " detID " << fHit->GetDetectorID() << endl;
  cout << "isochrone " << fIsochrone << endl;
  fPosition.Print();
}

ClassImp(PndTrkConformalHit)
