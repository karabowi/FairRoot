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

#include "BSEmcCreateRecoHitProcess.h"

#include "FairLink.h"
#include "FairMultiLinkedData.h"
#include "FairParSet.h"
#include "FairRootManager.h"
#include "fairlogger/Logger.h"

#include "PndContainerI.h"
#include "PndMutableContainerI.h"
#include "PndParameterRegister.h"
#include "PndProcess.h"

#include "BSEmcDigi.h"
#include "BSEmcCluster.h"
#include "BSEmcClusterPropertiesPar.h"

#include "BSEmcClusteringPar.h"
#include "BSEmcCrystalPositionPar.h"
#include "BSEmcDetectorID.h"
#include "BSEmcEnergyCalculationAlgo.h"
#include "BSEmcGeoNeighbouringRelationPar.h"
#include "BSEmcRecoHit.h"
#include "BSEmcSubCluster.h"
#include "BSEmcXClMoments.h"

BSEmcCreateRecoHitProcess::BSEmcCreateRecoHitProcess() : PndProcess{"BSEmcCreateRecoHitProcess"}, fClusteringParName(""), fPositionParName(""), fNeighbouringRelationParName("") {}

BSEmcCreateRecoHitProcess::~BSEmcCreateRecoHitProcess() {}

void BSEmcCreateRecoHitProcess::SetDetectorName(const std::string &t_detectorName)
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

// -------------------------------------------------------------------------
void BSEmcCreateRecoHitProcess::RequestDataContainer(PndContainerRegister *t_register)
{
  t_register->Request({fDigiBranchname, "BSEmcDigi", kFALSE, kFALSE});
  t_register->Request({fClusterBranchname, "BSEmcCluster", kFALSE, kFALSE});
  t_register->Request({fSubClusterBranchname, "BSEmcSubCluster", kFALSE, kFALSE});
  t_register->Request({fRecoHitBranchname, "BSEmcRecoHit", kTRUE, kTRUE});
}

// -------------------------------------------------------------------------
void BSEmcCreateRecoHitProcess::GetDataContainer(PndContainerRegister *t_register)
{
  fDigiArray = t_register->GetInput<BSEmcDigi>(fDigiBranchname);
  fClusterArray = t_register->GetInput<BSEmcCluster>(fClusterBranchname);
  fSubClusterArray = t_register->GetInput<BSEmcSubCluster>(fSubClusterBranchname);
  fRecoHitArray = t_register->GetOutput<BSEmcRecoHit>(fRecoHitBranchname);
  if (fRecoHitBranchname != "") {
    t_register->SetAsDefaultBranchFor(fRecoHitBranchname, "BSEmcRecoHit");
  }

  if (fSubClusterBranchname == "") {
    fSubClusterBranchname = t_register->GetCurrentDefaultBranchName<BSEmcSubCluster>();
  }
}

void BSEmcCreateRecoHitProcess::SetupParameters(const PndParameterRegister *t_parameterRegister)
{
  LOG(debug) << "BSEmcCreateRecoHitProcess::SetupParameters() - " << fDetectorName;
  {
    BSEmcClusteringPar *parameter = dynamic_cast<BSEmcClusteringPar *>(t_parameterRegister->GetParameter(fClusteringParName));
  }
  {
    BSEmcClusterPropertiesPar *parameter = dynamic_cast<BSEmcClusterPropertiesPar *>(t_parameterRegister->GetParameter(fClusterPropertiesParName));
    fPositionAlgo->SetClusterPropertiesPar(parameter);
  }
  {
    BSEmcGeoNeighbouringRelationPar *parameter = dynamic_cast<BSEmcGeoNeighbouringRelationPar *>(t_parameterRegister->GetParameter(fNeighbouringRelationParName));
    fEnergyProcess->SetNeighbouringRelation(parameter);
  }

  {
    BSEmcCrystalPositionPar *parameter = dynamic_cast<BSEmcCrystalPositionPar *>(t_parameterRegister->GetParameter(fPositionParName));
    fMomentsProcess->SetPositionPar(parameter);
    fPositionAlgo->SetPositionPar(parameter);
  }
  LOG(debug) << "BSEmcCreateRecoHitProcess::SetupParameters() done - " << fDetectorName;
}

