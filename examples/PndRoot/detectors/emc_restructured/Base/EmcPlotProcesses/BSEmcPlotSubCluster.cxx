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

#include "BSEmcPlotSubCluster.h"

#include <vector>

#include "RtypesCore.h"
#include "TMath.h"
#include "TVector3.h"

#include "FairMultiLinkedData.h"

#include "PndContainerI.h"
#include "PndMCTrack.h"

#include "BSEmcHistogrammer.h"
#include "BSEmcSubCluster.h"

void BSEmcPlotSubCluster::RequestDataContainer(PndContainerRegister *t_register)
{
  // Lets use the defaults set by the task
  t_register->Request({"", "PndMCTrack", kFALSE, kFALSE});
  t_register->Request({"", "BSEmcSubCluster", kFALSE, kFALSE});
}

void BSEmcPlotSubCluster::GetDataContainer(PndContainerRegister *t_register)
{
  fMCTrackArray = t_register->GetDefaultBranch<PndMCTrack>();
  fSubClusterArray = t_register->GetDefaultBranch<BSEmcSubCluster>();
}

void BSEmcPlotSubCluster::InitHistos(const TString &t_folderprefix)
{
  SetFolderPrefix(t_folderprefix);
  TString foldername = t_folderprefix + "/ControlHistos/SubCluster";
  fHistogrammer->Create1DHist("SubClusterNumber", foldername, {100, -0.5, 99.5, "Number of subcluster per Event"});
  fHistogrammer->Create1DHist("SubClusterTime", foldername, {500, 0, 50, "Time / [ns]"});
  fHistogrammer->Create1DHist("SubClusterEnergy", foldername, {1600, 0, 16, "Energy / [GeV]"});
  fHistogrammer->Create1DHist("TotalSubClusterEnergy", foldername, {1600, 0, 16, "Energy / [GeV]"});
  fHistogrammer->Create1DHist("SubClusterSize", foldername, {100, -0.5, 99.5, "Subc Clustersize"});

  fHistogrammer->Create2DHist("SubClusterPosition", foldername, {360, 0, 180, "Theta / [Deg]"}, {720, 0, 360, "Phi / [Deg]"});

  fHistogrammer->Create2DHist("SubClusterEnergyVsSubClusterSize", foldername, {1600, 0, 16, "Energy / [GeV]"}, {100, -0.5, 99.5, "Subclustersize"});

  fHistogrammer->Create1DHist("SubClusterPhiDiff", foldername, {720, -180, 180, "Phi difference (reco. Phi - gen. Phi"});
  fHistogrammer->Create1DHist("SubClusterThetaDiff", foldername, {360, -90, 90, "Theta difference (reco. Theta - gen. Theta"});
  fHistogrammer->Create2DHist("SubClusterPhiVsMCPhi", foldername, {720, 0, 360, "gen. Phi"}, {720, 0, 360, "reco. Phi"});
  fHistogrammer->Create2DHist("SubClusterThetaVsMCTheta", foldername, {360, 0, 180, "gen. Theta"}, {360, 0, 3180, "reco. Theta"});

  fHistogrammer->Create1DHist("SubClusterEnteringTracks", foldername, {100, -0.5, 99.5, "Number of entering tracks"});
  fHistogrammer->Create1DHist("SubClusterExitingTracks", foldername, {200, -0.5, 199.5, "Number of exiting tracks"});
}

void BSEmcPlotSubCluster::Process()
{
  fHistogrammer->Fill("SubClusterNumber", fSubClusterArray->GetSize(), 1);
  Double_t totalenergy = 0;

  for (const BSEmcSubCluster *subcluster : fSubClusterArray->GetVectorOfPtrToConst()) {
    totalenergy += subcluster->GetEnergy();

    fHistogrammer->Fill("SubClusterTime", subcluster->GetTimeStamp(), 1);

    fHistogrammer->Fill("SubClusterEnergy", subcluster->GetEnergy(), 1);
    fHistogrammer->Fill("SubClusterSize", subcluster->GetDigis().size(), 1);

    fHistogrammer->Fill("SubClusterEnergyVsSubClusterSize", subcluster->GetEnergy(), subcluster->GetNumberOfDigis(), 1);

    TVector3 pos = subcluster->GetPosition();
    Double_t theta = GetThetaOfInDeg(pos);
    Double_t phi = GetPhiOfInDeg(pos);
    fHistogrammer->Fill("SubClusterPosition", theta, phi, 1);

    fHistogrammer->Fill("SubClusterEnteringTracks", subcluster->GetTrackEntering().GetNLinks(), 1);
    fHistogrammer->Fill("SubClusterExitingTracks", subcluster->GetTrackExiting().GetNLinks(), 1);
    // if MCTruth info exists (assuming single primary particle)
    if (fMCTrackArray->GetSize() > 0) {
      const PndMCTrack *primarytrack = fMCTrackArray->GetConstElementPtr(0);
      TVector3 momentum = primarytrack->GetMomentum();
      Double_t mctheta = momentum.Theta() * TMath::RadToDeg();
      Double_t mcphi = momentum.Phi();
      if (mcphi < 0) {
        mcphi += 2 * TMath::Pi();
      }
      mcphi *= TMath::RadToDeg();

      fHistogrammer->Fill("SubClusterPhiDiff", phi - mcphi, 1);
      fHistogrammer->Fill("SubClusterThetaDiff", theta - mctheta, 1);
      fHistogrammer->Fill("SubClusterPhiVsMCPhi", mcphi, phi, 1);
      fHistogrammer->Fill("SubClusterThetaVsMCTheta", mctheta, theta, 1);
    }
  }
  fHistogrammer->Fill("TotalSubClusterEnergy", totalenergy, 1);
}
