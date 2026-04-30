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
 * PndDrcDigiWriteoutBuffer.cxx
 *
 */
// -------------------------------------------------------------------------
// -----                   PndDrcDigiWriteoutBuffer source file        -----
// -----         HARPHOOL KUMAWAT h.kumawat@gsi.de                     -----
// -----                                                               -----
// -------------------------------------------------------------------------

#include "PndDrcDigiWriteoutBuffer.h"

#include "PndDrcDigi.h"

ClassImp(PndDrcDigiWriteoutBuffer);

PndDrcDigiWriteoutBuffer::PndDrcDigiWriteoutBuffer() : FairWriteoutBuffer()
{

  // TODO Auto-generated constructor stub
}

PndDrcDigiWriteoutBuffer::PndDrcDigiWriteoutBuffer(TString branchName, TString folderName, Bool_t persistance)
  : FairWriteoutBuffer(branchName, "PndDrcDigi", folderName, persistance)
{
}

PndDrcDigiWriteoutBuffer::~PndDrcDigiWriteoutBuffer()
{
  // TODO Auto-generated destructor stub
}

void PndDrcDigiWriteoutBuffer::AddNewDataToTClonesArray(FairTimeStamp *data)
{
  FairRootManager *ioman = FairRootManager::Instance();
  TClonesArray *myArray = ioman->GetTClonesArray(fBranchName);
  if (fVerbose > 1) {
    std::cout << "Data Inserted: " << *(PndDrcDigi *)(data) << std::endl;
  }
  new ((*myArray)[myArray->GetEntries()]) PndDrcDigi(*(PndDrcDigi *)(data));
}

double PndDrcDigiWriteoutBuffer::FindTimeForData(FairTimeStamp *data)
{
  std::map<PndDrcDigi, double>::iterator it;
  PndDrcDigi myData = *(PndDrcDigi *)data;
  it = fData_map.find(myData);
  if (it == fData_map.end())
    return -1;
  else
    return it->second;
}

void PndDrcDigiWriteoutBuffer::FillDataMap(FairTimeStamp *data, double activeTime)
{
  PndDrcDigi myData = *(PndDrcDigi *)data;
  fData_map[myData] = activeTime;
}

void PndDrcDigiWriteoutBuffer::EraseDataFromDataMap(FairTimeStamp *data)
{
  PndDrcDigi myData = *(PndDrcDigi *)data;
  if (fData_map.find(myData) != fData_map.end()) {
    fData_map.erase(fData_map.find(myData));
  }
}

std::vector<std::pair<double, FairTimeStamp *>> PndDrcDigiWriteoutBuffer::Modify(std::pair<double, FairTimeStamp *> oldData, std::pair<double, FairTimeStamp *> newData)
{
  std::vector<std::pair<double, FairTimeStamp *>> result;

  Double_t oldStamp = ((PndDrcDigi *)oldData.second)->GetTimeStamp();
  Double_t newStamp = ((PndDrcDigi *)newData.second)->GetTimeStamp();
  Double_t deadTime = oldData.first - oldStamp;
  if (newStamp - oldStamp > deadTime - 0.0000001)
    result.push_back(newData);
  else
    oldData.first = newStamp + deadTime;

  result.push_back(oldData);

  // std::pair<double, FairTimeStamp*> Result1;
  // if (newData.first > 0)  Result1.first = oldData.first + (newData.first - oldData.first);
  // Result1.second = oldData.second;
  // result.push_back(oldData);

  if (fVerbose > 0) {
    std::cout << "Modify hit" << std::endl;
    std::cout << "OldData: " << oldData.first << " : " << ((PndDrcDigi *)oldData.second)->GetTimeStamp() << "(" << ((PndDrcDigi *)oldData.second)->GetSensorId() << ")"
              << " NewData: " << newData.first << " : " << newData.second << "  " << ((PndDrcDigi *)newData.second)->GetTimeStamp() << "("
              << ((PndDrcDigi *)newData.second)->GetSensorId() << ")" << std::endl;
    // std::cout << "Resulting Data: " << Result1.first << " : " << Result1.second << std::endl;
  }

  return result;
}
