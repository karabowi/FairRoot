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

// -----                      PndSttHelixHit source file               -----
// -----                                                               -----
// -------------------------------------------------------------------------

#include "PndSttHelixHit.h"

#include <iostream>
#include "TMath.h"
#include "TVector3.h"
#include "PndSttHit.h"

#include "PndDetectorList.h"

using namespace std;

/** Default constructor **/
PndSttHelixHit::PndSttHelixHit() : FairHit(), fHitIndex(0), fdEdx(0), fIsochrone(0), fIsochroneError(0), fTubeID(0)
{
  Clear();
}

PndSttHelixHit::PndSttHelixHit(Int_t detID, Int_t tubeID, Int_t hitindex, Int_t mcindex, TVector3 &pos, TVector3 &dpos, Double_t isochrone, Double_t isochroneError, Double_t dedx)
  : FairHit(detID, pos, dpos, mcindex), fHitIndex(hitindex), fdEdx(dedx), fIsochrone(isochrone), fIsochroneError(isochroneError), fTubeID(tubeID)
{
  SetLink(FairLink("STTHit", hitindex));
}

// DELETE THIS CHECK
// void PndSttHelixHit::CopyHitToHelixHit(PndSttHit *aHit, Int_t hitindex)
//{

//}

/** Public method Clear **/
void PndSttHelixHit::Clear()
{
  fHitIndex = 0;
  fdEdx = 0.;
  fTubeID = 0;
  fIsochrone = 0.;
  fIsochroneError = 0.;
  ResetLinks();
}

/** Destructor **/
PndSttHelixHit::~PndSttHelixHit() {}

void PndSttHelixHit::Print()
{
  // reco & center positions
  cout << "position " << GetX() << " " << GetY() << " " << GetZ() << endl;

  // index
  cout << "mc, hit indexes " << GetRefIndex() << " " << GetHitIndex() << endl;

  // drift
  cout << "drift radius " << GetIsochrone() << endl;
}

ClassImp(PndSttHelixHit)
