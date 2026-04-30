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

//----------------------------------------------------------------------
// Author List:
//      Phil Strother                  	Original author
// 	Dima Melnichuk 			- adaption for PANDA
// 	Philipp Mahlberg		- integration in timebased simulation concept
//----------------------------------------------------------------------

// #pragma once
#ifndef BSEMCMCDEPOSITTOTIMEBASEDWAVEFORMS_HH
#define BSEMCMCDEPOSITTOTIMEBASEDWAVEFORMS_HH

#include <PndPersistencyTask.h>
#include <cassert>
#include <string>
#include <vector>

#include "TClonesArray.h"
#include "TROOT.h"
#include "TRandom.h"
#include "TStopwatch.h"
#include "TString.h"

#include "FairLink.h"
#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "PndTCAConstContainer.h"

#include "BSEmcAbsWaveformSimulator.h"
#include "BSEmcDigiPar.h"
#include "BSEmcMCDeposit.h"
#include "BSEmcWaveform.h"
#include "BSEmcWaveformBuffer.h"
#include "BSEmcWaveformData.h"

/**
 * @brief Taks to create waveforms from deposits.
 *
 * This is an alternative to PndEmcDepositsToWaveform, which despite its name could be
 * used for all the EMC, not just FwEndcap.
 * @author Dima Melnichuk 			- adaption for PANDA
 * @author 	Philipp Mahlberg <mahlberg@hiskp.uni-bonn.de> - integration in timebased simulation concept
 * @ingroup EmcBase
 */
template <class ParSet>
class BSEmcMCDepositToTimebasedWaveforms : public PndPersistencyTask {
 public:
  // Constructors
  BSEmcMCDepositToTimebasedWaveforms(const std::string &t_detectorname, Bool_t t_storewaves = kTRUE)
    : PndPersistencyTask("BSEmcMCDepositToTimebasedWaveforms"), fDetectorName(t_detectorname)
  {
    SetPersistency(t_storewaves);
  }

  // Destructor
  virtual ~BSEmcMCDepositToTimebasedWaveforms(){};

  /**
   * @brief Init Task
   *
   * Prepares the TClonesArray of BSEmcMCDeposit for reading and
   * BSEmcMultiWaveform for writing.
   * Also reads the EMC parameters.
   *
   * @return InitStatus
   * @retval kSUCCESS success
   */
  virtual InitStatus Init() /*override*/
  {
    if (fMCDepositBranchName == "") {
      fMCDepositBranchName = BSEmcDataBranchNames::fgMCDepositBranchName + this->fDetectorName;
    }

    if (fWaveformBranchName == "") {
      fWaveformBranchName = BSEmcDataBranchNames::fgMultiWaveformBranchName + this->fDetectorName;
    }
    if (fWaveformDataBranchName == "") {
      fWaveformDataBranchName = BSEmcDataBranchNames::fgWaveformDataBranchName + this->fDetectorName;
    }

    if (fSimulator == nullptr) {
      LOG(debug) << "BSEmcMCDepositToTimebasedWaveforms for " << fMCDepositBranchName << " has no Simulator set. Calling SetupSimulator!";
      SetupSimulator();
      fSimulator->SetMCDepositBranchName(fMCDepositBranchName);
    }
    if (fSimulator == nullptr) {
      LOG(error) << "BSEmcMCDepositToTimebasedWaveforms for " << fMCDepositBranchName << " has no Simulator set. Aborting!";
      exit(-1);
    }

    // Get RootManager
    FairRootManager *ioman = FairRootManager::Instance();
    if (ioman == nullptr) {
      LOG(error) << "BSEmcMCDepositToTimebasedWaveforms<ParSet>::Init: "
                 << "RootManager not instantiated!";
      return kFATAL;
    }
    fMCDepositArray.SetBranchName(fMCDepositBranchName);
    fMCDepositArray.SetTCA(dynamic_cast<TClonesArray *>(ioman->GetObject(fMCDepositBranchName)));
    // Create and activiate output Buffer....choose between BSEmcWaveform and BSEmcMultiWaveform
    fWaveformBuffer = new BSEmcWaveformBuffer(fWaveformBranchName, "BSEmcMultiWaveform", "Emc", GetPersistency());
    LOG(debug) << "Using BSEmcMultiWaveform in WaveformBuffer.";

    fWaveformBuffer = (BSEmcWaveformBuffer *)ioman->RegisterWriteoutBuffer(fWaveformBranchName, fWaveformBuffer);
    fWaveformBuffer->ActivateBuffering(fActivateBuffering);

    if (fStoreDataClass) {
      fWaveformBuffer->StoreWaveformData(fWaveformDataBranchName, "Emc", kTRUE);
    }

    fUse_photon_statistic = fPhotonStatisticPar->GetUseDigiEffectiveSmearingMode() == 1;

    if (fUse_photon_statistic) {
      LOG(debug) << "BSEmcMCDepositToTimebasedWaveforms: " << fDetectorName << " using photon statistic";
      fNPhotoElectronsPerMeV = fPhotonStatisticPar->GetDetectedPhotonsPerMeV() * fPhotonStatisticPar->GetSensitiveArea() / fPhotonStatisticPar->GetRearCrystalSurfaceArea() *
                               fPhotonStatisticPar->GetQuantumEfficiency();
      fExcessNoiseFactor = fPhotonStatisticPar->GetExcessNoiseFactor();
    } else {
      fNPhotoElectronsPerMeV = 0;
      fExcessNoiseFactor = 1;
    }
    fOverlapTime = fPhotonStatisticPar->GetOverlapTime();
    LOG(debug) << "BSEmcMCDepositToTimebasedWaveforms::Init() for " << fMCDepositBranchName << " done.";
    return kSUCCESS;
  }

