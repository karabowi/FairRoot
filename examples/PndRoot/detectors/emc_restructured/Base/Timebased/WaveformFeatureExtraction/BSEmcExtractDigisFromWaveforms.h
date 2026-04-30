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

//#pragma once

#ifndef BSEMCEXTRACTDIGISFROMWAVEFORMS_HH
#define BSEMCEXTRACTDIGISFROMWAVEFORMS_HH

#include <PndPersistencyTask.h>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "TClonesArray.h"
#include "TStopwatch.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "PndTCAMutableContainer.h"

#include "BSEmcAbsPSA.h"
#include "BSEmcDigi.h"
#include "BSEmcDigiPar.h"
#include "BSEmcMCDeposit.h"
#include "BSEmcMultiWaveform.h"
#include "BSEmcWaveformData.h"

#include "math.h"

/**
 * @brief Task to create digis from waveforms.
 *
 * This is an alternative to BSEmcWaveformToDigi, which despite its name could be
 * used for all the EMC, not just FwEndcap.
 * @author Ph. Mahlberg <mahlberg@hiskp.uni-bonn.de>
 * @ingroup EmcBase
 */
template <class ParSet>
class BSEmcExtractDigisFromWaveforms : public PndPersistencyTask {
 public:
  // Constructors
  BSEmcExtractDigisFromWaveforms(const std::string &t_detectorname = "Barrel", Bool_t t_storedigis = kTRUE)
    : PndPersistencyTask("BSEmcExtractDigisFromWaveforms"), fDetectorName(t_detectorname)
  {
    SetPersistency(t_storedigis);
  }

  // Destructor
  virtual ~BSEmcExtractDigisFromWaveforms()
  {
    if (fPSA != nullptr) {
      delete fPSA;
      fPSA = nullptr;
    }
  }

  /**
   * @brief Init Task
   *
   * Prepares the TClonesArray of BSEmcMultiWaveform for reading and BSEmcDigi for writing.
   * Also reads the EMC parameters and prepares the pulseshapes
   * (BSEmcAbsPulseshape) and pulse shape analyser (BSEmcAbsPSA)
   *
   * @return InitStatus
   * @retval kSUCCESS success
   */
  virtual InitStatus Init()
  {

    LOG(debug) << "BSEmcExtractDigisFromWaveforms<ParSet>::Init";
    fMCDepositBranchName = BSEmcDataBranchNames::fgMCDepositBranchName + fDetectorName;
    if (fWaveformBranchName == "") {
      fWaveformBranchName = BSEmcDataBranchNames::fgMultiWaveformBranchName + fDetectorName;
    }
    if (fDigiBranchName == "") {
      fDigiBranchName = BSEmcDataBranchNames::fgDigiBranchName + fDetectorName;
    }
    // Get RootManager
    FairRootManager *ioman = FairRootManager::Instance();
    if (ioman == nullptr) {
      LOG(error) << "BSEmcExtractDigisFromWaveforms<ParSet>::Init: "
                 << "RootManager not instantiated!";
      return kFATAL;
    }
    fDigiArray.SetBranchName(fDigiBranchName);
    if (ioman->CheckBranch(fDigiBranchName) == 0) {
      ioman->Register(fDigiBranchName, "BSEmcDigi", "EMC", GetPersistency());
    }
    fDigiArray.SetTCA(dynamic_cast<TClonesArray *>(ioman->GetObject(fDigiBranchName)));

    LOG(debug) << "BSEmcExtractDigisFromWaveforms<ParSet>::Init: Registered " << fDigiBranchName;
    // Get input array
    fWaveformArray = (TClonesArray *)ioman->GetObject(fWaveformBranchName);
    if (fWaveformArray == nullptr) {
      // check if EmcWaveform contains MultiWaveforms
      fWaveformArray = (TClonesArray *)ioman->GetObject(fWaveformBranchName);
      if ((fWaveformArray == nullptr) || (!fWaveformArray->GetClass()->InheritsFrom("BSEmcMultiWaveform"))) {
        LOG(error) << "BSEmcExtractDigisFromWaveforms<ParSet>::Init: "
                   << "No BSEmcWaveform array containing multi waveforms!";
        return kFATAL;
      }
    }

    DefinePSA();
    if (fPSA == nullptr) {
      LOG(error) << "No PSA was defined. Aborting!";
      return kFATAL;
    }

    fEnergyDigiThreshold = fDigiPar->GetEnergyDigiThreshold();

    LOG(debug) << "BSEmcExtractDigisFromWaveforms: Intialization successfull";
    return kSUCCESS;
  }

