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
 * PndSttCA.cxx
 *
 *  Created on: May 20, 2014
 *      Author: schumann
 */

#include "PndSttCA.h"
#include "TString.h"
#include "PndSttHit.h"
#include "FairRootManager.h"
#include "PndSttSkewedHit.h"
#include "PndSttCATrackletGenerator.h"
#include "TTimeStamp.h"

using namespace std;

ClassImp(PndSttCA);

void PndSttCA::AddHits(TClonesArray *hits, TString branchName)
{

  fTrackFinderData->AddHits(hits, branchName);
}

void PndSttCA::FindTracks()
{

  // init TrackFinderData
  vector<Double_t> timesGenerateNeighorhoodData;

  // store timestamps for runtime analysis
  timesGenerateNeighorhoodData.push_back(TTimeStamp());
  fTrackFinderData->GenerateNeighborhoodData();
  timesGenerateNeighorhoodData.push_back(TTimeStamp());

  fTrackletGenerator = new PndSttCATrackletGenerator(fTrackFinderData);
  fTrackletGenerator->SetBz(fBz);
  fTrackletGenerator->SetUseGPU(fUseGPU);

  // calculation on GPU is only possible without multiple STTHits and a maximum number of MAX_THREADS_PER_BLOCK
  // cout << "fUseGPU: " << fUseGPU << " fTrackFinderData->GetAllowDoubleHits(): " << fTrackFinderData->GetAllowDoubleHits() << " fTrackFinderData->GetNumHitsWithoutDouble():" <<
  // fTrackFinderData->GetNumHitsWithoutDouble() << " MAX_THREADS_PER_BLOCK: " << MAX_THREADS_PER_BLOCK << endl;
  if (fUseGPU && (fTrackFinderData->GetAllowDoubleHits() == kFALSE) && (fTrackFinderData->GetNumHitsWithoutDouble() < MAX_THREADS_PER_BLOCK)) {
    fTrackletGenerator->SetDevTubeNeighboringsPointer(fDev_tubeNeighborings);
    // cout << "fDev_tubeNeighborings: " << *fDev_tubeNeighborings << endl;
  }

  fTrackletGenerator->FindTracks();

  StoreTrackData();

  // cpu time measurement
  fTimeStampsTrackletGen.push_back(fTrackletGenerator->GetTimeStamps());
  fTimeStampsGenerateNeighborhoodData.push_back(timesGenerateNeighorhoodData);
}

;