  /**
   * @brief Runs the task.
   *
   * Uses the waveform simulator (BSEmcAbsWaveformSimulator) to generate a waveform
   * from the deposit and fill the buffer (BSEmcWaveformBuffer).
   *
   * @param opt unused
   * @return void
   */
  virtual void Exec(Option_t * /*t_opt*/) /*override*/
  {
    LOG(debug) << "BSEmcMCDepositToTimebasedWaveforms<ParSet> " << fDetectorName << " Exec() ";
    FairRootManager *ioman = FairRootManager::Instance();

    TStopwatch timer;
    timer.Start();

    const BSEmcMCDeposit *theDeposit = nullptr;

    Int_t nDeposits = fMCDepositArray.GetSize();
    LOG(debug) << "BSEmcMCDepositToTimebasedWaveforms<ParSet> " << fDetectorName << " Deposit array contains " << nDeposits << " deposits";

    for (Int_t iDeposit = 0; iDeposit < nDeposits; iDeposit++) {

      theDeposit = fMCDepositArray.GetConstElementPtr(iDeposit);
      Double_t energy = theDeposit->GetEnergy();
      if (energy == 0) {
        continue;
      }
      LOG(debug3) << "deposit energy : " << energy;
      if (fUse_photon_statistic) {
        Double_t crystalPhotonsMeV = 1.0e3 * energy * fNPhotoElectronsPerMeV;
        Double_t photonStatFactor = gRandom->Gaus(1, sqrt(fExcessNoiseFactor / crystalPhotonsMeV));
        LOG(debug3) << "photonStatFactor: " << photonStatFactor;
        energy *= photonStatFactor;
        LOG(debug3) << "energy now: " << energy;
        // energy *= 1e3;
        LOG(debug3) << "energy now in MeV: " << energy;
      }
      // construct corresponding waveform data Object
      BSEmcWaveformData wfData(theDeposit->GetDetectorID(), fSimulator);
      wfData.SetOverlapTime(fOverlapTime);
      // register deposit...timebased framework uses ns, whereas emc deals with seconds as time unit
      FairLink linkToDeposit(-1, ioman->GetEntryNr(), fMCDepositBranchName, iDeposit, 1.0);
      if (fActivateBuffering) {
        wfData.AddDeposit(linkToDeposit, ioman->GetEventTime() + theDeposit->GetTime() * 1.0e9, energy);
      } else {
        wfData.AddDeposit(linkToDeposit, theDeposit->GetTime() * 1.0e9, energy);
      }
      LOG(debug) << "BSEmcMCDepositToTimebasedWaveforms " << fDetectorName << " adding deposit with event time " << ioman->GetEventTime()
                 << " and deposit time: " << theDeposit->GetTime() * 1.0e9 << " and energy: " << energy;
      fWaveformBuffer->FillNewData(&wfData);
    }
    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    LOG(debug) << "BSEmcMCDepositToTimebasedWaveforms, Real time " << rtime << " s, CPU time " << ctime << " s";
  }

