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

/*
 * PndTrackingQAAna.cxx
 *
 *  Created on: 09.05.2023
 *      Author: tstockmanns
 */

#include "PndTrackingQAAna.h"
#include "PndTrackingQA.h"
#include "FairLogger.h"

#include <TCanvas.h>
#include <TStyle.h>
#include <TF1.h>
#include <TImageDump.h>

#include "FWHM.h"

ClassImp(PndTrackingQAAna);

PndTrackingQAAna::PndTrackingQAAna()
{
  // TODO Auto-generated constructor stub
}

PndTrackingQAAna::~PndTrackingQAAna()
{
  if (fTree != nullptr)
    delete (fTree);

  if (fFile != nullptr)
    delete (fFile);
}

void PndTrackingQAAna::Init()
{
  if (fOutputFile == nullptr) {
    fOutputFile = new TFile("QAHistos.root", "RECREATE");
  }
  if (fFile == nullptr) {
    fFile = new TFile(fQAFileName.c_str());
  }
  fTree = static_cast<TTree *>(fFile->Get("pndsim"));
  if (fTree == nullptr) {
    LOG(error) << "No tree with name pndsim found in file: " << fQAFileName << std::endl;
  }
  InitHistograms();
  fSummary = static_cast<PndTrackingQASummary *>(fFile->Get("TrackingQASummary"));
  if (fSummary == nullptr) {
    LOG(error) << "No TrackingQASummary found in file " << fQAFileName << std::endl;
  }
}

