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
// -----               PndSdsHitProducerIdeal source file             -----
// -------------------------------------------------------------------------

#include "TClonesArray.h"

#include "FairRootManager.h"
#include "PndSdsHitProducerIdeal.h"
#include "PndSdsHit.h"
#include "PndSdsMCPoint.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"

// -----   Default constructor   -------------------------------------------
PndSdsHitProducerIdeal::PndSdsHitProducerIdeal() : PndSdsTask("Ideal SDS Hit Producer"), fPointArray(nullptr), fHitArray(nullptr)
{
  SetPersistency(kTRUE);
}
// -------------------------------------------------------------------------

// -----   Named constructor   -------------------------------------------
PndSdsHitProducerIdeal::PndSdsHitProducerIdeal(const char *name) : PndSdsTask(name), fPointArray(), fHitArray()
{
  SetPersistency(kTRUE);
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndSdsHitProducerIdeal::~PndSdsHitProducerIdeal() {}

// -----   Public method Init   --------------------------------------------
InitStatus PndSdsHitProducerIdeal::Init()
{
  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();

  SetBranchNames();

  if (!ioman) {
    std::cout << "-E- PndSdsHitProducerIdeal::Init: "
              << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  // Get input array
  fPointArray = (TClonesArray *)ioman->GetObject(fInBranchName);

  if (!fPointArray) {
    std::cout << "-W- PndSdsHitProducerIdeal::Init: "
              << "No SDSPoint array!" << std::endl;
    return kERROR;
  }

  // Create and register output array
  fHitArray = new TClonesArray("PndSdsHit");
  ioman->Register(fOutBranchName, fFolderName, fHitArray, GetPersistency());

  LOG(info) << " PndSdsHitProducerIdeal: Intialisation successfull";
  return kSUCCESS;
}
// -------------------------------------------------------------------------
void PndSdsHitProducerIdeal::SetParContainers()
{
  // Get Base Container
  // FairRun* ana = FairRun::Instance();
  // FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  // fGeoPar = (PndSdsGeoPar*)(rtdb->getContainer("PndSdsGeoPar"));
}

// -----   Public method Exec   --------------------------------------------
void PndSdsHitProducerIdeal::Exec(Option_t *)
{
  // Reset output array
  if (!fHitArray)
    Fatal("Exec", "No HitArray");

  fHitArray->Delete();

  // Declare some variables
  PndSdsMCPoint *point = 0;

  Int_t detID = 0; // Detector ID
  // trackID = 0;     // Track index //[R.K. 01/2017] unused variable

  TVector3 pos, dpos; // Position and error vectors

  // Loop over PndSdsMCPoints
  Int_t nPoints = fPointArray->GetEntriesFast();

  for (Int_t iPoint = 0; iPoint < nPoints; iPoint++) {
    point = (PndSdsMCPoint *)fPointArray->At(iPoint);
    //	std::cout << " Ideal Hit Producer -Point-: " << point << std::endl;
    if (!point)
      continue;

    // Detector ID
    detID = point->GetDetectorID();

    // MCTrack ID
    // trackID = point->GetTrackID(); //[R.K. 01/2017] unused variable

    TVector3 position(point->GetX() + gRandom->Gaus(0, fDx), point->GetY() + gRandom->Gaus(0, fDy), point->GetZ()+ + gRandom->Gaus(0, fDz));

    dpos.SetXYZ(fDx, fDy, fDz);
    TMatrixD cov(3, 3);
    cov[0][0] = fDx * fDx;
    cov[1][1] = fDy * fDy;
    cov[2][2] = fDz * fDz;

    // Create new hit
    PndSdsHit *myHit = new ((*fHitArray)[iPoint]) PndSdsHit(detID, point->GetSensorID(), position, dpos, -1, point->GetEnergyLoss(), 1, iPoint);
    myHit->SetClusterIndex(fInBranchId, iPoint);
    myHit->SetCov(cov);
    //	std::cout << "Hit created for module: " << point->GetDetName() << std::endl;

  } // Loop over MCPoints

  // Event summary
  //  std::cout << "-I- PndSdsHitProducerIdeal: " << nPoints << " PndSdsMCPoints, "
  //      << nPoints << " Hits created." << std::endl;
}
// -------------------------------------------------------------------------

ClassImp(PndSdsHitProducerIdeal)
