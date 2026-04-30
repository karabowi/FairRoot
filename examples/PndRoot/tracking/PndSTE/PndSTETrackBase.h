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

/** PndSTETrackBase
 *@author Jenny Regina <jenny.regina@physics.uu.se>
 *@created July 2021
 *@version 1.0
 **
 ** Base class for hit inclusion algorithms
 **
 ** Task Level RECO
 **/

#ifndef PndSTETrackBase_H_
#define PndSTETrackBase_H_

#include "PndTrack.h"
#include "PndTrackCand.h"
#include "PndRiemannTrack.h"
#include "FairLink.h"
#include "FairMultiLinkedData.h"
#include <vector>

#include "PndSTESettings.h"

class PndSTETrackBase : public  PndSTESettings {

 public:
  /** Default Constructor */
  PndSTETrackBase(){};

  /** Default Destructor **/
  virtual ~PndSTETrackBase(){

  };

  /** @brief Function to get a track after the refit */
  PndTrack GetTrack(int i) { return fTracks[i]; };

  /** @brief Function to get a track candidate after the refit */
  PndTrackCand GetTrackCand(int i) { return fTrackCands[i]; };

  /** @brief Function to get a Riemann track after the refit */
  PndRiemannTrack GetRiemannTrack(int i) { return fRiemannTracks[i]; };

  /** @brief Function to get the number of track candidates after the refit */
  int NumTrackCands() { return fTrackCands.size(); };

  /** @brief Function to get the number of Riemann tracks after the refit */
  int NumRiemannTracks() { return fRiemannTracks.size(); };

 protected:

  std::map<PndSdsHit *, int> fMapMvdHitBranchId;

  std::vector<PndTrackCand> fTrackCands;       // Container for the track cands with additional hits included.
  std::vector<PndTrack> fTracks;               // Container for the tracks with additional hits included
  std::vector<PndRiemannTrack> fRiemannTracks; // Container for the Riemann tracks with additional hits included

  /** @brief x-coordinate of the center of gravity of the hits in one track, used for computing hemispheres
   * The x-coordinate but not y-coordinate is not used to test which hemisphere the hit is in */
  double fxc;

  double fLineSlope = 0; // The line slope for hemisphere exclusion

  std::vector<double> fChi2OneHit;                    // Vector of chi2 values for individual hits calculated from their distance to a track
  double fSumChi2 = 0;                                // Sum of all individual chi2 from the hits
  double fReducedChi2 = 0;                            // Reduced chi2 for one track calculated from its hits
  std::map<double, PndSdsHit *> fMapChi2OneHitMvdHit; // Map between Mvd hit and its chi2

  std::vector<double> fChi2OneHitSZ;                    // Vector of chi2 values in SZ space for individual hits calculated from their distance to a track
  double fSumChi2SZ = 0;                                // Sum of all individual chi2 from the hits in SZ space
  double fReducedChi2SZ = 0;                            // Reduced chi2 for one track calculated from its hits in SZ space
  std::map<double, PndSdsHit *> fMapChi2OneHitMvdHitSZ; // Map between Mvd hit and its chi2 in SZ space

  ClassDef(PndSTETrackBase, 1);
};

#endif /* PndSTETrackBase_H_ */
