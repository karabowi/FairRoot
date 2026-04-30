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
#include "DetPlane.h"
// This Class' Header ------------------
#include "PndSdsRecoHit2.h"
#include "PndSdsMCPoint.h"
#include "PndSdsHit.h"
#include "PndGeoHandling.h"
#include "TGeoManager.h"
#include "FairRootManager.h"

// Class Member definitions -----------

ClassImp(PndSdsRecoHit2);

PndSdsRecoHit2::~PndSdsRecoHit2() {}

PndSdsRecoHit2::PndSdsRecoHit2() : PlanarMeasurement(fNparHitRep) {}

PndSdsRecoHit2::PndSdsRecoHit2(PndSdsMCPoint *point) : PlanarMeasurement(fNparHitRep)
{
  std::cout << " -I- PndSdsRecoHit2::PndSdsRecoHit2(PndSdsMCPoint*) called." << std::endl;

  rawHitCoords_[0] = point->GetX();
  rawHitCoords_[1] = point->GetY();

  // we set the covariances to (50mu)^2 by hand.
  rawHitCov_[0][0] = 0.0050 * 0.0050; // cm //TODO: cm is rigt?
  rawHitCov_[1][1] = 0.0050 * 0.0050; // cm //TODO: cm is rigt?

  TVector3 o(0., 0., point->GetZ()), u(1., 0., 0.), v(0., 1., 0.);

  genfit::SharedPlanePtr plane(new genfit::DetPlane(o, u, v));
  setPlane(plane);
}

PndSdsRecoHit2::PndSdsRecoHit2(PndSdsHit *hit, const genfit::TrackCandHit *candHit) : PlanarMeasurement(fNparHitRep)
{

  //  std::cout<<" -I- PndSdsRecoHit::PndSdsRecoHit(PndSdsHit*) called."<<std::endl;
  //  std::cout<<*hit<<std::endl;

  setDetId(candHit->getDetId());
  setHitId(candHit->getHitId());

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

  rawHitCoords_[0] = localpos.X();
  rawHitCoords_[1] = localpos.Y();

  TMatrixD cova = fGeoH->MasterToLocalErrorsShortId(hit->GetCov(), id);
  // project only the 2 dimensions of cov.
  rawHitCov_[0][0] = cova[0][0];
  rawHitCov_[0][1] = cova[0][1];
  rawHitCov_[1][0] = cova[1][0];
  rawHitCov_[1][1] = cova[1][1];

  //  std::cout<<" -I- PndSdsRecoHit::PndSdsRecoHit: Wrote a hit with"
  //  <<"\n(x,y) = ("<<localpos.X()<<","<<localpos.Y()<<")."
  //  <<"\nCovariance Matrix is";
  //  rawHitCov_.Print();
  //  std::cout<<"From 3D hit matrix";
  //  cova.Print();

  genfit::SharedPlanePtr plane(new genfit::DetPlane(oo, uu, vv));
  setPlane(plane, candHit->getPlaneId());
}
//============================================================================
