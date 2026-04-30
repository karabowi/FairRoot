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

#include "BSEmcClusteringPar.h"

#include "FairParamList.h"

ClassImp(BSEmcClusteringPar);
const std::string BSEmcClusteringPar::fgParameterName = "EmcClusteringPar";
BSEmcClusteringPar::BSEmcClusteringPar(const char *t_name, const char *t_title, const char *t_context) : FairParGenericSet(t_name, t_title, t_context) {}

BSEmcClusteringPar::~BSEmcClusteringPar(void) {}

void BSEmcClusteringPar::clear(void) {}

void BSEmcClusteringPar::putParams(FairParamList *t_list)
{
  if (t_list == nullptr) {
    return;
  }
  // MakeDigiCluster
  t_list->add("DigiEnergyThreshold", fDigiEnergyThreshold);
  t_list->add("ClusterActiveTime", fClusterActiveTime);
  // LocalMaxFinder
  t_list->add("MaximumsEnergyThreshold", fMaximumsEnergyThreshold);
  t_list->add("CutSlope", fCutSlope);
  t_list->add("CutOffset", fCutOffset);
  t_list->add("ERatioCorr", fERatioCorr);
  t_list->add("NeighbourECut", fNeighbourECut);
  // ExpClusterSplitter
  t_list->add("MoliereRadius", fMoliereRadius);
  t_list->add("ExponentialConstant", fExponentialConstant);
  t_list->add("MaxIterations", fMaxIterations);
  t_list->add("CentroidShift", fCentroidShift);
  t_list->add("MaxSubClusters", fMaxSubClusters);
  t_list->add("MinDigiEnergy", fMinDigiEnergy);

  t_list->add("ClusterEnergyCut", fClusterEnergyCut);
  t_list->add("SearchConeAngle", fSearchConeAngle);
}

Bool_t BSEmcClusteringPar::getParams(FairParamList *t_list)
{
  if (t_list == nullptr) {
    return kFALSE;
  }
  if (!t_list->fill("DigiEnergyThreshold", &fDigiEnergyThreshold)) {
    return kFALSE;
  }
  if (!t_list->fill("ClusterActiveTime", &fClusterActiveTime)) {
    return kFALSE;
  }

  if (!t_list->fill("MaximumsEnergyThreshold", &fMaximumsEnergyThreshold)) {
    return kFALSE;
  }
  if (!t_list->fill("CutSlope", &fCutSlope)) {
    return kFALSE;
  }
  if (!t_list->fill("CutOffset", &fCutOffset)) {
    return kFALSE;
  }
  if (!t_list->fill("ERatioCorr", &fERatioCorr)) {
    return kFALSE;
  }
  if (!t_list->fill("NeighbourECut", &fNeighbourECut)) {
    return kFALSE;
  }

  if (!t_list->fill("MoliereRadius", &fMoliereRadius)) {
    return kFALSE;
  }
  if (!t_list->fill("ExponentialConstant", &fExponentialConstant)) {
    return kFALSE;
  }
  if (!t_list->fill("MaxIterations", &fMaxIterations)) {
    return kFALSE;
  }
  if (!t_list->fill("CentroidShift", &fCentroidShift)) {
    return kFALSE;
  }
  if (!t_list->fill("MaxSubClusters", &fMaxSubClusters)) {
    return kFALSE;
  }
  if (!t_list->fill("MinDigiEnergy", &fMinDigiEnergy)) {
    return kFALSE;
  }
  if (!t_list->fill("ClusterEnergyCut", &fClusterEnergyCut)) {
    return kFALSE;
  }
  if (!t_list->fill("SearchConeAngle", &fSearchConeAngle)) {
    return kFALSE;
  }

  return kTRUE;
}
