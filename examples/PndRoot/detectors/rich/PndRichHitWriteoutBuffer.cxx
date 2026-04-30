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
 * PndRichHitWriteoutBuffer.cxx
 *
 *  Created on: Mar 12, 2017
 *      Author: K. Beloborodov
 */

#include "PndRichHitWriteoutBuffer.h"

ClassImp(PndRichHitWriteoutBuffer);

#include "PndRichDigi.h"

PndRichHitWriteoutBuffer::PndRichHitWriteoutBuffer() : FairWriteoutBuffer() {}

PndRichHitWriteoutBuffer::PndRichHitWriteoutBuffer(TString branchName, TString folderName, Bool_t persistance)
  : FairWriteoutBuffer(branchName, "PndRichDigi", folderName, persistance)
{
}

PndRichHitWriteoutBuffer::~PndRichHitWriteoutBuffer()
{
  // TODO Auto-generated destructor stub
}

void PndRichHitWriteoutBuffer::AddNewDataToTClonesArray(FairTimeStamp *data)
{
  FairRootManager *ioman = FairRootManager::Instance();
  TClonesArray *myArray = ioman->GetTClonesArray(fBranchName);
  if (fVerbose > 1)
    std::cout << "Data Inserted: " << *(PndRichDigi *)(data) << std::endl;
  new ((*myArray)[myArray->GetEntries()]) PndRichDigi(*(PndRichDigi *)(data));
}

double PndRichHitWriteoutBuffer::FindTimeForData(FairTimeStamp *data)
{
  std::map<PndRichDigi, double>::iterator it;
  PndRichDigi myData = *(PndRichDigi *)data;
  it = fData_map.find(myData);
  if (it == fData_map.end())
    return -1;
  else
    return it->second;
}
void PndRichHitWriteoutBuffer::FillDataMap(FairTimeStamp *data, double activeTime)
{
  PndRichDigi myData = *(PndRichDigi *)data;
  fData_map[myData] = activeTime;
}
void PndRichHitWriteoutBuffer::EraseDataFromDataMap(FairTimeStamp *data)
{
  PndRichDigi myData = *(PndRichDigi *)data;
  if (fData_map.find(myData) != fData_map.end())
    fData_map.erase(fData_map.find(myData));
}
