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

// The FTS point is not defined by the (x, y, z) coordinates, but we ONLY know the
// (x, y) coordinates of the wire center (not the z!); moreover we know the drift
// radius. In order to find the MEASURED POINT to be used in the GFKalman fit a
// preliminary propagation must be performed to the point of closest approach to
// the firing wire.
// The plane is a virtual detector plane defined via:
// dj = wvi - vpf     (U coordinate in GFDetPlane)
// dk = wiredirection (V coordinate in GFDetPlane)
// (see GFWireHitPolicy.cxx)
//
// input: 8 entries
// 0-1-2 ==> x,y,z of the 1st extremity of the firing wire;
// 3-4-5 ==> x,y,z of the 2nd extremity of the firing wire;
// 6     ==> drift radius;

// modified by Elisabetta Prencipe 19/5/2014
// This Class' Header ------------------
#include "PndFtsRecoHit2.h"

// C/C++ Headers ----------------------

// Collaborating Class Headers --------
#include "PndFtsHit.h"
#include "PndFtsTube.h"
#include "PndFtsMapCreator.h"
#include "PndGeoFtsPar.h"
#include "FairRuntimeDb.h"
#include "FairRunAna.h"

#include "TMath.h"

// Class Member definitions -----------
using namespace std;

ClassImp(PndFtsRecoHit2)

  PndFtsRecoHit2::~PndFtsRecoHit2()
{
}

PndFtsRecoHit2::PndFtsRecoHit2() : genfit::WireMeasurement(NparHitRep) {}

// TO BE CHANGED TO USE TUBE ARRAY
PndFtsRecoHit2::PndFtsRecoHit2(PndFtsHit *currenthit) : genfit::WireMeasurement(NparHitRep)
{

  FairRuntimeDb *rtdb = FairRunAna::Instance()->GetRuntimeDb();
  PndGeoFtsPar *ftsParameters = (PndGeoFtsPar *)rtdb->getContainer("PndGeoFtsPar");
  Int_t tubeID = currenthit->GetTubeID();
  PndFtsMapCreator *mapper = new PndFtsMapCreator(ftsParameters);
  PndFtsTube *tube = (PndFtsTube *)mapper->GetTubeFromTubeID(tubeID);

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

// TO BE CHANGED TO USE TUBE ARRAY
PndFtsRecoHit2::PndFtsRecoHit2(PndFtsHit *currenthit, const genfit::TrackCandHit *hit, TClonesArray *tubeArray) : genfit::WireMeasurement(NparHitRep)
{

  setDetId(hit->getDetId());
  setHitId(hit->getHitId());

  Int_t tubeID = currenthit->GetTubeID();
  PndFtsTube *tube = (PndFtsTube *)tubeArray->At(tubeID);

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
