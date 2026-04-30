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

/*
 * PndHelixPropagator.h
 *
 *  Created on: Sep 23, 2013
 *  Updated: April 2021
 *      Author: stockman, Gaiser
 */

#ifndef PNDHELIXPROPAGATOR_H_
#define PNDHELIXPROPAGATOR_H_

#include <utility>
#include "TVector3.h"
#include "FairTrackPar.h"
#include "PndPropagator.h"
#include "FairLogger.h"
#include "FairField.h"
#include "FairLogger.h"

/**
 * @class PndHelixPropagator
 * @brief Helix propagator for PandaRoot.
 */
class PndHelixPropagator : public PndPropagator {
 protected:
  Double_t fFieldStrength;    ///< magentic field strength in z-Direction in Tesla
  Double_t fCharge;           ///< sign of charge
  Bool_t fBackPropagate;      ///< kTRUE if backward propagation

  Double_t fDeltaPhi;         ///< propagated angle
  FairField *fMagneticField;  ///< magnetic field in detector

  Double_t fA, fB, fC, fD;

  /**
   * @brief Calculate radius of track circle of charged particle.
   * @return radius in cm
   */
  Double_t Radius()
  {
    return fInitialMomentum.Perp() / (fFieldStrength * 0.3) * 100;
  }

  /**
   * @brief Find position of track circle center.
   * @return position of circle center
   */
  TVector3 FindCircleCenter();

 public:
  PndHelixPropagator();             ///< default constructor
  PndHelixPropagator(const TString &name, const TString &title);
  PndHelixPropagator(Double_t fieldStrength, TVector3 origin, TVector3 momentum, Double_t charge);
  virtual ~PndHelixPropagator() {}; ///< default destructor

  /**
   * @brief Detemines the minimum distance of the xy projection of a track to a point.
   * @details The xy projection is assumed to be a circle
   * @param TStart start point of propagated track
   * @return the minimum distance of the xy projection of a track to a point.
   */
  Double_t FindDistanceToPCAXYProjection(FairTrackPar *TStart);

  /**
   * @brief Find point of closest approach on track.
   * @details If the magnetic field does not vanish and the particle has transverse momentum,
   *          FindPCAOnHelix is called. Else FindPCAOnStraightLine is called.
   * @param TStart start point of propagated track
   * @return kTRUE is successful
   */
  Bool_t FindPCA(FairTrackPar* TStart);
  
  /**
   * @brief Find point of closest approach on helix track.
   * @param TStart start point of propagated track
   * @return kTRUE is successful
   */
  Bool_t FindPCAOnHelix();

  /**
   * @brief Find point of closest approach on straight track.
   * @param TStart start point of propagated track
   * @return kTRUE is successful
   */
  Bool_t FindPCAOnStraightLine();

  /**
   * @brief Function to propagate track to point of closest approach.
   * @details This function first calls FindPCA, then propagates the initial momentum 
   *          to the point of closest approach.
   * @param TStart track parameters at point from which to propagate
   * @param TEnd track parameters at end point of propagation
   * @param charge charge of particle
   * @return kTRUE if successful
   */
  Bool_t Propagate(FairTrackPar* TStart, FairTrackPar* TEnd, Int_t charge);

  /**
   * @brief Calculate position and momentum at requested z-Value.
   * @param zpos z-position [cm] to which to propagate
   * @return track parameters at end point of propagation
   */
  FairTrackPar PropagateToZ(Double_t zpos);

  /**
   * @brief Propagate to closest distance to given xy position.
   * @param xypos xy-position to which to propagate
   * @return track parameters at end point of propagation
   */
  FairTrackPar PropagateToXYPos(TVector2 xypos);

  /**
   * @brief Calculate position and momentum at requested arc length.
   * @param arclength arc length to which to propagate
   * @return track parameters at end point of propagation
   */
  FairTrackPar PropagateToS(Double_t arclength);

