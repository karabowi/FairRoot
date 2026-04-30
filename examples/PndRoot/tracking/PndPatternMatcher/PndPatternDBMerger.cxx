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
 * PndPatternDBMerger.cxx
 *
 *  Created on: Nov 15, 2017
 *      Author: vagrant
 */

#include "PndPatternDBMerger.h"
#include <iostream>
#include <utility>
#include "boost/range/adaptor/reversed.hpp"

ClassImp(PndPatternDBMerger)

  PndPatternDBMerger::PndPatternDBMerger()
{
  fpatternTree = nullptr;
  foutputDBFile = nullptr;
  foutputTree = nullptr;

  bPattern = nullptr;
  bMergedPattern = nullptr;

  finputFilename = "patternDB.root";
  finputTreename = "trackPatterns";
  foutputFilename = "patternDBclean.root";
  foutputTreename = finputTreename;
}

PndPatternDBMerger::~PndPatternDBMerger() {}

void PndPatternDBMerger::Execute()
{
  std::cout << "Executing PndPatternDBMerger" << std::endl;

  bool bInput = InitInputChain();
  if (bInput) {
    std::cout << "Input chain successfully initialised" << std::endl;
  }

  InitOutputTree();

  IdenticalEntries entriesToBeMerged;
  entriesToBeMerged = FindIdenticalEntries();

  std::cout << "Merged entries to be created: " << entriesToBeMerged.size() << std::endl;
  for (auto iter : boost::adaptors::reverse(entriesToBeMerged)) {
    std::vector<PndPattern> patternsToBeMerged = GetPatternsForMerging(iter);
    PndPattern mergedPattern = MergePatterns(patternsToBeMerged);
    FillOutputTree(mergedPattern);
  }

  std::cout << "Saving output tree" << std::endl;
  foutputTree->AutoSave();
  foutputDBFile->Close();
}

bool PndPatternDBMerger::InitInputChain()
{
  std::cout << "Initialising input chain with TTree" << finputTreename << std::endl;
  fpatternTree = new TChain(finputTreename);
  std::cout << "Adding input file(s) with name" << finputFilename << std::endl;
  fpatternTree->Add(finputFilename);

  fpatternTree->SetBranchAddress("pattern", &bPattern);

  if (!fpatternTree)
    return false;
  return true;
}
void PndPatternDBMerger::InitOutputTree()
{
  std::cout << "Initialising output file and tree" << std::endl;
  foutputDBFile = new TFile(foutputFilename, "RECREATE");
  foutputTree = new TTree(foutputTreename, "Tree containing merged patterns");
  foutputTree->Branch("pattern", &bMergedPattern);
}
IdenticalEntries PndPatternDBMerger::FindIdenticalEntries()
{
  std::cout << "Finding identical patterns in input chain" << std::endl;

  //  pattern, entries in file for corresponding pattern
  typedef std::map<std::set<int>, std::set<int>> PatternEntries;
  PatternEntries patternEntries;
  Long64_t nEntries = fpatternTree->GetEntries();
  std::cout << "Entries to process: " << nEntries << std::endl;

  for (Long64_t iEntry = 0; iEntry < nEntries; ++iEntry) {
    if (iEntry % 10000 == 0) {
      std::cout << "processing entry " << iEntry << " of " << nEntries << std::endl;
    }
    fpatternTree->GetEntry(iEntry);
    std::set<int> tubeIDs = bPattern->GetTubeIDs();
    patternEntries[tubeIDs].insert(iEntry);
  }

  std::cout << "Unique patterns found in input chain: " << patternEntries.size() << std::endl;

  std::cout << "Creating map with entries to be merged" << std::endl;
  //  number of entries, actual entries in file
  IdenticalEntries mapEntries;
  for (auto iter : patternEntries) {
    std::set<int> &vEntries = iter.second;
    //    set this to 1 to only consider recurring patterns
    if (vEntries.size() > 0) {
      mapEntries.insert(std::make_pair(vEntries.size(), vEntries));
    }
  }

  return mapEntries;
}
std::vector<PndPattern> PndPatternDBMerger::GetPatternsForMerging(std::pair<int, std::set<int>> entryPair)
{
  std::vector<PndPattern> patternsToBeMerged;
  for (auto const &entry : entryPair.second) {
    fpatternTree->GetEntry(entry);
    patternsToBeMerged.push_back(*bPattern);
  }
  return patternsToBeMerged;
}
PndPattern PndPatternDBMerger::MergePatterns(std::vector<PndPattern> &entries)
{
  PndPattern merged;
  if (entries.size() == 0)
    return merged;
  merged.SetTubeIDs(entries.at(0).GetTubeIDs());
  merged.SetSectorID(entries.at(0).GetSectorID());

  for (auto const &pattern : entries) {
    int currentCount = merged.GetCount();
    int newCount = currentCount + pattern.GetCount();
    merged.SetPatternCount(newCount);
    std::vector<TVector3> momenta = pattern.GetMomenta();
    merged.AddMomenta(momenta);
  }
  return merged;
}
void PndPatternDBMerger::FillOutputTree(PndPattern &mergedPattern)
{
  bMergedPattern = &mergedPattern;
  foutputTree->Fill();
}
