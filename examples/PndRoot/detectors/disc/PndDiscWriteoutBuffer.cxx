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

//-------------------------------------------------------------------------
// Author:      Oliver Merle (Oliver.Merle@exp2.physik.uni-giessen.de)
// Changes:     Mustafa Schmidt (Mustafa.A.Schmidt@physik.uni-giessen.de)
// Date:        30.11.2015
// Description: Customization of FairRoot Write out buffer
//              for time-based simulations
//-------------------------------------------------------------------------

#include "PndDiscWriteoutBuffer.h"

#include <TROOT.h>

#include <stdexcept>

ClassImp(PndDiscWriteoutBuffer);

PndDiscWriteoutBuffer::PndDiscWriteoutBuffer() : FairWriteoutBuffer()
{
#ifdef RRTTI
  buffer = (TBuffer *)gROOT->ProcessLine("new TBufferFile(TBuffer::kWrite,10000);");
  if (!buffer)
    throw std::runtime_error(std::string("WriteoutBuffer::AddNewDataToTClonesArray: cannot allocate buffer."));
#endif
}

PndDiscWriteoutBuffer::PndDiscWriteoutBuffer(TString branchName, TString folderName, Bool_t persistance)
  : FairWriteoutBuffer(branchName, "PndDiscDigitizedHit", folderName, persistance)
{
#ifdef RRTTI
  buffer = (TBuffer *)gROOT->ProcessLine("new TBufferFile(TBuffer::kWrite,10000);");
  if (!buffer)
    throw std::runtime_error(std::string("WriteoutBuffer::AddNewDataToTClonesArray: cannot allocate buffer."));
#endif
}

PndDiscWriteoutBuffer::~PndDiscWriteoutBuffer()
{
#ifdef RRTTI
  delete buffer;
#endif
}

// TODO: move this to functor
std::vector<std::pair<double, FairTimeStamp *>> PndDiscWriteoutBuffer::Modify(std::pair<double, FairTimeStamp *> oldData, std::pair<double, FairTimeStamp *> newData)
{
  std::vector<std::pair<double, FairTimeStamp *>> result;
  if (oldData.first >= newData.second->GetTimeStamp()) {
    result.push_back(oldData);
  } else {
    oldData.first = -1;
    result.push_back(oldData);
    result.push_back(newData);
  }
  return result;
}

#ifdef FWB_USE_INHERITANCE
// The interface is not needed anymore and can go back to
// FairWriteoutBuffer:
void PndDiscWriteoutBuffer::AddNewDataToTClonesArray(FairTimeStamp *data)
{
  FairRootManager *ioman = FairRootManager::Instance();
  TClonesArray *myArray = ioman->GetTClonesArray(fBranchName);

#ifndef RRTTI
  if (fVerbose > 1)
    std::cout << "Data Inserted: " << *(PndDiscDigitizedHit *)(data) << std::endl;
  new ((*myArray)[myArray->GetEntries()]) PndDiscDigitizedHit(*(PndDiscDigitizedHit *)(data));
  // data->Delete();
#else
  // Get metadata, check types
  TClass *class_meta = myArray->GetClass();
  if (data->IsA() != class_meta) { // Check that types are compatible:
    data->IsA()->Dump();
    class_meta->Dump();
    throw std::runtime_error(std::string("WriteoutBuffer::AddNewDataToTClonesArray: wrong object type."));
  }

  // create a new object in TClonesArray via default ctor:
  TObject *new_instance = myArray->New(myArray->GetEntries());

  // and copy object contents to new one using the streamer:

  // store object in created buffer
  buffer->SetWriteMode();
  buffer->ResetMap();
  buffer->SetBufferOffset(0);
  buffer->MapObject(data);
  ((TObject *)data)->Streamer(*buffer);

  // read back content to new object
  buffer->SetReadMode();
  buffer->ResetMap();
  buffer->SetBufferOffset(0);
  buffer->MapObject(new_instance);
  new_instance->Streamer(*buffer);

  // Reset some bits
  new_instance->ResetBit(kIsReferenced);
  new_instance->ResetBit(kCanDelete);

  // data is serialized and not needed anymore (buffer specific, could be moved):
  data->Delete();
#endif
}

double PndDiscWriteoutBuffer::FindTimeForData(FairTimeStamp *data)
{
#ifndef RRTTI
  std::map<PndDiscDigitizedHit, double>::iterator it;
  PndDiscDigitizedHit myData = *(PndDiscDigitizedHit *)data;
  it = fData_map.find(myData);
  if (it == fData_map.end())
    return -1.;
  else
    return it->second;
#else
  DataMap_t::iterator it = fData_map.find(data);
  if (it == fData_map.end())
    return -1.;
  else
    return it->second;
#endif
}

void PndDiscWriteoutBuffer::FillDataMap(FairTimeStamp *data, double activeTime)
{
#ifndef RRTTI
  PndDiscDigitizedHit myData = *(PndDiscDigitizedHit *)data;
  fData_map[myData] = activeTime;
#else
  fData_map[data] = activeTime;
#endif
}

void PndDiscWriteoutBuffer::EraseDataFromDataMap(FairTimeStamp *data)
{
#ifndef RRTTI
  PndDiscDigitizedHit myData = *(PndDiscDigitizedHit *)data;
  if (fData_map.find(myData) != fData_map.end())
    fData_map.erase(fData_map.find(myData));
#else
  DataMap_t::iterator it = fData_map.find(data);
  if (it != fData_map.end())
    fData_map.erase(it);
#endif
}
#endif
