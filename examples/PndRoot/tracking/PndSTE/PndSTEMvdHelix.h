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

/** PndSTEMvdHelix
 *@author Jenny Regina <jenny.regina@physics.uu.se>
 *@created 12.07.2021
 *@version 1.0
 **
 ** PANDA class for adding compatible hits from the MVD to a track that has already been fitted using a helix propagation
 **
 ** Task Level RECO
 **/

#ifndef PndMvdTrackerHelix_H_
#define PndMvdTrackerHelix_H_

#include "PndSTETools.h"
#include "PndSdsHit.h"
#include "FairMultiLinkedData.h"

#include <vector>

class PndSTEMvdHelix : public PndSTETrackBase {

 public:
  /** Default Constructor **/
  PndSTEMvdHelix(){};

  /** Default Destructor **/
  virtual ~PndSTEMvdHelix(){

  };

  /** @brief Add hits to the track when a helix extrapolation is performed
   * @details Fills the tracks and track candidates with MVD hits included and the track re-fitted
   * @param inputTrackArray Already fitted track from STT tracking
   * @param inputTrackCandArray Track candidate cotaining the STT hits from the STT tracking
   * @param inputPixelArray Array of MVD pixel hits
   * @param inputStripArray Array of MVD strip hits
   * @return Vector of tracks tracks that have been re-fitted
   */
  std::vector<PndTrack> AddHits(TClonesArray *inputTrackArray, TClonesArray *inputTrackCandArray, TClonesArray *inputPixelArray, TClonesArray *inputStripArray);

  ClassDef(PndSTEMvdHelix, 1);
};

#endif /* PndSTEMvdHelix_H_ */
