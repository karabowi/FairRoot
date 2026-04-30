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

#ifndef PNDMVDCALCTOT_H
#define PNDMVDCALCTOT_H

#include "FairLogger.h"
#include "TObject.h"
#include "TString.h"
#include "TRandom.h"
#include <iostream>
#include <iomanip>

//! Class to calculate the time over threshold
/** @author D.-L.Pohl <d.pohl@fz-juelich.de>
 *
 * This class calculates the time over threshold by using the charge
 * of a pixel/strip. To calculate the TOT a simple capacitor model (integrator) is used.
 * The rising time tr [ns] is assumed ti be constant. This leads to a rising straight line.
 * A constant current unloads the capacitor with a ratio of: a [e/ns].
 * The distance between the points of intersection between the resulting triangle
 * and the threshold line is the TOT.
 *
 * q
 * |
 * |     /\     a
 * |    /     \
 * |---/----------\--------------- threshold
 * |  /               \
 * ------------------------------- q = 0 (charge)
 *    | tr|                    t[ns]
 *      <-  TOT ->
 */

class PndMvdCalcTot {
 public:
  /// Default constructor
  PndMvdCalcTot()
  {
    ftr = 100.;
    fa = 60.;
    fthreshold = 3000;
    fclockfrequency = 50.;
    ftimestep = 1. / fclockfrequency * 1000.;
    SetStartOffset();
    LOG(info) << " <PndMvdCalcTot::PndMvdCalcTot()>: default constructor used!";
  };

  /// Main constructor
  PndMvdCalcTot(Double_t tr, Double_t a, Double_t threshold, Double_t clockfrequency)
  {
    ftr = tr;
    fa = a;
    fthreshold = threshold;
    fclockfrequency = clockfrequency;
    ftimestep = 1. / fclockfrequency * 1000.;
    SetStartOffset();
  };

  /// Destructor
  ~PndMvdCalcTot(){};

  void SetStartOffset();

  Double_t GetTot(Double_t fcharge);
  void SetParameter(Double_t tr, Double_t a, Double_t threshold);
  Double_t GetPileUpTime(Double_t fcharge); // returns the time the capacitor is loaded and therefore the time the hit MVD is blind for other events

 private:
  Double_t fthreshold;      // in electrons
  Double_t ftr;             // raising time [ns]
  Double_t fa;              // falling ration [e/ns]
  Double_t Q, Qt;           // max charge, threshold charge
  Double_t t1e, t2e;        // exact time point when signal is over threshold/under threshold again
  Double_t fclockfrequency; // the frequency the readout chip works with
  Double_t ftimestep;       // one time step of the clock
  Double_t fstarttime;      // absolute point when TOT begins
  Double_t fstoptime;       // absolute point when TOT ends
  Double_t ftimeoffset;     // difference between rising clock edge and when detector gets hit

  Double_t GetTotWC(); // assumes a clock which results in a quantized TOT

};

#endif /*MVDCALCTOT_H*/
