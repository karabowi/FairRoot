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

#include "BSEmcCluster.h"

#include "BSEmcClusteringPar.h"
#include "BSEmcFlagSplitOffs.h"
#include "BSEmcCrystalPositionPar.h"
#include "BSEmcDigi.h"
#include "BSEmcRecoHit.h"
#include "BSEmcSubCluster.h"
#include "FairLogger.h"
#include "PndProcess.h"
#include <string>
#include <limits>

BSEmcFlagSplitOffs::BSEmcFlagSplitOffs()
  : PndProcess{"BSEmcFlagSplitOffs"}, fPositionParName(BSEmcCrystalPositionPar::fgParameterName), fNeighbouringRelationParName(BSEmcGeoNeighbouringRelationPar::fgParameterName),
    fSplitOffParName(BSEmcSplitOffPar::fgParameterName)
{
}

BSEmcFlagSplitOffs::~BSEmcFlagSplitOffs() {}

void BSEmcFlagSplitOffs::SetDetectorName(const std::string &t_detectorName)
{
  fDetectorName = t_detectorName;
  fPositionParName += fDetectorName;
  fNeighbouringRelationParName += fDetectorName;
  fSplitOffParName += fDetectorName;

  fParameterList.push_back(fPositionParName);
  fParameterList.push_back(fNeighbouringRelationParName);
  fParameterList.push_back(fSplitOffParName);
}

void BSEmcFlagSplitOffs::RequestDataContainer(PndContainerRegister *t_register)
{
  t_register->Request({fDigiBranchname, "BSEmcDigi", kFALSE, kFALSE});
  t_register->Request({fClusterBranchname, "BSEmcCluster", kFALSE, kFALSE});
  t_register->Request({fSubClusterBranchname, "BSEmcSubCluster", kFALSE, kFALSE});
  t_register->Request({fRecoHitBranchname, "BSEmcRecoHit", kTRUE, kTRUE});
}

void BSEmcFlagSplitOffs::GetDataContainer(PndContainerRegister *t_register)
{
  fDigiArray = t_register->GetInput<BSEmcDigi>(fDigiBranchname);
  fClusterArray = t_register->GetInput<BSEmcCluster>(fClusterBranchname);
  fSubClusterArray = t_register->GetInput<BSEmcSubCluster>(fSubClusterBranchname);
  fRecoHitArray = t_register->GetOutput<BSEmcRecoHit>(fRecoHitBranchname);
}

void BSEmcFlagSplitOffs::SetupParameters(const PndParameterRegister *t_parameterRegister)
{
  fNeighbouringRelationPar = dynamic_cast<BSEmcGeoNeighbouringRelationPar *>(t_parameterRegister->GetParameter(fNeighbouringRelationParName));

  fPositionPar = dynamic_cast<BSEmcCrystalPositionPar *>(t_parameterRegister->GetParameter(fPositionParName));

  fSplitOffPar = dynamic_cast<BSEmcSplitOffPar *>(t_parameterRegister->GetParameter(fSplitOffParName));
  f1PEDFunction.reset(new TF1("1PEDFunction", fSplitOffPar->Get1PEDFunction(), 0, 1000000));      //
  f2PEDFunction.reset(new TF1("2PEDFunction", fSplitOffPar->Get2PEDFunction(), 0, 100));          // Fetch SplitOff parameters
  fHighPEDFunction.reset(new TF1("HighPEDFunction", fSplitOffPar->GetHighPEDFunction(), 0, 100)); //

  LOG(info) << "BSEmcFlagSplitOffs::SetupParameters() done ";
}

std::map<int, std::vector<const BSEmcSubCluster *>> BSEmcFlagSplitOffs::CreateClusterMap()
{
  std::map<int, std::vector<const BSEmcSubCluster *>> ClusterMap;
  for (int i = 0; i < fClusterArray->GetSize(); i++) {                                    //
    for (const BSEmcSubCluster *subcluster : fSubClusterArray->GetVectorOfPtrToConst()) { // Find all subclusters resulting from each cluster
      if (subcluster->GetParentClusterIndex() == i) {                                     // and connect them via the map
        ClusterMap[i].push_back(subcluster);                                              //
      }
    }
  }
  return ClusterMap;
}

