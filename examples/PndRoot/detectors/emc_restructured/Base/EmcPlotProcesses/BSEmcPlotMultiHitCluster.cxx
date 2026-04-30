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

#include "BSEmcPlotMultiHitCluster.h"

#include <algorithm>
#include <string>
#include <utility>

#include "TH2.h"
#include "TList.h"
#include "TText.h"

#include "PndContainerI.h"

#include "BSEmc2DMapper.h"
#include "BSEmcCluster.h"
#include "BSEmcDigi.h"
#include "BSEmcEnergies_t.h"
#include "BSEmcHistogrammer.h"
#include "BSEmcRecoHit.h"
#include "BSEmcSubCluster.h"

BSEmcPlotMultiHitCluster::BSEmcPlotMultiHitCluster(BSEmc2DMapper *t_mapper) : BSEmcPlotProcess("BSEmcPlotMultiHitCluster"), fMapper(t_mapper) {}

BSEmcPlotMultiHitCluster::~BSEmcPlotMultiHitCluster()
{
  delete fMapper;
}

void BSEmcPlotMultiHitCluster::RequestDataContainer(PndContainerRegister *t_register)
{
  // Lets use the defaults set by the task
  t_register->Request({"", "BSEmcDigi", kFALSE, kFALSE});
  t_register->Request({"", "BSEmcCluster", kFALSE, kFALSE});
  t_register->Request({"", "BSEmcSubCluster", kFALSE, kFALSE});
  t_register->Request({"", "BSEmcRecoHit", kFALSE, kFALSE});
}

void BSEmcPlotMultiHitCluster::GetDataContainer(PndContainerRegister *t_register)
{
  fDigiArray = t_register->GetDefaultBranch<BSEmcDigi>();
  fClusterArray = t_register->GetDefaultBranch<BSEmcCluster>();
  fSubClusterArray = t_register->GetDefaultBranch<BSEmcSubCluster>();
  fRecoHitArray = t_register->GetDefaultBranch<BSEmcRecoHit>();
}

void BSEmcPlotMultiHitCluster::InitHistos(const TString &t_folderprefix)
{
  SetFolderPrefix(t_folderprefix);
  TString foldername = t_folderprefix + "/MultiHitCluster";
  fHistogrammer->Create1DHist("MultiHitClusterNumber", foldername, {100, -0.5, 99.5, "Number of multi hit cluster per Event"});
  fHistogrammer->Create2DHist("MultiHitClusterHitEnergyFraction", foldername, {9, -0.5, 8.5, "Number of Neighbours"},
                              {200, 0, 2, "Fraction Max Neighbour Energy / Central Crystal Energy"});
}

void BSEmcPlotMultiHitCluster::Create2DHistOfClusterDigis(const BSEmcCluster *t_cluster)
{
  TString clusterEventName = "MultiHitClusterEvent_" + std::to_string(fEvent);
  fHistogrammer->Create2DHist(clusterEventName, fFolderprefix + "/MultiHitCluster/EventDisplay", {100, -0.5, 99.5, "Theta Index"}, {160, -0.5, 159.5, "Phi Index"});
  for (const BSEmcDigiInfo_t &digiInfo : t_cluster->GetDigis()) {
    const BSEmcDigi *digi = fDigiArray->GetConstElementPtr(digiInfo.fDigiIdx);
    std::pair<Int_t, Int_t> xy = fMapper->GetXYIndices(digiInfo.fDetectorId);
    fHistogrammer->Fill(clusterEventName, xy.first, xy.second, digi->GetEnergy());
  }
  for (const BSEmcDigiInfo_t &maxInfo : t_cluster->GetMaximaDigis()) {
    std::pair<Int_t, Int_t> xy = fMapper->GetXYIndices(maxInfo.fDetectorId);
    fHistogrammer->Get2DHist(clusterEventName)->GetListOfFunctions()->Add(new TText(xy.first - 0.25, xy.second - 0.25, "X"));
  }
}

std::vector<const BSEmcRecoHit *> BSEmcPlotMultiHitCluster::GetRecoHitsInCluster(Int_t t_clusterindex)
{
  std::vector<const BSEmcRecoHit *> clusterHits;
  for (const BSEmcRecoHit *recoHit : fRecoHitArray->GetVectorOfPtrToConst()) {
    Int_t subclusterIndex = recoHit->GetSubClusterIndex();
    const BSEmcSubCluster *subcluster = fSubClusterArray->GetConstElementPtr(subclusterIndex);
    if (subcluster->GetParentClusterIndex() == t_clusterindex) {
      clusterHits.push_back(recoHit);
    }
  }
  return clusterHits;
}

void BSEmcPlotMultiHitCluster::FillMaxNeighbourEnergyOverCentralEnergy(Int_t t_clusterindex)
{
  std::vector<const BSEmcRecoHit *> clusterHits = GetRecoHitsInCluster(t_clusterindex);
  std::sort(clusterHits.begin(), clusterHits.end(), [](const BSEmcRecoHit *a, const BSEmcRecoHit *b) { return a->GetRawEnergy() < b->GetRawEnergy(); });
  for (const auto *clusterHit : clusterHits) {
    BSEmcEnergies_t energies = BSEmcEnergies_t::GetEnergies(clusterHit->GetCentralCrystalId(), fDigiArray->GetVectorOfPtrToConst(), fNeighbouringRelationPar);
    fHistogrammer->Fill("MultiHitClusterHitEnergyFraction", energies.NeighbourNumber, energies.MaxRatio(), 1);
  }
}

void BSEmcPlotMultiHitCluster::Process()
{
  Int_t multihitclusternumber = 0;
  Int_t clusterindex = 0;
  for (const BSEmcCluster *cluster : fClusterArray->GetVectorOfPtrToConst()) {
    Int_t numberOfClusterHits = cluster->GetNumberOfSubCluster();
    if (numberOfClusterHits > 1) {
      Create2DHistOfClusterDigis(cluster);
      FillMaxNeighbourEnergyOverCentralEnergy(clusterindex);
      ++multihitclusternumber;
    }
    ++clusterindex;
  }
  fHistogrammer->Fill("MultiHitClusterNumber", multihitclusternumber, 1);
  ++fEvent;
}