void PndTrackingQAAna::InitHistograms()
{
  fHistos["IdealTracksPerEvent"] = new TH1I("fIdealTracksPerEvent", "Ideal Tracks per Event", 21, -0.5, 20.5);
  fHistos["IdealTracksPerEvent"]->GetXaxis()->SetTitle("Tracks/Event");
  fHistos["IdealTracksPerEvent"]->SetLineWidth(2);

  fHistos["IdealPHisto"] = new TH1I("fIdealPHisto", "Ideal Total Momentum", 900, -0.05, 4.45);
  fHistos["IdealPHisto"]->GetXaxis()->SetTitle("p [GeV/c]");
  fHistos["IdealPHisto"]->SetLineWidth(2);

  fHistos["IdealPtHisto"] = new TH1I("fIdealPtHisto", "Ideal Tansversal Momentum", 900, -0.05, 4.45);
  fHistos["IdealPtHisto"]->GetXaxis()->SetTitle("p_{t} [GeV/c]");
  fHistos["IdealPtHisto"]->SetLineWidth(2);

  fHistos["IdealPlHisto"] = new TH1I("fIdealPlHisto", "Ideal Longitudinal Momentum", 650, -2, 4.5);
  fHistos["IdealPtHisto"]->GetXaxis()->SetTitle("p_{l} [GeV/c]");
  fHistos["IdealPtHisto"]->SetLineWidth(2);

  fHistos["IdealDPCA"] = new TH1I("fIdealDPCA", "MC Track distance to primary interaction point in XY", 1000, 0, 10.0);
  fHistos["IdealDPCA"]->GetXaxis()->SetTitle("distance [cm]");
  fHistos["IdealDPCA"]->SetLineWidth(2);

  fHistos["IdealDZ"] = new TH1I("fIdealDZ", "MC Track Z position", 1000, -2., 8.0);
  fHistos["IdealDZ"]->GetXaxis()->SetTitle("distance in Z [cm]");
  fHistos["IdealDZ"]->SetLineWidth(2);

  fHistos["PHisto"] = new TH1D("fPHisto", "Momentum Resolution", 1000, -1, 1);
  fHistos["PHisto"]->GetXaxis()->SetTitle("p^{RECO} - p^{MC} / GeV");
  fHistos["PHisto"]->GetYaxis()->SetTitle("counts");
  fHistos["PHisto"]->SetLineWidth(2);

  fHistos["PRelHisto"] = new TH1D("fPRelHisto", "Relative Momentum Resolution", 200, -1, 1);
  fHistos["PRelHisto"]->GetXaxis()->SetTitle("(p^{RECO} - p^{MC}) / p^{MC}");
  fHistos["PRelHisto"]->GetYaxis()->SetTitle("counts");
  fHistos["PRelHisto"]->SetLineWidth(2);

  fHistos["PtHisto"] = new TH1D("fPtHisto", "Transverse Momentum Resolution", 1000, -1, 1);
  fHistos["PtHisto"]->GetXaxis()->SetTitle("p_{t}^{RECO} - p_{t}^{MC} / GeV");
  fHistos["PtHisto"]->GetYaxis()->SetTitle("counts");
  fHistos["PtHisto"]->SetLineWidth(2);

  fHistos["PtRelHisto"] = new TH1D("fPtRelHisto", "Relative Transverse Momentum Resolution", 200, -1, 1);
  fHistos["PtRelHisto"]->GetXaxis()->SetTitle("(p_{t}^{RECO} - p_{t}^{MC}) / p_{t}^{MC}");
  fHistos["PtRelHisto"]->GetYaxis()->SetTitle("counts");
  fHistos["PtRelHisto"]->SetLineWidth(2);

  fHistos["PlHisto"] = new TH1D("fPlHisto", "Longitudinal Momentum Resolution", 1000, -1, 1);
  fHistos["PlHisto"]->GetXaxis()->SetTitle("p_{l}^{RECO} - p_{l}^{MC} / GeV");
  fHistos["PlHisto"]->GetYaxis()->SetTitle("counts");
  fHistos["PlHisto"]->SetLineWidth(2);

  fHistos["PlRelHisto"] = new TH1D("fPlRelHisto", "Relative Longitudinal Momentum Resolution", 200, -1, 1);
  fHistos["PlRelHisto"]->GetXaxis()->SetTitle("(p_{l}^{RECO} - p_{l}^{MC}) / p_{l}^{MC}");
  fHistos["PlRelHisto"]->GetYaxis()->SetTitle("counts");
  fHistos["PlRelHisto"]->SetLineWidth(2);

  fHistos["Efficiency"] = new TH1D("fEfficiency", "nHits correct vs. nMCHits", 101, -0.005, 1.005);
  fHistos["Efficiency"]->GetXaxis()->SetTitle("Track efficiency");
  fHistos["Efficiency"]->GetYaxis()->SetTitle("counts");
  fHistos["Efficiency"]->SetLineWidth(2);

  fHistos["Purity"] = new TH1D("fPurity", "nHits correct vs. all hits in track", 101, -0.005, 1.005);
  fHistos["Purity"]->GetXaxis()->SetTitle("Track purity");
  fHistos["Purity"]->GetYaxis()->SetTitle("counts");
  fHistos["Purity"]->SetLineWidth(2);

  fHistos["QualyHisto"] = new TH1I("fQualyHisto", "Quality of Trackfinding;;Counts", 28, -15.5, 12.5);
  fHistos["QualyHisto"]->SetDrawOption("TEXT HIST");
  LabelQualityHistogram(fHistos["QualyHisto"]);

  fHistos["QualyHisto_rel_all"] = new TH1D("fQualyHisto_rel_all", "Tracking Quality for all tracks", 26, -15.5, 10.5);
  fHistos["QualyHisto_rel_all"]->SetBarWidth(0.45);
  fHistos["QualyHisto_rel_all"]->SetBarOffset(0.1);
  fHistos["QualyHisto_rel_all"]->SetFillColor(kBlue);
  LabelQualityHistogram(fHistos["QualyHisto_rel_all"]);

  fHistos["QualyHisto_rel_possible"] = new TH1D("fQualyHisto_rel_possible", "Tracking Quality for possible tracks", 26, -15.5, 10.5);
  fHistos["QualyHisto_rel_possible"]->SetBarWidth(0.4);
  fHistos["QualyHisto_rel_possible"]->SetBarOffset(0.55);
  fHistos["QualyHisto_rel_possible"]->SetFillColor(kRed);
  LabelQualityHistogram(fHistos["QualyHisto_rel_possible"]);

  fHistos["Efficiency_vs_phi"] = new TH1D("fEfficiency_vs_phi", "Track Efficiency vs #phi; #phi", 90, -180, 180);
  fHistos["Efficiency_vs_phi"]->SetLineWidth(2);
  fHistos["Efficiency_vs_theta"] = new TH1D("fEfficiency_vs_theta", "Track Efficiency vs #theta; #theta", 180, 0, 180);
  fHistos["Efficiency_vs_theta"]->SetLineWidth(2);

  fHistos["Efficiency_vs_p"] = new TH1D("fEfficiency_vs_p", "Track Efficiency vs p; p [GeV/c]", 100, 0, 3);
  fHistos["Efficiency_vs_p"]->SetLineWidth(2);

  fHistos["Efficiency_vs_pt"] = new TH1D("fEfficiency_vs_pt", "Track Efficiency vs pt; #p_t [GeV/c]", 100, 0, 3);
  fHistos["Efficiency_vs_pt"]->SetLineWidth(2);

  fHistos["Efficiency_vs_pl"] = new TH1D("fEfficiency_vs_pl", "Track Efficiency vs pl; #p_l [GeV/c]", 100, 0, 3);
  fHistos["Efficiency_vs_pl"]->SetLineWidth(2);

  fHistos["Efficiency_vs_dPCA"] = new TH1D("fEfficiency_vs_dPCAXY", "Track Efficiency vs distance to prim vertex in XY; distance [cm]", 100, 0, 10);
  fHistos["Efficiency_vs_dPCA"]->SetLineWidth(2);

  fHistos["Efficiency_vs_zVertex"] = new TH1D("fEfficiency_vs_z", "Track Efficiency vs. z; z [cm]", 200, -10, 10);
  fHistos["Efficiency_vs_zVertex"]->SetLineWidth(2);
}

