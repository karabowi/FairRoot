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
 * PndPatternDBMerger.h
 *
 *  Created on: Nov 15, 2017
 *      Author: vagrant
 */

#ifndef PNDTRACKERS_PNDPATTERNMATCHER_PNDPATTERNDBMERGER_H_
#define PNDTRACKERS_PNDPATTERNMATCHER_PNDPATTERNDBMERGER_H_
#include "PndPattern.h"
#include <TChain.h>
#include <TFile.h>
#include <TString.h>
#include <TTree.h>

// number of identical entries, actual entries in file
typedef std::multimap<int, std::set<int>> IdenticalEntries;

class PndPatternDBMerger {
 public:
  PndPatternDBMerger();
  virtual ~PndPatternDBMerger();

  void SetInputFilename(TString filename) { finputFilename = filename; }
  void SetInputTreename(TString treename) { finputTreename = treename; }
  void SetOutputFilename(TString filename) { foutputFilename = filename; }
  void SetOutputTreename(TString treename) { foutputTreename = treename; }

  void Execute();

 private:
  bool InitInputChain();
  void InitOutputTree();

  IdenticalEntries FindIdenticalEntries();
  std::vector<PndPattern> GetPatternsForMerging(std::pair<int, std::set<int>> entryPair);
  PndPattern MergePatterns(std::vector<PndPattern> &entries);
  void FillOutputTree(PndPattern &mergedPattern);

  TChain *fpatternTree;
  TFile *foutputDBFile;
  TTree *foutputTree;

  TString finputFilename;
  TString finputTreename;
  TString foutputFilename;
  TString foutputTreename;

  PndPattern *bPattern;
  PndPattern *bMergedPattern;

  ClassDef(PndPatternDBMerger, 1)
};

#endif /* PNDTRACKERS_PNDPATTERNMATCHER_PNDPATTERNDBMERGER_H_ */
