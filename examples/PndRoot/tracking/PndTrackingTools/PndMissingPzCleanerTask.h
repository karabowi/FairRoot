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
 * PndMissingPzCleanerTask.h
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

#ifndef PndMissingPzCleanerTask_H_
#define PndMissingPzCleanerTask_H_

#include "PndPersistencyTask.h"
#include "TString.h"

class TClonesArray;

class PndMissingPzCleanerTask : public PndPersistencyTask {
 public:
  PndMissingPzCleanerTask()
    : PndPersistencyTask("Missing Pz Track Cleaner Task"), fInputTrackBranch(""), fOutputTrackBranch(""), fTracks(nullptr), fFilteredTracks(nullptr), fRemoveTrack(kFALSE)
  {
    SetPersistency(kTRUE);
  };
  PndMissingPzCleanerTask(const TString &branchName)
    : PndPersistencyTask("Missing Pz Track Cleaner Task"), fInputTrackBranch(branchName), fOutputTrackBranch(""), fTracks(nullptr), fFilteredTracks(nullptr), fRemoveTrack(kFALSE)
  {
    SetPersistency(kTRUE);
  };
  virtual ~PndMissingPzCleanerTask(){};

  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);
  virtual void FinishEvent();
  virtual void Finish();

  void SetInputTrackBranch(const TString &name) { fInputTrackBranch = name; }
  void SetOutputTrackBranch(const TString &name) { fOutputTrackBranch = name; }
  void SetRemoveTrack(const Bool_t &remove) { fRemoveTrack = remove; }
  void SetPersistence(const Bool_t &val) { SetPersistency(val); }

 private:
  TString fInputTrackBranch;
  TString fOutputTrackBranch;

  TClonesArray *fTracks;
  TClonesArray *fFilteredTracks;

  Bool_t fRemoveTrack; ///< if this flag is set than the track will not be written to the output branch. Otherwise the flag -30 will be set.

  ClassDef(PndMissingPzCleanerTask, 1);
};

#endif /* PndMissingPzCleanerTask_H_ */
