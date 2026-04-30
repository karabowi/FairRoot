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
 * @class PndTrackingQA
 * @brief Performs quality assurance tests on PndTrack and PndTrackCandidates
 * @details Analyses if hits in found PndTrack(Cands) are comming from the same MC track,
 * checks how many MC tracks have been found, determines efficiency and purity information
 * and assigns a quality label to each found track. The result is stored in two TClonesArrays,
 * one for the information per reconstructed track one for each MC track.
 * Needs as an input the reconstructed track data and, as a basis, the output of an ideal tracking
 * algorithm from the same data.
 *
 * @date 21.06.2023
 * @author Tobias Stockmanns <t.stockmanns@fz-juelich.de>
 */

#pragma once

#include "PndTrackFunctor.h"
#include "PndTrackingQARecoInfo.h"
#include "PndTrackingQAMCInfo.h"
#include "PndTrackingQABranchEnum.h"
#include "PndTrack.h"

#include "TString.h"

class PndTrackingQA {
 public:
  /**
   * @brief constructor
   *
   * @param trackBranchName name of the branch containing the track or trackCand data
   * @param idealTrackName name of the track data from the ideal track finder
   * @param pndTrackNotTrackCand decides if the trackBranch contains PndTracks or PndTrackCands
   */
  PndTrackingQA(TString trackBranchName, TString idealTrackName, Bool_t pndTrackNotTrackCand = kTRUE);

  /**
   * @brief second constructor
   *
   * @param trackBranchName name of the branch containing the track or trackCand data
   * @param idealTrackName name of the track data from the ideal track finder
   * @param posTrack PndTrackFunctor which determines if a track is reconstructible or not
   * @param pndTrackNotTrackCand decides if the trackBranch contains PndTracks or PndTrackCands
   */
  PndTrackingQA(TString trackBranchName, TString idealTrackName, PndTrackFunctor *posTrack, Bool_t pndTrackNotTrackCand = kTRUE);

  virtual ~PndTrackingQA();

  virtual void Init();

  /**
   * @brief Add branch names of hits which should be considere in the tracking QA
   */

  void AddHitsBranchName(TString name) { fBranchNames.push_back(name); }
  void SetHitsBranchNames(std::vector<TString> names) { fBranchNames = names; }

  /**
   * @brief STT info if a hit was in a skewed or parallel tube is needed.
   * Therefore the TClonesArray with the STTHits and the tube array created by PndSttMapCreator has
   * to be set.
   */

  void SetSttInfo(TClonesArray *sttHitArray, TClonesArray *sttTubeArray)
  {
    fSttHitArray = sttHitArray;
    fSttTubeArray = sttTubeArray;
  }

  void SetVerbose(int val) { fVerbose = val; }

  /**
   * @brief Main method of algorithms
   * @param recoTrackInfo empty TClonesArray of type PndTrackingQARecoInfo to store reco info data
   * @param mcTrackInfo empty TClonesArray of type PndTrackingQAMCInfo to store mc info data
   */
  void AnalyseEvent(TClonesArray *recoTrackInfo, TClonesArray *mcTrackInfo);

 protected:
  std::map<FairLink, PndTrackingQAMCInfo *> FillMCTrackInfo(TClonesArray *mcTrackInfo);
  double CalcPCA(PndTrack *track);
  int GetMCInfoQualification(PndTrackingQAMCInfo *mcInfo, PndTrack *idealTrack);
  std::map<TString, FairMultiLinkedData> AnalyseTrackCand(PndTrackCand *trackCand);
  FairMultiLinkedData GetMCInfoForBranch(TString branchName, PndTrackCand *trackCand);
  void PrintTrackInfo(std::map<TString, FairMultiLinkedData> info);
  FairLink GetMostProbableMCTrack(std::map<TString, FairMultiLinkedData> &trackInfo);
  PndTrackingQARecoInfo GetRecoInfoFromRecoTrack(FairLink trackId, PndTrackingQAMCInfo *mcTrackInfo);
  void AssociateRecoTracksToMCTracks(PndTrackingQARecoInfo &recoInfo);

 private:
  std::vector<TString> fBranchNames; //!<! branch names of hits taken into account in the analysis (e.g. MVDHitsPixel, STTHit, ...)
  FairRootManager *ioman = nullptr;
  TString fTrackBranchName;
  TString fIdealTrackName;
  Bool_t fPndTrackNotTrackCand = kTRUE; // kTRUE if track and kFALSE if track cand
  PndTrackFunctor *fPossibleTrack = nullptr;

  std::map<FairLink, PndTrackingQAMCInfo *> fMCInfoMap; //< MCTrackId, Associated MCInfo>

  Int_t fVerbose = 0;

  TClonesArray *fTrack = nullptr;
  TClonesArray *fMCTrack = nullptr;
  TClonesArray *fIdealTrack = nullptr;
  TClonesArray *fTrackCand = nullptr;
  TClonesArray *fIdealTrackCand = nullptr;

  TClonesArray *fSttTubeArray = nullptr;
  TClonesArray *fSttHitArray = nullptr;

  ClassDef(PndTrackingQA, 1);
};
