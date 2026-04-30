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
#ifndef BSEMCWAVEFORMGENERATIONPROCESS_HH
#define BSEMCWAVEFORMGENERATIONPROCESS_HH

#include <string>

#include "Rtypes.h"
#include "RtypesCore.h"
#include "TString.h"

#include "PndProcess.h"

#include "BSEmcDigiPar.h"

class PndParameterRegister;
class BSEmcAbsWaveformSimulator;
class BSEmcWaveformBuffer;
class BSEmcDigiPar;
class BSEmcMCDeposit;
class TBuffer;
class TClass;
class TMemberInspector;
template <class T>
class PndContainerI;

/**
 * @brief Base class Process to create waveforms out of BSEmcMCDeposits.
 *
 * This is a base class for emc detector specific waveform generation processes.
 * Derived processes need to implement and set the BSEmcAbsWaveformSimulator.
 * The BSEmcAbsWaveformSimulator should actually be a
 * BSEmcMultiWaveformSimulator
 * @author Phil Strother                  	Original author
 * @author Dima Melnichuk 			- adaption for PANDA
 * @author Philipp Mahlberg	<mahlberg@hiskp.uni-bonn.de>	- integration
 * in timebased
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de> - process changes
 * @ingroup EmcBase
 */
class BSEmcWaveformGenerationProcess : public PndProcess {
 public:
  // Constructors
  BSEmcWaveformGenerationProcess(BSEmcAbsWaveformSimulator *t_simulator = nullptr, Bool_t t_storewaves = kTRUE, Bool_t t_storeData = kTRUE, Bool_t t_timebased = kFALSE);
  // Destructor
  virtual ~BSEmcWaveformGenerationProcess();
  virtual void RequestDataContainer(PndContainerRegister *t_register) /*override*/;
  virtual void GetDataContainer(PndContainerRegister *t_register) /*override*/;
  virtual void SetDetectorName(const std::string &t_detectorName) /*override*/;
  virtual void SetupParameters(const PndParameterRegister *t_parameterRegister) /*override*/;
  virtual void Process() /*override*/;
  void AddDepositData(const BSEmcMCDeposit *t_deposit, const Int_t t_depositIndex);

  void RunTimebased(Bool_t t_timebased = kTRUE) { fActivateBuffering = t_timebased; };
  void SetStorageOfData(Bool_t t_storeWaves = kTRUE) { fStoreWaves = (t_storeWaves); };
  void StoreDataClass(Bool_t t_storeData = kTRUE) { fStoreDataClass = t_storeData; };

  void SetMCDepositBranchName(const TString &t_branchName) { fMCDepositBranchname = t_branchName; }
  void SetWaveformBranchName(const TString &t_branchName) { fWaveformBranchName = t_branchName; }
  void SetWaveformDataBranchName(const TString &t_branchName) { fWaveformDataBranchName = t_branchName; }
  void SetPhotonStatisticParName(const std::string &t_photonStatParName) { fPhotonStatisticParName = t_photonStatParName; }

 protected:
  void SetSimulator(BSEmcAbsWaveformSimulator *t_simulator) { fSimulator = t_simulator; }
  TString fMCDepositBranchname{""};
  TString fWaveformBranchName{""};
  TString fWaveformDataBranchName{""};
  PndContainerI<BSEmcMCDeposit> *fMCDepositArray{nullptr};
  /** Output array of BSEmcWaveforms **/
  BSEmcWaveformBuffer *fWaveformBuffer{nullptr};
  BSEmcAbsWaveformSimulator *fSimulator{nullptr};

  Bool_t fStoreWaves{kFALSE};
  Bool_t fStoreDataClass{kFALSE};
  Bool_t fActivateBuffering{kFALSE};
  std::string fPhotonStatisticParName{""};
  BSEmcDigiPar *fPhotonStatisticPar{nullptr};
  Bool_t fUse_photon_statistic{kFALSE};
  Double_t fNPhotoElectronsPerMeV{0};
  Double_t fExcessNoiseFactor{0};
  Double_t fOverlapTime{0};

  ClassDef(BSEmcWaveformGenerationProcess, 2);
};

#endif /*BSEMCWAVEFORMGENERATIONPROCESS_HH*/
