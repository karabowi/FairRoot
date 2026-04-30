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

#ifndef BSEMCFWENDCAPAPDANDVPTTWAVEFORMGENERATOR_HH
#define BSEMCFWENDCAPAPDANDVPTTWAVEFORMGENERATOR_HH
#include <algorithm>
#include <memory>
#include <string>
#include <vector>

#include "Rtypes.h"
#include "RtypesCore.h"

#include "PndContainerI.h"
#include "PndParameterRegister.h"
#include "PndProcess.h"

#include "BSEmcFwEndcapCosmicWaveformGeneration.h"

class TClonesArray;
class BSEmcDigiPar;
class BSEmcMCDeposit;
class BSEmcDigi;
class PndParameterRegister;
class TBuffer;
class TClass;
class TMemberInspector;
template <class T>
class PndConstContainerI;
template <class T>
class PndMutableContainerI;

/**
 * @class BSEmcFwEndcapAPDAndVPTTWaveformGenerator
 * @brief IdealDigitizationProcess (non waveform) for FwEndcap requires special treatment since the FwEndcap has  VPTTs instead of APDs for some crystals
 * @details
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-01
 * @ingroup FwEndcap
 */
class BSEmcFwEndcapAPDAndVPTTWaveformGenerator : public PndProcess {
 public:
  BSEmcFwEndcapAPDAndVPTTWaveformGenerator();
  virtual ~BSEmcFwEndcapAPDAndVPTTWaveformGenerator();

  virtual void SetDetectorName(const std::string &t_detectorName) /*override*/;
  virtual void SetupParameters(const PndParameterRegister *t_parameterRegister) /*override*/;
  virtual void RequestDataContainer(PndContainerRegister *t_register) /*override*/;
  virtual void GetDataContainer(PndContainerRegister *t_register) /*override*/;
  virtual void Process() /*override*/;

  void SetVpttIds(const std::vector<Int_t> &t_vpttIds)
  {
    fVpttIds = t_vpttIds;
    std::sort(fVpttIds.begin(), fVpttIds.end());
  }
  void SetMCDepositBranchName(const TString &t_branchName) { fMCDepositBranchName = t_branchName; }
  void SetWaveformBranchName(const TString &t_branchName) { fWaveformBranchName = t_branchName; }
  void SetWaveformDataBranchName(const TString &t_branchName) { fWaveformDataBranchName = t_branchName; }
  void RunTimebased(Bool_t t_timebased = kTRUE)
  {
    fAPDWaveformGenerator->RunTimebased(t_timebased);
    fVPTTWaveformGenerator->RunTimebased(t_timebased);
  };
  void SetStorageOfData(Bool_t t_storeWaves = kTRUE)
  {
    fAPDWaveformGenerator->SetStorageOfData(t_storeWaves);
    fVPTTWaveformGenerator->SetStorageOfData(t_storeWaves);
  };
  void StoreDataClass(Bool_t t_storeData = kTRUE)
  {
    fAPDWaveformGenerator->StoreDataClass(t_storeData);
    fVPTTWaveformGenerator->StoreDataClass(t_storeData);
  };

 protected:
  virtual Bool_t IsVpttDeposit(const BSEmcMCDeposit *t_deposit) const;

 private:
  PndContainerI<BSEmcMCDeposit> *fMCDepositArray{nullptr};
  TString fMCDepositBranchName{""};
  TString fWaveformBranchName{""};
  TString fWaveformDataBranchName{""};
  std::unique_ptr<BSEmcFwEndcapCosmicWaveformGeneration> fAPDWaveformGenerator{new BSEmcFwEndcapCosmicWaveformGeneration()};
  std::unique_ptr<BSEmcFwEndcapCosmicWaveformGeneration> fVPTTWaveformGenerator{new BSEmcFwEndcapCosmicWaveformGeneration()};
  std::vector<Int_t> fVpttIds{}; // searching a vector for each deposit is bad.
                                 // Need a different approach.

  ClassDef(BSEmcFwEndcapAPDAndVPTTWaveformGenerator, 1);
};

#endif /*BSEMCFWENDCAPAPDANDVPTTWAVEFORMGENERATOR_HH*/
