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
 * @class PndIdealTrackFinder
 *
 * @brief Ideal track finder for all types of tracking detectors
 * @detailed The PndIdealTrackFinder combines all hits in given branches into PndTrackCand and calculates the PndTrack based on MC information
 * @author Tobias Stockmanns <t.stockmanns@fz-juelich.de>
 * @date Apr 12, 2010
 *
 */

#ifndef PndIdealTrackFinder_H_
#define PndIdealTrackFinder_H_

// framework includes
#include "PndPersistencyTask.h"
#include "FairMCPoint.h"
#include "PndTrackCand.h"
#include "PndTrackFunctor.h"
#include "FairTSBufferFunctional.h"

#include "TClonesArray.h"
#include "TDatabasePDG.h"
#include <math.h>

class PndIdealTrackFinder : public PndPersistencyTask {
 public:
  PndIdealTrackFinder(TString name = "PndIdealTrackFinder");
  virtual ~PndIdealTrackFinder();

  virtual InitStatus Init();

  /**
   * Add a branch name of detector hits which should be added to the `PndTrack`.
   * If no branch name is given per default all hits of tracking detectors are taken.
   * @param name name of branch in input root file
   */
  virtual void AddBranchName(TString name)
  {
    fBranchNames.push_back(name);
  }

  /**
   * Add a branch name of detector hits which should be added to the `PndTrack`.
   * If no branch name is given per default all hits of tracking detectors are taken.
   * @param name name of branch in input root file
   * @param mcName name of the branch which contains the MC points connected to the hit branch. If no mcName is given a default map is taken.
   */
  virtual void AddBranchName(TString name, TString mcName)
  {
    fBranchNames.push_back(name);
    fPointBranchMap[name] = mcName;
  }

  /**
   * Sets the output branch name of the generated `PndTrack`s.
   * If this method is not called "IdealTrack" is taken.
   * @param name name of branch in output root file
   */
  virtual void SetOutputBranchName(TString name) { fOutBranchName = name; };

  /**
   * Main method called for each event
   */
  virtual void Exec(Option_t *opt);

  //  virtual void Finish();

  // taken from sttmvdtracking/PndSttMvdGemTrackingIdeal.h

  /**
   * Sets how much the mc truth momentum values are smeared by absolute values
   * @param sigmax absolute smearing in px in [GeV/c]
   * @param sigmay absolute smearing in py in [GeV/c]
   * @param sigmaz absoulte smearing in pz in [GeV/c]
   */
  void SetMomentumSmearing(Double_t sigmax, Double_t sigmay, Double_t sigmaz)
  {
    fMomSigma.SetXYZ(fabs(sigmax), fabs(sigmay), fabs(sigmaz));
    fRelative = kFALSE;
  };

  /**
   * Sets how much the mc truth momentum values are smeared by a relative value. The relative smearing is applied to pt and pz individually.
   * => The error applied to px and py is 1/Sqrt(2) * dpop while for pz it is dpop
   * @param dpop relative momentum smearing
   */
  void SetRelativeMomentumSmearing(Double_t dpop)
  {
    fDPoP = fabs(dpop);
    fRelative = kTRUE;
  };

  /**
   * (De-)Activates time based reconstruction
   */
  void SetRunTimeBased(bool valRunTimeBased = true) { fRunTimeBased = valRunTimeBased; };

  /**
   * Sets how much the mc truth vertex position is smeared by absolute values
   * @param sigmax absolute smearing in x in [cm]
   * @param sigmay absolute smearing in y in [cm]
   * @param sigmaz absoulte smearing in z in [cm]
   */
  void SetVertexSmearing(Double_t sigmax = -1., Double_t sigmay = -1., Double_t sigmaz = -1.) { fVtxSigma.SetXYZ(fabs(sigmax), fabs(sigmay), fabs(sigmaz)); }; // in cm

  /**
   * Sets an artificial efficiency of the ideal track finder
   * @param eff Fraction of tracks which are generated. A value of 1 means all ideal tracks, 0 no tracks are generated.
   */
  void SetTrackingEfficiency(Double_t eff = 1.) { fEfficiency = eff; };

  /**
   * The track selector decides which kind of ideal tracks are generated from the features of MC tracks.
   * E.g. the "StandardTrackFunctor" creates a track if more than 3 MVD hits or more than 5 (MVD+STT+GEM) hits are from the same track.
   * @param selector string which defines a track selector. Valid strings can be found in PndTrackFunctor
   */
  void SetTrackSelector(TString selector)
  {
	fTrackSelector = PndTrackFunctor::make_PndTrackFunctor(selector.Data());
  }

  /**
   * Defines if the output track candidates are stored to a file or are just transient.
   */
  void SetPersistence(Bool_t persistence) { SetPersistency(persistence); }

 protected:
  virtual void CreateTrackCands();
  virtual void CreateTracks();
  virtual void FilterTrackCands();
  virtual FairMCPoint *GetFairMCPoint(TString hitBranch, FairMultiLinkedData_Interface *links, FairMultiLinkedData &array);
  // taken from sttmvdtracking/PndSttMvdGemTrackingIdeal.h
  virtual void SmearVector(TVector3 &vec, const TVector3 &sigma);

 protected:
  // For time-based data
  // Functor for start time

  Bool_t fRunTimeBased;

  BinaryFunctor *fFunctor;
  double fStopTimeValue;

  TString fOutBranchName;
  TClonesArray *fTrackCand;
  TClonesArray *fTrack;
  TClonesArray *fMCTrack;
  PndTrackFunctor *fTrackSelector;
  std::map<TString, TClonesArray *> fBranchMap;
  std::vector<TString> fBranchNames;
  std::map<FairLink, PndTrackCand> fTrackCandMap;
  std::map<FairLink, FairMCPoint> fFirstPointMap;
  std::map<FairLink, FairMCPoint> fLastPointMap;
  std::map<TString, TString> fPointBranchMap;

  TDatabasePDG *fPdg; //!<! Particle DB

  Int_t fHitCount;

  // Parameters for fake tracking taken from sttmvdtracking/PndSttMvdGemTrackingIdeal.h
  TVector3 fMomSigma;   ///< Momentum smearing sigma [GeV]
  Double_t fDPoP;       ///< Relative momentum Smearing
  Bool_t fRelative;     ///< flag
  TVector3 fVtxSigma;   ///< Vertex smearing sigma [cm]
  Double_t fEfficiency; ///< Tracking efficiency - if (0 <= e < 1), some tracks will be discarded

  ClassDef(PndIdealTrackFinder, 2);
};

#endif /* PndIdealTrackFinder_H_ */
