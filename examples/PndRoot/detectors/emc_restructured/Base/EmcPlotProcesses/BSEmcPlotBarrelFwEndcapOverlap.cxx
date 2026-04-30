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

#include "BSEmcPlotBarrelFwEndcapOverlap.h"

//#include <bits/exception.h>
#include <math.h>
#include <vector>

#include "RtypesCore.h"
#include "TVector3.h"

#include "fairlogger/Logger.h"

#include "BSEmcHistogrammer.h"

void BSEmcPlotBarrelFwEndcapOverlap::RequestDataContainer(PndContainerRegister *t_register)
{
  // Lets use the defaults set by the task
  t_register->Request({"EmcRecoHitFwEndcap", "BSEmcRecoHit", kFALSE, kFALSE});
  t_register->Request({"EmcRecoHitBarrel", "BSEmcRecoHit", kFALSE, kFALSE});
}

void BSEmcPlotBarrelFwEndcapOverlap::GetDataContainer(PndContainerRegister *t_register)
{

  fFWECRecoHitArray = t_register->GetInput<BSEmcRecoHit>("EmcRecoHitFwEndcap");
  fBarrelRecoHitArray = t_register->GetInput<BSEmcRecoHit>("EmcRecoHitBarrel");
}

void BSEmcPlotBarrelFwEndcapOverlap::InitHistos(const TString & /*t_folderprefix*/)
{
  TString foldername = "Emc/Theta22";
  SetFolderPrefix(foldername);
  fHistogrammer->Create2DHist("BarrelPhiVsFWECPhi", foldername, {360, 0, 360, "FWEC RecoHit Phi / [Deg]"}, {360, 0, 360, "Barrel RecoHit Phi / [Deg]"});
  fHistogrammer->Create2DHist("BarrelThetaVsFWECTheta", foldername, {180, 0, 180, "FWEC RecoHit Theta / [Deg]"}, {180, 0, 180, "Barrel RecoHit Theta / [Deg]"});
  fHistogrammer->Create1DHist("OverlapCausedMultiplicityInFWEC", foldername, {25, -0.5, 24.5, "FwEndcap Multiplicity"});
  fHistogrammer->Create1DHist("OverlapCausedFwEndcapRecoHitEnergy", foldername, {1600, 0, 16, "Reco Hit Energy deposited in FwEndcap/ [GeV]"});
  fHistogrammer->Create1DHist("OverlapCausedFwEndcapTotalEnergy", foldername, {1600, 0, 16, "Total Energy deposited in FwEndcap/ [GeV]"});
  fHistogrammer->Create1DHist("OverlapCausedThetaDiff", foldername, {250, -25, 25, "(FWEC RecoHit Theta - Barrel RecoHit Theta) / [Deg]"});
  fHistogrammer->Create1DHist("OverlapCausedPhiDiff", foldername, {500, -50, 50, "(FWEC RecoHit Phi - Barrel RecoHit Phi ) / [Deg]"});

}

void BSEmcPlotBarrelFwEndcapOverlap::Process()
{

  if (fBarrelRecoHitArray->GetSize() > 0 && fFWECRecoHitArray->GetSize() > 0) {
    fHistogrammer->Fill("OverlapCausedMultiplicityInFWEC", fFWECRecoHitArray->GetSize(), 1);

    for (const auto *barrelrecohit : fBarrelRecoHitArray->GetVectorOfPtrToConst()) {
      TVector3 barrelpos = barrelrecohit->GetPosition();
      Double_t barrelphi = GetPhiOfInDeg(barrelpos);
      Double_t barreltheta = GetThetaOfInDeg(barrelpos);
      if (std::abs(barreltheta - 22) < 1) {
        Double_t fwenergy = 0;
        for (const auto *fwrecohit : fFWECRecoHitArray->GetVectorOfPtrToConst()) {
          TVector3 fwpos = fwrecohit->GetPosition();
          Double_t fwphi = GetPhiOfInDeg(fwpos);
          Double_t fwtheta = GetThetaOfInDeg(fwpos);
          fHistogrammer->Fill("BarrelPhiVsFWECPhi", fwphi, barrelphi, 1);
          fHistogrammer->Fill("BarrelThetaVsFWECTheta", fwtheta, barreltheta, fwrecohit->GetRawEnergy());
          fHistogrammer->Fill("OverlapCausedThetaDiff", fwtheta - barreltheta, 1);
          fHistogrammer->Fill("OverlapCausedPhiDiff", fwphi - barrelphi, 1);
          fHistogrammer->Fill("OverlapCausedFwEndcapRecoHitEnergy", fwrecohit->GetRawEnergy(), 1);
          fwenergy += fwrecohit->GetRawEnergy();
        }
        fHistogrammer->Fill("OverlapCausedFwEndcapTotalEnergy", fwenergy, 1);
      }
    }
  }
}
