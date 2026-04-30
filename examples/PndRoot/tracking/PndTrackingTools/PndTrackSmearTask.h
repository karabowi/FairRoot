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
 * PndTrackSmearTask.h
 *
 * \brief Task to flag or to remove tracks with missing Pz information
 *
 * This task takes as an input any PndTrack branch. It goes through all the tracks. For those tracks only consisting of STTHits
 * and with a ParamFirst::pz value between 0.015 and 0.025 it either sets the PndTrack::fFlag to -30 or deletes the track.
 * What action is taken is selected by SetRemoveTrack(). If SetRemoveTrack is set to kTRUE the track is deleted otherwise flaged.
 * The filtered PndTracks are written in an output branch either with a given name by SetOutputTrackBranch() of, if no branch name is
 * given by InputTrackBranchName + "_filtered".
 *
 *  Created on: 22.3.2017
 *      Author: t.stockmanns
 */

#ifndef PndTrackSmearTask_H_
#define PndTrackSmearTask_H_

#include "PndPersistencyTask.h"
#include "TString.h"
#include "TVector3.h"
#include "FairTrackParP.h"

class TClonesArray;

class PndTrackSmearTask : public PndPersistencyTask {
 public:
  PndTrackSmearTask() : PndPersistencyTask("Missing Pz Track Cleaner Task"), fInputTrackBranches(), fOutputTrackBranches(), fTracks(0), fMomSigma(0.03, 0.03, 0.03)
  {
    SetPersistency(kTRUE);
  };

  PndTrackSmearTask(const TString &branchName)
    : PndPersistencyTask("Missing Pz Track Cleaner Task"), fInputTrackBranches(), fOutputTrackBranches(), fTracks(0), fMomSigma(0.03, 0.03, 0.03)
  {
    SetPersistency(kTRUE);
    AddInputTrackBranch(branchName);
  };

  virtual ~PndTrackSmearTask(){};

  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);
  virtual void FinishEvent();
  virtual void Finish();

  void SetMomSmearing(TVector3 dpop) { fMomSigma = dpop; }
  void AddInputTrackBranch(const TString &name)
  {
    fInputTrackBranches.push_back(name);
    fOutputTrackBranches.push_back(name + "_fakeonline");
  }
  void SetPersistence(const Bool_t &val) { SetPersistency(val); }

 private:
  FairTrackParP SmearTrackPar(FairTrackParP par);
  void SmearMom(TVector3 &vec, Double_t Cov66[6][6]);

  std::vector<TString> fInputTrackBranches;
  std::vector<TString> fOutputTrackBranches;
  std::vector<TClonesArray *> fTracks;
  std::vector<TClonesArray *> fOutputTracks;

  TVector3 fMomSigma; ///< Momentum smearing sigma (dp/p)

  ClassDef(PndTrackSmearTask, 1);
};

#endif /* PndTrackSmearTask_H_ */
