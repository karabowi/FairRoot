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
 * PatternMatcher.h
 *
 *  Created on: Jun 16, 2017
 *      Author: Michael Papenbrock
 */

#ifndef PNDTRACKERS_PATTERNMATCHINGTESTS_PATTERNMATCHER_H_
#define PNDTRACKERS_PATTERNMATCHINGTESTS_PATTERNMATCHER_H_

#include <FairTask.h>
#include <PndGeoSttPar.h>
#include <PndTrackCand.h>
#include <TClonesArray.h>
#include <TFile.h>
#include <TTree.h>

class PndPatternMatcher : public FairTask {
 public:
  PndPatternMatcher();
  virtual ~PndPatternMatcher();

  void SetOutputBranchname(TString name) { fTrackCandName = name; }
  void SetPersistence(Bool_t val) { fPersistence = val; }
  void SetMatchRatio(float ratio) { fMatchRatio = ratio; }

 protected:
  virtual void SetParContainers();
  virtual InitStatus Init();
  virtual void Exec(Option_t *opt);
  virtual void FinishTask();

 private:
  void LoadPatternDB(TString filename);
  void FindMatch();
  void CreateTrackCandFromMatch(std::set<int> partialMatch);

  //  Input data
  PndGeoSttPar *fSttParameters;
  TClonesArray *fEventHeader;
  TClonesArray *fSTTHitArray;

  TFile *dbFile;
  TTree *patternTree;
  std::vector<std::set<int>> dbTubeIDs;

  //  Output data
  std::vector<PndTrackCand> fPartialCand;
  TClonesArray *fPartialMatchCand;
  TString fTrackCandName;

  Bool_t fPersistence;

  bool verboseMatches;
  float fMatchRatio;

  ClassDef(PndPatternMatcher, 1)
};

#endif /* PNDTRACKERS_PATTERNMATCHINGTESTS_PATTERNMATCHER_H_ */
