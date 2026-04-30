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

// -------------------------------------------------------------------------
// -----             PndSdsStripClusterer source file             -----
// -----          Converted 27.11.2007 from R.Jaekel by R.Kliemt       -----
// -------------------------------------------------------------------------

#include "PndSdsStripClusterer.h"

// includes for sort algorithms
#include <algorithm>
#include <functional>
#include <vector>
#include "TMath.h"
#include "math.h"
// enum SensorSide { SensorSide::kTOP, SensorSide::kBOTTOM };

PndSdsStripClusterer::PndSdsStripClusterer(Int_t DigiType) : fSortedDigis(), fTopclusters(), fBotclusters(), fLeftDigis(), fClusters(), fDigiType(-1)
{
  SetDigiType(DigiType);
}

PndSdsStripClusterer::~PndSdsStripClusterer() {}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void PndSdsStripClusterer::Reinit()
{
  fSortedDigis.clear();
  fClusters.clear();
  fTopclusters.clear();
  fBotclusters.clear();
  fLeftDigis.clear();
}

void PndSdsStripClusterer::AddDigi(Int_t sensorID, SensorSide side, Int_t, Int_t strip, Int_t iDigi) // timestamp //[R.K.03/2017] unused variable(s)
{
  fSortedDigis[sensorID][side][1][strip] = iDigi;
  //[R.K.30.3.'12] Defunc timestamp binning as it comes in ns precision.
  // TODO: invent soemthing better with timestamp!
  //  fSortedDigis[sensorID][side][timestamp][strip] = iDigi;
}

//------------------------------------------------------------------------------
PndSdsClusterStrip *PndSdsStripClusterer::GetCluster(Int_t i)
{
  if (0 <= i && (Int_t)fClusters.size() < i)
    return fClusters[i];
  else {
    Fatal("GetCluster", "fClusters out of bounds i=%i at size=%zu", i, fClusters.size());
  }
  return nullptr;
}
//------------------------------------------------------------------------------
PndSdsClusterStrip *PndSdsStripClusterer::GetTopCluster(Int_t i)
{
  if (0 <= i && (Int_t)fTopclusters.size() < i)
    return GetCluster(fTopclusters[i]);
  else {
    Fatal("GetTopCluster", "fTopclusters out of bounds i=%i at size=%zu", i, fTopclusters.size());
  }
  return nullptr;
}
//------------------------------------------------------------------------------
PndSdsClusterStrip *PndSdsStripClusterer::GetBotCluster(Int_t i)
{
  if (0 <= i && (Int_t)fBotclusters.size() < i)
    return GetCluster(fBotclusters[i]);
  else {
    Fatal("GetBotCluster", "fBotclusters out of bounds i=%i at size=%zu", i, fBotclusters.size());
  }
  return nullptr;
}

//------------------------------------------------------------------------------
void PndSdsStripClusterer::AddCluster(const std::vector<Int_t> &onecluster, SensorSide side)
{
  Int_t clindex = fClusters.size();
  if (side == SensorSide::kBOTTOM) {
    fBotclusters.push_back(clindex);
    // Info("PndSdsStripClusterer::AddCluster","Adding Cluster number %i to the bottom list.",clindex);
  } else if (side == SensorSide::kTOP) {
    fTopclusters.push_back(clindex);
    // Info("AddCluster","Adding Cluster number %i to the top list.",clindex);
  }
  PndSdsClusterStrip *cl = new PndSdsClusterStrip(fDigiType, onecluster);
  cl->SetSensorSide(side);
  fClusters.push_back(cl);
}

ClassImp(PndSdsStripClusterer);