const BSEmcSubCluster *BSEmcFlagSplitOffs::FindSubCluster(const BSEmcDigiInfo_t &max, std::vector<const BSEmcSubCluster *> subcluster_array)
{
  const BSEmcSubCluster *parent_subcluster{nullptr};
  for (const BSEmcSubCluster *subcluster : subcluster_array) {  //
    if (max.fDetectorId == subcluster->GetCentralCrystalId()) { // Find subcluster belonging to given maximum
      parent_subcluster = subcluster;                           //
      break;
    }
  }
  return parent_subcluster;
}

std::vector<BSEmcDigiInfo_t> BSEmcFlagSplitOffs::FindSecondaryMaxima(const BSEmcDigiInfo_t &centralmax, const std::vector<BSEmcDigiInfo_t> &maxima)
{
  std::vector<BSEmcDigiInfo_t> secondary_maxima;
  for (const BSEmcDigiInfo_t &max : maxima) {  //
    if (max.fDigiIdx != centralmax.fDigiIdx) { // Collect all maxima that are not the given central maximum
      secondary_maxima.push_back(max);         //
    }
  }
  return secondary_maxima;
}

double BSEmcFlagSplitOffs::CalculateShowerMass(const std::vector<const BSEmcDigi *> &DigiArray, const BSEmcCluster *cluster)
{
  const std::vector<BSEmcDigiInfo_t> &digis = cluster->GetDigis();
  double energy_sum = 0;
  TVector3 momentum_sum;
  for (BSEmcDigiInfo_t digi : digis) {
    const double digi_energy = DigiArray[digi.fDigiIdx]->GetEnergy();
    TVector3 momentum = fPositionPar->GetPosition(DigiArray[digi.fDigiIdx]->GetDetectorId()); // Create momentum vector of digi by taking the position
    momentum = momentum * (digi_energy / momentum.Mag());                                     // vector and scaling it to the energy
    energy_sum += digi_energy;
    momentum_sum += momentum;
  }
  return sqrt(pow(energy_sum, 2) - momentum_sum.Mag2()); // Calculate invariant mass of all digis within the cluster
}

bool BSEmcFlagSplitOffs::ShowerMassCut(const std::vector<const BSEmcDigi *> &DigiArray, const BSEmcCluster *cluster)
{
  const double shower_mass = CalculateShowerMass(DigiArray, cluster);
  const double cluster_energy = cluster->GetEnergy();
  return (f2PEDFunction->Eval(cluster_energy) > shower_mass); // Apply cut on the shower mass for 2 PED clusters that was fetched from the parameter file
}

double BSEmcFlagSplitOffs::CalculateHighPEDShowerMass(const std::vector<const BSEmcDigi *> &DigiArray, const BSEmcSubCluster *subcluster1, const BSEmcSubCluster *subcluster2)
{
  std::vector<BSEmcDigiInfo_t> first_digis = subcluster1->GetDigis();
  const std::vector<BSEmcDigiInfo_t> second_digis = subcluster2->GetDigis();
  first_digis.insert(first_digis.end(), second_digis.begin(), second_digis.end()); // Combine digis of the two given subclusters
  double energy_sum = 0;
  TVector3 momentum_sum;
  for (BSEmcDigiInfo_t digi : first_digis) {
    const double digi_energy = DigiArray[digi.fDigiIdx]->GetEnergy() * digi.fWeight;
    TVector3 momentum = fPositionPar->GetPosition(DigiArray[digi.fDigiIdx]->GetDetectorId()); // Create momentum vector of digi by taking the position
    momentum = momentum * (digi_energy / momentum.Mag());                                     // vector and scaling it to the energy
    energy_sum += digi_energy;
    momentum_sum += momentum;
  }
  return sqrt(pow(energy_sum, 2) - momentum_sum.Mag2()); // Calculate invariant mass of all the collected digis
}

bool BSEmcFlagSplitOffs::HighPEDCut(const std::vector<const BSEmcDigi *> &DigiArray, const BSEmcSubCluster *subcluster1, const BSEmcSubCluster *subcluster2)
{
  const double shower_mass = CalculateHighPEDShowerMass(DigiArray, subcluster1, subcluster2);
  const double cluster_energy = subcluster1->GetEnergy() + subcluster2->GetEnergy();
  return (fHighPEDFunction->Eval(cluster_energy) > shower_mass); // Apply cut on the shower mass for high PED clusters that was fetched from the parameter file
}

