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
//
//  1) Parameters of the filter can be set directly with SetData() method
//  one using one of the predefined filters (trapezoidal - SetupTrapez(),
//  triangle - SetupTriangle(), bipolar triangle - SetupBipolarTriangle() etc.)
//  2) Two cosiquently applied filters can be convoluted with Convolute() method
//  3) Filtration is performed with Filter() method
//  4) Example of Moving Average Filter (average by 3 points)
// 	PndEmcFadcFilter *flt1  = new PndEmcFadcFilter();
//	   Double_t data[3]={1./3.,1./3.,1./3.};
//	   flt1->SetData(data,3,0);
//
//  Author Dima Melnychuk
//  (based on original code by A.Mykulyak)
//-----------------------------------------------------------

#include <math.h>
#include <algorithm>
#include <iostream>

#include "PndEmcFadcFilter.h"
#include "PndEmcAbsPulseshape.h"

using namespace std;

PndEmcFadcFilter::PndEmcFadcFilter() : fCoeff(), fOffset(0), fType(arbitrary), fIntegerize(kFALSE), fNormFactor(1.), fShiftCount(0)
{
  fCoeff.clear();
}

PndEmcFadcFilter::~PndEmcFadcFilter() {}

// Perform filtration of the input signal
void PndEmcFadcFilter::Filter(const std::vector<Double_t> &in, std::vector<Double_t> &out) const
{
  Int_t i_size = in.size();
  if (i_size == 0) {
    out.clear();
    return;
  }

  Int_t i_nlpad = -MinIndex();
  Int_t i_nrpad = MaxIndex();
  Int_t i_ntab = fCoeff.size();
  std::vector<Double_t> temp(i_size + i_nlpad + i_nrpad);

  Double_t *p_data = &temp[0];
  for (Int_t i = 0; i < i_nlpad; i++)
    *p_data++ = in[0];
  for (Int_t i = 0; i < i_size; i++)
    *p_data++ = in[i];
  for (Int_t i = 0; i < i_nrpad; i++)
    *p_data++ = in[i_size - 1];

  out.resize(i_size);

  p_data = &temp[0];
  for (Int_t i = 0; i < i_size; i++) {
    const Double_t *p_d = p_data;
    const Double_t *p_c = &fCoeff[0];
    Double_t d_sum = 0.;

    for (Int_t j = 0; j < i_ntab; j++)
      d_sum += *p_c++ * *p_d++;
    p_data++;

    d_sum *= fNormFactor;
    if (fIntegerize)
      d_sum = floor(d_sum);
    out[i] = d_sum;
  }

  return;
}

// Convolute this filter with a filt .
void PndEmcFadcFilter::Convolute(const PndEmcFadcFilter &filt)
{
  PndEmcFadcFilter tmp(*this);
  Convolute(tmp, filt);
  return;
}

// Convolute f1 with f2
void PndEmcFadcFilter::Convolute(const PndEmcFadcFilter &lhs, const PndEmcFadcFilter &rhs)
{
  Int_t i_nlhs = lhs.Size();
  Int_t i_nrhs = rhs.Size();
  Int_t i_size = i_nlhs + i_nrhs - 1;

  Clear();
  fCoeff.resize(i_size);

  for (Int_t i = 0; i < i_size; i++) {
    Double_t f_sum = 0.;
    for (Int_t j = 0; j < i_nlhs; j++) {
      Int_t k = j - i + i_nrhs - 1;
      Double_t f_rhs = (k >= 0 && k < i_nrhs) ? rhs.fCoeff[k] : 0.;
      f_sum += lhs.fCoeff[j] * f_rhs;
    }
    fCoeff[i] = f_sum;
  }

  fType = lhs.fType * rhs.fType;
  if (fType == arbitrary) {
    fOffset = (i_size) / 2; // ???
  } else {
    fOffset = (i_size) / 2;
  }

  fIntegerize = lhs.fIntegerize & rhs.fIntegerize;
  fShiftCount = (fIntegerize) ? lhs.fShiftCount + rhs.fShiftCount : 0;
  fNormFactor = lhs.fNormFactor * rhs.fNormFactor;

  return;
}

