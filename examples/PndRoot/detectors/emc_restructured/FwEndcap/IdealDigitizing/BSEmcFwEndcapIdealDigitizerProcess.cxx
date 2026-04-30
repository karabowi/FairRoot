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

#include "BSEmcFwEndcapIdealDigitizerProcess.h"

#include "PndConstContainerI.h"
#include "PndMutableContainerI.h"

#include "BSEmcDigi.h"

#include "BSEmcIdealDigitizationProcess.h"
#include "BSEmcMCDeposit.h"
#include "PndParameterRegister.h"

BSEmcFwEndcapIdealDigitizerProcess::BSEmcFwEndcapIdealDigitizerProcess() : PndProcess{"BSEmcFwEndcapIdealDigitizerProcess"} {}

BSEmcFwEndcapIdealDigitizerProcess::~BSEmcFwEndcapIdealDigitizerProcess() {}

void BSEmcFwEndcapIdealDigitizerProcess::SetDetectorName(const std::string &t_detectorname)
{
  fDetectorName = t_detectorname;
  fAPDDigitizer->SetReadoutType("APD");
  fAPDDigitizer->SetDetectorName("FwEndcap");
  fVPTDigitizer->SetReadoutType("VPT");
  fVPTDigitizer->SetDetectorName("FwEndcap");

  for (const std::string &param : fVPTDigitizer->GetListOfRequiredParameters()) {
    fParameterList.push_back(param);
  }
  for (const std::string &param : fAPDDigitizer->GetListOfRequiredParameters()) {
    fParameterList.push_back(param);
  }
}

void BSEmcFwEndcapIdealDigitizerProcess::SetupParameters(const PndParameterRegister *t_paramRegister)
{
  fVPTDigitizer->SetupParameters(t_paramRegister);
  fAPDDigitizer->SetupParameters(t_paramRegister);
}

// -------------------------------------------------------------------------
void BSEmcFwEndcapIdealDigitizerProcess::RequestDataContainer(PndContainerRegister *t_register)
{
  // Lets use the defaults set by the task
  t_register->Request({fMCDepositBranchname, "BSEmcMCDeposit", kFALSE, kFALSE});
  t_register->Request({fDigiBranchname, "BSEmcDigi", kTRUE, kTRUE});
  fAPDDigitizer->RequestDataContainer(t_register);
  fVPTDigitizer->RequestDataContainer(t_register);
}

// -------------------------------------------------------------------------
void BSEmcFwEndcapIdealDigitizerProcess::GetDataContainer(PndContainerRegister *t_register)
{
  fMCDepositArray = t_register->GetInput<BSEmcMCDeposit>(fMCDepositBranchname);
  fDigiArray = t_register->GetOutput<BSEmcDigi>(fDigiBranchname);

  fAPDDigitizer->GetDataContainer(t_register);
  fVPTDigitizer->GetDataContainer(t_register);
}

void BSEmcFwEndcapIdealDigitizerProcess::Process()
{
  fDigiArray->Reset();
  Int_t nDeposits = fMCDepositArray->GetSize();

  for (Int_t depositIndex = 0; depositIndex < nDeposits; ++depositIndex) {
    const BSEmcMCDeposit *deposit = fMCDepositArray->GetConstElementPtr(depositIndex);
    BSEmcDigi *digi{nullptr};
    if (IsVpttDeposit(deposit)) {
      digi = fVPTDigitizer->DigitizeDeposit(deposit, depositIndex);
    } else {
      digi = fAPDDigitizer->DigitizeDeposit(deposit, depositIndex);
    }
    if (digi != nullptr) {
      fDigiArray->CreateCopy(*digi);
      delete digi;
    }
  }
}

Bool_t BSEmcFwEndcapIdealDigitizerProcess::IsVpttDeposit(const BSEmcMCDeposit *t_deposit) const
{
  return std::binary_search(fVpttIds.begin(), fVpttIds.end(), t_deposit->GetDetectorID());
}
