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

// -------------------------------------------------------------------------
// -----                PndParticleRequirements header file            -----
// -----                  Created 16/09/19  by T.Stockmann             -----
// -------------------------------------------------------------------------

/** @class PndParticleRequirements
 ** @author T.Stockmanns <t.stockmanns@fz-juelich.de>
 ** Data class
 **
 **/

#pragma once

#include "TVector3.h"
#include "TVector2.h"
#include "TList.h"

#include "PndMCTrackInfo.h"
#include <iostream>
#include <string>

class PndParticleRequirements {

 public:
  /**  Default constructor  **/
  PndParticleRequirements();

  /**  Destructor  **/
  virtual ~PndParticleRequirements();

  /**  Output to screen  **/
  void Print(std::ostream &out = std::cout);
  std::string ShortRequirementsAsString();

  void Init();

  friend std::ostream &operator<<(std::ostream &out, PndParticleRequirements &track)
  {
    track.Print(out);
    return out;
  }

  /**  Setters  **/

  void SetMinDecayTime(Double_t val) { fMinDecayTime = val; }
  void SetMaxDecayTime(Double_t val) { fMaxDecayTime = val; }

  void SetPIDMothers(std::vector<int> val) { fPIDMothers = val; }
  void SetPIDDaughters(std::vector<int> val) { fPIDDaughters = val; }
  void SetPID(int val) { fPID = val; }
  void SetStage(int val) { fStage = val; }
  void SetMinTrackingPoints(int val) { fMinTrackingPoints = val; }
  void AddMaxMomentumDifference(int branchType, TVector2 val) { fMomentumDifference[branchType] = val; }
  void AddTypeRequirements(int branchType, int howOftenSeen) { fTypeCount[branchType] = howOftenSeen; }
  void SetCheckTracking(TString barrelPrefix, TString forwardPrefix, TString particlePropagatorName);
  void SetCheckPid(TString algorithms, double minProb)
  {
    fPidAlgorithms = algorithms;
    fMinPidProb = minProb;
    fCheckPid = true;
  }

  void SetCheckNeutral();
  int GetNRequirements();

  int GetPdgCode() const { return fPID; };
  std::vector<int> GetMothers() const { return fPIDMothers; }
  std::vector<int> GetDaughters() const { return fPIDDaughters; }
  int GetStage() const { return fStage; }
  std::map<int, TVector2> GetMomentumDifference() const { return fMomentumDifference; }
  std::map<int, int> GetTypeCount() const { return fTypeCount; }
  Double_t GetMinDecayTime() const { return fMinDecayTime; }
  Double_t GetMaxDecayTime() const { return fMaxDecayTime; }
  int GetMinTrackingPoints() const { return fMinTrackingPoints; }
  bool GetCheckTracking() const { return fCheckTracking; }
  bool GetCheckNeutral() const { return fCheckNeutral; }
  bool GetCheckPid() const { return fCheckPid; }
  PndPidProbability GetPidProbability(PndMCTrackInfo &info);

  std::vector<int> CheckTrack(PndMCTrackInfo &info);
  void Found() { fFound = true; }
  void ClearFound() { fFound = false; }
  bool GetFound() const { return fFound; }

 protected:
  int CheckMothers(std::vector<int> mothers);
  int CheckDaughters(std::vector<int> daughters);
  int CheckStage(int stage);
  std::vector<int> CheckTracking(PndMCTrackInfo &info);
  std::vector<int> CheckTypeCount(PndMCTrackInfo &info);
  std::vector<int> CheckMomentum(PndMCTrackInfo &info);
  int CheckPid(PndMCTrackInfo &info);
  int CheckOverall(std::vector<int> results);
  void InitTracking();
  void InitPid();
  void InitNeutral();

 private:
  Double_t fMinDecayTime;
  Double_t fMaxDecayTime;
  std::vector<int> fPIDMothers;
  std::vector<int> fPIDDaughters;
  int fPID;
  Int_t fStage;
  std::map<int, TVector2> fMomentumDifference; //< difference between p transversal and p longitudinal between reco track and MC track
  std::map<int, int> fTypeCount;
  TString fBarrelTrackerPrefix;
  TString fForwardTrackerPrefix;
  TString fPropagatorParticle;
  TString fPidAlgorithms;
  std::vector<int> fCheckBranchesBarrel;
  std::vector<int> fCheckBranchesForward;
  int fCheckBranchChargedCand; // branchID of ChargedCand
  int fMinTrackingPoints;
  std::vector<TVector2> fMomentumResolution;
  std::vector<int> fPidBranches; //< list of branches taking into account for PID determination
  double fMinPidProb;            //< required minimum pid probability

  bool fFound;
  bool fCheckTracking;
  bool fCheckNeutral;
  bool fCheckPoints;
  bool fCheckPid;

  ClassDef(PndParticleRequirements, 1);
};

