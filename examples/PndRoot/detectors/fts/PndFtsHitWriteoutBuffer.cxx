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
 * PndFtsHitWriteoutBuffer.cxx
 *
 *  Created on: May 10, 2011
 *      Author: stockman
 */

#include "PndFtsHitWriteoutBuffer.h"

ClassImp(PndFtsHitWriteoutBuffer);

#include "PndFtsHit.h"

PndFtsHitWriteoutBuffer::PndFtsHitWriteoutBuffer() : FairWriteoutBuffer() {}

PndFtsHitWriteoutBuffer::PndFtsHitWriteoutBuffer(TString branchName, TString folderName, Bool_t persistance) : FairWriteoutBuffer(branchName, "PndFtsHit", folderName, persistance)
{
}

PndFtsHitWriteoutBuffer::~PndFtsHitWriteoutBuffer()
{
  // TODO Auto-generated destructor stub
}

void PndFtsHitWriteoutBuffer::AddNewDataToTClonesArray(FairTimeStamp *data)
{
  FairRootManager *ioman = FairRootManager::Instance();
  TClonesArray *myArray = ioman->GetTClonesArray(fBranchName);
  if (fVerbose > 1)
    std::cout << "Data Inserted: " << *(PndFtsHit *)(data) << std::endl;
  new ((*myArray)[myArray->GetEntries()]) PndFtsHit(*(PndFtsHit *)(data));
}

double PndFtsHitWriteoutBuffer::FindTimeForData(FairTimeStamp *data)
{
  std::map<PndFtsHit, double>::iterator it;
  PndFtsHit myData = *(PndFtsHit *)data;
  it = fData_map.find(myData);
  if (it == fData_map.end())
    return -1;
  else
    return it->second;
}
void PndFtsHitWriteoutBuffer::FillDataMap(FairTimeStamp *data, double activeTime)
{
  PndFtsHit myData = *(PndFtsHit *)data;
  fData_map[myData] = activeTime;
}
void PndFtsHitWriteoutBuffer::EraseDataFromDataMap(FairTimeStamp *data)
{
  PndFtsHit myData = *(PndFtsHit *)data;
  if (fData_map.find(myData) != fData_map.end())
    fData_map.erase(fData_map.find(myData));
}
