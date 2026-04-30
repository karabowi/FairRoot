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

#include "PndMvdDigiEventAna.h"

#include "PndSdsMCPoint.h"

#include "TVector3.h"
#include "TGeoManager.h"
#include "TLegend.h"

using std::vector;

// ClassdImp(PndMvdDigiEventAna)

ClassImp(PndMvdDigiEventAna);

PndMvdDigiEventAna::PndMvdDigiEventAna(TString fileName) : PndMvdEventAna(fileName)
{
  Init(fileName);
  SetCanvasColumns(6);

  fGeoH = PndGeoHandling::Instance();
}

void PndMvdDigiEventAna::InitBranch()
{
  fClassName = "PndSdsDigiPixel";
  fBranchName = "MVDPixelDigis";
}

void PndMvdDigiEventAna::InitHistos()
{
  for (std::map<TString, TH1 *>::const_iterator ki = fHistos.begin(); ki != fHistos.end(); ki++) {
    delete (ki->second);
  }
  fHistos.clear();
}

void PndMvdDigiEventAna::AnaHitNr(Int_t Nr)
{
  fHitArray->Delete();
  fTree->GetEntry(fActiveEvent);
  PndSdsDigiPixel *hit = (PndSdsDigiPixel *)fHitArray->At(Nr);
  // if (hit->GetDetName().Contains("119_2")){
  if (fHistos[hit->GetDetName()] == 0) {
    fHistos[hit->GetDetName()] = new TH2I("HitDistribution", hit->GetDetName().Data(), 400, 0, 400, 101, 0, 100);
    // fDrawOption[hit->GetDetName()] = "colz";
  }
  if (fFeHits[hit->GetDetName()] == 0) {
    fFeHits[hit->GetDetName()] = new std::vector<Int_t>(30, 0);
    fDrawOption[hit->GetDetName()] = "colz";
  }
  TH2 *tempHisto = (TH2 *)(fHistos[hit->GetDetName()]);
  tempHisto->Fill((Double_t)(hit->GetPixelColumn() + hit->GetFE() * 16), (Double_t)(hit->GetPixelRow())); //, (Double_t)(hit->GetCharge()));
  fFeHits[hit->GetDetName()]->at(hit->GetFE())++;
  fDigiPixelArray[hit->GetDetName()].push_back(*hit);
}