// Set parameters of the filter
void PndEmcFadcFilter::SetData(Double_t data[], Int_t i_size, Int_t i_offset)
{
  Clear();
  fCoeff.resize(i_size);
  for (Int_t i = 0; i < i_size; i++)
    fCoeff[i] = data[i];

  if (i_size == 1) {
    fType = symmetric;
  } else {
    Int_t i_nsym = 0;
    Int_t i_nasym = 0;
    Int_t i_size2 = i_size / 2;
    for (Int_t i = 0; i < i_size2; i++) {
      Double_t f_left = data[i];
      Double_t f_right = data[i_size - i - 1];
      if (f_left == f_right)
        i_nsym += 1;
      if (f_left == -f_right)
        i_nasym += 1;
    }
    fType = arbitrary;
    if (i_nsym == i_size2)
      fType = symmetric;
    if (i_nasym == i_size2)
      fType = antisymmetric;
  }

  if (fType != arbitrary || i_offset < 0)
    i_offset = i_size / 2;

  fOffset = i_offset;

  return;
}

// Setup moving average filter
void PndEmcFadcFilter::SetupMA(Int_t i_width)
{
  Int_t i_size = i_width;
  Clear();
  fCoeff.resize(i_size);
  fType = symmetric;

  for (Int_t i = 0; i < i_size; i++) {
    fCoeff[i] = 1. / i_width;
  }

  return;
}

// Setup moving window deconvolution filter
void PndEmcFadcFilter::SetupMWD(Int_t i_width, Double_t tau)
{
  Int_t i_size = i_width;
  Clear();
  fCoeff.resize(i_size);
  fType = arbitrary;

  fCoeff[0] = 1;
  fCoeff[i_size - 1] = -(1 - 1. / tau);

  for (Int_t i = 1; i < i_size - 1; i++) {
    fCoeff[i] = 1. / tau;
  }

  return;
}

// Matched digital filter
void PndEmcFadcFilter::SetupMatchedFilter(Int_t i_width, PndEmcAbsPulseshape *pulseshape, Double_t sampleRate)
{
  Int_t i_size = i_width;
  Clear();
  fCoeff.resize(i_size);
  fType = arbitrary;

  Double_t amplitude = 1.;
  Double_t t;
  Double_t val = 0;

  for (int i = 0; i < i_size; i++) {
    t = i / sampleRate;
    val = pulseshape->value(t, amplitude, 0.);
    fCoeff[i] = val;
  }

  std::reverse(fCoeff.begin(), fCoeff.end());

  return;
}

void PndEmcFadcFilter::SetupBipolarTrapez(Int_t i_rise, Int_t i_flat, Int_t i_width)
{
  Int_t i_size = 2 * i_rise + 2 * i_flat + i_width + 1;
  Clear();

  fCoeff.resize(i_size);
  fType = symmetric;

  for (Int_t i = 0; i < i_rise; i++) {
    fCoeff[i] = i;
    fCoeff[i_size - i - 1] = -i;
  }

  for (Int_t i = i_rise; i < (i_rise + i_flat); i++) {
    fCoeff[i] = i_rise;
    fCoeff[i_flat + i_width + i + 1] = -i_rise;
  }

  for (Int_t i = (i_rise + i_flat); i <= (i_rise + i_flat + i_width); i++) {
    fCoeff[i] = i_rise - 2 * i_rise / i_width * (i - i_rise - i_width);
  }

  return;
}

// Setup filter with trapezoidal weighting function.
void PndEmcFadcFilter::SetupTrapez(Int_t i_rise, Int_t i_flat)
{
  Int_t i_size = 2 * i_rise + i_flat;
  Clear();

  fOffset = (i_size) / 2;
  fCoeff.resize(i_size);
  fType = symmetric;

  for (Int_t i = 0; i < i_size; i++)
    fCoeff[i] = i_rise + 1;
  for (Int_t i = 0; i < i_rise; i++) {
    fCoeff[i] = i + 1;
    fCoeff[i_size - i - 1] = i + 1;
  }

  return;
}

