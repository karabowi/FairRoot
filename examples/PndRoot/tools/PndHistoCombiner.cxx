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
 * PndHistoCombiner.cxx
 *
 *  Created on: 21.05.2021
 *      Author: tstockmanns
 */

#include <PndHistoCombiner.h>

#include "TFile.h"
#include "TString.h"
#include "TKey.h"
#include "TH1.h"
#include "TH1F.h"
#include "TLegend.h"
#include "TPaveStats.h"
#include "TLatex.h"
#include "TTree.h"
#include "TLeafElement.h"

#include <Rtypes.h>
#include <RtypesCore.h>
#include <TObjArray.h>
#include <TStyle.h>
#include <cstddef>
#include <iostream>
#include <utility>

ClassImp(PndHistoCombiner);

PndHistoCombiner::PndHistoCombiner()
{
  // TODO Auto-generated constructor stub
}

PndHistoCombiner::~PndHistoCombiner()
{
  // TODO Auto-generated destructor stub
}

void PndHistoCombiner::InitFiles()
{
  for (auto &fileInfo : fFileData) {
    std::cout << "fileInfo " << fileInfo.fFileName << std::endl;
    fileInfo.fFile = new TFile(fileInfo.fFileName, "READ");

    std::cout << "fileInfo " << fileInfo.fFileName << " " << fileInfo.fFile << std::endl;
    if (fileInfo.fFile->IsZombie()) {
      std::cout << "Error reading File: " << fileInfo.fFile << std::endl;
    }
  }
}

int PndHistoCombiner::GenerateHistos()
{
  Int_t statbackup = gStyle->GetOptStat();
  gStyle->SetOptStat(1111110);

  InitFiles();
  if (fFileData.size() == 0) {
    std::cout << "Error: No files in list! Did you add files via AddFileNamesAndShortNames?" << std::endl;
    return 0;
  }
  std::cout << "Beginning " << fFileData[0].fFile << std::endl;

  TKey *key;
  TIter next(fFileData[0].fFile->GetListOfKeys());

  while ((key = static_cast<TKey *>(next()))) {
    TObject *obj = key->ReadObj();

    // only check TH1's
    if (!obj->InheritsFrom("TH1"))
      continue;
    TString name = obj->GetName();
    // std::cout << "HistoName: " << name << std::endl;
    if (CheckHistoName(name) != true)
      continue;
    // std::cout << "HistoAccepted: " << name << std::endl;

    fCanCreator.SelectPad(fPadNumber++);
    std::vector<TH1 *> histos;

    for (auto aFileI : fFileData) {
      TH1 *h = static_cast<TH1 *>(aFileI.fFile->Get(name));
      if (h == nullptr)
        continue;
      histos.push_back(h);
    }
//    std::cout << "We have " << histos.size() << " histos with the name " << name << std::endl;
    // Draw and test histograms
    if (obj->InheritsFrom("TH2")) {
      DrawAndTest2DHistoStack(histos);
    } else {
      DrawAndTestHistoStack(histos);
    }
  }

  fCanCreator.SaveCanvasses();
  if (fCreateCDashOutput) {
    CreateCDashOutput();
  }
  gStyle->SetOptStat(statbackup);
  return fFailCount;
}

int PndHistoCombiner::GenerateBranches()
{
  InitFiles();
  if (fFileData.size() == 0) {
    std::cout << "Error: No files in list! Did you add files via AddFileNamesAndShortNames?" << std::endl;
    return 0;
  }
  std::cout << "Beginning " << fFileData[0].fFile << std::endl;

  TTree *firstTree = (TTree *)fFileData[0].fFile->Get("pndsim");
  TObjArray *leaves = firstTree->GetListOfLeaves();

  for (int iLeaf = 0; iLeaf < leaves->GetSize(); iLeaf++) {
    TLeafElement *leaf = (TLeafElement *)leaves->At(iLeaf);
    TString type = leaf->GetTypeName();
    TString name = leaf->GetName();
    // std::cout << "BranchName: " << name << std::endl;

    if (CheckBranchType(type) == false)
      continue;
    if (CheckBranchName(name) == false)
      continue;
    std::cout << "BranchAccepted: " << name << "  \t " << type << std::endl;

    fCanCreator.SelectPad(fPadNumber++);

    std::vector<TH1 *> histos;
    int ifile = 0;
    TH1 *h0 = nullptr;
    TH1 *h = nullptr;

    for (auto aFileI : fFileData) {
      TString hname = Form("h_%s_%d", name.Data(), ifile);

      // copy first histogram properties to new root histo name
      if (ifile != 0)
        h = (TH1 *)h0->Clone(hname);
      std::cout << "\t histogram name " << hname << std::endl;

      // Draw and extract histogram
      TTree *atree = (TTree *)aFileI.fFile->Get("pndsim");
      atree->Draw(Form("%s>>%s", name.Data(), hname.Data()));
      h = (TH1 *)gPad->GetPrimitive(hname);
      histos.push_back(h);

      if (ifile == 0)
        h0 = h;
      ifile++;
    }
    DrawAndTestHistoStack(histos);
  }
  fCanCreator.SaveCanvasses();
  if (fCreateCDashOutput) {
    CreateCDashOutput();
  }
  return fFailCount;
}

