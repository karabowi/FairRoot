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

#include "BSEmcWaveformGenerationProcess.h"

#include <math.h>
#include <stdlib.h>
#include <string>

#include "TRandom.h"
#include "TStopwatch.h"
#include "TString.h"

#include "FairLink.h"
#include "FairParSet.h"
#include "FairRootManager.h"
#include "FairWriteoutBuffer.h"
#include "fairlogger/Logger.h"

#include "PndContainerI.h"
#include "PndParameterRegister.h"
#include "PndProcess.h"

#include "BSEmcDataBranchNames.h"
#include "BSEmcDigiPar.h"

#include "BSEmcMCDeposit.h"
#include "BSEmcWaveformBuffer.h"
#include "BSEmcWaveformData.h"

BSEmcWaveformGenerationProcess::BSEmcWaveformGenerationProcess(BSEmcAbsWaveformSimulator *t_simulator, Bool_t t_storewaves, Bool_t t_storeData, Bool_t t_timebased)
  : PndProcess("BSEmcWaveformGenerationProcess"), fSimulator(t_simulator), fStoreWaves(t_storewaves), fStoreDataClass(t_storeData), fActivateBuffering(t_timebased)
{
}

//--------------
// Destructor --
//--------------

BSEmcWaveformGenerationProcess::~BSEmcWaveformGenerationProcess() {}

/**
 * @brief Sets the detectorname and fills the parameterlist with names of all required parameter
 *
 * @param t_detectorName
 */
void BSEmcWaveformGenerationProcess::SetDetectorName(const std::string &t_detectorName)
{
  this->fDetectorName = t_detectorName;
}

// -------------------------------------------------------------------------
void BSEmcWaveformGenerationProcess::RequestDataContainer(PndContainerRegister *t_register)
{
  // Lets use the defaults set by the task
  t_register->Request({fMCDepositBranchname, "BSEmcMCDeposit", kFALSE, kFALSE});

  FairRootManager *ioman = FairRootManager::Instance();
  if (ioman == nullptr) {
    LOG(error) << "BSEmcWaveformGenerationProcess::Init: "
               << "RootManager not instantiated!";
    exit(-1);
  }
  if (fWaveformBranchName == "") {
    fWaveformBranchName = BSEmcDataBranchNames::fgMultiWaveformBranchName + this->fDetectorName;
  }
  if (fWaveformDataBranchName == "") {
    fWaveformDataBranchName = BSEmcDataBranchNames::fgWaveformDataBranchName + this->fDetectorName;
  }
  // Create and activiate output Buffer....choose between BSEmcWaveform and BSEmcMultiWaveform
  fWaveformBuffer = new BSEmcWaveformBuffer(fWaveformBranchName, "BSEmcMultiWaveform", "Emc", fStoreWaves);
  LOG(info) << "Using BSEmcMultiWaveform in WaveformBuffer.";

  fWaveformBuffer = dynamic_cast<BSEmcWaveformBuffer *>(ioman->RegisterWriteoutBuffer(fWaveformBranchName, fWaveformBuffer));
  fWaveformBuffer->ActivateBuffering(fActivateBuffering);

  if (fStoreDataClass) {
    fWaveformBuffer->StoreWaveformData(fWaveformDataBranchName, "Emc", kTRUE);
  }
}

// -------------------------------------------------------------------------
void BSEmcWaveformGenerationProcess::GetDataContainer(PndContainerRegister *t_register)
{
  fMCDepositArray = t_register->GetInput<BSEmcMCDeposit>(fMCDepositBranchname);
  fMCDepositBranchname = fMCDepositArray->GetBranchName();

  t_register->SetAsDefaultBranchFor(fWaveformBranchName, BSEmcMultiWaveform().ClassName());
}

/**
 * @brief Fetches the parameteres requested in SetDetectorName
 *
 * @param t_parameterRegister
 */
