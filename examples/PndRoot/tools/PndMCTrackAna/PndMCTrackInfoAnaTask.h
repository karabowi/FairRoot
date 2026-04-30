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


/** @class PndMCTrackInfoAnaTask
 *  @author Tobias Stockmanns <t.stockmanns@fz-juelich.de>
 *  @brief Analyses PndMCTrackInfo objects to determine at which reconstruction stage
 *  a reconstruction of a particle failed
 **
 **/

#pragma once

#include "PndPersistencyTask.h"
#include "PndMCTrackInfo.h"
#include "PndMCTrack.h"

#include "PndEventRequirements.h"
#include "PndEventCheck.h"

#include "TClonesArray.h"

#include <string>
#include <vector>
#include <map>

class TClonesArray;

class PndMCTrackInfoAnaTask : public PndPersistencyTask {
 public:
  /** Default constructor **/
  PndMCTrackInfoAnaTask();

  /** Destructor **/
  virtual ~PndMCTrackInfoAnaTask();

  virtual void SetParContainers();
  virtual InitStatus Init();
  virtual InitStatus ReInit();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  /**
   * Defines the requirements for a reconstructed event. See PndEventRequirements or PndParticleRequirements
   * @param eventRequirements PndEventRequirements object
   */
  void SetEventRequirements(PndEventRequirements eventRequirements) { fEventCheck.SetRequirements(eventRequirements); }

 protected:
  void Register();
  void Reset();
  void FinishTask();
  void SetupEventRequirements();

  void AssignTClonesArrays(std::map<TString, TClonesArray *> &map);
  void AnalyseResult(PndEventCheck &eventCheck);
  int GetPointOfFailure(std::vector<int> result);
  void AddPointOfFailure(int requirement, int position);
  void InitPointOfFailure();
  void InitInterestingEvents();
  std::vector<std::vector<std::vector<int>>> GetInterestingEvents() const { return fInterestingEvents; }  ///< particle, requirement, list of events with failure

 private:
  PndEventCheck fEventCheck;
  TClonesArray *fMCTracks = nullptr;
  TClonesArray *fMCTrackInfo = nullptr;
  TList *fBranchList = nullptr;
  std::vector<int> fReqResult;
  std::vector<std::vector<int>> fPointOfFailure;
  std::vector<std::vector<std::vector<int>>> fInterestingEvents;

  ClassDef(PndMCTrackInfoAnaTask, 1);
};