void PndTrackingQAAna::LabelQualityHistogram(TH1 *hist)
{
  for (auto val : TrackingQA::qualityNumbers::listOfNumbers) {
    hist->GetXaxis()->SetBinLabel(hist->FindFixBin(val), TrackingQA::qualityNumbers::QualityNumberToDetailedString(val).c_str());
  }
}

void PndTrackingQAAna::FillQualityHisto()
{
  for (auto val : TrackingQA::qualityNumbers::listOfNumbers) {
    fHistos["QualyHisto"]->Fill(val, fSummary->GetResult(val));
  }
  fHistos["QualyHisto"]->Fill(TrackingQA::qualityNumbers::kMcAllTracks, fSummary->GetAllTracks());
  fHistos["QualyHisto"]->Fill(TrackingQA::qualityNumbers::kMcAllTracksWithHits, fSummary->GetAllTracksWithAtLeast3Hits());
}

void PndTrackingQAAna::AnalyseQAData()
{
  FillQualityHisto();

  SetQualityHisto(fHistos["QualyHisto_rel_all"], kTRUE, fSummary->GetAllTracksWithAtLeast3Hits());
  SetQualityHisto(fHistos["QualyHisto_rel_possible"], kTRUE, fSummary->GetAllPossibleTracks());

  FillIdealHistos();
  FillMomHistos();
  FillEfficiencyHistos();

  if (fSaveHistosAsPictures) {
    SaveHistosAsPicture();
  }
  FillResults();

  for (auto res : fResults) {
    std::cout << res.first << " " << res.second << std::endl;
  }

  fOutputFile->cd();
  for (auto histo : fHistos) {
    histo.second->Write();
  }
  fOutputFile->Close();
}

