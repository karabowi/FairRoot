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

#include "BSEmcClusteringProcess.h"

#include <algorithm>

#include "TError.h"
#include "TMathBase.h"

#include "FairParSet.h"
#include "FairRunAna.h"
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

BSEmcClusteringProcess::BSEmcClusteringProcess()
  : PndProcess{"BSEmcClusteringProcess"}, fClusteringParName(""), fPositionParName(""), fNeighbouringRelationParName(""), fDigiBranchName(""), fClusterActiveTime(0),
    fDigiEnergyThreshold(0), fDigiCandidates({}), fNClusters(0), fNDigisPassed(0), fClusterNumbersForSameCluster({})
{
}

BSEmcClusteringProcess::~BSEmcClusteringProcess() {}

void BSEmcClusteringProcess::SetDetectorName(const std::string &t_detectorName)
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

void BSEmcClusteringProcess::SetupParameters(const PndParameterRegister *t_paramRegister)
{
  // fDigiBranchName = BSEmcDataBranchNames::fgDigiBranchName + fDetectorName;
  {
    BSEmcClusteringPar *parameter = dynamic_cast<BSEmcClusteringPar *>(t_paramRegister->GetParameter(fClusteringParName));
    fClusterActiveTime = parameter->GetClusterActiveTime();
    fDigiEnergyThreshold = parameter->GetDigiEnergyThreshold();
    LOG(debug) << "BSEmcClusteringProcess::Init() - using: " << fDigiBranchName << " fClusterActiveTime = " << fClusterActiveTime
               << " fDigiEnergyThreshold = " << fDigiEnergyThreshold << " Maximum Time Between Digis: " << fClusterActiveTime << " ns";
  }

  fNeighbouringRelationPar = dynamic_cast<BSEmcGeoNeighbouringRelationPar *>(t_paramRegister->GetParameter(fNeighbouringRelationParName.c_str()));

  BSEmcCrystalPositionPar *positionPar = dynamic_cast<BSEmcCrystalPositionPar *>(t_paramRegister->GetParameter(fPositionParName.c_str()));
  {
    BSEmcClusterPropertiesPar *parameter = dynamic_cast<BSEmcClusterPropertiesPar *>(t_paramRegister->GetParameter(fClusterPropertiesParName));
    fPositionProcess->SetClusterPropertiesPar(parameter);
    fPositionProcess->SetPositionPar(positionPar);
  }

  fEnergyProcess->SetNeighbouringRelation(fNeighbouringRelationPar);
}

void BSEmcClusteringProcess::RequestDataContainer(PndContainerRegister *t_register)
{
  t_register->Request({fDigiBranchname, "BSEmcDigi", kFALSE, kFALSE});
  t_register->Request({fClusterBranchname, "BSEmcCluster", kTRUE, kTRUE});
}

void BSEmcClusteringProcess::GetDataContainer(PndContainerRegister *t_register)
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

void BSEmcClusteringProcess::Process()
{
  fClusterArray->Reset();
  LOG(debug) << fDetectorName << " - BSEmcClusteringProcess::Process() - " << fDigiArray->GetSize() << " digis to cluster.";
  std::vector<BSEmcCluster *> cluster = Clustering(fDigiArray->GetVectorOfPtrToConst());
  for (BSEmcCluster *acluster : cluster) {
    fClusterArray->CreateCopy(*acluster);
    delete acluster;
  }
  LOG(debug) << fDetectorName << " - BSEmcClusteringProcess::Process() - " << fClusterArray->GetSize() << " cluster found.";
}

std::vector<BSEmcCluster *> BSEmcClusteringProcess::Clustering(const std::vector<const BSEmcDigi *> &t_digis)
{
  fNClusters = 0;
  CreateClusterableDigiCandidates(t_digis);
  FindNeighboursOfEveryDigi();
  PrimaryClustering();
  SecondaryClustering();
  CleanUpClusterNumbering();
  std::vector<BSEmcCluster *> cluster = CreateEmcCluster();
  for (BSEmcCluster *acluster : cluster) {

    acluster->SetEnergy(fEnergyProcess->CalculateEnergy(acluster, t_digis));
    acluster->SetPosition(fPositionProcess->CalculatePosition(acluster, t_digis));
    acluster->SetTimeStamp(acluster->CalculateTimeStamp(t_digis));
  }
  std::sort(cluster.begin(), cluster.end(), [](BSEmcCluster *a, BSEmcCluster *b) { return a->GetTimeStamp() < b->GetTimeStamp(); });

  return cluster;
}

void BSEmcClusteringProcess::CreateClusterableDigiCandidates(const std::vector<const BSEmcDigi *> &t_digis)
{
  fDigiCandidates.clear();
  fNDigisPassed = 0;
  Int_t nDigis = t_digis.size();
  for (Int_t iDigi = 0; iDigi < nDigis; ++iDigi) {
    const BSEmcDigi *theDigi = t_digis.at(iDigi);

    // thresholds: if energy is below the corresponding threshold, digi is not considered in clustering
    if (theDigi->GetEnergy() < fDigiEnergyThreshold) {
      continue;
    }

    DigiClusteringCandidate digiCandidate;
    digiCandidate.DigiIdx = iDigi;
    digiCandidate.ClusterNumber = -1;
    digiCandidate.DetectorId = theDigi->GetDetectorId();
    digiCandidate.Time = theDigi->GetTimeStamp();
    digiCandidate.NNeighbours = 0;
    digiCandidate.Neighbours.clear();
    fDigiCandidates.push_back(digiCandidate);

    fNDigisPassed++;
  }

  if (fNDigisPassed > nDigis) {
    Fatal("Exec", "Attempt to process more digis than are present in this timebunch");
  }
}

