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

//---------------------------------------------------------------------
// Description:
//           energy()            returns energy sum of digis in cluster.
//           where()             returns a TVector3 at the centre of the
//                                 cluster.
//           x()                 returns the x element of this.
//           y()                 returns the y element of this.
//           z()                 returns the z element of this.
//
//	 Software developed for the BaBar Detector at the SLAC B-Factory.
// Adapted for the PANDA experiment at GSI
//
// Author List:
//	Xiaorong Shi            Lawrence Livermore National Lab
//	Steve Playfer           University of Edinburgh
//	Stephen Gowdy           University of Edinburgh
//	Helmut Marsiske         SLAC

//---------------------------------------------------------------------

#include "BSEmcCluster.h"

#include <FairMultiLinkedData.h>
#include <algorithm>
#include <iterator>
#include <limits>
#include <stdlib.h> // for abort

#include "Rtypes.h"     // for ClassImp
#include "RtypesCore.h" // for Int_t, kFALSE, Double_t, Bool_t, Opti...
#include "TVector3.h"   // for TVector3

#include "FairLink.h"          // for FairLink
#include "FairTimeStamp.h"     // for FairTimeStamp
#include "fairlogger/Logger.h" // for LOG

#include "BSEmcDigi.h" // for BSEmcDigi

//----------------
// Constructors --
//----------------

BSEmcCluster::BSEmcCluster()
  : FairTimeStamp(), fEnergyValid(kFALSE), fEnergy(0), fWhereValid(kFALSE), fWhere(TVector3(0, 0, 0)), fNSubClusters(0), fTrackEntering(), fTrackExiting()
{
}

BSEmcCluster::BSEmcCluster(const BSEmcCluster &t_other)
  : FairTimeStamp(t_other), fDigis(t_other.fDigis), fMaxima(t_other.fMaxima), fEnergyValid(t_other.fEnergyValid), fEnergy(t_other.fEnergy), fWhereValid(t_other.fWhereValid),
    fWhere(t_other.fWhere), fNSubClusters(t_other.fNSubClusters), fTrackEntering(t_other.fTrackEntering), fTrackExiting(t_other.fTrackExiting), fRadius(t_other.fRadius),
    fXRadius(t_other.fXRadius), fYRadius(t_other.fYRadius)
{
}

//--------------
// Destructor --
//--------------

BSEmcCluster::~BSEmcCluster() {}

Double_t BSEmcCluster::GetEnergy() const
{
  if (fEnergyValid) {
    return fEnergy;
  } else {
    LOG(error) << "Energy of cluster is not defined! Aborting!";
    abort();
  }
}

TVector3 BSEmcCluster::GetPosition() const
{
  if (fWhereValid) {
    return fWhere;
  } else {
    LOG(error) << "Position of cluster is not defined! Aborting!";
    abort();
  }
}

void BSEmcCluster::AddDigi(const BSEmcDigiInfo_t &t_digiInfo)
{
  invalidateCache(kFALSE);
  fDigis.push_back(t_digiInfo);
}

void BSEmcCluster::AddDigi(Int_t t_arrayindex, Int_t t_detectorId, Double_t t_weight)
{
  AddDigi({t_arrayindex, t_detectorId, t_weight});
}

void BSEmcCluster::AddDigiLink(FairLink t_entrynr)
{
  SetInsertHistory(kFALSE);
  AddLink(t_entrynr);
  SetInsertHistory(kTRUE);
}

void BSEmcCluster::RemoveDigi(Int_t t_arrayindex)
{
  fDigis.erase(std::remove_if(fDigis.begin(), fDigis.end(), [t_arrayindex](const BSEmcDigiInfo_t &a) { return a.fDigiIdx == t_arrayindex; }), fDigis.end());
}

void BSEmcCluster::AddCluster(const BSEmcCluster &t_cluster)
{
  const std::vector<BSEmcDigiInfo_t> &tmpList = t_cluster.GetDigis();
  for (const BSEmcDigiInfo_t &digi : tmpList) {
    AddDigi(digi);
  }
}

void BSEmcCluster::AddMaximum(const BSEmcDigiInfo_t &t_maximum)
{
  fMaxima.push_back(t_maximum);
}

