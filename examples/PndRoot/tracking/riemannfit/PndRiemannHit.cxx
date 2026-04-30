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
//      Implementation of class PndRiemannHit
//      see PndRiemannHit.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndRiemannHit.h"
#include "TMath.h"
// C/C++ Headers ----------------------
#include "FairHit.h"
#include <assert.h>
#include <iostream>

// Collaborating Class Headers --------
#include "PndRiemannTrack.h"

// Class Member definitions -----------

// scale factor! r=1 => z'=0.5 !!!
// to normalize cm-range
#define RIEMANNSCALE 1.

ClassImp(PndRiemannHit)

  PndRiemannHit::PndRiemannHit(double mx, double my, double mz, double dx, double dy, double dz)
  : fCovX(3, 3), fHit(0), fS(-1), fZ(mz), fDeltaZ(-1.), fAlpha(-1.), fVerbose(0)
{
  setXYZ(mx, my, mz);
  setDXYZ(dx, dy, dz);
}

PndRiemannHit::PndRiemannHit(FairHit *cl, int myHitID) : fCovX(3, 3), fHit(cl), fHitID(myHitID), fS(-1), fZ(-1.), fDeltaZ(-1.), fAlpha(-1.), fVerbose(0)
{
  setHit(cl);
}

PndRiemannHit::~PndRiemannHit() {}

void PndRiemannHit::setXYZ(double mx, double my, double mz)
{
  TVector2 a(mx, my);
  double r = a.Mod() / RIEMANNSCALE;
  //	double phi=a.Phi();
  double r2 = r * r;

  fX.SetX(mx / RIEMANNSCALE);
  fX.SetY(my / RIEMANNSCALE);
  fX.SetZ(r2);
  fZ = mz;
}

void PndRiemannHit::setDXYZ(double dx, double dy, double dz)
{
  fCovX[0][0] = TMath::Power(dx / RIEMANNSCALE, 2);
  fCovX[1][1] = TMath::Power(dy / RIEMANNSCALE, 2);
  fCovX[2][2] = 4 * (TMath::Power(fX.X(), 2) * fCovX[0][0] + 2 * fX.X() * fX.Y() * dx * dy + TMath::Power(fX.Y(), 2) * fCovX[1][1]) / TMath::Power(RIEMANNSCALE, 2);

  fDeltaZ = dz / RIEMANNSCALE;
}

void PndRiemannHit::setHit(FairHit *cl)
{
  setXYZ(cl->GetX(), cl->GetY(), cl->GetZ());
  setDXYZ(cl->GetDx(), cl->GetDy(), cl->GetDz());
}

double PndRiemannHit::z() const
{
  return fZ;
}

void PndRiemannHit::calcPosOnTrk(PndRiemannTrack *trk)
{
  assert(trk != nullptr);
  TVectorD o = trk->orig();
  o *= RIEMANNSCALE; // convert back to cm;
  double r = trk->r();
  r *= RIEMANNSCALE;
  const PndRiemannHit *firstHit = trk->getHit(0);
  assert(firstHit != nullptr);
  TVector2 k(firstHit->x().X() - o[0], firstHit->x().Y() - o[1]);
  TVector2 l(fX.X() - o[0], fX.Y() - o[1]);
  fAlpha = l.DeltaPhi(k);
  //  if (fAlpha < 0)
  //	  fAlpha += TMath::Pi()*2;
  fS = fAlpha * r;
  if (fVerbose > 1)
    std::cout << "Alpha/r " << fAlpha << "/" << r << "--> z/s " << fHit->GetZ() << "/" << fS << std::endl;
}

double PndRiemannHit::sigmaXY() const
{
  double result = TMath::Sqrt(fCovX[0][0] + fCovX[1][1]);
  if (result == 0.) {
    return 1.;
  } else
    return result;
}