double BSEmcFlagSplitOffs::CalculateDistance(const std::vector<const BSEmcDigi *> &DigiArray, const BSEmcDigiInfo_t &firstmax, const BSEmcDigiInfo_t &secondmax)
{
  const TVector3 firstposition = fPositionPar->GetPosition(DigiArray[firstmax.fDigiIdx]->GetDetectorId());
  const TVector3 secondposition = fPositionPar->GetPosition(DigiArray[secondmax.fDigiIdx]->GetDetectorId());
  return sqrt(pow(firstposition.X() - secondposition.X(), 2) + pow(firstposition.Y() - secondposition.Y(), 2) + pow(firstposition.Z() - secondposition.Z(), 2));
}

double BSEmcFlagSplitOffs::FindSmallestDistance(const std::vector<const BSEmcCluster *> &ClusterArray, const std::vector<const BSEmcDigi *> &DigiArray, const BSEmcDigiInfo_t &max)
{
  double distance = std::numeric_limits<double>::max();
  for (const BSEmcCluster *cluster : ClusterArray) {
    const std::vector<BSEmcDigiInfo_t> &maxima = cluster->GetMaximaDigis();
    if (maxima.size() > 0) {
      if (maxima[0].fDetectorId != max.fDetectorId) {
        const std::vector<BSEmcDigiInfo_t> &digis = cluster->GetDigis();
        for (BSEmcDigiInfo_t digi : digis) {
          const double temp_distance = CalculateDistance(DigiArray, max, digi); // Calculate distance of given maximum to each digi of other clusters
          if (temp_distance < distance) {
            distance = temp_distance; // Store the smallest distance that was calculated so far
          }
        }
      }
    }
  }
  return distance;
}

double
BSEmcFlagSplitOffs::FindEnergyClosestCluster(const std::vector<const BSEmcCluster *> &ClusterArray, const std::vector<const BSEmcDigi *> &DigiArray, const BSEmcDigiInfo_t &max)
{
  double distance = std::numeric_limits<double>::max();
  const BSEmcCluster *closest_cluster = nullptr;
  if (ClusterArray.size() > 1) {
    for (const BSEmcCluster *cluster : ClusterArray) {
      const std::vector<BSEmcDigiInfo_t> &maxima = cluster->GetMaximaDigis();
      if (maxima.size() > 0) {
        if (maxima[0].fDetectorId != max.fDetectorId) {
          const std::vector<BSEmcDigiInfo_t> &digis = cluster->GetDigis();
          for (BSEmcDigiInfo_t digi : digis) {
            const double temp_distance = CalculateDistance(DigiArray, max, digi); // Calculate distance of given maximum to each digi of other clusters
            if (temp_distance < distance) {
              distance = temp_distance;  // Store the smallest distance that was calculated so far
              closest_cluster = cluster; // Store the closest cluster found so far
            }
          }
        }
      }
    }
  }
  if (closest_cluster != nullptr) {
    return closest_cluster->GetEnergy(); // If other clusters are present, return the energy of the closest one
  } else {
    return 0;
  }
}

bool BSEmcFlagSplitOffs::RatioCut(const double ratio, const double distance)
{
  return (f1PEDFunction->Eval(ratio) > distance); // Apply cut for 1 PED clusters that was fetched from the parameter file
}

void BSEmcFlagSplitOffs::FlagHighPED(BSEmcFlagSplitOffs::Cluster_Info &ClusterInfo, std::vector<int> &SplitOffSubclusterIds)
{
  const std::vector<BSEmcDigiInfo_t> maxima = ClusterInfo.cluster->GetMaximaDigis();
  const BSEmcDigiInfo_t firstmax = ClusterInfo.cluster->GetMaximumDigiInfo(ClusterInfo.DigiArray);
  const std::vector<BSEmcDigiInfo_t> secondarymaxima = FindSecondaryMaxima(firstmax, maxima);
  const BSEmcSubCluster *first_sub = FindSubCluster(firstmax, ClusterInfo.SubclusterArray);
  for (const BSEmcDigiInfo_t secondmax : secondarymaxima) {                                     //
    const BSEmcSubCluster *second_sub = FindSubCluster(secondmax, ClusterInfo.SubclusterArray); // Combine each secondary subcluster with the primary one and collect
    if (HighPEDCut(ClusterInfo.DigiArray, first_sub, second_sub)) {                             // the id of the secondary one, if it is marked as SplitOff by the cut
      SplitOffSubclusterIds.push_back(second_sub->GetCentralCrystalId());                       //
    }
  }
}

