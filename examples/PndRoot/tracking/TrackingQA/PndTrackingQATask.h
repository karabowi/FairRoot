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
 * @class PndTrackingQATask
 * @brief Task to call PndTrackingQA algorihm
 *
 * @data 21.06.2023
 * @author Tobias Stockmanns <t.stockmanns@fz-juelich.de>
 */

#include "PndTrackFunctor.h"
#include "PndGeoSttPar.h"
#include "PndTrackingQASummary.h"

#include "FairTask.h"

#include "TObject.h"
#include "TString.h"

#include <vector>

class TClonesArray;

#pragma once

class PndTrackingQATask : public FairTask {
 public:
  PndTrackingQATask(TString trackBranchName, TString idealBranchName, Bool_t pndTrackData = kTRUE);
  virtual ~PndTrackingQATask();

  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  virtual void Finish();

  //! Sets the branch name of the ideal tracking data which serves as a reference
  void SetIdealTrackBranchName(TString name) { fIdealTrackBranchName = name; };

  //! Sets the branch name of the track data which should be analyzed.
  void SetTrackBranchName(TString name, Bool_t pndTrackData = kTRUE)
  {
    fPndTrackNotTrackCand = pndTrackData;
    fTrackBranchName = name;
  } // alternatively PndTrackCandData can be analysed. Therefore pndTrackData has to be set to kFALSE

  //! Adds branch names of detector data which should be taken into account in the analysis
  void AddHitsBranchName(TString name) { fBranchNames.push_back(name); }

  void SetOutputMCInfoBranchName(TString name) { fMCInfoBranchName = name; }
  void SetOutputRecoInfoBranchName(TString name) { fRecoInfoBranchName = name; }

  void SetFunctorName(TString name) { fPossibleTrackFunctorName = name; }
  void SetFunctor();

 protected:
  void FillQASummary();
  int GetNGhosts();

 private:
  std::vector<TString> fBranchNames;              //!
  TString fMCInfoBranchName, fRecoInfoBranchName; //!

  PndTrackingQASummary fSummary;

  TClonesArray *fTrack = nullptr;
  TClonesArray *fMCTrack = nullptr;
  TClonesArray *fSttHitArray = nullptr;
  TClonesArray *fMCTrackInfo = nullptr;
  TClonesArray *fRecoTrackInfo = nullptr;
  TClonesArray *fIdealTrack = nullptr;
  TClonesArray *fSttTubeArray = nullptr;
  PndGeoSttPar *fSttParameters = nullptr;

  FairRootManager *ioman = nullptr;

  TString fPossibleTrackFunctorName;
  PndTrackFunctor *fPossibleTrackFunctor = nullptr;

  TString fTrackBranchName;
  TString fIdealTrackBranchName;
  Bool_t fPndTrackNotTrackCand = kTRUE; // kTRUE if track and kFALSE if track cand

  ClassDef(PndTrackingQATask, 1);
};
