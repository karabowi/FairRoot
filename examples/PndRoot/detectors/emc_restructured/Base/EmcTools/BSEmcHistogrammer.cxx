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

#include "BSEmcHistogrammer.h"

#include <stdlib.h>
#include <utility>

#include "TAxis.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TObject.h"
#include "TString.h"

#include "fairlogger/Logger.h"

BSEmcHistogrammer::BSEmcHistogrammer()
{
  TH1::AddDirectory(kFALSE);
}

BSEmcHistogrammer::BSEmcHistogrammer(TFile *t_file)
{
  fFile = t_file;
}

BSEmcHistogrammer::~BSEmcHistogrammer()
{
  ClearAllHists();
}

void BSEmcHistogrammer::CreateFolders(const TString &t_foldername)
{
  TObjArray *folders = t_foldername.Tokenize('/');
  TString path = "";
  for (Int_t i = 0; i < folders->GetEntriesFast(); i++) {
    TObjString *folder = dynamic_cast<TObjString *>(folders->At(i));
    TString folderStr = folder->GetString();
    if (fFile->cd(path + folderStr)) {
      // pass
    } else {
      fFile->mkdir(path + folderStr);
      fFile->cd(path + folderStr);
    }
    path += folderStr + "/";
  }
  delete folders;
}

Bool_t BSEmcHistogrammer::FolderExists(const TString &t_foldername)
{
  Bool_t exists = kFALSE;
  if (fFile->cd(t_foldername)) {
    exists = kTRUE;
    fFile->cd();
  }
  return exists;
}

TCanvas *BSEmcHistogrammer::CreateCanvas(const TString &t_key, const TString &t_foldername)
{
  auto *canvas = new TCanvas(t_key, t_key);
  if (!FolderExists(t_foldername)) {
    CreateFolders(t_foldername);
  }
  fCanvasMap[t_foldername + t_key] = canvas;
  return canvas;
}

void BSEmcHistogrammer::WriteCanvases()
{
  fFile->cd();
  for (auto entry : fCanvasMap) {
    fFile->cd();
    TString path = "";
    TObjArray *folders = entry.first.Tokenize('/');
    for (Int_t i = 0; i < folders->GetEntriesFast() - 1; i++) {
      TObjString *folder = dynamic_cast<TObjString *>(folders->At(i));
      TString folderStr = folder->GetString();
      path += folderStr + "/";
      fFile->cd(path);
    }
    TObjString *name = dynamic_cast<TObjString *>(folders->At(folders->GetEntriesFast() - 1));
    TString nameStr = name->GetString();
    fFile->cd(path);
    entry.second->Write(nameStr);
    delete folders;
  }
}

TH1F *BSEmcHistogrammer::Create1DHist(const TString &t_key, const TString &t_foldername, AxisData t_xdata, AxisData t_ydata)
{
  auto *hist = new TH1F(t_key, t_key, t_xdata.BinNumber, t_xdata.Begin, t_xdata.End);
  hist->GetXaxis()->SetTitle(t_xdata.Title);
  hist->GetXaxis()->SetTitleOffset(t_xdata.Offset);
  hist->GetYaxis()->SetTitle(t_ydata.Title);
  hist->GetYaxis()->SetTitleOffset(t_ydata.Offset);

  if (!FolderExists(t_foldername)) {
    CreateFolders(t_foldername);
  }
  hist->SetDirectory(fFile->GetDirectory(t_foldername));
  fHistoMap[t_key] = hist;
  return hist;
}

TH2F *BSEmcHistogrammer::Create2DHist(const TString &t_key, const TString &t_foldername, AxisData t_xdata, AxisData t_ydata)
{
  auto *hist = new TH2F(t_key, t_key, t_xdata.BinNumber, t_xdata.Begin, t_xdata.End, t_ydata.BinNumber, t_ydata.Begin, t_ydata.End);
  hist->GetXaxis()->SetTitle(t_xdata.Title);
  hist->GetXaxis()->SetTitleOffset(t_xdata.Offset);
  hist->GetYaxis()->SetTitle(t_ydata.Title);
  hist->GetYaxis()->SetTitleOffset(t_ydata.Offset);

  if (!FolderExists(t_foldername)) {
    CreateFolders(t_foldername);
  }
  hist->SetDirectory(fFile->GetDirectory(t_foldername));
  fHistoMap[t_key] = hist;
  return hist;
}

TH3F *BSEmcHistogrammer::Create3DHist(const TString &t_key, const TString &t_foldername, AxisData t_xdata, AxisData t_ydata, AxisData t_zdata)
{
  auto *hist = new TH3F(t_key, t_key, t_xdata.BinNumber, t_xdata.Begin, t_xdata.End, t_ydata.BinNumber, t_ydata.Begin, t_ydata.End, t_zdata.BinNumber, t_zdata.Begin, t_zdata.End);
  hist->GetXaxis()->SetTitle(t_xdata.Title);
  hist->GetXaxis()->SetTitleOffset(t_xdata.Offset);
  hist->GetYaxis()->SetTitle(t_ydata.Title);
  hist->GetYaxis()->SetTitleOffset(t_ydata.Offset);
  hist->GetZaxis()->SetTitle(t_zdata.Title);
  hist->GetZaxis()->SetTitleOffset(t_zdata.Offset);

  if (!FolderExists(t_foldername)) {
    CreateFolders(t_foldername);
  }
  hist->SetDirectory(fFile->GetDirectory(t_foldername));
  fHistoMap[t_key] = hist;
  return hist;
}

void BSEmcHistogrammer::Fill(const TString &t_key, Double_t t_value, Double_t t_weight)
{
  Get1DHist(t_key)->Fill(t_value, t_weight);
}

void BSEmcHistogrammer::Fill(const TString &t_key, Double_t t_x, Double_t t_y, Double_t t_weight)
{
  Get2DHist(t_key)->Fill(t_x, t_y, t_weight);
}

void BSEmcHistogrammer::ClearAllHists()
{
  fHistoMap.clear();
}

Bool_t BSEmcHistogrammer::DoesHistoExist(const TString &t_key) const
{
  auto pos = fHistoMap.find(t_key);
  if (pos != fHistoMap.end()) {
    if (pos->second != nullptr) {
      return kTRUE;
    }
  }
  return kFALSE;
}

void BSEmcHistogrammer::AddHist(TH1 *t_hist, const TString &t_key, const TString &t_foldername)
{
  fHistoMap[t_key] = t_hist;
  if (!FolderExists(t_foldername)) {
    CreateFolders(t_foldername);
  }
  t_hist->SetDirectory(fFile->GetDirectory(t_foldername));
}

TH1 *BSEmcHistogrammer::GetHist(const TString &t_key)
{
  if (DoesHistoExist(t_key)) {
    return fHistoMap[t_key];
  }
  LOG(error) << "BSEmcHistogrammer::GetHist(TString t_key=" << t_key << ") abort! Histo called " << t_key << " does not exist.";
  exit(-1);
  return nullptr;
}
