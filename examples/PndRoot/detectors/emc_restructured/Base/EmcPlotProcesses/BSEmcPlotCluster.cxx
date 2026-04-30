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

#include "BSEmcPlotCluster.h"

#include <memory>
#include <vector>

#include "TMath.h"
#include "TObject.h"
#include "TVector3.h"

#include "FairMultiLinkedData.h"
#include "FairRootManager.h"
#include "fairlogger/Logger.h"

#include "PndContainerI.h"
#include "PndMCTrack.h"

#include "BSEmcCluster.h"
#include "BSEmcDigi.h"
#include "BSEmcHistogrammer.h"

void BSEmcPlotCluster::RequestDataContainer(PndContainerRegister *t_register)
{
  // Lets use the defaults set by the task
  t_register->Request({"", "BSEmcDigi", kFALSE, kFALSE});
  t_register->Request({"", "BSEmcCluster", kFALSE, kFALSE});
}

void BSEmcPlotCluster::GetDataContainer(PndContainerRegister *t_register)
{
  fDigiArray = t_register->GetDefaultBranch<BSEmcDigi>();
  fClusterArray = t_register->GetDefaultBranch<BSEmcCluster>();
}

void BSEmcPlotCluster::InitHistos(const TString &t_folderprefix)
{
  SetFolderPrefix(t_folderprefix);
  TString foldername = t_folderprefix + "/ControlHistos/Cluster";
  fHistogrammer->Create1DHist("ClusterNumber", foldername, {20, -0.5, 19.5, "Number of reco hits per Event"});
  fHistogrammer->Create1DHist("ClusterTime", foldername, {500, 0, 50, "Time / [ns]"});
  fHistogrammer->Create1DHist("ClusterMaxTime", foldername, {500, 0, 50, "Time / [ns]"});
  fHistogrammer->Create1DHist("ClusterMaxTimeDiff", foldername, {500, 0, 50, "Time / [ns]"});
  fHistogrammer->Create1DHist("ClusterEnergy", foldername, {1600, 0, 16, "Energy / [GeV]"});
  fHistogrammer->Create1DHist("ClusterMaxEnergy", foldername, {1600, 0, 16, "Energy / [GeV]"});
  fHistogrammer->Create1DHist("TotalClusterEnergy", foldername, {1600, 0, 16, "Energy / [GeV]"});
  fHistogrammer->Create1DHist("ClusterSize", foldername, {100, -0.5, 99.5, "Subc Clustersize"});
  fHistogrammer->Create1DHist("ClusterSubClusterNumber", foldername, {100, -0.5, 99.5, "Number of Subcluster"});
  fHistogrammer->Create1DHist("ClusterMaximaNumber", foldername, {100, -0.5, 99.5, "Number of Maxima"});
  fHistogrammer->Create1DHist("ClusterMaximaSubClusterDiff", foldername, {99, -49.5, 49.5, "Number of Maxima - Number of Subcluster"});

  fHistogrammer->Create2DHist("ClusterPosition", foldername, {360, 0, 180, "Theta / [Deg]"}, {720, 0, 360, "Phi / [Deg]"});
  fHistogrammer->Create2DHist("ClusterEnergyVsClusterSize", foldername, {1600, 0, 16, "Energy / [GeV]"}, {100, -0.5, 99.5, "Subclustersize"});
  fHistogrammer->Create2DHist("ClusterEnergyVsSubClusterNumber", foldername, {1600, 0, 16, "Energy / [GeV]"}, {100, -0.5, 99.5, "Subclustersize"});

  fHistogrammer->Create1DHist("ClusterPhiDiff", foldername, {720, -180, 180, "Phi difference (reco. Phi - gen. Phi"});
  fHistogrammer->Create1DHist("ClusterThetaDiff", foldername, {360, -90, 90, "Theta difference (reco. Theta - gen. Theta"});
  fHistogrammer->Create2DHist("ClusterPhiVsMCPhi", foldername, {720, 0, 360, "gen. Phi"}, {720, 0, 360, "reco. Phi"});
  fHistogrammer->Create2DHist("ClusterThetaVsMCTheta", foldername, {360, 0, 180, "gen. Theta"}, {360, 0, 3180, "reco. Theta"});

  fHistogrammer->Create1DHist("ClusterEnteringTracks", foldername, {100, -0.5, 99.5, "Number of entering tracks"});
  fHistogrammer->Create1DHist("ClusterExitingTracks", foldername, {200, -0.5, 199.5, "Number of exiting tracks"});
}

Int_t BSEmcPlotCluster::GetMaxDigiIndex(const BSEmcCluster *t_cluster) const
{
  Int_t maximumdigi = 0;
  Double_t maxenergy = 0, currentenergy = 0;
  for (const BSEmcDigiInfo_t &digiInfo : t_cluster->GetMaximaDigis()) {
    const BSEmcDigi *digi = fDigiArray->GetConstElementPtr(digiInfo.fDigiIdx);
    currentenergy = digi->GetRawEnergy();
    if (currentenergy > maxenergy) {
      maximumdigi = digiInfo.fDigiIdx;
      maxenergy = currentenergy;
    }
  }
  return maximumdigi;
}