void PndTrackingQAAna::SetQualityHisto(TH1 *histo, Bool_t relative, Int_t base)
{
  Int_t allTracks = fSummary->GetAllTracks();
  Int_t allTracksWithHits = fSummary->GetAllTracksWithAtLeast3Hits();
  Int_t allPossibleTracksWithHits = fSummary->GetAllPossibleTracks();
  Int_t allTracksWithHitsNotFound = fSummary->GetAllTracksWithHitsNotFound();

  Int_t mcLessThanThreePrim = fSummary->GetResult(TrackingQA::qualityNumbers::kMcLessThanThreePrim);
  Int_t mcAtLeastThreePrim = fSummary->GetResult(TrackingQA::qualityNumbers::kMcAtLeastThreePrim);
  Int_t mcAtLeastThreeSec = fSummary->GetResult(TrackingQA::qualityNumbers::kMcAtLeastThreeSec);
  Int_t mcPossiblePrim = fSummary->GetResult(TrackingQA::qualityNumbers::kMcPossiblePrim);
  Int_t mcPossibleSec = fSummary->GetResult(TrackingQA::qualityNumbers::kMcPossibleSec);

  Double_t divisor = 1.0;
  if (relative == kTRUE) {
    divisor = allTracks / 100.0;
  }

  histo->Fill(TrackingQA::qualityNumbers::kMcAllTracksWithHits, (Double_t)allTracksWithHits / divisor);
  histo->Fill(TrackingQA::qualityNumbers::kMcLessThanThreePrim, (Double_t)mcLessThanThreePrim / divisor);
  histo->Fill(TrackingQA::qualityNumbers::kMcAtLeastThreePrim, (Double_t)mcAtLeastThreePrim / divisor);
  histo->Fill(TrackingQA::qualityNumbers::kMcAtLeastThreeSec, (Double_t)mcAtLeastThreeSec / divisor);
  histo->Fill(TrackingQA::qualityNumbers::kMcPossiblePrim, (Double_t)mcPossiblePrim / divisor);
  histo->Fill(TrackingQA::qualityNumbers::kMcPossibleSec, (Double_t)fSummary->GetResult(TrackingQA::qualityNumbers::kMcPossibleSec) / divisor);

  divisor = 1.0;
  if (mcLessThanThreePrim > 0) {
    if (relative == kTRUE) {
      divisor = mcLessThanThreePrim / 100.0;
    }
    histo->Fill(TrackingQA::qualityNumbers::kLessThanThreePrim, (Double_t)fSummary->GetFoundPrimaryTracksLessThan3Hits() / divisor);
  }

  divisor = 1.0;
  if (mcAtLeastThreePrim > 0) {
    if (relative == kTRUE) {
      divisor = mcAtLeastThreePrim / 100.0;
    }
    histo->Fill(TrackingQA::qualityNumbers::kAtLeastThreePrim, (Double_t)fSummary->GetFoundPrimaryTracksMoreThan3Hits() / divisor);
  }

  divisor = 1.0;
  if (mcAtLeastThreeSec > 0) {
    if (relative == kTRUE) {
      divisor = mcAtLeastThreeSec / 100.0;
    }
    histo->Fill(TrackingQA::qualityNumbers::kAtLeastThreeSec, (Double_t)fSummary->GetFoundSecondaryTracksMoreThan3Hits() / divisor);
  }

  divisor = 1.0;
  if (mcPossiblePrim > 0) {
    if (relative == kTRUE) {
      divisor = mcPossiblePrim / 100.0;
    }
    histo->Fill(TrackingQA::qualityNumbers::kPossiblePrim, (Double_t)fSummary->GetFoundPrimaryTracksPossible() / divisor);
  }

  divisor = 1.0;
  if (mcPossibleSec > 0) {
    if (relative == kTRUE) {
      divisor = mcPossibleSec / 100.0;
    }
    histo->Fill(TrackingQA::qualityNumbers::kPossibleSec, (Double_t)fSummary->GetFoundSecondaryTracksPossible() / divisor);
  }

  Double_t baseDouble = base;
  if (relative == kTRUE) {
    baseDouble /= 100.0;
  }

  if (base == 0)
    return;

  histo->Fill(TrackingQA::qualityNumbers::kFullyPure, (Double_t)fSummary->GetResult(TrackingQA::qualityNumbers::kFullyPure) / baseDouble);
  histo->Fill(TrackingQA::qualityNumbers::kFullyImpure, (Double_t)fSummary->GetResult(TrackingQA::qualityNumbers::kFullyImpure) / baseDouble);
  histo->Fill(TrackingQA::qualityNumbers::kPartiallyPure, (Double_t)fSummary->GetResult(TrackingQA::qualityNumbers::kPartiallyPure) / baseDouble);
  histo->Fill(TrackingQA::qualityNumbers::kPartiallyImpure, (Double_t)fSummary->GetResult(TrackingQA::qualityNumbers::kPartiallyImpure) / baseDouble);
  histo->Fill(TrackingQA::qualityNumbers::kGhost, (Double_t)fSummary->GetResult(TrackingQA::qualityNumbers::kGhost) / baseDouble);
  histo->Fill(TrackingQA::qualityNumbers::kClone, (Double_t)fSummary->GetResult(TrackingQA::qualityNumbers::kClone) / baseDouble);
  histo->Fill(TrackingQA::qualityNumbers::kFound, (Double_t)fSummary->GetResult(TrackingQA::qualityNumbers::kFound) / baseDouble);
  histo->Fill(TrackingQA::qualityNumbers::kNotFound, (base - (Double_t)fSummary->GetResult(TrackingQA::qualityNumbers::kFound)) / baseDouble);
}

