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

/** @class PndMCTrackInfoTask
 ** @brief Generates additional information for MCTracks stored in PndMCTrackInfo. Called via "/macro/tools/MCTrackInfo.C".
 ** See PndMCTrackInfo for more details
 ** @author Tobias Stockmanns <t.stockmanns@fz-juelich.de>
 **
 **/

#pragma once

#include "PndPersistencyTask.h"
#include "PndMCTrackInfo.h"
#include "PndMCTrack.h"
#include "PndPidProbability.h"

#include "TClonesArray.h"

#include <string>
#include <vector>
#include <map>

class TClonesArray;

class PndMCTrackInfoTask : public PndPersistencyTask {
 public:
  /** Default constructor **/
  PndMCTrackInfoTask();

  /** Destructor **/
  virtual ~PndMCTrackInfoTask();

  virtual void SetParContainers();
  virtual InitStatus Init();
  virtual InitStatus ReInit();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  virtual void FinishEvent();
  virtual void FinishTask();

  /**
   * Sets the maximum stage for how many generations (stages) of daughter particles a PndMCTrackInfo is generated
   * @param stage maximum stage
   */
  void SetMaxStage(int stage) { fMaxStage = stage; }

  /**
   * Sets the maximum stage for how many generations (stages) of neutral daughter particles a PndMCTrackInfo is generated.
   * This additional limit for neutral particles is needed to reduce the processing times.
   * @param stage maximum stage
   */
  void SetMaxStageNeutralCheck(int stage) { fMaxStageNeutralCheck = stage; }

  /**
   * Sets the relative momentum difference (mcp - p)/mcp up to a reconstructed neutral track is still regarded as found
   * @param stage maximum stage
   */
  void SetMaxMomentumDifferenceNeutralCheck(double val) { fMaxNeutralMomDiff = val; }

  /**
   * Adds a branch with MC points to be investigated.
   * If no branches are given, all tracking detectors are added
   * @param name branch name
   */
  void AddPointBranchName(TString name) { fPointBranches[name]; }

  /**
   * Adds a branch which output of a tracking algorithm for the analysis.
   * If no branches are given, "BarrelTrack" and "FtsIdealTrack" are taken
   * @param name branch name
   */
  void AddTrackingBranchName(TString name) { fTrackingBranches[name]; }

  /**
   * Adds a branch with charged or neutral candidates to the analysis.
   * If no branches are given all candidate branches are taken
   * @param name branch name
   */
  void AddCandidateBranchName(TString name) { fCandidateBranches[name]; }

  /**
   * Adds a branch with a pid algorithm to the analysis.
   * If no branches are given all pid branches are taken
   * @param name branch name
   */
  void AddPidBranchName(TString name) { fPidBranches[name]; }

//  void AddBranchName(TString name) { fBranches[name]; }

  void UseKalmanTracks() { fUseKalmanTracks = true; }
  void UsePidInfo() { fUsePidInfo = true; }
  void UseChargeCands() { fUseChargeCands = true; }

  void PrintPrimariesWithDaughters(int stage = 0);

 protected:
  void Register();
  void Reset();

  std::vector<int> GetPIDMothers(int motherID);
  void PrintDaughters(PndMCTrackInfo mother, int stage);
  void AssignPoints();
  void AssignTrackingBranches();
  void AssignCandidateBranches();
  void AssignPidBranches(int mcIndex, int chargedCandIndex);
  //  double GetPidProbability(PndPidProbability *prob, int pdgCode);
  void AssignBranches();
  void FillOutputArray();
  void FillPointInfo();
  void FillTypesNumbers();

  void AssignTClonesArrays(std::map<TString, TClonesArray *> &map);
  void AssignNeutralCand(TClonesArray *neutralbranch);

 private:
  TClonesArray *fMCTracks = nullptr;
  TClonesArray *fMCTrackInfo = nullptr;
  std::map<TString, TClonesArray *> fPointBranches;
  std::map<TString, TClonesArray *> fTrackingBranches;
  std::map<TString, TClonesArray *> fCandidateBranches;
  std::map<TString, TClonesArray *> fPidBranches;
//  std::map<TString, TClonesArray *> fBranches;
  std::map<int, PndMCTrackInfo> fTrackInfo;
  std::vector<int> fMCIdPhoton;  // needed to check for neutral candidates matching with photon momentum

  bool fUseKalmanTracks;
  bool fUsePidInfo;
  bool fUseChargeCands;
  double fMaxNeutralMomDiff;  // Maximum difference of (mcmomentum - recomomentum)/mcmomentum for neutrals
  int fMaxStageNeutralCheck;  // Maximum stage the check for matching neutral cand for photon is done (if equal to MaxStage the test is very time consuming)
  int fMaxStage;              ///< maximum number of mothers recorded

  ClassDef(PndMCTrackInfoTask, 8);
};

