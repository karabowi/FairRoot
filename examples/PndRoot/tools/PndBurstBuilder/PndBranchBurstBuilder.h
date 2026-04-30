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
 * @class PndBranchBurstBuilder
 *
 * @brief Basic Class for building bursts of events in the simulation.
 * @details
 * For the different simulation types (event based, semi time based, time based) the appropriate classes must be derived
 * the pure virtual function "GetBurstData" must be implemented.
 *
 * @date July 12, 2017
 * @author Dominik Steinschaden
 *
 */

#pragma once

#include "TClonesArray.h"
#include "FairTask.h"
#include <vector>

class PndBranchBurstBuilder : public FairTask {
 public:
  /** Default constructor **/
  PndBranchBurstBuilder();
  /** Destructor **/
  virtual ~PndBranchBurstBuilder();

  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  virtual TClonesArray *GetBurstData(size_t inBranchNum) = 0;

  /// Set OutputBranchName prefix. Default one is "Burst"
  void SetOutputPrefix(TString prefix) { fOutputPrefix = prefix; };

  /// Set and add the branches for which you want to combine events
  void AddInputBranch(TString branchName) { fInBranchNames.push_back(branchName); };

  /// Set Persistance to kTRUE if you want to store the combined Events. otherwise they are just buffered for the direct use of other tasks in the same run
  void SetPersistence(Bool_t val) { fPersistence = val; };

 protected:
  std::vector<TString> fInBranchNames;
  std::vector<TClonesArray *> fInArrays;
  std::vector<TClonesArray *> fOutArrays;

  TString fOutputPrefix;
  Bool_t fPersistence;

  ClassDef(PndBranchBurstBuilder, 0);
};

