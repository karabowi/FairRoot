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
 * PndPropagator.h
 *
 *  Created: April 2021
 *   Author: Gaiser
 */

#ifndef PNDPROPAGATOR_H
#define PNDPROPAGATOR_H

#include <Rtypes.h>      // for THashConsistencyHolder, ClassDef
#include <RtypesCore.h>  // for kTRUE
#include <iosfwd>        // for string
#include <string>        // for basic_string

#include "TNamed.h"      // for TNamed
#include "TString.h"     // for TString
#include "TVector3.h"    // for TVector3

#include "FairLogger.h"
#include "FairField.h"
#include "FairTrackPar.h"

namespace PndProp {
  /**
   * @struct PCAOutputStruct
   * @brief output of PCA finding algorithm
   */
  struct PCAOutputStruct
  {
    PCAOutputStruct()
      : PCAStatusFlag(1)                  ///< if 0: success, else: failure
      , Radius(0.)                        ///< radius if the found circle
      , OnTrackPCA(TVector3(0., 0., 0.))  ///< point of closest approach on track
      , OnWirePCA(TVector3(0., 0., 0.))   ///< point of closest approach on wire
      , Distance(0.)                      ///< distance between track and wire in the PCA
      , TrackLength(0.)                   ///< track length to add to the GEANE one
    {}
    Int_t PCAStatusFlag;
    Double_t Radius;
    TVector3 OnTrackPCA;
    TVector3 OnWirePCA;
    Double_t Distance;
    Float_t TrackLength;
  };
}

/**
 * @class PndPropagator
 * @brief Propagator interface class for PandaRoot.
 */
class PndPropagator : public TNamed
{
 protected:
  Int_t fPcaMode;             ///< if 1: propagate to point, if 2: propagate to line, if 0: no pca
  TVector3 fPoint;            ///< point to which to calculate pca
  TVector3 fWire1;            ///< first anchor point of line to which to calculate pca
  TVector3 fWire2;            ///< second anchor point of line to which to calculate pca
  Float_t fDestinationLength; ///< track length when PropagateToTrack is chosen
  TString fVolumeName;        ///< volume name
  Int_t fVolumeCopyNo;        ///< volume copy number
  Bool_t fVolumeEnter;        ///< true if enter volume

  TVector3 fInitialPosition;  ///< initial position in cm
  TVector3 fInitialMomentum;  ///< initial momentum in GeV
  TVector3 fFinalPosition;    ///< final position
  TVector3 fFinalMomentum;    ///< final momentum

  PndProp::PCAOutputStruct fPcaOutput;

 public:
  PndPropagator(); ///< default constructor
  PndPropagator(const TString& name, const TString& title);

  virtual ~PndPropagator() {}; ///< default destructor

  virtual void Init(FairTrackPar* /* TParam */){};

  /**
   * @brief Propagate track to point, wire, plane or volume.
   * @param TStart track parameters at point from which to propagate
   * @param TEnd track parameters at end point of propagation
   * @param PDG pdg code of the particle to propagate
   * @return kTRUE if successful
   */
  virtual Bool_t Propagate([[gnu::unused]] FairTrackPar* TStart,
                           [[gnu::unused]] FairTrackPar* TEnd,
                           [[gnu::unused]] Int_t PDG)
  {
    return kFALSE;
  }

  /**
   * @brief Propagate track to point, wire, plane or volume.
   * @param x1 coordinates of point from which to propagate
   * @param p1 momentum at point from which to propagate
   * @param x2 coordinates of end point of propagation
   * @param p2 momentum at end point of propagation
   * @param PDG pdg code of the particle to propagate
   * @return kTRUE if successful
   */
  virtual Bool_t Propagate([[gnu::unused]] Float_t* x1,
                           [[gnu::unused]] Float_t* p1,
                           [[gnu::unused]] Float_t* x2,
                           [[gnu::unused]] Float_t* p2,
                           [[gnu::unused]] Int_t PDG)
  {
    return kFALSE;
  }

