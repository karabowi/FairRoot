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
#include "BSEmcMergeRecoHits.h"
#include "BSEmcCrystalPositionPar.h"
#include "BSEmcClusterPropertiesPar.h"
#include "BSEmcDigi.h"
#include "BSEmcRecoHit.h"
#include "BSEmcSubCluster.h"
#include "FairLogger.h"
#include "PndProcess.h"
#include "TF1.h"
#include <string>
#include <limits>

BSEmcMergeRecoHits::BSEmcMergeRecoHits()
  : PndProcess{"BSEmcMergeRecoHits"}, fPositionParName(BSEmcCrystalPositionPar::fgParameterName),

    fNeighbouringRelationParName(BSEmcGeoNeighbouringRelationPar::fgParameterName), fPositionPar(nullptr)
{
}

BSEmcMergeRecoHits::~BSEmcMergeRecoHits() {}

void BSEmcMergeRecoHits::SetDetectorName(const std::string &t_detectorName)
{
  fDetectorName = t_detectorName;
  if (fClusterPropertiesParName == "") {
    fClusterPropertiesParName = BSEmcClusterPropertiesPar::fgParameterName + fDetectorName;
  }
  if (fClusteringParName == "") {
    fClusteringParName = BSEmcClusteringPar::fgParameterName + fDetectorName;
  }
  fPositionParName += fDetectorName;
  fNeighbouringRelationParName += fDetectorName;

  fParameterList.push_back(fClusteringParName);
  fParameterList.push_back(fClusterPropertiesParName);
  fParameterList.push_back(fPositionParName);
  fParameterList.push_back(fNeighbouringRelationParName);
}

void BSEmcMergeRecoHits::RequestDataContainer(PndContainerRegister *t_register)
{
  t_register->Request({fDigiBranchname, "BSEmcDigi", kFALSE, kFALSE});
  t_register->Request({fClusterBranchname, "BSEmcCluster", kFALSE, kFALSE});
  t_register->Request({fSubClusterBranchname, "BSEmcSubCluster", kFALSE, kFALSE});
  t_register->Request({fRecoHitBranchname, "BSEmcRecoHit", kTRUE, kTRUE});
}

void BSEmcMergeRecoHits::GetDataContainer(PndContainerRegister *t_register)
{
  fDigiArray = t_register->GetInput<BSEmcDigi>(fDigiBranchname);
  fClusterArray = t_register->GetInput<BSEmcCluster>(fClusterBranchname);
  fSubClusterArray = t_register->GetInput<BSEmcSubCluster>(fSubClusterBranchname);
  fRecoHitArray = t_register->GetOutput<BSEmcRecoHit>(fRecoHitBranchname);
}

void BSEmcMergeRecoHits::SetupParameters(const PndParameterRegister *t_parameterRegister)
{
  BSEmcClusteringPar *clustering_parameter = dynamic_cast<BSEmcClusteringPar *>(t_parameterRegister->GetParameter(fClusteringParName));
  fClusterEnergyCut = clustering_parameter->GetClusterEnergyCut();

  BSEmcGeoNeighbouringRelationPar *parameter = dynamic_cast<BSEmcGeoNeighbouringRelationPar *>(t_parameterRegister->GetParameter(fNeighbouringRelationParName));

  fEnergyProcess->SetNeighbouringRelation(parameter);

  BSEmcClusterPropertiesPar *cluster_parameter = dynamic_cast<BSEmcClusterPropertiesPar *>(t_parameterRegister->GetParameter(fClusterPropertiesParName));

  fPositionPar = dynamic_cast<BSEmcCrystalPositionPar *>(t_parameterRegister->GetParameter(fPositionParName));
  fPositionProcess->SetClusterPropertiesPar(cluster_parameter);
  fPositionProcess->SetPositionPar(fPositionPar);
  LOG(info) << "BSEmcMergeRecoHits::SetupParameters() done ";
}