  /**
   * @brief Calculate position and momentum at requested.
   * @param step angle in deg from start position
   * @return track parameters at end point of propagation
   */
  FairTrackPar PropagateByAngle(Double_t step);

  /**
   * @brief Set PCA propagation.
   * @param pca_mode if = 1: closest approach to point
   *                 if = 2: closest approach to wire
   *                 if = 0: no closest approach
   * @param dir if = +1: move forward
   *            if = -1: move backward
   * @param par initial track parameters
   * @return kTRUE if successful 
   */
  Bool_t SetPCAPropagation(Int_t pca_mode = 1,
                           Int_t dir = 1,
                           [[gnu::unused]] FairTrackPar* par = nullptr)
  {
    if (dir >= 0) fBackPropagate = kFALSE;
    else fBackPropagate = kTRUE;
    if (pca_mode != 1) { // TODO: add other propagation modes -> propagation to wire, volume, and plane
      LOG(info) << "PndHelixPropagator::SetPCAPropagation: only propagation to point implemented at the moment";
      return kFALSE;
    }
    fPcaMode = pca_mode;
    return kTRUE;
  }

  /**
   * @brief Method to set the plane to propagate particles to.
   * @param v0 first plane defining vector
   * @param v1 second plane defining vector
   * @param v2 third plane defining vector
   * @return kTRUE if successful 
   */
  Bool_t SetDestinationPlane([[gnu::unused]] const TVector3& v0,
                             [[gnu::unused]] const TVector3& v1,
                             [[gnu::unused]] const TVector3& v2)
  {
    LOG(info) << "PndHelixPropagator: Propagation to plane not implemented yet";
    return kFALSE;
  }

  /**
   * @brief Method to set the plane to propagate particles from.
   * @param v0 first plane defining vector
   * @param v1 second plane defining vector
   * @return kTRUE if successful
   */
  Bool_t SetOriginPlane([[gnu::unused]] const TVector3& v0, [[gnu::unused]] const TVector3& v1)
  {
    LOG(info) << "PndHelixPropagator: Propagation from plane not implemented yet";
    return kFALSE;
  }

  /**
   * @brief Method to set the volume to propagate particles to.
   * @param volName volume name
   * @param copyNo copy number
   * @param option other options
   * @return kTRUE if successful 
   */
  Bool_t SetDestinationVolume([[gnu::unused]] std::string volName,
                              [[gnu::unused]] Int_t copyNo,
                              [[gnu::unused]] Int_t option)
  {
    LOG(info) << "PndHelixPropagator: Propagation to volume not implemented yet";
    return kFALSE;
  }

  /**
   * @brief Method to set the length to propagate particles to.
   * @param length track length
   * @return kTRUE if successful 
   */
  Bool_t SetDestinationLength([[gnu::unused]] Float_t length)
  {
    LOG(info) << "PndHelixPropagator: Propagation to given track length not implemented yet";
    return kFALSE;
  };

  /**
   * @brief Set initial position and momentum from track parameters.
   * @param TStart initial track parameters
   */
  void Init(FairTrackPar* TStart);

  /**
   * @brief Set field strength in z-Direction.
   * @param field_strength magnetic field strength in Tesla
   */
  void SetFieldStrength(Double_t field_strength) { fFieldStrength = field_strength; };
  
  /**
   * @brief Set magnetic field in detector.
   * @param field magnetic field
   */
  void SetMagneticField(FairField *field) { fMagneticField = field; };

  /**
   * @brief Get difference in phi between initial and final vector.
   */
  Double_t GetDeltaPhi() { return fDeltaPhi; };

  /**
   * @brief Get point to which to propagate.
   */
  TVector3 GetPoint() { return fPoint; };

  /**
   * @brief Get propagation mode.
   */
  Int_t GetPcaMode() { return fPcaMode; };

  ClassDef(PndHelixPropagator, 1)
};

#endif /* PNDHELIXPROPAGATOR_H_ */
