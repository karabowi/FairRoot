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
//  BSEmcMCPoint
//
//  Geant point for BSEmc detector
//
//  Created 14/08/06  by S.Spataro
//
///////////////////////////////////////////////////////////////

#include "BSEmcMCPoint.h"

#include "Rtypes.h"     // for ClassImp
#include "RtypesCore.h" // for kFALSE, Bool_t, Double_t, Int_t, kTRUE
#include "TVector3.h"   // for TVector3

#include "FairLink.h"          // for FairLink
#include "FairMCPoint.h"       // for FairMCPoint
#include "fairlogger/Logger.h" // for LOG

// -----   Default constructor   -------------------------------------------
BSEmcMCPoint::BSEmcMCPoint() : FairMCPoint(), fEntering(kFALSE), fExiting(kFALSE) {}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
BSEmcMCPoint::BSEmcMCPoint(Int_t t_trackID, Int_t t_detID, Int_t t_evtID, const TVector3 &t_pos, const TVector3 &t_mom, Double_t t_tof, Double_t t_length, Double_t t_eLoss,
                           Bool_t t_entering, Bool_t t_exiting)
  : FairMCPoint(t_trackID, t_detID, t_pos, t_mom, t_tof, t_length, t_eLoss, t_evtID), fEntering(t_entering), fExiting(t_exiting)
{
  FairMCPoint::SetLink(FairLink{"MCTrack", t_trackID});
}
// -------------------------------------------------------------------------

// -----   Copy constructor   ------------------------------------------
BSEmcMCPoint::BSEmcMCPoint(const BSEmcMCPoint &t_point) : FairMCPoint(t_point), fEntering(t_point.fEntering), fExiting(t_point.fExiting)
{
  SetLinks(t_point.GetLinks());
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
BSEmcMCPoint::~BSEmcMCPoint() {}
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void BSEmcMCPoint::Print(const Option_t * /*opt*/) const
{
  LOG(info) << "BSEmcMCPoint: EMC Point for track " << fTrackID << " in detector " << fDetectorID;
  LOG(info) << "    Position (" << fX << ", " << fY << ", " << fZ << ") cm";
  LOG(info) << "    Momentum (" << fPx << ", " << fPy << ", " << fPz << ") GeV";
  LOG(info) << "    Time " << fTime << " ns,  Length " << fLength << " cm,  Energy loss " << fELoss * 1.0e06 << " keV";
  if (GetEntering() == kTRUE) {
    LOG(info) << " Particle entering the crystal!";
  }
  if (GetExiting() == kFALSE) {
    LOG(info) << " Particle exiting the crystal!";
  }
}
// -------------------------------------------------------------------------

Bool_t BSEmcMCPoint::operator<(const BSEmcMCPoint &t_rhs) const
{
  if (fDetectorID < t_rhs.fDetectorID) {
    return kTRUE;
  }
  if (fDetectorID == t_rhs.fDetectorID) {
    return fTime < t_rhs.fTime;
  }
  return kFALSE;
}

ClassImp(BSEmcMCPoint)
