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
 * PndSttHitWriteoutBuffer.cxx
 *
 *  Created on: May 10, 2011
 *      Author: stockman
 */

#include "PndSttHitWriteoutBuffer.h"

ClassImp(PndSttHitWriteoutBuffer);

#include "PndSttHit.h"

PndSttHitWriteoutBuffer::PndSttHitWriteoutBuffer() : FairWriteoutBuffer() {}

PndSttHitWriteoutBuffer::PndSttHitWriteoutBuffer(TString branchName, TString folderName, Bool_t persistance) : FairWriteoutBuffer(branchName, "PndSttHit", folderName, persistance)
{
}

PndSttHitWriteoutBuffer::~PndSttHitWriteoutBuffer()
{
  // TODO Auto-generated destructor stub
}

void PndSttHitWriteoutBuffer::AddNewDataToTClonesArray(FairTimeStamp *data)
{
  FairRootManager *ioman = FairRootManager::Instance();
  TClonesArray *myArray = ioman->GetTClonesArray(fBranchName);
  if (fVerbose > 1)
    std::cout << "Data Inserted: " << *(PndSttHit *)(data) << std::endl;
  new ((*myArray)[myArray->GetEntries()]) PndSttHit(*(PndSttHit *)(data));
}

double PndSttHitWriteoutBuffer::FindTimeForData(FairTimeStamp *data)
{
  std::map<PndSttHit, double>::iterator it;
  PndSttHit myData = *(PndSttHit *)data;
  it = fData_map.find(myData);
  if (it == fData_map.end())
    return -1;
  else
    return it->second;
}
void PndSttHitWriteoutBuffer::FillDataMap(FairTimeStamp *data, double activeTime)
{
  PndSttHit myData = *(PndSttHit *)data;
  fData_map[myData] = activeTime;
}
void PndSttHitWriteoutBuffer::EraseDataFromDataMap(FairTimeStamp *data)
{
  PndSttHit myData = *(PndSttHit *)data;
  if (fData_map.find(myData) != fData_map.end())
    fData_map.erase(fData_map.find(myData));
}
