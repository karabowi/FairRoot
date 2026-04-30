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

/** PndSTESettings
 *@author Jenny Regina <jenny.regina@physics.uu.se>
 *@created July 2021
 *@version 1.0
 **
 ** Class containing variables and settings common
 ** for hit inclusion algorithms and the task
 **
 ** Task Level RECO
 **/

#ifndef PndSTESettings_H_
#define PndSTESettings_H_

#include "PndTrack.h"
#include "PndTrackCand.h"
#include "PndRiemannTrack.h"
#include "PndSdsHit.h"
#include "FairLink.h"
#include "FairMultiLinkedData.h"
#include "FairField.h"
#include <vector>

class PndSTESettings {

 public:
  /** Default Constructor */
  PndSTESettings(){};

  /** Default Destructor **/
  virtual ~PndSTESettings(){

  };

  /** @brief Function to set the magnetic field strength */
  void SetMagneticField();

  double GetMagneticField() { return fBz; }

  /** @brief Function to choose which detectors to include */
  void IncludeDetector(bool includeMvd, bool includeGem, bool includeBtof)
  {
    fIncludeMvd = includeMvd;
    fIncludeGem = includeGem;
    fIncludeBtof = includeBtof;
  };

  /** @brief Function to set the maximum allowed distance of closest approach between the MVD hit and the track */
  void SetMvdHitDist(Double_t val) { fMvdHitDist = val; };

  /** @brief Function to set the maximum allowed distance of closest approach between the GEM hit and the track**/
  void SetGemHitDist(Double_t val) { fGemHitDist = val; };

  /** @brief Function to set the maximum allowed distance of closest approach between the BTOF hit and the track */
  void SetBtofHitDist(Double_t val) { fBtofHitDist = val; };

  /** @brief Function to set ia a helix extrapolation should be used in the procedure */
  void SetUseHelix(Bool_t val) { fUseHelix = val; };

  /** @brief Function to set ia a Riemann track should be used in the procedure */
  void SetUseRiemannTrack(Bool_t val) { fUseRiemann = val; };

  /** @brief Function to choose to give an ideal track as input to the MVD hit fiinding
   *  @details This is for testing purpouses to minimize possible biases from previous hit
   * clusterization algorithms.
   * If an ideal track is used, a Riemann track will be fitted to the clustered hits
   */
  void SetUseIdealTrack(bool val) { fIdealTrack = val; };

  /** @brief Function to choose if the 2D or 3D function should be used
   *   @details If fRunIn3D = true the 3D funtion is used, if fRunIn3D = false the 2D function is used
   * */
  void SetRunIn3D(bool val) { fRunIn3D = val; };

  /** @brief Function to choose if there is an area exclusion in the Mvd hit inclusion
   * @details By default there is no area exclusion.
   * This function is used to choose to exclude no area,
   * fNoMVDAreaExclusion=true if no area should be excluded */
  void SetNoAreaExclusion(bool val) { fNoMVDAreaExclusion = val; };

  /** @brief Function to choose to use the hemispehere method for excluding a certain area of the detector */
  void SetUseHemisphere(bool useHemisphere) { fUseHemisphere = useHemisphere; };

  /** @brief Function to set the proper weights for the MVD hits for the Riemann fitting procedure */
  void SetWeightsMvdHit(double weight)
  {
    fWeightMVD = weight;
    fAdjustWeightsMvd = true;
  };

  /** Function to set if a track should be drawn */
  void SetDrawTracks(bool val) { fDrawTrack = val; }

 protected:
  bool fIncludeMvd = false;  // true if Mvd hits should be used
  bool fIncludeGem = false;  // true if Gem hits should be used
  bool fIncludeBtof = false; // true if Btof hits should be used

  double fBz; // In units of Tesla [T]

  bool fUseHelix = false;           // true if helix extrapolation should be used
  bool fIdealTrack = false;         // true if ideal track should be used
  bool fUseRiemann = false;         // true if Riemann track should be used
  bool fRunIn3D = false;            // true if 3D method should be used
  bool fUseHemisphere = false;      // true if hemisphere exclusion should be used
  bool fNoMVDAreaExclusion = false; // true if no area should be excluded in the Mvd hit exclusion
  bool fAdjustWeightsMvd = false;   // If true the weight has been adjusted
  double fWeightMVD = -1.0;         // Default adjusted weight

  double fMvdHitDist = 9999999.0;  // Maximum distance between Mvd hit and extrapolated or Riemann track
  double fGemHitDist = 9999999.0;  // Maximum distance between Gem hit and extrapolated track.
  double fBtofHitDist = 9999999.0; // Maximum distance between Btof hit and extrapolated track.

  bool fDrawTrack = false; // True if a track should be drawn

  ClassDef(PndSTESettings, 1);
};

#endif /* PndSTESettings_H_ */
