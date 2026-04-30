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

#include "PndSttCellTrackFinderTask.h"

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
#include "PndRiemannTrack.h"
//#include "PndSttMapCreator.h"
#include "PndStt2GeoHandler.h"
#include "PndSttCellTrackFinder.h"

// J.R for testing, remove later!
#include "PndSttHit.h"

using std::cout;
using std::endl;

// Macro for printing the calculation times into files called calcTimesCPU*.txt
//#define PRINT_CALC_TIMES

ClassImp(PndSttCellTrackFinderTask);

void PndSttCellTrackFinderTask::SetParContainers()
{
  FairRuntimeDb *rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fSttParameters = (PndGeoSttPar *)rtdb->getContainer("PndGeoSttPar");
}

InitStatus PndSttCellTrackFinderTask::Init()
{
  FairRootManager *ioman = FairRootManager::Instance();

  fStopTimeValueCellTrack = 0.0;
  eventNumber = 0;
  fFunctorCellTrack = new StopTime();

  if (!ioman) {
    std::cout << "-E- PndSttCellTrackFinderTask::Init: "
              << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  fEventHeader = (TClonesArray *)ioman->GetObject("EventHeader.");
  if (!fEventHeader) {
    LOG(warn) << "  PndSttCellTrackFinderTask::Init: No EventHeader array! Needed for EventNumber";
    return kERROR;
  }

  if (fHitBranch.size() == 0) {
    std::cout << "-W- PndSttCellTrackFinderTask::Init: "
              << "No Branch Names given with AddHitBranch(TString branchName)! Standard BranchNames taken!" << std::endl;
    fHitBranch.push_back(fInBranchNamePrefix + "STTHit");
    // fHitBranch.push_back("STTParalHit");
    // fHitBranch.push_back("STTSkewHit");
    fHitBranch.push_back("fInBranchNamePrefix+STTCombinedSkewedHits");
  }

  for (int i = 0; i < (int)fHitBranch.size(); i++) {
    InitHitArray(fHitBranch[i]);
  }

  if (fSTTHitArray.size() == 0) {
    std::cout << "No InputBranches containing STTHit data are initialised for the PndSttCellTrackFinderTask" << std::endl;
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
  PndStt2GeoHandler *geoH = PndStt2GeoHandler::Instance(fSttParameters);
  fTubeArray = geoH->GetTubeArray();
  fTrackFinder = new PndSttCellTrackFinder(geoH);

  fTrackFinder->SetRunTimeBased(fRunTimeBased);
  fTrackFinder->SetClusterTime(fClusterTime);
  fTrackFinder->SetCalcFirstTrackletInf(fAnalyseSteps);
  fTrackFinder->SetUseGPU(fUseGPU);
  fTrackFinder->SetVerbose(fVerbose);
  fTrackFinder->SetBz(BB[2] / 10.);
  fTrackFinder->SetCalcWithCorrectedIsochrones(fCalcWithCorrectedIsochrones);

#ifdef RUNCUDA
  if (fUseGPU) {
    // Copy static data to device

    // Macros were defined in PndSttCellTrackletGenerator.h
    int numElements = NUM_SKEWED_STRAWS * MAX_SKEWED_NEIGHBORS + NUM_UNSKEWED_STRAWS * MAX_UNSKEWED_NEIGHBORS;
    // allocate cpu memory, initialize with 0 (important for gpu algorithm)
    int *tubeNeighborings = (int *)calloc(numElements, sizeof(int));

    PndSttGeometryMap *tmpGeometryMap = fTrackFinder->GetTrackFinderDataObject()->GetGeometryMap();

    PndSttTube *tube;
    TVector3 pos;
    TArrayI neighbors;
    int tubeID;
    int skewedOffset = NUM_UNSKEWED_STRAWS * MAX_UNSKEWED_NEIGHBORS;

    // fill array with neighborhood data
    for (int i = 1; i < fTubeArray->GetEntriesFast(); ++i) {
      tube = (PndSttTube *)fTubeArray->At(i);
      tubeID = tube->GetTubeID();
      neighbors = tmpGeometryMap->GetNeighboringsByMap(tubeID);

      // store at first data of unskewed tubes in array, at the end of the array: skewed tubes neighborings
      if (tubeID >= START_TUBE_ID_SKEWED && tubeID <= END_TUBE_ID_SKEWED) {
        // inner unskewed tube
        for (int j = 0; j < neighbors.GetSize(); ++j) {
          tubeNeighborings[skewedOffset + j * NUM_SKEWED_STRAWS + (tubeID - START_TUBE_ID_SKEWED)] = neighbors[j];
        }

      } else if (tubeID < START_TUBE_ID_SKEWED) {
        // middle skewed tube
        for (int j = 0; j < neighbors.GetSize(); ++j) {
          tubeNeighborings[j * NUM_UNSKEWED_STRAWS + (tubeID - 1)] = neighbors[j];
        }
      } else if (tubeID > END_TUBE_ID_SKEWED) {
        // outer unskewed tube
        for (int j = 0; j < neighbors.GetSize(); ++j) {
          tubeNeighborings[j * NUM_UNSKEWED_STRAWS + (tubeID - (END_TUBE_ID_SKEWED - START_TUBE_ID_SKEWED + 1) - 1)] = neighbors[j];
        }
      }
    }

    fDev_tubeNeighborings = AllocateStaticData(tubeNeighborings, numElements);

    fTrackFinder->SetDevTubeNeighboringsPointer(fDev_tubeNeighborings);
    free(tubeNeighborings);
  }
#endif

  if (fAnalyseSteps) {
    // store information of primary tracklets
    fFirstTrackCandArray = ioman->Register(fOutBranchNamePrefix + "FirstTrackCand", "PndTrackCand", "STT", fPersistence);
    fFirstTrackArray = ioman->Register(fOutBranchNamePrefix + "FirstTrack", "PndTrack", "STT", fPersistence);
    fFirstRiemannTrackArray = ioman->Register(fOutBranchNamePrefix + "FirstRiemannTrack", "PndRiemannTrack", "STT", fPersistence);
  }

  fCombiTrackCandArray = ioman->Register(fOutBranchNamePrefix + "CombiTrackCand", "PndTrackCand", "STT", fPersistence);
  fCombiTrackArray = ioman->Register(fOutBranchNamePrefix + "CombiTrack", "PndTrack", "STT", fPersistence);
  fCombiRiemannTrackArray = ioman->Register(fOutBranchNamePrefix + "CombiRiemannTrack", "PndRiemannTrack", "STT", fPersistence);

  if (fCalcWithCorrectedIsochrones) {

    fCorrectedIsochronesArray = ioman->Register(fOutBranchNamePrefix + "CorrectedIsochrones", "FairHit", "STT", fPersistence);
  }

  LOG(info) << " PndSttCellTrackFinderTask: Initialisation successfull";
  if (fRunWithSortedHits) {
    for (int i = 0; i < fHitBranch.size(); i++) {

      fSTTHitBranch[i] = fHitBranch[i];
    }
  }
  // fInitDone = kTRUE;
  return kSUCCESS;
}

// -----   Public method Exec   --------------------------------------------
void PndSttCellTrackFinderTask::Exec(Option_t *)
{

  // FairEventHeader* myEventHeader = (FairEventHeader*) fEventHeader; //[R.K. 01/2017] unused variable?
  // int eventNumber = myEventHeader->GetMCEntryNumber() - 1; //[R.K. 01/2017] unused variable?

  if (fVerbose > 0) {
    cout << "====================Begin PndSttCellTrackFinderTask::Exec=======================" << endl;
    cout << "Event #" << eventNumber << endl;
    eventNumber++;
  }

  // Reset output array
  if (!fFirstTrackCandArray)

    Fatal("Exec", "No trackCandArray");

  fTrackFinder->Reset();

  if (fRunWithSortedHits) {

    // std::cout << "STT Hit branch size: " << fSTTHitBranch.size() << std::endl;
    // std::cout << "Hit branch size: " << fHitBranch.size() << std::endl;

    if (fRunWithSortedHits) {
      // std::cout << "Number of STTHitArrays: " << fSTTHitArray.size() << std::endl;
      for (int i = 0; i < (int)fSTTHitBranch.size(); i++) {
        fSTTHitArray[i]->Delete(); // Clear the TClones array for the next time burst
      }
    }

    for (int i = 0; i < (int)fHitBranch.size(); i++) {
      fSTTHitBranch[i] = fHitBranch[i];
    }

    // fStopTimeValueCellTrack+=2000.0; // In [ns]
    // std::cout << "fStopTimeCellTrack= " << fStopTimeValueCellTrack << std::endl;

    for (int i = 0; i < (int)fSTTHitBranch.size(); i++) {

      fSTTHitArray[i] =
        (TClonesArray *)FairRootManager::Instance()->GetData(fSTTHitBranch[i], fFunctorCellTrack, fStopTimeValueCellTrack - 2000.0, fFunctorCellTrack, fStopTimeValueCellTrack);
    }
  }

  for (int i = 0; i < (int)fSTTHitArray.size(); i++) {

    fTrackFinder->AddHits((TClonesArray *)fSTTHitArray[i], fSTTHitBranch[i]);
  }

  fTrackFinder->FindTracks();

  if (fAnalyseSteps) {
    for (int i = 0; i < fTrackFinder->NumFirstTrackCands(); i++) {
      // PndTrackCand* myCand = //[R.K.03/2017] unused variable
      new ((*fFirstTrackCandArray)[i]) PndTrackCand(fTrackFinder->GetFirstTrackCand(i));
    }

    for (int i = 0; i < fTrackFinder->NumFirstRiemannTracks(); ++i) {
      // PndRiemannTrack* myTrack = //[R.K.03/2017] unused variable
      new ((*fFirstRiemannTrackArray)[i]) PndRiemannTrack(fTrackFinder->GetFirstRiemannTrack(i));
    }
  }

  for (int i = 0; i < fTrackFinder->NumCombinedTracks(); ++i) {
    PndTrackCand *myCand = new ((*fCombiTrackCandArray)[i]) PndTrackCand(fTrackFinder->GetCombiTrackCand(i));
    PndTrack *myTrack = new ((*fCombiTrackArray)[i]) PndTrack(fTrackFinder->GetCombiTrack(i));
    myTrack->SetTrackCandRef(myCand);
    myTrack->SetTrackCand(*myCand);

    myCand->SetEntryNr(FairLink(-1, FairRootManager::Instance()->GetEntryNr(), "CombiTrackCand", fCombiTrackCandArray->GetEntriesFast() - 1));
    myTrack->SetEntryNr(FairLink(-1, FairRootManager::Instance()->GetEntryNr(), "CombiTrack", fCombiTrackArray->GetEntriesFast() - 1));
  }

  for (int i = 0; i < fTrackFinder->NumCombinedRiemannTracks(); ++i) {
    // PndRiemannTrack* myRiemannTrack = //[R.K.03/2017] unused variable
    new ((*fCombiRiemannTrackArray)[i]) PndRiemannTrack(fTrackFinder->GetCombiRiemannTrack(i));
  }

  if (fCalcWithCorrectedIsochrones) {
    std::map<int, FairHit *> correctedIsochrones = fTrackFinder->GetCorrectedIsochrones();

    for (std::map<int, FairHit *>::iterator iter = correctedIsochrones.begin(); iter != correctedIsochrones.end(); iter++) {
      // FairHit* myCorrectedHit = //[R.K.03/2017] unused variable
      new ((*fCorrectedIsochronesArray)[fCorrectedIsochronesArray->GetEntries()]) FairHit(*iter->second);
    }
  }

  if (fVerbose > 0)
    cout << "#FirstTracklets: " << fTrackFinder->GetNumPrimaryTracklets() << ", #CombinedTracklets: " << fTrackFinder->NumCombinedTracks() << endl;

  // fCombiTrackCandArray->Sort();	//WALTER CHANGED HERE!!!!!!!!!!!!!!!!
  // fCombiTrackArray->Sort();		//WALTER CHANGED HERE!!!!!!!!!!!!!!!!
}

void PndSttCellTrackFinderTask::FinishEvent()
{

  if (fAnalyseSteps) {
    fFirstTrackCandArray->Delete();
    fFirstRiemannTrackArray->Delete();
  }

  fCombiTrackCandArray->Delete();
  fCombiTrackArray->Delete();
  fCombiRiemannTrackArray->Delete();

  if (fCalcWithCorrectedIsochrones)
    fCorrectedIsochronesArray->Delete();

  vector<int> numHits;
  numHits.push_back(fTrackFinder->NumHits());
  numHits.push_back(fTrackFinder->NumHitsWithoutDouble());
  numHits.push_back(fTrackFinder->NumUnambiguousNeighbors());
  fNumHitsPerEvent.push_back(numHits);
}

void PndSttCellTrackFinderTask::FinishTask()
{
#ifdef RUNCUDA
  if (fUseGPU) {
    FreeStaticData(fDev_tubeNeighborings);
  }
#endif

#ifdef PRINT_CALC_TIMES
  // write calculation times, calcTimesTrackletGen.size()=#Events, calcTimesTrackletGen[i].size()=#measured times for Event #i
  vector<vector<Double_t>> calcTimesTrackletGen = fTrackFinder->GetTimeStampsTrackletGen();
  vector<vector<Double_t>> calcTimesNeighborhood = fTrackFinder->GetTimeStampsGenerateNeighborhoodData();

  FILE *fp = fopen("calcTimesCPU.txt", "w");
  fprintf(fp, "%3s \t %3s \t %3s \t %3s \t %10s \t %10s \t %10s \t %10s \t %10s \t %10s \t %10s \t %10s \t %10s \t %10s \t %10s \n", "#E", "#H", "#HWD", "#UHWD", "EvaluateState()",
          "InitStartTracklets()", "EvaluateMultiState()", "GenerateTracklets()", "CombineTrackletsMultiStages()", "AssignAmbiguousHits()", "SplitData()", "AddMissingHits()",
          "CreatePndTrackCands()", "FindTracks()", "CalcNeighborData");
  for (int i = 0; i < calcTimesTrackletGen.size(); ++i) {
    fprintf(fp, "%3i \t %3i \t %3i \t %3i", i, fNumHitsPerEvent.at(i).at(0), fNumHitsPerEvent.at(i).at(1), fNumHitsPerEvent.at(i).at(2));
    // trackletGen data
    for (int j = 0; j < calcTimesTrackletGen[i].size(); j += 2) {
      fprintf(fp, "\t %.15f", (calcTimesTrackletGen[i].at(j + 1) - calcTimesTrackletGen[i].at(j)) * 1000);
    }
    // neighborhood data
    fprintf(fp, "\t %.15f", (calcTimesNeighborhood[i].at(1) - calcTimesNeighborhood[i].at(0)) * 1000);

    fprintf(fp, "\n");
  }

  fclose(fp);
#endif
}

void PndSttCellTrackFinderTask::InitHitArray(TString branchName)
{
  TClonesArray *tempArray = (TClonesArray *)FairRootManager::Instance()->GetObject(branchName);

  if (tempArray == nullptr) {
    std::cout << "-W- PndSttCellTrackFinderTask::Init: "
              << "No hitArray for BranchName #############################################" << branchName.Data() << std::endl;
  }
  if (strcmp(tempArray->GetClass()->GetName(), "PndSttHit") == 0) {
    // Checks so that the data is of type SttHit, this is the case for time sorted Stt hits as well
    fSTTHitArray.push_back(tempArray);
    fSTTHitBranch.push_back(branchName);
  }
}
