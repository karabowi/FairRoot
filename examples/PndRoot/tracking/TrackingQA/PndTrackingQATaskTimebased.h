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
// -----                 PndTrackingQATaskTimebased header file             -----
// -----                  Created 18/01/10  by T.Stockmanns             -----
// -------------------------------------------------------------------------

/** PndTrackingQATaskTimebased.h
 *@author T.Stockmanns <t.stockmanns@fz-juelich.de>
 **
 **
 **/

#ifndef PndTrackingQATaskTimebased_H
#define PndTrackingQATaskTimebased_H

// framework includes
#include "FairTask.h"
#include "PndDetectorList.h"
#include "PndTrack.h"
#include "PndTrackCand.h"
#include "PndTrackingQualityMCInfo.h"
#include "PndGeoSttPar.h"
#include "TH2.h"
#include "THStack.h"
#include "RhoTuple.h"
#include "PndTrackFunctor.h"

#include "PndTrackingCloneInfo.h"
#include <vector>
#include <map>

class TClonesArray;

class PndTrackingQATaskTimebased : public FairTask {
 public:
  /** Default constructor **/
  PndTrackingQATaskTimebased(TString trackBranchName, TString idealBranchName, Bool_t pndTrackData = kTRUE);

  /** Destructor **/
  virtual ~PndTrackingQATaskTimebased();

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

  void SetFunctorName(TString name) { fPossibleTrackFunctorName = name; }
  void SetFunctor();

  void SetRunTimeBased(bool runTimeBased) { fRunTimeBased = runTimeBased; }

  // changes the definition of primary and secondary
  // a primary track is then defined as a track with dPCA (distance from point of closest approach to IP) in the xy-projection < a specified distance
  // currently the dPCA is calculated in the xy-projection, where the particle track is assumed to be a circle.
  void SetSecondaryDefinitionPCAXY(double dPCA)
  {
    fSecondaryDefinitionPCAXY = kTRUE;
    fdPCA = dPCA;
  }

 private:
  double fSumTime;

  //  virtual void FillMapTrackQualifikation();
  const TString fHypoName[5] = {"Electron", "Muon", "Pion", "Kaon", "Proton"}; //!

  /** Analyses the track data and assigns quality indicator to track.
   * Quality indicators:
   * 1  : Not assigned
   * 2  : All hits of the MC track were found and no additional once
   * 3  : Some hits of the MC track were found and no additional once
   * 4  : 70 % of all hits found belong to this MC track
   */

  virtual void FillQualyHisto(std::map<Int_t, Int_t> trackQualifikation, Int_t nGhosts, Int_t nClones);
  virtual void FillQualyHistoTimeBased(std::map<FairLink, Int_t> trackQualifikation, Int_t nGhosts);
  virtual void FillMCStatus(std::map<Int_t, Int_t> trackMCStatus);
  virtual void FillMCStatusTimeBased(std::map<FairLink, Int_t> trackMCStatus);
  virtual void FillEfficiencies(std::map<Int_t, std::map<TString, std::pair<Double_t, Int_t>>> efficiencies);
  virtual void FillEfficienciesTimeBased(std::map<FairLink, std::map<TString, std::pair<Double_t, Int_t>>> efficiencies);
  virtual void MapToHist(std::map<Int_t, Double_t>, TH1 *);
  virtual void MapToHistTimeBased(std::map<FairLink, Double_t>, TH1 *);

  virtual void SetQualyHisto(TH1 *histo, Bool_t relative, Int_t base = 1);

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
  std::map<TString, TH2 *> fMapEfficiencies; //!

  std::map<int, int> fMCInfoIdIdealId;               //!
  std::map<FairLink, int> fTimeBasedMCInfoIdIdealId; //!

  Int_t fNGhosts;

  Double_t clonesTimeBasedTotal;
  Int_t fTotClonesTimeBased;

  TClonesArray *fTrack;
  TClonesArray *fMCTrack;
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

  TString fPossibleTrackFunctorName;

  bool fRunTimeBased; // Set to kTRUE/true to run time based, kFALSE/false is default and makes the task run event based

  std::map<FairLink, Int_t> fTimeBasedMapTrackMCStatusForCloneCalc;

  PndTrackFunctor *fPossibleTrackFunctor;

  RhoTuple *fTuple;
  TH1 *fIdealTracksPerEvent;
  TH1 *fIdealPHisto;
  TH1 *fIdealPtHisto;
  TH1 *fIdealPlHisto;
  TH1 *fIdealPrimTracksPerEvent;
  TH1 *fIdealSecTracksPerEvent;
  TH1 *fPHisto;
  TH1 *fPRelHisto;
  TH1 *fPtHisto;
  TH1 *fPtRelHisto;
  TH1 *fPlHisto;
  TH1 *fPlRelHisto;
  TH1 *fQualyHisto;
  THStack *fQualyStack;
  TH1 *fQualyHisto_mc;
  TH1 *fQualyHisto_neg;
  TH1 *fQualyHisto_pos;
  TH1 *fQualyHisto_all;
  TH1 *fQualyHisto_rel_all;
  TH1 *fQualyHisto_rel_possible;

  TH1 *fEventPurityHisto;

  Int_t fEventNr;
  bool fSecondaryDefinitionPCAXY;
  double fdPCA;

  void Register();

  void Reset();

  ClassDef(PndTrackingQATaskTimebased, 1);
};

#endif
