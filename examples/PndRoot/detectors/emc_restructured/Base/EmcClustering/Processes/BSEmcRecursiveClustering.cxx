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

#include "BSEmcRecursiveClustering.h"

#include <algorithm>
#include <utility>

#include "FairParSet.h"
#include "fairlogger/Logger.h"

#include "PndContainerI.h"
#include "PndMutableContainerI.h"
#include "PndParameterRegister.h"

#include "BSEmcCluster.h"
#include "BSEmcClusterPropertiesPar.h"

#include "BSEmcClusteringPar.h"
#include "BSEmcCrystalPositionPar.h"
#include "BSEmcDataBranchNames.h"
#include "BSEmcDigi.h"
#include "BSEmcEnergyCalculationAlgo.h"
#include "BSEmcGeoItem.h"
#include "BSEmcGeoNeighbouringRelationPar.h"
#include "BSEmcPositionAlgo.h"

BSEmcRecursiveClustering::BSEmcRecursiveClustering()
  : PndProcess{"BSEmcRecursiveClustering"}, fClusteringParName(""), fPositionParName(""), fNeighbouringRelationParName(""), fClusterActiveTime(0), fDigiEnergyThreshold(0)
{
}

BSEmcRecursiveClustering::~BSEmcRecursiveClustering() {}

void BSEmcRecursiveClustering::SetDetectorName(const std::string &t_detectorName)
{
  fDetectorName = t_detectorName;
  if (fClusteringParName == "") {
    fClusteringParName = BSEmcClusteringPar::fgParameterName + fDetectorName;
  }
  if (fClusterPropertiesParName == "") {
    fClusterPropertiesParName = BSEmcClusterPropertiesPar::fgParameterName + fDetectorName;
  }
  if (fPositionParName == "") {
    fPositionParName = BSEmcCrystalPositionPar::fgParameterName + fDetectorName;
  }
  if (fNeighbouringRelationParName == "") {
    fNeighbouringRelationParName = BSEmcGeoNeighbouringRelationPar::fgParameterName + fDetectorName;
  }

  fParameterList.push_back(fClusteringParName);
  fParameterList.push_back(fClusterPropertiesParName);
  fParameterList.push_back(fNeighbouringRelationParName);
  fParameterList.push_back(fPositionParName);
}

void BSEmcRecursiveClustering::SetupParameters(const PndParameterRegister *t_paramRegister)
{
  {
    BSEmcClusteringPar *parameter = dynamic_cast<BSEmcClusteringPar *>(t_paramRegister->GetParameter(fClusteringParName));
    fClusterActiveTime = parameter->GetClusterActiveTime();
    fDigiEnergyThreshold = parameter->GetDigiEnergyThreshold();
    LOG(debug) << "BSEmcRecursiveClustering::Init() - using: fClusterActiveTime = " << fClusterActiveTime << " fDigiEnergyThreshold = " << fDigiEnergyThreshold
               << " Maximum Time Between Digis: " << fClusterActiveTime << " ns";
  }
  fNeighbouringRelationPar = dynamic_cast<BSEmcGeoNeighbouringRelationPar *>(t_paramRegister->GetParameter(fNeighbouringRelationParName.c_str()));

  BSEmcCrystalPositionPar *positionPar = dynamic_cast<BSEmcCrystalPositionPar *>(t_paramRegister->GetParameter(fPositionParName.c_str()));
  {
    BSEmcClusterPropertiesPar *parameter = dynamic_cast<BSEmcClusterPropertiesPar *>(t_paramRegister->GetParameter(fClusterPropertiesParName));
    fPositionProcess->SetClusterPropertiesPar(parameter);
  }
  fPositionProcess->SetPositionPar(positionPar);
  fEnergyProcess->SetNeighbouringRelation(fNeighbouringRelationPar);

  for (const auto &idToNeighbourlist : fNeighbouringRelationPar->GetRelationMap()) {
    Int_t const id = idToNeighbourlist.first;
    fIDToCrystal[id] = std::unique_ptr<BSEmcGeoItem>{new BSEmcGeoItem()};
    fIDToCrystal[id]->SetID(id);
  }
  for (const auto &idToNeighbourlist : fNeighbouringRelationPar->GetRelationMap()) {
    Int_t const id = idToNeighbourlist.first;
    for (Int_t neighbour : idToNeighbourlist.second) {
      fIDToCrystal[id]->AddNeighbour(fIDToCrystal[neighbour].get());
    }
  }
}

