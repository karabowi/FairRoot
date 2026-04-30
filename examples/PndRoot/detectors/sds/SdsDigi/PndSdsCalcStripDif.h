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

//
// C++ Interface: PndSdsCalcStrip
//
// Description:
//
//
// Author: HG Zaunick <hg.zaunick@physik.tu-dresden.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PNDSDSCALCSTRIPDIF_H
#define PNDSDSCALCSTRIPDIF_H

#include "PndSdsStrip.h"
//#include "FairGeoVector.h"
#include "PndSdsStripDigiPar.h"

#include <TVector2.h>
#include <TRandom.h>

#include <iostream>
#include <vector>

#include "PndDetectorList.h"

//! Class for calculating strip indices from wafer hits
/**
 * @author HG Zaunick <hg.zaunick@physik.tu-dresden.de>
 *
 **/
class PndSdsCalcStripDif {
 public:
  /**
   * Default constructor
   * No reason to use it
   */
  PndSdsCalcStripDif();

  /**
   * Constructor
   * Create Object with all necessary parameters
   * @param pitch Pitch between Strips (=width of strips)
   * @param orient Orientation Angle of Strips in x-y-Plane from x-Axis
   * @param nrStrips Number of Strips in the active area
   * @param nrFeChannels Number of Channels per Frontend
   * @param firstStripAnchor Coordinate in x-y-Plane which belongs to the first Strip (Index 0).
   * Usually this is the readout point of the strip
   * @param threshold Charge-Threshold applied to the signal (electrons)
   * @param noise Noise superimposed to the signal (electrons)
   */
  PndSdsCalcStripDif(Double_t pitch, Double_t orient, Int_t nrStrips, Int_t nrFeChannels, const TVector2 &firstStripAnchor, Double_t threshold, Double_t noise, Double_t sigma);

  PndSdsCalcStripDif(const PndSdsStripDigiPar *digipar, SensorSide side = SensorSide::kTOP);

  /**
   * Get List of hit channels from given Wafer hit.
   * @param inx x-coordinate of entry point
   * @param iny y-coordinate of entry point
   * @param inz z-coordinate of entry point
   * @param outx x-coordinate of exit point
   * @param outy y-coordinate of exit point
   * @param outz z-coordinate of exit point
   * @param eLoss Energy Loss inside the detector element (GeV)
   * @return List of fired strips
   */
  std::vector<PndSdsStrip> GetStrips(Double_t inx, Double_t iny, Double_t inz, Double_t outx, Double_t outy, Double_t outz, Double_t eLoss);

  /**
   * Calculate Frontend number from strip-index
   * @param stripNr strip index
   * @return Frontend number
   */
  Int_t CalcFEfromStrip(Int_t stripNr) const;

  /**
   * Calculate Channel index (on Frontend) from strip-index
   * @param stripNr strip index
   * @return channel on frontend
   */
  Int_t CalcChannelfromStrip(Int_t stripNr) const;

  /**
   * Calculate global strip index from Frontend number and -channel
   * @param fe frontend number
   * @param channel frontend channel
   * @param strip global strip index
   * @param side
   */
  void CalcFeChToStrip(Int_t fe, Int_t channel, Int_t &strip, enum SensorSide &side) const;
  void CalcStripPointOnLine(const Double_t strip, TVector2 &point) const;

  /**
   * Get Strip Direction (strip orientation angle)
   */
  const TVector2 GetStripDirection() const { return fStripDir; }

  void SetVerboseLevel(Int_t level) { fVerboseLevel = level; }
  std::ostream &operator<<(std::ostream &out);

  Double_t ChargeDiffusion(Double_t u, Double_t a, Double_t path, Double_t dir, Double_t Q);

  void Print() const;

 private:
  Double_t fPitch;     /// strip pitch (cm)
  Double_t fOrient;    /// strip orientation angle to x axis
  Int_t fNrStrips;     /// Nr. of strips on active area
  Int_t fNrFeChannels; /// Nr of Channels per FE
  TVector2 fAnchor;    /// anchor point on first strip
  Double_t fThreshold; /// charge threshold
  Double_t fNoise;     /// ENC
  TVector2 fStripDir;  /// vector perpendicular to strip direction
  TVector2 fOrthoDir;  /// vector orthogonal to strip direction
  Int_t fVerboseLevel;
  Double_t fSigma;

  /**
   * Calculate continuous strip-number parameter from given Point in local system
   * @param x x-coordinate
   * @param y y-coordinate
   * @return strip-parameter nu
   */
  Double_t CalcStripFromPoint(Double_t x, Double_t y);

  /**
   * Apply gaussian distributed noise to given charge
   * @param charge Equivalent Noise Charge
   * @return Smeared Charge
   */
  Double_t SmearCharge(Double_t charge);
  const Double_t ChargeFromEloss(Double_t eloss) const { return eloss / (3.61e-9); }
};

#endif /* SSDCALCSTRIPDIF_H */
