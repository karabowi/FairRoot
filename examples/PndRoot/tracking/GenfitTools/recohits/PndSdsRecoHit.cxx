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

//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndSdsRecoHit
//      see PndSdsRecoHit.h for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//      Ralf Kliemt           TUD            (application to PndSds)
//
//-----------------------------------------------------------

// C/C++ Headers ----------------------
// root Headers ----------------------
#include "TMatrixT.h"
#include "TMath.h"
// Collaborating Class Headers --------
#include "FairMCPoint.h"
#include "GeaneTrackRep.h"
#include "RKTrackRep.h"
#include "GFDetPlane.h"
// This Class' Header ------------------
#include "PndSdsRecoHit.h"
#include "PndSdsMCPoint.h"
#include "PndSdsHit.h"
#include "PndGeoHandling.h"
#include "TGeoManager.h"
#include "FairRootManager.h"

// Class Member definitions -----------

ClassImp(PndSdsRecoHit);

PndSdsRecoHit::~PndSdsRecoHit() {}

PndSdsRecoHit::PndSdsRecoHit() : GFRecoHitIfc<GFPlanarHitPolicy>(fNparHitRep) {}

PndSdsRecoHit::PndSdsRecoHit(PndSdsMCPoint *point) : GFRecoHitIfc<GFPlanarHitPolicy>(fNparHitRep)
{
  std::cout << " -I- PndSdsRecoHit::PndSdsRecoHit(PndSdsMCPoint*) called." << std::endl;

  fHitCoord[0][0] = point->GetX();
  fHitCoord[1][0] = point->GetY();

  // we set the covariances to (50mu)^2 by hand.
  fHitCov[0][0] = 0.0050 * 0.0050; // cm //TODO: cm is rigt?
  fHitCov[1][1] = 0.0050 * 0.0050; // cm //TODO: cm is rigt?

  TVector3 o(0., 0., point->GetZ()), u(1., 0., 0.), v(0., 1., 0.);

  fPolicy.setDetPlane(GFDetPlane(o, u, v));
}

PndSdsRecoHit::PndSdsRecoHit(PndSdsHit *hit) : GFRecoHitIfc<GFPlanarHitPolicy>(fNparHitRep)
{

  //  std::cout<<" -I- PndSdsRecoHit::PndSdsRecoHit(PndSdsHit*) called."<<std::endl;
  //  std::cout<<*hit<<std::endl;

  Int_t id = hit->GetSensorID();

  //  FairRootManager* ioman = FairRootManager::Instance();
  //  TString fGeoFile = ioman->GetInFile()->GetName();
  fGeoH = PndGeoHandling::Instance();
  TString path = fGeoH->GetPath(id);
  //  std::cout<<"Detector path: "<<path.Data()<<std::endl;
  TVector3 oo, uu, vv;
  fGeoH->GetOUVShortId(id, oo, uu, vv);

  TVector3 position = hit->GetPosition();
  TVector3 localpos = fGeoH->MasterToLocalShortId(position, id);

  fHitCoord[0][0] = localpos.X();
  fHitCoord[1][0] = localpos.Y();

  TMatrixD cova = fGeoH->MasterToLocalErrorsShortId(hit->GetCov(), id);
  // project only the 2 dimensions of cov.
  fHitCov[0][0] = cova[0][0];
  fHitCov[0][1] = cova[0][1];
  fHitCov[1][0] = cova[1][0];
  fHitCov[1][1] = cova[1][1];

  //  std::cout<<" -I- PndSdsRecoHit::PndSdsRecoHit: Wrote a hit with"
  //  <<"\n(x,y) = ("<<localpos.X()<<","<<localpos.Y()<<")."
  //  <<"\nCovariance Matrix is";
  //  fHitCov.Print();
  //  std::cout<<"From 3D hit matrix";
  //  cova.Print();

  fPolicy.setDetPlane(GFDetPlane(oo, uu, vv));
}
//============================================================================

TMatrixT<double> PndSdsRecoHit::getHMatrix(const GFAbsTrackRep *stateVector)
{

  if (dynamic_cast<const RKTrackRep *>(stateVector) != nullptr) {
    // Uses TrackParP (q/p,v',w',v,w)
    // coordinates are defined by detplane!
    TMatrixT<double> HMatrix(2, 5);

    HMatrix[0][0] = 0.;
    HMatrix[0][1] = 0.;
    HMatrix[0][2] = 0.;
    HMatrix[0][3] = 1.;
    HMatrix[0][4] = 0.;

    HMatrix[1][0] = 0.;
    HMatrix[1][1] = 0.;
    HMatrix[1][2] = 0.;
    HMatrix[1][3] = 0.;
    HMatrix[1][4] = 1.;

    return HMatrix;
  }

  // !! TODO I copied this from the DemoRecoHit - check validity!!!
  if (dynamic_cast<const GeaneTrackRep *>(stateVector) != nullptr) {
    // Uses TrackParP (q/p,v',w',v,w)
    // coordinates are defined by detplane!
    TMatrixT<double> HMatrix(fNparHitRep, 5);

    HMatrix[0][0] = 0.;
    HMatrix[0][1] = 0.;
    HMatrix[0][2] = 0.;
    HMatrix[0][3] = 1.;
    HMatrix[0][4] = 0.;

    HMatrix[1][0] = 0.;
    HMatrix[1][1] = 0.;
    HMatrix[1][2] = 0.;
    HMatrix[1][3] = 0.;
    HMatrix[1][4] = 1.;
    return HMatrix;
  } else {
    std::cerr << "PndSdsRecoHit can only handle state"
              << " vectors of type GeaneTrackRep -> abort" << std::endl;
    throw;
  }
}

Double_t PndSdsRecoHit::residualScalar(GFAbsTrackRep *,
                                       const TMatrixT<Double_t> &) // stateVector state// [R.K.03/2017] unused variable(s)
{
  throw;
}
