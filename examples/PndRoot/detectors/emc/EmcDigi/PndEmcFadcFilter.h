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
//
// Description:
//      FADC FIR (finite impulse response) filter
//  Author Dima Melnychuk
//  (based on original code by A.Mykulyak)
//-----------------------------------------------------------

#ifndef PNDEMCFadcFilter_HXX
#define PNDEMCFadcFilter_HXX

#include <vector>
#include "TObject.h"

class PndEmcAbsPulseshape;

/**
 * @brief FADC FIR (finite impulse response) filter
 * @ingroup PndEmc
 */
class PndEmcFadcFilter : public TObject {

 public:
  enum type { arbitrary = 0, symmetric = 1, antisymmetric = -1 };

  PndEmcFadcFilter();
  virtual ~PndEmcFadcFilter();

  virtual void Filter(const std::vector<Double_t> &in, std::vector<Double_t> &out) const;
  void SetData(Double_t data[], Int_t i_size, Int_t i_offset);
  Int_t MinIndex() const;
  Int_t MaxIndex() const;
  Int_t Size() const;
  void Convolute(const PndEmcFadcFilter &filt);
  void Convolute(const PndEmcFadcFilter &lhs, const PndEmcFadcFilter &rhs);
  void Clear();

  // Different filters
  // void            SetupIntegrator(Int_t i_width);
  void SetupMA(Int_t i_width);                                                                  // Moving average
  void SetupMWD(Int_t i_width, Double_t tau);                                                   // Moving window deconvolution
  void SetupMatchedFilter(Int_t i_width, PndEmcAbsPulseshape *pulseshape, Double_t sampleRate); // Matched digital filter
  void SetupBipolarTrapez(Int_t i_rise, Int_t i_flat, Int_t i_width);
  // void            SetupTriangle(Int_t i_rise);
  void SetupTrapez(Int_t i_rise, Int_t i_flat);
  void SetupBipolarTriangle(Int_t i_rise);
  void SetupDifferentiator(Int_t i_lag = 0, Int_t i_width = 1);
  void SetupDoubleDifferentiator(Int_t i_npos = 1, Int_t i_nneg = 1, Int_t i_nzero = 0);
  void SetupPZDifferentiator(Int_t i_lag = 0, Double_t d_fac = 1.);
  void SetNormalizeFloating(Double_t d_norm = 1.);
  void SetNormalizeInteger(Int_t i_shift = 0);

  std::vector<Double_t> GetWeights() const { return fCoeff; }

 private:
  std::vector<Double_t> fCoeff;
  Int_t fOffset;
  Int_t fType; // arbitrary, symmetric, antisymmetric
  Bool_t fIntegerize;
  Double_t fNormFactor;
  Int_t fShiftCount;

  ClassDef(PndEmcFadcFilter, 1)
};

#endif
