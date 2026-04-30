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

#include "PndSttCATask.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Root includes
#include "TROOT.h"
#include "TString.h"
#include "TClonesArray.h"
#include "TParticlePDG.h"

// framework includes
#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairRunAna.h"
#include "FairEventHeader.h"
#include "FairField.h"
#include "FairLogger.h"

// PndMvd includes
#include "PndTrackCand.h"
//#include "PndSttMapCreator.h"
#include "PndStt2GeoHandler.h"
#include "PndSttCA.h"

using std::cout;
using std::endl;

ClassImp(PndSttCATask);

void PndSttCATask::SetParContainers()
{
  FairRuntimeDb *rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fSttParameters = (PndGeoSttPar *)rtdb->getContainer("PndGeoSttPar");
}

InitStatus PndSttCATask::Init()
{
  FairRootManager *ioman = FairRootManager::Instance();

  if (!ioman) {
    std::cout << "-E- PndSttCATask::Init: "
              << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  fEventHeader = (TClonesArray *)ioman->GetObject("EventHeader.");
  if (!fEventHeader) {
    LOG(warn) << "  PndSttCATask::Init: No EventHeader array! Needed for EventNumber";
    return kERROR;
  }

  if (fHitBranch.size() == 0) {
    std::cout << "-W- PndSttCATask::Init: "
              << "No Branch Names given with AddHitBranch(TString branchName)! Standard BranchNames taken!" << std::endl;
    fHitBranch.push_back(fInBranchNamePrefix + "STTHit");
    fHitBranch.push_back("fInBranchNamePrefix+STTCombinedSkewedHits");
  }

  for (int i = 0; i < (int)fHitBranch.size(); i++) {
    InitHitArray(fHitBranch[i]);
  }
  if (fSTTHitArray.size() == 0) {
    std::cout << "No InputBranches containing STTHit data are initialised for the PndSttCATask" << std::endl;
    return kERROR;
  }

  FairField *Field = FairRunAna::Instance()->GetField();
  Double_t po[3], BB[3];
  po[0] = 0.;
  po[1] = 0.;
  po[2] = 0.;
  Field->GetFieldValue(po, BB);
  cout << "Field Strength: " << BB[2] / 10. << endl;

  //PndSttMapCreator *mapper = new PndSttMapCreator(fSttParameters);
  //fTubeArray = mapper->FillTubeArray();
  //PndStt2GeoHandler *geoH = new PndStt2GeoHandler(fSttParameters);
  PndStt2GeoHandler *geoH = PndStt2GeoHandler::Instance(fSttParameters);

  fTrackFinder = new PndSttCA(geoH);
  //fTrackFinder = new PndSttCA(fTubeArray);
  fTrackFinder->SetBz(BB[2] / 10.);
  fTrackFinder->SetUseGPU(fUseGPU);

#ifdef RUNCUDA
  if (fUseGPU) {
    // Copy static data to device

    // Macros were defined in PndSttCellTrackletGenerator.h
    int numElements = NUM_SKEWED_STRAWS * MAX_SKEWED_NEIGHBORS + NUM_UNSKEWED_STRAWS * MAX_UNSKEWED_NEIGHBORS;
    // allocate cpu memory, initialize with 0 (important for gpu algorithm)
    int *tubeNeighborings = (int *)calloc(numElements, sizeof(int));
    // cout << "numElements: " << numElements << endl;
    // cout << "tubeNeighborings: " << *tubeNeighborings << endl;

    PndStt2GeoHandler *tmpGeometryMap = fTrackFinder->GetTrackFinderDataObject()->GetSttGeoH();
    //PndSttGeometryMap *tmpGeometryMap = fTrackFinder->GetTrackFinderDataObject()->GetGeometryMap();

    PndSttTube *tube;
    TVector3 pos;
    TArrayI neighbors;
    int tubeID;
    int skewedOffset = NUM_UNSKEWED_STRAWS * MAX_UNSKEWED_NEIGHBORS;

    // fill array with neighborhood data
    // cout << "fTubeArray: " << fTubeArray->GetEntriesFast() << endl;

    for (int i = 1; i < fTubeArray->GetEntriesFast(); ++i) {
      tube = (PndSttTube *)fTubeArray->At(i);
      tubeID = tube->GetTubeID();
      neighbors = tmpGeometryMap->GetNeighborings(tubeID);
      //neighbors = tmpGeometryMap->GetNeighboringsByMap(tubeID);

      // store at first data of unskewed tubes in array, at the end of the array: skewed tubes neighborings
      if (tubeID >= START_TUBE_ID_SKEWED && tubeID <= END_TUBE_ID_SKEWED) {
        // inner unskewed tube
        for (int j = 0; j < neighbors.GetSize(); ++j) {
          tubeNeighborings[skewedOffset + j * NUM_SKEWED_STRAWS + (tubeID - START_TUBE_ID_SKEWED)] = neighbors[j];
          // cout << "tubeNeighborings index: " << skewedOffset + j * NUM_SKEWED_STRAWS + (tubeID - START_TUBE_ID_SKEWED) << " neighbors[j]: " << neighbors[j] << endl;
        }

      } else if (tubeID < START_TUBE_ID_SKEWED) {
        // middle skewed tube
        for (int j = 0; j < neighbors.GetSize(); ++j) {
          tubeNeighborings[j * NUM_UNSKEWED_STRAWS + (tubeID - 1)] = neighbors[j];
          // cout << "tubeNeighborings index: " <<j * NUM_UNSKEWED_STRAWS + (tubeID - 1) << " neighbors[j]: " << neighbors[j] << endl;
        }
      } else if (tubeID > END_TUBE_ID_SKEWED) {
        // outer unskewed tube
        for (int j = 0; j < neighbors.GetSize(); ++j) {
          tubeNeighborings[j * NUM_UNSKEWED_STRAWS + (tubeID - (END_TUBE_ID_SKEWED - START_TUBE_ID_SKEWED + 1) - 1)] = neighbors[j];
          // cout << "tubeNeighborings index: " << j * NUM_UNSKEWED_STRAWS + (tubeID - (END_TUBE_ID_SKEWED - START_TUBE_ID_SKEWED + 1) - 1) << " neighbors[j]: " << neighbors[j] <<
          // endl;
        }
      }
    }
    // cout << "tubeNeighborings size: " << sizeof(tubeNeighborings)/sizeof(tubeNeighborings[0]) << endl;
    fDev_tubeNeighborings = AllocateStaticData(tubeNeighborings, numElements);
    fTrackFinder->SetDevTubeNeighboringsPointer(fDev_tubeNeighborings);
    free(tubeNeighborings);
  }
#endif

  // store information of tracklets
  fFirstTrackCandArray = ioman->Register(fOutBranchNamePrefix + "SttCATrackCand", "PndTrackCand", "STT", fPersistence);

  LOG(info) << " PndSttCATask: Initialisation successfull";
  return kSUCCESS;
}

// -----   Public method Exec   --------------------------------------------
void PndSttCATask::Exec(Option_t *)
{

  // cout << "====================Begin PndSttCATask::Exec=======================" << endl;

  // Reset output array
  if (!fFirstTrackCandArray)
    Fatal("Exec", "No trackCandArray");

  fTrackFinder->Reset();

  for (int i = 0; i < (int)fSTTHitBranch.size(); i++) {
    fTrackFinder->AddHits(fSTTHitArray[i], fSTTHitBranch[i]);
  }

  fTrackFinder->FindTracks();

  for (int i = 0; i < fTrackFinder->NumFirstTrackCands(); i++) {
    new ((*fFirstTrackCandArray)[i]) PndTrackCand(fTrackFinder->GetFirstTrackCand(i));
  }
}

void PndSttCATask::FinishEvent()
{
  fFirstTrackCandArray->Delete();

#ifdef RUNCUDA
  if (fUseGPU) {
    FreeStaticData(fDev_tubeNeighborings);
  }
#endif

  vector<int> numHits;
  numHits.push_back(fTrackFinder->NumHits());
  numHits.push_back(fTrackFinder->NumHitsWithoutDouble());
  numHits.push_back(fTrackFinder->NumUnambiguousNeighbors());
  fNumHitsPerEvent.push_back(numHits);
}

void PndSttCATask::FinishTask() {}

void PndSttCATask::InitHitArray(TString branchName)
{
  TClonesArray *tempArray = (TClonesArray *)FairRootManager::Instance()->GetObject(branchName);
  if (tempArray == nullptr) {
    std::cout << "-W- PndSttCATask::Init: "
              << "No hitArray for BranchName #############################################" << branchName.Data() << std::endl;
  }
  if (strcmp(tempArray->GetClass()->GetName(), "PndSttHit") == 0) {
    fSTTHitArray.push_back(tempArray);
    fSTTHitBranch.push_back(branchName);
  }
}
