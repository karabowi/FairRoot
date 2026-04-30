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
// PndTrkHit.cxx
//
// Class for pattern recognition hit
//
// authors: Lia Lavezzi - INFN Pavia (2012)
//

#include "PndTrkHit.h"

#include "PndDetectorList.h"

#include <iostream>

// ROOT
#include "TArc.h"
#include "TMarker.h"
#include "TMath.h"

using namespace std;

// hit equality
// Bool_t operator==(const PndTrkHit &hit1, const PndTrkHit &hit2) {
//   cout << "######################### operator <" << endl;
//   return  hit1.fDetectorID == hit2.fDetectorID && hit1.fHitID == hit2.fHitID;
// }

Bool_t PndTrkHit::operator==(const PndTrkHit &hit1)
{
  return fDetectorID == hit1.fDetectorID && fHitID == hit1.fHitID;
}

Bool_t PndTrkHit::operator<(const PndTrkHit &hit1)
{
  return fSortVariable < hit1.fSortVariable;
}

// // hit sorting
// Bool_t operator<(const PndTrkHit *hit1, const PndTrkHit *hit2) {
//   cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! operator <" << endl;
//   return  hit1->fSortVariable == hit2->fSortVariable;
// }

PndTrkHit::PndTrkHit()
  : fHitID(-1), fDetectorID(-1), fIRegion(-1), fSensorID(-1), fUsed(0), fUsedAsRefHit(0), fPosition(0., 0., 0.), fIsochrone(0), fSortVariable(-1), fPhi(-1), fSector(-1)
{
}

PndTrkHit::PndTrkHit(Int_t hitID, Int_t detID, Bool_t used, Int_t iregion, Int_t sensorID, TVector3 &pos, Double_t isochrone, Double_t sortvar)
  : fHitID(hitID), fDetectorID(detID), fIRegion(iregion), fSensorID(sensorID), fUsed(used), fUsedAsRefHit(0), fPosition(pos.X(), pos.Y(), pos.Z()), fIsochrone(isochrone),
    fSortVariable(sortvar), fPhi(-1), fSector(-1)
{
}

// PndTrkHit::PndTrkHit(Int_t hitID, Int_t detID, Bool_t used, Int_t iregion, Int_t sensorID, TVector3& pos) :  fHitID(hitID), fDetID(detID), fUsed(used), fIRegion(iregion),
// fSensorID(sensorID), fPosition(pos.X(), pos.Y(), pos.Z()), fIsochrone(0) {}

PndTrkHit::PndTrkHit(const PndTrkHit &hit)
  : FairTimeStamp(hit), fHitID(hit.fHitID), fDetectorID(hit.fDetectorID), fIRegion(hit.fIRegion), fSensorID(hit.fSensorID), fUsed(hit.fUsed), fUsedAsRefHit(hit.fUsedAsRefHit),
    fPosition(hit.fPosition), fIsochrone(hit.fIsochrone), fSortVariable(hit.fSortVariable), fPhi(hit.fPhi)
{

  double phi = fPosition.Phi() * TMath::RadToDeg();
  if (phi < 0 && phi < -30)
    phi += 360;
  if (phi > -30 && phi <= 30)
    fSector = 4;
  else if (phi > 30 && phi <= 90)
    fSector = 5;
  else if (phi > 90 && phi <= 150)
    fSector = 0;
  else if (phi > 150 && phi <= 210)
    fSector = 1;
  else if (phi > 210 && phi <= 270)
    fSector = 2;
  else if (phi > 270 && phi <= 330)
    fSector = 3;
}

PndTrkHit::~PndTrkHit() {}

Int_t PndTrkHit::Compare(const TObject *hit) const
{
  //  Compare abstract method. Must be overridden if a class wants to be able
  //  to compare itself with other objects. Must return -1 if this is smaller
  //  than obj, 0 if objects are equal and 1 if this is larger than obj.
  //  cout << "comparison " << fSortVariable<< " " << ((PndTrkHit*) hit)->fSortVariable << " " << (fSortVariable < ((PndTrkHit*) hit)->fSortVariable) << " " << (fSortVariable ==
  //  ((PndTrkHit*) hit)->fSortVariable) << endl;
  if (fSortVariable < ((PndTrkHit *)hit)->fSortVariable)
    return -1;
  else if (fSortVariable == ((PndTrkHit *)hit)->fSortVariable)
    return 0;
  else
    return 1;
}

Double_t PndTrkHit::GetDistance(PndTrkHit *fromhit)
{
  TVector3 frompoint = fromhit->GetPosition();
  TVector3 position = GetPosition();
  return GetDistance(frompoint);
}

Double_t PndTrkHit::GetDistance(TVector3 frompoint)
{
  TVector3 position = GetPosition();
  return (position - frompoint).Mag();
}

Double_t PndTrkHit::GetXYDistance(PndTrkHit *fromhit)
{
  TVector3 frompoint = fromhit->GetPosition();
  TVector3 position = GetPosition();
  return GetXYDistance(frompoint);
}

Double_t PndTrkHit::GetXYDistance(TVector3 frompoint)
{
  TVector3 position = GetPosition();
  return (position - frompoint).Perp();
}

Double_t PndTrkHit::GetXYDistanceFromTrack(double x0, double y0, double R)
{
  return fabs(R - TMath::Sqrt((fPosition.X() - x0) * (fPosition.X() - x0) + (fPosition.Y() - y0) * (fPosition.Y() - y0)));
}

void PndTrkHit::Draw(Color_t color)
{
  //  cout << "draw "<< fIRegion << endl;
  Int_t style = 1;
  if (fUsed && color == kBlack) {
    style = 0;
    color = kGray;
  }
  TMarker *mrk = nullptr;
  TArc *arc = nullptr;

  switch (fIRegion) {
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
    mrk = new TMarker(fPosition.X(), fPosition.Y(), 6);
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
    mrk->SetMarkerSize(0.5);
    mrk->Draw("SAME");
  }
  if (arc) {
    arc->SetFillColor(color);
    arc->SetLineColor(color);
    arc->SetFillStyle(style);
    arc->Draw("SAME");
  }
}

void PndTrkHit::DrawTube(Color_t color)
{
  //  cout << "draw "<< fIRegion << endl;
  // Int_t style = 1;  // [R.K. 03/2017] Unused variables
  if (fUsed && color == kBlack) {
    // style = 0; // [R.K. 03/2017] Unused variables
    color = kGray;
  }
  TMarker *mrk = nullptr;
  TArc *arc = nullptr;

  switch (fIRegion) {
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
    arc = new TArc(fPosition.X(), fPosition.Y(), TUBERADIUS);
    break;
  }
  case 4:
  case 5: {
    //      arc = new TArc(fPosition.X(), fPosition.Y(), TUBERADIUS); // CHECK skew
    mrk = new TMarker(fPosition.X(), fPosition.Y(), 3);
    break;
  }
  }

  if (mrk) {
    mrk->SetMarkerColor(color);
    mrk->Draw("SAME");
  }
  if (arc) {
    arc->SetFillColor(color);
    arc->SetLineColor(color);
    //     arc->SetFillStyle(style);
    arc->Draw("SAME");
  }
}

void PndTrkHit::Print()
{

  cout << "###############################" << endl;
  cout << "hitID " << fHitID << " used " << fUsed << " iregion " << fIRegion << endl;
  cout << " detID " << fDetectorID << " isensor " << fSensorID << " isochrone " << fIsochrone << endl;
  fPosition.Print();
}

ClassImp(PndTrkHit)
