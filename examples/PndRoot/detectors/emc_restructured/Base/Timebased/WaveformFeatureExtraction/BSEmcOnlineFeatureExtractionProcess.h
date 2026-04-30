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

#ifndef BSEMCONLINEFEATUREEXTRACTIONPROCESS_HH
#define BSEMCONLINEFEATUREEXTRACTIONPROCESS_HH

#include <memory>
#include <string>
#include <vector>

#include "Rtypes.h"
#include "RtypesCore.h"
#include "TString.h"

#include "PndContainerI.h"
#include "PndMutableContainerI.h"
#include "PndProcess.h"

#include "BSEmcDigi.h"

#include "BSEmcMultiWaveform.h"

class PndParameterRegister;
class BSEmcAbsPSA;
class BSEmcWaveform;
class TBuffer;
class TClass;
class TMemberInspector;
template <class T>
class PndMutableContainerI;

/**
 * @brief Online feature extraction process intended as baseclass. Online like, but not actually online.
 *
 * Derived processes only need to implement and set the PSA that should be used.
 * Extracts Features (energy and time) from BSEmcMultiWaveform objects and creates BSEmcDigis out of those Features
 *
 * @author Ph. Mahlberg <mahlberg@hiskp.uni-bonn.de>, process changes -> Ben Salisbury <salisbury@hiskp.uni-bonn.de>
 * @ingroup EmcBase
 */
class BSEmcOnlineFeatureExtractionProcess : public PndProcess {
 public:
  BSEmcOnlineFeatureExtractionProcess();
  virtual ~BSEmcOnlineFeatureExtractionProcess();

  virtual void SetDetectorName(const std::string &t_detectorName) /*override*/;
  virtual void RequestDataContainer(PndContainerRegister *t_register) /*override*/;
  virtual void GetDataContainer(PndContainerRegister *t_register) /*override*/;
  virtual void SetupParameters(const PndParameterRegister *t_parameterRegister) /*override*/;
  virtual void Process() /*override*/;

  void SetStorageOfData(Bool_t t_val); // Method to specify whether digis are stored or not.
  void SetPSA(BSEmcAbsPSA *t_psa) { fPSA = t_psa; }
  std::vector<BSEmcDigi *> ExtractDigis(const std::vector<const BSEmcMultiWaveform *> &t_waveforms) const;
  void SetMCDepositBranchName(const TString &t_branchname) { fMCDepositBranchName = t_branchname; }
  void SetWaveformBranchName(const TString &t_branchname) { fWaveformBranchName = t_branchname; }
  void SetDigiBranchName(const TString &t_branchname) { fDigiBranchName = t_branchname; }

 protected:
  virtual Double_t GetTimeStamp(const BSEmcWaveform *t_waveform, Double_t t_digi_time) const;
  virtual BSEmcDigi::eGAIN GetGainType(const BSEmcWaveform *t_waveform, Int_t t_deposit) const = 0;

 protected:
  Double_t fTimebunchCutTime{0};

  TString fMCDepositBranchName{""};
  TString fWaveformBranchName{""};
  TString fDigiBranchName{""};

  PndMutableContainerI<BSEmcDigi> *fDigiArray{nullptr};
  PndContainerI<BSEmcMultiWaveform> *fWaveformArray{};

  Double_t fEnergyDigiThreshold{0};

  BSEmcAbsPSA *fPSA{nullptr};

  ClassDef(BSEmcOnlineFeatureExtractionProcess, 1);
};

#endif /*BSEMCONLINEFEATUREEXTRACTIONPROCESS_HH*/
