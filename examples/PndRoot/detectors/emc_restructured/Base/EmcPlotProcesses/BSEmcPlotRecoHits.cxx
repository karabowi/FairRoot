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

#include "BSEmcPlotRecoHits.h"

#include <vector>

#include "RtypesCore.h"
#include "TMath.h"
#include "TVector3.h"

#include "FairLink.h"
#include "FairMultiLinkedData.h"
#include "fairlogger/Logger.h"

#include "PndContainerI.h"
#include "PndMCTrack.h"

#include "BSEmcEnergies_t.h"
#include "BSEmcHistogrammer.h"
#include "BSEmcRecoHit.h"

void BSEmcPlotRecoHits::RequestDataContainer(PndContainerRegister *t_register)
{
  // Lets use the defaults set by the task
  t_register->Request({"", "PndMCTrack", kFALSE, kFALSE});
  t_register->Request({"", "BSEmcDigi", kFALSE, kFALSE});
  t_register->Request({"", "BSEmcRecoHit", kFALSE, kFALSE});
}

void BSEmcPlotRecoHits::GetDataContainer(PndContainerRegister *t_register)
{
  fMCTrackArray = t_register->GetDefaultBranch<PndMCTrack>();
  fDigiArray = t_register->GetDefaultBranch<BSEmcDigi>();
  fRecoHitArray = t_register->GetDefaultBranch<BSEmcRecoHit>();
}

void BSEmcPlotRecoHits::InitHistos(const TString &t_folderprefix)
{
  SetFolderPrefix(t_folderprefix);
  TString foldername = t_folderprefix + "/ControlHistos/RecoHits";
  fHistogrammer->Create1DHist("RecoHitNumber", foldername, {100, -0.5, 99.5, "Number of reco hits per Event"});

  fHistogrammer->Create1DHist("RecoHitTime", foldername, {500, 0, 50, "Time / [ns]"});
  fHistogrammer->Create1DHist("RecoHitEnergy", foldername, {1600, 0, 16, "Energy / [GeV]"});
  fHistogrammer->Create1DHist("RecoHitEnergy1", foldername, {1600, 0, 16, "Energy / [GeV]"});
  fHistogrammer->Create1DHist("RecoHitEnergy9", foldername, {1600, 0, 16, "Energy / [GeV]"});
  fHistogrammer->Create1DHist("RecoHitEnergy9OverEnergy1", foldername, {500, 0, 5, "Energy9 Over Energy1"});
  fHistogrammer->Create1DHist("RecoHitEnergy25", foldername, {1600, 0, 16, "Energy / [GeV]"});
  fHistogrammer->Create1DHist("TotalRecoHitEnergy", foldername, {1600, 0, 16, "Energy / [GeV]"});

  fHistogrammer->Create2DHist("RecoHitEnergyVsClusterSize", foldername, {1600, 0, 16, "Energy / [GeV]"}, {100, -0.5, 99.5, "Clustersize"});
  fHistogrammer->Create2DHist("RecoHitEnergyVsSubClusterSize", foldername, {1600, 0, 16, "Energy / [GeV]"}, {100, -0.5, 99.5, "Subclustersize"});

  fHistogrammer->Create1DHist("RecoHitCorrEnergy", foldername, {1600, 0, 16, "Energy / [GeV]"});
  fHistogrammer->Create1DHist("RecoHitLateralMoment", foldername, {120, 0.0, 1.2, "Lateral Moment"});
  fHistogrammer->Create1DHist("RecoHitZ20", foldername, {120, 0.0, 1.2, "Zernike20 Moment"});
  fHistogrammer->Create1DHist("RecoHitZ53", foldername, {120, 0.0, 1.2, "Zernike53 Moment"});

  fHistogrammer->Create2DHist("RecoHitPosition", foldername, {360, 0, 180, "Theta / [Deg]"}, {720, 0, 360, "Phi / [Deg]"});

  fHistogrammer->Create2DHist("MaxNeighbourEnergyOverEnergyVsNeighbourNumber", foldername, {9, -0.5, 8.5, "Number of Neighbours"},
                              {120, 0, 1.2, "Max Neighbour Crystal Energy / Central Crystal Energy"});

  fHistogrammer->Create1DHist("RecoHitPhiDiff", foldername, {720, -180, 180, "Phi difference (reco. Phi - gen. Phi)"});
  fHistogrammer->Create1DHist("RecoHitThetaDiff", foldername, {400, -2, 2, "Theta difference (reco. Theta - gen. Theta)"});
  fHistogrammer->Create1DHist("RecoHitThetaDiffForCorrected", foldername, {400, -2, 2, "Theta difference (corr. reco. Theta - gen. Theta)"});
  fHistogrammer->Create2DHist("RecoHitPhiVsMCPhi", foldername, {720, 0, 360, "gen. Phi"}, {720, 0, 360, "reco. Phi"});
  fHistogrammer->Create2DHist("RecoHitThetaVsMCTheta", foldername, {360, 0, 180, "gen. Theta"}, {360, 0, 180, "reco. Theta"});

  fHistogrammer->Create1DHist("RecoHitEnteringTracks", foldername, {100, -0.5, 99.5, "Number of entering tracks"});
  fHistogrammer->Create1DHist("RecoHitExitingTracks", foldername, {200, -0.5, 199.5, "Number of exiting tracks"});
  fHistogrammer->Create1DHist("RecoHitMcTracksLinks", foldername, {200, -0.5, 199.5, "Links on MC tracks"});
}

