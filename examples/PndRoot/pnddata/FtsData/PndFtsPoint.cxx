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

#include "PndFtsPoint.h"
#include "PndDetectorList.h"

#include <iostream>
#include "math.h"
#include "FairLogger.h"
using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndFtsPoint::PndFtsPoint()
  : FairMCPoint(), fX_out_local(0), fY_out_local(0), fZ_out_local(0), fX_in_local(0.), fY_in_local(0), fZ_in_local(0), fPx_out(0), fPy_out(0), fPz_out(0), fPx_in(0), fPy_in(0),
    fPz_in(0), fMass(0), fTubeID(0), fChamberID(0), fLayerID(0)
{
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndFtsPoint::PndFtsPoint(Int_t trackID, Int_t detID, Int_t tubeID, Int_t chamberID, Int_t layerID, TVector3 pos, TVector3 posInLocal, TVector3 posOutLocal, TVector3 momIn,
                         TVector3 momOut, Double_t tof, Double_t length, Double_t eLoss, Double_t mass)
  : FairMCPoint(trackID, detID, pos, momIn, tof, length, eLoss), fX_out_local(posOutLocal.X()), fY_out_local(posOutLocal.Y()), fZ_out_local(posOutLocal.Z()),
    fX_in_local(posInLocal.X()), fY_in_local(posInLocal.Y()), fZ_in_local(posInLocal.Z()), fPx_out(momOut.Px()), fPy_out(momOut.Py()), fPz_out(momOut.Pz()), fPx_in(momIn.Px()),
    fPy_in(momIn.Py()), fPz_in(momIn.Pz()), fMass(mass), fTubeID(tubeID), fChamberID(chamberID), fLayerID(layerID)
{
  // reset MC momentum
  fPx = (momIn.Px() + momOut.Px()) / 2.;
  fPy = (momIn.Py() + momOut.Py()) / 2.;
  fPz = (momIn.Pz() + momOut.Pz()) / 2.;
  SetLink(FairLink("MCTrack", trackID));
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndFtsPoint::~PndFtsPoint() {}
// -------------------------------------------------------------------------

// -----   Copy constructor   ------------------------------------------
PndFtsPoint::PndFtsPoint(const PndFtsPoint &point)
  : FairMCPoint(point.fTrackID, point.fDetectorID, TVector3(point.fX, point.fY, point.fZ), TVector3(point.fPx, point.fPy, point.fPz), point.fTime, point.fLength, point.fELoss,
                point.fEventId),
    fX_out_local(point.fX_out_local), fY_out_local(point.fY_out_local), fZ_out_local(point.fZ_out_local), fX_in_local(point.fX_in_local), fY_in_local(point.fY_in_local),
    fZ_in_local(point.fZ_in_local), fPx_out(point.fPx_out), fPy_out(point.fPy_out), fPz_out(point.fPz_out), fPx_in(point.fPx_in), fPy_in(point.fPy_in), fPz_in(point.fPz_in),
    fMass(point.fMass), fTubeID(point.fTubeID), fChamberID(point.fChamberID), fLayerID(point.fLayerID)
{
}
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void PndFtsPoint::Print(const Option_t *opt) const
{
  LOG(info) << " PndFtsPoint: FTS Point for track " << fTrackID << " in detector " << fDetectorID;
  cout << "    Position (" << fX << ", " << fY << ", " << fZ << ") cm" << endl;
  cout << "    Momentum (" << fPx << ", " << fPy << ", " << fPz << ") GeV" << endl;
  cout << "    Time " << fTime << " ns,  Length " << fLength << " cm,  Energy loss " << fELoss * 1.0e06 << " keV"
       << " opt=" << opt << endl;
}
// -------------------------------------------------------------------------

ClassImp(PndFtsPoint)
