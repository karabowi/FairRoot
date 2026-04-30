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
//                                            ^
//                                            |  ....             /
//                                         1.0|   ....           /|
//                                            |                 / | fCutSlope
//                                            |  splitoffs     /  |
//          MaxE of neighbours - fERatioCorr  |    and        /___|
// ERatio =  -------------------------------  |  hadrons     /..
//                    MaxE-fERatioCorr        |             / ..
//                                            |  ....      /  ..
//                                            |   ....    /  ...  <-- merged pions
//                                            |          /  ....      and photons
//                                            |  ...    /   ....
//                                         0.0|        /    ....
//                                            |------------------->
//                                             0     /     6 7 8
//                                            <------->             number of neighbours
//                                            fCutOffset             with energy > fNeighbourECut

#include "BSEmcRemoveSplitOffMax.h"

#include <algorithm>

#include "FairParSet.h"

#include "PndContainerI.h"
#include "PndMutableContainerI.h"
#include "PndParameterRegister.h"

#include "BSEmcCluster.h"

#include "BSEmcClusteringPar.h"
#include "BSEmcDigi.h"
#include "BSEmcGeoNeighbouringRelationPar.h"

// -------------------------------------------------------------------------
BSEmcRemoveSplitOffMax::BSEmcRemoveSplitOffMax()
  : PndProcess{"BSEmcRemoveSplitOffMax"}, fClusteringParName(""), fNeighbouringRelationParName(""), fDigiArray(nullptr), fClusterArray(nullptr), fCutSlope(0), fCutOffset(0),
    fERatioCorr(0), fNeighbourECut(0), fNeighbouringRelationPar(nullptr)
{
}

// -------------------------------------------------------------------------
BSEmcRemoveSplitOffMax::~BSEmcRemoveSplitOffMax() {}

// -------------------------------------------------------------------------
void BSEmcRemoveSplitOffMax::SetDetectorName(const std::string &t_detectorName)
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

// -------------------------------------------------------------------------
void BSEmcRemoveSplitOffMax::RequestDataContainer(PndContainerRegister *t_register)
{
  t_register->Request({fDigiBranchname, "BSEmcDigi", kFALSE, kFALSE});
  t_register->Request({fClusterBranchname, "BSEmcCluster", kTRUE, kTRUE});
}

// -------------------------------------------------------------------------
void BSEmcRemoveSplitOffMax::GetDataContainer(PndContainerRegister *t_register)
{
  fDigiArray = t_register->GetInput<BSEmcDigi>(fDigiBranchname);
  fClusterArray = t_register->GetOutput<BSEmcCluster>(fClusterBranchname);
}

// -------------------------------------------------------------------------
void BSEmcRemoveSplitOffMax::SetupParameters(const PndParameterRegister *t_paramRegister)
{
  BSEmcClusteringPar *parameter = dynamic_cast<BSEmcClusteringPar *>(t_paramRegister->GetParameter(fClusteringParName));
  fCutSlope = parameter->GetCutSlope();
  fCutOffset = parameter->GetCutOffset();
  fERatioCorr = parameter->GetERatioCorr();
  fNeighbourECut = parameter->GetNeighbourECut();
  fMaximumsEnergyThreshold = parameter->GetMaximumsEnergyThreshold();
  fNeighbouringRelationPar = dynamic_cast<BSEmcGeoNeighbouringRelationPar *>(t_paramRegister->GetParameter(fNeighbouringRelationParName));
}

// -------------------------------------------------------------------------
void BSEmcRemoveSplitOffMax::Process()
{
  RemoveSplitOffMax(fDigiArray->GetVectorOfPtrToConst(), fClusterArray->GetVector());
}

// -------------------------------------------------------------------------
void BSEmcRemoveSplitOffMax::RemoveSplitOffMax(const std::vector<const BSEmcDigi *> &t_digiCont, const std::vector<BSEmcCluster *> &t_clusterCont)
{
  for (BSEmcCluster *cluster : t_clusterCont) {
    std::vector<BSEmcDigiInfo_t> maximaDigis = cluster->GetMaximaDigis();
    if (maximaDigis.size() > 0) {
      for (auto &clusterDigi : maximaDigis) {
        std::vector<const BSEmcDigi *> neighbours = GetNeigboursOf(t_digiCont, clusterDigi, cluster->GetDigis());
        const BSEmcDigi *digi = t_digiCont.at(clusterDigi.fDigiIdx);
        if (IsSplitOffOrHadron(digi, neighbours)) {
          cluster->RemoveMaximum(clusterDigi.fDigiIdx);
        }
      }
    }
    if (cluster->GetMaximaDigis().size() == 0) {
      const auto &digis = cluster->GetDigis();
      BSEmcDigiInfo_t maxDigiInfo = digis[0];
      Double_t maxE = 0;
      for (auto &clusterDigiInfo : digis) {
        const BSEmcDigi *digi = t_digiCont.at(clusterDigiInfo.fDigiIdx);
        if (digi->GetEnergy() > maxE) {
          maxE = digi->GetEnergy();
          maxDigiInfo = clusterDigiInfo;
        }
      }
      if (t_digiCont.at(maxDigiInfo.fDigiIdx)->GetEnergy() > fMaximumsEnergyThreshold) {
        cluster->AddMaximum(maxDigiInfo);
      }
    }
  }
}

// -------------------------------------------------------------------------
std::vector<const BSEmcDigi *> BSEmcRemoveSplitOffMax::GetNeigboursOf(const std::vector<const BSEmcDigi *> &t_digiCont, const BSEmcDigiInfo_t &t_digi,
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
Bool_t BSEmcRemoveSplitOffMax::IsSplitOffOrHadron(const BSEmcDigi *t_digi, const std::vector<const BSEmcDigi *> &t_neighbours) const
{
  Double_t numberOFneighbours{0.0};
  Double_t neighbourMaxE{0.0};

  for (const auto &neighbour : t_neighbours) {
    if (neighbour->GetEnergy() > neighbourMaxE) {
      neighbourMaxE = neighbour->GetEnergy();
    }
    if (neighbour->GetEnergy() > fNeighbourECut) {
      numberOFneighbours += 1.0;
    }
  }

  if (numberOFneighbours == 0.0) {
    return kTRUE;
  }

  if ((fCutSlope * (numberOFneighbours - fCutOffset)) < (neighbourMaxE - fERatioCorr) / (t_digi->GetEnergy() - fERatioCorr)) {
    return kTRUE;
  }

  return kFALSE;
}
