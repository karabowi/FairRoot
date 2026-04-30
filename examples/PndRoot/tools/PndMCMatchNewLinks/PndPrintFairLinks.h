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
 * @class PndPrintFairLinks
 * @author T.Stockmanns <t.stockmanns@fz-juelich.de>
 *
 * @brief Prints all FairLinks of a given branch to the screen
 **/

#pragma once

// framework includes
#include "FairTask.h"

#include <vector>
#include <map>
#include "TObjString.h"

class TClonesArray;

class PndPrintFairLinks : public FairTask {
 public:
  /** Default constructor **/
  PndPrintFairLinks();

  /** Destructor **/
  virtual ~PndPrintFairLinks();


  virtual void AddBranchName(const TString &name) { fSelectedBranches->AddLast(new TObjString(name.Data())); }

  virtual void PrintBranchNameList(TList *branches);

  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  virtual void Finish();

 protected:
  void InitBranchList(TList *branches);

 private:
  std::map<Int_t, TClonesArray *> fBranches;
  TList *fSelectedBranches;

  void Register();

  void Reset();

  void ProduceHits();

  ClassDef(PndPrintFairLinks, 1);
};