  /**
   * @brief Runs the task.
   *
   * The task loops over the waveforms and uses the pulse shape analyser (BSEmcAbsPSA) to
   * extract signal height and timing. If the energy is above the threshold (@ref fEnergyDigiThreshold),
   * a BSEmcDigi is created.
   *
   * @param opt unused
   * @return void
   */

  virtual void Exec(Option_t * /*unused*/)
  {
    fDigiArray.Reset();
    // fDigiArray->Delete();
    TStopwatch timer;
    timer.Start();

    Double_t energy = NAN;
    Double_t digi_time = NAN;
    Int_t nDeposits = 0;
    Int_t detId = 0;
    Int_t nWaveforms = fWaveformArray->GetEntriesFast();
    BSEmcWaveform *theWaveform = nullptr;

    for (Int_t iWaveform = 0; iWaveform < nWaveforms; iWaveform++) {

      theWaveform = (BSEmcWaveform *)fWaveformArray->At(iWaveform);
      detId = theWaveform->GetDetectorId();

      nDeposits = fPSA->Process(theWaveform);

      for (Int_t iDeposit = 0; iDeposit < nDeposits; ++iDeposit) {
        fPSA->GetHit(iDeposit, energy, digi_time);

        // if (energy > fEnergyDigiThreshold) {
        Double_t timestamp = GetTimeStamp(theWaveform, digi_time);
        BSEmcDigi *myDigi = fDigiArray.CreateCopy(BSEmcDigi(detId, energy, timestamp - FairRootManager::Instance()->GetEventTime()));
        // BSEmcDigi *myDigi = new ((*fDigiArray)[fDigiArray->GetEntriesFast()]) BSEmcDigi(detId, energy, timestamp);
        myDigi->SetGainType(GetGainType(theWaveform, iDeposit));
        myDigi->ResetLinks();
        myDigi->AddLinks(theWaveform->GetLinksWithType(FairRootManager::Instance()->GetBranchId(fMCDepositBranchName)));
        FairMultiLinkedData mcdeplinks = myDigi->GetLinksWithType(FairRootManager::Instance()->GetBranchId(fMCDepositBranchName));
        for (const FairLink &link : mcdeplinks.GetLinks()) {
          LOG(debug) << link;
        }
        LOG(debug) << "BSEmcExtractDigisFromWaveforms for " << fDetectorName << " created Digi(detId: " << detId << ", energy: " << energy << ", timestamp: " << timestamp
                   << ") and has " << myDigi->GetLinksWithType(FairRootManager::Instance()->GetBranchId(fMCDepositBranchName)).GetNLinks() << " links to EmcMCDeposits";
        // }
      }
    }

    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    LOG(debug) << "BSEmcExtractDigisFromWaveforms, Real time " << rtime << " s, CPU time " << ctime << " s";
  }

  void SetStorageOfData(Bool_t t_val)
  {
    SetPersistency(t_val);
    return;
  }
  void SetDigiBranchName(const TString &t_digiBranchName) { fDigiBranchName = t_digiBranchName; }
  void SetWaveformBranchName(const TString &t_digiBranchName) { fWaveformBranchName = t_digiBranchName; }

 protected:
  /** Get parameter containers **/
  virtual void SetParContainers()
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
    fDigiPar = dynamic_cast<ParSet *>(db->getContainer(ParSet::fgParameterName.c_str()));
  }
  virtual Double_t GetTimeStamp(BSEmcWaveform *t_waveform, Double_t t_digi_time) const
  {
    Double_t sampleRate = t_waveform->GetSampleRate();

    t_digi_time /= sampleRate;
    t_digi_time *= 1e9; // ns
    return t_waveform->GetTimeStamp() + t_digi_time;
  }

  virtual BSEmcDigi::eGAIN GetGainType(BSEmcWaveform *t_waveform, Int_t t_dep) const = 0;
  virtual void DefinePSA() = 0;

 protected:
  const std::string fDetectorName{""};
  TString fMCDepositBranchName{""};
  /** Input array of BSEmcWaveforms **/
  // PndTCAConstContainer<BSEmcWaveform> fWaveformArray;
  PndTCAMutableContainer<BSEmcDigi> fDigiArray{"Emc", kTRUE};
  TClonesArray *fWaveformArray{nullptr};
  // TClonesArray *fDigiArray{nullptr};
  TString fDigiBranchName{""};
  TString fWaveformBranchName{""};
  Double_t fEnergyDigiThreshold{0};

  BSEmcAbsPSA *fPSA{nullptr};

  ParSet *fDigiPar{nullptr}; //!< Timebased Digitisation parameter container
  ClassDef(BSEmcExtractDigisFromWaveforms, 1);
};

#endif /*BSEMCEXTRACTDIGISFROMWAVEFORMS_HH*/
