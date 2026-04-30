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

#include <iostream>
#include "PndSensorNamePar.h"
#include "FairLogger.h"
#include <string>

PndSensorNamePar::PndSensorNamePar(const char *name, const char *title, const char *context) : FairParGenericSet(name, title, context), fSensorNames()
{
  clear();
  fSensorNames = new TObjArray();
  fSensorNames->SetOwner(kTRUE);
}

PndSensorNamePar::PndSensorNamePar(const PndSensorNamePar &L) : FairParGenericSet(L)
{
  fSensorNames = new TObjArray(*(L.GetSensorNames()));
}

PndSensorNamePar::~PndSensorNamePar(void)
{
  if (fSensorNames)
    delete fSensorNames;
}

PndSensorNamePar &PndSensorNamePar::operator=(const PndSensorNamePar &L)
{
  fSensorNames = new TObjArray(*(L.GetSensorNames()));
  return *this;
}

void PndSensorNamePar::putParams(FairParamList *list)
{
  if (!list)
    return;
  list->addObject("SensorNames", fSensorNames);
}

Bool_t PndSensorNamePar::getParams(FairParamList *list)
{
  if (!list)
    return kFALSE;
  if (!list->fillObject("SensorNames", fSensorNames))
    return kFALSE;
  FillMap();
  return kTRUE;
}

Int_t PndSensorNamePar::SensorInList(TObjString *name)
{
  auto iter = fMapOfSensorNames.find(name->GetString());
  if (iter != fMapOfSensorNames.end())
    return iter->second;
  else
    return -1;
}

void PndSensorNamePar::FillMap()
{
  LOG(info) << "PndSensorNamePar::FillMap() - There are " << fSensorNames->GetEntriesFast() << " entries.";
  if (fSensorNames->GetEntriesFast() == 0)
    return;
  for (int i = 0; i < fSensorNames->GetEntries(); i++) {
    TObjString *SensName = (TObjString *)fSensorNames->At(i);
    TString sensString = SensName->GetString();
    Ssiz_t delimiterPos = sensString.Last(':');
    TString sensName = sensString(0, delimiterPos);
    Int_t length = sensString.Length() - sensName.Length();
    Int_t id = TString(sensString(delimiterPos + 2, length)).Atoi();
    fMapOfSensorNames[sensName] = id;
    fMapOfSensorIndizes[id] = sensName;
  }
  LOG(info) << "PndSensorNamePar::FillMap() done";
}

TString PndSensorNamePar::GetSensorName(Int_t index)
{

  auto iter = fMapOfSensorIndizes.find(index);
  if (iter != fMapOfSensorIndizes.end()) {
    return iter->second;
  } else {
    LOG(error) << "-E- PndSensorNamePar::GetSensorName index " << index << " not in list!";
    return "";
  }
}

Int_t PndSensorNamePar::AddSensorName(TObjString *name)
{
  if (SensorInList(name) < 0) {
    Int_t entries = fSensorNames->GetEntries();
    TString sensName = name->GetString() + ": " + std::to_string(entries);
    TObjString *sensObjStr = new TObjString(sensName);
    fSensorNames->AddLast(sensObjStr);
    this->setChanged();
    FairRun *fRun = FairRun::Instance();
    this->setInputVersion(fRun->GetRunId(), 1);
    fMapOfSensorNames[name->GetString()] = entries;
    fMapOfSensorIndizes[entries] = name->GetString();
    return entries;
  } else {
    return SensorInList(name);
  }
}

Int_t PndSensorNamePar::AddSensorNameAndId(TObjString *name, Int_t id)
{
  if (SensorInList(name) < 0) {
    TString sensName = name->GetString() + ": " + std::to_string(id);
    TObjString *sensObjStr = new TObjString(sensName);
    fSensorNames->AddLast(sensObjStr);
    this->setChanged();
    FairRun *fRun = FairRun::Instance();
    this->setInputVersion(fRun->GetRunId(), 1);
    fMapOfSensorNames[name->GetString()] = id;
    fMapOfSensorIndizes[id] = name->GetString();
    return id;
  } else {
    Int_t tmp = SensorInList(name);
    LOG(trace) << "PndSensorNamePar::AddSensorNameAndId(" << name->GetString() << ", " << id << ") already set. Returning current id: " << tmp;
    return tmp;
  }

  return fSensorNames->GetEntries() - 1;
}

std::vector<TString> PndSensorNamePar::GetSensorNamesWithString(TString identifier)
{
  std::vector<TString> result;
  for (auto &value : fMapOfSensorNames) {
    TString tstring{value.first};
    if (tstring.Contains(identifier)) {
      result.push_back(tstring);
    }
  }
  return result;
}

void PndSensorNamePar::Print()
{
  LOG(info) << "SensorNames:";
  LOG(info) << "Entries: " << fSensorNames->GetEntries();
  for (std::map<Int_t, TString>::iterator iter = fMapOfSensorIndizes.begin(); iter != fMapOfSensorIndizes.end(); iter++) {
    LOG(info) << iter->first << ": " << iter->second.Data();
  }
}

ClassImp(PndSensorNamePar);

std::ostream &operator<<(std::ostream &output, const PndSensorNamePar &par)
{
  output << "SensorNames:" << std::endl;
  output << "Entries: " << par.fSensorNames->GetEntries() << std::endl;

  for (std::map<Int_t, TString>::const_iterator iter = par.fMapOfSensorIndizes.begin(); iter != par.fMapOfSensorIndizes.end(); iter++) {
    output << iter->first << ": " << iter->second.Data() << std::endl;
  }
  return output;
}