  void RunTimebased(Bool_t t_timebased = kTRUE) { fActivateBuffering = t_timebased; };
  void SetSimulator(BSEmcAbsWaveformSimulator *t_simulator) { fSimulator = t_simulator; }
  void SetStorageOfData(Bool_t t_storeWaves = kTRUE) { SetPersistency(t_storeWaves); }; //!< Method to specify whether waveforms are stored or not.
  void StoreDataClass(Bool_t t_storeData = kTRUE) { fStoreDataClass = t_storeData; };

  void SetMCDepositBranchName(const TString &t_branchName) { fMCDepositBranchName = t_branchName; }
  void SetWaveformBranchName(const TString &t_branchName) { fWaveformBranchName = t_branchName; }
  void SetWaveformDataBranchName(const TString &t_branchName) { fWaveformDataBranchName = t_branchName; }
  void SetPhotonStatisticParName(const std::string &t_photonStatParName) { fPhotonStatisticParName = t_photonStatParName; }
  void SetWaveformGenParName(const std::string &t_waveformGenParName) { fWaveformGenParName = t_waveformGenParName; }

 protected:
  /** Get parameter containers **/
  virtual void SetParContainers() /*override*/
  {
    // Get run and runtime database
    FairRun *run = FairRun::Instance();
    if (run == nullptr) {
      Fatal("SetParContainers", "No analysis run");
    }
    FairRuntimeDb *db = run->GetRuntimeDb();
    if (db == nullptr) {
      Fatal("SetParContainers", "No runtime database");
    }
    // Get Emc digitisation parameter container
    if (fWaveformGenParName == "") {
      fWaveformGenParName = ParSet::fgParameterName;
    }
    fDigiPar = dynamic_cast<ParSet *>(db->getContainer(fWaveformGenParName.c_str()));

    if (fPhotonStatisticParName == "") {
      fPhotonStatisticParName = BSEmcDigiPar::fgParameterName + fDetectorName;
    }
    fPhotonStatisticPar = dynamic_cast<BSEmcDigiPar *>(db->getContainer(fPhotonStatisticParName.c_str()));
  }
  virtual void SetupSimulator() = 0;

  /** Input array**/
  TString fDetectorName{""};
  TString fMCDepositBranchName{""};
  TString fWaveformBranchName{""};
  TString fWaveformDataBranchName{""};
  PndTCAConstContainer<BSEmcMCDeposit> fMCDepositArray{};
  /** Output array of BSEmcWaveforms **/
  BSEmcWaveformBuffer *fWaveformBuffer{nullptr};

  Bool_t fStoreDataClass{kFALSE};
  Bool_t fActivateBuffering{kFALSE};
  std::string fPhotonStatisticParName{""};
  BSEmcDigiPar *fPhotonStatisticPar{nullptr};

  std::string fWaveformGenParName{""};
  ParSet *fDigiPar{nullptr}; //!< Digitisation parameter container
  Bool_t fUse_photon_statistic{kFALSE};
  Double_t fNPhotoElectronsPerMeV{0};
  Double_t fExcessNoiseFactor{0};
  Double_t fOverlapTime{0};
  BSEmcAbsWaveformSimulator *fSimulator{nullptr};

  ClassDef(BSEmcMCDepositToTimebasedWaveforms, 2);
};

#endif /*BSEMCMCDEPOSITTOTIMEBASEDWAVEFORMS_HH*/
