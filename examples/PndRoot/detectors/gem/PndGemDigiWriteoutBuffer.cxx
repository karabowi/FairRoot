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
 * PndGemDigiWriteoutBuffer.cxx
 *
 *  Created on: May 10, 2011
 *      Author: stockman
 */

#include "PndGemDigiWriteoutBuffer.h"

ClassImp(PndGemDigiWriteoutBuffer);

#include "PndGemDigi.h"

PndGemDigiWriteoutBuffer::PndGemDigiWriteoutBuffer() : FairWriteoutBuffer()
{

  // TODO Auto-generated constructor stub
}

PndGemDigiWriteoutBuffer::PndGemDigiWriteoutBuffer(TString branchName, TString folderName, Bool_t persistance)
  : FairWriteoutBuffer(branchName, "PndGemDigi", folderName, persistance)
{
}

PndGemDigiWriteoutBuffer::~PndGemDigiWriteoutBuffer()
{
  // TODO Auto-generated destructor stub
}

std::vector<std::pair<double, FairTimeStamp *>> PndGemDigiWriteoutBuffer::Modify(std::pair<double, FairTimeStamp *> oldData, std::pair<double, FairTimeStamp *> newData)
{
  std::vector<std::pair<double, FairTimeStamp *>> result;
  std::pair<double, FairTimeStamp *> singleResult;
  if (newData.first > 0)
    singleResult.first = oldData.first;// + newData.first;
  singleResult.second = oldData.second;
  ((PndGemDigi *)singleResult.second)->AddCharge(((PndGemDigi *)newData.second)->GetCharge());
  if (fVerbose > 0) {
    std::cout << "Modify hit" << std::endl;
    std::cout << "OldData: " << oldData.first << " : " << oldData.second << " NewData: " << newData.first << " : " << newData.second << std::endl;
    std::cout << "Resulting Data: " << singleResult.first << " : " << singleResult.second << std::endl;
  }

  result.push_back(singleResult);
  return result;
}

void PndGemDigiWriteoutBuffer::AddNewDataToTClonesArray(FairTimeStamp *data)
{
  FairRootManager *ioman = FairRootManager::Instance();
  TClonesArray *myArray = ioman->GetTClonesArray(fBranchName);
  if (fVerbose > 1)
    std::cout << "Data Inserted: " << *(PndGemDigi *)(data) << std::endl;
  new ((*myArray)[myArray->GetEntries()]) PndGemDigi(*(PndGemDigi *)(data));
}

double PndGemDigiWriteoutBuffer::FindTimeForData(FairTimeStamp *data)
{
  std::map<PndGemDigi, double>::iterator it;
  PndGemDigi myData = *(PndGemDigi *)data;
  it = fData_map.find(myData);
  if (it == fData_map.end())
    return -1;
  else
    return it->second;
}
void PndGemDigiWriteoutBuffer::FillDataMap(FairTimeStamp *data, double activeTime)
{
  PndGemDigi myData = *(PndGemDigi *)data;
  fData_map[myData] = activeTime;
}
void PndGemDigiWriteoutBuffer::EraseDataFromDataMap(FairTimeStamp *data)
{
  PndGemDigi myData = *(PndGemDigi *)data;
  if (fData_map.find(myData) != fData_map.end())
    fData_map.erase(fData_map.find(myData));
}
