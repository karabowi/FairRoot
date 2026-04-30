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

#include "BSEmcPlotAllRecoHits.h"

//#include <bits/exception.h>
#include <vector>

#include "RtypesCore.h"
#include "TVector3.h"

#include "fairlogger/Logger.h"

#include "BSEmcHistogrammer.h"
#include "BSEmcRecoHit.h"

void BSEmcPlotAllRecoHits::RequestDataContainer(PndContainerRegister *t_register)
{
  // Lets use the defaults set by the task
  t_register->Request({"EmcRecoHitFwEndcap", "BSEmcRecoHit", kFALSE, kFALSE});
  t_register->Request({"EmcRecoHitBarrel", "BSEmcRecoHit", kFALSE, kFALSE});
  t_register->Request({"EmcRecoHitBwEndcap", "BSEmcRecoHit", kFALSE, kFALSE});
  t_register->Request({"EmcRecoHitShashlik", "BSEmcRecoHit", kFALSE, kFALSE});
}

void BSEmcPlotAllRecoHits::GetDataContainer(PndContainerRegister *t_register)
{
  fFWECRecoHitArray = t_register->GetInput<BSEmcRecoHit>("EmcRecoHitFwEndcap");
  fBarrelRecoHitArray = t_register->GetInput<BSEmcRecoHit>("EmcRecoHitBarrel");
  fBWECRecoHitArray = t_register->GetInput<BSEmcRecoHit>("EmcRecoHitBwEndcap");
  fShashlikRecoHitArray = t_register->GetInput<BSEmcRecoHit>("EmcRecoHitShashlik");
}

void BSEmcPlotAllRecoHits::InitHistos(const TString &t_folderprefix)
{
  SetFolderPrefix(t_folderprefix);
  TString foldername = "ControlHistos/RecoHits";
  fHistogrammer->Create1DHist("AllRecoHitsNumber", foldername, {100, -0.5, 99.5, "Number of reco hits per Event"});
  fHistogrammer->Create1DHist("TotalAllRecoHitsEnergy", foldername, {1600, 0, 16, "Energy / [GeV]"});
  fHistogrammer->Create2DHist("AllRecoHitsPosition", foldername, {360, 0, 180, "Theta / [Deg]"}, {720, 0, 360, "Phi / [Deg]"});

}

void BSEmcPlotAllRecoHits::FillPositionPlot(const BSEmcRecoHit *t_recoHit)
{
  TVector3 pos = t_recoHit->GetPosition();
  Double_t theta = GetThetaOfInDeg(pos);
  Double_t phi = GetPhiOfInDeg(pos);
  fHistogrammer->Fill("AllRecoHitsPosition", theta, phi, 1);
}

void BSEmcPlotAllRecoHits::Process()
{

  UInt_t recohitnumber = fFWECRecoHitArray->GetSize() + fBarrelRecoHitArray->GetSize() + fBWECRecoHitArray->GetSize() + fShashlikRecoHitArray->GetSize();
  fHistogrammer->Fill("AllRecoHitsNumber", recohitnumber, 1);

  Double_t totalenergy = 0;
  for (const BSEmcRecoHit *recoHit : fFWECRecoHitArray->GetVectorOfPtrToConst()) {
    totalenergy += recoHit->GetRawEnergy();
    FillPositionPlot(recoHit);
  }
  for (const BSEmcRecoHit *recoHit : fBarrelRecoHitArray->GetVectorOfPtrToConst()) {
    totalenergy += recoHit->GetRawEnergy();
    FillPositionPlot(recoHit);
  }
  for (const BSEmcRecoHit *recoHit : fBWECRecoHitArray->GetVectorOfPtrToConst()) {
    totalenergy += recoHit->GetRawEnergy();
    FillPositionPlot(recoHit);
  }
  for (const BSEmcRecoHit *recoHit : fShashlikRecoHitArray->GetVectorOfPtrToConst()) {
    totalenergy += recoHit->GetRawEnergy();
    FillPositionPlot(recoHit);
  }
  fHistogrammer->Fill("TotalAllRecoHitsEnergy", totalenergy, 1);
}
