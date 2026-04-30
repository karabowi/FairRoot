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
 
#include "PndStt2Point.h"

#include "PndDetectorList.h"
#include "FairLogger.h"

#include <iostream>
#include "math.h"

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndStt2Point::PndStt2Point()
  : PndMCPoint(), fTubeID(0)
{
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndStt2Point::PndStt2Point(Int_t trackID, Int_t detID, Int_t tubeID, TVector3 posIn, TVector3 posOut, TVector3 momIn, TVector3 momOut, Double_t tof, Double_t length, Double_t eLoss)
  : PndMCPoint(trackID, detID, posIn, posOut, momIn, momOut, tof, length, eLoss), fTubeID(tubeID)
{
  // reset MC monentum
  SetLink(FairLink("MCTrack", trackID));
}

//PndStt2Point::PndStt2Point(const PndStt2Point &point)
  //: FairMCPoint(point.fTrackID, point.fDetectorID, TVector3(point.fX, point.fY, point.fZ), TVector3(point.fPx, point.fPy, point.fPz), point.fTime, point.fLength, point.fELoss,
                //point.fEventId),
    //fX_out_local(point.fX_out_local), fY_out_local(point.fY_out_local), fZ_out_local(point.fZ_out_local), fX_in_local(point.fX_in_local), fY_in_local(point.fY_in_local),
    //fZ_in_local(point.fZ_in_local), fPx_in(point.fPx_in), fPy_in(point.fPy_in), fPz_in(point.fPz_in), fPx_out(point.fPx_out), fPy_out(point.fPy_out), fPz_out(point.fPz_out),
    //fMass(point.fMass), fTubeID(point.fTubeID)
//{
  //SetLink(FairLink("MCTrack", point.fTrackID));
//}
// -------------------------------------------------------------------------

//Double_t PndStt2Point::GetTrueDistance()
//{

  //TVector3 x1(0., 0., -75.); // CHECK
  //TVector3 x2(0., 0., 75.);  // CHECK
  //TVector3 x3(fX_in_local, fY_in_local, fZ_in_local);
  //TVector3 x4(fX_out_local, fY_out_local, fZ_out_local);

  //return fabs((x3 - x1).Dot((x2 - x1).Cross(x4 - x3))) / fabs(((x2 - x1).Cross(x4 - x3)).Mag());
//}

// -----   Destructor   ----------------------------------------------------
PndStt2Point::~PndStt2Point() {}
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void PndStt2Point::Print(const Option_t *opt) const
{
  LOG(info) << " PndStt2Point: Stt Point for track " << fTrackID << " in detector " << fDetectorID<< ", tube " << fTubeID;
  cout << "    Position (" << fX << ", " << fY << ", " << fZ << ") cm" << endl;
  cout << "    Momentum (" << fPx << ", " << fPy << ", " << fPz << ") GeV" << endl;
  cout << "    Time " << fTime  << " ns,  Length " << fLength << " cm,  Energy loss " << fELoss * 1.0e06 << " keV"
       << " opt = " << opt << endl;
}
// -------------------------------------------------------------------------

ClassImp(PndStt2Point)
