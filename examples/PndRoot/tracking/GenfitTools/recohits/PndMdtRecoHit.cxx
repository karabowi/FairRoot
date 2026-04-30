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
//      Implementation of class PndMdtRecoHit
//      see PndMdtRecoHit.h for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//      Stefano Spataro      UNITO          (adapted for MDT)
//
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndMdtRecoHit.h"

// C/C++ Headers ----------------------
#include <assert.h>

// Collaborating Class Headers --------
#include "PndMdtPoint.h"
#include "PndMdtHit.h"
#include "GeaneTrackRep.h"

// Class Member definitions -----------

ClassImp(PndMdtRecoHit)

PndMdtRecoHit::~PndMdtRecoHit() {}

PndMdtRecoHit::PndMdtRecoHit() : SpacepointRecoHit(NparHitRep), fHit(nullptr) {}

PndMdtRecoHit::PndMdtRecoHit(double x, double y, double z, double sigx, double sigy, double sigz) : SpacepointRecoHit(NparHitRep), fHit(nullptr)
{
  fHitCoord[0][0] = x;
  fHitCoord[1][0] = y;
  fHitCoord[2][0] = z;

  fHitCov[0][0] = sigx * sigx;
  fHitCov[1][1] = sigy * sigy;
  fHitCov[2][2] = sigz * sigz;
}

PndMdtRecoHit::PndMdtRecoHit(const TVector3 &pos, const TVector3 &sig, bool smear) : SpacepointRecoHit(NparHitRep), fHit(nullptr)
{
  if (!smear) {
    fHitCoord[0][0] = pos.X();
    fHitCoord[1][0] = pos.Y();
    fHitCoord[2][0] = pos.Z();
  } else {
    fHitCoord[0][0] = gRandom->Gaus(pos.X(), sig.X());
    fHitCoord[1][0] = gRandom->Gaus(pos.Y(), sig.Y());
    fHitCoord[2][0] = gRandom->Gaus(pos.Z(), sig.Z());
  }
  fHitCov[0][0] = sig.X() * sig.X();
  fHitCov[1][1] = sig.Y() * sig.Y();
  fHitCov[2][2] = sig.Z() * sig.Z();
}

PndMdtRecoHit::PndMdtRecoHit(PndMdtPoint *point) : SpacepointRecoHit(NparHitRep), fHit(nullptr)
{
  fHitCoord[0][0] = point->GetX();
  fHitCoord[1][0] = point->GetY();
  fHitCoord[2][0] = point->GetZ();
  // fixed errors on the monte carlo points
  fHitCov[0][0] = 0.5;
  fHitCov[1][1] = 0.5;
  fHitCov[2][2] = 0.5;
}

PndMdtRecoHit::PndMdtRecoHit(PndMdtHit *hit) : SpacepointRecoHit(NparHitRep)
{
  TVector3 hitpos(0, 0, 0);
  hit->Position(hitpos);
  fHitCoord[0][0] = hitpos.X();
  fHitCoord[1][0] = hitpos.Y();
  fHitCoord[2][0] = hitpos.Z();

  TVector3 sig(0, 0, 0);
  hit->PositionError(sig);
  fHitCov[0][0] = sig.X() * sig.X();
  fHitCov[1][1] = sig.Y() * sig.Y();
  fHitCov[2][2] = sig.Z() * sig.Z();

  fHit = hit;
}

GFAbsRecoHit *PndMdtRecoHit::clone()
{
  return new PndMdtRecoHit(*this);
}

TMatrixT<double> PndMdtRecoHit::getHMatrix(const GFAbsTrackRep *stateVector)
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
  } else {
    std::cerr << "PndMdtRecoHit can only handle state"
              << " vectors of type GeaneTrackRep-> abort" << std::endl;
    throw;
  }
}
