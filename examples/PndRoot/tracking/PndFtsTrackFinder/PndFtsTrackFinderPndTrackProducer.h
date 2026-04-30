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

/**
 *@class PndFtsTrackFinderPndTrackProducer
 *@brief Class that manages conversion from PndFtsTrackFinder::FullTrack to PndTrack and PndTrckCand
 *@author Bartosz Sobol
 *@date: 11.02.2022
 */

#pragma once

#include <TVector3.h>

#include <FairField.h>

#include "PndFtsTrack.h"
#include "PndTrack.h"
#include "PndFtsContext.h"

class PndFtsTrackFinderPndTrackProducer {
 public:
  PndFtsTrackFinderPndTrackProducer(const PndFtsTrackFinder::PndFtsContext &fReconContext, int32_t inBranchId, TClonesArray *inHits);

  /** Creates a PndTrack with PndTrackCand and required links from given PndFtsTrackFinder::PndFtsFullTrack.
   * and writes them into adequate branches.
   * @param track Track in PndFtsTrackFinder format.
   * @param iTrack Index of the track in the current event.
   * @return Pointer to newly created PndTracl
   */
  void CreatePndTrack(const PndFtsTrackFinder::PndFtsFullTrack &track, TObject *&pndTrackDest, TObject *&pndTrackCandDest, TObject *&pndAnalyticTrackDest) const;

 private:
  /**
   * Calculates a position vector on track at given Z coordinate.
   * @param posZ Z coordinate in global coordinate system.
   * @param track Track on which position is ot be calculated.
   * @return Calculated position.
   */
  [[nodiscard]] TVector3 GetPosAtZ(float posZ, const PndFtsTrackFinder::PndFtsFullTrack &track) const;

  /**
   * Calculates a momentum unit vector on track at given Z coordinate.
   * @param pos Position in global coordinate system.
   * @param track Track of particle which momentum is to be calculated.
   * @return Calculated momentum unit vector.
   */
  [[nodiscard]] TVector3 GetMomUnitAtPos(const TVector3 &pos, const PndFtsTrackFinder::PndFtsFullTrack &track) const;

  /**
   * Calculates a particle momentum magnitude for given track.
   * @param track
   * @return Calculated momentum mangitude.
   */
  [[nodiscard]] double GetMomMag(const PndFtsTrackFinder::PndFtsFullTrack &track) const;

  /**
   * Calculates Y component of B-field at given point.
   * @param pos Point in global coordinate system.
   * @return Calculated field component.
   */
  [[nodiscard]] double GetBYFromField(const TVector3 &pos) const;

  [[nodiscard]] double GetRadius(const PndFtsTrackFinder::PndFtsFullTrack &track) const;

  const PndFtsTrackFinder::PndFtsContext &fReconContext; //!< Context for track reconstruction (contains constants, options, etc.)

  int32_t fInBranchId; //!< ID of the ROOT branch containing Hits to process. Also a detector Id for PndTrackCand.

  TClonesArray *fInHits; //!< Pointer to ROOT branch containing input hits.

  FairField *fField; //!< Pointer to PANDA magnetic field object
};
