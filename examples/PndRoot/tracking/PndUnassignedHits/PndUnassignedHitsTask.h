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
 * PndUnassignedHitsTask.h
 *
 *  Created on: Nov 22, 2018
 *      Author: Tobias Stockmanns <t.stockmanns@fz-juelich.de>
 */

#ifndef PndUnassignedHitsTask_H_
#define PndUnassignedHitsTask_H_

#include "PndPersistencyTask.h"

#include "FairMultiLinkedData.h"

#include <map>

class TClonesArray;
class FairMultiLinkedData;

class PndUnassignedHitsTask : public PndPersistencyTask {
 public:
  PndUnassignedHitsTask() : PndPersistencyTask("Unassigned Hits Task"), fUnassignedBranchExtension("Unassigned"){};
  virtual ~PndUnassignedHitsTask(){};

  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  virtual void AddHitBranch(TString branchName);
  virtual void AddTrackCands(TString trackCandName);
  virtual void SetUnassignedBranchExtension(TString name) { fUnassignedBranchExtension = name; };
  void SetVerbose(Int_t verbose) { fVerbose = verbose; };

 protected:
  void RegisterBranches();
  void RegisterTrackCands();
  void FillTrackCands();
  void FillUnassignedHits(TString branchName);
  void FillOutputBranch(TString branchName, std::vector<int> unusedHits);

 private:
  std::map<TString, TClonesArray *> fHitBranches;           //!
  std::map<TString, TClonesArray *> fUnassignedHitBranches; //!
  std::map<TString, TClonesArray *> fTrackCands;            //!
  FairMultiLinkedData fHitsInTracks;                        //!
  TString fUnassignedBranchExtension;
  ClassDef(PndUnassignedHitsTask, 1);
};

#endif /* PndUnassignedHitsTask_H_ */