double BSEmcMergeRecoHits::CalculateDistance(const TVector3 &firstposition, const TVector3 &secondposition)
{
  return sqrt(pow(firstposition.X() - secondposition.X(), 2) + pow(firstposition.Y() - secondposition.Y(), 2) + pow(firstposition.Z() - secondposition.Z(), 2));
}

Int_t BSEmcMergeRecoHits::FindClosestHit(const BSEmcRecoHit *CurrentRecoHit)
{
  Int_t SubClusterId = -1;
  double distance = std::numeric_limits<double>::max();
  for (const BSEmcRecoHit *MainRecoHit : fRecoHitArray->GetVectorOfPtrToConst()) {
    if (MainRecoHit != nullptr && CurrentRecoHit->GetCentralCrystalId() != MainRecoHit->GetCentralCrystalId() &&
        MainRecoHit->IsSplitOff() == false) { // Ensure not merging into another SplitOff
      const double temp_distance = CalculateDistance(CurrentRecoHit->GetPosition(), MainRecoHit->GetPosition());
      if (temp_distance < distance && MainRecoHit->IsSplitOff() == false) {
        distance = temp_distance;                              // Find closest hit
        if (MainRecoHit->GetRawEnergy() > fClusterEnergyCut) { // Ensure not merging into low enerrgy hit
          SubClusterId = MainRecoHit->GetSubClusterIndex();
        }
      }
    }
  }
  return SubClusterId;
}

void BSEmcMergeRecoHits::FillTmpSubcluster(BSEmcSubCluster *TmpSubCluster, const BSEmcSubCluster *SubCluster)
{
  std::vector<Int_t> usedDigiIds;
  const std::vector<BSEmcDigiInfo_t> SubClusterDigis = SubCluster->GetDigis();
  const std::vector<BSEmcDigiInfo_t> MainSubClusterDigis = TmpSubCluster->GetDigis();
  TmpSubCluster->Clear();
  for (BSEmcDigiInfo_t SubClusterDigi : SubClusterDigis) {
    for (BSEmcDigiInfo_t MainSubClusterDigi : MainSubClusterDigis) {
      if (SubClusterDigi.fDigiIdx == MainSubClusterDigi.fDigiIdx) {                                      //
        const double new_weight = MainSubClusterDigi.fWeight + SubClusterDigi.fWeight;                   // If subclusters share digis
        TmpSubCluster->AddDigi(MainSubClusterDigi.fDigiIdx, MainSubClusterDigi.fDetectorId, new_weight); // add them with combined weight
        usedDigiIds.push_back(SubClusterDigi.fDigiIdx);                                                  //
      }
    }
  }
  for (const BSEmcDigiInfo_t SubClusterDigi : SubClusterDigis) {
    if (std::find(usedDigiIds.begin(), usedDigiIds.end(), SubClusterDigi.fDigiIdx) == usedDigiIds.end()) {             //
      TmpSubCluster->AddDigi(SubClusterDigi.fDigiIdx, SubClusterDigi.fDetectorId, SubClusterDigi.fWeight);             //
    }                                                                                                                  //
  }                                                                                                                    // Add remaining digis
  for (const BSEmcDigiInfo_t MainSubClusterDigi : MainSubClusterDigis) {                                               //
    if (std::find(usedDigiIds.begin(), usedDigiIds.end(), MainSubClusterDigi.fDigiIdx) == usedDigiIds.end()) {         //
      TmpSubCluster->AddDigi(MainSubClusterDigi.fDigiIdx, MainSubClusterDigi.fDetectorId, MainSubClusterDigi.fWeight); //
    }
  }
  TmpSubCluster->SetEnergy(fEnergyProcess->CalculateEnergy(TmpSubCluster, fDigiArray->GetVectorOfPtrToConst()));             //
  const TVector3 newsubClusterpos = fPositionProcess->CalculatePosition(TmpSubCluster, fDigiArray->GetVectorOfPtrToConst()); // Update properties of
  TmpSubCluster->SetPosition(newsubClusterpos);                                                                              // combined subcluster
  TmpSubCluster->SetCentralCrystalId(TmpSubCluster->GetCentralCrystalId());                                                  //
}