void PndHistoCombiner::DrawAndTest2DHistoStack(std::vector<TH1 *> &histos)
{

  // Loop through the histograms in the stack
  for (size_t i = 1; i < histos.size(); i++) {
    if (histos[0] != nullptr && histos[i] != nullptr) { // protect but increase i later
      if (fThresholds.size() > 0) {
        // Here we do the testing
    	PndHistoCombiner::histoComparisonValues p = PerformTest(histos[0], histos[i]);
        AddCDashOutput(fFileData[i].fShortName, histos[i]->GetTitle(), p);
      }
      if (i > 1) {
        fCanCreator.SelectPad(fPadNumber++); // As 2D plots can not share the same pad, select a new one for each pair
      }
      TH1 *hpull = histos[0]->GetAsymmetry(histos[i]);
      hpull->SetStats(kFALSE);
      hpull->SetTitle(histos[0]->GetTitle());
      hpull->SetTitleSize(0.);
      hpull->SetLabelSize(3 * histos[0]->GetLabelSize("X"), "X");
      hpull->SetLabelSize(3 * histos[0]->GetLabelSize("Y"), "Y");
      gPad->SetGridy();
      hpull->Draw("colz");
    }
  }
}

void PndHistoCombiner::DrawAndTestHistoStack(std::vector<TH1 *> &histos)
{
  // Start with preparing the current canvas if pulls are required
  std::pair<TPad *, TPad *> thepads;
  if (fDoPulls) {
    thepads = SplitPadForPulls();
    thepads.first->cd();
  }

  // Loop through the histograms in the stack
  Float_t maximumY = 0.;
  int i = 0;
  for (TH1 *h : histos) {
    if (h != nullptr) { // protect but increase i later
      h->SetLineColor(1 + i);
      h->SetLineWidth(2);
      if (i == 0) {
        h->Draw();
      } else {
        h->Draw("same");
        if (fThresholds.size() > 0) {
          // Here we do the testing
          PndHistoCombiner::histoComparisonValues p = PerformTest(histos[0], h);
//          fFileData[i].fP = p;
          AddCDashOutput(fFileData[i].fShortName, h->GetTitle(), p);
        }
      }
      if (h->GetMaximum() > maximumY) {
        maximumY = h->GetMaximum();
      }
    }
    i++;
  }
  histos[0]->GetYaxis()->SetRangeUser(0, maximumY * 1.25);
  CreateLegend(histos);

  // now actually calculate and draw the pulls
  if (fDoPulls) {
    thepads.second->cd();
    DrawPulls(histos);
  }
  // Loop through the histograms in the stack
}

PndHistoCombiner::histoComparisonValues PndHistoCombiner::PerformTest(TH1 *h1, TH1 *h2)
{
  TString title = h1->GetTitle();
  TString histoName = h1->GetName();
  PndHistoCombiner::histoComparisonValues result;

  result.fKol = h2->KolmogorovTest(h1);
  if (!(h1->InheritsFrom("TH2"))){				//only for 1D histos are Chi2 and AD defined
	result.fChi2 = h2->Chi2Test(h1);
    result.fAD = h2->AndersonDarlingTest(h1);
  }

//  switch (fTestType) {
//  case 1:
//    title += " P(K)=";
//    result.fKol = h2->KolmogorovTest(h1);
//    break;
//  case 2:
//    title += " P(#Chi^{2})=";
//    p = h2->Chi2Test(h1);
//    break;
//  default: break;
//  }
//  title += Form("%.2f", p);
  histoComparisonValues threshold;

  if (fThresholds.size() == 1 && fThresholds[0].first.IsNull())
	  threshold = fThresholds[0].second;
  else {
	  auto threshold_iter = std::find_if(fThresholds.begin(), fThresholds.end(),
		[&histoName](std::pair<TString, histoComparisonValues>& val){
			  if (histoName.Contains(val.first))
				return true;
			  else return false;});
	  if (threshold_iter == fThresholds.end()){
		  LOG(warning) << "No thresholds found for histo: " << title;
		  return result;
	  } else {
		  LOG(debug) << "Thresholds: " << threshold_iter->first.Data() << " : " << threshold_iter->second.fKol << "/" << threshold_iter->second.fChi2 << "/" << threshold_iter->second.fAD;
	  }
	  threshold = threshold_iter->second;
  }

  if (TestPassed(title, result, threshold) == false) {
    title += " - FAILED";
    fFailCount++;
    LOG(info) << "*** Incompatible distribution " << title << " p value " <<  result.fKol << "/" << result.fChi2 << "/" << result.fAD
    		  << " threshold " << threshold.fKol << "/" << threshold.fChi2 << "/" << threshold.fAD;
  }
  h1->SetTitle(title);
  return result;
}

std::pair<TPad*, TPad*> PndHistoCombiner::SplitPadForPulls()
{
  // TStyle *thestyle = gStyle; //warn unused variable
  TPad *pad1 = new TPad("pad1", "The pad 80% of the height", 0.0, 0.2, 1.0, 1.0);
  TPad *pad2 = new TPad("pad2", "The pad 20% of the height", 0.0, 0.0, 1.0, 0.25);
  pad1->Draw();
  pad2->Draw();
  return std::make_pair(pad1, pad2);
}