void BSEmcRecursiveClustering::RequestDataContainer(PndContainerRegister *t_register)
{
  t_register->Request({fDigiBranchname, "BSEmcDigi", kFALSE, kFALSE});
  t_register->Request({fClusterBranchname, "BSEmcCluster", kTRUE, kTRUE});
}

void BSEmcRecursiveClustering::GetDataContainer(PndContainerRegister *t_register)
{
  fDigiArray = t_register->GetInput<BSEmcDigi>(fDigiBranchname);
  fClusterArray = t_register->GetOutput<BSEmcCluster>(fClusterBranchname);
  if (fDigiBranchname != "") {
    t_register->SetAsDefaultBranchFor(fDigiBranchname, "BSEmcDigi");
  }
  if (fClusterBranchname != "") {
    t_register->SetAsDefaultBranchFor(fClusterBranchname, "BSEmcCluster");
  }
}

void BSEmcRecursiveClustering::Process()
{
  std::vector<BSEmcCluster *> cluster = Clustering(fDigiArray->GetVectorOfPtrToConst());
  for (BSEmcCluster *acluster : cluster) {
    fClusterArray->CreateCopy(*acluster);
    delete acluster;
  }
}

void BSEmcRecursiveClustering::Reset()
{
  for (BSEmcGeoItem *crystal : fChangedCrystals) {
    crystal->Reset();
  }
  fChangedCrystals.clear();
}

void BSEmcRecursiveClustering::FillGeoItems(const std::vector<const BSEmcDigi *> &t_digis)
{
  Int_t digicounter = 0;
  for (const BSEmcDigi *digi : t_digis) {
    Double_t const id = digi->GetDetectorId();
    if (digi->GetEnergy() > fDigiEnergyThreshold) {
      fChangedCrystals.insert(fIDToCrystal[id].get());
      fIDToCrystal[id]->AddDigi(digi);
      fIDToCrystal[id]->AddDigiIndex(digicounter);
    }
    ++digicounter;
  }
}

std::vector<std::vector<BSEmcGeoItem *>> BSEmcRecursiveClustering::SpatialClustering(const std::vector<const BSEmcDigi *> &t_digis)
{
  Int_t nClusters = 0;

  std::vector<std::vector<BSEmcGeoItem *>> clusterstubs;
  for (const BSEmcDigi *digi : t_digis) {
    Double_t const id = digi->GetDetectorId();
    if (!fIDToCrystal[id]->IsAdded()) {
      clusterstubs.push_back({});
      fIDToCrystal[id]->FindClusterCrystals(clusterstubs[nClusters]);
      ++nClusters;
    }
  }
  return clusterstubs;
}

std::vector<BSEmcCluster *>
BSEmcRecursiveClustering::ConvertToCluster(const std::vector<std::vector<BSEmcGeoItem *>> &t_clusterstubs, const std::vector<const BSEmcDigi *> &t_digis) const
{
  std::vector<BSEmcCluster *> result;
  for (const std::vector<BSEmcGeoItem *> &clusterstub : t_clusterstubs) {
    if (clusterstub.size() > 0) {
      BSEmcCluster *cluster = new BSEmcCluster{};
      for (BSEmcGeoItem *item : clusterstub) {
        cluster->AddDigi(item->GetDigiIndices()[0], item->GetID());
      }
      cluster->SetEnergy(fEnergyProcess->CalculateEnergy(cluster, t_digis));
      cluster->SetPosition(fPositionProcess->CalculatePosition(cluster, t_digis));
      cluster->SetTimeStamp(cluster->CalculateTimeStamp(t_digis));
      result.push_back(cluster);
    }
  }
  return result;
}

std::vector<BSEmcCluster *> BSEmcRecursiveClustering::Clustering(const std::vector<const BSEmcDigi *> &t_digis)
{
  Reset();
  FillGeoItems(t_digis);
  std::vector<std::vector<BSEmcGeoItem *>> clusterstubs = SpatialClustering(t_digis);
  std::vector<BSEmcCluster *> cluster = ConvertToCluster(clusterstubs, t_digis);
  std::sort(cluster.begin(), cluster.end(), [](const BSEmcCluster *a, const BSEmcCluster *b) { return a->GetTimeStamp() < b->GetTimeStamp(); });
  return cluster;
}
