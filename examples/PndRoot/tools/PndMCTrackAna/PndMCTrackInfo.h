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
// -----                      PndMCTrackInfo header file                   -----
// -----                  Created 16/09/19  by T.Stockmann             -----
// -------------------------------------------------------------------------

/**
 * @class PndMCTrackInfo.h
 * @author T.Stockmanns <t.stockmanns@fz-juelich.de>
 * @briefe Data class to contain additional information for an MCTrack
 **
 **/

#ifndef PNDMCTRACKINFO_H
#define PNDMCTRACKINFO_H 1

#include "FairMultiLinkedData_Interface.h"
#include "PndPidProbability.h"

#include "TVector3.h"
#include <iostream>
#include <map>

class PndMCTrackInfo : public FairMultiLinkedData_Interface {

 public:
  /**  Default constructor  **/
  PndMCTrackInfo();

  /**  Destructor  **/
  virtual ~PndMCTrackInfo();

  /**  Output to screen  **/
  void Print(std::ostream &out = std::cout);

  friend std::ostream &operator<<(std::ostream &out, PndMCTrackInfo &track)
  {
    track.Print(out);
    return out;
  }

  /**  Accessors  **/

  /**
   * Returns which daughters belong to the MC track (as indices to the MCTrack TClonesArray)
   */
  std::vector<int> GetDaughters();
  TVector3 GetDecayVertex() const { return fDecayVertex; }
  Double_t GetDecayTime() const { return fDecayTime; }

  /**
   * Returns which generation of daughter particles this particle is compared to its primary particle
   * (final particle of event generator)
   */
  Int_t GetStage() const { return fStage; }

  /**
   * Returns the PID of the mother particles. The first entry in the vector is the primary particle,
   * the second the first daughter and so on
   */
  std::vector<int> GetPIDMothers() const { return fPIDMothers; }

  /**
   * Returns the PID of all direct daughter particles
   */
  std::vector<int> GetPIDDaughters() const { return fPIDDaughters; }

  /**
   * Returns the PID of the particle
   */
  int GetPID() const { return fPID; }

  /**
   * Returns the number of hits for this MC track for a given branchName
   * @param branch name
   */
  Int_t GetNHits(TString branchName);

  int GetMVDPoints() const { return fMVDPoints; }
  int GetSTTPoints() const { return fSTTPoints; }
  int GetGEMPoints() const { return fGEMPoints; }
  int GetFTSPoints() const { return fFTSPoints; }
  int GetTotalPoints() const { return fTotalPoints; }

  /**
   * Returns how often a link to a type (branch) is in the list of FairLinks
   */
  int GetTypeCount(int type)
  {
    if (fTypeCount.count(type) > 0) {
      return fTypeCount[type];
    } else {
      return -1;
    }
  };

  /**
   * Returns the momentum difference for pt, pz for a reconstruction type (branch)
   */
  TVector2 GetDeltaMomentum(int type)
  {
    TVector2 result;
    if (fMomentumDifference.count(type) > 0) {
      return fMomentumDifference[type];
    }
    return result;
  }

  /**
   * Returns the PID probabilites  for a pid type (branch)
   * @param type branch index
   */
  PndPidProbability GetPidProbability(int type)
  {
    PndPidProbability result;
    if (fPidProb.count(type) > 0) {
      return fPidProb[type];
    }
    return result;
  }

  // Setters
  void SetPID(int pid) { fPID = pid; }
  void AddDaughter(FairLink doughter);
  void AddPIDDaughter(int pid) { fPIDDaughters.push_back(pid); }
  void SetPIDMothers(std::vector<int> pid)
  {
    fPIDMothers = pid;
    fStage = fPIDMothers.size();
  }
  void SetDecayVertex(TVector3 vertex) { fDecayVertex = vertex; }
  void SetDecayTime(Double_t time) { fDecayTime = time; }
  void SetRecoMomentum(int branchType, TVector3 value) { fMomentumReco[branchType] = value; } // branchType typeID of branch containing the reconstructed track
  void SetDeltaMomentum(int branchType, double pt, double pl) { fMomentumDifference[branchType] = TVector2(pt, pl); }; // mc momentum - reco momentum
  void SetPidProbability(int branchType, PndPidProbability &value) { fPidProb[branchType] = value; }

  void FillPoints();
  void FillTypeCounts();

 protected:
 private:
  TVector3 fDecayVertex;
  Double_t fDecayTime;
  int fPID;
  std::vector<int> fPIDMothers;
  std::vector<int> fPIDDaughters;
  std::vector<int> fDaughters;
  Int_t fStage;
  Int_t fMVDPoints;
  Int_t fSTTPoints;
  Int_t fGEMPoints;
  Int_t fFTSPoints;
  Int_t fTotalPoints;
  std::map<int, TVector3> fMomentumReco;
  std::map<int, TVector2> fMomentumDifference; //< difference between p transversal and p longitudinal between reco track and MC track
  std::map<int, PndPidProbability> fPidProb;   //< pid probability from different algorithms

  std::map<int, int> fTypeCount; //! BranchType, how often exists a link to type

  ClassDef(PndMCTrackInfo, 5);
};

#endif
