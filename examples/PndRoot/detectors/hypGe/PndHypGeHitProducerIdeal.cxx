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
// -----                CbmStsHitProducerIdeal source file             -----
// -----                  Created 10/01/06  by V. Friese               -----
// -------------------------------------------------------------------------

#include "TClonesArray.h"

#include "FairRootManager.h"
#include "PndHypGeHitProducerIdeal.h"
#include "PndHypGeHit.h"
//#include "PndHypGeHitInfo.h"
#include "PndHypGePoint.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"

// -----   Default constructor   -------------------------------------------
PndHypGeHitProducerIdeal::PndHypGeHitProducerIdeal() : FairTask("Ideal HYPGE Hit Producer")
{ //
  fBranchName = "HypGePoint";
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndHypGeHitProducerIdeal::~PndHypGeHitProducerIdeal() {}

// -----   Public method Init   --------------------------------------------
InitStatus PndHypGeHitProducerIdeal::Init()
{
  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();

  if (!ioman) {
    cout << "-E- PndHypGeHitProducerIdeal::Init: "
         << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  // Get input array
  fPointArray = (TClonesArray *)ioman->GetObject(fBranchName);

  if (!fPointArray) {
    cout << "-W- PndHypGeHitProducerIdeal::Init: "
         << "No HYPGEPoint array!" << endl;
    return kERROR;
  }

  // Create and register output array
  fHitArray = new TClonesArray("PndHypGeHit");
  ioman->Register("HypGeHit", "HypGe", fHitArray, kTRUE);

  LOG(info) << " PndHypGeHitProducerIdeal: Intialisation successfull";
  return kSUCCESS;
}
// -------------------------------------------------------------------------
void PndHypGeHitProducerIdeal::SetParContainers()
{
  // Get Base Container
  FairRun *ana = FairRun::Instance();
  FairRuntimeDb *rtdb = ana->GetRuntimeDb();
  fGeoPar = (PndGeoHypGePar *)(rtdb->getContainer("PndGeoHypGePar"));
}

// -----   Public method Exec   --------------------------------------------
void PndHypGeHitProducerIdeal::Exec(Option_t *)
{
  // Reset output array
  if (!fHitArray)
    Fatal("Exec", "No HitArray");

  fHitArray->Delete();

  // Declare some variables
  PndHypGePoint *point = nullptr;

  Int_t detID = 0, // Detector ID
    trackID = 0;   // Track index

  Double_t ener, den; // Position and error vectors

  // Loop over HypGePoints
  Int_t nPoints = fPointArray->GetEntriesFast();

  for (Int_t iPoint = 0; iPoint < nPoints; iPoint++) {
    point = (PndHypGePoint *)fPointArray->At(iPoint);
    cout << " Ideal Hit Producer -Point-: " << point << endl;
    if (!point)
      continue;

    // Detector ID
    detID = point->GetDetectorID();

    // MCTrack ID
    trackID = point->GetTrackID();
    ener = point->GetEnergyLoss();
    den = 0.000003; /// energy resolution of 3 keV
    smear(ener, den);

    // Create new hit
    new ((*fHitArray)[iPoint]) PndHypGeHit(trackID, detID, ener, den);
    cout << "Hit created for module: " << std::endl;

  } // Loop over MCPoints

  // Event summary
  LOG(info) << " PndHypGeHitProducerIdeal: " << nPoints << " HypGePoints, " << nPoints << " Hits created.";
}
// -------------------------------------------------------------------------
void PndHypGeHitProducerIdeal::smear(Double_t &ener, Double_t &den)
{
  /// smear a 3d vector

  Double_t e = ener;

  // y=pos.y(),
  // z=pos.z(),
  Double_t sige = den;

  // sigy=dpos.y();
  // sigz=dpos.z();

  sige = gRandom->Gaus(0, sige);
  //	sigy=gRandom->Gaus(0,sigy);
  // sigz=gRandom->Gaus(0,sigz);

  e += sige;
  // y += sigy;
  // z += sigz;

  ener = e;
  return;
}

ClassImp(PndHypGeHitProducerIdeal)
