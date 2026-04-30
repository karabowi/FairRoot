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

// The STT point is not defined by the (x, y, z) coordinates, but we ONLY know the
// (x, y) coordinates of the wire center (not the z!); moreover we know the drift
// radius. In order to find the MEASURED POINT to be used in the Kalman fit a
// preliminary propagation must be performed to the point of closest approach to
// the firing wire.
// The plane is a virtual detector plane defined via:
// dj = wvi - vpf     (U coordinate in DetPlane)
// dk = wiredirection (V coordinate in DetPlane)
// (see WireMeasurement.cpp)
//
// input: 8 entries
// 0-1-2 ==> x,y,z of the 1st extremity of the firing wire;
// 3-4-5 ==> x,y,z of the 2nd extremity of the firing wire;
// 6     ==> drift radius;

// modified by Elisabetta Prencipe 19/5/2014

// This Class' Header ------------------
#include "PndSttRecoHit2.h"

// C/C++ Headers ----------------------

// Collaborating Class Headers --------
#include "PndSttHit.h"
#include "PndSttTube.h"
#include "PndSttMapCreator.h"
#include "PndSttTubeMap.h"
#include "PndGeoSttPar.h"
#include "FairRuntimeDb.h"
#include "FairRunAna.h"

#include "TMath.h"

// Class Member definitions -----------
using namespace std;

ClassImp(PndSttRecoHit2)

  PndSttRecoHit2::~PndSttRecoHit2()
{
}

PndSttRecoHit2::PndSttRecoHit2() : genfit::WireMeasurement(NparHitRep) {}

//// TO BE CHANGED TO USE TUBE ARRAY
//PndSttRecoHit2::PndSttRecoHit2(PndSttHit *currenthit) : genfit::WireMeasurement(NparHitRep)
//{

  //FairRuntimeDb *rtdb = FairRunAna::Instance()->GetRuntimeDb();
  //PndGeoSttPar *sttParameters = (PndGeoSttPar *)rtdb->getContainer("PndGeoSttPar");
  //Int_t tubeID = currenthit->GetTubeID();
  //PndSttMapCreator *mapper = new PndSttMapCreator(sttParameters);
  //PndSttTube *tube = (PndSttTube *)mapper->GetTubeFromTubeID(tubeID);

  //// wire1(3), wire2(3), rdrift
  //TVector3 wiredirection = tube->GetWireDirection();
  //TVector3 wiredirection2 = tube->GetHalfLength() * wiredirection;
  //TVector3 cenposition = tube->GetPosition();
  //TVector3 wire1, wire2;
  //wire1 = cenposition - wiredirection2;
  //wire2 = cenposition + wiredirection2;
  ////   cout << "Wiredirection, wire1, wire2 " << endl;
  ////   wiredirection.Print();
  ////   wire1.Print();
  ////   wire2.Print();
  //rawHitCoords_[0] = wire1.X();
  //rawHitCoords_[1] = wire1.Y();
  //rawHitCoords_[2] = wire1.Z();
  //rawHitCoords_[3] = wire2.X();
  //rawHitCoords_[4] = wire2.Y();
  //rawHitCoords_[5] = wire2.Z();
  //rawHitCoords_[6] = currenthit->GetIsochrone();

  //// errors on drift radius
  //for (int i = 0; i < NparHitRep; i++)
    //for (int j = 0; j < NparHitRep; j++)
      //rawHitCov_[i][j] = 0.;
  ////   rawHitCov_[6][6] = 0.0100 * 0.0100;
  //rawHitCov_[6][6] = pow(currenthit->GetIsochroneError(), 2);

  //// cut on distance
  //setMaxDistance(0.5);
//}

// TO BE CHANGED TO USE TUBE ARRAY
PndSttRecoHit2::PndSttRecoHit2(PndSttHit *currenthit, const genfit::TrackCandHit *hit, TClonesArray *tubeArray) : genfit::WireMeasurement(NparHitRep)
{

  setDetId(hit->getDetId());
  setHitId(hit->getHitId());

  Int_t tubeID = currenthit->GetTubeID();
  PndSttTube *tube = nullptr;
  if (tubeArray != nullptr) {
    tube = (PndSttTube *)tubeArray->At(tubeID);
  } else {
    tube = PndSttTubeMap::Instance()->GetTube(tubeID);
  }

  // wire1(3), wire2(3), rdrift
  TVector3 wiredirection = tube->GetWireDirection();
  TVector3 wiredirection2 = tube->GetHalfLength() * wiredirection;
  TVector3 cenposition = tube->GetPosition();
  TVector3 wire1, wire2;
  wire1 = cenposition - wiredirection2;
  wire2 = cenposition + wiredirection2;
  //   cout << "Wiredirection, wire1, wire2 " << endl;
  //   wiredirection.Print();
  //   wire1.Print();
  //   wire2.Print();
  rawHitCoords_[0] = wire1.X();
  rawHitCoords_[1] = wire1.Y();
  rawHitCoords_[2] = wire1.Z();
  rawHitCoords_[3] = wire2.X();
  rawHitCoords_[4] = wire2.Y();
  rawHitCoords_[5] = wire2.Z();
  rawHitCoords_[6] = currenthit->GetIsochrone();

  // errors on drift radius
  for (int i = 0; i < NparHitRep; i++)
    for (int j = 0; j < NparHitRep; j++)
      rawHitCov_[i][j] = 0.;
  //   rawHitCov_[6][6] = 0.0100 * 0.0100;
  rawHitCov_[6][6] = pow(currenthit->GetIsochroneError(), 2);

  // cut on distance
  setMaxDistance(0.5);
}