void BSEmcFlagSplitOffs::Flag2PED(BSEmcFlagSplitOffs::Cluster_Info &ClusterInfo, std::vector<int> &SplitOffSubclusterIds)
{
  const std::vector<BSEmcDigiInfo_t> maxima = ClusterInfo.cluster->GetMaximaDigis();
  const BSEmcDigiInfo_t firstmax = ClusterInfo.cluster->GetMaximumDigiInfo(ClusterInfo.DigiArray);
  const std::vector<BSEmcDigiInfo_t> secondarymaxima = FindSecondaryMaxima(firstmax, maxima);
  const BSEmcSubCluster *subcluster = FindSubCluster(secondarymaxima[0], ClusterInfo.SubclusterArray);
  if (ShowerMassCut(ClusterInfo.DigiArray, ClusterInfo.cluster)) {      // Apply shower mass cut to cluster and store
    SplitOffSubclusterIds.push_back(subcluster->GetCentralCrystalId()); // the subcluster id, if it is marked as SplitOff
  }
}

void BSEmcFlagSplitOffs::Flag1PED(BSEmcFlagSplitOffs::Cluster_Info &ClusterInfo, std::vector<int> &SplitOffSubclusterIds)
{
  const std::vector<BSEmcDigiInfo_t> maxima = ClusterInfo.cluster->GetMaximaDigis();
  const BSEmcDigiInfo_t max = maxima[0];
  const double distance = FindSmallestDistance(ClusterInfo.ClusterArray, ClusterInfo.DigiArray, max);           // Collect information of
  const double closest_energy = FindEnergyClosestCluster(ClusterInfo.ClusterArray, ClusterInfo.DigiArray, max); // the next closest cluster
  const BSEmcSubCluster *subcluster = FindSubCluster(max, ClusterInfo.SubclusterArray);
  if (closest_energy > 0 && RatioCut(ClusterInfo.cluster->GetEnergy() / closest_energy, distance)) { // Apply cut to given cluster and the next closest one and store
    SplitOffSubclusterIds.push_back(subcluster->GetCentralCrystalId());                              // subcluster id of the original one, if it is marked as SplitOff
  }
}

void BSEmcFlagSplitOffs::Process()
{
  std::map<int, std::vector<const BSEmcSubCluster *>> ClusterMap = CreateClusterMap(); // Create map connecting clusters to their subclusters
  std::map<int, std::vector<const BSEmcSubCluster *>>::iterator it;
  std::vector<int> SplitOffSubclusterIds{};
  const std::vector<const BSEmcDigi *> DigiArray = fDigiArray->GetVectorOfPtrToConst();
  const std::vector<const BSEmcCluster *> ClusterArray = fClusterArray->GetVectorOfPtrToConst();
  for (it = ClusterMap.begin(); it != ClusterMap.end(); it++) {
    const BSEmcCluster *cluster = fClusterArray->GetConstElementPtr(it->first);
    Cluster_Info ClusterInfo(DigiArray, ClusterArray, cluster, it->second); // Package ClusterInfo
    const std::vector<BSEmcDigiInfo_t> &maxima = ClusterInfo.cluster->GetMaximaDigis();
    if (maxima.size() == 1) {
      Flag1PED(ClusterInfo, SplitOffSubclusterIds); // Collect subcluster id's of 1PED subclusters that are supposed to be flagged
    }
    if (maxima.size() == 2) {
      Flag2PED(ClusterInfo, SplitOffSubclusterIds); // Collect subcluster id's of 2PED subclusters that are supposed to be flagged
    }
    if (maxima.size() > 2 && maxima.size() < 8) {
      FlagHighPED(ClusterInfo, SplitOffSubclusterIds); // Collect subcluster id's of high PED subclusters that are supposed to be flagged
    }
  }
  for (BSEmcRecoHit *recohit : fRecoHitArray->GetVector()) {
    if (std::find(SplitOffSubclusterIds.begin(), SplitOffSubclusterIds.end(), recohit->GetCentralCrystalId()) != SplitOffSubclusterIds.end()) {
      recohit->SetSplitOff(true); // Set flag for RecoHits corresponding to collected subcluster id's
    }
  }
}