void PndHistoCombiner::DrawPulls(std::vector<TH1 *> &histos)
{
  if (histos[0] == nullptr)
    return;
  for (size_t i = 1; i < histos.size(); i++) {
    if (histos[i] == nullptr)
      continue;
    // TH1F hpull = GetPullHisto(histos[0],histos[i]);
    TH1 *hpull = histos[0]->GetAsymmetry(histos[i]);
    hpull->SetStats(kFALSE);
    hpull->SetTitle("");
    hpull->SetTitleSize(0.);
    hpull->SetLabelSize(3 * histos[0]->GetLabelSize("X"), "X");
    hpull->SetLabelSize(3 * histos[0]->GetLabelSize("Y"), "Y");
    gPad->SetGridy();
    hpull->Draw("ex0");
  }
}

bool PndHistoCombiner::CheckHistoName(TString histoName)
{
  if (fHistoSelector.size() == 0)
    return true;
  else {
    for (auto selector : fHistoSelector) {
      if (histoName.Contains(selector)) {
        return true;
      }
    }
  }
  return false;
}

bool PndHistoCombiner::CheckBranchName(TString branchName)
{
  if (branchName.EndsWith("_"))
    return false;
  if (branchName.EndsWith("fUniqueID"))
    return false;
  if (branchName.EndsWith("fBits"))
    return false;
  if (branchName.EndsWith("fLink"))
    return false;
  if (branchName.Contains(" "))
    return false;
  if (fBranchSelector.size() == 0)
    return true;
  else {
    for (auto selector : fBranchSelector) {
      if (branchName.Contains(selector)) {
        return true;
      }
    }
  }
  return false;
}

bool PndHistoCombiner::CheckBranchType(TString branchType)
{
  branchType.ToLower();
  if (branchType.BeginsWith("int"))
    return true;
  if (branchType.BeginsWith("float"))
    return true;
  if (branchType.BeginsWith("double"))
    return true;
  return false;
}

void PndHistoCombiner::CreateCDashOutput()
{
  for (auto cdashresult : fCDashOutput) {
    std::cout << cdashresult.Data() << std::endl;
  }

  std::vector<TString> canvasPaths = fCanCreator.GetOutputFileNames();
  for (auto fullPath : canvasPaths) {
    Size_t lastSlash = fullPath.Last('/');
    TString fileName = fullPath(lastSlash + 1, fullPath.Length() - lastSlash);
    std::cout << "<DartMeasurementFile name=\"" << fileName.Data() << "\" type=\"image/gif\">" << fullPath.Data() << "</DartMeasurementFile>" << std::endl;
  }
}

void PndHistoCombiner::AddCDashOutput(TString prefix, TString histoName, histoComparisonValues value)
{
  TString htitle = RemoveSpecialCharacters(histoName);
  TString hprefix = RemoveSpecialCharacters(prefix);

  TString output = "<DartMeasurement name=\"" + hprefix + "_" + htitle + "_Kol" + "\" type=\"numeric/double\">";
  output += value.fKol;
  output += "</DartMeasurement>";
  fCDashOutput.push_back(output);
  output = "<DartMeasurement name=\"" + hprefix + "_" + htitle + "_Chi2" + "\" type=\"numeric/double\">";
  output += value.fChi2;
  output += "</DartMeasurement>";
  fCDashOutput.push_back(output);
  output = "<DartMeasurement name=\"" + hprefix + "_" + htitle + "_AD" + "\" type=\"numeric/double\">";
  output += value.fAD;
  output += "</DartMeasurement>";
  fCDashOutput.push_back(output);
}

TString PndHistoCombiner::RemoveSpecialCharacters(TString input)
{
  input.ReplaceAll(" ", "");
  input.ReplaceAll("#", "");
  input.ReplaceAll("(", "");
  input.ReplaceAll(")", "");
  input.ReplaceAll("/", "");
  input.ReplaceAll("^", "");
  input.ReplaceAll("[", "");
  input.ReplaceAll("]", "");
  input.ReplaceAll(":", "");
  input.ReplaceAll(".", "");
  input.ReplaceAll("{", "");
  input.ReplaceAll("}", "");

  return input;
}

void PndHistoCombiner::CreateLegend(std::vector<TH1 *> &histos)
{
  auto legend = new TLegend(0.025, 0.775, 0.25, 0.925);
  legend->SetHeader("File Comparison", "C"); // option "C" allows to center the header
  for (size_t i = 0; i < histos.size(); i++) {
    legend->AddEntry(histos[i], fFileData[i].fShortName, "f");
  }
  legend->Draw();
  if (TString(histos[0]->GetTitle()).Contains("FAILED")) {
    TLatex *pText = new TLatex(0.4, 0.8, "FAILED");
    pText->SetNDC(kTRUE);
    pText->SetTextColor(kRed + 1);
    pText->SetTextSize(0.075);
    pText->Draw();
  }
}
