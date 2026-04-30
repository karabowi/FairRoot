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

//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id:$
//
// Description:
//	Class BSEmcExpClusterSplittingProcess.
//      Implementation of ClusterSplitter which splits
//      on the basis of exponential distance from the bump centroid.
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Adapted for the PANDA experiment at GSI
//
// Author List:
//      Phil Strother
//
// Copyright Information:
//	Copyright (C) 1997               Imperial College
//
// Modified:
// M. Babai
//------------------------------------------------------------------------

#include "BSEmcExpClusterSplittingProcess.h"

#include <algorithm>
#include <math.h>
#include <utility>

#include "TVector3.h"

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
#include "BSEmcGeoNeighbouringRelationPar.h"
#include "BSEmcPositionAlgo.h"
#include "BSEmcSubCluster.h"

#include "math.h"

//----------------
// Constructors --
//----------------

BSEmcExpClusterSplittingProcess::BSEmcExpClusterSplittingProcess()
  : PndProcess{"BSEmcExpClusterSplittingProcess"}, fClusteringParName(""), fPositionParName(""), fNeighbouringRelationParName(""), fPositionPar(nullptr), fClusterEnergyCut(0)
{
}

//--------------
// Destructor --
//--------------

BSEmcExpClusterSplittingProcess::~BSEmcExpClusterSplittingProcess() {}

void BSEmcExpClusterSplittingProcess::SetDetectorName(const std::string &t_detectorName)
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
  fParameterList.push_back(fPositionParName);
  fParameterList.push_back(fNeighbouringRelationParName);
}

void BSEmcExpClusterSplittingProcess::RequestDataContainer(PndContainerRegister *t_register)
{
  t_register->Request({fDigiBranchname, "BSEmcDigi", kFALSE, kFALSE});
  t_register->Request({fClusterBranchname, "BSEmcCluster", kTRUE, kTRUE});
  t_register->Request({fSubClusterBranchname, "BSEmcSubCluster", kTRUE, kTRUE});
}

void BSEmcExpClusterSplittingProcess::GetDataContainer(PndContainerRegister *t_register)
{
  fDigiArray = t_register->GetInput<BSEmcDigi>(fDigiBranchname);
  fClusterArray = t_register->GetOutput<BSEmcCluster>(fClusterBranchname);
  fSubClusterArray = t_register->GetOutput<BSEmcSubCluster>(fSubClusterBranchname);

  if (fDigiBranchname != "") {
    t_register->SetAsDefaultBranchFor(fDigiBranchname, "BSEmcDigi");
  }
  if (fClusterBranchname != "") {
    t_register->SetAsDefaultBranchFor(fClusterBranchname, "BSEmcCluster");
  }
  if (fSubClusterBranchname != "") {
    t_register->SetAsDefaultBranchFor(fSubClusterBranchname, "BSEmcSubCluster");
  }
}

void BSEmcExpClusterSplittingProcess::SetupParameters(const PndParameterRegister *t_paramRegister)
{
  {
    BSEmcClusteringPar *parameter = dynamic_cast<BSEmcClusteringPar *>(t_paramRegister->GetParameter(fClusteringParName));
    fMaxSubClusters = parameter->GetMaxSubClusters();
    fMaxIterations = parameter->GetMaxIterations();
    fCentroidShift = parameter->GetCentroidShift();
    fExponentialConstant = parameter->GetExponentialConstant();
    fMoliereRadius = parameter->GetMoliereRadius();
    fMinDigiEnergy = parameter->GetMinDigiEnergy();
    fClusterEnergyCut = parameter->GetClusterEnergyCut();
  }
  BSEmcGeoNeighbouringRelationPar *neighbouringRelation = dynamic_cast<BSEmcGeoNeighbouringRelationPar *>(t_paramRegister->GetParameter(fNeighbouringRelationParName));
  fEnergyProcess->SetNeighbouringRelation(neighbouringRelation);

  fPositionPar = dynamic_cast<BSEmcCrystalPositionPar *>(t_paramRegister->GetParameter(fPositionParName));

  {
    BSEmcClusterPropertiesPar *parameter = dynamic_cast<BSEmcClusterPropertiesPar *>(t_paramRegister->GetParameter(fClusterPropertiesParName));
    fPositionProcess->SetClusterPropertiesPar(parameter);
    fPositionProcess->SetPositionPar(fPositionPar);
  }
}

