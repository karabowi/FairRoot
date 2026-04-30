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

/** PndSTEBTof
 *@author Jenny Regina <jenny.regina@physics.uu.se>
 *@updated 09.02.2021
 *@version 1.0
 **
 ** PANDA class for finding compatible hits in the Barrel ToF from a track that has already been fitted
 **
 ** Task Level RECO
 **/

#ifndef PndSTEBTof_H_
#define PndSTEBTof_H_

#include "PndSTETrackBase.h"
#include "PndSciTHit.h"
#include "TClonesArray.h"

#include <vector>

class TClonesArray;

class PndSTEBTof : public PndSTETrackBase {
 public:
  /** Default Constructor **/
  PndSTEBTof(){};

  /** Default Destructor **/
  virtual ~PndSTEBTof(){

  };

  // TODO: THis funcction might need to be changed since one do not want PID detector hits in a track
  /** @brief Function to add the chosen hit to a track candidate
   *  @details Fills the tracks and track candidates with BTOF hits included but no refit is done with the hits
   *  @param initialTrackArray Array of tracks from STT tracking
   *  @param initialTrackCandArray Array of track candidates from STT tracking
   *  @param initialHitArray Array of all BTOF hits
   */
  void AddHit(TClonesArray *initialTrackArray, TClonesArray *initialTrackCandArray, TClonesArray *initialHitArray);

 private:
  /** @brief Function to use the helix propagator to propagate STT fitted track to BTOF
   *  @details The propagation is performed from the last point in the track which corresponds to the last STT hit
   *           This function can be used with or without a Riemann fitted track
   *           This function requires the z-component of the momentum to be present
   *  @param btofTrack The track to test for compatibility with hits
   *  @param inputHitArray All input BTOF hits in one event
   *  @return The btof hit closest to the track
   */
  PndSciTHit *FindBestHitWithHelix(PndTrack btofTrack, TClonesArray *inputHitArray);

  ClassDef(PndSTEBTof, 1);
};

#endif /* PndSTEBTof_H_ */
