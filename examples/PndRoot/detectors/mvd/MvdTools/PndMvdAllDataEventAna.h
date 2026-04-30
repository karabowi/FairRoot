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

#ifndef PNDMVDALLDATAEVENTANA_H
#define PNDMVDALLDATAEVENTANA_H

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

#include "PndMvdEventAna.h"
#include "PndSdsMCPoint.h"
#include "PndSdsDigiPixel.h"
#include "PndSdsHit.h"
#include "PndSdsCluster.h"
#include "PndSdsCalcFePixel.h"
#include "PndEventDisplay.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TFile.h"
#include "TClonesArray.h"
#include "TVector3.h"
#include "TGeoNode.h"

#include <vector>

/** PndMvdAllDataEventAna.h
 * @author t.stockmanns <t.stockmanns@fz-juelich.de>
 * Analysis class which loads in a complete simulation chain
 */
class PndMvdAllDataEventAna : public PndMvdEventAna {
 public:
  PndMvdAllDataEventAna(){};
  PndMvdAllDataEventAna(TString fileName);
  virtual ~PndMvdAllDataEventAna(){};

  virtual void Init(TString filename);
  virtual void InitBranch();
  virtual void InitHistos();
  virtual void AnaHits();
  virtual void BeginOfEventAction() { ClearAllVectors(); };
  virtual void EndOfEventAction() { Create3DGeoHits(); };
  virtual void ClearHistos();

  void PrintHitArray();
  void PrintDigiArray();
  void PrintClusterArray();
  void PrintRecoArray();

  void FillHitHistos();
  void FillDigiHistos();
  void FillClusterHistos();
  void FillRecoHistos();
  void FillHitPerClusterHistos();
  void FillHitResolutionHistos();
  void Fill3DHisto();
  void FillHitProjHistos();

  void DrawHitHisto(TString detName, TCanvas *extCan = nullptr, Int_t pad = 0);
  void DrawDigiHisto(TString detName, TCanvas *extCan = nullptr, Int_t pad = 0);
  void DrawClusterHisto(TString detName, TCanvas *extCan = nullptr, Int_t pad = 0);
  void DrawRecoHisto(TString detName, TCanvas *extCan = nullptr, Int_t pad = 0);
  void DrawAllHistos(TString detName, TCanvas *extCan = nullptr);
  void DrawAllHistos(Int_t i, TCanvas *extCan = nullptr);
  void Draw3D(TString opt = "", TCanvas *extCan = nullptr, Int_t pad = 0);
  void DrawAllTracks(TCanvas *extCan = nullptr, Int_t pad = 0);
  void DrawHitTracks(TCanvas *extCan = nullptr, Int_t pad = 0);
  void DrawEvent(bool tracks = false, TCanvas *extCan = nullptr);
  void DrawTopVolume(TCanvas *extCan = nullptr, Int_t pad = 0, const char *opt = "");
  void DrawHisxy(TCanvas *extCan = nullptr, Int_t pad = 0);
  void DrawHisrz(TCanvas *extCan = nullptr, Int_t pad = 0);
  void DrawHistoVec(std::vector<TH1 *> *vec, TCanvas *extCan = nullptr, Int_t pad = 0) const;
  void DrawHisto(TH1 *histo, TCanvas *extCan = nullptr, Int_t pad = 0) const;
  void DrawHitPerCluster(TCanvas *extCan = nullptr, Int_t pad = 0) { DrawHisto(fAllHitPerClusterHistos, extCan, pad); };
  void DrawResolution(TCanvas *extCan = nullptr, Int_t pad = 0) { DrawHisto(fAllHitResolutionHistos, extCan, pad); };

  void Create3DGeoHits();

  void ClearHistoMaps(std::map<TString, TH1 *> *myHistos) const;
  void ClearAllHMaps();
  void ClearHistoVector(std::vector<TH1 *> *myVectors) const;
  void ClearAllVectors();

  TVector3 GetLocalHitPoints(TString detName, TVector3 input);
  std::vector<Int_t> GetHitPerCluster(PndSdsCluster *clusterCand);
  std::vector<TString> GetModulesHit();
  TVector3 CalcMeanHitPos(std::vector<Int_t> points);

 protected:
  TClonesArray *fDigiArray;
  TClonesArray *fClusterArray;
  TClonesArray *fRecoArray;
  TClonesArray *fGeoTrackArray;
  TClonesArray *fTrackFArray;

 private:
  std::map<TString, TH1 *> fDigiHistos;
  std::map<TString, TH1 *> fClusterHistos;
  std::map<TString, TH1 *> fRecoHistos;
  std::map<TString, TH1 *> fHitPerClusterHistos;
  std::map<TString, TH1 *> fHitResolutionHistos;

  std::vector<TH1 *> fHitHistoVec;
  std::vector<TH1 *> fDigiHistoVec;
  std::vector<TH1 *> fClusterHistoVec;
  std::vector<TH1 *> fRecoHistoVec;

  TGeoVolume *fMvdTopVolume;
  TGeoVolume *fRecoTopVolume;
  TGeoVolume *fRecoVolume;

  TH1 *fRecoErrorHisto;
  TH1 *fAllHitPerClusterHistos;
  TH1 *fAllHitResolutionHistos;
  TH3D *f3DMCHisto;
  TH3D *f3DRecoHisto;
  std::vector<TH1 *> fSimHisxy;
  std::vector<TH1 *> fSimHisrz;
  std::vector<TH1 *> fRecoHisxy;
  std::vector<TH1 *> fRecoHisrz;

  TCanvas *fCan1;
  TCanvas *fCan2;

  PndSdsCalcFePixel *fPixelCon;
  PndEventDisplay *fGeoList;

  ClassDef(PndMvdAllDataEventAna, 1);
};

#endif
