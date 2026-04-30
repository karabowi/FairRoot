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
 * PndLmdCalStrip.h *
 *  Created on: Jul 28, 2009
 *      Author: huagen
 */
//*************************************************************************************
// this class PndLmdCalStrip will handle the different strip structure of the silicon
// sensor.The different strip structure shall be covered, for example, short strips in
// two segments, curved strips in one segment etc. Due to thin sensor the strips shall
// be made in one side.
// The charge diffusion after ionization will be taken into consideration. The gaussian
// smearing with sigma 8um was employeed for the charge diffussion
// For each strip fired the noise shall be added
//**************************************************************************************

#ifndef PNDLMDCALSTRIP_H_
#define PNDLMDCALSTRIP_H_

#include <iostream>
#include <cmath>
#include "PndLmdDigiPara.h"
#include "TRandom.h"

#include "PndLmdStrips.h"

#include "TVector2.h"
#include <vector>
#include "PndDetectorList.h"

class PndLmdCalStrip {
 public:
  // default constructor
  PndLmdCalStrip();
  // pitch @param the width of the strip
  // orient @param the strip orient angle cross with x axis
  // firstStripAnchor @param the first pitch point
  // threshold @param the threshold of the signal
  // noise @param the electric noise
  // nrStrips @param the number of strips
  // nrFeChannels @param the number of channels per FE modules
  PndLmdCalStrip(Double_t pitch, Double_t orient, Int_t nrStrips, Int_t nrFeChannels, const TVector2 &firstStripAnchor, Double_t threshold, Double_t noise, Double_t sigma);

  // fdigipara @ the digitization parameters stored with ASCii file
  // segment @ the segment of the sensor, left or right
  PndLmdCalStrip(const PndLmdDigiPara *digipara, SensorSegment segment = kLEFT);
  // for curve strips
  //	PndLmdCalStrip(const PndLmdDigiPara *digipara, SensorSide side = SensorSide::kTOP);

  ~PndLmdCalStrip(){};

  // public method for strips number calculation
  std::vector<PndLmdStrips> GetStrips(Double_t inx, Double_t iny, Double_t inz, Double_t outx, Double_t outy, Double_t outz, Double_t eloss);
  // digi the strips
  std::vector<PndLmdStrips> DigiStripFromPoint(Double_t inx, Double_t iny, Double_t inz, Double_t outx, Double_t outy, Double_t outz, Double_t eloss);

  std::vector<PndLmdStrips> GetCurveStrip(Double_t inx, Double_t iny, Double_t inz, Double_t outx, Double_t outy, Double_t outz, Double_t eLoss);

  // calculate the channels
  Int_t CalChannelFromStrip(Int_t stripNr);

  // calculate the FE number
  Int_t CalFeFromStrip(Int_t stripNr);

  // Smear noise to the signal
  Double_t AddNoise(Double_t charge);

  // charge diffusion during collection
  Double_t ChargeDiffusion(Double_t u, Double_t a, Double_t path, Double_t dir, Double_t Q);

  void print() const;

 private:
  Double_t fPitch;     // the width of strip
  Double_t fOrient;    // the strip orientation
  TVector2 fAnchor;    // anchor point for first strip
  Int_t fNrStrips;     // the number of strips in active area
  Int_t fNrFEChannels; // the number of channels per FE
  Double_t fThreshold;
  Double_t fNoise;
  TVector2 fStripDir;
  TVector2 fOrthoDir;
  Double_t fSigma;

  // calculate the strip number based on the point information
  // the coordinate x, and y in the local coordinate system of the sensor
  // return the strip number with double_t
  Double_t CalStripFromPoint(Double_t x, Double_t y);

  Double_t CalCurveStripFromPoint(Double_t x, Double_t y);

  // the conversion from Eloss to charge.
  Double_t ChargeFromEloss(Double_t eloss) const;

  ClassDef(PndLmdCalStrip, 3);
};

#endif /* PNDLMDCALSTRIP_H_ */
