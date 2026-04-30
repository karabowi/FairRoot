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

// *************************************************************************
// Original author: Ralf Kliemt ralf.kliemt(at)hiskp(dot)uni-bonn(dot)de
// Maintainer: martin (dot) j [dot] galuska <at> physik {dot} uni (minus) giessen <dot> de
//
// ideal tracker using MC id & track info
// Gaussian momentum smearing & fake efficiency included
//
// removed tracks that turn around in the dipole field (because of fitter problems)
// removed tracks that have less than 5 hits in the FTS (TODO a more realistic criterion is needed)
// user can specify that number via SetMinFtsHitsPerTrack()
// for FTS studies SetMinFtsHitsPerTrack(5) should be good,
// for studies of overall detector performance SetMinFtsHitsPerTrack(1) gets rid off an efficiency drop 5⁰ < theta < 9⁰ that is due to the current lack of tracking starting from
// GEM hits
//
// Created: 28.01.2011
// Last modification: 24.02.2014
//
// *************************************************************************

#ifndef FTSTRACKERIDEALH
#define FTSTRACKERIDEALH

//#include "TObject.h"
//#include "TObjArray.h"
//#include "TArrayI.h"
//#include "TBenchmark.h"
#include "TVector3.h"
#include "TDatabasePDG.h"
//#include "Riostream.h"
#include "FairTask.h"
#include "PndGeoFtsPar.h"
#include <cmath>
#include "Rtypes.h" // for Double_t, Int_t, etc

class PndFtsTrackerIdeal : public FairTask {

 public:
  PndFtsTrackerIdeal();
  virtual ~PndFtsTrackerIdeal();

  virtual void Exec(Option_t *option);
  virtual InitStatus Init();
  virtual void Finish();

  void Reset();
  void Register();

  void SetParContainers();

  void SetTrackOutput(TString name = "FTSTrkIdeal") { fTracksArrayName = name; };

  void SetMomentumSmearing(Double_t sigmax = -1., Double_t sigmay = -1., Double_t sigmaz = -1.)
  {
    fMomSigma.SetXYZ(fabs(sigmax), fabs(sigmay), fabs(sigmaz));
    fRelative = kFALSE;
  }; // in GeV
  void SetRelativeMomentumSmearing(Double_t dpop = -1.)
  {
    fDPoP = fabs(dpop);
    fRelative = kTRUE;
  };                                                                                                                                                           // in GeV
  void SetVertexSmearing(Double_t sigmax = -1., Double_t sigmay = -1., Double_t sigmaz = -1.) { fVtxSigma.SetXYZ(fabs(sigmax), fabs(sigmay), fabs(sigmaz)); }; // in cm
  void SetTrackingEfficiency(Double_t eff = 1.) { fEfficiency = eff; };
  Int_t SetMinFtsHitsPerTrack(Int_t minFtsHitsPerTrack = 5); // checks argument, sets fMinFtsHitsPerTrack (in any case) and gives new value back

  void DeactivateFtsStation(Int_t i) { fStationsDisabled[i] = true; };

  void SetFtsActivity(Bool_t act = kTRUE) { fBranchActive[0] = act; }
  void SetGemActivity(Bool_t act = kTRUE) { fBranchActive[1] = act; }
  void SetMvdActivity(Bool_t act = kTRUE)
  {
    fBranchActive[2] = act;
    fBranchActive[3] = act;
  }

  void SetPersistence(Bool_t persistence) { fPersistence = persistence; }

 protected:
  void SmearFWD(TVector3 &vec, const TVector3 &sigma); // smearing with doubled sigma in z direction

  TClonesArray *fMCTracks;               //! Array of PndMCTrack
  TClonesArray *fMCPoints[4];            //! Array of event's points
  TClonesArray *fHits[4];                //! Array of event's hits
  Int_t fBranchIDs[4];                   //! Array of Branch IDs
  Bool_t fBranchActive[4];               //! Array of Branch Activeness
  std::map<int, bool> fStationsDisabled; //! Array of disabled stations

  TClonesArray *fTrackCands; //! Array of found track candidates
  TClonesArray *fTracks;     //! Array of found tracks
  TClonesArray *fTrackIds;   //! Array of track IDs (Links)

  // Parameters for fake tracking
  Int_t fMinFtsHitsPerTrack; // Only tracks that have at least this number of hits in the FTS can be found by the ideal tracker
  // default is 1 for overall detector performance studies (see explanation in forum)
  // 5 should be used for FTS standalone performance studies
  TVector3 fMomSigma;   // Momentum smearing sigma [GeV]
  Double_t fDPoP;       // Relative momentum Smearing
  Bool_t fRelative;     // falg
  TVector3 fVtxSigma;   // Vertex smearing sigma [cm]
  Double_t fEfficiency; // Tracking efficiency - if (0 <= e < 1), some tracks will be discarded
  Bool_t fPersistence;  // Persistence of tracking TCAs

  TString fTracksArrayName; // Branch name where to store the Track candidates
  TDatabasePDG *pdg;        //! Particle DB

  PndGeoFtsPar *fFtsParameters;
  TClonesArray *fTubeArrayFts;

  PndFtsTrackerIdeal(const PndFtsTrackerIdeal &L);
  PndFtsTrackerIdeal &operator=(const PndFtsTrackerIdeal &) { return *this; }

  ClassDef(PndFtsTrackerIdeal, 1);
};
#endif
