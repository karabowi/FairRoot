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
//      Implementation of class DemoSPHit
//      see DemoSPHit.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
// modified for hyp purpose by A.sanchez
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndHypRecoSPHit.h"

// C/C++ Headers ----------------------
#include <assert.h>

// Collaborating Class Headers --------
#include "FairMCPoint.h"
#include "FairHit.h"
#include "LSLTrackRep.h"
#include "GeaneTrackRep.h"
//#include "DetPlane.h"
#include "PndHypHit.h"
// Class Member definitions -----------

ClassImp(PndHypRecoSPHit)

  PndHypRecoSPHit::~PndHypRecoSPHit()
{
}

PndHypRecoSPHit::PndHypRecoSPHit() : SpacepointRecoHit(NparHitRep) {}

PndHypRecoSPHit::PndHypRecoSPHit(double x, double y, double z, double sigx, double sigy, double sigz) : SpacepointRecoHit(NparHitRep)
{
  fHitCoord[0][0] = x;
  fHitCoord[1][0] = y;
  fHitCoord[2][0] = z;

  fHitCov[0][0] = sigx * sigx;
  fHitCov[1][1] = sigy * sigy;
  fHitCov[2][2] = sigz * sigz;
}

PndHypRecoSPHit::PndHypRecoSPHit(const TVector3 &pos, const TVector3 &sig) : SpacepointRecoHit(NparHitRep)
{
  fHitCoord[0][0] = pos.X();
  fHitCoord[1][0] = pos.Y();
  fHitCoord[2][0] = pos.Z();
  fHitCov[0][0] = sig.X() * sig.X();
  fHitCov[1][1] = sig.Y() * sig.Y();
  fHitCov[2][2] = sig.Z() * sig.Z();
}

PndHypRecoSPHit::PndHypRecoSPHit(PndHypHit *point) // FairMCPoint* point
  : SpacepointRecoHit(NparHitRep)
{
  fHitCoord[0][0] = point->GetX();
  fHitCoord[1][0] = point->GetY();
  fHitCoord[2][0] = point->GetZ();
  // fixed errors on the monte carlo points
  fHitCov[0][0] = 0.5;
  fHitCov[1][1] = 0.5;
  fHitCov[2][2] = 0.5;
}

GFAbsRecoHit *PndHypRecoSPHit::clone()
{
  return new PndHypRecoSPHit(*this);
}

TMatrixT<double> PndHypRecoSPHit::getHMatrix(const GFAbsTrackRep *stateVector)
{
  assert(stateVector != nullptr);
  if (dynamic_cast<const GeaneTrackRep *>(stateVector) != nullptr) {
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
  } else if (dynamic_cast<const LSLTrackRep *>(stateVector) != nullptr) {
    // Uses TrackParP (u,v,u',v',q/p)
    // coordinates are defined by detplane!
    TMatrixT<double> HMatrix(2, 5);

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
    std::cerr << "PndHypRecoSPHit can only handle state"
              << " vectors of type GeaneTrackRep or LSLTrackRep-> abort" << std::endl;
    throw;
  }
}
