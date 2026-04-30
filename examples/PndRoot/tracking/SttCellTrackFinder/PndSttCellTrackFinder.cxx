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

/*
 * PndSttCellTrackFinder.cxx
 *
 *  Created on: May 20, 2014
 *      Author: schumann
 */

#include "PndSttCellTrackFinder.h"
#include "TString.h"
#include "PndSttHit.h"
#include "FairRootManager.h"
#include "PndSttSkewedHit.h"
#include "PndSttCellTrackletGenerator.h"
#include "TTimeStamp.h"

using namespace std;

ClassImp(PndSttCellTrackFinder);

void PndSttCellTrackFinder::AddHits(TClonesArray *hits, TString branchName)
{

  // J.R To Trace How The Hits Are Added
  // std::cout << "PndSttCellTrackFinder::AddHits " << std::endl;
  // for (int i = 0; i< hits->GetEntries(); i++){
  //	PndSttHit* currentHit = (PndSttHit*) hits -> At(i);
  //	std::cout << "TubeId: " << currentHit->GetTubeID() << ", Time: " << currentHit->GetTimeStamp() << std::endl;
  //}

  fTrackFinderData->AddHits(hits, branchName);
}

void PndSttCellTrackFinder::FindTracks()
{

  // init TrackFinderData
  vector<Double_t> timesGenerateNeighorhoodData;

  fTrackFinderData->SetRunTimeBased(fRunTimeBased);

  fTrackFinderData->SetClusterTime(fClusterTime);
  // store timestamps for runtime analysis

  timesGenerateNeighorhoodData.push_back(TTimeStamp());
  fTrackFinderData->GenerateNeighborhoodData();
  timesGenerateNeighorhoodData.push_back(TTimeStamp());

  // fTrackFinderData->PrintInfo();

  fTrackletGenerator = new PndSttCellTrackletGenerator(fTrackFinderData);

  fTrackletGenerator->SetCalcFirstTrackletInf(fCalcFirstTrackletInf);
  fTrackletGenerator->SetVerbose(fVerbose);
  fTrackletGenerator->SetBz(fBz);

  // calculation on GPU is only possible without multiple STTHits and a maximum number of MAX_THREADS_PER_BLOCK
  if (fUseGPU && (fTrackFinderData->GetAllowDoubleHits() == kFALSE) && (fTrackFinderData->GetNumHitsWithoutDouble() < MAX_THREADS_PER_BLOCK)) {
    fTrackletGenerator->SetUseGPU(fUseGPU);
    fTrackletGenerator->SetDevTubeNeighboringsPointer(fDev_tubeNeighborings);
  }

  fTrackFinderData->SetClusterTime(fClusterTime);
  fTrackFinderData->SetRunTimeBased(fRunTimeBased);

  if (fCalcWithCorrectedIsochrones) {
    fHitCorrector = new PndSttHitCorrector(fTrackFinderData);
    fHitCorrector->CorrectHits();
    map<int, FairHit *> correctedIsochrones = fHitCorrector->GetCorrectedHits();

    // Get tracks with corrected hits
    fTrackletGenerator->SetCorrectedHits(correctedIsochrones);
  }

  fTrackletGenerator->FindTracks();
  // fTrackletGenerator->PrintInfo();

  StoreTrackData();

  // cpu time measurement
  fTimeStampsTrackletGen.push_back(fTrackletGenerator->GetTimeStamps());
  fTimeStampsGenerateNeighborhoodData.push_back(timesGenerateNeighorhoodData);
}

;
