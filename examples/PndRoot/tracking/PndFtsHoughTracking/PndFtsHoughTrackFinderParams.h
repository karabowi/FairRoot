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
 @class PndFtsHoughTrackFinderParams

 @author Martin J. Galuska <martin [dot] j [dot] galuska (at) physik [dot] uni (minus) giessen [dot] de>

 @brief Parameters for Hough space track finder.
 Created: 09.02.2015
 */

#ifndef PndFtsHoughTrackFinderParams_H
#define PndFtsHoughTrackFinderParams_H

#include <iostream>
#include "TObject.h"

class PndFtsHoughTrackFinderParams : public TObject {
 public:
  // Constructors/Destructors ---------
  PndFtsHoughTrackFinderParams(); // assigns standard values, overwrite lateron
  ~PndFtsHoughTrackFinderParams();

  virtual void Print() const
  {
    std::cout << "fZLineParabola=" << fZLineParabola << " cm fZParabolaLine=" << fZParabolaLine << "cm\n";
    std::cout << "fParabolaStepsPerThetaDeg=" << fParabolaStepsPerThetaDeg << " fParabolaHwScan=" << fParabolaHwScan << " fParabolaNBinsPzxInv=" << fParabolaNBinsPzxInv
              << " fParabolaQDivPzxArgMax=" << fParabolaQDivPzxArgMax << " a.u.\n";
  };

  // getters
  // parabola theta
  Double_t getParabolaStepsPerThetaDeg() const { return fParabolaStepsPerThetaDeg; };
  Double_t getParabolaHwScan() const { return fParabolaHwScan; };
  // parabola 2nd value Q/p_zx
  Int_t getParabolaNBinsPzxInv() const { return fParabolaNBinsPzxInv; };
  Double_t getParabolaQDivPzxArgMax() const { return fParabolaQDivPzxArgMax; };

  Double_t getZLineParabola() const { return fZLineParabola; };
  Double_t getZParabolaLine() const { return fZParabolaLine; };

  Int_t getNBinsY() const { return fUnused; };

  // setters
  void setParabolaParams(Double_t parabolaStepsPerThetaDeg, Double_t parabolaHwScan, Int_t parabolaNBinsPzxInv, Double_t parabolaQDivPzxArgMax)
  {
    fParabolaStepsPerThetaDeg = parabolaStepsPerThetaDeg;
    fParabolaHwScan = parabolaHwScan;

    fParabolaNBinsPzxInv = parabolaNBinsPzxInv;
    fParabolaQDivPzxArgMax = parabolaQDivPzxArgMax;
  }

  void setZPositionsForSwitchingLineParabolaLine(Double_t zLineParabola, Double_t zParabolaLine)
  {
    fZLineParabola = zLineParabola;
    fZParabolaLine = zParabolaLine;
  }

 private:
  // parabola theta
  Double_t fParabolaStepsPerThetaDeg; // how many scan steps should the Hough transform do per degree in theta when searching for the parabola - greater number means finer scanning
                                      // in theta (will multiplied and rounded lateron)
  Double_t fParabolaHwScan;           // how many half widths of the peak for the line before dipole will be scanned around the peak theta value to find the theta for the parabola
  // parabola 2nd value Q/p_zx
  Int_t fParabolaNBinsPzxInv;      // number of bins for 2nd value (Q/p_{zx})
  Double_t fParabolaQDivPzxArgMax; // - for low border, + for high border of Hough space for Q/p_{zx} (in arbitrary units)

  // line parabola line switching (in zx plane) z positions
  Double_t fZLineParabola; // z position where I go from line before dipole to parabola
  Double_t fZParabolaLine; // z position where I go from parabola to line behind dipole

  Int_t fUnused;

 public:
  ClassDef(PndFtsHoughTrackFinderParams, 1);
};

// inline functions

#endif
