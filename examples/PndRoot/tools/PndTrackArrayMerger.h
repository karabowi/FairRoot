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
 * @class PndTrackArrayMerger
 * @brief Copies the data of several input TClonesArrays of the same type into one output TClonesArray.
 *
 * @author Ralf Kliemt <r.kliemt@gsi.de>
 * @date  May, 2011
 */
#pragma once

#include "FairTask.h"

#include "TString.h"
#include "TClonesArray.h"
#include <vector>

class PndTrackArrayMerger : public FairTask {

 public:
  PndTrackArrayMerger();
  explicit PndTrackArrayMerger(TString s);
  virtual ~PndTrackArrayMerger();

  virtual void SetParContainers();
  virtual InitStatus Init();
  virtual InitStatus ReInit();
  virtual void Exec(Option_t *opt);
  virtual void FinishEvent();

  void SetPersistance(Bool_t p = kTRUE) { fPersistance = p; }
  void AddInputBranch(TString s) { fInputBranchList.push_back(s); }
  void SetOutputBranch(TString s) { fOutputBranch = s; }

 private:
  Bool_t fPersistance; //! Flag if to store
  TString fOutputBranch;
  TClonesArray *fOutputArray;
  std::vector<TString> fInputBranchList;
  std::vector<TClonesArray *> fInputArrayList;

  PndTrackArrayMerger(const PndTrackArrayMerger &L);
  PndTrackArrayMerger &operator=(const PndTrackArrayMerger &) { return *this; }

  ClassDef(PndTrackArrayMerger, 1);
};
