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
// -----                   PndSttTrackMatch header file                -----
// -----                  Created 28/03/06  by V. Friese               -----
// -------------------------------------------------------------------------

/** PndSttTrackMatch.h
 *@author R.Castelijns <r.castelijns@fz-juelich.de>
 **
 ** Data structure describing the matching of a reconstructed PndSttTrack
 ** with a Monte Carlo PndMCTrack.
 **/

#ifndef PNDSTTTRACKMATCH_H
#define PNDSTTTRACKMATCH_H 1

#include "TObject.h"

class PndSttTrackMatch : public TObject {

 public:
  /** Default constructor **/
  PndSttTrackMatch();

  /** Standard constructor
   *@param mcTrackID   Index of matched MCTrack
   *@param nTrue       Good hits from matched MCTrack
   *@param nWrong      Good Hits from other MCTracks
   *@param nFake       Fake Hits
   *@param nTracks     Number of MCTracks with common points
   **/
  PndSttTrackMatch(Int_t mcTrackID, Int_t nTrue, Int_t nWrong, Int_t nFake, Int_t nTracks);

  /** Destructor **/
  virtual ~PndSttTrackMatch();

  /** Accessors **/
  Int_t GetMCTrackID() { return fMCTrackID; };
  Int_t GetNofTrueHits() { return fNofTrueHits; };
  Int_t GetNofWrongHits() { return fNofWrongHits; };
  Int_t GetNofFakeHits() { return fNofFakeHits; };
  Int_t GetNofMCTracks() { return fNofMCTracks; };

 private:
  /** Best matching PndMCTrack  **/
  Int_t fMCTrackID;

  /** Number of good hits belonging to the matched MCTrack **/
  Int_t fNofTrueHits;

  /** Number of good hits belonging to other MCTracks **/
  Int_t fNofWrongHits;

  /** Number of fake hits **/
  Int_t fNofFakeHits;

  /** Number of MCTracks with common points **/
  Int_t fNofMCTracks;

  ClassDef(PndSttTrackMatch, 1);
};

#endif
