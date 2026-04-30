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
// -----                   PndGemTrackFinderOnHits header file          -----
// -----                  Created 02.06.2009 by R. Karabowicz          -----
// -------------------------------------------------------------------------

/** \class PndGemTrackFinderOnHits
 *  \author R. Karabowicz <r.karabowicz@gsi.de>
 *  \date 19.03.2009
 *  \brief OnHits track finding algorithm
 *
 *  Track finding procedure operates on reco hits.
 *  Reads GEM hits, creates array of PndGemTracks
 **/

#ifndef PNDGEMTRACKFINDERONHITS_H
#define PNDGEMTRACKFINDERONHITS_H

#include "PndMCTrack.h"

#include "PndGemHit.h"
#include "PndGemTrackFinder.h"
#include "PndGemDigiPar.h"

#include <vector>

struct TrackSegment {
  Int_t stationIndex[2];
  Int_t hitIndex[4];
  Double_t trackMom;
  Double_t trackPhi;
  Double_t trackTheta;
  Int_t recoTrackIndex;
};

class PndGemTrackFinderOnHits : public PndGemTrackFinder {

 public:
  /** Default constructor **/
  PndGemTrackFinderOnHits();

  /** Destructor **/
  virtual ~PndGemTrackFinderOnHits();

  /** DoFind method
   * \param hitArray    Array of Gem hits
   * \param trackArray  Array of PndGemTrack
   * \return  Number of created tracks
   **/
  virtual Int_t DoFind(TClonesArray *hitArray, TClonesArray *trackArray, TClonesArray *trackCandArray);

  /** Public modifiers **/
  void SetVerbose(const Int_t &verbose) { fVerbose = verbose; };
  void SetPrimary(const Int_t &primary) { fPrimary = primary; };
  void SetSigmaMult(Double_t tempd) { fSigmaMult = tempd; };

 private:
  PndGemDigiPar *fDigiPar;

  /** Arrays of MC information **/
  TClonesArray *fMCTrackArray;
  TClonesArray *fMCPointArray;

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

  Double_t fSigmaMult;

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
  Int_t fNofClHits;

  Int_t fNofExpectedTrackSegments;
  Int_t fNofFoundTrackSegments;
  std::vector<TrackSegment> fTrackSegments;
  Int_t FindTrackSegments(TClonesArray *hitArray, Int_t stat1Id, Int_t stat2Id);
  Int_t MatchTrackSegments();
  void RemoveCloneTracks(Int_t nofRecoTracks);
  Int_t CreateTracks(TClonesArray *hitArray, TClonesArray *trackArray, TClonesArray *trackCandArray, Int_t nofRecoTracks);

  void PrintTrackSegments(TClonesArray *hitArray);
  void PrintTracks(TClonesArray *hitArray, Int_t nofRecoTracks);
  void PrintMCTrackSegments(TClonesArray *hitArray);
  void PrintMCTracks(TClonesArray *hitArray, Int_t nofRecoTracks);

  /** Get parameter containers **/
  virtual void SetParContainers();

  /** Initialisation **/
  virtual void Init();

  ClassDef(PndGemTrackFinderOnHits, 1);
};

#endif
