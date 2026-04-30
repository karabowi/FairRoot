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

/** PndSTEGem
 *@author Jenny Regina <jenny.regina@physics.uu.se>
 *@updated 09.02.2021
 *@version 1.0
 **
 ** PANDA class for finding compatible hits in the GEM from a track that has already been fitted
 ** A helix is extrapolated to the z-position each GEM station from the last hit in the STT fitted track.
 ** If GEM hits are found within a certain user set distance of closest approach between the track and the
 ** hit, the hit is added to the track.
 ** The track is subsequently refitted.
 ** There is no restriction on how many GEM hits from each station that can be added to a track in order to
 ** account for hits beeing registered in more planes.
 **
 ** Task Level RECO
 **/

#ifndef PndSTEGem_H_
#define PndSTEGem_H_

#include "PndSTETrackBase.h"
#include "PndGemHit.h"
#include "TClonesArray.h"

#include <vector>

class TClonesArray;

class PndSTEGem : public PndSTETrackBase {
 public:
  /** Default Constructor **/
  PndSTEGem(){};

  /** Default Destructor **/
  virtual ~PndSTEGem(){

  };

  /** @brief Function to add the chosen hits to a track candidate
   *  @details Fills the tracks and track candidates with GEM hits included and the track re-fitted
   *  @param inputTrackArray Array of tracks from STT tracking
   *  @param inputTrackCandArray Array of track candidates from STT tracking
   *  @param inputHitArray Array of all GEM hits
   */
  void AddHits(TClonesArray *inputTrackArray, TClonesArray *inputTrackCandArray, TClonesArray *inputHitArray);

 private:
  /** @brief Main function to use the helix propagator to propagate STT fitted track to GEM planes
   *  @details The propagation is performed from the last point in the track which corresponds to the last STT hit
   *           This function can be used with or without a Riemann fitted track
   *           This function requires the z-component of the momentum to be present
   *  @param inputTrack The track to test for compatibility with hits
   *  @param gemHitArray The array of GEM hits
   *  @return Vector of compatible GEM hits
   */
  std::vector<PndGemHit *> FindBestHitsWithHelix(PndTrack inputTrack, TClonesArray *gemHitArray);

  ClassDef(PndSTEGem, 1);
};

#endif /* PndSTEGem_H_ */
