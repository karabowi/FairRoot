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
 * PndSciTHitWriteoutBuffer.cxx
 *
 *  Created on: May 13, 2015
 *      Author: Steinschaden Dominik
 *  Last update: 6.2015
 */

#include "PndSciTHitWriteoutBuffer.h"

ClassImp(PndSciTHitWriteoutBuffer);

#include "PndSciTHit.h"

PndSciTHitWriteoutBuffer::PndSciTHitWriteoutBuffer() : FairWriteoutBuffer()
{
  // TODO Auto-generated constructor stub
}

PndSciTHitWriteoutBuffer::PndSciTHitWriteoutBuffer(TString branchName, TString folderName, Bool_t persistance)
  : FairWriteoutBuffer(branchName, "PndSciTHit", folderName, persistance)
{
}

PndSciTHitWriteoutBuffer::~PndSciTHitWriteoutBuffer()
{
  // TODO Auto-generated destructor stub
}

void PndSciTHitWriteoutBuffer::AddNewDataToTClonesArray(FairTimeStamp *data)
{
  FairRootManager *ioman = FairRootManager::Instance();
  TClonesArray *myArray = ioman->GetTClonesArray(fBranchName);
  if (fVerbose > 1)
    std::cout << "Data Inserted: " << *(PndSciTHit *)(data) << std::endl;
  new ((*myArray)[myArray->GetEntries()]) PndSciTHit(*(PndSciTHit *)(data));
}

double PndSciTHitWriteoutBuffer::FindTimeForData(FairTimeStamp *data)
{
  std::map<PndSciTHit, double>::iterator it;
  PndSciTHit myData = *(PndSciTHit *)data;
  it = fData_map.find(myData);
  if (it == fData_map.end())
    return -1;
  else {
    if (fVerbose > 1)
      std::cout << "Search: " << myData << " Found: " << (PndSciTHit)it->first << " Time: " << it->second << std::endl;
    return it->second;
  }
}

void PndSciTHitWriteoutBuffer::FillDataMap(FairTimeStamp *data, double activeTime)
{
  PndSciTHit myData = *(PndSciTHit *)data;
  fData_map[myData] = activeTime;
}

void PndSciTHitWriteoutBuffer::EraseDataFromDataMap(FairTimeStamp *data)
{
  PndSciTHit myData = *(PndSciTHit *)data;
  if (fData_map.find(myData) != fData_map.end())
    fData_map.erase(fData_map.find(myData));
}

std::vector<std::pair<double, FairTimeStamp *>> PndSciTHitWriteoutBuffer::Modify(std::pair<double, FairTimeStamp *> oldData, std::pair<double, FairTimeStamp *> newData)
{
  std::vector<std::pair<double, FairTimeStamp *>> result;
  std::pair<double, FairTimeStamp *> singleResult;

  singleResult.first = oldData.first;
  singleResult.second = oldData.second;
  ((PndSciTHit *)singleResult.second)->AddCharge(((PndSciTHit *)newData.second)->GetCharge());

  if (fVerbose > 0) {
    std::cout << "Modify hit" << std::endl;
    std::cout << "OldData: " << oldData.first << " : " << oldData.second << " NewData: " << newData.first << " : " << newData.second << std::endl;
    std::cout << "Resulting Data: " << singleResult.first << " : " << singleResult.second << std::endl;
  }

  result.push_back(singleResult);
  return result;
}