void PndMvdDigiEventAna::CreateMaxHitsHisto()
{

  TH1I *hPix1 = new TH1I("hPix1", "Hitrate per Module", 600, 0, 600);
  hPix1->SetLineColor(2);
  hPix1->GetXaxis()->SetTitle("Module ID");
  hPix1->GetYaxis()->SetTitle("Hits per second");
  hPix1->SetStats(kFALSE);
  TH1I *hPix2 = new TH1I("hPix2", "Hitrate per Module", 600, 0, 600);
  hPix2->SetLineColor(3);
  hPix2->GetXaxis()->SetTitle("Module ID");
  hPix2->GetYaxis()->SetTitle("Hits per second");
  hPix2->SetStats(kFALSE);
  TH1I *hMPix1 = new TH1I("hMPix1", "Hitrate per Module", 600, 0, 600);
  hMPix1->SetLineColor(4);
  hMPix1->GetXaxis()->SetTitle("Module ID");
  hMPix1->GetYaxis()->SetTitle("Hits per second");
  hMPix1->SetStats(kFALSE);
  TH1I *hMPix2 = new TH1I("hMPix2", "Hitrate per Module", 600, 0, 600);
  hMPix2->SetLineColor(5);
  hMPix2->GetXaxis()->SetTitle("Module ID");
  hMPix2->GetYaxis()->SetTitle("Hits per second");
  hMPix2->SetStats(kFALSE);
  TH1I *hSDisk1 = new TH1I("hSDisk1", "Hitrate per Module", 600, 0, 600);
  hSDisk1->SetLineColor(6);
  hSDisk1->GetXaxis()->SetTitle("Module ID");
  hSDisk1->GetYaxis()->SetTitle("Hits per second");
  hSDisk1->SetStats(kFALSE);
  TH1I *hSDisk2 = new TH1I("hSDisk2", "Hitrate per Module", 600, 0, 600);
  hSDisk2->SetLineColor(7);
  hSDisk2->GetXaxis()->SetTitle("Module ID");
  hSDisk2->GetYaxis()->SetTitle("Hits per second");
  hSDisk2->SetStats(kFALSE);
  TH1I *hRest = new TH1I("hRest", "Hitrate per Module", 600, 0, 600);
  hRest->SetLineColor(1);
  hRest->GetXaxis()->SetTitle("Module ID");
  hRest->GetYaxis()->SetTitle("Hits per second");
  hRest->SetStats(kFALSE);
  for (UInt_t i = 0; i < fHistoVector.size(); i++) {
    TString hName = fHistoVector[i]->GetTitle();
    // std::cout << hName << " " << fGeoH->GetPath(hName.Data()) << std::endl;
    Int_t weight = 10000000 / fAnaEvents;
    std::cout << weight << std::endl;
    if (fGeoH->GetPath(hName.Data()).Contains("MiniPixelDisk_1"))
      hMPix1->Fill(i, fHistoVector[i]->GetEntries() * weight);
    else if (fGeoH->GetPath(hName.Data()).Contains("MiniPixelDisk_2")) {
      hMPix2->Fill(i, fHistoVector[i]->GetEntries() * weight);
    } else if (fGeoH->GetPath(hName.Data()).Contains("PixelDisk_1"))
      hPix1->Fill(i, fHistoVector[i]->GetEntries() * weight);
    else if (fGeoH->GetPath(hName.Data()).Contains("PixelDisk_2"))
      hPix2->Fill(i, fHistoVector[i]->GetEntries() * weight);
    else if (fGeoH->GetPath(hName.Data()).Contains("StripDiskMedium2_1"))
      hSDisk1->Fill(i, fHistoVector[i]->GetEntries() * weight);
    else if (fGeoH->GetPath(hName.Data()).Contains("StripDiskMedium2_2"))
      hSDisk2->Fill(i, fHistoVector[i]->GetEntries() * weight);
    else
      hRest->Fill(i, fHistoVector[i]->GetEntries() * weight);
  }

  hMPix1->Draw("");
  hMPix2->Draw("same");

  hPix1->Draw("same");
  hPix2->Draw("same");

  hSDisk1->Draw("same");
  hSDisk2->Draw("same");

  hRest->Draw("same");
  /*	TLegend* leg = new TLegend();
    leg->AddEntry("hMPix1","Mini pixel disk 1","l");
    leg->AddEntry("hMPix2","Mini pixel disk 2","l");
    leg->AddEntry("hPix1","Pixel disk 1","l");
    leg->AddEntry("hPix2","Pixel disk 2","l");
    leg->AddEntry("hSDisk1","Combined disk 1","l");
    leg->AddEntry("hSDisk2","Combined disk 2","l");
    leg->DrawClone("same");*/
}

void PndMvdDigiEventAna::DrawSingleModuleFEHisto(TString path)
{
  TH1I *feHisto = new TH1I("feHisto", path.Data(), 500, 0, 10000);
  vector<Int_t> *fe = fFeHits[path.Data()];
  for (UInt_t i = 0; i < fe->size(); i++) {
    feHisto->Fill(fe->at(i));
  }
  feHisto->Draw("");
}

void PndMvdDigiEventAna::DrawAllModulesFEHisto()
{
  TH1I *feHisto = new TH1I("feHisto", "Hits in FE", 500, 0, 10000);
  std::map<TString, std::vector<Int_t> *>::const_iterator ki;
  for (ki = fFeHits.begin(); ki != fFeHits.end(); ++ki) {
    for (UInt_t i = 0; i < ki->second->size(); i++)
      feHisto->Fill(ki->second->at(i));
  }
  feHisto->Draw("");
}

void PndMvdDigiEventAna::EndOfEventAction() {}
