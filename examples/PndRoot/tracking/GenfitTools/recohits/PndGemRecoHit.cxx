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
#include "GeaneTrackRep.h"
#include "GFDetPlane.h"
// This Class' Header ------------------
#include "PndGemRecoHit.h"
#include "PndGemMCPoint.h"
#include "PndGemHit.h"
//#include "PndGemGeoHandling.h"
#include "TGeoManager.h"
#include "FairRootManager.h"

// Class Member definitions -----------

ClassImp(PndGemRecoHit);

PndGemRecoHit::~PndGemRecoHit()
{
  //   if(fGeoH!=0)
  //   delete (fGeoH);
}

PndGemRecoHit::PndGemRecoHit() : GFRecoHitIfc<GFPlanarHitPolicy>(fNparHitRep)
{
  //  fGeoH = new PndGemGeoHandling(gGeoManager);
}

PndGemRecoHit::PndGemRecoHit(PndGemMCPoint *point) : GFRecoHitIfc<GFPlanarHitPolicy>(fNparHitRep)
{
  //  std::cout<<" -I- PndGemRecoHit::PndGemRecoHit(PndGemMCPoint*) called."<<std::endl;

  fHitCoord[0][0] = point->GetX();
  fHitCoord[1][0] = point->GetY();

  fHitCov[0][0] = 0.01;
  fHitCov[1][1] = 0.01;

  TVector3 o(0., 0., point->GetZ()), u(1., 0., 0.), v(0., 1., 0.);

  fPolicy.setDetPlane(GFDetPlane(o, u, v));
}
/*
PndGemRecoHit::PndGemRecoHit(PndGemHit* hit)
  : GFRecoHitIfc<GFPlanarHitPolicy>(fNparHitRep)
{

  std::cout<<" -I- PndGemRecoHit::PndGemRecoHit(PndGemHit*) called for hit in detector "<< hit->GetDetName() << std::endl;
  //  std::cout<<*hit<<std::endl;

  TString id =  hit->GetDetName();

  FairRootManager* ioman = FairRootManager::Instance();
  TString fGeoFile = ioman->GetInFile()->GetName();
  //  PndGemGeoHandling* fGeoH = new PndGemGeoHandling(fGeoFile.Data());
  //TString path = fGeoH->GetPath(id);

  Double_t hitX = hit->GetX();
  Double_t hitY = hit->GetY();

  TVector3 oo (0.,0.,hit->GetZ()),
    uu ( 1.0, 0.0, 0.0),
    vv ( 0.0, 1.0, 0.0);

  fHitCoord[0][0] = hitX;
  fHitCoord[1][0] = hitY;

  fHitCov[0][0] = 0.0001;
  fHitCov[1][1] = 0.0001;

  fPolicy.setDetPlane(DetPlane(oo,uu,vv));
//============================================================================
}
*/

PndGemRecoHit::PndGemRecoHit(PndGemHit *hit) : GFRecoHitIfc<GFPlanarHitPolicy>(fNparHitRep)
{

  //   std::cout<<" -I- PndGemRecoHit::PndGemRecoHit(PndGemHit*) called."<<std::endl;
  //  std::cout<<*hit<<std::endl;

  FairRootManager *ioman = FairRootManager::Instance();
  TString fGeoFile = ioman->GetInFile()->GetName();
  //  PndGemGeoHandling* fGeoH = new PndGemGeoHandling(fGeoFile.Data());
  // TString path = fGeoH->GetPath(id);

  Double_t hitX = hit->GetX(), hitY = hit->GetY();
  Double_t phiAValue = TMath::ATan(hitX / hitY);
  if (hitY < 0)
    phiAValue += TMath::Pi();
  else if (hitX < 0)
    phiAValue += 2. * TMath::Pi();

  TVector3 oo(0., 0., hit->GetZ()),
    //            uu ( TMath::Cos(phiAValue),TMath::Sin(phiAValue),0),
    //            vv (-TMath::Sin(phiAValue),TMath::Cos(phiAValue),0);
    uu(TMath::Sin(phiAValue), TMath::Cos(phiAValue), 0), vv(TMath::Cos(phiAValue), -TMath::Sin(phiAValue), 0);

  fHitCoord[0][0] = TMath::Sqrt(hitX * hitX + hitY * hitY);
  fHitCoord[1][0] = 0.;

  fHitCov[0][0] = hit->GetDr() * hit->GetDr();
  fHitCov[1][1] = hit->GetDp() * hit->GetDp();

  if (hit->GetDp() < 1.e-8) {
    uu.SetXYZ(1., 0., 0.);
    vv.SetXYZ(0., 1., 0.);

    fHitCoord[0][0] = hitX;
    fHitCoord[1][0] = hitY;

    fHitCov[0][0] = hit->GetDx() * hit->GetDx();
    fHitCov[1][1] = hit->GetDy() * hit->GetDy();
  }

  fPolicy.setDetPlane(GFDetPlane(oo, uu, vv));
  //============================================================================
}

TMatrixT<double> PndGemRecoHit::getHMatrix(const GFAbsTrackRep *stateVector)
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
    std::cerr << "DemoRecoHit can only handle state"
              << " vectors of type GeaneTrackRep -> abort" << std::endl;
    throw;
  }
}

Double_t PndGemRecoHit::residualScalar(GFAbsTrackRep *,
                                       const TMatrixT<Double_t> &) // stateVector state // [R.K.03/2017] unused variable(s)
{
  throw;
}