void PndTrackingQAAna::FillIdealHistos()
{
  fTree->Draw("MCTrackInfo@.GetEntries() >> fIdealTracksPerEvent", "", "goff");
  fTree->Draw("MCTrackInfo.fMomFirst.Mag() >> fIdealPHisto", "", "goff");
  fTree->Draw("MCTrackInfo.fMomFirst.Perp() >> fIdealPtHisto", "", "goff");
  fTree->Draw("MCTrackInfo.fMomFirst.Z() >> fIdealPlHisto", "", "goff");
  fTree->Draw("MCTrackInfo.fdPCA2d >> fIdealDPCA", "", "goff");
  fTree->Draw("MCTrackInfo.fVertex.fZ >> fIdealDZ", "", "goff");
}

double PndTrackingQAAna::FitMomHistos(std::string histoName){
	fHistos[histoName]->Smooth();
	double fwhm = FWHM(fHistos[histoName]);
	return fwhm * 100.;
}

void PndTrackingQAAna::FillMomHistos()
{
  fTree->Draw("RecoTrackInfo[MCTrackInfo.fAssoRecoTrackID.fIndex].fMomFirst.Mag() - RecoTrackInfo[MCTrackInfo.fAssoRecoTrackID.fIndex].fMCMomFirst.Mag() >> fPHisto",
              "MCTrackInfo.fAssoRecoTrackID.fIndex > -1", "goff");
  fTree->Draw("(RecoTrackInfo[MCTrackInfo.fAssoRecoTrackID.fIndex].fMomFirst.Mag() - "
              "RecoTrackInfo[MCTrackInfo.fAssoRecoTrackID.fIndex].fMCMomFirst.Mag())/RecoTrackInfo[MCTrackInfo.fAssoRecoTrackID.fIndex].fMCMomFirst.Mag() >> fPRelHisto",
              "MCTrackInfo.fAssoRecoTrackID.fIndex > -1", "goff");

  fResults["PRes"] = FitMomHistos("PRelHisto");

  fTree->Draw("RecoTrackInfo[MCTrackInfo.fAssoRecoTrackID.fIndex].fMomFirst.Perp() - RecoTrackInfo[MCTrackInfo.fAssoRecoTrackID.fIndex].fMCMomFirst.Perp() >> fPtHisto",
              "MCTrackInfo.fAssoRecoTrackID.fIndex > -1", "goff");
  fTree->Draw("(RecoTrackInfo[MCTrackInfo.fAssoRecoTrackID.fIndex].fMomFirst.Perp() - "
              "RecoTrackInfo[MCTrackInfo.fAssoRecoTrackID.fIndex].fMCMomFirst.Perp())/RecoTrackInfo[MCTrackInfo.fAssoRecoTrackID.fIndex].fMCMomFirst.Perp() >> fPtRelHisto",
              "MCTrackInfo.fAssoRecoTrackID.fIndex > -1", "goff");

  fResults["PtRes"] = FitMomHistos("PtRelHisto");

  fTree->Draw("RecoTrackInfo[MCTrackInfo.fAssoRecoTrackID.fIndex].fMomFirst.Z() - RecoTrackInfo[MCTrackInfo.fAssoRecoTrackID.fIndex].fMCMomFirst.Z() >> fPlHisto",
              "MCTrackInfo.fAssoRecoTrackID.fIndex > -1", "goff");
  fTree->Draw("(RecoTrackInfo[MCTrackInfo.fAssoRecoTrackID.fIndex].fMomFirst.Z() - "
              "RecoTrackInfo[MCTrackInfo.fAssoRecoTrackID.fIndex].fMCMomFirst.Z())/RecoTrackInfo[MCTrackInfo.fAssoRecoTrackID.fIndex].fMCMomFirst.Z() >> fPlRelHisto",
              "MCTrackInfo.fAssoRecoTrackID.fIndex > -1", "goff");

  fResults["PlRes"] = FitMomHistos("PlRelHisto");
}

