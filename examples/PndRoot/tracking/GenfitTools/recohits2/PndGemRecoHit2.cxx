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
//      Implementation of class PndGemRecoHit
//      see PndGemRecoHit.h for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//      Ralf Kliemt          TUD            (application to PndMvd)
//      Radoslaw Karabowicz  GSI            (conversion to GEM)
//
//-----------------------------------------------------------

// C/C++ Headers ----------------------
// root Headers ----------------------
#include "TMatrixT.h"
#include "TMath.h"
#include <TFile.h>
// Collaborating Class Headers --------
#include "FairMCPoint.h"
#include "RKTrackRep.h"
#include "PlanarMeasurement.h"
// This Class' Header ------------------
#include "PndGemRecoHit2.h"
#include "PndGemMCPoint.h"
#include "PndGemHit.h"
//#include "PndGemGeoHandling.h"
#include "TGeoManager.h"
#include "FairRootManager.h"
#include "FairLogger.h"

// Class Member definitions -----------

ClassImp(PndGemRecoHit2);

PndGemRecoHit2::~PndGemRecoHit2()
{
  //   if(fGeoH!=0)
  //   delete (fGeoH);
}

PndGemRecoHit2::PndGemRecoHit2() : genfit::PlanarMeasurement(fNparHitRep)
{
  //  fGeoH = new PndGemGeoHandling(gGeoManager);
}

PndGemRecoHit2::PndGemRecoHit2(PndGemMCPoint *point) : genfit::PlanarMeasurement(fNparHitRep)
{
  //  std::cout<<" -I- PndGemRecoHit::PndGemRecoHit(PndGemMCPoint*) called."<<std::endl;

  rawHitCoords_[0] = point->GetX();
  rawHitCoords_[1] = point->GetY();

  rawHitCov_[0][0] = 0.01;
  rawHitCov_[1][1] = 0.01;

  TVector3 o(0., 0., point->GetZ()), u(1., 0., 0.), v(0., 1., 0.);

  genfit::SharedPlanePtr plane(new genfit::DetPlane(o, u, v));
  setPlane(plane);
}

PndGemRecoHit2::PndGemRecoHit2(PndGemHit *hit, const genfit::TrackCandHit *candHit) : genfit::PlanarMeasurement(fNparHitRep)
{

	setDetId(candHit->getDetId());
	setHitId(candHit->getHitId());

	FairRootManager *ioman = FairRootManager::Instance();

	Double_t hitX = hit->GetX();
	Double_t hitY = hit->GetY();

	TVector3 oo(0., 0., hit->GetZ());
	TVector3 uu;
	TVector3 vv;

	if (hit->GetSensorNr() == 1){ //Phi, r sensor
		Double_t phiValue = TMath::ATan2(hitY, hitX);


		uu.SetXYZ(TMath::Cos(phiValue), TMath::Sin(phiValue), 0.);  // phi vector
		vv.SetXYZ(-TMath::Sin(phiValue), TMath::Cos(phiValue), 0.); // perpendicular to phi

		rawHitCoords_[0] = TMath::Sqrt(hitX * hitX + hitY * hitY);	// length (radius) along phi vector
		rawHitCoords_[1] = 0.;

		rawHitCov_[0][0] = hit->GetDr() * hit->GetDr();		//var in r
		rawHitCov_[1][1] = hit->GetDp() * hit->GetDp();		//var in phi

		genfit::SharedPlanePtr plane(new genfit::DetPlane(oo, uu, vv));

	} else if (hit->GetSensorNr() == 2){ //x y sensor

		uu.SetXYZ(1., 0., 0.);
		vv.SetXYZ(0., 1., 0.);

		rawHitCoords_[0] = hitX;
		rawHitCoords_[1] = hitY;

		rawHitCov_[0][0] = hit->GetDx();
		rawHitCov_[1][1] = hit->GetDy();

	} else {
		LOG(error) << "GEM Sensor Number not defined! " << hit->GetSensorNr() << " should either be 1 or 2";
	}

	genfit::SharedPlanePtr plane(new genfit::DetPlane(oo, uu, vv));
	setPlane(plane, candHit->getPlaneId());

//  // Old Implementation which does not work for x,y GEM planes
//  //   std::cout<<" -I- PndGemRecoHit::PndGemRecoHit(PndGemHit*) called."<<std::endl;
//  //  std::cout<<*hit<<std::endl;
//
//  setDetId(candHit->getDetId());
//  setHitId(candHit->getHitId());
//
//  FairRootManager *ioman = FairRootManager::Instance();
//  TString fGeoFile = ioman->GetInFile()->GetName();
//  //  PndGemGeoHandling* fGeoH = new PndGemGeoHandling(fGeoFile.Data());
//  // TString path = fGeoH->GetPath(id);
//
//  Double_t hitX = hit->GetX(), hitY = hit->GetY();
//  Double_t phiAValue = TMath::ATan(hitX / hitY);
//  if (hitY < 0)
//    phiAValue += TMath::Pi();
//  else if (hitX < 0)
//    phiAValue += 2. * TMath::Pi();
//
//  TVector3 oo(0., 0., hit->GetZ()),
//    //            uu ( TMath::Cos(phiAValue),TMath::Sin(phiAValue),0),
//    //            vv (-TMath::Sin(phiAValue),TMath::Cos(phiAValue),0);
//    uu(TMath::Sin(phiAValue), TMath::Cos(phiAValue), 0), vv(TMath::Cos(phiAValue), -TMath::Sin(phiAValue), 0);
//
//  rawHitCoords_[0] = TMath::Sqrt(hitX * hitX + hitY * hitY);
//  rawHitCoords_[1] = 0.;
//
//  rawHitCov_[0][0] = hit->GetDr() * hit->GetDr();
//  rawHitCov_[1][1] = hit->GetDp() * hit->GetDp();
//
//  genfit::SharedPlanePtr plane(new genfit::DetPlane(oo, uu, vv));
//  setPlane(plane, candHit->getPlaneId());
  //============================================================================
}
