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

/** PndSTETools
 *@author Jenny Regina <jenny.regina@physics.uu.se>
 *@created July 2021
 *@version 1.0
 **
 ** PANDA class for functions that are used in the hit finding algorithm
 **
 ** Task Level RECO
 **/

#ifndef PndSTETools_H_
#define PndSTETools_H_

#include "PndSTETrackBase.h"
#include "FairRootManager.h"
#include "FairMultiLinkedData.h"
#include "PndRiemannTrack.h"
#include "PndSdsHit.h"
#include "PndSTEDraw.h"

class TClonesArray;
class FairMultiLinkedData;

class PndSTETools : public PndSTETrackBase {

 public:
  /** Default Constructor **/
  PndSTETools(){};

  /** Default Destructor **/
  virtual ~PndSTETools(){};

  /** @brief A function that returns the distance between a MVD hit and the Riemann track
   *  @param mvdRiemannTrack Input Riemann track
   *  @param mvdHit Input Mvd hit
   *  @return The distance between the hit and track
   */
  double GetDistanceToTrack(PndRiemannTrack mvdRiemannTrack, PndSdsHit *mvdHit);

  /** @brief A function that returns the distance between a Riemann hit and the Riemann track
   *  @param mvdRiemannTrack Input Riemann track
   *  @param hit Input Riemann hit
   *  @return The distance between the hit and track
   */
  double GetDistanceToTrackRiemannHit(PndRiemannTrack mvdRiemannTrack, PndRiemannHit hit);

  /** @brief Function to compare the error between two Riemann tracks between two steps of the iterations
   *  @param oldRiemannTrack Riemann track from iteration i-1 in 3D method for including Mvd hits
   *  @param newRiemannTrack Riemann track from iteration i in 3D method for including Mvd hits
   *  @return Riemann track with smallest error in the radius
   */
  PndRiemannTrack CompareError(PndRiemannTrack oldRiemannTrack, PndRiemannTrack newRiemannTrack);

  /** @brief Funtion returning the value of fNewTrack. Can be used in other classes to determine
   * which Riemann track to use in the continued tracking
   * @return true if the track in iteration i-1 has smaller error than the track from iteration i
   */
  bool NewTrackHasSmallestError() { return fNewTrack; };

  /** @brief Function to examine if a specific hit has already been included in a track
   *  @param mvdHit Hit to be tested if it is in a certain track
   *  @param trackCand Track cand corresponding to the track the hit is tested for
   *  @return true if hit is in track, false if hit is not in track
   */
  bool IsHitInTrack(PndSdsHit *mvdHit, PndTrackCand trackCand);

  /** @brief Function to calculate the slope of the line defining the hemisphere surface
   *  @details The slope is set within the function and is accessible from other classes
   *  @param track The track for which the slope should be calculated
   */
  void CalcSlope(PndTrack *track);

  /** @brief Function returning the value of the line slope for a track
   *  @return The line slope
   */
  double GetLineSlope() { return fLineSlope; }

  /** @brief Function that refits the Riemann track with a set of MVD hits.
   * @details The function returns all MVD hits except the worst ftted one,
   * i.e. the one with the largest contribution to the chi2
   * @param riemannTrack Riemann track to be refitted
   * @param mvdHits Vector of Mvd hits to add to the track
   * @return All Mvd hits except the one with the largest contribution to the chi2
   */
  std::vector<PndSdsHit *> RefitRiemannTrack(PndRiemannTrack riemannTrack, std::vector<PndSdsHit *> mvdHits);

  /** @brief Function for choosing to draw the tracks with hits */
  void DrawTracks(bool val) { fDrawTrack = val; };

  /** @brief Function to calculate the chi2 of a Riemann track with MVD hits
   *  @param riemannTrack Input Riemann track for the calculation
   *  @param mvdHitVector hits to be added to the track for the calculation
   */
  void CalcChi2(PndRiemannTrack riemannTrack, std::vector<PndSdsHit *> mvdHitVector);

  /** @brief Function to calculate the chi2 of a Riemann track with MVD hits in SZ plane
   *  @param riemannTrack Input Riemann track for the calculation
   *  @param mvdHitVector hits to be added to the track for the calculation
   */
  void CalcChi2SZ(PndRiemannTrack riemannTrack, std::vector<PndSdsHit *> mvdHitVector);

  /** @brief Function that calculates the residuals (distances) between the track and the hits in SZ space
   *  @param track Input Riemann track for the calculation
   *  @param mvdHitVec hits to be added to the track for the calculation
   */
  void CalcResidual(PndRiemannTrack track, std::vector<PndSdsHit *> mvdHitVec);

  /** @brief Function to test if the MVD hit is in the same hemisphere as the Center-of-Gravity of the STT hits in the track
   *  @param myHit Mvd hit to be tested
   *  @param xc X position of the center of the Center-of-Gravity of the track
   *  @param lineSlope Already calculated line slope of the track
   *  @return true if hit is on opposite hemisphere, false if it is in the same hemiphere
   */
  bool IsHitInCorrectHemisphere(PndSdsHit *myHit, double xc, double lineSlope);

 private:
  /** @brief Set to true if an updated Riemann track is found to have a smaller error than an old track
   * This variable is set when using the function CompareError()
   */
  bool fNewTrack = false;

  bool fDrawTrack = false; // true if the tracks should be drawn, false otherwise

  ClassDef(PndSTETools, 1);
};

#endif /*PndSTETools_H_*/
