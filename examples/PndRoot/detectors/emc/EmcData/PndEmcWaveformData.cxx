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

#include "PndEmcWaveformData.h"

#include <map>
#include <ostream>
#include "assert.h"

class PndEmcAbsWaveformSimulator;
class PndEmcWaveform;

using std::cout;
using std::endl;
using std::map;
using std::pair;

ClassImp(PndEmcWaveformData);

PndEmcWaveformData::PndEmcWaveformData() : FairTimeStamp(), fDetectorId(-1), fWfSimulator() {}

PndEmcWaveformData::PndEmcWaveformData(Int_t detId, PndEmcAbsWaveformSimulator *wfSimulator) : FairTimeStamp(), fDetectorId(detId), fWfSimulator(wfSimulator) {}

bool PndEmcWaveformData::operator<(const PndEmcWaveformData &wfData) const
{
  return (fDetectorId < wfData.fDetectorId);
}

std::ostream &operator<<(std::ostream &out, PndEmcWaveformData &wfData)
{
  out << "waveform links to " << wfData.fHitMap.size() << " PndEmcHits:" << endl;

  int count = 0;
  for (map<Double_t, Double_t>::iterator it = wfData.fHitMap.begin(); it != wfData.fHitMap.end(); ++it) {
    out << "\t#" << count++ << "\t time: " << it->first << "\t energy: " << it->second << endl;
  }

  return out;
}

PndEmcWaveformData &PndEmcWaveformData::operator+=(const PndEmcWaveformData &toAdd)
{
  fHitMap.insert(toAdd.fHitMap.begin(), toAdd.fHitMap.end());
  AddLinks(toAdd.GetLinks());
  return *this;
}

bool PndEmcWaveformData::equal(FairTimeStamp *data)
{
  PndEmcWaveformData *wfData = dynamic_cast<PndEmcWaveformData *>(data);
  if (wfData != 0 && wfData->GetDetectorId() == fDetectorId) {
    return true;
  } else {
    return false;
  }
}

void PndEmcWaveformData::AddHit(const FairLink &link, Double_t absEventTime, Double_t energy)
{
  this->AddLink(link);
  fHitMap.insert(pair<Double_t, Double_t>(absEventTime, energy));
}

void PndEmcWaveformData::GetHitParameter(Int_t hitNo, Double_t &time, Double_t &energy)
{
  assert(hitNo >= 0 && hitNo < (int)fHitMap.size());
  map<Double_t, Double_t>::iterator it = fHitMap.begin();
  while (hitNo--)
    it++;
  time = it->first;
  energy = it->second;
}
