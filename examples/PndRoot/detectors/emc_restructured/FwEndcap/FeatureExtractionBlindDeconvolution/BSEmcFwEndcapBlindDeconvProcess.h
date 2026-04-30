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

#ifndef BSEMCFWENDCAPBLINDDECOFEATUREEXTRACTIONPROCESS_HH
#define BSEMCFWENDCAPBLINDDECOFEATUREEXTRACTIONPROCESS_HH

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

#include "Rtypes.h"
#include "RtypesCore.h"
#include "TString.h"

#include "PndConstContainer.h"
#include "PndMutableContainerI.h"
#include "PndProcess.h"

#include "BSEmcDigi.h"
#include "BSEmcMultiWaveform.h"

#include "BSEmcPSAFPGABlindDeconv.h"
#include "BSEmcPSAFPGABaselineFinder.h"
#include "BSEmcPSAFPGAPeakfinder.h"
#include "BSEmcPSAFPGAPileUpDetection.h"
#include "BSEmcPSAFPGAPeakfinderTime.h"

class PndParameterRegister;
class BSEmcAbsPSA;
class BSEmcWaveform;
class TBuffer;
class TClass;
class TMemberInspector;
template <class T>
class PndMutableContainerI;

/**
 * @brief Blind deconvolution feature extraction approach for the FwEndcap.
 *
 * @author Celina Frenkel <frenkel@hiskp.uni-bonn.de>
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>
 * @ingroup FwEndcap
 */
class BSEmcFwEndcapBlindDeconvProcess : public PndProcess {
 public:
  BSEmcFwEndcapBlindDeconvProcess();
  virtual ~BSEmcFwEndcapBlindDeconvProcess();

  virtual void SetDetectorName(const std::string &t_detectorName) /*override*/;
  virtual void RequestDataContainer(PndContainerRegister *t_register) /*override*/;
  virtual void GetDataContainer(PndContainerRegister *t_register) /*override*/;
  virtual void SetupParameters(const PndParameterRegister *t_parameterRegister) /*override*/;
  virtual void Process() /*override*/;

  std::vector<BSEmcPSAFPGAPeakFeatures_t> ExtractFeatures(const std::vector<Double_t> &t_samples, const Int_t t_detId) const;
  std::vector<BSEmcDigi *> ExtractDigis(const std::vector<const BSEmcMultiWaveform *> &t_waveforms) const;
  void SetMCDepositBranchName(const TString &t_branchname) { fMCDepositBranchName = t_branchname; }
  void SetDigiBranchName(const TString &t_branchname) { fDigiBranchName = t_branchname; }
  void SetWaveformBranchName(const TString &t_branchname) { fWaveformBranchName = t_branchname; }

  Bool_t IsVptt(const Int_t t_detId) const;
  void SetVpttIds(const std::vector<Int_t> &t_vpttIds)
  {
    fVpttIds = t_vpttIds;
    std::sort(fVpttIds.begin(), fVpttIds.end());
  }

 protected:
  TString fMCDepositBranchName{""};
  TString fDigiBranchName{""};
  TString fWaveformBranchName{""};

  PndMutableContainerI<BSEmcDigi> *fDigiArray{nullptr};
  PndContainerI<BSEmcMultiWaveform> *fWaveformArray{nullptr};

  Double_t fEnergyDigiThreshold{0};

  BSEmcPSAFPGABlindDeconv fBlindDeconvoluterAPD{};
  BSEmcPSAFPGABaselineFinder fBaselineFinderAPD{};
  BSEmcPSAFPGAPeakfinder fPeakFinderAPD{};
  BSEmcPSAFPGAPeakfinderTime fPeakTimeFinderAPD{};
  BSEmcPSAFPGAPileUpDetection fPileUpDetectorAPD{};

  BSEmcPSAFPGABlindDeconv fBlindDeconvoluterVPTT{};
  BSEmcPSAFPGABaselineFinder fBaselineFinderVPTT{};
  BSEmcPSAFPGAPeakfinder fPeakFinderVPTT{};
  BSEmcPSAFPGAPeakfinderTime fPeakTimeFinderVPTT{};
  BSEmcPSAFPGAPileUpDetection fPileUpDetectorVPTT{};

  std::vector<Int_t> fVpttIds{};

  ClassDef(BSEmcFwEndcapBlindDeconvProcess, 1);
};

#endif /*BSEMCFWENDCAPBLINDDECOFEATUREEXTRACTIONPROCESS_HH*/
