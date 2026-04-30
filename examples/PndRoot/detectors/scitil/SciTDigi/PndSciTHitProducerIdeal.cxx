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
// FairSciTProducerIdeal source file
//
//  created by A. Sanchez
//  modified by D. Steinschaden
//  last update  04.2015
// -------------------------------------------------------------------------

#include "PndSciTHitProducerIdeal.h"
#include "PndSciTHit.h"
#include "PndSciTPoint.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairGeoVector.h"
#include "FairLogger.h"

#include "TVector3.h"
#include "TGeoBBox.h"
#include "TClonesArray.h"
#include "TGeoManager.h"

#include <cmath>

// -----   Default constructor   -------------------------------------------
PndSciTHitProducerIdeal::PndSciTHitProducerIdeal() : PndPersistencyTask("Ideal PndSciT Hit Producer"), fInBranchName(""), fdt(0.1) // auto time resolution 0.1 ns
{
  fInBranchName = "SciTPoint";
  fGeoH = nullptr;
  SetPersistency(kTRUE);
}
// -------------------------------------------------------------------------

// -----   Default constructor   -------------------------------------------
PndSciTHitProducerIdeal::PndSciTHitProducerIdeal(Double_t dt) : PndPersistencyTask("Ideal PndSciT Hit Producer"), fInBranchName(""), fdt(dt)
{
  fInBranchName = "SciTPoint";
  fGeoH = nullptr;
  SetPersistency(kTRUE);
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndSciTHitProducerIdeal::~PndSciTHitProducerIdeal() {}

// -----   Public method Init   --------------------------------------------
InitStatus PndSciTHitProducerIdeal::Init()
{
  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();

  if (!ioman) {
    std::cout << "-E- PndSciTHitProducerIdeal::Init: "
              << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  // Get input array
  fPointArray = (TClonesArray *)ioman->GetObject(fInBranchName);

  if (!fPointArray) {
    std::cout << "-W- PndSciTHitProducerIdeal::Init: "
              << "No SciTPoint array!" << std::endl;
    return kERROR;
  }

  // Create and register output array
  fHitArray = ioman->Register("SciTHit", "PndSciTHit", "SciT", GetPersistency());

  LOG(info) << " PndSciTHitProducerIdeal: Intialisation successfull";
  return kSUCCESS;
}
// -------------------------------------------------------------------------
void PndSciTHitProducerIdeal::SetParContainers()
{
  // Get Base Container
  // FairRun* ana = FairRun::Instance();
  // FairRuntimeDb* rtdb=ana->GetRuntimeDb();

  if (fGeoH == nullptr) {
    std::cout << "ScitTil fGeoH is loading" << std::endl;
    fGeoH = PndGeoHandling::Instance();
  } else
    std::cout << "ScitTil fGeoH is already defind but shouldn't" << std::endl;
  if (fGeoH == nullptr) {
    std::cout << "ScitTil fGeoH was loaded but is still nullptr" << std::endl;
  }
  fGeoH->SetParContainers();

  return;
}

// -----   Public method Exec   --------------------------------------------
void PndSciTHitProducerIdeal::Exec(Option_t *)
{
  // Reset output array
  if (!fHitArray)
    Fatal("Exec", "No HitArray");

  fHitArray->Delete();

  // Declare some variables

  PndSciTPoint *point = nullptr;

  Int_t detectorID; // Detector ID /shortID
  TString detectorName;
  Double_t time;
  TVector3 zeroVector(0, 0, 0);

  TVector3 detectorPosition;
  TVector3 hitPosition;
  TVector3 sensorDim; // Sensor dimension always in half the lenghts in root!
  TVector3 dHitPosition;

  // Loop over SciTPoints
  Int_t nPoints = fPointArray->GetEntriesFast();

  for (Int_t iPoint = 0; iPoint < nPoints; iPoint++) {
    point = (PndSciTPoint *)fPointArray->At(iPoint);
    if (fVerbose > 0)
      std::cout << " Ideal Hit Producer -Point-: " << point << std::endl;
    if (!point)
      continue;

    // Detector ID
    detectorID = point->GetDetectorID();
    detectorName = point->GetDetName();

    // HitPosition in the middle of the sensor = Detector Position

    detectorPosition = fGeoH->LocalToMasterShortId(zeroVector, detectorID);
    hitPosition = detectorPosition;

    // Get the range for the Hit position

    // sensor Dimensions equivalent to the potential error of the hitPosition in the center of the Tile. Attention,in real its no Gaussian shaped distribution but an rectangual!!

    sensorDim = fGeoH->GetSensorDimensionsShortId(detectorID);
    dHitPosition = sensorDim * 2 * (1 / sqrt(12));

    // produce realistic timestamp

    time = point->GetTime(); // Get MCTime
    smear(time, fdt);        // smear with fdt to  creat realistic Time

    // Create new hit
    new ((*fHitArray)[iPoint])
      PndSciTHit(detectorID, detectorName, time + FairRootManager::Instance()->GetEventTime(), fdt, 0, 0, 0, 0, hitPosition, dHitPosition, iPoint, point->GetEnergyLoss());
  } // Loop over MCPoints

  fHitArray->Sort();
  // Event summary
  if (fVerbose > 1)
    LOG(info) << " PndSciTHitProducerIdeal: " << nPoints << " SciTPoints, " << nPoints << " Hits created.";
}
// -------------------------------------------------------------------------

void PndSciTHitProducerIdeal::smear(Double_t &time, Double_t &dt)
{
  /// smear a 3d vector

  Double_t t = time;
  // std::cout<<" time "<<time<<std::endl;
  Double_t sigt;

  sigt = gRandom->Gaus(0, dt);
  t += sigt;
  time = t;
  return;
}

ClassImp(PndSciTHitProducerIdeal)
