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
// -----                 PndSttTrackFinderIdeal header file            -----
// -----                  Created 28/03/06  by R. Castelijns           -----
// -------------------------------------------------------------------------

/** PndSttTrackFinderIdeal
 *@author R.Castelijns <r.castelijns@fz-juelich.de>
 **
 ** Ideal track finder in the STT for simulated data.
 ** For each MCTrack having at least 3 SttPoints, a SttTrack is created
 ** and the corresponding SttHits are attached using the correspondence
 ** between SttHit and SttPoint.
 **/

#ifndef PNDSTTTRACKFINDERIDEAL
#define PNDSTTTRACKFINDERIDEAL 1

#include "PndSttTrackFinder.h"
#include "PndMCTrack.h"

#include "TList.h"
#include "TClonesArray.h"

class PndTrackCand;
class PndSttHit;
class FairMCPoint;
// class TClonesArray;

class PndSttTrackFinderIdeal : public PndSttTrackFinder {

 public:
  void GetTrackletCircular(Double_t firstX, Double_t firstY, Double_t firstR, Double_t secondX, Double_t secondY, Double_t secondR, Double_t thirdX, Double_t thirdY,
                           Double_t thirdR, Double_t *circleRadii, Double_t *circleCentersX, Double_t *circleCentersY) const;

  // void ZoomTrack(Double_t &dSeed, Double_t &phiSeed, Double_t &rSeed, PndSttTrack *track);  // not implemented
  void GetTrack(Double_t &dSeed, Double_t &phiSeed, Double_t &rSeed, Double_t &zSeed, Double_t &tanLamSeed, Int_t mcTrackNo);

  /** Default constructor **/
  PndSttTrackFinderIdeal();

  /** Standard constructor **/
  PndSttTrackFinderIdeal(Int_t verbose);

  /** Destructor **/
  virtual ~PndSttTrackFinderIdeal();

  /** Initialisation **/
  virtual void Init();

  /** Track finding algorithm
   ** This just reads MC truth (MCTracks and MCPoints), creates
   ** one StsTrack for each MCTrack and attaches the hits according
   ** to the MCTrack of the corresponding MCPoint
   **
   *@param mHitArray   Array of MAPS hits
   *@param trackArray  Array of CbmStsTrack
   **
   *@value Number of tracks created
   **/
  virtual Int_t DoFind(TClonesArray *trackCandArray, TClonesArray *trackArray, TClonesArray *helixHitArray);

  virtual void AddHitCollection(TClonesArray *mHitArray, TClonesArray *mPointArray)
  {
    fHitCollectionList.Add(mHitArray);
    fPointCollectionList.Add(mPointArray);
  }
  void plotAllStraws();
  Bool_t putStraw(Double_t xpos, Double_t ypos, Double_t radius);

  TClonesArray *fTubeArray;
  void SetTubeArray(TClonesArray *tubeArray) { fTubeArray = tubeArray; };

 private:
  /** Arrays of MC information **/
  TClonesArray *fMCTrackArray;

  Bool_t rootoutput;

  TList fHitCollectionList;
  TList fPointCollectionList;
  PndSttHit *GetHitFromCollections(Int_t hitCounter);
  FairMCPoint *GetPointFromCollections(Int_t hitCounter);

  ClassDef(PndSttTrackFinderIdeal, 1);
};

#endif
