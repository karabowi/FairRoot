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
// -----                   PndGemTrackFinderIdeal header file          -----
// -----                  Created 19.03.2009 by R. Karabowicz          -----
// -----               according to the PndDchTrackFinderIdeal         -----
// -------------------------------------------------------------------------

/** \class PndGemTrackFinderIdeal
 *  \author R. Karabowicz <r.karabowicz@gsi.de>
 *  \date 19.03.2009
 *  \brief Ideal track finding algorithm
 *
 *  Track finding procedure operates on mc data.
 *  Reads MC truth (MCTracks and MCPoints), creates
 *  one PndTrack for each MCTrack and attaches the hits according
 *	to the MCTrack of the corresponding MCPoint
 **/

#ifndef PNDGEMTRACKFINDERIDEAL_H
#define PNDGEMTRACKFINDERIDEAL_H

#include "PndMCTrack.h"

#include "PndGemHit.h"
#include "PndGemTrackFinder.h"

class PndGemTrackFinderIdeal : public PndGemTrackFinder {

 public:
  /** Default constructor **/
  PndGemTrackFinderIdeal();

  /** Destructor **/
  virtual ~PndGemTrackFinderIdeal();

  /** Initialisation **/
  virtual void Init();

  /** DoFind method
   * \param hitArray    Array of Gem hits
   * \param trackArray  Array of PndGemTrack
   * \return  Number of created tracks
   **/
  virtual Int_t DoFind(TClonesArray *hitArray, TClonesArray *trackArray, TClonesArray *trackCandArray);

  /** Public modifiers **/
  void SetVerbose(const Int_t &verbose) { fVerbose = verbose; };
  void SetPrimary(const Int_t &primary) { fPrimary = primary; };

 private:
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

  /** Event counter **/
  Int_t fNofEvents; ///< event counter

  ClassDef(PndGemTrackFinderIdeal, 1);
};

#endif
