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

#include "PndMvdReadInTBDataTask.h"

#include <iostream>
#include <fstream>

// Root includes
#include "TROOT.h"
#include "TString.h"
#include "TClonesArray.h"
#include "TParticlePDG.h"
#include "PndSdsDigiTopix4.h"
#include "PndSdsDigiTopix4Header.h"

// framework includes
#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairRunAna.h"

// PndMvd includes

PndMvdReadInTBDataTask::PndMvdReadInTBDataTask() : FairTask("MVDReadInTBDataTask"), fClockFrequency(50), fEvent(0) {}

PndMvdReadInTBDataTask::~PndMvdReadInTBDataTask()
{
  for (int i = 0; i < fReader.size(); i++) {
    delete (fReader[i]);
  }
  fReader.clear();
}

void PndMvdReadInTBDataTask::SetParContainers()
{
  FairRuntimeDb *rtdb = FairRunAna::Instance()->GetRuntimeDb();
}

InitStatus PndMvdReadInTBDataTask::ReInit()
{
  fInitDone = kTRUE;
  return kSUCCESS;
}

// -----   Public method Init   --------------------------------------------
InitStatus PndMvdReadInTBDataTask::Init()
{
  FairRootManager *ioman = FairRootManager::Instance();
  std::cout << "PndMvdReadInTBDataTask::Init start of Init" << std::endl;

  if (!ioman) {
    std::cout << "-E- PndMvdReadInTBDataTask::Init: "
              << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  for (int i = 0; i < fFileNames.size(); i++) {
    std::cout << "PndMvdReadInTBDataTask::Init FileName: " << fFileNames[i][0] << std::endl;
    fReader.push_back(new PndMvdReadInToPix4TBData());
    fEndOfFile.push_back(kFALSE);
    fReader[i]->SetFileName(fFileNames[i]);
    fReader[i]->SetClockFrequency(fClockFrequency);
    fReader[i]->SetVerbose(fVerbose);
    fReader[i]->SetFE(i + 1);
    std::cout << "PndMvdReadInTBDataTask::Init before Reader Init" << std::endl;
    fReader[i]->Init();
  }

  fDigiArray = ioman->Register("ToPix4Hits", "PndSdsDigiTopix4", "MVD", kTRUE);
  fFrameHeaderArray = ioman->Register("ToPix4FrameHeader", "PndSdsDigiTopix4Header", "MVD", kTRUE);
  // fAllFrameHeaderArray = ioman->Register("AllToPix4FrameHeader","PndSdsDigiTopix4Header","MVD", kTRUE);

  LOG(info) << " PndMvdReadInTBDataTask: Initialisation successfull";
  fInitDone = kTRUE;
  return kSUCCESS;
}

// -----   Public method Exec   --------------------------------------------
void PndMvdReadInTBDataTask::Exec(Option_t *)
{
  if (fEvent % 10000 == 0) {
    std::cout << "PndMvdReadInTBDataTask::Exec called - Event " << std::dec << fEvent << std::endl;
  }
  fEvent++;

  for (int i = 0; i < fReader.size(); i++) {
    if (fEndOfFile[i] != kTRUE) {
      if (fVerbose > 1)
        std::cout << std::endl << "Reader: " << i << " running " << std::endl;
      std::vector<std::vector<PndSdsDigiTopix4>> data;
      fEndOfFile[i] = fReader[i]->ReadInData(data);
      WriteoutToPix4Frames(data);
    }
  }
  Bool_t endOfFiles = kTRUE;
  for (int j = 0; j < fEndOfFile.size(); j++) {
    endOfFiles &= fEndOfFile[j];
  }
  if (endOfFiles == kTRUE) {
    std::cout << "Number of: " << std::endl;
    for (int i = 0; i < fReader.size(); i++) {
      UInt_t nonSequential = fReader[i]->GetNonSequenctialFC();
      std::cout << i << " : non sequential header " << nonSequential << " double header: " << fReader[i]->GetDoubleHeader() << " double trailer: " << fReader[i]->GetDoubleTrailer()
                << std::endl;
      std::cout << i << " SuperFrameCount: " << fReader[i]->GetSuperFrameCount() << std::endl;

      std::cout << i << " Total Header Count : " << fReader[i]->GetTotalHeaderCount() << std::endl;
      std::cout << i << " Total Trailer Count : " << fReader[i]->GetTotalTrailerCount() << std::endl;

      std::cout << i << " Total (build) Frames : " << fReader[i]->GetTotalFrameCount() << std::endl;
      std::cout << i << " Hamming Loss Frame Count: " << fReader[i]->GetHammingLossFrameCount() << std::endl;
      std::cout << i << " CRC Loss Frame Count: " << fReader[i]->GetCRCLossFrameCount() << std::endl;
      std::cout << i << " Correct Frame Count: " << fReader[i]->GetCorrectFrameCount() << std::endl;

      std::cout << i << " Total Hits : " << fReader[i]->GetTotalHitCount() << std::endl;
      std::cout << i << " Pre Frame Loss Hit Count : " << fReader[i]->GetPreFrameLossHitCount() << std::endl;
      std::cout << i << " Hamming Loss Hit Count : " << fReader[i]->GetHammingLossHitCount() << std::endl;
      std::cout << i << " CRC Loss Hit Count : " << fReader[i]->GetCRCLossHitCount() << std::endl;
      std::cout << i << " Correct Hit Count : " << fReader[i]->GetCorrectHitCount() << std::endl;
    }
    FairRootManager::Instance()->SetFinishRun(kTRUE);
  }
}

void PndMvdReadInTBDataTask::FinishEvent()
{
  fDigiArray->Delete();
  fFrameHeaderArray->Delete();
  //	fAllFrameHeaderArray->Delete();
}

void PndMvdReadInTBDataTask::SetNumberOfFrontEnds(Int_t numberfrontends)
{
  for (int i = 0; i < numberfrontends; i++) {
    std::vector<std::string> vector;
    fFileNames.push_back(vector);
  }
}

void PndMvdReadInTBDataTask::WriteoutToPix4Digi(PndSdsDigiTopix4 &data)
{
  new ((*fDigiArray)[fDigiArray->GetEntriesFast()]) PndSdsDigiTopix4(data);
}

void PndMvdReadInTBDataTask::WriteoutToPix4Frames(std::vector<std::vector<PndSdsDigiTopix4>> &frames)
{
  for (auto it1 : frames) {
    for (auto it2 : it1) {
      WriteoutToPix4Digi(it2);
    }
  }
}

ClassImp(PndMvdReadInTBDataTask);