  /**
   * @brief Find point of closest approach to point or wire.
   * @param pca if = 1: closest approach to point
   *            if = 2: closest approach to wire
   *            if = 0: no closest approach
   * @param PDGCode pdg code of the particle
   * @param point point with respect to which calculate the closest approach
   * @param wire first anchor point of line with respect to which calculate the closest approach
   * @param wire2 second anchor point of line with respect to which calculate the closest approach
   * @param maxdistance geometrical distance[start - point/wire extr] * 2
   * @return kTRUE if successful 
   */
  virtual PndProp::PCAOutputStruct FindPCA([[gnu::unused]] Int_t PCA,
                                           [[gnu::unused]] Int_t PDGCode,
                                           [[gnu::unused]] TVector3 Point,
                                           [[gnu::unused]] TVector3 Wire1,
                                           [[gnu::unused]] TVector3 Wire2,
                                           [[gnu::unused]] Double_t MaxDistance)
  {
    return PndProp::PCAOutputStruct();
  }

  /* =========================================================================================================================================== */
  
  /**
   * @brief Method to set to propagate only parameters
   * @return kTRUE if successful 
   */
  virtual Bool_t SetPropagateOnlyParameters() { return kFALSE; }

  /**
   * @brief Set PCA propagation.
   * @param pca if = 1: closest approach to point
   *            if = 2: closest approach to wire
   *            if = 0: no closest approach
   * @param dir if = +1: move forward
   *            if = -1: move backward
   * @param par initial track parameters
   * @return kTRUE if successful 
   */
  virtual Bool_t SetPCAPropagation([[gnu::unused]] Int_t pca,
                                   [[gnu::unused]] Int_t dir = 1,
                                   [[gnu::unused]] FairTrackPar* par = nullptr)
  {
    return kTRUE;
  }

  /**
   * @brief Method to set the plane to propagate particles to.
   * @param v0 first plane defining vector
   * @param v1 second plane defining vector
   * @param v2 third plane defining vector
   * @return kTRUE if successful 
   */
  virtual Bool_t SetDestinationPlane([[gnu::unused]] const TVector3& v0,
                                     [[gnu::unused]] const TVector3& v1,
                                     [[gnu::unused]] const TVector3& v2)
  {
    return kFALSE;
  }

  /**
   * @brief Method to set the plane to propagate particles from.
   * @param v0 first plane defining vector
   * @param v1 second plane defining vector
   * @return kTRUE if successful
   */
  virtual Bool_t SetOriginPlane([[gnu::unused]] const TVector3& v0, [[gnu::unused]] const TVector3& v1)
  {
    return kFALSE;
  }

  /**
   * @brief Method to set the volume to propagate particles to.
   * @param volName volume name
   * @param copyNo copy number
   * @param option other options
   * @return kTRUE if successful 
   */
  virtual Bool_t SetDestinationVolume(std::string volName,
                                      Int_t copyNo,
                                      Int_t option)
  {
    fVolumeName = volName;
    fVolumeCopyNo = copyNo;
    if (option == 1) fVolumeEnter = kTRUE;
    else fVolumeEnter = kFALSE;
    return kTRUE;
  }

  /**
   * @brief Method to set the length to propagate particles to.
   * @param length track length
   * @return kTRUE if successful 
   */
  virtual Bool_t SetDestinationLength(Float_t length)
  {
    fDestinationLength = length;
    return kFALSE;
  };

  /**
   * @brief Method to set the point to propagate particles to.
   * @param point destination point
   * @return kTRUE if successful 
   */
  virtual Bool_t SetDestinationPoint(const TVector3 point)
  {
    fPoint = point;
    return kTRUE;
  };

  /**
   * @brief Method to set wire to propagate to.
   * @param wire1 first anchor point of wire to propagate to
   * @param wire2 second anchor point of wire to propagate to
   */
  virtual Bool_t SetDestinationWire(const TVector3 wire1, const TVector3 wire2)
  {
    fWire1 = wire1;
    fWire2 = wire2;
    return kTRUE;
  };

  /* =========================================================================================================================================== */

  /**
   * @brief get PCAOutputStruct containing all relevant pca informations.
   */
  virtual PndProp::PCAOutputStruct GetPcaOutput() { return fPcaOutput; };

  /**
   * Get Track length at point of closest approach.
   */
  virtual Float_t GetLengthAtPCA() { return fPcaOutput.TrackLength; };
  
  /**
   * @brief Get distance between point of closest approach and vertex.
   */
  virtual Float_t GetDistAtPCA() { return fPcaOutput.Distance; };
  
  /**
   * @brief Get position of pca on track.
   */
  virtual TVector3 GetPCA() { return fPcaOutput.OnTrackPCA; };

  ClassDef(PndPropagator, 1);
};

#endif //PNDPROPAGATOR_H