void PndTrackingQAAna::FillEfficiencyHistos()
{
  fTree->Draw("RecoTrackInfo[MCTrackInfo.fAssoRecoTrackID.fIndex].GetEfficiency() >> fEfficiency", "MCTrackInfo.fAssoRecoTrackID.fIndex > -1", "goff");
  fTree->Draw("RecoTrackInfo[MCTrackInfo.fAssoRecoTrackID.fIndex].GetPurity() >> fPurity", "MCTrackInfo.fAssoRecoTrackID.fIndex > -1", "goff");

  SetEfficiencyVsHisto(dynamic_cast<TH1D *>(fHistos["Efficiency_vs_phi"]), "MCTrackInfo.fMomFirst.Phi() * TMath::RadToDeg()",
                       "MCTrackInfo.fMCQuality == -2 || MCTrackInfo.fMCQuality == -1");
  SetEfficiencyVsHisto(dynamic_cast<TH1D *>(fHistos["Efficiency_vs_theta"]), "MCTrackInfo.fMomFirst.Theta() * TMath::RadToDeg()",
                       "MCTrackInfo.fMCQuality == -2 || MCTrackInfo.fMCQuality == -1");
  SetEfficiencyVsHisto(dynamic_cast<TH1D *>(fHistos["Efficiency_vs_p"]), "MCTrackInfo.fMomFirst.Mag()", "MCTrackInfo.fMCQuality == -2 || MCTrackInfo.fMCQuality == -1");
  SetEfficiencyVsHisto(dynamic_cast<TH1D *>(fHistos["Efficiency_vs_pt"]), "MCTrackInfo.fMomFirst.Perp()", "MCTrackInfo.fMCQuality == -2 || MCTrackInfo.fMCQuality == -1");
  SetEfficiencyVsHisto(dynamic_cast<TH1D *>(fHistos["Efficiency_vs_pl"]), "MCTrackInfo.fMomFirst.Z()", "MCTrackInfo.fMCQuality == -2 || MCTrackInfo.fMCQuality == -1");
  SetEfficiencyVsHisto(dynamic_cast<TH1D *>(fHistos["Efficiency_vs_dPCA"]), "MCTrackInfo.fdPCA2d", "MCTrackInfo.fMCQuality == -2 || MCTrackInfo.fMCQuality == -1");
  SetEfficiencyVsHisto(dynamic_cast<TH1D *>(fHistos["Efficiency_vs_zVertex"]), "MCTrackInfo.fVertex.x()", "MCTrackInfo.fMCQuality == -2 || MCTrackInfo.fMCQuality == -1");
}

void PndTrackingQAAna::FillResults()
{
  fResults["HitEfficiency"] = fHistos["Efficiency"]->GetMean() * 100.;
  fResults["HitPurity"] = fHistos["Purity"]->GetMean() * 100.;
  fResults["TrackEfficiencyAllTracks"] = (double)fSummary->GetResult(TrackingQA::qualityNumbers::kFound) / fSummary->GetAllTracksWithAtLeast3Hits() * 100.0;
  fResults["TrackEfficiencyAllPossible"] = (double)fSummary->GetResult(TrackingQA::qualityNumbers::kFound) / fSummary->GetAllPossibleTracks() * 100.0;
}

void PndTrackingQAAna::SetEfficiencyVsHisto(TH1D *h1, TString selector, TCut cut, Double_t hitEfficiency)
{
  TH1D *temp = new TH1D("temp", "temp", h1->GetNbinsX(), h1->GetXaxis()->GetXmin(), h1->GetXaxis()->GetXmax()); // create a copy of h1
  TCut mc_cut = cut;
  TCut reco_cut = mc_cut + "MCTrackInfo.GetRecoTrackID().fIndex>-1";
  TString effCut = "RecoTrackInfo.GetEfficiency()>";
  effCut += hitEfficiency;
  reco_cut += effCut;
  reco_cut += "RecoTrackInfo.IsClone() == 0";

  TString tempSelector = selector + ">>temp";
  fTree->Draw(tempSelector, mc_cut, "goff");

  tempSelector = selector + ">>" + h1->GetName();
  fTree->Draw(tempSelector, reco_cut, "goff");

  temp->Sumw2();
  h1->Sumw2();
  h1->Divide(temp);
}