void BSEmcExpClusterSplittingProcess::Process()
{
  fSubClusterArray->Reset();
  std::vector<BSEmcSubCluster *> subcluster = SubClustering(fDigiArray->GetVectorOfPtrToConst(), fClusterArray->GetVector());
  for (BSEmcSubCluster *asubcluster : subcluster) {
    fSubClusterArray->CreateCopy(*asubcluster);
    delete asubcluster;
  }
}

/**
 * @brief Runs the task
 *
 * The algorithm is as follows: We will index each subCluster by its
 * maximum digi's PndEmcTwoCoordIndex.  We will set up a list of
 * subCluster centroids which to start with will be synonymous with the
 * location of the maxima.  We then apportion a weight to each
 * digi, according to its distance from the centroids.  We then
 * construct the subClusters according to these weights, which will
 * presumably give a different set of centroids.  This is repeated
 * until the centroids are static within tolerance, or we reach
 * the maximum number of iterations.
 *
 * @param opt unused
 * @return void
 */
std::vector<BSEmcSubCluster *> BSEmcExpClusterSplittingProcess::SubClustering(const std::vector<const BSEmcDigi *> &t_digiCont, const std::vector<BSEmcCluster *> &t_clusterCont)
{
  fSubClusterVector.clear();
  Int_t nClusters = t_clusterCont.size();
  for (Int_t iCluster = 0; iCluster < nClusters; iCluster++) {
    BSEmcCluster *theCluster = t_clusterCont.at(iCluster);
    std::vector<BSEmcDigiInfo_t> maximaDigiInfos = theCluster->GetMaximaDigis();
    Int_t numberOfSubClusters = maximaDigiInfos.size();

    if (theCluster->GetEnergy() < fClusterEnergyCut) {
      theCluster->SetNSubClusters(numberOfSubClusters);
      continue;
    }

    if (numberOfSubClusters <= 1 || numberOfSubClusters >= fMaxSubClusters) {
      if (numberOfSubClusters == 0) {
        LOG(warn) << "BSEmcExpClusterSplittingProcess::Process() - cluster " << iCluster << " has no maximum but enough energy. Ignoring it.";
        continue;
      }
      // Limit the max number of subClusters in the cluster to 8 (default)
      // in this case, we clearly have a cluster, but no subClusters to speak of.
      // Make 1 subCluster with weights all equal to 1
      SplitIntoSingleSubCluster(theCluster, iCluster);
    } else {

      // Now we can create the BSEmcSubClusters

      // The algorithm is as follows: We will index each subCluster by its
      // maximum digi's DetectorID.  We will set up a list of
      // subCluster centroids which to start with will be synonymous with the
      // location of the maxima.  We then apportion a weight to each
      // digi, according to its distance from the centroids.  We then
      // construct the subClusters according to these weights, which will
      // presumably give a different set of centroids.  This is repeated
      // until the centroids are static within tolerance, or we reach
      // the maximum number of iterations.
      SplitIntoMultipleSubCluster(theCluster, iCluster, t_digiCont, t_clusterCont);
    }

    // Int_t nSubClusters = maximaDigiInfos.size();
    // theCluster->SetNSubClusters(nSubClusters);
    LOG(debug) << "BSEmcExpClusterSplittingProcess::Process() -  Number of clusters = " << nClusters;
    // LOG(debug)<<"BSEmcExpClusterSplittingProcess::Process()"<<  " - " << " Number of subClusters = "<<nSubClusters;
  }
  for (BSEmcSubCluster *subcluster : fSubClusterVector) {
    subcluster->SetEnergy(fEnergyProcess->CalculateEnergy(subcluster, t_digiCont));
    subcluster->SetPosition(fPositionProcess->CalculatePosition(subcluster, t_digiCont));
    subcluster->SetTimeStamp(subcluster->CalculateTimeStamp(t_digiCont));
  }
  std::sort(fSubClusterVector.begin(), fSubClusterVector.end(), [](const BSEmcSubCluster *a, const BSEmcSubCluster *b) { return a->GetTimeStamp() < b->GetTimeStamp(); });
  return fSubClusterVector;
}

void BSEmcExpClusterSplittingProcess::SplitIntoSingleSubCluster(BSEmcCluster *t_cluster, Int_t t_clusterIdx)
{
  BSEmcSubCluster *currentSubCluster = new BSEmcSubCluster{};
  fSubClusterVector.push_back(currentSubCluster);
  currentSubCluster->SetParentClusterIndex(t_clusterIdx);
  currentSubCluster->SetTimeStamp(t_cluster->GetTimeStamp());
  currentSubCluster->SetTimeStampError(t_cluster->GetTimeStampError());
  if (t_cluster->GetMaximaDigis().size() > 0) {
    currentSubCluster->SetCentralCrystalId(t_cluster->GetMaximaDigis()[0].fDetectorId);
  } else {
    LOG(error) << "Creating SubCluster without central crystal!";
  }
  for (const auto &digiProxy : t_cluster->GetDigis()) {
    currentSubCluster->AddDigi(digiProxy.fDigiIdx, digiProxy.fDetectorId, 1.0);
  }
  t_cluster->SetNSubClusters(1);
}

