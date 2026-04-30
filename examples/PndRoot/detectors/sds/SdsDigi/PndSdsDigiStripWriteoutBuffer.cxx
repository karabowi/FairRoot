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
 * PndSdsDigiStripWriteoutBuffer.cxx
 *
 *  Created on: May 10, 2011
 *      Author: stockman
 */

#include "PndSdsDigiStripWriteoutBuffer.h"

ClassImp(PndSdsDigiStripWriteoutBuffer);

#include "PndSdsDigiStrip.h"
#include "PndSdsDigiStrip.h"

PndSdsDigiStripWriteoutBuffer::PndSdsDigiStripWriteoutBuffer() : FairWriteoutBuffer(), fData_map()
{
  // TODO Auto-generated constructor stub
}

PndSdsDigiStripWriteoutBuffer::PndSdsDigiStripWriteoutBuffer(TString branchName, TString folderName, Bool_t persistance)
  : FairWriteoutBuffer(branchName, "PndSdsDigiStrip", folderName, persistance), fData_map()

{
}

PndSdsDigiStripWriteoutBuffer::~PndSdsDigiStripWriteoutBuffer()
{
  // TODO Auto-generated destructor stub
}

std::vector<std::pair<double, FairTimeStamp *>> PndSdsDigiStripWriteoutBuffer::Modify(std::pair<double, FairTimeStamp *> oldData, std::pair<double, FairTimeStamp *> newData)
{
  std::vector<std::pair<double, FairTimeStamp *>> result;
  std::pair<double, FairTimeStamp *> singleResult;
  if (newData.first > 0)
    singleResult.first = oldData.first + newData.first;
  singleResult.second = oldData.second;
  ((PndSdsDigiStrip *)singleResult.second)->AddCharge(((PndSdsDigiStrip *)newData.second)->GetCharge());
  if (fVerbose > 0) {
    std::cout << "Modify hit" << std::endl;

    std::cout << "OldData: " << oldData.first << " : " << oldData.second << " NewData: " << newData.first << " : " << newData.second << std::endl;
    std::cout << "Resulting Data: " << singleResult.first << " : " << singleResult.second << std::endl;
  }
  result.push_back(singleResult);
  return result;
}

void PndSdsDigiStripWriteoutBuffer::AddNewDataToTClonesArray(FairTimeStamp *data)
{
  FairRootManager *ioman = FairRootManager::Instance();
  TClonesArray *myArray = ioman->GetTClonesArray(fBranchName);
  if (fVerbose > 1)
    std::cout << "Data Inserted: " << *(PndSdsDigiStrip *)(data) << std::endl;
  new ((*myArray)[myArray->GetEntries()]) PndSdsDigiStrip(*(PndSdsDigiStrip *)(data));
}

double PndSdsDigiStripWriteoutBuffer::FindTimeForData(FairTimeStamp *data)
{
  std::map<PndSdsDigiStrip, double>::iterator it;
  PndSdsDigiStrip myData = *(PndSdsDigiStrip *)data;
  it = fData_map.find(myData);
  PndSdsDigiStrip dataInMap;
  if (it == fData_map.end())
    return -1;
  else {
    if (fVerbose > 1) {
      std::cout << "Search: " << myData << " Found: ";
      dataInMap = it->first;
      dataInMap.Print();
      std::cout << " Time: " << it->second << std::endl;
    }
    return it->second;
  }
}
void PndSdsDigiStripWriteoutBuffer::FillDataMap(FairTimeStamp *data, double activeTime)
{
  PndSdsDigiStrip myData = *(PndSdsDigiStrip *)data;
  fData_map[myData] = activeTime;
}
void PndSdsDigiStripWriteoutBuffer::EraseDataFromDataMap(FairTimeStamp *data)
{
  PndSdsDigiStrip myData = *(PndSdsDigiStrip *)data;
  if (fData_map.find(myData) != fData_map.end())
    fData_map.erase(fData_map.find(myData));
}
