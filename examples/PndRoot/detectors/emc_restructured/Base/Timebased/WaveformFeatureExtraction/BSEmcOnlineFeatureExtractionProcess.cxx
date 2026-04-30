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

#include "BSEmcOnlineFeatureExtractionProcess.h"

// #include <bits/exception.h>
#include <math.h>
#include <set>

#include "TStopwatch.h"
#include "TString.h"

#include "FairLink.h"
#include "FairMultiLinkedData.h"
#include "FairRootManager.h"
#include "fairlogger/Logger.h"

#include "PndMutableContainerI.h"

#include "BSEmcAbsPSA.h"
#include "BSEmcDataBranchNames.h"
#include "BSEmcDigi.h"

#include "BSEmcMultiWaveform.h"
#include "BSEmcWaveform.h"

BSEmcOnlineFeatureExtractionProcess::BSEmcOnlineFeatureExtractionProcess() : PndProcess("BSEmcOnlineFeatureExtractionProcess") {}

//--------------
// Destructor --
//--------------
BSEmcOnlineFeatureExtractionProcess::~BSEmcOnlineFeatureExtractionProcess()
{
  if (fPSA != nullptr) {
    delete fPSA;
    fPSA = nullptr;
  }
}

/**
 * @brief Sets the detectorname and fills the parameterlist with names of all required parameter
 *
 * @param t_detectorName
 */
void BSEmcOnlineFeatureExtractionProcess::SetDetectorName(const std::string &t_detectorName)
{
  this->fDetectorName = t_detectorName;
  fMCDepositBranchName = BSEmcDataBranchNames::fgMCDepositBranchName + this->fDetectorName;
}

// -------------------------------------------------------------------------
void BSEmcOnlineFeatureExtractionProcess::RequestDataContainer(PndContainerRegister *t_register)
{
  // Lets use the defaults set by the task
  t_register->Request({fWaveformBranchName, "BSEmcMultiWaveform", kFALSE, kFALSE});
  t_register->Request({fDigiBranchName, "BSEmcDigi", kTRUE, kTRUE});
}

// -------------------------------------------------------------------------
void BSEmcOnlineFeatureExtractionProcess::GetDataContainer(PndContainerRegister *t_register)
{
  fWaveformArray = t_register->GetInput<BSEmcMultiWaveform>(fWaveformBranchName);
  fDigiArray = t_register->GetOutput<BSEmcDigi>(fDigiBranchName);
}

/**
 * @brief Fetches the parameteres requested in SetDetectorName
 *
 * @param t_parameterRegister
 */
void BSEmcOnlineFeatureExtractionProcess::SetupParameters(const PndParameterRegister * /*t_parameterRegister*/) /*override*/ {}

Double_t BSEmcOnlineFeatureExtractionProcess::GetTimeStamp(const BSEmcWaveform *t_waveform, Double_t t_digi_time) const
{
  Double_t sampleRate = t_waveform->GetSampleRate();

  t_digi_time /= sampleRate;
  t_digi_time *= 1e9; // ns
  return t_waveform->GetTimeStamp() + t_digi_time;
}

/**
 * @brief Runs the process.
 *
 * Loops over the waveforms and uses the pulse shape analyser (BSEmcAbsPSA) to
 * extract signal height and timing. As energy is not known at this point, every extracted feature is
 * transformed into an BSEmcDigi.
 *
 */
void BSEmcOnlineFeatureExtractionProcess::Process()
{
  fDigiArray->Reset();
  std::vector<BSEmcDigi *> digis = ExtractDigis(fWaveformArray->GetVectorOfPtrToConst());
  for (BSEmcDigi *digi : digis) {
    fDigiArray->CreateCopy(*digi);
    delete digi;
  }
}

/**
 * @brief Runs the process.
 *
 * Loops over the waveforms and uses the pulse shape analyser (BSEmcAbsPSA) to
 * extract signal height and timing. As energy is not known at this point, every extracted feature is
 * transformed into an BSEmcDigi.
 *
 */
std::vector<BSEmcDigi *> BSEmcOnlineFeatureExtractionProcess::ExtractDigis(const std::vector<const BSEmcMultiWaveform *> &t_waveforms) const
{

  std::vector<BSEmcDigi *> result;
  TStopwatch timer;
  timer.Start();

  Double_t energy = NAN;
  Double_t digi_time = NAN;
  Int_t nDeposits = 0;
  Int_t detId = 0;

  for (const BSEmcWaveform *theWaveform : t_waveforms) {
    detId = theWaveform->GetDetectorId();
    nDeposits = fPSA->Process(theWaveform);
    bool isDigitized = kFALSE;
    for (Int_t iDeposit = 0; iDeposit < nDeposits; ++iDeposit) {
      fPSA->GetHit(iDeposit, energy, digi_time);
      Double_t timestamp = GetTimeStamp(theWaveform, digi_time);
      BSEmcDigi *myDigi = new BSEmcDigi(detId, energy, timestamp - fEventTime);
      result.push_back(myDigi);
      BSEmcDigi::eGAIN gain = GetGainType(theWaveform, iDeposit);
      myDigi->SetGainType(gain);
      myDigi->ResetLinks();
      myDigi->AddLinks(theWaveform->GetLinksWithType(FairRootManager::Instance()->GetBranchId(fMCDepositBranchName)));
      FairMultiLinkedData mcdeplinks = myDigi->GetLinksWithType(FairRootManager::Instance()->GetBranchId(fMCDepositBranchName));
      for (const FairLink &link : mcdeplinks.GetLinks()) {
        LOG(debug) << link;
      }
      LOG(debug) << "BSEmcOnlineFeatureExtractionProcess for " << this->fDetectorName << " created Digi(detId: " << detId << ", energy: " << energy << ", timestamp: " << timestamp
                 << ") and has " << myDigi->GetLinksWithType(FairRootManager::Instance()->GetBranchId(fMCDepositBranchName)).GetNLinks() << " links to EmcMCDeposits";
      LOG_IF(debug, isDigitized) << "Two digis created for Crystal " << detId;
      isDigitized = kTRUE;
    }
  }

  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  LOG(debug) << "BSEmcOnlineFeatureExtractionProcess, Real time " << rtime << " s, CPU time " << ctime << " s for " << t_waveforms.size() << " waveforms";
  return result;
}