void PndTrackingQAAna::SaveHistoVectorAsPicture(std::vector<TH1 *> hVector, TString identifier, int col, int row, TString options)
{

  TString fileName = fPicturePrefix + "_" + identifier + "." + fPictureFormat;
  TString canName = "Can" + fPicturePrefix + "_" + identifier;
  TCanvas can(canName, canName);
  int canId = 1;
  can.Divide(col, row);

  for (auto histo : hVector) {
    if (histo == nullptr) {
      std::cout << "No Histo available: " << identifier << " id " << canId << std::endl;
    } else {
      can.cd(canId++);
      histo->Draw(options);
    }
  }
  gStyle->SetImageScaling(3.0); // seems to have no effect

  try {
	TImageDump *dump = new TImageDump(fileName);
	can.Paint();
	dump->Close();
  } catch (...) {
    LOG(warning) << "PNG output did not work!";
    fPngOutputSuccessful = false;
  }

  if (fPngOutputSuccessful == true)
    fPngFiles.push_back(fileName.Data());

//  if (fCDashOutput) {
//    TString path = fWorkdir + "/";
//    std::cout << "<DartMeasurementFile name=\"" << fileName << "\" type=\"image/png\">" << path + fileName << "</DartMeasurementFile>" << std::endl;
//  }
}

void PndTrackingQAAna::SaveHistosAsPicture()
{
  //  gStyle->SetImageScaling(3.0);

  std::vector<TH1 *> histos;
  histos.assign({fHistos["IdealTracksPerEvent"], fHistos["IdealPHisto"], fHistos["IdealPtHisto"], fHistos["IdealPlHisto"], fHistos["IdealDPCA"], fHistos["IdealDZ"]});
  SaveHistoVectorAsPicture(histos, "ideal", 2, 3);

  histos.assign({fHistos["PHisto"], fHistos["PRelHisto"], fHistos["PtHisto"], fHistos["PtRelHisto"], fHistos["PlHisto"], fHistos["PlRelHisto"]});
  SaveHistoVectorAsPicture(histos, "momentumResolution", 2, 3);

  histos.assign({fHistos["QualyHisto_rel_all"], fHistos["QualyHisto_rel_possible"]});
  SaveHistoVectorAsPicture(histos, "quality", 1, 2, "HISTO TEXT");

  histos.assign({fHistos["Efficiency"], fHistos["Purity"]});
  SaveHistoVectorAsPicture(histos, "EfficienciesPurities", 1, 2);

  histos.assign({fHistos["Efficiency_vs_phi"], fHistos["Efficiency_vs_theta"], fHistos["Efficiency_vs_p"], fHistos["Efficiency_vs_pt"], fHistos["Efficiency_vs_pl"],
                 fHistos["Efficiency_vs_dPCA"], fHistos["Efficiency_vs_zVertex"]});
  SaveHistoVectorAsPicture(histos, "EffVsValues", 2, 4);
}

std::array<Double_t, 6> PndTrackingQAAna::DoubleGaussFit(TH1 *hisDiff)
{
  Double_t par[6] = {40., 0., 0.08, 10., 0.1, 0.2};
  // prefit peak
  TF1 *g2 = new TF1("g2", "gaus", -1, 1);
  hisDiff->Fit(g2, "RQ");
  g2->GetParameters(&par[0]);
  // fit total
  TF1 *total = new TF1("total", "gaus(0)+gaus(3)", -1, 1);
  total->SetParameters(par);
  total->SetLineColor(kRed);
  total->SetLineWidth(2);
  total->SetLineStyle(7);
  hisDiff->Fit(total, "RQ");
  total->GetParameters(&par[0]);
  std::array<Double_t, 6> result;
  std::copy_n(std::begin(par), 6, std::begin(result));
  total->DrawCopy();
  return result;
}
