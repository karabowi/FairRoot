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
// Description:
//	Class Emc2DLocMaxMaxFinder.
//      Searches for local maxima in a cluster based on the ratio
//      between the energy of the maxima crystal and that of
//      its neighbours
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
// Adapted for the PANDA experiment at GSI
//
// Author List:
//      Phil Strother
//      Helmut Schmuecker
//
// Copyright Information:
//	Copyright (C) 1997	            Imperial College
// Modified:
// M. Babai
//------------------------------------------------------------------------

#include "BSEmcLocalMaxFindingProcess.h"

#include <algorithm>
#include <iterator>

#include "FairParSet.h"

#include "PndContainerI.h"
#include "PndMutableContainerI.h"
#include "PndParameterRegister.h"

#include "BSEmcCluster.h"

#include "BSEmcClusteringPar.h"
#include "BSEmcDigi.h"
#include "BSEmcGeoNeighbouringRelationPar.h"

BSEmcLocalMaxFindingProcess::BSEmcLocalMaxFindingProcess()
  : PndProcess{"BSEmcLocalMaxFindingProcess"}, fClusteringParName(""), fNeighbouringRelationParName(""), fMaximumsEnergyThreshold(0), fNeighbouringRelationPar(nullptr)
{
}

BSEmcLocalMaxFindingProcess::~BSEmcLocalMaxFindingProcess() {}

void BSEmcLocalMaxFindingProcess::SetDetectorName(const std::string &t_detectorName)
{
  fDetectorName = t_detectorName;
  if (fClusteringParName == "") {
    fClusteringParName = BSEmcClusteringPar::fgParameterName + fDetectorName;
  }
  if (fNeighbouringRelationParName == "") {
    fNeighbouringRelationParName = BSEmcGeoNeighbouringRelationPar::fgParameterName + fDetectorName;
  }
  fParameterList.push_back(fClusteringParName);
  fParameterList.push_back(fNeighbouringRelationParName);
}

void BSEmcLocalMaxFindingProcess::RequestDataContainer(PndContainerRegister *t_register)
{
  t_register->Request({fDigiBranchname, "BSEmcDigi", kFALSE, kFALSE});
  t_register->Request({fClusterBranchname, "BSEmcCluster", kTRUE, kTRUE});
}

void BSEmcLocalMaxFindingProcess::GetDataContainer(PndContainerRegister *t_register)
{
  fDigiArray = t_register->GetInput<BSEmcDigi>(fDigiBranchname);
  fClusterArray = t_register->GetOutput<BSEmcCluster>(fClusterBranchname);
}

void BSEmcLocalMaxFindingProcess::SetupParameters(const PndParameterRegister *t_paramRegister)
{
  BSEmcClusteringPar *parameter = dynamic_cast<BSEmcClusteringPar *>(t_paramRegister->GetParameter(fClusteringParName));
  fNeighbouringRelationPar = dynamic_cast<BSEmcGeoNeighbouringRelationPar *>(t_paramRegister->GetParameter(fNeighbouringRelationParName));
  fMaximumsEnergyThreshold = parameter->GetMaximumsEnergyThreshold();
}

// void BSEmcLocalMaxFindingProcess::SetupParameters(BSEmcClusteringPar *parameter)
// {
//   fMaximumsEnergyThreshold = parameter->GetMaximumsEnergyThreshold();
//   fEmcStructure = EmcStructure::Instance();
// }

// -------------------------------------------------------------------------
void BSEmcLocalMaxFindingProcess::Process()
{
  FindLocalMaxima(fDigiArray->GetVectorOfPtrToConst(), fClusterArray->GetVector());
}

// -------------------------------------------------------------------------
void BSEmcLocalMaxFindingProcess::FindLocalMaxima(const std::vector<const BSEmcDigi *> &t_digiCont, const std::vector<BSEmcCluster *> &t_clusterCont)
{
  for (BSEmcCluster *cluster : t_clusterCont) {
    std::vector<BSEmcDigiInfo_t> clusterdigis = cluster->GetDigis();

    // std::sort(std::begin(clusterdigis), std::end(clusterdigis), [t_digiCont](const BSEmcDigiInfo_t &left, const BSEmcDigiInfo_t &right) {
    //   return (t_digiCont.at(left.fDigiIdx))->GetEnergy() < (t_digiCont.at(right.fDigiIdx))->GetEnergy();
    // });

    for (auto &clusterDigi : clusterdigis) {
      std::vector<const BSEmcDigi *> neighbours = GetNeigboursOf(t_digiCont, clusterDigi, clusterdigis);
      const BSEmcDigi *digi = t_digiCont.at(clusterDigi.fDigiIdx);

      if (IsLocalMaximum(digi, neighbours)) {
        cluster->AddMaximum(clusterDigi.fDigiIdx, clusterDigi.fDetectorId);
      }
    }
  }
}

// -------------------------------------------------------------------------
std::vector<const BSEmcDigi *> BSEmcLocalMaxFindingProcess::GetNeigboursOf(const std::vector<const BSEmcDigi *> &t_digiCont, const BSEmcDigiInfo_t &t_digi,
                                                                           const std::vector<BSEmcDigiInfo_t> &t_potentialneighbours) const
{
  std::vector<const BSEmcDigi *> neighbours;
  const std::vector<Int_t> &neighbourIds = fNeighbouringRelationPar->GetNeighbourIds(t_digi.fDetectorId);
  for (const auto &candidate : t_potentialneighbours) {
    Bool_t areNeighbours = (std::find(neighbourIds.begin(), neighbourIds.end(), candidate.fDetectorId) != neighbourIds.end());
    if (t_digi.fDetectorId != candidate.fDetectorId && areNeighbours) {
      neighbours.push_back(t_digiCont.at(candidate.fDigiIdx));
    }
  }
  return neighbours;
}

// -------------------------------------------------------------------------
Bool_t BSEmcLocalMaxFindingProcess::IsLocalMaximum(const BSEmcDigi *t_digi, const std::vector<const BSEmcDigi *> &t_neighbours) const
{

  if (t_digi->GetEnergy() < fMaximumsEnergyThreshold) {
    return kFALSE;
  }

  for (const auto &neighbour : t_neighbours) {
    if (neighbour->GetEnergy() > t_digi->GetEnergy()) {
      return kFALSE;
    }
  }

  return kTRUE;
}
