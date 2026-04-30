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
 * PndPatternDBGenerator.h
 *
 *  Created on: Nov 8, 2017
 *      Author: Michael Papenbrock
 */

#ifndef PNDTRACKERS_PNDPATTERNMATCHER_PNDPATTERNDBGENERATOR_H_
#define PNDTRACKERS_PNDPATTERNMATCHER_PNDPATTERNDBGENERATOR_H_

#include <FairTask.h>
#include <PndGeoSttPar.h>
#include <PndMCTrack.h>
#include <PndPattern.h>
#include <PndSttHit.h>

#include <TTree.h>
#include <TClonesArray.h>

typedef std::vector<PndSttHit *> HitArray;
typedef std::multimap<int, int> HitSectorMap;
typedef std::vector<PndPattern> PndPatterns;

class PndPatternDBGenerator : public FairTask {
 public:
  PndPatternDBGenerator();
  virtual ~PndPatternDBGenerator();

  void SetOutputFilename(TString filename = "patternDB.root") { foutputFilename = filename; }

 protected:
  virtual void SetParContainers();
  virtual InitStatus Init();
  virtual void Exec(Option_t *opt);
  virtual void FinishTask();

 private:
  void GenerateTrackPatterns();
  HitSectorMap FillSectorHitMap(HitArray sttHitArray);
  PndPatterns FillSectorPatterns(HitArray hitArray, PndMCTrack *mcTrack);
  PndPatterns FillTrackPatterns(HitArray hitArray, PndMCTrack *mcTrack);
  void AddPatternsToTree(PndPatterns patterns, TTree *tree);

  PndGeoSttPar *fSttParameters;
  TClonesArray *fEventHeader;
  TClonesArray *fTubeArray;
  TClonesArray *fSttHitArray;
  TClonesArray *fMCTrackArray;
  TClonesArray *trackCands;

  TFile *foutputFile;
  TTree *fsectorPatternTree;
  TTree *ftrackPatternTree;

  PndPattern *bPattern;

  int sttBranchID;
  int mcTrackID;
  TString foutputFilename;

  int nTotalTracks;
  int nMultipleMCTrackLinks;

  ClassDef(PndPatternDBGenerator, 1)
};

#endif /* PNDTRACKERS_PNDPATTERNMATCHER_PNDPATTERNDBGENERATOR_H_ */
