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
 * PndEmcDigiWriteoutBuffer.cxx
 */

#include "PndEmcDigiWriteoutBuffer.h"

ClassImp(PndEmcDigiWriteoutBuffer);

#include "PndEmcDigi.h"

PndEmcDigiWriteoutBuffer::PndEmcDigiWriteoutBuffer() : FairWriteoutBuffer() {}

PndEmcDigiWriteoutBuffer::PndEmcDigiWriteoutBuffer(TString branchName, TString folderName, Bool_t persistance)
  : FairWriteoutBuffer(branchName, "PndEmcDigi", folderName, persistance)
{
}

PndEmcDigiWriteoutBuffer::~PndEmcDigiWriteoutBuffer() {}

void PndEmcDigiWriteoutBuffer::AddNewDataToTClonesArray(FairTimeStamp *data)
{
  FairRootManager *ioman = FairRootManager::Instance();
  TClonesArray *myArray = ioman->GetTClonesArray(fBranchName);
  if (fVerbose > 1)
    std::cout << "Data Inserted: " << *(PndEmcDigi *)(data) << std::endl;
  new ((*myArray)[myArray->GetEntries()]) PndEmcDigi(*(PndEmcDigi *)(data));
}

double PndEmcDigiWriteoutBuffer::FindTimeForData(FairTimeStamp *data)
{
  std::map<PndEmcDigi, double>::iterator it;
  PndEmcDigi myData = *(PndEmcDigi *)data;
  it = fData_map.find(myData);
  if (it == fData_map.end())
    return -1;
  else
    return it->second;
}
void PndEmcDigiWriteoutBuffer::FillDataMap(FairTimeStamp *data, double activeTime)
{
  PndEmcDigi myData = *(PndEmcDigi *)data;
  fData_map[myData] = activeTime;
}
void PndEmcDigiWriteoutBuffer::EraseDataFromDataMap(FairTimeStamp *data)
{
  PndEmcDigi myData = *(PndEmcDigi *)data;
  if (fData_map.find(myData) != fData_map.end())
    fData_map.erase(fData_map.find(myData));
}
