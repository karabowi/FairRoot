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

// -------------------------------------------------------------------------
/** PndSciTAnaTask.h
 *@author Alicia Sanchez <a.sanchez@gsi.de>
 **
 ** Ideal SciT Mass Reco
 */
// -------------------------------------------------------------------------
#ifndef PNDTOFANAIDEAL_H
#define PNDTOFANAIDEAL_H

#include "FairTask.h"
//#include "PndGeoHypPar.h"
#include "PndSciTPoint.h"
#include "TVector3.h"
#include "TRandom.h"
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "FairGeoVector.h"
#include "FairGeoTransform.h"
#include "TGeoMatrix.h"
#include "TGeoBBox.h"
#include <iostream>
#include "GFTrackCand.h"
#include "TFile.h"
#include "TTree.h"

#include "PndRiemannTrack.h"
#include <string>

#include <map>
#include <vector>
class TClonesArray;

class PndSciTAnaIdeal : public FairTask {
 public:
  /** Default constructor **/
  PndSciTAnaIdeal();

  /** Destructor **/
  ~PndSciTAnaIdeal();

  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();
  virtual InitStatus ReInit();
  void SetVerbose(int val) { fVerbose = val; }
  void smear(TVector3 &pos, TVector3 &dpos);
  void WriteHistograms();
  void SetTreeFName(const Char_t *Name) { fFileName = Name; };
  void SetRPreFitter(bool rie) { friemann = rie; };
  void SetLHKaonCut(bool cut) { fLHcut = cut; };
  void SetSciFDT(Double_t dt) { fScFtime = dt; };
  void SetCTrack(Bool_t stt) { fstt = stt; };

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);
  virtual void Finish();

 private:
  PndHypGeoHandling *fGeoH;

  TString fBranchName;
  TString fBranchName2;
  TString fTrackCand;
  TString fBranchName3;
  TString fBranchName4;
  TString fTpcBranch;

  /** Input array of PndSciTPoints **/
  TClonesArray *fPointArray;
  TClonesArray *fHPointArray;
  TClonesArray *fSTPointArray;
  TClonesArray *fTrigArray;
  TClonesArray *fsciFPointArray;
  TClonesArray *fTrackCandArray;
  TClonesArray *fHitOutputArray;
  TGeoHMatrix *fCurrentTransMat;
  PndHypPoint *fCurrentHypPoint;
  TClonesArray *fTpcArray;
  TClonesArray *fMicroCandidates;

  Int_t GetChargeIon(Int_t ion);
  PndRiemannTrack *GetRiemannTrack(GFTrackCand *cand);
  Double_t GetDPhi(Double_t *par, TVector3 &v1, TVector3 &v2);
  void SetSciTEventCorr(Int_t ev, Int_t mult);

  // void AddAndExpand(Int_t trackID, Int_t detnum, Int_t iHit);
  /** Output array of PndSciTHits **/
  TClonesArray *fmcArray;
  // std::map<Int_t, GFTrackCand*> fTrackCandMap;
  int evt;
  double fQ;

  TH2F *histo;
  TH2F *map1;
  TH2F *map2;
  TH2F *map3;
  TH2F *map4;
  TH2F *histpi;
  TH2F *histk;
  TH2F *histp;
  TH1F *Dpi1;
  TH1F *Dpi2;
  TH1F *Dp2;
  TH1F *Dpix;
  TVector3 fvecp;
  Bool_t friemann;
  Bool_t fLHcut;

  Bool_t fstt;
  Double_t fScFtime;

  TFile *fFile;
  int fVerbose;
  TTree *t;
  TClonesArray *fEvt;
  Int_t activeCnt;
  Double_t weight;
  Double_t seed;
  HypStatDecay *fread;     //! Gives Access to the Statistical decay products
  const Char_t *fFileName; //! Input file name
  // PndGeoSciTPar* fGeoPar;

  /** smearing and geometry access **/
  void InitTransMat();
  void smear(TVector3 &pos);
  void smearLocal(TVector3 &pos);
  void Register();

  void Reset();

  void ProduceHits();

  ClassDef(PndSciTAnaIdeal, 1);
};

#endif
