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

/** PndSTETrackCreator
 *@author Jenny Regina <jenny.regina@physics.uu.se>
 *@created 14.07.2021
 *@version 1.0
 **
 ** PANDA class that handles the interface between the different classes that creates the tracks and the task
 **
 ** Task Level RECO
 **/

#ifndef PndSTETrackCreator_H_
#define PndSTETrackCreator_H_

#include "PndSTETrackBase.h"

#include "TClonesArray.h"

#include <vector>

class TClonesArray;

class PndSTETrackCreator : public PndSTETrackBase {
 public:
  /** Default Constructor */
  PndSTETrackCreator(){};

  /** Default Destructor **/
  virtual ~PndSTETrackCreator(){

  };

  /** @brief Function to call the chosen function to add Mvd hits to a track
   *  @details Handles all algorithms for Mvd hit inclusion
   *  @param inputTrackArray Array of tracks already fitted in the STT
   *  @param inputRiemannTrackArray Array of Riemann tracks fitted in the STT
   *  @param inputTrackCandArray Array of track cands of the tracks fitted in the STT
   *  @param inputPixelArray Array of pixel hits that should be tested if they should be added to the track
   *  @param inputStripArray Array of strip hits that should be tested if they should be added to the track
   */
  void AddHitsToMvdTrack(TClonesArray *inputTrackArray, TClonesArray *inputRiemannTrackArray, TClonesArray *inputTrackCandArray, TClonesArray *inputPixelArray,
                         TClonesArray *inputStripArray);

  /** @brief Function to call the chosen function to add Gem or Btof hits to a track
   *  @details Handles all algorithms for Mvd hit inclusion
   *  @param inputTrackArray Array of tracks already fitted in the STT
   *  @param inputTrackCandArray Array of track cands of the tracks fitted in the STT
   *  @param inputGemOrBtofArray Array of pixel hits that should be tested if they should be added to the track
   */
  void AddHitsToGemOrBtofTrack(TClonesArray *inputTrackArray, TClonesArray *inputTrackCandArray, TClonesArray *inputGemOrBtofArray);

  /** @brief The function that calls the correct algorithms according to user settings */
  void CreateTracks();

 private:
  TClonesArray *fInputPndTracks;        // Array of input tracks
  TClonesArray *fInputPndRiemannTracks; // Array of input Riemann tracks
  TClonesArray *fInputPndTrackCands;    // Array of track cands

  TClonesArray *fInputMvdHitsPixels; // Input pixel hit array
  TClonesArray *fInputMvdHitsStrips; // Input strip hit array

  TClonesArray *fInputGemOrBtofHits; // Input Gem hit or Btof hit array

  ClassDef(PndSTETrackCreator, 1);
};

#endif /* PndSTETrackCreator_H_ */