void BSEmcMergeRecoHits::UpdateRecoHit(const BSEmcSubCluster *MainSubCluster, const Int_t MainSubClusterId)
{
  for (BSEmcRecoHit *MainRecoHit : fRecoHitArray->GetVector()) {
    if (MainRecoHit != nullptr && MainRecoHit->GetSubClusterIndex() == MainSubClusterId) {
      MainRecoHit->SetRawEnergy(fEnergyProcess->CalculateEnergy(MainSubCluster, fDigiArray->GetVectorOfPtrToConst())); //
      MainRecoHit->SetEnergy1(fEnergyProcess->CalculateE1(MainSubCluster, fDigiArray->GetVectorOfPtrToConst()));       // Update energy info
      MainRecoHit->SetEnergy9(fEnergyProcess->CalculateE9(MainSubCluster, fDigiArray->GetVectorOfPtrToConst()));       // of main RecoHit
      MainRecoHit->SetEnergy25(fEnergyProcess->CalculateE25(MainSubCluster, fDigiArray->GetVectorOfPtrToConst()));     //
      break;
    }
  }
}

void BSEmcMergeRecoHits::CleanUpRecoHitArray()
{
  fRecoHitArray->Compress(); // Compress in order to remove gaps in the container
  for (unsigned int i = 0; i < fRecoHitArray->GetSize(); i++) {
    const BSEmcRecoHit *NewHit = fRecoHitArray->GetConstElementPtr(i);
    if (NewHit->GetRawEnergy() < fClusterEnergyCut) {
      fRecoHitArray->RemoveAt(i); // Remove RecoHits whose energy falls below cluster energy cut
    }
  }
  fRecoHitArray->Compress(); // Compress in order to remove gaps in the container
}

void BSEmcMergeRecoHits::Process()
{
  std::map<const Int_t, std::vector<Int_t>> SplitOffMap;
  for (unsigned int i = 0; i < fRecoHitArray->GetSize(); i++) {
    BSEmcRecoHit *RecoHit = fRecoHitArray->GetElement(i);
    if (RecoHit->IsSplitOff()) {                               //
      Int_t SubClusterId = RecoHit->GetSubClusterIndex();      //
      const Int_t MainSubClusterId = FindClosestHit(RecoHit);  // Create map collecting subcluster id's of all flagged
      if (MainSubClusterId != -1) {                            // RecoHits and the hits they will be merged into
        SplitOffMap[MainSubClusterId].push_back(SubClusterId); //
      }                                                        //
    }
  }
  for (const auto &SplitOff : SplitOffMap) {
    Int_t MainSubClusterId = SplitOff.first;
    std::vector<Int_t> SplitOffVector = SplitOff.second;
    BSEmcSubCluster TmpSubCluster;
    for (int i2 = 0; i2 < fSubClusterArray->GetSize(); i2++) {
      if (i2 == MainSubClusterId) {
        TmpSubCluster = *fSubClusterArray->GetConstElementPtr(i2); // Fetch subcluster to be merged into
      }
    }
    for (Int_t SplitOffId : SplitOffVector) {
      for (int i2 = 0; i2 < fSubClusterArray->GetSize(); i2++) {
        if (i2 == SplitOffId) {
          FillTmpSubcluster(&TmpSubCluster, fSubClusterArray->GetConstElementPtr(i2)); // Add SplitOff subclusters to main subcluster
          fRecoHitArray->RemoveAt(SplitOffId);                                         // Remove the RecoHit marked as SplitOff
        }
      }
    }
    UpdateRecoHit(&TmpSubCluster, MainSubClusterId); // Update RecoHit with the combined information
  }
  CleanUpRecoHitArray(); // Remove gaps in the container and apply energy cut
}
