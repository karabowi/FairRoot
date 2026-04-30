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
 * PndMdtWaveformWriteoutBuffer.cxx
 */
//#include <assert.h>
#include "PndMdtWaveformWriteoutBuffer.h"

ClassImp(PndMdtWaveformWriteoutBuffer);

PndMdtWaveformWriteoutBuffer::PndMdtWaveformWriteoutBuffer() : FairWriteoutBuffer()
{
  fVerbose = 0;
}

PndMdtWaveformWriteoutBuffer::PndMdtWaveformWriteoutBuffer(TString branchName, TString folderName, Bool_t persistance)
  : FairWriteoutBuffer(branchName, "PndMdtWaveform", folderName, persistance)
{
  fVerbose = 0;
}

PndMdtWaveformWriteoutBuffer::~PndMdtWaveformWriteoutBuffer() {}
void PndMdtWaveformWriteoutBuffer::AddNewDataToTClonesArray(FairTimeStamp *data)
{
  FairRootManager *ioman = FairRootManager::Instance();
  TClonesArray *myArray = ioman->GetTClonesArray(fBranchName);
  if (fVerbose > 1)
    std::cout << "PndMdtWaveformWriteoutBuffer:: " << fBranchName << *(PndMdtWaveform *)(data) << std::endl;
  //((*myArray)[myArray->GetEntries()])  = data; //data->Clone();
  new ((*myArray)[myArray->GetEntries()]) PndMdtWaveform(*(PndMdtWaveform *)(data));
}

double PndMdtWaveformWriteoutBuffer::FindTimeForData(FairTimeStamp *data)
{
  PndMdtWaveform *myData = ((PndMdtWaveform *)data);
  std::map<PndMdtWaveform *, double>::iterator it = fData_map.find(myData);
  if (it == fData_map.end())
    return -1;
  else
    return it->second;
}
void PndMdtWaveformWriteoutBuffer::FillDataMap(FairTimeStamp *data, double activeTime)
{
  PndMdtWaveform *myData = ((PndMdtWaveform *)data);
  fData_map.insert(std::pair<PndMdtWaveform *, double>(myData, activeTime));
}
void PndMdtWaveformWriteoutBuffer::EraseDataFromDataMap(FairTimeStamp *data)
{
  PndMdtWaveform *myData = ((PndMdtWaveform *)data);
  std::map<PndMdtWaveform *, double>::iterator it = fData_map.find(myData);
  if (it != fData_map.end()) {
    fData_map.erase(it);
  }
}
std::vector<std::pair<double, FairTimeStamp *>> PndMdtWaveformWriteoutBuffer::Modify(std::pair<double, FairTimeStamp *> oldData, std::pair<double, FairTimeStamp *> newData)
{

  std::vector<std::pair<double, FairTimeStamp *>> result;
  std::pair<double, FairTimeStamp *> singleResult;

  if (fVerbose > 2) {
    std::cout << "PndMdtWaveformWriteoutBuffer::Modify:: OldData: " << oldData.first << " : " << oldData.second << " NewData: " << newData.first << " : " << newData.second
              << std::endl;
    std::cout << "<OldData>=============================================>" << std::endl;
    oldData.second->Print();
    std::cout << "<============================================</OldData>" << std::endl;
    std::cout << "<NewData>=============================================>" << std::endl;
    newData.second->Print();
    std::cout << "<============================================</NewData>" << std::endl;
  }
  PndMdtWaveform *oldWave = (PndMdtWaveform *)oldData.second;
  PndMdtWaveform *newWave = (PndMdtWaveform *)newData.second;
  (*oldWave) += (*newWave);

  singleResult.second = oldWave;
  singleResult.first = oldWave->GetActiveTime();

  if (fVerbose > 2) {
    std::cout << "<ResultData>=============================================>" << std::endl;
    std::cout << "Resulting Data: " << singleResult.first << " : " << singleResult.second << std::endl;
    singleResult.second->Print();
    std::cout << "<============================================</ResultData>" << std::endl;
  }

  result.push_back(singleResult);
  return result;
}
