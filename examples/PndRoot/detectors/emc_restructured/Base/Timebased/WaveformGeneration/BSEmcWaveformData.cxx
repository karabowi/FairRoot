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

#include "BSEmcWaveformData.h"

#include <iostream>
#include <map>
#include <ostream>
#include <utility>

#include "assert.h"

class FairLink;

using std::map;
using std::pair;

ClassImp(BSEmcWaveformData);
BSEmcWaveformData::BSEmcWaveformData() : FairTimeStamp(), fDetectorId(-1), fWfSimulator() {}

BSEmcWaveformData::BSEmcWaveformData(Int_t t_detId, BSEmcAbsWaveformSimulator *t_wfSimulator) : FairTimeStamp(), fDetectorId(t_detId), fWfSimulator(t_wfSimulator) {}

Bool_t BSEmcWaveformData::operator<(const BSEmcWaveformData &t_wfData) const
{
  return (fDetectorId < t_wfData.fDetectorId);
}

BSEmcWaveformData &BSEmcWaveformData::operator+=(const BSEmcWaveformData &t_toAdd)
{
  fDepMap.insert(t_toAdd.fDepMap.begin(), t_toAdd.fDepMap.end());
  AddLinks(t_toAdd.GetLinks());
  return *this;
}

Bool_t BSEmcWaveformData::equal(FairTimeStamp *t_data)
{
  BSEmcWaveformData *wfData = dynamic_cast<BSEmcWaveformData *>(t_data);
  if (wfData != nullptr && wfData->GetDetectorId() == fDetectorId) {
    return kTRUE;
  } else {
    return kFALSE;
  }
}

void BSEmcWaveformData::AddDeposit(const FairLink &t_link, Double_t t_absEventTime, Double_t t_energy)
{
  this->AddLink(t_link);
  fDepMap.insert(pair<Double_t, Double_t>(t_absEventTime, t_energy));
}

void BSEmcWaveformData::GetDepositParameter(Int_t t_depositNo, Double_t &t_time, Double_t &t_energy)
{
  assert(t_depositNo >= 0 && t_depositNo < (Int_t)fDepMap.size());
  map<Double_t, Double_t>::iterator it = fDepMap.begin();
  while ((t_depositNo--) != 0) {
    it++;
  }
  t_time = it->first;
  t_energy = it->second;
}
