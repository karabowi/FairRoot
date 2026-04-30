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
// PndSciTDigiTask source file
//
//  created by D. Steinschaden
//  last update  06.2015
// -------------------------------------------------------------------------

#include "PndSciTDigiTask.h"
#include "PndSciTHit.h"
#include "PndSciTPoint.h"
#include "PndSciTHitSorterTask.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairGeoVector.h"
#include "FairEventHeader.h"
#include "FairLogger.h"

#include "TVector3.h"
#include "TGeoBBox.h"
#include "TClonesArray.h"
#include "TGeoManager.h"

#include <cmath>

// -----   Default constructor   -------------------------------------------
PndSciTDigiTask::PndSciTDigiTask() : PndPersistencyTask("PndSciT Hit Producer")
{
  fInBranchName = "SciTPoint";
  fOutBranchName = "SciTHit";
  fSortedOutBranchName = "SciTSortedHit";

  fTimeOrderedDigi = kFALSE;
  fActivateBuffering = kTRUE;
  SetPersistency(kTRUE);

  fGeoH = nullptr;

  fdt = 0.075;        // auto time resolution 0.075 ns
  fDeadtime = 1000.0; // Tile dead time after a hit in ns
  fPileupTime = 10.0; // Time of possible Pile up after a Hit in ns
}
// -------------------------------------------------------------------------