void BSEmcCreateRecoHitProcess::Process()
{
  fRecoHitArray->Reset();
  std::vector<BSEmcRecoHit *> recoHits = CreateRecoHits(fDigiArray->GetVectorOfPtrToConst(), fClusterArray->GetVectorOfPtrToConst(), fSubClusterArray->GetVectorOfPtrToConst());
  for (BSEmcRecoHit *recoHit : recoHits) {
    fRecoHitArray->CreateCopy(*recoHit);
    delete recoHit;
  }
}

std::vector<BSEmcRecoHit *> BSEmcCreateRecoHitProcess::CreateRecoHits(const std::vector<const BSEmcDigi *> &t_digiCont, const std::vector<const BSEmcCluster *> &t_clusterCont,
                                                                      const std::vector<const BSEmcSubCluster *> &t_subclusterCont) const
{
  std::vector<BSEmcRecoHit *> result;
  Int_t nSubCluster = t_subclusterCont.size();
  LOG(trace) << "BSEmcCreateRecoHitProcess::Process() - " << nSubCluster << " SubCluster to be transformed into BSEmcRecoHit.";
  for (Int_t subClusterIdx = 0; subClusterIdx < nSubCluster; ++subClusterIdx) {
    const BSEmcSubCluster *subcluster = t_subclusterCont.at(subClusterIdx);
    const BSEmcCluster *cluster = t_clusterCont.at(subcluster->GetParentClusterIndex());
    BSEmcRecoHit *recoHit = new BSEmcRecoHit{};
    result.push_back(recoHit);
    recoHit->SetLink(FairLink{fSubClusterBranchname, subClusterIdx});
    recoHit->SetClusterIndex(subcluster->GetParentClusterIndex());
    recoHit->SetSubClusterIndex(subClusterIdx);
    recoHit->SetTimeStamp(subcluster->CalculateTimeStamp(t_digiCont));
    recoHit->SetPosition(fPositionAlgo->CalculatePosition(subcluster, t_digiCont));
    recoHit->SetNumberOfSubClusterCrystals(subcluster->GetNumberOfDigis());
    recoHit->SetNumberOfSubCluster(cluster->GetNumberOfSubCluster());
    recoHit->SetNumberOfClusterCrystals(cluster->GetNumberOfDigis());
    const Int_t detId = cluster->GetDigis()[0].fDetectorId;
    recoHit->SetModule(BSEmcDetectorID{detId}.GetModule());
    recoHit->SetCentralCrystalId(subcluster->GetCentralCrystalId());
    // MC Info
    recoHit->SetMcList(subcluster->GetMcList(FairRootManager::Instance()->GetBranchId("MCTrack"))); // TODO: shift FairRootManager down into the Tasks
    LOG(trace) << "Setting McList for RecoHit: McList size is " << recoHit->GetMcSize();
    LOG(trace) << "Setting Track entering for RecoHit to " << subcluster->GetTrackEntering();
    recoHit->SetTrackEntering(subcluster->GetTrackEntering());
    recoHit->SetTrackExiting(subcluster->GetTrackExiting());
    LOG(trace) << "Setting Track exiting for RecoHit to " << subcluster->GetTrackExiting();

    // Energy
    recoHit->SetRawEnergy(fEnergyProcess->CalculateEnergy(subcluster, t_digiCont));
    recoHit->SetEnergy1(fEnergyProcess->CalculateE1(subcluster, t_digiCont));
    recoHit->SetEnergy9(fEnergyProcess->CalculateE9(subcluster, t_digiCont));
    recoHit->SetEnergy25(fEnergyProcess->CalculateE25(subcluster, t_digiCont));

      // Moments
    Moments moments = fMomentsProcess->CalculateMoments(subcluster, t_digiCont);
    recoHit->SetLateralMoment(moments.Lateral);
    recoHit->SetZ20(moments.Z20);
    recoHit->SetZ53(moments.Z53);
  }
  return result;
}
