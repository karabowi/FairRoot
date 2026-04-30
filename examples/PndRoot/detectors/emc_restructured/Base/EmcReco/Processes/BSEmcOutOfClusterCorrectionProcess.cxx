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

#include "BSEmcOutOfClusterCorrectionProcess.h"

#include <algorithm>
#include <math.h>
#include <numeric>

#include "TMath.h"
#include "TVector3.h"

#include "FairParSet.h"
#include "fairlogger/Logger.h"

#include "PndMutableContainerI.h"
#include "PndParameterRegister.h"

#include "BSEmcCluster.h"

#include "BSEmcClusteringPar.h"
#include "BSEmcRecoHit.h"

BSEmcOutOfClusterCorrectionProcess::BSEmcOutOfClusterCorrectionProcess() : PndProcess{"BSEmcOutOfClusterCorrectionProcess"}, fClusteringParName("") {}

BSEmcOutOfClusterCorrectionProcess::~BSEmcOutOfClusterCorrectionProcess() {}

void BSEmcOutOfClusterCorrectionProcess::SetDetectorName(const std::string &t_detectorName)
{
  fDetectorName = t_detectorName;
  if (fClusteringParName == "") {
    fClusteringParName = BSEmcClusteringPar::fgParameterName + fDetectorName;
  }
  fParameterList.push_back(fClusteringParName);
}

// -------------------------------------------------------------------------
void BSEmcOutOfClusterCorrectionProcess::RequestDataContainer(PndContainerRegister *t_register)
{
  t_register->Request({fClusterBranchname, "BSEmcCluster", kFALSE, kFALSE});
  t_register->Request({fRecoHitBranchname, "BSEmcRecoHit", kTRUE, kTRUE});
}

// -------------------------------------------------------------------------
void BSEmcOutOfClusterCorrectionProcess::GetDataContainer(PndContainerRegister *t_register)
{
  fClusterArray = t_register->GetInput<BSEmcCluster>(fClusterBranchname);
  fRecoHitArray = t_register->GetOutput<BSEmcRecoHit>(fRecoHitBranchname);
}

void BSEmcOutOfClusterCorrectionProcess::SetupParameters(const PndParameterRegister *t_parameterRegister)
{
  BSEmcClusteringPar *parameter = dynamic_cast<BSEmcClusteringPar *>(t_parameterRegister->GetParameter(fClusteringParName));
  fMinClusterEnergy = parameter->GetClusterEnergyCut();
  fSearchConeAngle = parameter->GetSearchConeAngle() / TMath::RadToDeg();
  fClusterActiveTime = parameter->GetClusterActiveTime();

  LOG(debug) << "BSEmcOutOfClusterCorrectionProcess::Init: "
             << "fMinClusterEnergy: " << fMinClusterEnergy << ", fSearchConeAngle: " << fSearchConeAngle << ", fClusterActiveTime: " << fClusterActiveTime;
}

void BSEmcOutOfClusterCorrectionProcess::Process()
{
  CorrectingRecoHitsForLossOfLowEnergyCluster(fClusterArray->GetVectorOfPtrToConst(), fRecoHitArray->GetVector());
}

void BSEmcOutOfClusterCorrectionProcess::CorrectingRecoHitsForLossOfLowEnergyCluster(const std::vector<const BSEmcCluster *> &t_clusterCont,
                                                                                     const std::vector<BSEmcRecoHit *> &t_recoHitsCont) const
{
  for (const BSEmcCluster *cluster : t_clusterCont) {
    if (cluster->GetEnergy() < fMinClusterEnergy) {
      std::vector<BSEmcRecoHit *> nearestHits = FindClosestRecoHits(cluster, t_recoHitsCont);
      UpdateNearestRecoHitEnergies(cluster, nearestHits);
    }
  }
}

std::vector<BSEmcRecoHit *> BSEmcOutOfClusterCorrectionProcess::FindClosestRecoHits(const BSEmcCluster *t_cluster, const std::vector<BSEmcRecoHit *> &t_recoHitsCont) const
{
  std::vector<BSEmcRecoHit *> nearestHits;
  for (BSEmcRecoHit *ahit : t_recoHitsCont) {
    if (ahit != nullptr) {
      Double_t angle = t_cluster->GetPosition().Angle(ahit->GetPosition());
      LOG(debug3) << "BSEmcOutOfClusterCorrectionProcess::FindClosestRecoHits() - Found angle: " << angle;

      if (angle < fSearchConeAngle && std::abs(t_cluster->GetTimeStamp() - ahit->GetTimeStamp()) < fClusterActiveTime) {
        nearestHits.push_back(ahit);
      }
    }
  }
  return nearestHits;
}

void BSEmcOutOfClusterCorrectionProcess::UpdateNearestRecoHitEnergies(const BSEmcCluster *t_cluster, std::vector<BSEmcRecoHit *> &t_nearestHits) const
{
  std::sort(t_nearestHits.begin(), t_nearestHits.end(), [](const BSEmcRecoHit *a, const BSEmcRecoHit *b) { return a->GetRawEnergy() < b->GetRawEnergy(); });

  if (t_nearestHits.size() > 1) {
    Double_t totalEnergy = std::accumulate(t_nearestHits.begin(), t_nearestHits.end(), 0.0, [](Double_t sum, const BSEmcRecoHit *hit) { return sum + hit->GetRawEnergy(); });

    for (BSEmcRecoHit *hit : t_nearestHits) {
      hit->SetRawEnergy(hit->GetRawEnergy() * (1 + t_cluster->GetEnergy() / totalEnergy));
    }

  } else if (t_nearestHits.size() == 1) {
    t_nearestHits[0]->SetRawEnergy(t_cluster->GetEnergy() + t_nearestHits[0]->GetRawEnergy());
  } else {
    return;
  }

  return;
}