Double_t BSEmcPlotCluster::GetMaxTimeDiff(const BSEmcCluster *t_cluster) const
{
  const BSEmcDigi *maxdigi = fDigiArray->GetConstElementPtr(GetMaxDigiIndex(t_cluster));
  Double_t maxtime = maxdigi->GetTimeStamp();
  Double_t timediff = 0, currentdiff = 0, currenttime = 0;
  for (const BSEmcDigiInfo_t &digiInfo : t_cluster->GetDigis()) {
    const BSEmcDigi *currentdigi = fDigiArray->GetConstElementPtr(digiInfo.fDigiIdx);
    currenttime = currentdigi->GetTimeStamp();
    currentdiff = maxtime - currenttime;
    if (timediff < currentdiff) {
      timediff = currentdiff;
    }
  }
  return timediff;
}

void BSEmcPlotCluster::Process()
{

  fHistogrammer->Fill("ClusterNumber", fClusterArray->GetSize(), 1);
  Double_t totalenergy = 0;
  Int_t clustercounter = 1;
  for (const BSEmcCluster *cluster : fClusterArray->GetVectorOfPtrToConst()) {
    totalenergy += cluster->GetEnergy();
    const BSEmcDigi *maxdigi = fDigiArray->GetConstElementPtr(GetMaxDigiIndex(cluster));
    fHistogrammer->Fill("ClusterTime", cluster->GetTimeStamp(), 1);
    fHistogrammer->Fill("ClusterMaxTime", maxdigi->GetTimeStamp(), 1);
    fHistogrammer->Fill("ClusterMaxTimeDiff", GetMaxTimeDiff(cluster), 1);

    fHistogrammer->Fill("ClusterEnergy", cluster->GetEnergy(), 1);
    fHistogrammer->Fill("ClusterMaxEnergy", maxdigi->GetEnergy(), 1);
    fHistogrammer->Fill("ClusterSize", cluster->GetDigis().size(), 1);
    fHistogrammer->Fill("ClusterSubClusterNumber", cluster->GetNumberOfSubCluster(), 1);
    fHistogrammer->Fill("ClusterMaximaNumber", cluster->GetMaximaDigis().size(), 1);
    fHistogrammer->Fill("ClusterMaximaSubClusterDiff", cluster->GetMaximaDigis().size() - cluster->GetNumberOfSubCluster(), 1);
    if (cluster->GetMaximaDigis().size() - cluster->GetNumberOfSubCluster() != 0) {
      LOG(debug) << "BSEmcPlotCluster::Process() - Event " << fEvent << " cluster " << clustercounter << " has Maxima-SubCluster Diff of "
                 << cluster->GetMaximaDigis().size() - cluster->GetNumberOfSubCluster() << ": Maxima - " << cluster->GetMaximaDigis().size() << ", subcluster - "
                 << cluster->GetNumberOfSubCluster();
    }
    fHistogrammer->Fill("ClusterEnergyVsClusterSize", cluster->GetEnergy(), cluster->GetNumberOfDigis(), 1);
    fHistogrammer->Fill("ClusterEnergyVsSubClusterNumber", cluster->GetEnergy(), cluster->GetNumberOfSubCluster(), 1);

    fHistogrammer->Fill("ClusterEnteringTracks", cluster->GetTrackEntering().GetNLinks(), 1);
    fHistogrammer->Fill("ClusterExitingTracks", cluster->GetTrackExiting().GetNLinks(), 1);

    TVector3 pos = cluster->GetPosition();
    Double_t theta = GetThetaOfInDeg(pos);
    Double_t phi = GetPhiOfInDeg(pos);
    fHistogrammer->Fill("ClusterPosition", theta, phi, 1);

    // if MCTruth info exists (assuming single primary particle)

    if (cluster->GetTrackEntering().GetNLinks() > 0) {
      std::unique_ptr<PndMCTrack> primarytrack =
        std::unique_ptr<PndMCTrack>(dynamic_cast<PndMCTrack *>(FairRootManager::Instance()->GetCloneOfLinkData(cluster->GetTrackEntering().GetLink(0))));
      TVector3 momentum = primarytrack->GetMomentum();
      Double_t mctheta = momentum.Theta() * TMath::RadToDeg();
      Double_t mcphi = momentum.Phi();
      if (mcphi < 0) {
        mcphi += 2 * TMath::Pi();
      }
      mcphi *= TMath::RadToDeg();

      fHistogrammer->Fill("ClusterPhiDiff", phi - mcphi, 1);
      fHistogrammer->Fill("ClusterThetaDiff", theta - mctheta, 1);
      fHistogrammer->Fill("ClusterPhiVsMCPhi", mcphi, phi, 1);
      fHistogrammer->Fill("ClusterThetaVsMCTheta", mctheta, theta, 1);
    }

    ++clustercounter;
  }
  fHistogrammer->Fill("TotalClusterEnergy", totalenergy, 1);
  ++fEvent;
}
