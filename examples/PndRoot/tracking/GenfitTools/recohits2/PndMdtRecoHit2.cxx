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
#include "PndMdtRecoHit2.h"

// C/C++ Headers ----------------------
#include <assert.h>

// Collaborating Class Headers --------
#include "PndMdtPoint.h"
#include "PndMdtHit.h"

// Class Member definitions -----------

ClassImp(PndMdtRecoHit2)

PndMdtRecoHit2::~PndMdtRecoHit2() {}

PndMdtRecoHit2::PndMdtRecoHit2() : genfit::SpacepointMeasurement(NparHitRep), fHit(nullptr) {}

PndMdtRecoHit2::PndMdtRecoHit2(double x, double y, double z, double sigx, double sigy, double sigz) : genfit::SpacepointMeasurement(NparHitRep), fHit(nullptr)
{
  rawHitCoords_[0] = x;
  rawHitCoords_[1] = y;
  rawHitCoords_[2] = z;

  rawHitCov_[0][0] = sigx * sigx;
  rawHitCov_[1][1] = sigy * sigy;
  rawHitCov_[2][2] = sigz * sigz;
}

PndMdtRecoHit2::PndMdtRecoHit2(const TVector3 &pos, const TVector3 &sig, bool smear) : genfit::SpacepointMeasurement(NparHitRep), fHit(nullptr)
{
  if (!smear) {
    rawHitCoords_[0] = pos.X();
    rawHitCoords_[1] = pos.Y();
    rawHitCoords_[2] = pos.Z();
  } else {
    rawHitCoords_[0] = gRandom->Gaus(pos.X(), sig.X());
    rawHitCoords_[1] = gRandom->Gaus(pos.Y(), sig.Y());
    rawHitCoords_[2] = gRandom->Gaus(pos.Z(), sig.Z());
  }
  rawHitCov_[0][0] = sig.X() * sig.X();
  rawHitCov_[1][1] = sig.Y() * sig.Y();
  rawHitCov_[2][2] = sig.Z() * sig.Z();
}

PndMdtRecoHit2::PndMdtRecoHit2(PndMdtPoint *point) : genfit::SpacepointMeasurement(NparHitRep), fHit(nullptr)
{
  rawHitCoords_[0] = point->GetX();
  rawHitCoords_[1] = point->GetY();
  rawHitCoords_[2] = point->GetZ();
  // fixed errors on the monte carlo points
  rawHitCov_[0][0] = 0.5;
  rawHitCov_[1][1] = 0.5;
  rawHitCov_[2][2] = 0.5;
}

PndMdtRecoHit2::PndMdtRecoHit2(PndMdtHit *hit, const genfit::TrackCandHit *candHit) : genfit::SpacepointMeasurement(NparHitRep)
{
  setDetId(candHit->getDetId());
  setHitId(candHit->getHitId());

  TVector3 hitpos(0, 0, 0);
  hit->Position(hitpos);
  rawHitCoords_[0] = hitpos.X();
  rawHitCoords_[1] = hitpos.Y();
  rawHitCoords_[2] = hitpos.Z();

  TVector3 sig(0, 0, 0);
  hit->PositionError(sig);
  rawHitCov_[0][0] = sig.X() * sig.X();
  rawHitCov_[1][1] = sig.Y() * sig.Y();
  rawHitCov_[2][2] = sig.Z() * sig.Z();

  fHit = hit;
}