void BSEmcClusteringProcess::FindNeighboursOfEveryDigi()
{

  for (Int_t d = 0; d < fNDigisPassed - 1; ++d) { // check all pairs of digis, so all digis up to the second-to-last one
    for (Int_t e = d + 1; e < fNDigisPassed; ++e) {
      if (AreDigisNeighbours(e, d)) {
        fDigiCandidates[d].Neighbours.push_back(e);
        fDigiCandidates[d].NNeighbours++;
      }
    }
  }
}

Bool_t BSEmcClusteringProcess::AreDigisNeighbours(Int_t t_digi1, Int_t t_digi2) const
{
  // Time
  Double_t dt = 0;
  if (FairRunAna::Instance() != nullptr && FairRunAna::Instance()->IsTimeStamp()) {
    dt = TMath::Abs(fDigiCandidates[t_digi1].Time - fDigiCandidates[t_digi2].Time); // also take into account that non-consecutive digi pairs may differ in time more than dtau ns
  }
  // space
  const Bool_t areNeighbours = (fDigiCandidates[t_digi1].DetectorId == fDigiCandidates[t_digi2].DetectorId) ||
                               (fNeighbouringRelationPar->AreNeighbours(fDigiCandidates[t_digi1].DetectorId, fDigiCandidates[t_digi2].DetectorId));
  return areNeighbours && dt <= fClusterActiveTime;
}

void BSEmcClusteringProcess::PrimaryClustering()
{
  fClusterNumbersForSameCluster.clear();
  for (auto &digi : fDigiCandidates) {
    if (digi.ClusterNumber < 0) {
      digi.ClusterNumber = fNClusters++;
      AssignNeighbouringDigisSameClusterNumber(digi);
    } else {
      AssignNeighbouringDigisSameClusterNumber(digi);
    }
  }
  // TODO: Is this necessary?
  if (fNDigisPassed != 0) {
    if (fDigiCandidates[fNDigisPassed - 1].ClusterNumber < 0) {
      fDigiCandidates[fNDigisPassed - 1].ClusterNumber = fNClusters++;
    }
  }
}

void BSEmcClusteringProcess::AssignNeighbouringDigisSameClusterNumber(DigiClusteringCandidate &t_digi)
{
  for (Int_t j = 0; j < t_digi.NNeighbours; j++) {
    Int_t neighbouridx = t_digi.Neighbours[j];
    DigiClusteringCandidate &neighbour = fDigiCandidates[neighbouridx];
    if (neighbour.ClusterNumber < 0) {
      neighbour.ClusterNumber = t_digi.ClusterNumber; // if hit hasn't been added, put internal cluster nr here
    } else if (neighbour.ClusterNumber != t_digi.ClusterNumber) {
      if (t_digi.ClusterNumber > neighbour.ClusterNumber) {
        fClusterNumbersForSameCluster.push_back({t_digi.ClusterNumber, neighbour.ClusterNumber});
      } else {
        fClusterNumbersForSameCluster.push_back({neighbour.ClusterNumber, t_digi.ClusterNumber});
      }
    }
  }
}

void BSEmcClusteringProcess::SecondaryClustering()
{
  // Iterate over all clusters with multiple clusternumbers and reset them to one cluster number
  Int_t lowerClusterNr = 0, higherClusterNr = 0;
  for (UInt_t i = 0; i < fClusterNumbersForSameCluster.size(); ++i) {
    higherClusterNr = fClusterNumbersForSameCluster[i].first;
    lowerClusterNr = fClusterNumbersForSameCluster[i].second;
    if (higherClusterNr != lowerClusterNr) {
      for (auto &digicand : fDigiCandidates) {
        if (digicand.ClusterNumber == higherClusterNr) {
          digicand.ClusterNumber = lowerClusterNr;
        }
      }
      for (UInt_t j = i; j < fClusterNumbersForSameCluster.size(); j++) {
        if (fClusterNumbersForSameCluster[j].first == higherClusterNr) {
          fClusterNumbersForSameCluster[j].first = lowerClusterNr;
        }
        if (fClusterNumbersForSameCluster[j].second == higherClusterNr) {
          fClusterNumbersForSameCluster[j].second = lowerClusterNr;
        }
      }
    }
  }
}

void BSEmcClusteringProcess::CleanUpClusterNumbering()
{
  Int_t clusterNr = 0;
  for (Int_t currentClusterNumber = 0; currentClusterNumber < fNClusters; ++currentClusterNumber) {
    Int_t n = 0;
    for (auto &digicand : fDigiCandidates) {
      if (digicand.ClusterNumber == currentClusterNumber) {
        n++;
        digicand.ClusterNumber = clusterNr;
      }
    }
    if (n > 0) {
      clusterNr++;
    }
  }

  fNClusters = clusterNr + 1;
}

std::vector<BSEmcCluster *> BSEmcClusteringProcess::CreateEmcCluster()
{
  BSEmcCluster *cluster = nullptr;
  std::vector<BSEmcCluster *> result;
  std::sort(fDigiCandidates.begin(), fDigiCandidates.end(), [](const DigiClusteringCandidate &a, const DigiClusteringCandidate &b) { return a.ClusterNumber < b.ClusterNumber; });
  for (auto &digicand : fDigiCandidates) {
    if (digicand.ClusterNumber < (Int_t)result.size()) {
      cluster = result.at(digicand.ClusterNumber);
    } else {
      cluster = new BSEmcCluster{};
      result.push_back(cluster);
    }
    cluster->AddDigi(digicand.DigiIdx, digicand.DetectorId);
  }
  return result;
}