// -----   Default constructor   -------------------------------------------
PndSciTDigiTask::PndSciTDigiTask(Double_t dt, Double_t deadtime) : PndPersistencyTask("Ideal PndSciT Hit Producer")
{
  fInBranchName = "SciTPoint";
  fOutBranchName = "SciTHit";
  fSortedOutBranchName = "SciTSortedHit";

  fTimeOrderedDigi = kFALSE;
  fActivateBuffering = kTRUE;
  SetPersistency(kTRUE);

  fGeoH = nullptr;

  fdt = dt;
  fDeadtime = deadtime;
  fPileupTime = 10.0;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndSciTDigiTask::~PndSciTDigiTask() {}

// -----   Public method Init   --------------------------------------------
InitStatus PndSciTDigiTask::Init()
{
  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();

  if (!ioman) {
    std::cout << "-E- PndSciTDigiTask::Init: "
              << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  // Get input array
  fPointArray = (TClonesArray *)ioman->GetObject(fInBranchName);

  if (!fPointArray) {
    std::cout << "-W- PndSciTDigiTask::Init: "
              << "No SciTPoint array!" << std::endl;
    return kERROR;
  }
  // eventbased
  // Create and register output array
  // fHitArray = ioman->Register(OutBranchName, ClassName, FolderName, kTRUE);
  // fHitArray = ioman->Register("SciTHit", "PndSciTHit", "SciT", kTRUE);

  // time and event based simulation:
  // fDataBuffer = new PndSciTHitWriteoutBuffer(fOutBranchName, fFolderName", fPersistance);
  fDataBuffer = new PndSciTHitWriteoutBuffer(fOutBranchName, "SciT", GetPersistency());
  fDataBuffer = (PndSciTHitWriteoutBuffer *)ioman->RegisterWriteoutBuffer(fOutBranchName, fDataBuffer);
  fDataBuffer->ActivateBuffering(fActivateBuffering); // Buffering always activated to handle the Pile up in Event and Time based Version

  LOG(info) << " PndSciTDigiTask: Intialisation successfull";
  return kSUCCESS;
}
// -------------------------------------------------------------------------

void PndSciTDigiTask::RunTimeBased()
{

  fTimeOrderedDigi = kTRUE;
  this->Add(new PndSciTHitSorterTask(2000, 1, fOutBranchName, fSortedOutBranchName, "SciT"));
}

// ----------------------------------------------------------------------------
void PndSciTDigiTask::SetParContainers()
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
void PndSciTDigiTask::Exec(Option_t *)
{
  // Reset output array
  if (!fHitArray)
    Fatal("Exec", "No HitArray");

  // fHitArray->Clear();

  // Declare some variables

  PndSciTPoint *point = nullptr;

  Double_t nBC408 = 1.58;

  Int_t detectorID; // Detector ID /shortID
  TString detectorName;
  Double_t mcTime, eventTime, hitTime, detectingTime;
  Double_t sipm1, sipm2, dSiPm;

  TVector3 zeroVector(0, 0, 0);

  TVector3 detectorPosition;
  TVector3 mcPosition;
  Double_t xSiPm1, xSiPm2;
  TVector3 hitPosition;
  Double_t hitXPosition;
  TVector3 sensorDim; // Sensor dimension always in half the lenghts in root!
  TVector3 dHitPosition;

  // calculate invariant values outside of a loop:

  dSiPm = sqrt(2) * fdt;                                    // time resolution of a single (row of) SiPm
  Double_t cBC408 = 299792458.0 / nBC408 * (100 / (1.0e9)); // Light in BC408 Scintillator in [cm/ns]

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

    mcPosition = point->GetMeanPosition();
    mcPosition = fGeoH->MasterToLocalShortId(mcPosition, detectorID);

    sensorDim = fGeoH->GetSensorDimensionsShortId(detectorID);

    xSiPm1 = sensorDim(0) + mcPosition(0);
    xSiPm2 = sensorDim(0) - mcPosition(0);

    // produce realistic timestamp

    mcTime = point->GetTime(); // Get MCTime
    eventTime = FairRootManager::Instance()->GetEventTime();
    hitTime = mcTime + eventTime;

    sipm1 = hitTime + xSiPm1 / cBC408;
    sipm2 = hitTime + xSiPm2 / cBC408;

    smear(sipm1, dSiPm); // smear with fdt to  creat realistic Time
    smear(sipm2, dSiPm); // smear with fdt to  creat realistic Time

    detectingTime = (sipm1 + sipm2) / 2.0 - sensorDim(0) / cBC408;

    // HitPosition in the middle of the sensor = Detector Position

    detectorPosition = fGeoH->LocalToMasterShortId(zeroVector, detectorID);

    hitXPosition = (sipm1 - sipm2) / 2 * cBC408;
    if (hitXPosition > sensorDim(0)) {
      hitXPosition = sensorDim(0);
    } else if (hitXPosition < -sensorDim(0)) {
      hitXPosition = -sensorDim(0);
    }
    hitPosition.SetXYZ(hitXPosition, 0., 0.);

    hitPosition = fGeoH->LocalToMasterShortId(hitPosition, detectorID);

    // sensor Dimensions equivalent to the potential error of the hitPosition in the center of the Tile. Attention,in real its no Gaussian shaped distribution but an rectangual!!

    dHitPosition = (1 / sqrt(12)) * 2 * sensorDim; // without x position by time difference

    dHitPosition.SetX(fdt * cBC408);

    // Create new hit
    // new ((*fHitArray)[iPoint]) PndSciTHit(detectorID, detectorName,
    // 			            time+FairRootManager::Instance()->GetEventTime(), fdt,
    //				    hitPosition,dHitPosition,
    //				    iPoint,
    //				    point->GetEnergyLoss());

    PndSciTHit *tempHit = new PndSciTHit(detectorID, detectorName, detectingTime, fdt, sipm1, dSiPm, sipm2, dSiPm, hitPosition, dHitPosition, iPoint, point->GetEnergyLoss());
    if (fTimeOrderedDigi) {
      tempHit->ResetLinks();
      FairEventHeader *evtHeader = (FairEventHeader *)FairRootManager::Instance()->GetObject("EventHeader.");
      tempHit->AddLink(FairLink(evtHeader->GetInputFileId(), FairRootManager::Instance()->GetEntryNr(), fInBranchName, iPoint));

      tempHit->AddLink(FairLink(-1, FairRootManager::Instance()->GetEntryNr(), "EventHeader.", -1));
    }

    fDataBuffer->FillNewData(tempHit, hitTime, hitTime + fDeadtime);

    delete tempHit;

  } // Loop over MCPoints

  // fHitArray->Sort();
  // Event summary
  if (fVerbose > 1)
    LOG(info) << " PndSciTDigiTask: " << nPoints << " SciTPoints, " << nPoints << " Hits created.";

  //---------- Write Out ALL Data after every Event in EventBased Version only -------

  if (fTimeOrderedDigi == kFALSE && fActivateBuffering == kTRUE)
    fDataBuffer->WriteOutAllData();
}
// -------------------------------------------------------------------------

void PndSciTDigiTask::smear(Double_t &time, Double_t &dt)
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

ClassImp(PndSciTDigiTask)