void BSEmcWaveformGenerationProcess::SetupParameters(const PndParameterRegister *t_parameterRegister) /*override*/
{
  LOG(debug) << "BSEmcWaveformGenerationProcess::SetupParameters() - fetching " << fPhotonStatisticParName;
  fPhotonStatisticPar = dynamic_cast<BSEmcDigiPar *>(t_parameterRegister->GetParameter(fPhotonStatisticParName));
  fUse_photon_statistic = fPhotonStatisticPar->GetUseDigiEffectiveSmearingMode() == 1;

  if (fUse_photon_statistic) {
    LOG(debug) << "BSEmcWaveformGenerationProcess: " << fDetectorName << " using photon statistic";
    fNPhotoElectronsPerMeV = fPhotonStatisticPar->GetDetectedPhotonsPerMeV() * fPhotonStatisticPar->GetSensitiveArea() / fPhotonStatisticPar->GetRearCrystalSurfaceArea() *
                             fPhotonStatisticPar->GetQuantumEfficiency();
    fExcessNoiseFactor = fPhotonStatisticPar->GetExcessNoiseFactor();
  } else {
    fNPhotoElectronsPerMeV = 0;
    fExcessNoiseFactor = 1;
  }
  fOverlapTime = fPhotonStatisticPar->GetOverlapTime();
  LOG(debug) << "BSEmcWaveformGenerationProcess::Init() for " << fMCDepositBranchname << " done.";
}

/**
 * @brief Runs the Process.
 *
 * Uses the waveform simulator (BSEmcAbsWaveformSimulator) to generate a waveform
 * from the deposit and fill the buffer (BSEmcWaveformBuffer).
 *
 * @param opt unused
 * @return void
 */

void BSEmcWaveformGenerationProcess::Process()
{
  LOG(debug) << "BSEmcWaveformGenerationProcess " << fDetectorName << " Exec() ";

  TStopwatch timer;
  timer.Start();

  // Variable declaration
  const BSEmcMCDeposit *theDeposit = nullptr;

  // Loop over BSEmcMCDeposit to add them to correspondent waveforms
  Int_t nDeposits = fMCDepositArray->GetSize();
  LOG(debug) << "BSEmcWaveformGenerationProcess " << fDetectorName << " Deposit array contains " << nDeposits << " deposits";

  for (Int_t iDeposit = 0; iDeposit < nDeposits; iDeposit++) {

    theDeposit = fMCDepositArray->GetConstElementPtr(iDeposit);
    AddDepositData(theDeposit, iDeposit);
  }
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  LOG(debug) << "BSEmcWaveformGenerationProcess, Real time " << rtime << " s, CPU time " << ctime << " s";
}

void BSEmcWaveformGenerationProcess::AddDepositData(const BSEmcMCDeposit *t_deposit, const Int_t t_depositIndex)
{
  FairRootManager *ioman = FairRootManager::Instance();
  if (ioman == nullptr) {
    LOG(error) << "BSEmcWaveformGenerationProcess::AddDepositData no FairRootManager found.";
    return;
  }
  Double_t energy = t_deposit->GetEnergy();
  if (energy == 0) {
    return;
  }
  LOG(debug1) << "deposit energy : " << energy;
  if (fUse_photon_statistic) {
    Double_t crystalPhotonsMeV = 1.0e3 * energy * fNPhotoElectronsPerMeV;
    Double_t photonStatFactor = gRandom->Gaus(1, sqrt(fExcessNoiseFactor / crystalPhotonsMeV));
    LOG(debug1) << "photonStatFactor: " << photonStatFactor;
    energy *= photonStatFactor;
    LOG(debug1) << "energy now: " << energy;
  }
  // construct corresponding waveform data Object
  BSEmcWaveformData wfData(t_deposit->GetDetectorID(), fSimulator);
  wfData.SetOverlapTime(fOverlapTime);

  // register deposit...timebased framework uses ns, whereas emc deals with seconds as time unit
  FairLink linkToDeposit(-1, ioman->GetEntryNr(), fMCDepositBranchname, t_depositIndex, 1.0);
  if (fActivateBuffering) {
    wfData.AddDeposit(linkToDeposit, ioman->GetEventTime() + t_deposit->GetTime() * 1.0e9, energy);
  } else {
    wfData.AddDeposit(linkToDeposit, t_deposit->GetTime() * 1.0e9, energy);
  }
  LOG(debug1) << "BSEmcWaveformGenerationProcess " << fDetectorName << " adding deposit with event time " << ioman->GetEventTime()
              << " and deposit time: " << t_deposit->GetTime() * 1.0e9 << " and energy: " << energy;
  fWaveformBuffer->FillNewData(&wfData);
}
