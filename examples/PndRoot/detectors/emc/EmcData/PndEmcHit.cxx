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
//  PndEmcHit
//
//  Emc digitised hit
//
//  Created 14/08/06  by S.Spataro
//
///////////////////////////////////////////////////////////////

#include "PndEmcHit.h"
#include "PndDetectorList.h"
#include <iostream>

using namespace std;

// -----   Default constructor   -------------------------------------------
PndEmcHit::PndEmcHit() : FairHit(), fEnergy(0), fMcList(0), fTrackEntering(), fTrackExiting()
{
  fMcList.clear();
  fClusterList.clear();
}
// -------------------------------------------------------------------------

// -----   Constructor           -------------------------------------------
PndEmcHit::PndEmcHit(Int_t trackid, Int_t id, Float_t energy, Float_t time, Float_t X, Float_t Y, Float_t Z)
  : FairHit(), fEnergy(energy), fMcList(0), fTrackEntering(), fTrackExiting()
{
  SetTime(time);
  fRefIndex = trackid;
  fDetectorID = id;
  fX = X;
  fY = Y;
  fZ = Z;
  fDx = 0.;
  fDy = 0.;
  fDz = 0.;
  fMcList.clear();
  fClusterList.clear();
}
// -----   Constructor           -------------------------------------------
PndEmcHit::PndEmcHit(Int_t trackid, Int_t id, Float_t energy, Float_t time, Float_t X, Float_t Y, Float_t Z, std::vector<Int_t> McList, FairMultiLinkedData enteringTracks,
                     FairMultiLinkedData exitingTracks, std::set<Int_t> clusterList)
  : FairHit(), fEnergy(energy), fMcList(McList), fTrackEntering(enteringTracks), fTrackExiting(exitingTracks), fClusterList(clusterList)
{
  SetTime(time);
  fRefIndex = trackid;
  fDetectorID = id;
  fX = X;
  fY = Y;
  fZ = Z;
  fDx = 0.;
  fDy = 0.;
  fDz = 0.;
  // SetLinks(FairMultiLinkedData("MCTrack", McList));
  // SetLinks(enteringTracks);
}
// -----   Constructor           -------------------------------------------
 
// Copy
PndEmcHit::PndEmcHit(const PndEmcHit &copy)
  : FairHit(copy), fEnergy(copy.fEnergy), fMcList(0), fTrackEntering(copy.fTrackEntering), fTrackExiting(copy.fTrackExiting), fClusterList(copy.fClusterList)
{
  fRefIndex = copy.fRefIndex;
  fDetectorID = copy.fDetectorID;
  fX = copy.fX;
  fY = copy.fY;
  fZ = copy.fZ;
  // PointList and McList not copied...
}
// -----   Destructor   ----------------------------------------------------
PndEmcHit::~PndEmcHit() {}
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void PndEmcHit::Print(const Option_t *) const
{
  cout << "EMC hit: cellid=" << GetDetectorID() << ", Energy=" << fEnergy;
  if (fRefIndex > 0)
    cout << ", TrackID= " << fRefIndex << std::endl;
  cout << " TrackEntering: " << fTrackEntering << std::endl;
  cout << "TrackExiting: " << fTrackExiting << std::endl;
  cout << "FairLinks: " << *((FairMultiLinkedData_Interface *)this) << std::endl;
  cout << "ClusterIDs: ";
  for (auto clusterId : fClusterList) {
    std::cout << clusterId << " ";
  }
  cout << endl;
  cout << "Deposited Energy: ";
  for (auto depositedEnergy : fDepositedEnergy) {
    cout << depositedEnergy.first << " : " << depositedEnergy.second << " GeV - ";
  }
  cout << endl;
  //  cout << ", x=" << GetX() << ", y=" << GetY() << endl << flush;
}
// -------------------------------------------------------------------------

Short_t PndEmcHit::GetXPad() const
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

Short_t PndEmcHit::GetYPad() const
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
ClassImp(PndEmcHit)
