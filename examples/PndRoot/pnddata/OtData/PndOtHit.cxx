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

/**
last revision December, 2012
 **/

#include "PndOtHit.h"

#include "PndDetectorList.h"

#include <iostream>
#include "TMath.h"
/** Default constructor **/
PndOtHit::PndOtHit() : fIsochrone(0.), fIsochroneError(0.), fPulse(0.), fDepCharge(0.), fTubeID(0), fChamberID(0), fLayerID(0), fSkewed(0)
{
  // ClearHit();
}

PndOtHit::PndOtHit(Int_t detID, Int_t tubeID, Int_t chamberID, Int_t layerID, Int_t skew, Int_t mcindex, TVector3 &pos, TVector3 &dpos, Double_t p, Double_t isochrone,
                     Double_t isochroneError, Double_t chDep)
  : FairHit(detID, pos, dpos, mcindex), fIsochrone(isochrone), fIsochroneError(isochroneError), fPulse(p), fDepCharge(chDep), fTubeID(tubeID), fChamberID(chamberID),
    fLayerID(layerID), fSkewed(skew)
{
  SetTimeStamp(p);
  SetLink(FairLink("OTPoint", mcindex));
}

/** Public method ClearHit **/
void PndOtHit::ClearHit()
{
  fTubeID = 0;
  fChamberID = 0;
  fLayerID = 0;
  fSkewed = 0;
  fPulse = 0;
  fIsochrone = 0.;
  fIsochroneError = 0.;
  fDepCharge = 0.;
}

/** Destructor **/
PndOtHit::~PndOtHit() {}

ClassImp(PndOtHit)
