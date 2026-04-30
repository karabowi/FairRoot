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

/** PndSTEMvd
 *@author Jenny Regina <jenny.regina@physics.uu.se>
 *@updated 09.02.2021
 *@version 1.0
 **
 ** PANDA class for adding compatible hits from the MVD to a track that has already been fitted in the STT.
 ** This function works in both 2D and 3D
 ** 2D: best hits from each layer is included in the track
 ** 3D: all hits in one event is added to the track and the hits are iteratively removed until the best fitting hets remain
 **
 ** Task Level RECO
 **/

#ifndef PndSTEMvd_H_
#define PndSTEMvd_H_

#include "PndSTETrackBase.h"
#include "PndTrackCand.h"
#include "PndRiemannTrack.h"
#include "TClonesArray.h"
#include "PndTrack.h"
#include "PndSdsHit.h"
#include "FairLink.h"
#include "FairMultiLinkedData.h"

#include <vector>

class TClonesArray;

class PndGemDigiPar;

class PndSTEMvd : public PndSTETrackBase {
 public:
  /** Default Constructor **/
  PndSTEMvd(){};

  /** Default Destructor **/
  virtual ~PndSTEMvd(){

  };

  /** @brief Add the chosen MVD hits to the track
   *  @details Fills the tracks and track candidates with MVD hits included and the track re-fitted
   * @param inputTrackArray Already fitted track from STT tracking
   * @param inputRiemannTrackArray Already fitted Riemann track from STT tracking
   * @param inputTrackCandArray Track candidate cotaining the STT hits from the STT tracking
   * @param inputPixelArray Array of MVD pixel hits
   * @param inputStripArray Array of MVD strip hits
   * @return Vector of Riemann tracks that have been re-fitted
   */
  std::vector<PndRiemannTrack>
  AddHits(TClonesArray *inputTrackArray, TClonesArray *inputRiemannTrackArray, TClonesArray *inputTrackCandArray, TClonesArray *inputPixelArray, TClonesArray *inputStripArray);

  ClassDef(PndSTEMvd, 1);
};

#endif /* PndSTEMvd_H_ */
