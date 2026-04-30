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

// -------------------------------------------------------------------------
// -----                PndMdtHitProducerIdeal source file             -----
// -----                  Created 11/06/08  by  S.Spataro              -----
// -------------------------------------------------------------------------

#include "PndMdtHitProducerIdeal.h"

#include "PndMdtHit.h"
#include "PndMdtPoint.h"

#include "FairRootManager.h"
#include "FairDetector.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"

#include "TClonesArray.h"
#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TGeoNode.h"
#include "TGeoMatrix.h"
#include "TVector3.h"
#include "TRandom.h"

#include <iostream>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndMdtHitProducerIdeal::PndMdtHitProducerIdeal() : PndPersistencyTask("Ideal MDT Hit Producer"), fPosResolution(-1.)
{
  SetPersistency(kTRUE);
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndMdtHitProducerIdeal::~PndMdtHitProducerIdeal() {}
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
InitStatus PndMdtHitProducerIdeal::Init()
{

  cout << "-I- PndMdtHitProducerIdeal::Init: "
       << "INITIALIZATION *********************" << endl;

  // FairRun* sim = FairRun::Instance(); //[R.K. 01/2017] unused variable?
  // FairRuntimeDb* rtdb=sim->GetRuntimeDb(); //[R.K. 01/2017] unused variable?

  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    cout << "-E- PndMdtHitProducerIdeal::Init: "
         << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  // Get input array
  fPointArray = (TClonesArray *)ioman->GetObject("MdtPoint");
  if (!fPointArray) {
    cout << "-W- PndMdtHitProducerIdeal::Init: "
         << "No MdtPoint array!" << endl;
    return kERROR;
  }

  // Create and register output array
  fHitArray = new TClonesArray("PndMdtHit");

  ioman->Register("MdtHit", "Mdt", fHitArray, GetPersistency());

  if (fPosResolution > 0.)
    cout << "-I- PndMdtHitProducerIdeal::Init: "
         << "Hit Position smearing: " << fPosResolution << " [cm]" << endl;

  LOG(info) << " PndMdtHitProducerIdeal: Intialization successfull";

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndMdtHitProducerIdeal::Exec(Option_t *)
{

  // Reset output array
  if (!fHitArray)
    Fatal("Exec", "No HitArray");

  fHitArray->Delete();

  // Loop over MdtPoints
  Int_t nPoints = fPointArray->GetEntriesFast();
  PndMdtPoint *point = 0;
  TVector3 pos;
  TVector3 sig(fPosResolution, fPosResolution, fPosResolution);

  for (Int_t iPoint = 0; iPoint < nPoints; iPoint++) {
    point = (PndMdtPoint *)fPointArray->At(iPoint);
    if (point->GetEnergyLoss() == 0)
      continue;
    // if ( (point->GetModule()==1) && (TMath::Odd(point->GetDetectorID())) ) continue;
    point->Position(pos);
    if (fPosResolution > 0.) {
      pos.SetX(gRandom->Gaus(pos.X(), fPosResolution));
      pos.SetY(gRandom->Gaus(pos.Y(), fPosResolution));
      pos.SetZ(gRandom->Gaus(pos.Z(), fPosResolution));
    }
    AddHit(point->GetDetectorID(), pos, sig, iPoint);

  } // Loop over MCPoints
}
// -------------------------------------------------------------------------

// -----   Private method AddHit   --------------------------------------------
PndMdtHit *PndMdtHitProducerIdeal::AddHit(Int_t detID, TVector3 &pos, TVector3 &dpos, Int_t index)
{
  // It fills the PndMdtHit category

  TClonesArray &clref = *fHitArray;
  Int_t size = clref.GetEntriesFast();
  return new (clref[size]) PndMdtHit(detID, pos, dpos, index);
}
// ----

ClassImp(PndMdtHitProducerIdeal)
