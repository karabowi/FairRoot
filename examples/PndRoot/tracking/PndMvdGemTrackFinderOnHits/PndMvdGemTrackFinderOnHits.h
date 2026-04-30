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
// -----                   PndMvdGemTrackFinderOnHits header file          -----
// -----                  Created 02.06.2009 by R. Karabowicz          -----
// -------------------------------------------------------------------------

/** \class PndMvdGemTrackFinderOnHits
 *  \author R. Karabowicz <r.karabowicz@gsi.de>
 *  \date 19.03.2009
 *  \brief OnHits track finding algorithm
 *
 *  Track finding procedure operates on reco hits.
 *  Reads GEM hits, creates array of PndGemTracks
 **/

#ifndef PNDMVDGEMTRACKFINDERONHITS_H
#define PNDMVDGEMTRACKFINDERONHITS_H

#include "PndPersistencyTask.h"
#include "FairMCPoint.h"
#include "PndMCTrack.h"

#include "PndSdsHit.h"
#include "PndGemHit.h"
#include "PndGemTrackFinder.h"
#include "PndGemDigiPar.h"

#include <vector>

struct TrackSegment {
  Int_t detId[2];
  Int_t hitIndex[2];
  Double_t trackMom;
  Double_t trackPhi;
  Double_t trackTheta;
  Int_t recoTrackIndex;
};

class PndMvdGemTrackFinderOnHits : public PndPersistencyTask {

 public:
  /** Default constructor **/
  PndMvdGemTrackFinderOnHits();

  /** Default constructor **/
  PndMvdGemTrackFinderOnHits(Int_t iVerbose);

  /** Destructor **/
  virtual ~PndMvdGemTrackFinderOnHits();

  /** Execution **/
  virtual void Exec(Option_t *opt);

  /** Public modifiers **/
  void SetVerbose(const Int_t &verbose) { fVerbose = verbose; };

 private:
  PndGemDigiPar *fDigiPar;

  /** Input hit arrays **/
  TClonesArray *fMvdPixelHitArray;
  TClonesArray *fMvdStripHitArray;
  TClonesArray *fGemHitArray;

  /** Output track arrays **/
  TClonesArray *fTrackArray;
  TClonesArray *fTrackCandArray;

  /** Arrays of MC information **/
  TClonesArray *fMCTrackArray;
  TClonesArray *fMCGemPointArray;
  TClonesArray *fMCMvdPointArray;

  /** fGemStructure **/
  //  PndGemStructure *fGemStructure;

  /** Verbosity level.
   ** 0 - quit
   ** 1 - event level
   ** 2 - track level
   ** 3 - debug (maximal output)
   **/
  Int_t fVerbose;

  /** Primaries level
   ** 0 - All MC tracks are considered
   ** 1 - Only primary MC tracks
   **/
  Int_t fPrimary;

  // Parameters, taken from the digiPar file
  Double_t fParThetaA;
  Double_t fParThetaB;

  Double_t fParTheta0;
  Double_t fParTheta1;
  Double_t fParTheta2;
  Double_t fParTheta3;

  Double_t fParRadPhi0;
  Double_t fParRadPhi2;

  Double_t fParMat0[3];
  Double_t fParMat1[3];

  /** Event counter **/
  Int_t fNofEvents; // event counter
  Int_t fMCAvailable;

  Int_t fNofExpectedTrackSegments;
  Int_t fNofFoundTrackSegments;
  std::vector<TrackSegment> fTrackSegments;
  Int_t FindTrackSegments(Int_t stat1Id, Int_t stat2Id);
  Int_t FindTrackSegments(Int_t stat2Id);
  Int_t FindTrackSegments();
  Int_t MatchTrackSegments();
  void RemoveCloneTracks(Int_t nofRecoTracks);
  Int_t CreateTracks(Int_t nofRecoTracks);

  void PrintTrackSegments();
  void PrintTracks(Int_t nofRecoTracks);
  void PrintMCTrackSegments();
  void PrintMCTracks(Int_t nofRecoTracks);

  /** Get parameter containers **/
  virtual void SetParContainers();

  /** Finish **/
  virtual void Finish();

  /** Intialisation **/
  virtual InitStatus Init();

  /** Reinitialisation **/
  virtual InitStatus ReInit();

  ClassDef(PndMvdGemTrackFinderOnHits, 1);
};

#endif