// Setup filter with bipolar triangular weighting function.
void PndEmcFadcFilter::SetupBipolarTriangle(Int_t i_rise)
{
  Int_t i_size = 4 * i_rise + 1;
  Clear();

  fOffset = 2 * i_rise;
  fCoeff.resize(i_size);
  fType = antisymmetric;

  for (Int_t i = 0; i <= i_rise; i++) {
    fCoeff[i] = i;
    fCoeff[fOffset - i] = i;
    fCoeff[fOffset + i] = -i;
    fCoeff[i_size - i - 1] = -i;
  }

  return;
}

// Setup a differentiator
void PndEmcFadcFilter::SetupDifferentiator(Int_t i_lag, Int_t i_width)
{
  Int_t i_size = i_lag + 2 * i_width;
  Clear();

  fOffset = (i_size) / 2;
  fCoeff.resize(i_size);
  fType = antisymmetric;

  for (Int_t i = 0; i < i_size; i++)
    fCoeff[i] = 0;
  for (Int_t i = 0; i < i_width; i++) {
    fCoeff[i] = -1.;
    fCoeff[i_size - i - 1] = 1.;
  }

  return;
}

// Setup a 2nd order differentiator.
void PndEmcFadcFilter::SetupDoubleDifferentiator(Int_t i_npos, Int_t i_nneg, Int_t i_nzero)
{
  Int_t i_size = i_npos + 2 * (i_nzero + i_nneg);
  Clear();

  fOffset = (i_size) / 2;
  fCoeff.resize(i_size);
  fType = symmetric;

  Int_t i_apos = 2 * i_nneg;
  Int_t i_aneg = i_npos;
  Int_t i_div = 1;
  for (Int_t i = 2; i < max(i_apos, i_aneg); i++) {
    if (i_apos % i == 0 && i_aneg % i == 0)
      i_div = i;
  }
  i_apos /= i_div;
  i_aneg /= i_div;

  for (Int_t i = 0; i < i_size; i++)
    fCoeff[i] = i_apos;
  for (Int_t i = 0; i < i_nneg; i++) {
    fCoeff[i] = -i_aneg;
    fCoeff[i_size - i - 1] = -i_aneg;
  }
  for (Int_t i = i_nneg; i < i_nneg + i_nzero; i++) {
    fCoeff[i] = 0.;
    fCoeff[i_size - i - 1] = 0.;
  }

  return;
}

// Setup a differentiator with Pole/Zero cancelation.
void PndEmcFadcFilter::SetupPZDifferentiator(Int_t i_lag, Double_t d_fac)
{
  Int_t i_size = i_lag + 2;
  Clear();

  fOffset = (i_size) / 2;
  fCoeff.resize(i_size);
  fType = arbitrary;

  for (Int_t i = 0; i < i_size; i++)
    fCoeff[i] = 0.;
  fCoeff[0] = -d_fac;
  fCoeff[i_size - 1] = 1.;

  return;
}

// Use floating normalization with factor a d_norm .
void PndEmcFadcFilter::SetNormalizeFloating(Double_t d_norm)
{
  fIntegerize = kFALSE;
  fShiftCount = 0;
  fNormFactor = d_norm;
  return;
}

// Use integer normalization with a shift count of a i_shift .
void PndEmcFadcFilter::SetNormalizeInteger(Int_t i_shift)
{
  fIntegerize = kTRUE;
  fShiftCount = i_shift;
  fNormFactor = 1.;
  for (Int_t i = 0; i < i_shift; i++)
    fNormFactor *= 0.5;
  return;
}

// Returns the lowest index.
Int_t PndEmcFadcFilter::MinIndex() const
{
  return -fOffset;
}

// Returns the highest index.
Int_t PndEmcFadcFilter::MaxIndex() const
{
  return fCoeff.size() - 1 - fOffset;
}

// Returns size of filter (number of tabs).
Int_t PndEmcFadcFilter::Size() const
{
  return fCoeff.size();
}

void PndEmcFadcFilter::Clear()
{
  fCoeff.clear();
  fOffset = 0;
  fType = arbitrary;
  return;
}

ClassImp(PndEmcFadcFilter);
