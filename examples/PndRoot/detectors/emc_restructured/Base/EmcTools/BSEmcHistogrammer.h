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

#ifndef BSEMCHISTOGRAMMER_HH
#define BSEMCHISTOGRAMMER_HH

#include <map>

#include "Rtypes.h"
#include "RtypesCore.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TString.h"

class TBuffer;
class TCanvas;
class TClass;
class TFile;
class TMemberInspector;

/**
 * @class BSEmcHistogrammer
 * @brief Simple wrapper around std::map<TString (histname), TH1*> to help with histogram creation and writting to file.
 * @details
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup EmcBase
 */
class BSEmcHistogrammer {
 public:
  struct AxisData {
    Int_t BinNumber{100};
    Double_t Begin{-0.5};
    Double_t End{99.5};
    TString Title{"#"};
    Double_t Offset{1};

    AxisData(Int_t t_binNumber = 100, Double_t t_begin = -0.5, Double_t t_end = 99.5, TString t_title = "#", Double_t t_offset = 1)
      : BinNumber(t_binNumber), Begin(t_begin), End(t_end), Title(t_title), Offset(t_offset)
    {
    }
  };
  // using Key = TString;
  BSEmcHistogrammer();
  BSEmcHistogrammer(TFile *t_file);
  virtual ~BSEmcHistogrammer();

  void SetOutputFile(TFile *t_file) { fFile = t_file; }
  TFile *GetOutputFile() { return fFile; }

  TCanvas *CreateCanvas(const TString &t_key, const TString &t_foldername);
  TH1F *Create1DHist(const TString &t_key, const TString &t_foldername, AxisData t_xdata, AxisData t_ydata = AxisData{});
  TH2F *Create2DHist(const TString &t_key, const TString &t_foldername, AxisData t_xdata, AxisData t_ydata);
  TH3F *Create3DHist(const TString &t_key, const TString &t_foldername, AxisData t_xdata, AxisData t_ydata, AxisData t_zdata);

  void Fill(const TString &t_key, Double_t t_value, Double_t t_weight);
  void Fill(const TString &t_key, Double_t t_x, Double_t t_y, Double_t t_weight);
  TH3F *Get3DHist(const TString &t_key) { return dynamic_cast<TH3F *>(GetHist(t_key)); }
  TH2F *Get2DHist(const TString &t_key) { return dynamic_cast<TH2F *>(GetHist(t_key)); }
  TH1F *Get1DHist(const TString &t_key) { return dynamic_cast<TH1F *>(GetHist(t_key)); }
  TH1 *GetHist(const TString &t_key);
  void AddHist(TH1 *t_hist, const TString &t_key, const TString &t_foldername);

  void ClearAllHists();
  Bool_t DoesHistoExist(const TString &t_key) const;

  void WriteCanvases();

 private:
  void CreateFolders(const TString &t_foldername);

  Bool_t FolderExists(const TString &t_foldername);

 private:
  TFile *fFile{nullptr};
  std::map<TString, TH1 *> fHistoMap{};
  std::map<TString, TCanvas *> fCanvasMap{};
  ClassDef(BSEmcHistogrammer, 1);
};

#endif /*BSEMCHISTOGRAMMER_HH*/
