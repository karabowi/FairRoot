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

#include "BSEmcPlotRecoHitVsMC.h"

#include <utility>
#include <vector>

#include "RtypesCore.h"
#include "TObject.h"
#include "TString.h"
#include "TVector3.h"

#include "FairMultiLinkedData.h"
#include "FairRootManager.h"

#include "PndContainerI.h"
#include "PndMCTrack.h"

#include "BSEmcHistogrammer.h"
#include "BSEmcRecoHit.h"

void BSEmcPlotRecoHitVsMC::RequestDataContainer(PndContainerRegister *t_register)
{
  // Lets use the defaults set by the task
  t_register->Request({"", "PndMCTrack", kFALSE, kFALSE});
  t_register->Request({"", "BSEmcRecoHit", kFALSE, kFALSE});
}

void BSEmcPlotRecoHitVsMC::GetDataContainer(PndContainerRegister *t_register)
{
  fMCTrackArray = t_register->GetDefaultBranch<PndMCTrack>();
  fRecoHitArray = t_register->GetDefaultBranch<BSEmcRecoHit>();
}

void BSEmcPlotRecoHitVsMC::InitHistos(const TString &t_folderprefix)
{
  SetFolderPrefix(t_folderprefix);
  TString foldername = t_folderprefix + "/RecoHitVsMC";

  fHistogrammer->Create1DHist("RecoMCPhiDiff", foldername, {720, -180, 180, "Phi difference (reco. Phi - gen. Phi)"});
  fHistogrammer->Create1DHist("RecoMCThetaDiff", foldername, {360, -90, 90, "Theta difference (reco. Theta - gen. Theta)"});
  fHistogrammer->Create2DHist("PhiVsMCPhi", foldername, {720, 0, 360, "gen. Phi"}, {720, 0, 360, "reco. Phi"});
  fHistogrammer->Create2DHist("ThetaVsMCTheta", foldername, {360, 0, 180, "gen. Theta"}, {360, 0, 180, "reco. Theta"});
  fHistogrammer->Create2DHist("ThetaVsMCThetaMCEnergyOverRecoEnergy", foldername, {360, 0, 180, "gen. Theta"}, {360, 0, 180, "reco. Theta"});
  fHistogrammer->Create1DHist("MCEnergyOverRecoEnergy", foldername, {1000, 0, 10, "MC Energy (just Momentum-Vec-Mag) / RecoHit Energy"});
}

std::unique_ptr<PndMCTrack> BSEmcPlotRecoHitVsMC::GetMostPrimaryMCTrack(const BSEmcRecoHit *t_recohit) const
{
  FairMultiLinkedData mcLinks = t_recohit->GetTrackEntering();
  std::unique_ptr<PndMCTrack> currenttrack{};
  for (Int_t j = 0; j < mcLinks.GetNLinks(); j++) {
    std::unique_ptr<PndMCTrack> mctrack = std::unique_ptr<PndMCTrack>(dynamic_cast<PndMCTrack *>(FairRootManager::Instance()->GetCloneOfLinkData(mcLinks.GetLink(j))));
    if (mctrack->GetMotherID() == -1) {
      return mctrack;
    } else if (currenttrack == nullptr || mctrack->GetMotherID() < currenttrack->GetMotherID()) {
      currenttrack = std::move(mctrack);
    }
  }
  return currenttrack;
}

void BSEmcPlotRecoHitVsMC::Process()
{
  if (fMCTrackArray->GetSize() > 0) {
    for (const BSEmcRecoHit *recoHit : fRecoHitArray->GetVectorOfPtrToConst()) {
      std::unique_ptr<PndMCTrack> mctrack = GetMostPrimaryMCTrack(recoHit);
      if (mctrack != nullptr) {
        TVector3 momentum = mctrack->GetMomentum();
        Double_t mctheta = GetThetaOfInDeg(momentum);
        Double_t mcphi = GetPhiOfInDeg(momentum);
        TVector3 recoHitDir = recoHit->GetPosition();
        Double_t theta = GetThetaOfInDeg(recoHitDir);
        Double_t phi = GetPhiOfInDeg(recoHitDir);
        fHistogrammer->Fill("RecoMCPhiDiff", phi - mcphi, 1);
        fHistogrammer->Fill("RecoMCThetaDiff", theta - mctheta, 1);
        fHistogrammer->Fill("PhiVsMCPhi", mcphi, phi, 1);
        fHistogrammer->Fill("ThetaVsMCTheta", mctheta, theta, 1);
        fHistogrammer->Fill("MCEnergyOverRecoEnergy", mctrack->GetMomentum().Mag() / recoHit->GetRawEnergy(), 1);
        // Momentum().Mag() not correct
        fHistogrammer->Fill("ThetaVsMCThetaMCEnergyOverRecoEnergy", mctheta, theta, mctrack->GetMomentum().Mag() / recoHit->GetRawEnergy());
      }
    }
  }
}
