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
 @class PndFtsHoughSpaceBinning

 @author Martin J. Galuska <martin [dot] j [dot] galuska (at) physik [dot] uni (minus) giessen [dot] de>

 @brief Helper class for Hough space containing binning.
 Created: 09.02.2015
 */

#ifndef PndFtsHoughSpaceBinning_H
#define PndFtsHoughSpaceBinning_H

#include <iostream>
#include "TObject.h"
#include <cmath>
#include "TMath.h"
#include <math.h>

class PndFtsHoughSpaceBinning : public TObject {
 public:
  // Constructors/Destructors ---------
  PndFtsHoughSpaceBinning(); // do not use this constructor
  PndFtsHoughSpaceBinning(Int_t nBinsThetaVal,
                          Double_t thetaRadLowVal,  // in rad
                          Double_t thetaRadHighVal, // in rad

                          Int_t nBinsYVal, Double_t yLowVal,
                          Double_t yHighVal); // supports asymmetric boundaries for Hough spaces
  PndFtsHoughSpaceBinning(Int_t stepsPerThetaDeg, Double_t thetaDegLowHighVal,

                          Int_t nBinsYVal,
                          Double_t yLowHighVal); // only symmetric boundaries for Hough spaces
  ~PndFtsHoughSpaceBinning();

  inline void Print() const;

  Int_t getNBinsTheta() const { return fNBinsTheta; };
  Double_t getThetaRadLow() const { return fThetaRadLow; };
  Double_t getThetaRadHigh() const { return fThetaRadHigh; };

  Int_t getNBinsY() const { return fNBinsY; };
  Double_t getYHigh() const { return fYHigh; };
  Double_t getYLow() const { return fYLow; };

 private:
  Int_t fNBinsTheta;
  Double_t fThetaRadLow;
  Double_t fThetaRadHigh;

  Int_t fNBinsY;
  Double_t fYLow;
  Double_t fYHigh;

 public:
  ClassDef(PndFtsHoughSpaceBinning, 1);
};

// inline functions
void PndFtsHoughSpaceBinning::Print() const
{
  std::cout << "x: (" << fNBinsTheta << ", " << fThetaRadLow << ", " << fThetaRadHigh << ") -- y: (" << fNBinsY << ", " << fYLow << ", " << fYHigh << '\n';
}

#endif
