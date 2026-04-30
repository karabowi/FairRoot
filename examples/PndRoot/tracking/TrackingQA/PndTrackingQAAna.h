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

/**
 * @class PndTrackingQAAna
 *
 * @date 09.05.2023
 * @author Tobias Stockmanns <t.stockmanns@fz-juelich.de>
 */

#pragma once

#include "PndTrackingQASummary.h"

#include <TH1D.h>
#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TCut.h>

#include <map>
#include <string>


class PndTrackingQAAna : public TObject {
 public:
  PndTrackingQAAna();
  PndTrackingQAAna(std::string fileName) : fQAFileName(fileName){};
  PndTrackingQAAna(TFile *file) : fFile(file){};

  void SetQAFileName(TString fileName) { fQAFileName = fileName; }

  void SetSaveHistosAsPictures(bool val) { fSaveHistosAsPictures = val; }
  void SetPicturePrefix(TString prefix) { fPicturePrefix = prefix; };
  void SetPictureFormat(TString format) { fPictureFormat = format; }
  void SetWorkdir(TString workdir) { fWorkdir = workdir; }

  PndTrackingQASummary *GetTrackingQASummary() const { return fSummary; }
  std::map<std::string, double> GetResults() const { return fResults; }
  std::vector<std::string> GetPngFilenames() const { return fPngFiles; }
  void FillResults();

  virtual ~PndTrackingQAAna();

  void Init();
  void AnalyseQAData();

 protected:
  void InitHistograms();
  void LabelQualityHistogram(TH1 *h);
  void FillQualityHisto();
  void FillIdealHistos();
  void FillMomHistos();
  void FillEfficiencyHistos();
  virtual void SetQualityHisto(TH1 *histo, Bool_t relative, Int_t base = 1);
  void SaveHistoVectorAsPicture(std::vector<TH1 *>, TString identifier, int col, int row, TString options = "");
  void SaveHistosAsPicture();
  void SetEfficiencyVsHisto(TH1D *h1, TString selector, TCut cut = "", Double_t hitEfficiency = 0.7); ///< Generates track efficiency histo vs. a selected parameter
  std::array<Double_t, 6> DoubleGaussFit(TH1 *hisDiff);
  double FitMomHistos(std::string histoName);

 private:
  std::map<std::string, TH1 *> fHistos;
  std::map<std::string, double> fResults;
  std::vector<std::string> fPngFiles;
  std::string fQAFileName;
  TFile *fFile = nullptr;
  TFile *fOutputFile = nullptr;
  TTree *fTree = nullptr;
  PndTrackingQASummary *fSummary = nullptr;

  TString fPicturePrefix = "test";
  TString fPictureFormat = "gif";
//  bool fCDashOutput = true;
  bool fSaveHistosAsPictures = true;
  TString fWorkdir = "."; // directory to tell cdash where to find png output files
  bool fPngOutputSuccessful = true;

  ClassDef(PndTrackingQAAna, 1);
};