void BSEmcCluster::AddMaximum(Int_t t_arrayindex, Int_t t_detectorid)
{
  AddMaximum({t_arrayindex, t_detectorid, 1});
}

void BSEmcCluster::RemoveMaximum(Int_t t_arrayindex)
{
  fMaxima.erase(std::remove_if(fMaxima.begin(), fMaxima.end(), [t_arrayindex](const BSEmcDigiInfo_t &a) { return a.fDigiIdx == t_arrayindex; }), fMaxima.end());
}

std::vector<Int_t> BSEmcCluster::GetMcList(Int_t t_branchId) const
{
  std::vector<Int_t> mcList;
  std::vector<FairLink> mcLinks;
  FairMultiLinkedData mcFairLinks = GetLinksWithType(t_branchId);

  for (Int_t i = 0; i < mcFairLinks.GetNLinks(); i++) {
    mcLinks.push_back(mcFairLinks.GetLink(i));
  }

  std::sort(mcLinks.begin(), mcLinks.end(), [](const FairLink &a, const FairLink &b) -> Bool_t { return a < b; });

  std::transform(mcLinks.begin(), mcLinks.end(), std::back_inserter(mcList), [](const FairLink &link) -> Int_t { return link.GetIndex(); });

  return mcList;
}

void BSEmcCluster::invalidateCache(Bool_t t_kState)
{
  fEnergyValid = t_kState;
  fWhereValid = t_kState;
}

Int_t BSEmcCluster::NumberOfDigis() const
{
  return fDigis.size();
}

void BSEmcCluster::SetNSubClusters(UInt_t t_nSubClusters)
{
  fNSubClusters = t_nSubClusters;
}

void BSEmcCluster::Print(const Option_t * /*unused*/) const
{
  LOG(info) << "*********************************";
  LOG(info) << "total energy of cluster: " << GetEnergy();
  LOG(info) << "TrackEntering: " << fTrackEntering;
  LOG(info) << "TrackExiting: " << fTrackExiting;
}

Double_t BSEmcCluster::GetMaximumEnergy(const std::vector<const BSEmcDigi *> &t_digis) const
{
  BSEmcDigiInfo_t digiInfo = GetMaximumDigiInfo(t_digis);
  const BSEmcDigi *digi = t_digis.at(digiInfo.fDigiIdx);
  return (digi->GetEnergy() * digiInfo.fWeight);
}

const BSEmcDigi *BSEmcCluster::GetMaximumDigi(const std::vector<const BSEmcDigi *> &t_digis) const
{
  BSEmcDigiInfo_t digiInfo = GetMaximumDigiInfo(t_digis);
  return t_digis.at(digiInfo.fDigiIdx);
}

BSEmcDigiInfo_t BSEmcCluster::GetMaximumDigiInfo(const std::vector<const BSEmcDigi *> &t_digis) const
{
  Double_t max_energy = 0;
  BSEmcDigiInfo_t maxDigiInfo;

  for (const BSEmcDigiInfo_t &digiInfo : fDigis) {
    const BSEmcDigi *digi = t_digis.at(digiInfo.fDigiIdx);
    if (max_energy < digi->GetEnergy() * digiInfo.fWeight) {
      max_energy = digi->GetEnergy() * digiInfo.fWeight;
      maxDigiInfo = digiInfo;
    }
  }

  return (maxDigiInfo);
}

Double_t BSEmcCluster::CalculateTimeStamp(const std::vector<const BSEmcDigi *> &t_digis) const
{
  Double_t timeStamp = std::numeric_limits<Double_t>::lowest();
  Double_t energy = std::numeric_limits<Double_t>::lowest();
  for (const BSEmcDigiInfo_t &digiInfo : GetDigis()) {
    const BSEmcDigi *digi = t_digis.at(digiInfo.fDigiIdx);
    if (digi->GetEnergy() * digiInfo.fWeight > energy) {
      energy = digi->GetEnergy() * digiInfo.fWeight;
      timeStamp = digi->GetTimeStamp();
    }
  }
  return timeStamp;
}

ClassImp(BSEmcCluster)
