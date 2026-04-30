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


#include "PndEmcIdealDigitizers.h"

#include "PndPersistencyTask.h"
#include "PndSensorNameIdMap.h"

#include "BSEmcApplyCalibrationProcess.h"
#include "BSEmcCounter.h"
#include "BSEmcDataBranchNames.h"
#include "BSEmcDigiDCSetterProcess.h"

#include "BSEmcDigitizerTask.h"
#include "BSEmcFwEndcapIdealDigitizerProcess.h"
#include "BSEmcFwEndcapVpttIds.h"
#include "BSEmcIdealDigitizationProcess.h"
#include "BSEmcParameterLoadingProcess.h"
#include "BSEmcShashlikDetector.h"
#include "BSEmcShashlikIdealDigitizationProcess.h"

BSEmcBarrelIdealDigitizer::BSEmcBarrelIdealDigitizer() : PndPersistencyTask("BSEmcBarrelIdealDigitizer")
{
  BSEmcDigitizerTask *barreldigitizer = new BSEmcDigitizerTask("Barrel");
  barreldigitizer->SetDigiBranchName(BSEmcDataBranchNames::fgDigiBranchName + "Barrel");
  barreldigitizer->AddProcess(new BSEmcIdealDigitizationProcess());
  barreldigitizer->AddProcess(new BSEmcApplyCalibrationProcess());
  barreldigitizer->AddProcess(new BSEmcDigiDCSetterProcess());
  this->Add(barreldigitizer);
}

BSEmcBwEndcapIdealDigitizer::BSEmcBwEndcapIdealDigitizer() : PndPersistencyTask("BSEmcBwEndcapIdealDigitizer")
{
  BSEmcDigitizerTask *bwendcapdigitizer = new BSEmcDigitizerTask("BwEndcap");
  bwendcapdigitizer->SetDigiBranchName(BSEmcDataBranchNames::fgDigiBranchName + "BwEndcap");

  bwendcapdigitizer->AddProcess(new BSEmcIdealDigitizationProcess());
  bwendcapdigitizer->AddProcess(new BSEmcApplyCalibrationProcess());
  bwendcapdigitizer->AddProcess(new BSEmcDigiDCSetterProcess());

  this->Add(bwendcapdigitizer);
}

BSEmcFwEndcapIdealDigitizer::BSEmcFwEndcapIdealDigitizer() : PndPersistencyTask("BSEmcFwEndcapIdealDigitizer")
{
  BSEmcDigitizerTask *fwendcapcalibration = new BSEmcDigitizerTask("FwEndcap");
  fwendcapcalibration->SetDigiBranchName(BSEmcDataBranchNames::fgDigiBranchName + "FwEndcap");

  BSEmcFwEndcapIdealDigitizerProcess *fwendcapdigitizer = new BSEmcFwEndcapIdealDigitizerProcess();
  fwendcapdigitizer->SetVpttIds(BSEmcFwEndcapVpttIds::GetVpttIds());
  fwendcapcalibration->AddProcess(fwendcapdigitizer);
  fwendcapcalibration->AddProcess(new BSEmcApplyCalibrationProcess());
  fwendcapcalibration->AddProcess(new BSEmcDigiDCSetterProcess());

  this->Add(fwendcapcalibration);
}

BSEmcShashlikIdealDigitizer::BSEmcShashlikIdealDigitizer() : PndPersistencyTask("BSEmcShashlikIdealDigitizer")
{
  BSEmcDigitizerTask *shashlikdigitizer = new BSEmcDigitizerTask("Shashlik");
  shashlikdigitizer->SetDigiBranchName(BSEmcDataBranchNames::fgDigiBranchName + "Shashlik");

  shashlikdigitizer->AddProcess(new BSEmcShashlikIdealDigitizationProcess());
  shashlikdigitizer->AddProcess(new BSEmcApplyCalibrationProcess());
  shashlikdigitizer->AddProcess(new BSEmcDigiDCSetterProcess());

  this->Add(shashlikdigitizer);
}

BSEmcIdealDigitizer::BSEmcIdealDigitizer() : PndPersistencyTask("BSEmcIdealDigitizer")
{
  TString vmcdir = gSystem->Getenv("VMCWORKDIR");
  TString qadir = vmcdir + "/macro/params/";
  PndProcessTask *general = new BSEmcDigitizerTask("General");
  general->AddProcess(new BSEmcParameterLoadingProcess(TString{qadir + "EmcCrystalsNeighbouringRelations.txt"}.Data()));
  general->AddProcess(new BSEmcCounter());
  this->Add(general);
  this->Add(new BSEmcBarrelIdealDigitizer);
  this->Add(new BSEmcBwEndcapIdealDigitizer);
  this->Add(new BSEmcFwEndcapIdealDigitizer);
  this->Add(new BSEmcShashlikIdealDigitizer);
}
