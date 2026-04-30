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

/** PndSTEMvdHitFinder
 *@author Jenny Regina <jenny.regina@physics.uu.se>
 *@updated 09.02.2021
 *@version 1.0
 **
 ** PANDA class for finding compatible hits in the MVD from a track that has already been fitted
 ** This class only finds the best hits using different algorithms but do not add them to a track
 ** since additional criteria might be used for this.
 **
 ** Task Level RECO
 **/

#ifndef PndSTEMvdHitFinder_H_
#define PndSTEMvdHitFinder_H_

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

class PndSTEMvdHitFinder : public PndSTETrackBase {
 public:
  /** Default Constructor **/
  PndSTEMvdHitFinder(){};

  /** Default Destructor **/
  virtual ~PndSTEMvdHitFinder(){

  };

  /** @brief Function for finding the best fitting MVD hits to the track in 3D
   * @param mvdRiemannTrack Already fitted Riemann track from STT tracking
   * @param mvdPixelHitsArray Array of MVD pixel hits
   * @param mvdStripHitsArray Array of MVD strip hits
   * @return A vector of compatible hits
   */
  std::vector<PndSdsHit *> FindBestHits3D(PndRiemannTrack mvdRiemannTrack, TClonesArray *mvdPixelHitsArray, TClonesArray *mvdStripHitsArray);

  /** @brief Function to find the hits fitting the best with one Riemann track
   * @details This function only handles barrel layers
   * @param riemannTrack Already fitted Riemann track from STT tracking
   * @param mvdPixelHitsArray Array of MVD pixel hits
   * @param mvdStripHitsArray Array of MVD strip hits
   * @return A vector of compatible hits
   * */
  std::vector<PndSdsHit *> FindBestHitsFromRiemannTrack(PndRiemannTrack riemannTrack, TClonesArray *mvdPixelHitsArray, TClonesArray *mvdStripHitsArray);

  /** @brief Function for finding the best fitting MVD hits to the track in 3D
   * @details A helix propagation is used to find the best hits
   * @param mvdTrack Already fitted track from STT tracking
   * @param mvdPixelHitsArray Array of MVD pixel hits
   * @param mvdStripHitsArray Array of MVD strip hits
   * @return A vector of compatible hits
   */
  std::vector<PndSdsHit *> FindBestHitsWithHelix(PndTrack *mvdTrack, TClonesArray *mvdPixelHitsArray, TClonesArray *mvdStripHitsArray);

  /** @brief Funtion to set the x and y psition of the first hit of the track
   * @details Used for the quadrant exclusion
   * @param x X-position of the first hit in the track
   * @param y Y-position of the first hit in the track
   */
  void SetXYPosTrack(double x, double y)
  {
    fXposTrack = x;
    fYposTrack = y;
  }

 private:
  double fXposTrack = 0.0; // X-position of the first hit in the track
  double fYposTrack = 0.0; // Y-position of the first hit in the track

  ClassDef(PndSTEMvdHitFinder, 1);
};

#endif /* PndSTEMvdHitFinder_H_ */
