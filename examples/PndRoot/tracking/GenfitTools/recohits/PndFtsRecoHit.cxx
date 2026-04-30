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

// This Class' Header ------------------
#include "PndFtsRecoHit.h"

// C/C++ Headers ----------------------

// Collaborating Class Headers --------
#include "GeaneTrackRep.h"
#include "RKTrackRep.h"
#include "PndFtsHit.h"
#include "PndFtsTube.h"
#include "PndFtsMapCreator.h"
#include "PndGeoFtsPar.h"
#include "FairRuntimeDb.h"
#include "FairRunAna.h"

#include "TMath.h"

// Class Member definitions -----------
using namespace std;

ClassImp(PndFtsRecoHit)

  PndFtsRecoHit::~PndFtsRecoHit()
{
}

PndFtsRecoHit::PndFtsRecoHit() : WireHitRecoHit(NparHitRep) {}

// TO BE CHANGED TO USE TUBE ARRAY
PndFtsRecoHit::PndFtsRecoHit(PndFtsHit *currenthit) : WireHitRecoHit(NparHitRep)
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
  fHitCoord[0][0] = wire1.X();
  fHitCoord[1][0] = wire1.Y();
  fHitCoord[2][0] = wire1.Z();
  fHitCoord[3][0] = wire2.X();
  fHitCoord[4][0] = wire2.Y();
  fHitCoord[5][0] = wire2.Z();
  fHitCoord[6][0] = currenthit->GetIsochrone();

  // errors on drift radius
  for (int i = 0; i < NparHitRep; i++)
    for (int j = 0; j < NparHitRep; j++)
      fHitCov[i][j] = 0.;
  //   fHitCov[6][6] = 0.0100 * 0.0100;
  fHitCov[6][6] = pow(currenthit->GetIsochroneError(), 2);

  // cut on distance
  fPolicy.setMaxDistance(0.5);
}

// TO BE CHANGED TO USE TUBE ARRAY
PndFtsRecoHit::PndFtsRecoHit(PndFtsHit *currenthit, TClonesArray *tubeArray) : WireHitRecoHit(NparHitRep)
{

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
  fHitCoord[0][0] = wire1.X();
  fHitCoord[1][0] = wire1.Y();
  fHitCoord[2][0] = wire1.Z();
  fHitCoord[3][0] = wire2.X();
  fHitCoord[4][0] = wire2.Y();
  fHitCoord[5][0] = wire2.Z();
  fHitCoord[6][0] = currenthit->GetIsochrone();

  // errors on drift radius
  for (int i = 0; i < NparHitRep; i++)
    for (int j = 0; j < NparHitRep; j++)
      fHitCov[i][j] = 0.;
  //   fHitCov[6][6] = 0.0100 * 0.0100;
  fHitCov[6][6] = pow(currenthit->GetIsochroneError(), 2);

  // cut on distance
  fPolicy.setMaxDistance(0.5);
}

TMatrixT<double> PndFtsRecoHit::getHMatrix(const GFAbsTrackRep *stateVector)
{
  if (dynamic_cast<const GeaneTrackRep *>(stateVector) != nullptr) {
    TMatrixT<double> HMatrix(1, 5);

    HMatrix[0][0] = 0.;
    HMatrix[0][1] = 0.;
    HMatrix[0][2] = 0.;
    HMatrix[0][3] = 1.;
    HMatrix[0][4] = 0.;

    return HMatrix;
  } else if (dynamic_cast<const RKTrackRep *>(stateVector) !=
             nullptr) { // CHECK this has been added but not tested: if you are not going to use it for testing purposes, please use GeaneTrackRep
    TMatrixT<double> HMatrix(1, 5);

    HMatrix[0][0] = 0.;
    HMatrix[0][1] = 0.;
    HMatrix[0][2] = 0.;
    HMatrix[0][3] = 1.;
    HMatrix[0][4] = 0.;

    return HMatrix;
  } else {
    std::cerr << "PndFtsRecoHit can only handle state"
              << " vectors of type GeaneTrackRep -> abort" << std::endl;
    throw;
  }
}
