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

/////////////////////////////////////////////////////////////
//
//  PndEmcApdHit
//
//  Emc digitised hit
//
//  Created 14/08/06  by S.Spataro
//
///////////////////////////////////////////////////////////////

#include "PndEmcApdHit.h"
#include <iostream>

using namespace std;

// -----   Default constructor   -------------------------------------------
PndEmcApdHit::PndEmcApdHit() : FairHit(), fTime(0), fEnergy(0), fPointList(0), fNPoints(0)
{
  fPointList.clear();
}

// -------------------------------------------------------------------------

// -----   Constructor           -------------------------------------------
PndEmcApdHit::PndEmcApdHit(Int_t trackid, Int_t id, Float_t energy, Float_t time, Float_t X, Float_t Y, Float_t Z)
  : FairHit(), fTime(time), fEnergy(energy), fPointList(0), fNPoints(0)
{
  fRefIndex = trackid;
  fDetectorID = id;
  fX = X;
  fY = Y;
  fZ = Z;
  fDx = 0.;
  fDy = 0.;
  fDz = 0.;
  for (Int_t ii = 0; ii < 10; ii++)
    fPointIndex[ii] = -1;
}
// -----   Constructor           -------------------------------------------
PndEmcApdHit::PndEmcApdHit(Int_t trackid, Int_t id, Float_t energy, Float_t time, Float_t X, Float_t Y, Float_t Z, Int_t points, Int_t pointIndex[10])
  : FairHit(), fTime(time), fEnergy(energy), fPointList(0), fNPoints(points)
{
  fRefIndex = trackid;
  fDetectorID = id;
  fX = X;
  fY = Y;
  fZ = Z;
  fDx = 0.;
  fDy = 0.;
  fDz = 0.;
  for (Int_t ii = 0; ii < 10; ii++)
    fPointIndex[ii] = pointIndex[ii];
}
// -----   Constructor           -------------------------------------------
PndEmcApdHit::PndEmcApdHit(Int_t trackid, Int_t id, Float_t energy, Float_t time, Float_t X, Float_t Y, Float_t Z,
                           std::vector<PndEmcApdPoint *>) // PointList //[R.K.03/2017] unused variable(s)
  : FairHit(), fTime(time), fEnergy(energy), fPointList(0), fNPoints(0)
{
  fRefIndex = trackid;
  fDetectorID = id;
  fX = X;
  fY = Y;
  fZ = Z;
  fDx = 0.;
  fDy = 0.;
  fDz = 0.;
  for (Int_t ii = 0; ii < 10; ii++)
    fPointIndex[ii] = -1;
}

// Copy
PndEmcApdHit::PndEmcApdHit(const PndEmcApdHit &copy) : FairHit(copy), fTime(copy.fTime), fEnergy(copy.fEnergy), fPointList(copy.fPointList), fNPoints(copy.fNPoints)
{
  fRefIndex = copy.fRefIndex;
  fDetectorID = copy.fDetectorID;
  fX = copy.fX;
  fY = copy.fY;
  fZ = copy.fZ;
  for (Int_t ii = 0; ii < 10; ii++)
    fPointIndex[ii] = copy.fPointIndex[ii];
}
// -----   Destructor   ----------------------------------------------------
PndEmcApdHit::~PndEmcApdHit()
{
  fPointList.clear();
}
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void PndEmcApdHit::Print(const Option_t *) const
{
  cout << "EMC hit: cellid=" << GetDetectorID() << ", Energy=" << fEnergy;
  if (fRefIndex > 0)
    cout << ", TrackID= " << fRefIndex;
  //  cout << ", x=" << GetX() << ", y=" << GetY() << endl << flush;
}
// -------------------------------------------------------------------------

Short_t PndEmcApdHit::GetXPad() const
{
  // Return the X pad value for clusterization

  // Barrel EMC
  if (GetModule() == 1 || GetModule() == 2)
    return (GetCrystal() + (GetCopy() - 1) * 10);

  // BwEndCap and forward EMC
  if ((GetModule() == 4) || (GetModule() == 5)) {
    if (GetCopy() == 1) {
      return -GetRow() + 1;
    }
    if (GetCopy() == 2) {
      return -GetRow() + 1;
    }
    if (GetCopy() == 3) {
      return GetRow();
    }
    if (GetCopy() == 4) {
      return GetRow();
    }
  }

  // FwEndCap
  if (GetModule() == 3)
    return -(GetCrystal() - 36); // the minus sign before the paranthesis is introduced since the geometry of FwEndCap gets rotated by 180 deg around the y-axis in PndEmc.cxx;
                                 // this rotation was done in turn due to the way the geometry was defined in the geometry file of the forward end cap

  /*
   if (GetModule()==3 && GetCrystal()==999 && GetRow()==999)
    return GetCrystal();
  */

  // Test EMC
  if (GetModule() == 6)
    return (GetRow());

  return -1000; // failure
}

Short_t PndEmcApdHit::GetYPad() const
{
  // Return the Y pad value for clusterization

  // Barrel EMC
  if (GetModule() == 1)
    return (GetRow() + 29);

  if (GetModule() == 2)
    return (-GetRow() + 30);

  // BwEndCap and forward EMC
  if ((GetModule() == 4) || (GetModule() == 5)) {
    if (GetCopy() == 1) {
      return GetCrystal();
    }
    if (GetCopy() == 2) {
      return -GetCrystal() + 1;
    }
    if (GetCopy() == 3) {
      return -GetCrystal() + 1;
    }
    if (GetCopy() == 4) {
      return GetCrystal();
    }
  }

  // FwEndCap
  if (GetModule() == 3)
    return GetRow() - 37;

  /*
   if (GetModule()==3 && GetCrystal()==999 && GetRow()==999)
    return GetRow();
  */

  // Test EMC
  if (GetModule() == 6)
    return (GetCrystal());

  return -1000; // failure
}
ClassImp(PndEmcApdHit)
