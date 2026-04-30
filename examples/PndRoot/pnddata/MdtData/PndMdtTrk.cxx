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

#include "PndMdtTrk.h"
//#include "PndTrackCand.h"

// -----   Default constructor   -------------------------------------------
PndMdtTrk::PndMdtTrk()
  : fHitList(), fHitMult(), fHitDist(), fHitDeltaAngle(), fLayDist(), fIronDist(0.), fHitCount(0), fLayerCount(0), fMaxLayer(0), fHitBit(0), fModule(0), fChi2(0.)
{
  Clear();
}

/** Destructor **/
PndMdtTrk::~PndMdtTrk() {}

PndTrackCand *PndMdtTrk::AddTrackCand(const PndTrackCand *inTrackCand)
{
  PndTrackCand *outTrackCand = (PndTrackCand *)inTrackCand->Clone();
  for (Int_t ii = 0; ii < 25; ii++) {
    if (fHitList[ii] != -1)
      outTrackCand->AddHit(static_cast<int>(DetectorType::kMdtHit), fHitList[ii], (250 + ii * 10.) * (250 + ii * 10.));
  }
  return outTrackCand;
}

// -----   Clear function   -------------------------------------------
void PndMdtTrk::Clear()
{
  for (Int_t ii = 0; ii < 25; ii++) {
    fHitList[ii] = -1;
    fHitMult[ii] = 0;
    fHitDist[ii] = -1.;
    fLayDist[ii] = 0.;
  }
  fIronDist = 0.;
  fHitCount = 0;
  fLayerCount = 0;
  fMaxLayer = 0;
  fHitBit = 0;
  fModule = 0;
  fChi2 = -1;
}

ClassImp(PndMdtTrk)
