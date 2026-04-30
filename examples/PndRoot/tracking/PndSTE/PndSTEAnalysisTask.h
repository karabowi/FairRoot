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

/** PndSTEAnalysisTask
 *@author Jenny Regina <jenny.regina@physics.uu.se>
 *@updated 09.02.2021
 *@version 1.0
 **
 ** PANDA class for evaluating the efficiency of the track finding
 **
 ** The BToF hit finding efficiency can be evaluated here since the
 ** standard tracking quality assurance algorithm, PndTrackingQA.cxx
 ** evaluates the hit finding efficiency in tracking detectors but
 ** not in the BToF detector
 **
 ** The task contain a user set value for a "Hit cut", fCut. This value
 ** gives a number of hits in a track where the user wants to distinguish
 ** the bahavior of the track and look at the behavior of tracks with a
 ** larger number of hits and a smaller.
 **
 ** Task Level RECO
 **/

#ifndef PndSTEAnalysisTask_H_
#define PndSTEAnalysisTask_H_

#include "FairTask.h"
#include "PndTrackCand.h"
#include "PndTrack.h"
#include "PndMCTrack.h"
#include "TClonesArray.h"
#include "PndSciTHit.h"
#include "FairLink.h"

#include "TH2.h"

#include <vector>
#include <map>

class TClonesArray;

class PndSTEAnalysisTask : public FairTask {
 public:
  /** Default Constructor **/
  PndSTEAnalysisTask() : FairTask("Pnd Stt Track Extrapolator Analysis Task"){};

  /** Default Destructor **/
  virtual ~PndSTEAnalysisTask(){};

  /** @brief Initializes and loads the data for the task */
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  virtual void FinishEvent();

  virtual void Finish();

  /** @brief Function to set the input track branch name */
  void SetInputTrackBranchName(TString val) { fTrackBranchName = val; };

  /** @brief Function to set the input hit branch name */
  void SetInputHitBranchName(TString val) { fHitBranchName = val; };

  /** @brief Function for analyzing the efficiency of the BToF hit finding */
  void SetAnalyzeBtofHits(bool val) { fAnalyzeBtofHits = val; };

  /** @brief Function to set the cutoff value for the number of hits*/
  void SetHitCut(int val) { fCut = val; };

 private:
  /** @brief Initialize hit branch with branch name */
  void InitHitArray(TString);

  /** @brief Initialize the track array from the track branch name */
  void InitTrackArray(TString);

  /** @brief Initialize the histograms */
  void InitHistograms();

  /** @brief Function that perform the MC truth matching between the reconstructed track and the MC track */
  void AssociateTrackWithMCTrack(PndTrack *);

  /** @brief Function that perform the MC truth matching between the BToF hit and the MC track */
  void AssociateBToFHitWithMCTrack(PndSciTHit *);

  /** @brief Function that analyzes the hit finding efficiency of the BToF hits*/
  void AnalyzeBToFHits(PndTrack *);

  bool fAnalyzeBtofHits = false;

  TString fHitBranchName = "SciTHit"; // Default hit branch name for BTOF hits
  TString fTrackBranchName = "Track"; // Default track branch name

  int eventCount; // Counter for the number of events

  int fCorrectlyAssignedHits = 0; // Number of correctly assigned BTOF hits
  int fWronglyAssignedHits = 0;   // Number of wrongly assigned BTOF hits
  int fNotAssignedHits = 0;       // Number of BTOF hits that are not assigned

  int fCut;    // Minimum number of hits in a track so that it is well reconstructed (~15-18 for SttCellTrackFinder)
  int numHits; // Number of hits in a track, used to fill histograms

  TClonesArray *trackArray;
  TClonesArray *hitArray;

  std::map<PndTrack *, std::vector<int> > fMapRecoTrackIndices; // Maps the PndTrack to a vector with its entry number and index
  std::map<double, std::vector<int> > fMapBToFHitIndices;       // Maps the time of a BTOF hit to a vector with its entry number and index

  /** @brief Below are the declarations of all histograms filled in the task */
  TH1 *hMomRes;
  TH1 *hRelMomRes;
  TH1 *hMomResWithCut;
  TH1 *hMomRelResWithCut;
  TH1 *hMomResCutAwayPart;
  TH1 *hMomRelResCutAwayPart;

  TH1 *hMomRealisticExpanded;
  TH1 *hRelMomRealisticExpanded;

  TH1 *hMomResWithCutPTCut;
  TH1 *hMomRelResWithCutPTCut;
  TH1 *hMomResCutAwayPartPTCut;
  TH1 *hMomRelResCutAwayPartPTCut;

  TH2 *hMomRealistic;
  TH2 *hRelMomRealistic;
  TH2 *hMomAngleRealistic;
  TH2 *hRelMomAngleRealistic;
  TH2 *hMomNumHitsRealistic;
  TH2 *hRelMomNumHitsRealistic;

  TH2 *hPositiveRes_NumHits;
  TH2 *hNegativeRes_NumHits;
  TH2 *hGoodRes_NumHits;

  TH2 *hPositiveRes_RecoMom;
  TH2 *hNegativeRes_RecoMom;
  TH2 *hGoodRes_RecoMom;
  TH2 *hPositiveRes_MCMom;
  TH2 *hNegativeRes_MCMom;
  TH2 *hGoodRes_MCMom;
  TH2 *hPositiveRes_MCTheta;
  TH2 *hNegativeRes_MCTheta;
  TH2 *hGoodRes_MCTheta;
  TH2 *hPositiveRes_NumMCTracks;
  TH2 *hNegativeRes_NumMCTracks;
  TH2 *hGoodRes_NumMCTracks;

  TH2 *hPositiveRelRes_NumHits;
  TH2 *hNegativeRelRes_NumHits;
  TH2 *hGoodRelRes_NumHits;
  TH2 *hPositiveRelRes_RecoMom;
  TH2 *hNegativeRelRes_RecoMom;
  TH2 *hGoodRelRes_RecoMom;
  TH2 *hPositiveRelRes_MCMom;
  TH2 *hNegativeRelRes_MCMom;
  TH2 *hGoodRelRes_MCMom;
  TH2 *hPositiveRelRes_MCTheta;
  TH2 *hNegativeRelRes_MCTheta;
  TH2 *hGoodRelRes_MCTheta;
  TH2 *hPositiveRelRes_NumMCTracks;
  TH2 *hNegativeRelRes_NumMCTracks;
  TH2 *hGoodRelRes_NumMCTracks;
  TH2 *hAllRelRes_NumMCTracks;
  TH2 *hAllRes_NumMCTracks;

  TH2 *hTotMom;
  TH2 *hTotMomRel;

  TH2 *hTotMomFiltered;
  TH2 *hTotMomRelFiltered;

  TH2 *hNumMCTracksAbsolute;
  TH2 *hNumMCTracksRelative;

  TH2 *hMomFiltered;
  TH2 *hMomRelFiltered;

  TH1 *hMomResFiltered;
  TH1 *hMomRelResFiltered;

  TH1 *hNumMcTracksPerIdealTrack;

  ClassDef(PndSTEAnalysisTask, 1);
};

#endif /* PndSTEAnalysisTask_H_ */
