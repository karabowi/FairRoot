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
 * @class PndMCTruthMatch
 * @brief Prints the MCTrack info for all data objects in a branch
 *
 * @details
 * This task prints for all data objects of a given branch the matching MCTrack information
 * as long as the data derives from FairMultiLinkedData_Interface.
 *
 * @author Tobias Stockmanns <t.stockmanns@fz-juelich.de>
 * @date September, 2015

 */

#pragma once

// framework includes
#include "FairTask.h"

#include "TClonesArray.h"
#include <math.h>


class PndMCTruthMatch : public FairTask {
 public:
  PndMCTruthMatch();
  explicit PndMCTruthMatch(TString inBranchName);
  virtual ~PndMCTruthMatch();

  virtual InitStatus Init();
  virtual void SetInBranchName(TString name) { fInBranchName = name; }

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

 private:
  TClonesArray *fMCTrack = nullptr;
  TClonesArray *fInBranch = nullptr;

  TString fInBranchName = "";

  ClassDef(PndMCTruthMatch, 1);
};