void BSEmcExpClusterSplittingProcess::SplitIntoMultipleSubCluster(BSEmcCluster *t_cluster, Int_t t_clusterIdx, const std::vector<const BSEmcDigi *> &t_digiCont,
                                                                  const std::vector<BSEmcCluster *> &t_clusterCont)
{

  std::vector<BSEmcDigiInfo_t> maximaDigiInfos = t_cluster->GetMaximaDigis();
  std::vector<DigiLocationInfo> maximaPositions = GetEnergyAndLocationInfo(t_digiCont, maximaDigiInfos);

  std::map<Int_t, DigiLocationInfo> centroidPositions;
  for (auto &digi : maximaPositions) {
    centroidPositions[digi.fDetectorId] = digi;
  }

  std::vector<DigiLocationInfo> allDigiPositions = GetEnergyAndLocationInfo(t_digiCont, t_cluster->GetDigis());
  std::map<Int_t, BSEmcSubCluster *> tmpsubClusters;

  Int_t iterations = 0;
  Double_t averageCentroidShift = 0.0;
  do {
    averageCentroidShift = 0.0;

    // First clean up the old subClusters
    ClearSubClusters(tmpsubClusters);

    // Then loop over all the maxima and assign weights accordingly
    for (auto &maximumDigi : maximaDigiInfos) {
      Int_t currentMaxDetId = maximumDigi.fDetectorId;

      // Create the subCluster which will correspond to this maximum
      BSEmcSubCluster *currentSubCluster = new BSEmcSubCluster();
      tmpsubClusters.insert(std::map<Int_t, BSEmcSubCluster *>::value_type(currentMaxDetId, currentSubCluster));

      // Now we will loop over all the digis and add each of them
      // to this SubCluster with an appropriate weight
      for (auto &digiProxy : allDigiPositions) {
        Double_t weight = CalculateWeight(digiProxy.fDetectorId, currentMaxDetId, digiProxy.fLocation, centroidPositions);
        const BSEmcDigi *currentDigi = t_digiCont.at(digiProxy.fDigiIdx);
        if (currentDigi->GetEnergy() * weight > fMinDigiEnergy) {
          currentSubCluster->AddDigi(digiProxy.fDigiIdx, currentDigi->GetDetectorId(), weight);
        }
      }

      // Compute the shift of the centroid due to the current subClusters position
      TVector3 theOldCentroid = centroidPositions[currentMaxDetId].fLocation;

      currentSubCluster->SetEnergy(fEnergyProcess->CalculateEnergy(currentSubCluster, t_digiCont));
      TVector3 newsubClusterpos = fPositionProcess->CalculatePosition(currentSubCluster, t_digiCont);
      currentSubCluster->SetPosition(newsubClusterpos);
      TVector3 centroidShift{theOldCentroid - newsubClusterpos};
      currentSubCluster->SetCentralCrystalId(currentMaxDetId);
      averageCentroidShift += centroidShift.Mag();
    }

    averageCentroidShift /= (Double_t)maximaDigiInfos.size();
    UpdateCentroidPositions(tmpsubClusters, centroidPositions);
    iterations++;

  } while (iterations < fMaxIterations && averageCentroidShift > fCentroidShift);
  // Finally append the new subClusters to the TClonesArray.
  AddSubClusters(tmpsubClusters, t_clusterCont, t_clusterIdx);
  allDigiPositions.clear();
  centroidPositions.clear();
  ClearSubClusters(tmpsubClusters);
}

std::vector<DigiLocationInfo>
BSEmcExpClusterSplittingProcess::GetEnergyAndLocationInfo(const std::vector<const BSEmcDigi *> &t_digiCont, const std::vector<BSEmcDigiInfo_t> &t_digis) const
{
  std::vector<DigiLocationInfo> digiLocations;
  for (const auto &digiProxy : t_digis) {
    DigiLocationInfo digiLocation;
    digiLocation.fDetectorId = digiProxy.fDetectorId;
    digiLocation.fDigiIdx = digiProxy.fDigiIdx;

    const BSEmcDigi *theDigi = t_digiCont.at(digiProxy.fDigiIdx);
    digiLocation.fEnergy = theDigi->GetEnergy();
    digiLocation.fLocation = fPositionPar->GetPosition(digiProxy.fDetectorId);
    digiLocations.push_back(digiLocation);
  }
  return digiLocations;
}

