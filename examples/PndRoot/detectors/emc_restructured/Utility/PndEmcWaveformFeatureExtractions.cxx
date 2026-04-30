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

#include "PndEmcWaveformFeatureExtractions.h"

#include "PndPersistencyTask.h"
#include "PndSensorNameIdMap.h"

#include "BSEmcApplyCalibrationProcess.h"
#include "BSEmcBwEndcapDigi.h"
#include "BSEmcBwEndcapTimebasedWaveforms.h"
#include "BSEmcCounter.h"
#include "BSEmcDataBranchNames.h"
#include "BSEmcDigiDCSetterProcess.h"

#include "BSEmcDigitizerTask.h"
#include "BSEmcFwEndcapOnlineFeatureExtraction.h"
#include "BSEmcFwEndcapTimebasedWaveforms.h"
#include "BSEmcFwEndcapVpttIds.h"
#include "BSEmcIdealDigitizationProcess.h"
#include "BSEmcParameterLoadingProcess.h"

#include "BSEmcShashlykDigi.h"
#include "BSEmcShashlykTimebasedWaveforms.h"
#include "BSEmcShashlikIdealDigitizationProcess.h"

BSEmcBarrelWaveformFeatureExtraction::BSEmcBarrelWaveformFeatureExtraction() : PndPersistencyTask("BSEmcBarrelWaveformFeatureExtraction")
{
  BSEmcBwEndcapTimebasedWaveforms *waveformgeneration = new BSEmcBwEndcapTimebasedWaveforms("Barrel", kTRUE);
  waveformgeneration->RunTimebased(kFALSE);
  waveformgeneration->StoreDataClass(kTRUE);
  this->Add(waveformgeneration);
  this->Add(new BSEmcBwEndcapDigi("Barrel"));
  BSEmcDigitizerTask *calibration = new BSEmcDigitizerTask("Barrel");
  calibration->SetDigiBranchName(BSEmcDataBranchNames::fgDigiBranchName + "Barrel");
  calibration->AddProcess(new BSEmcApplyCalibrationProcess());
  calibration->AddProcess(new BSEmcDigiDCSetterProcess());
  this->Add(calibration);
}

BSEmcBwEndcapWaveformFeatureExtraction::BSEmcBwEndcapWaveformFeatureExtraction() : PndPersistencyTask("BSEmcBwEndcapWaveformFeatureExtraction")
{
  BSEmcBwEndcapTimebasedWaveforms *waveformgeneration = new BSEmcBwEndcapTimebasedWaveforms("BwEndcap", kTRUE);
  waveformgeneration->RunTimebased(kFALSE);
  waveformgeneration->StoreDataClass(kTRUE);
  this->Add(waveformgeneration);
  this->Add(new BSEmcBwEndcapDigi("BwEndcap"));

  BSEmcDigitizerTask *calibration = new BSEmcDigitizerTask("BwEndcap");
  calibration->SetDigiBranchName(BSEmcDataBranchNames::fgDigiBranchName + "BwEndcap");
  calibration->AddProcess(new BSEmcApplyCalibrationProcess());
  calibration->AddProcess(new BSEmcDigiDCSetterProcess());
  this->Add(calibration);
}

BSEmcFwEndcapWaveformFeatureExtraction::BSEmcFwEndcapWaveformFeatureExtraction() : PndPersistencyTask("BSEmcFwEndcapWaveformFeatureExtraction")
{
  BSEmcFwEndcapTimebasedWaveforms *fwendcapwaveformgeneration = new BSEmcFwEndcapTimebasedWaveforms(kTRUE);
  fwendcapwaveformgeneration->RunTimebased(kFALSE);
  fwendcapwaveformgeneration->StoreDataClass(kTRUE);
  this->Add(fwendcapwaveformgeneration);
  BSEmcDigitizerTask *fwendcapcalibration = new BSEmcDigitizerTask("FwEndcap");
  fwendcapcalibration->SetDigiBranchName(BSEmcDataBranchNames::fgDigiBranchName + "FwEndcap");
  fwendcapcalibration->AddProcess(new BSEmcFwEndcapOnlineFeatureExtraction());
  fwendcapcalibration->AddProcess(new BSEmcApplyCalibrationProcess());
  fwendcapcalibration->AddProcess(new BSEmcDigiDCSetterProcess());
  this->Add(fwendcapcalibration);
}

BSEmcShashlikWaveformFeatureExtraction::BSEmcShashlikWaveformFeatureExtraction() : PndPersistencyTask("BSEmcShashlikWaveformFeatureExtraction")
{
  BSEmcShashlykTimebasedWaveforms *waveformgeneration = new BSEmcShashlykTimebasedWaveforms("Shashlik", kTRUE);
  waveformgeneration->RunTimebased(kFALSE);
  waveformgeneration->StoreDataClass(kTRUE);
  this->Add(waveformgeneration);
  this->Add(new BSEmcShashlykDigi("Shashlik"));

  BSEmcDigitizerTask *calibration = new BSEmcDigitizerTask("Shashlik");
  calibration->SetDigiBranchName(BSEmcDataBranchNames::fgDigiBranchName + "Shashlik");
  calibration->AddProcess(new BSEmcApplyCalibrationProcess());
  calibration->AddProcess(new BSEmcDigiDCSetterProcess());
  this->Add(calibration);
}

BSEmcWaveformFeatureExtraction::BSEmcWaveformFeatureExtraction() : PndPersistencyTask("BSEmcWaveformFeatureExtraction")
{
  TString vmcdir = gSystem->Getenv("VMCWORKDIR");
  TString qadir = vmcdir + "/macro/params/";
  PndProcessTask *general = new BSEmcDigitizerTask("General");
  general->AddProcess(new BSEmcParameterLoadingProcess(TString{qadir + "EmcCrystalsNeighbouringRelations.txt"}.Data()));
  general->AddProcess(new BSEmcCounter());
  this->Add(general);
  this->Add(new BSEmcBarrelWaveformFeatureExtraction);
  this->Add(new BSEmcBwEndcapWaveformFeatureExtraction);
  this->Add(new BSEmcFwEndcapWaveformFeatureExtraction);
  this->Add(new BSEmcShashlikWaveformFeatureExtraction);
}
