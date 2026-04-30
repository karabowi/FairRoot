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

#ifndef BSEMCFWENDCAPIDEALDIGITIZER_HH
#define BSEMCFWENDCAPIDEALDIGITIZER_HH
#include <algorithm>
#include <memory>
#include <string>
#include <vector>

#include "Rtypes.h"
#include "RtypesCore.h"

#include "PndContainerI.h"
#include "PndParameterRegister.h"
#include "PndProcess.h"

#include "BSEmcIdealDigitizationProcess.h"

class TClonesArray;
class BSEmcDigiPar;
class BSEmcMCDeposit;
class BSEmcDigi;
class PndParameterRegister;
class TBuffer;
class TClass;
class TMemberInspector;
template <class T>
class PndContainerI;
template <class T>
class PndMutableContainerI;

/**
 * @class BSEmcFwEndcapIdealDigitizerProcess
 * @brief IdealDigitizationProcess (non waveform) for FwEndcap requires special treatment since the FwEndcap has  VPTTs instead of APDs for some crystals
 * @details
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-01
 * @ingroup FwEndcap
 */
class BSEmcFwEndcapIdealDigitizerProcess : public PndProcess {
 public:
  BSEmcFwEndcapIdealDigitizerProcess();
  virtual ~BSEmcFwEndcapIdealDigitizerProcess();
  virtual void RequestDataContainer(PndContainerRegister *t_register) /*override*/;
  virtual void GetDataContainer(PndContainerRegister *t_register) /*override*/;
  virtual void SetDetectorName(const std::string &t_detectorName) /*override*/;
  virtual void SetupParameters(const PndParameterRegister *t_parameterRegister) /*override*/;

  virtual void Process() /*override*/;

  void SetVpttIds(const std::vector<Int_t> &t_vpttIds)
  {
    fVpttIds = t_vpttIds;
    std::sort(fVpttIds.begin(), fVpttIds.end());
  }

  void SetMCDepositBranchName(const TString &t_branchname)
  {
    fMCDepositBranchname = t_branchname;
    fAPDDigitizer->SetMCDepositBranchName(t_branchname);
    fVPTDigitizer->SetMCDepositBranchName(t_branchname);
  }

  void SetDigiBranchName(const TString &t_branchname)
  {
    fDigiBranchname = t_branchname;
    fAPDDigitizer->SetDigiBranchName(t_branchname);
    fVPTDigitizer->SetDigiBranchName(t_branchname);
  }

 protected:
  virtual Bool_t IsVpttDeposit(const BSEmcMCDeposit *t_deposit) const;

 private:
  TString fMCDepositBranchname{""};
  TString fDigiBranchname{""};

  PndContainerI<BSEmcMCDeposit> *fMCDepositArray{nullptr};
  PndMutableContainerI<BSEmcDigi> *fDigiArray{nullptr};

  std::unique_ptr<BSEmcIdealDigitizationProcess> fAPDDigitizer{new BSEmcIdealDigitizationProcess()};
  std::unique_ptr<BSEmcIdealDigitizationProcess> fVPTDigitizer{new BSEmcIdealDigitizationProcess()};
  std::vector<Int_t> fVpttIds{}; // searching a vector for each deposit is bad. Need a different approach.

  ClassDef(BSEmcFwEndcapIdealDigitizerProcess, 1);
};

#endif /*BSEMCFWENDCAPIDEALDIGITIZER_HH*/
