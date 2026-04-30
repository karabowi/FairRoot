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
// -----                 PNDMCMATCHSELECTORTASK header file             -----
// -----                  Created 18/01/10  by T.Stockmanns             -----
// -------------------------------------------------------------------------

/** PNDMCMATCHSELECTORTASK.h
 *@author T.Stockmanns <t.stockmanns@fz-juelich.de>
 **
 ** Displays all available informations for a given event
 **/

#ifndef PndTrackingQualityBarrelTaskNewLinks_H
#define PndTrackingQualityBarrelTaskNewLinks_H

// framework includes
#include "FairTask.h"
#include "PndDetectorList.h"
#include "PndTrackCand.h"
#include "PndTrackingQualityMCInfo.h"
#include "PndGeoSttPar.h"
#include "TH2.h"
#include "THStack.h"
#include "RhoTuple.h"

#include <vector>
#include <map>

class TClonesArray;
class PndTrack;

class PndTrackingQualityBarrelTaskNewLinks : public FairTask {
 public:
  /** Default constructor **/
  PndTrackingQualityBarrelTaskNewLinks(TString trackBranchName, TString idealBranchName, Bool_t pndTrackData = kTRUE);

  /** Destructor **/
  virtual ~PndTrackingQualityBarrelTaskNewLinks();

  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  virtual void Finish();

  void SetIdealTrackBranchName(TString name) { fIdealTrackBranchName = name; };

  //! Sets the branch name of the track data which should be analyzed.
  void SetTrackBranchName(TString name, Bool_t pndTrackData = kTRUE)
  {
    fPndTrackOrTrackCand = pndTrackData;
    fTrackBranchName = name;
  } // alternatively PndTrackCandData can be analysed. Therefore pndTrackData has to be set to kFALSE

  //! Adds branch names of detector data which should be taken into account in the analysis
  void AddHitsBranchName(TString name) { fBranchNames.push_back(name); }

  void SetOutputMCInfoBranchName(TString name) { fMCInfoBranchName = name; }
  void SetOutputRecoInfoBranchName(TString name) { fRecoInfoBranchName = name; }

 private:
  //
  //  virtual void FillMapTrackQualifikation();

  /** Analyses the track data and assigns quality indicator to track.
   * Quality indicators:
   * 1  : Not assigned
   * 2  : All hits of the MC track were found and no additional once
   * 3  : Some hits of the MC track were found and no additional once
   * 4  : 70 % of all hits found belong to this MC track
   */

  virtual void FillQualyHisto(std::map<Int_t, Int_t> trackQualifikation, Int_t nGhosts);
  virtual void FillMCStatus(std::map<Int_t, Int_t> trackMCStatus);
  virtual void FillEfficiencies(std::map<Int_t, std::map<TString, std::pair<Double_t, Int_t>>> efficiencies);
  virtual void MapToHist(std::map<Int_t, Double_t>, TH1 *);

  virtual Int_t GetSumOfAllValidMCHits(FairMultiLinkedData *trackData);

  void AssociateRecoTracksToMCTracks();

  PndTrackingQualityMCInfo GetMCInfoFromIdealTrack(PndTrack *idealtrack);
  Int_t GetMCInfoIdFromIdealTrackId(int idealtrackid) { return fMCInfoIdIdealId[idealtrackid]; }

  void InitializeHistograms();
  void LabelQualyHistogram(TH1 *);
  void ColorHistogram();
  std::vector<TString> fBranchNames;              //!
  TString fMCInfoBranchName, fRecoInfoBranchName; //!

  std::map<TString, FairMultiLinkedData> fMapLinkData; //!
                                                       //  std::map<TString, Double_t> fPossibleTrackParameter;
  std::map<TString, TH2 *> fMapEfficiencies;           //!

  std::map<int, int> fMCInfoIdIdealId; //!

  Int_t fNGhosts;

  TClonesArray *fTrack;
  TClonesArray *fMCTrack;
  TClonesArray *fTrackCand;
  //  TClonesArray* fIdealTrackCand;
  TClonesArray *fSttHitArray;
  TClonesArray *fMCTrackInfo;
  TClonesArray *fRecoTrackInfo;
  TClonesArray *fIdealTrack;
  TClonesArray *fSttTubeArray;

  FairRootManager *ioman;

  std::map<Int_t, Int_t> fMapTrackQualifikation; //!

  TString fTrackBranchName;
  TString fIdealTrackBranchName;
  Bool_t fPndTrackOrTrackCand; // kTRUE if track and kFALSE if track cand
  PndGeoSttPar *fSttParameters;

  RhoTuple *fTuple;

  TH1 *fPHisto;
  TH1 *fPRelHisto;
  TH1 *fPtHisto;
  TH1 *fPtRelHisto;
  TH1 *fQualyHisto;
  THStack *fQualyStack;
  TH1 *fQualyHisto_mc;
  TH1 *fQualyHisto_neg;
  TH1 *fQualyHisto_pos;
  TH1 *fQualyHisto_all;

  Int_t fEventNr;

  void Register();

  void Reset();

  ClassDef(PndTrackingQualityBarrelTaskNewLinks, 1);
};

#endif