void BSEmcPlotRecoHits::Process()
{
  fHistogrammer->Fill("RecoHitNumber", fRecoHitArray->GetSize(), 1);
  Double_t totalenergy = 0;

  for (const BSEmcRecoHit *recoHit : fRecoHitArray->GetVectorOfPtrToConst()) {
    totalenergy += recoHit->GetRawEnergy();

    fHistogrammer->Fill("RecoHitTime", recoHit->GetTimeStamp(), 1);

    fHistogrammer->Fill("RecoHitEnergy", recoHit->GetRawEnergy(), 1);
    fHistogrammer->Fill("RecoHitEnergy1", recoHit->GetEnergy1(), 1);
    fHistogrammer->Fill("RecoHitEnergy9", recoHit->GetEnergy9(), 1);
    fHistogrammer->Fill("RecoHitEnergy9OverEnergy1", recoHit->GetEnergy9() / recoHit->GetEnergy1(), 1);
    fHistogrammer->Fill("RecoHitEnergy25", recoHit->GetEnergy25(), 1);
    fHistogrammer->Fill("RecoHitEnergyVsClusterSize", recoHit->GetRawEnergy(), recoHit->GetNumberOfClusterCrystals(), 1);
    fHistogrammer->Fill("RecoHitEnergyVsSubClusterSize", recoHit->GetRawEnergy(), recoHit->GetNumberOfSubClusterCrystals(), 1);

    fHistogrammer->Fill("RecoHitCorrEnergy", recoHit->GetCorrectedEnergy(), 1);
    fHistogrammer->Fill("RecoHitLateralMoment", recoHit->GetLateralMoment(), 1);
    fHistogrammer->Fill("RecoHitZ20", recoHit->GetZ20(), 1);
    fHistogrammer->Fill("RecoHitZ53", recoHit->GetZ53(), 1);
    fHistogrammer->Fill("RecoHitEnteringTracks", recoHit->GetTrackEntering().GetNLinks(), 1);
    fHistogrammer->Fill("RecoHitExitingTracks", recoHit->GetTrackExiting().GetNLinks(), 1);
    fHistogrammer->Fill("RecoHitMcTracksLinks", recoHit->GetMcSize(), 1);

    TVector3 pos = recoHit->GetPosition();
    Double_t theta = GetThetaOfInDeg(pos);
    Double_t phi = GetPhiOfInDeg(pos);
    fHistogrammer->Fill("RecoHitPosition", theta, phi, 1);

    // if MCTruth info exists (assuming single primary particle)
    if (fMCTrackArray->GetSize() > 0) {

      const PndMCTrack *primarytrack = fMCTrackArray->GetConstElementPtr(recoHit->GetTrackEntering().GetLink(0).GetIndex());
      TVector3 momentum = primarytrack->GetMomentum();
      Double_t mctheta = momentum.Theta() * TMath::RadToDeg();
      Double_t mcphi = momentum.Phi();
      if (mcphi < 0) {
        mcphi += 2 * TMath::Pi();
      }
      mcphi *= TMath::RadToDeg();

      fHistogrammer->Fill("RecoHitPhiDiff", phi - mcphi, 1);
      fHistogrammer->Fill("RecoHitThetaDiff", theta - mctheta, 1);
      Double_t corrtheta = GetThetaOfInDeg(recoHit->GetCorrectedPosition());

      fHistogrammer->Fill("RecoHitThetaDiffForCorrected", corrtheta - mctheta, 1);
      fHistogrammer->Fill("RecoHitPhiVsMCPhi", mcphi, phi, 1);
      fHistogrammer->Fill("RecoHitThetaVsMCTheta", mctheta, theta, 1);
    }
    LOG_IF(warning, recoHit->GetCentralCrystalId() < 100000000) << "BSEmcPlotRecoHits recoHit->GetCentralCrystalId() " << recoHit->GetCentralCrystalId();

    BSEmcEnergies_t energyinfo = BSEmcEnergies_t::GetEnergies(recoHit->GetCentralCrystalId(), fDigiArray->GetVectorOfPtrToConst(), fNeighbouringRelationPar);
    fHistogrammer->Fill("MaxNeighbourEnergyOverEnergyVsNeighbourNumber", energyinfo.NeighbourNumber, energyinfo.MaxRatio(), 1);
  }
  fHistogrammer->Fill("TotalRecoHitEnergy", totalenergy, 1);
}
