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
// C++ Interface: MvdCalcStrip
//
// Description:
//
//
// Author: HG Zaunick <hg.zaunick@physik.tu-dresden.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PNDHYPCALCSTRIP_H
#define PNDHYPCALCSTRIP_H

#include "PndHypHit.h"
#include "PndHypStrip.h"
//#include "FairGeoVector.h"
#include "PndHypStripDigiPar.h"

#include <TVector2.h>
#include <TRandom.h>

#include <iostream>
#include <vector>

class PndHypCalcStrip {
 public:
  PndHypCalcStrip();

  PndHypCalcStrip(Double_t pitch, Double_t orient, Int_t nrStrips, Int_t nrFeChannels, const TVector2 &firstStripAnchor, Double_t threshold, Double_t noise);

  PndHypCalcStrip(const PndHypStripDigiPar *digipar, SensorSide side = SensorSide::kTOP);

  std::vector<PndHypStrip> GetStrips(Double_t inx, Double_t iny, Double_t inz, Double_t outx, Double_t outy, Double_t outz, Double_t eLoss, int id);
  /**
   * Calculate Frontend number from strip-index
   * @param stripNr strip index
   * @return Frontend number
   */
  Int_t CalcFEfromStrip(Int_t stripNr) const;
  /**
  84 	     * Calculate Channel index (on Frontend) from strip-index
  85 	     * @param stripNr strip index
  86 	     * @return channel on frontend
  87 	     */
  Int_t CalcChannelfromStrip(Int_t stripNr) const;
  /**
     91 	     * Calculate global strip index from Frontend number and -channel
     92 	     * @param fe frontend number
     93 	     * @param channel frontend channel
     94 	     * @param strip global strip index
     95 	     * @param side
     96 	     */
  void CalcFeChToStrip(Int_t fe, Int_t channel, Int_t &strip, enum SensorSide &side) const;
  void CalcStripPointOnLine(const Double_t strip, TVector2 &point) const;
  /**
   * Get Strip Direction (strip orientation angle)
   */
  const TVector2 GetStripDirection() const { return fStripDir; }
  //	const Int_t GetSizeStrip() const{return fsize;}
  void SetVerboseLevel(Int_t level) { fVerboseLevel = level; }
  // void SetnrFe(Int_t s) {fNrStrips = s;}
  void SetreStrip(Int_t reStp) { fNrStrips = reStp; }
  void SetAnchor(TVector2 edge) { fAnchor = edge; }

  // Int_t GetSize() const {return fsize;}
  // std::ostream& operator<<(std::ostream& out);  // not implemented

  void print() const;

 private:
  Double_t fPitch;     /// strip pitch (cm)
  Double_t fOrient;    /// strip orientation angle
  Int_t fNrStrips;     /// Nr. of strips on active area
  Int_t fNrFeChannels; /// Nr of Channels per FE
  Double_t fThreshold; /// charge threshold
  Double_t fNoise;     /// ENC
  TVector2 fAnchor;    /// anchor point on first strip
  TVector2 fStripDir;  /// vector perpendicular to strip direction
  TVector2 fOrthoDir;  /// vector orthogonal to strip direction
  Int_t fVerboseLevel;
  Int_t fsize;

  int _id;

  /**
     125 	     * Calculate continuous strip-number parameter from given Point in local system
     126 	     * @param x x-coordinate
     127 	     * @param y y-coordinate
     128 	     * @return strip-parameter nu
     129 	     */

  Double_t CalcStripFromPoint(Double_t x, Double_t y);
  /**
     133 	     * Apply gaussian distributed noise to given charge
     134 	     * @param charge Equivalent Noise Charge
     135 	     * @return Smeared Charge
     136 	     */
  Double_t SmearCharge(Double_t charge);
  inline Double_t ChargeFromEloss(Double_t eloss) const { return eloss / (3.61e-9); }
};

#endif /* PNDHYPCALCSTRIP_H */
