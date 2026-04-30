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

//
// C++ Interface: PndMvdMCEventAna
//
// Description:
//
//
// Author: t.stockmanns <stockman@ikp455>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PNDMVDEVENTANA_H
#define PNDMVDEVENTANA_H

#include "PndSdsMCPoint.h"
#include "PndGeoHandling.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include "TClonesArray.h"
#include "TString.h"

#include <map>
#include <vector>
/**PndMvdEventAna.h
@author t.stockmanns
Abstract base clas for data analysis


*/
class PndMvdEventAna {
 public:
  PndMvdEventAna(){};
  PndMvdEventAna(TString fileName);
  virtual ~PndMvdEventAna(){};

  virtual void Init(TString fileName);
  void InitCanvas();

  void AnaEvents(Int_t count);
  void AnaEventNr(Int_t Nr);
  void NextEvent();
  void DrawHistos();
  void DrawHisto(TString path, TString opt = "");
  void SetCanvasColumns(Int_t col) { fCancol = col; };
  void SetDrawOption(TString histoName, TString option);
  void SetSelection(TString sel) { fSelection = sel; };
  void FillVector();
  void DrawVector(Int_t index, TString opt);
  Int_t GetEventNr() const { return fActiveEvent; };
  TString GetSelection() const { return fSelection; };
  TH1 *GetHisto(TString histoName) { return fHistos[histoName]; };
  TH1 *GetHisto(Int_t i) { return fHistoVector[i]; };
  virtual void InitBranch() = 0; // set fClassName and fBranchName here
  virtual void InitHistos() = 0;
  virtual void AnaHits();
  virtual void AnaHitNr(Int_t i){};
  virtual void BeginOfEventAction(){};
  virtual void EndOfEventAction(){};
  virtual void ClearHistos(){};

 protected:
  Int_t fActiveEvent;
  Int_t fAnaEvents;
  TFile *fFile;
  TTree *fTree;
  TClonesArray *fHitArray;
  TString fClassName;  // has to be set in constructor of derived class
  TString fBranchName; // has to be set in constructor of derived class
  TString fSelection;  // will be passed to derived classes to do an event selection
  TCanvas *fCan;
  std::map<TString, TH1 *> fHistos;
  std::map<TString, TString> fDrawOption; // stores the drawOption for the Draw command
  std::vector<TH1 *> fHistoVector;
  Int_t fCancol;
  PndGeoHandling *fGeoH;

  ClassDef(PndMvdEventAna, 1);
};

#endif
