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

#include "BSEmcEnergyCalculationAlgo.h"

#include <algorithm>
#include <vector>

#include "BSEmcCluster.h"
#include "BSEmcDigi.h"
#include "BSEmcGeoNeighbouringRelationPar.h"

BSEmcEnergyCalculationAlgo::BSEmcEnergyCalculationAlgo() : fNeighbouringRelationPar(nullptr) {}

BSEmcEnergyCalculationAlgo::~BSEmcEnergyCalculationAlgo() {}

Double_t BSEmcEnergyCalculationAlgo::CalculateEnergy(const BSEmcCluster *t_cluster, const std::vector<const BSEmcDigi *> &t_digis) const
{
  Double_t sum = 0.0;
  for (const auto &digiInfo : t_cluster->GetDigis()) {
    const BSEmcDigi *digi = t_digis.at(digiInfo.fDigiIdx);

    sum += digi->GetEnergy() * digiInfo.fWeight;
  }
  return sum;
}

Double_t BSEmcEnergyCalculationAlgo::CalculateE1(const BSEmcCluster *t_cluster, const std::vector<const BSEmcDigi *> &t_digis) const
{
  return (t_cluster->GetMaximumEnergy(t_digis));
}

Double_t BSEmcEnergyCalculationAlgo::CalculateE9(const BSEmcCluster *t_cluster, const std::vector<const BSEmcDigi *> &t_digis) const
{
  Double_t sum = (t_cluster->GetMaximumEnergy(t_digis));
  const BSEmcDigi *maxDigi = t_cluster->GetMaximumDigi(t_digis);
  const std::vector<Int_t> &max_neighbours = fNeighbouringRelationPar->GetNeighbourIds(maxDigi->GetDetectorId());
  std::vector<BSEmcDigiInfo_t> digiList = t_cluster->GetDigis();

  for (const BSEmcDigiInfo_t &digiInfo : digiList) {
    const BSEmcDigi *digi = t_digis.at(digiInfo.fDigiIdx);
    if (digi != maxDigi) {
      if (std::find(max_neighbours.begin(), max_neighbours.end(), digi->GetDetectorId()) != max_neighbours.end()) {
        sum += digi->GetEnergy() * digiInfo.fWeight;
      }
    }
  }
  return sum;
}

Double_t BSEmcEnergyCalculationAlgo::CalculateE25(const BSEmcCluster *t_cluster, const std::vector<const BSEmcDigi *> &t_digis) const
{

  Double_t sum = CalculateE9(t_cluster, t_digis);
  const BSEmcDigi *maxDigi = t_cluster->GetMaximumDigi(t_digis);
  std::vector<BSEmcDigiInfo_t> digiList = t_cluster->GetDigis();
  std::vector<BSEmcDigiInfo_t> visitedDigis;
  for (const BSEmcDigiInfo_t &digiInfoA : digiList) {
    const BSEmcDigi *digiA = t_digis.at(digiInfoA.fDigiIdx);
    if (digiA != maxDigi) {
      if (fNeighbouringRelationPar->AreNeighbours(digiA->GetDetectorId(), maxDigi->GetDetectorId())) {

        for (const BSEmcDigiInfo_t &digiInfoB : digiList) {
          const BSEmcDigi *digiB = t_digis.at(digiInfoB.fDigiIdx);
          if (digiB != maxDigi && !fNeighbouringRelationPar->AreNeighbours(digiB->GetDetectorId(), maxDigi->GetDetectorId()) &&
              fNeighbouringRelationPar->AreNeighbours(digiB->GetDetectorId(), digiA->GetDetectorId())) {
            auto foundpos = std::find(visitedDigis.begin(), visitedDigis.end(), digiInfoB);
            if (foundpos == visitedDigis.end()) {
              sum += digiInfoB.fWeight * digiB->GetEnergy();
              visitedDigis.push_back(digiInfoB);
            }
          }
        }
      }
    }
  }
  return sum;
}
