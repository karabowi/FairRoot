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

/*
 * BSEmcWaveformBuffer.cxx
 */

#include "BSEmcWaveformBuffer.h"

#include <math.h>
#include <vector>

#include "TClonesArray.h"

#include "FairLink.h"
#include "FairRootManager.h"
#include "FairTimeStamp.h"
#include "fairlogger/Logger.h"

#include "BSEmcAbsWaveformSimulator.h"
#include "BSEmcWaveform.h"
#include "BSEmcWaveformData.h"

ClassImp(BSEmcWaveformBuffer);

BSEmcWaveformBuffer::BSEmcWaveformBuffer() : FairWriteoutBuffer(), fStoreWaveformData(kFALSE), fWfDataArray(nullptr) {}

BSEmcWaveformBuffer::BSEmcWaveformBuffer(TString t_branchName, TString t_className, TString t_folderName, Bool_t t_persistance)
  : FairWriteoutBuffer(t_branchName, t_className, t_folderName, t_persistance), fStoreWaveformData(kFALSE), fWfDataArray(nullptr)
{
}

BSEmcWaveformBuffer::~BSEmcWaveformBuffer() {}

void BSEmcWaveformBuffer::FillNewData(BSEmcWaveformData *t_wfData)
{

  FairRootManager *ioman = FairRootManager::Instance();

  // calculate start and active time for timebased simulation framework
  // Only times greater or equal than the current EventTime are accepted by the framework, but the absolute time of the first sample might been set to lower times. To allow this,
  // the timebased simulation time parameters are increased as needed.
  Double_t startTime = NAN, activeTime = NAN;
  t_wfData->GetWaveformSimulator()->GetAbsoluteTimeInterval(t_wfData, startTime, activeTime);
  t_wfData->SetTimeStamp(startTime);
  t_wfData->SetTimeOfLastSample(activeTime);

  LOG(DEBUG3) << "BSEmcWaveformBuffer::FillNewData startTime: " << startTime << " TimeBeforeFirstDeposit " << t_wfData->GetWaveformSimulator()->GetTimeBeforeFirstDeposit(t_wfData);

  startTime = startTime < ioman->GetEventTime() ? ioman->GetEventTime() : startTime; // shifting startTime towards greater times
  activeTime += t_wfData->GetWaveformSimulator()->GetTimeBeforeFirstDeposit(
    t_wfData); // maximal shift in previous step is wfSimulator->GetTimeBeforeFirstDeposit(), avoid overlapping of generated waves in absolute time domain

  FairWriteoutBuffer::FillNewData(t_wfData, startTime, activeTime);
}

void BSEmcWaveformBuffer::StoreWaveformData(TString t_branchName, TString t_folderName, Bool_t t_persistance)
{
  FairRootManager *ioman = FairRootManager::Instance();
  fStoreWaveformData = kTRUE;
  fWfDataBranchName = t_branchName;
  LOG(debug) << "BSEmcWaveformBuffer::StoreWaveformData() into " << t_branchName;
  ioman->Register(t_branchName, "BSEmcWaveformData", t_folderName, t_persistance);
  fWfDataArray = ioman->GetTClonesArray(t_branchName);
}

void BSEmcWaveformBuffer::AddNewDataToTClonesArray(FairTimeStamp *t_data)
{

  FairRootManager *ioman = FairRootManager::Instance();
  TClonesArray *myArray = ioman->GetTClonesArray(fBranchName);
  LOG_IF(error, myArray == nullptr) << "Fetching of " << fBranchName << " failed.";
  BSEmcWaveformData *wfData = dynamic_cast<BSEmcWaveformData *>(t_data);
  if (fStoreWaveformData) {
    wfData->SetEntryNr(FairLink(-1, ioman->GetEntryNr(), fWfDataBranchName, fWfDataArray->GetEntries()));
  }
  BSEmcWaveform *wave = wfData->GetWaveformSimulator()->Simulate(wfData, myArray);

  if (wave != nullptr) {
    LOG_IF(debug, fVerbose > 1) << "Data Inserted: " << *wave;
  } else {
    LOG(error) << "BSEmcWaveformBuffer::AddNewDataToTClonesArray - wave == nullptr";
  }

  if (fStoreWaveformData) {
    new ((*fWfDataArray)[fWfDataArray->GetEntries()]) BSEmcWaveformData(*wfData);
  }
}

std::vector<std::pair<Double_t, FairTimeStamp *>> BSEmcWaveformBuffer::Modify(std::pair<Double_t, FairTimeStamp *> t_oldData, std::pair<Double_t, FairTimeStamp *> t_newData)
{

  BSEmcWaveformData *oldWfData = dynamic_cast<BSEmcWaveformData *>(t_oldData.second);
  BSEmcWaveformData *newWfData = dynamic_cast<BSEmcWaveformData *>(t_newData.second);

  (*oldWfData) += (*newWfData);
  delete newWfData;

  Double_t startTime = NAN, activeTime = NAN;
  oldWfData->GetWaveformSimulator()->GetAbsoluteTimeInterval(oldWfData, startTime, activeTime);
  oldWfData->SetTimeStamp(startTime);
  oldWfData->SetTimeOfLastSample(activeTime);

  activeTime += oldWfData->GetWaveformSimulator()->GetTimeBeforeFirstDeposit(oldWfData);

  return std::vector<std::pair<Double_t, FairTimeStamp *>>(1, std::pair<Double_t, FairTimeStamp *>(activeTime, oldWfData));
}

Double_t BSEmcWaveformBuffer::FindTimeForData(FairTimeStamp *t_data)
{
  std::map<BSEmcWaveformData, Double_t>::iterator it;
  BSEmcWaveformData myData = *(dynamic_cast<BSEmcWaveformData *>(t_data));
  it = fData_map.find(myData);
  if (it == fData_map.end()) {
    return -1;
  } else {
    return it->second;
  }
}

void BSEmcWaveformBuffer::FillDataMap(FairTimeStamp *t_data, Double_t t_activeTime)
{
  BSEmcWaveformData myData = *(dynamic_cast<BSEmcWaveformData *>(t_data));
  fData_map[myData] = t_activeTime;
}

void BSEmcWaveformBuffer::DeleteOldData()
{
  FairWriteoutBuffer::DeleteOldData();
  if (fStoreWaveformData) {
    fWfDataArray->Delete();
  }
}

void BSEmcWaveformBuffer::EraseDataFromDataMap(FairTimeStamp *t_data)
{
  BSEmcWaveformData myData = *(dynamic_cast<BSEmcWaveformData *>(t_data));
  if (fData_map.find(myData) != fData_map.end()) {
    fData_map.erase(fData_map.find(myData));
  }
}
