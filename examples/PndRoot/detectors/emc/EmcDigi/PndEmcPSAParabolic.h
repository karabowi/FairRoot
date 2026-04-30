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

//-----------------------------------------------------------
// Description:
//      Pulseshape analysis for ADC waveforms
//      Amplitude and time are determined from the simple parabolic fit
//      by three points (maximum bin and its left and right neigbours)
//
//-----------------------------------------------------------

#ifndef EMCPSAPARABOLIC_HH
#define EMCPSAPARABOLIC_HH

// Base Class Headers ----------------
#include <vector>
#include "PndEmcAbsPSA.h"
#include "TObject.h"

class PndEmcWaveform;

/**
 * @brief Pulseshape analysis for ADC waveforms
 *
 * Amplitude and time are determined from the simple parabolic fit
 * by three points (maximum bin and its left and right neigbours)
 * @ingroup PndEmc
 */
class PndEmcPSAParabolic : public PndEmcAbsPSA {
 public:
  // Constructors/Destructors ---------
  PndEmcPSAParabolic() {}
  virtual ~PndEmcPSAParabolic() { ; }

  virtual void Process(const PndEmcWaveform *waveform, Double_t &amplitude, Double_t &time);

  virtual Int_t Process(const PndEmcWaveform *waveform);
  virtual void GetHit(Int_t i, Double_t &energy, Double_t &time);

  void FitPeak(const std::vector<Double_t> &signal, Double_t &ampl, Double_t &pos, Int_t peakBin) const;
  void FitPeak(const std::vector<Double_t> &signal, Double_t &ampl, Double_t &pos, Int_t start, Int_t end) const;
  void FitPeak(const std::vector<Double_t> &signal, Double_t &ampl, Double_t &pos) const;

 private:
  Double_t lAmplitude;
  Double_t lTime;
};

#endif