void BSEmcExpClusterSplittingProcess::ClearSubClusters(std::map<Int_t, BSEmcSubCluster *> &t_subclusterMap) const
{
  std::map<Int_t, BSEmcSubCluster *>::iterator theSubClusterKiller = t_subclusterMap.begin();
  while (theSubClusterKiller != t_subclusterMap.end()) {
    BSEmcSubCluster *subCluster = theSubClusterKiller->second;
    delete subCluster;
    ++theSubClusterKiller;
  }
  t_subclusterMap.clear();
}

Double_t BSEmcExpClusterSplittingProcess::CalculateWeight(Int_t t_currentDigiDetID, Int_t t_currentMaxDetId, const TVector3 &t_currentdigisPosition,
                                                          const std::map<Int_t, DigiLocationInfo> &t_centroidPositions) const
{
  Double_t weight = 0;
  Double_t myEnergy = 0;
  Double_t myDistance = 0;
  Double_t totalDistanceEnergy = 0;

  for (const auto &centroidPosItr : t_centroidPositions) {
    Int_t centroidPosDetId = centroidPosItr.second.fDetectorId;
    TVector3 centroidPos = centroidPosItr.second.fLocation;
    Double_t theDistance = NAN;
    if ((t_currentDigiDetID) == (centroidPosDetId)) {
      theDistance = 0.0;
    } else {
      TVector3 distance{centroidPos - t_currentdigisPosition};
      theDistance = distance.Mag();
    }

    if (t_currentMaxDetId == centroidPosDetId) {
      // i.e. the maximum we are trying to find the distance from is
      // the one for which we are currently trying to make a subCluster
      myDistance = theDistance;
      myEnergy = centroidPosItr.second.fEnergy;
    }

    totalDistanceEnergy += centroidPosItr.second.fEnergy * exp(-fExponentialConstant * theDistance / fMoliereRadius);
  }

  if (totalDistanceEnergy > 0.0) {
    weight = myEnergy * exp(-fExponentialConstant * myDistance / fMoliereRadius) / (totalDistanceEnergy);
  } else {
    weight = 0;
  }
  return weight;
}

void BSEmcExpClusterSplittingProcess::UpdateCentroidPositions(const std::map<Int_t, BSEmcSubCluster *> &t_subclusterMap,
                                                              std::map<Int_t, DigiLocationInfo> &t_centroidPositions) const
{
  std::map<Int_t, DigiLocationInfo> tmp;
  for (const auto &subClusterItr : t_subclusterMap) {
    Int_t detId = subClusterItr.first;
    tmp[detId] = t_centroidPositions[detId];
    TVector3 theNewCentroid{(subClusterItr.second)->GetPosition()};
    tmp[detId].fLocation = theNewCentroid;
    tmp[detId].fSubClusterEnergy = subClusterItr.second->GetEnergy();
  }
  t_centroidPositions.clear();
  t_centroidPositions = tmp;
}

/**
 * @brief Adds a new BSEmcSubCluster to fSubClusterArray.
 *
 * I left out the time calculation here
 * @return BSEmcSubCluster*
 */
void BSEmcExpClusterSplittingProcess::AddSubClusters(const std::map<Int_t, BSEmcSubCluster *> &t_subclusterMap, const std::vector<BSEmcCluster *> &t_clusterCont,
                                                     Int_t t_clusterIdx)
{
  BSEmcCluster *rootCluster = t_clusterCont.at(t_clusterIdx);
  Int_t counter = 0;
  for (const auto &subClusterItr : t_subclusterMap) {
    BSEmcSubCluster *theNextSubCluster = new BSEmcSubCluster{*subClusterItr.second};
    fSubClusterVector.push_back(theNextSubCluster);
    LOG(trace) << "BSEmcExpClusterSplittingProcess::AddSubClusters - SubCluster Created!";

    theNextSubCluster->SetParentClusterIndex(t_clusterIdx);
    theNextSubCluster->SetTimeStamp(rootCluster->GetTimeStamp());
    theNextSubCluster->SetTimeStampError(rootCluster->GetTimeStampError());
    ++counter;
  }

  rootCluster->SetNSubClusters(counter);
}

ClassImp(BSEmcExpClusterSplittingProcess)
