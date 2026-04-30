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

// -------------------------------------------------------------------------
// -----                    SciTHit source file                -----
//  created by A. Sanchez
//  modified by D. Steinschaden
//  last update  05.2015
// -------------------------------------------------------------------------

#include <iostream>
#include "PndSciTHit.h"

// -----  Operator overload ----------------------------------------------
std::ostream &operator<<(std::ostream &out, PndSciTHit &hit)
{
  out << "PndSciTHit in Sensor: " << hit.GetDetectorID();
  return out;
}

bool PndSciTHit::operator<(const PndSciTHit &right) const
{

  if (fDetectorID < right.GetDetectorID())
    return true;
  else if (fDetectorID > right.GetDetectorID())
    return false;
  // If ID = right.ID
  return false;
}

bool PndSciTHit::operator>(const PndSciTHit &right) const
{

  if (fDetectorID > right.GetDetectorID())
    return true;
  else if (fDetectorID < right.GetDetectorID())
    return false;
  // If ID = right.ID
  return false;
}

bool PndSciTHit::equal(FairTimeStamp *data)
{
  PndSciTHit *hit = dynamic_cast<PndSciTHit *>(data);
  if (hit != 0 && fDetectorID == hit->GetDetectorID())
    return true;

  return false;
}

// -----   Default constructor   -------------------------------------------
PndSciTHit::PndSciTHit() : fDetName(""), fCharge(0.), fSiPm1(0.), fDSiPm1(0.), fSiPm2(0.), fDSiPm2(0.) {}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndSciTHit::PndSciTHit(Int_t detID, TString detName, Double_t time, Double_t dt, Double_t sipm1, Double_t dsipm1, Double_t sipm2, Double_t dsipm2, TVector3 &pos, TVector3 &dpos,
                       Int_t index, Double_t charge)
  : FairHit(detID, pos, dpos, index), fDetName(detName), fCharge(charge), fSiPm1(sipm1), fDSiPm1(dsipm1), fSiPm2(sipm2), fDSiPm2(dsipm2)
{
  SetTimeStamp(time);
  SetTimeStampError(dt);

  SetLink(FairLink(-1, -1, "SciTPoint", index));
}

// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndSciTHit::~PndSciTHit() {}
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void PndSciTHit::Print(const Option_t *opt) const
{
  std::cout << "Forward tof hit in detector " << fDetName << " at (" << fX << ", " << fY << ", " << fZ << ") cm "
            << ", Point " << fRefIndex << "opt=" << opt << std::endl;
}
// -------------------------------------------------------------------------
ClassImp(PndSciTHit)
