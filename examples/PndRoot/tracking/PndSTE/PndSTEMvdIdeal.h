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

/** PndSTEMvdIdeal
 *@author Jenny Regina <jenny.regina@physics.uu.se>
 *@created 08.07.2021
 *@version 1.0
 **
 ** PANDA class for finding compatible hits in the MVD from ideally clustered hits
 **
 ** Task Level RECO
 **/

#ifndef PndSTEMvdIdeal_H_
#define PndSTEMvdIdeal_H_

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

class PndSTEMvdIdeal : public PndSTETrackBase {
 public:
  /** Default Constructor **/
  PndSTEMvdIdeal(){};

  /** Default Destructor **/
  virtual ~PndSTEMvdIdeal(){

  };

  /** @brief Function to find MVD hits from clustered STT hits
   *  @details A Riemann track is fitted to the clustered STT hits
   *           This function can be used for testing purpouses using the IdealTrackFinder tracks as input
   * @param inputTrackArray Already fitted track from STT tracking
   * @param inputTrackCandArray Track candidate cotaining the STT hits from the STT tracking
   * @param inputPixelArray Array of MVD pixel hits
   * @param inputStripArray Array of MVD strip hits
   */
  void AddHits(TClonesArray *inputTrackArray, TClonesArray *inputTrackCandArray, TClonesArray *inputPixelArray, TClonesArray *inputStripArray);

 private:
  /** @brief Function to fit a Riemann track to hits that have already been clustered
   *  @details A Riemann track is fitted to the clustered STT hits
   *           This function can be used for testing purpouses using the IdealTrackFinder tracks as input
   * @param inputTrackArray Already fitted track from STT tracking
   * @return Vector of fitted Riemann tracks
   */
  std::vector<PndRiemannTrack> CreateRiemannTrack(TClonesArray *inputTrackArray);

  ClassDef(PndSTEMvdIdeal, 1);
};

#endif /* PndSTEMvdIdeal_H_ */
