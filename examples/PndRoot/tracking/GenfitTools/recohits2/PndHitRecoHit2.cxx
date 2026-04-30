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
//      Implementation of class PndHitRecoHit2
//      see PndHitRecoHit2.h for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//      Ralf Kliemt          Uni Bonn       (application to FairHits)
//
//-----------------------------------------------------------

// C/C++ Headers ----------------------
// root Headers ----------------------
#include "TMatrixT.h"
#include "TMath.h"
// Collaborating Class Headers --------
// This Class' Header ------------------
#include "PndHitRecoHit2.h"

// Class Member definitions -----------

ClassImp(PndHitRecoHit2);

PndHitRecoHit2::~PndHitRecoHit2() {}

PndHitRecoHit2::PndHitRecoHit2() : SpacepointMeasurement(fNparHitRep) {}

PndHitRecoHit2::PndHitRecoHit2(FairMCPoint *point) : SpacepointMeasurement(fNparHitRep)
{
  // std::cout<<" -I- PndHitRecoHit::PndHitRecoHit(FairMCPoint*) called."<<std::endl;

  rawHitCoords_[0] = point->GetX();
  rawHitCoords_[1] = point->GetY();
  rawHitCoords_[2] = point->GetZ();

  // fixed errors on the monte carlo points
  // we set the covariances to (500 mum)^2 by hand.
  Double_t sigmasq = 0.05 * 0.05; // cm //TODO: cm is rigt?
  rawHitCov_[0][0] = sigmasq;
  rawHitCov_[1][1] = sigmasq;
  rawHitCov_[2][2] = sigmasq;
}

PndHitRecoHit2::PndHitRecoHit2(FairHit *hit) : SpacepointMeasurement(fNparHitRep)
{
  //  std::cout<<" -I- PndHitRecoHit::PndHitRecoHit(PndSdsHit*) called."<<std::endl;

  rawHitCoords_[0] = hit->GetX();
  rawHitCoords_[1] = hit->GetY();
  rawHitCoords_[2] = hit->GetZ();

  rawHitCov_[0][0] = hit->GetDx() * hit->GetDx();
  rawHitCov_[1][1] = hit->GetDy() * hit->GetDy();
  rawHitCov_[2][2] = hit->GetDz() * hit->GetDz();
}
//============================================================================
