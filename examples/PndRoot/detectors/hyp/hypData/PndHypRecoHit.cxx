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
//      Ralf Kliemt           TUD            (application to PndMvd)
//
//-----------------------------------------------------------

// C/C++ Headers ----------------------
// root Headers ----------------------

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
//      Ralf Kliemt           TUD            (application to PndMvd)
//
//-----------------------------------------------------------

// C/C++ Headers ----------------------
// root Headers ----------------------
#include "TMatrixT.h"
#include "TMath.h"
// Collaborating Class Headers --------
#include "FairMCPoint.h"
#include "LSLTrackRep.h"
#include "GeaneTrackRep.h"
#include "GFDetPlane.h"
// This Class' Header ------------------
#include "PndHypRecoHit.h"
#include "PndHypPoint.h"
#include "PndHypHit.h"
#include "PndHypHit.h"
#include "PndHypGeoHandling.h"
#include "FairRootManager.h"
// Class Member definitions -----------

ClassImp(PndHypRecoHit);

PndHypRecoHit::~PndHypRecoHit()
{
  if (fGeoH != 0)
    delete (fGeoH);
}

PndHypRecoHit::PndHypRecoHit() : GFRecoHitIfc<GFPlanarHitPolicy>(fNparHitRep)
{
  fGeoH = new PndHypGeoHandling(gGeoManager);
}

PndHypRecoHit::PndHypRecoHit(PndHypPoint *point) : GFRecoHitIfc<GFPlanarHitPolicy>(fNparHitRep)
{
  std::cout << " -I- PndHypRecoHit::PndHypRecoHit(PndHypMCPoint*) called." << std::endl;

  fHitCoord[0][0] = point->GetX();
  fHitCoord[1][0] = point->GetY();

  fHitCov[0][0] = 0.01; // cost*sigx*sigx;
  fHitCov[1][1] = 0.01; // cost*sigy*sigy;

  TVector3 o(0., 0., point->GetZ()), u(1., 0., 0.), v(0., 1., 0.);

  fPolicy.setDetPlane(GFDetPlane(o, u, v));
}

PndHypRecoHit::PndHypRecoHit(PndHypHit *hit) : GFRecoHitIfc<GFPlanarHitPolicy>(fNparHitRep)
{

  // std::cout<<" -I- PndHypRecoHit::PndHypRecoHit(PndHypHit*) called."<<std::endl;
  // std::cout<<*hit<<std::endl;

  TString id = hit->GetDetName();

  FairRootManager *ioman = FairRootManager::Instance();
  TString fGeoFile = ioman->GetInFile()->GetName();
  fGeoH = new PndHypGeoHandling(fGeoFile.Data());

  TVector3 oo, uu, vv;
  fGeoH->GetOUVId(id, oo, uu, vv);

  TVector3 position = hit->GetPosition();
  TVector3 localpos = fGeoH->MasterToLocalId(position, id);

  fHitCoord[0][0] = localpos.X();
  fHitCoord[1][0] = localpos.Y();

  fHitCov[0][0] = 0.0050 * 0.0050;
  fHitCov[1][1] = 0.0050 * 0.0050;

  fPolicy.setDetPlane(GFDetPlane(oo, uu, vv));
}

TMatrixT<double> PndHypRecoHit::getHMatrix(const GFAbsTrackRep *stateVector)
{

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
  } else if (dynamic_cast<const LSLTrackRep *>(stateVector) != nullptr) {
    // LSLTrackRep (x,y,x',y',q/p)
    TMatrixT<double> HMatrix(fNparHitRep, 5);

    HMatrix[0][0] = 1.;
    HMatrix[0][1] = 0.;
    HMatrix[0][2] = 0.;
    HMatrix[0][3] = 0.;
    HMatrix[0][4] = 0.;

    HMatrix[1][0] = 0.;
    HMatrix[1][1] = 1.;
    HMatrix[1][2] = 0.;
    HMatrix[1][3] = 0.;
    HMatrix[1][4] = 0.;
    return HMatrix;

  } else {
    std::cerr << "DemoRecoHit can only handle state"
              << " vectors of type LSLTrackRep or GeaneTrackRep -> abort" << std::endl;
    throw;
  }
}

Double_t PndHypRecoHit::residualScalar(GFAbsTrackRep *stateVector, const TMatrixT<Double_t> &state)
{
  throw;
}
