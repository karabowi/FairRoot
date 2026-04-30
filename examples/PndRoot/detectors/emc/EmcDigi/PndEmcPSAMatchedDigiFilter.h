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
//
//-----------------------------------------------------------

#ifndef EMCPSAMATCHEDDIGIFILTER_HH
#define EMCPSAMATCHEDDIGIFILTER_HH

// Base Class Headers ----------------
#include <vector>
#include "PndEmcAbsPSA.h"
#include "TObject.h"

class PndEmcWaveform;
class PndEmcAbsPulseshape;

/**
 * @brief Pulseshape analysis for ADC waveforms
 * @ingroup PndEmc
 */
class PndEmcPSAMatchedDigiFilter : public PndEmcAbsPSA {
 public:
  // Constructors/Destructors ---------
  PndEmcPSAMatchedDigiFilter(const std::vector<Double_t> params, PndEmcAbsPulseshape *pulseshape);
  virtual ~PndEmcPSAMatchedDigiFilter() { ; }

  virtual Int_t Process(const PndEmcWaveform *waveform);

  virtual void GetHit(Int_t i, Double_t &energy, Double_t &time);

  virtual void Process(const PndEmcWaveform *waveform, Double_t &amplitude, Double_t &time);
  void Draw(const PndEmcWaveform *waveform);

  std::vector<Double_t> fParams; // Parameters of the filter
  PndEmcAbsPulseshape *fPulseshape;

 private:
  Double_t lAmplitude;
  Double_t lTime;
  PndEmcPSAMatchedDigiFilter(const PndEmcPSAMatchedDigiFilter &L);
  PndEmcPSAMatchedDigiFilter &operator=(const PndEmcPSAMatchedDigiFilter &) { return *this; };
};

#endif
