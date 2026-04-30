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

/** PndSTETask
 *@author Jenny Regina <jenny.regina@physics.uu.se>
 *@updated 09.02.2021
 *@version 1.0
 **
 ** PANDA class for finding hits in the MVD, GEM or Barrel ToF
 ** that are compatible with a track reconstructed e.g. in the STT.
 ** Hits in the MVD and GEM can be used in refits of the track.
 **
 ** Task Level RECO
 **/

#ifndef PndSTETask_H_
#define PndSTETask_H_

#include "FairTask.h"
#include "FairMultiLinkedData.h"
#include "PndRiemannTrack.h"
#include "PndSTESettings.h"

class TClonesArray;
class FairMultiLinkedData;

class PndSTETask : public FairTask, public PndSTESettings {
 public:
  /** Constructor **/
  PndSTETask() : FairTask("Mvd Tracker Task"){};

  /** Default Destructor **/
  virtual ~PndSTETask(){};

  virtual InitStatus Init();

  virtual void Exec(Option_t *opt);

  virtual void FinishEvent();

  /** @brief Function to set the persistency, true as default */
  void SetPersistence(bool val) { fPersistence = val; };

  /** @brief Function to set the input track branch name */
  void SetInputTrackBranchName(TString trackName) { fInputTrackBranchName = trackName; };

  /** @brief Function to set the input track cand branch name */
  void SetInputTrackCandBranchName(TString trackName) { fInputTrackCandBranchName = trackName; };

  /** @brief Function to set the input Riemann branch name */
  void SetInputRiemannTrackBranchName(TString trackName) { fInputRiemannTrackBranchName = trackName; };

  /** @brief Function to set the prefix to the output branch name
   * Default output branch name is "Track" that is used if no prefix is given
   */
  void SetOutputBranchNamePrefix(TString trackName) { fOutBranchNamePrefix = trackName; };

  /** @brief Function to choose if the hit finding should be done on time based data */
  // void SetRunTimeBased(bool val) { fRunTimeBased = val; };

  /** @brief User option for removing short tracks. It has been found that with e.g. the SttCellTrackFinder
   * short tracks with roughly less that 15 hits tend to be poorly fitted with poor momentum resolution.
   * Default is true
   */
  // void RemoveShortTracks(bool val) { fRemoveShortTracks = val; };

  /** @brief Function for filtering out the short tracks
   * Right now the output from this function is not used but the tracks are manually
   * filtered in the PndSTEMvd.cxx if needed
   */
  // void FilterTracks(std::vector<TClonesArray *>, std::vector<TClonesArray *>, std::vector<TClonesArray *>);

  /** @brief Function for initiating the chosen hit type */
  void InitHitArray(TString branchName);

  /** @brief Function for initiating the chosen track type */
  void InitTrackArray(TString branchName);

  /** @brief Function to initiate the chosen type of Riemann tracks */
  void InitRiemannTrackArray(TString branchName);

  /** @brief Function to initiate the chosen type of track cands */
  void InitTrackCandArray(TString branchName);

  /** @brief Function to add chosen hit branches to the algorithm */
  void AddHitBranch(TString branchName) { fHitBranchName.push_back(branchName); };

 private:
  bool fPersistence = true;

  /** @brief If time-stamps are utilized, In nano-seconds */
  // double fClusterTime = 250.0;

  /** @brief Set to "true" if timing information between track and hit should be taken into account for the matching */
  // bool fRunTimeBased = false;

  // bool fRemoveShortTracks = true;

  TString fOutBranchNamePrefix = "SttExtrapolated"; // Default output track branchname prefix
  TString fInBranchNamePrefix = "NoNameGiven";      // Default input track branch name prefix

  std::vector<TString> fHitBranchName; // Vector of input hit branch names

  std::vector<TClonesArray *> fMvdHitsPixelArray; // Input Mvd Hit Pixel Array
  std::vector<TClonesArray *> fMvdHitsStripArray; // Input Mvd Hit Strip Array
  std::vector<TClonesArray *> fGemHitsArray;      // Input Gem hit array
  std::vector<TClonesArray *> fBtofHitsArray;     // Input Btof array

  std::vector<TClonesArray *> fPndTrackCandArray;    // Input track cand branch from previous tracking
  std::vector<TClonesArray *> fPndTrackArray;        // Input track branch from previous tracking
  std::vector<TClonesArray *> fPndRiemannTrackArray; // Input RiemannTrack branch from previous tracking

  // Output branches
  TClonesArray *fOutTrackArray;        // Output track array
  TClonesArray *fOutTrackCandArray;    // Output track cand array
  TClonesArray *fOutRiemannTrackArray; // Output Riemann track array

  // Input branches
  TString fInputTrackBranchName;        // Input track branch name
  TString fInputTrackCandBranchName;    // Input track cand branch name
  TString fInputRiemannTrackBranchName; // Input Riemann track branch name

  TClonesArray *fEventHeader;

  double fSumTime; // For timing information

  // The three following vectors of TClones arrays are used if the tracks are filtered
  // TClonesArray *fPndTrackCandFileteredArray;   // Input track cand branch from previous tracking
  // TClonesArray *fPndRiemannTrackFilteredArray; // Input RiemannTrack branch from previous tracking
  // TClonesArray *fPndTrackFilteredArray;        // Input track branch from previous tracking
  // std::vector<TClonesArray *> fFilteredArrays; // Vector to put the previous three arrays in

  ClassDef(PndSTETask, 1);
};

#endif /*PndSTETask_H_*/
