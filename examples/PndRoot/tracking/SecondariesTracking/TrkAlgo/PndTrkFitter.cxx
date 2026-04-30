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

////////////////////////////////////////////////////////////
//
// PndTrkFitter
//
// Class to perform the Least Square analytical
// Fit with a straight line
//
// authors: Lia Lavezzi - INFN Pavia (2013)
//
////////////////////////////////////////////////////////////

#include "PndTrkFitter.h"

#include "TMatrixT.h"
#include "TMatrixD.h"

#include <iostream>

using namespace std;

// -----   Default constructor   -------------------------------------------
PndTrkFitter::PndTrkFitter() : fSx(0), fSy(0), fSxy(0), fSxx(0), fS1(0), fVerbose(0)
{
  fX.clear();
  fY.clear();
  fSigma.clear();
}
// -------------------------------------------------------------------------
PndTrkFitter::PndTrkFitter(int verbose) : fSx(0), fSy(0), fSxy(0), fSxx(0), fS1(0), fVerbose(verbose)
{
  fX.clear();
  fY.clear();
  fSigma.clear();
}
// -----   Destructor   ----------------------------------------------------
PndTrkFitter::~PndTrkFitter() {}

void PndTrkFitter::Reset()
{
  fSx = 0;
  fSy = 0;
  fSxy = 0;
  fSxx = 0;
  fSxxy = 0;
  fSxxx = 0;
  fSxxxx = 0;
  fS1 = 0;
  fX.clear();
  fY.clear();
  fSigma.clear();
}

void PndTrkFitter::SetPointToFit(double x, double y, double sigma)
{
  fX.push_back(x);
  fY.push_back(y);
  fSigma.push_back(sigma);
}

Bool_t PndTrkFitter::StraightLineFit(Double_t &fitm, Double_t &fitp)
{

  int nofPoints = fX.size();
  if (nofPoints == 0) {
    if (fVerbose > 1)
      cout << "PndTrkFitter::StraightLineFit: no points to fit! fill the array with PndTrkFitter::SetPointToFit()" << endl;
    Reset();
    return kFALSE; // CHECK
  }
  for (int ipnt = 0; ipnt < nofPoints; ipnt++) {
    fSx += fX[ipnt] / (fSigma[ipnt] * fSigma[ipnt]);
    fSy += fY[ipnt] / (fSigma[ipnt] * fSigma[ipnt]);

    fSxy += fX[ipnt] * fY[ipnt] / (fSigma[ipnt] * fSigma[ipnt]);
    fSxx += fX[ipnt] * fX[ipnt] / (fSigma[ipnt] * fSigma[ipnt]);

    fS1 += 1. / (fSigma[ipnt] * fSigma[ipnt]);
  }

  Double_t den = fSxx * fS1 - fSx * fSx;
  if (den == 0) {
    if (fVerbose > 1)
      cout << "PndTrkFitter:StraightLineFit: DEN == 0" << endl; // CHECK
    Reset();
    return kFALSE;
  }

  fitm = (fSxy * fS1 - fSx * fSy) / den;
  fitp = (fSxx * fSy - fSx * fSxy) / den;
  Reset();
  return kTRUE;
}

Double_t PndTrkFitter::StraightLineFitWithChi2(Double_t &fitm, Double_t &fitp)
{
  int nofPoints = fX.size();
  if (nofPoints == 0) {
    if (fVerbose > 1)
      cout << "PndTrkFitter::StraightLineFit: no points to fit! fill the array with PndTrkFitter::SetPointToFit()" << endl;
    Reset();
    return kFALSE; // CHECK
  }
  double Syy = 0;
  for (int ipnt = 0; ipnt < nofPoints; ipnt++) {
    fSx += fX[ipnt] / (fSigma[ipnt] * fSigma[ipnt]);
    fSy += fY[ipnt] / (fSigma[ipnt] * fSigma[ipnt]);

    fSxy += fX[ipnt] * fY[ipnt] / (fSigma[ipnt] * fSigma[ipnt]);
    fSxx += fX[ipnt] * fX[ipnt] / (fSigma[ipnt] * fSigma[ipnt]);

    fS1 += 1. / (fSigma[ipnt] * fSigma[ipnt]);

    Syy += fY[ipnt] * fY[ipnt] / (fSigma[ipnt] * fSigma[ipnt]);
  }

  Double_t den = fSxx * fS1 - fSx * fSx;
  if (den == 0) {
    if (fVerbose > 1)
      cout << "PndTrkFitter:StraightLineFit: DEN == 0" << endl; // CHECK
    Reset();
    return kFALSE;
  }

  fitm = (fSxy * fS1 - fSx * fSy) / den;
  fitp = (fSxx * fSy - fSx * fSxy) / den;

  double chi2 = Syy + fitm * fitm * fSxx + fitp * fitp * fS1 + 2 * fitm * fitp * fSx - 2 * fitm * fSxy - 2 * fitp * fSy;

  Reset();

  return chi2;
}

Bool_t PndTrkFitter::ConstrainedStraightLineFit(Double_t x0, Double_t y0, Double_t &fitm, Double_t &fitp)
{

  // cout << "xoy0 " << x0 << " " << y0 << endl;

  int nofPoints = fX.size();
  if (nofPoints == 0) {
    if (fVerbose > 1)
      cout << "PndTrkFitter::StraightLineFit: no points to fit! fill the array with PndTrkFitter::SetPointToFit()" << endl;
    Reset();
    return kFALSE; // CHECK
  }

  //  cout << "fittinh " << nofPoints << endl;

  for (int ipnt = 0; ipnt < nofPoints; ipnt++) {
    // cout << fX[ipnt] << " " << fY[ipnt] << " " << fSigma[ipnt] << endl;
    fSx += fX[ipnt] / (fSigma[ipnt] * fSigma[ipnt]);
    fSy += fY[ipnt] / (fSigma[ipnt] * fSigma[ipnt]);

    fSxy += fX[ipnt] * fY[ipnt] / (fSigma[ipnt] * fSigma[ipnt]);
    fSxx += fX[ipnt] * fX[ipnt] / (fSigma[ipnt] * fSigma[ipnt]);

    fS1 += 1. / (fSigma[ipnt] * fSigma[ipnt]);
  }

  Double_t den = 2 * x0 * fSx - fSxx - x0 * x0 * fS1;
  if (den == 0) {
    if (fVerbose > 1)
      cout << "PndTrkFitter:StraightLineFit: DEN == 0" << endl; // CHECK
    Reset();
    return kFALSE;
  }

  fitm = (y0 * fSx + x0 * fSy - fSxy - x0 * y0 * fS1) / den;

  fitp = y0 - fitm * x0;

  Reset();
  return kTRUE;
}

// FITTING IN X-Y PLANE:
// v = a + bu + cu^2
Bool_t PndTrkFitter::ParabolaFit(Double_t &fita, Double_t &fitb, Double_t &fitc)
{

  int nofPoints = fX.size();
  if (nofPoints == 0) {
    if (fVerbose > 1)
      cout << "PndTrkFitter::StraightLineFit: no points to fit! fill the array with PndTrkFitter::SetPointToFit()" << endl;
    Reset();
    return kFALSE; // CHECK
  }

  for (int ipnt = 0; ipnt < nofPoints; ipnt++) {

    fSx += fX[ipnt] / (fSigma[ipnt] * fSigma[ipnt]);
    fSy += fY[ipnt] / (fSigma[ipnt] * fSigma[ipnt]);

    fSxy += fX[ipnt] * fY[ipnt] / (fSigma[ipnt] * fSigma[ipnt]);
    fSxx += fX[ipnt] * fX[ipnt] / (fSigma[ipnt] * fSigma[ipnt]);

    fSxxy += fX[ipnt] * fX[ipnt] * fY[ipnt] / (fSigma[ipnt] * fSigma[ipnt]);
    fSxxx += fX[ipnt] * fX[ipnt] * fX[ipnt] / (fSigma[ipnt] * fSigma[ipnt]);

    fSxxx += fX[ipnt] * fX[ipnt] * fX[ipnt] * fX[ipnt] / (fSigma[ipnt] * fSigma[ipnt]);

    fS1 += 1. / (fSigma[ipnt] * fSigma[ipnt]);
  }

  TMatrixD matrix(3, 3);
  matrix[0][0] = fS1;
  matrix[0][1] = fSx;
  matrix[0][2] = fSxx;

  matrix[1][0] = fSx;
  matrix[1][1] = fSxx;
  matrix[1][2] = fSxxx;

  matrix[2][0] = fSxx;
  matrix[2][1] = fSxxx;
  matrix[2][2] = fSxxxx;

  Double_t determ;

  determ = matrix.Determinant();

  if (determ != 0) {
    matrix.Invert();
  } else {
    return 0;
    if (fVerbose == 2)
      cout << "DET 0" << endl;
  }

  TMatrixD column(3, 1);
  column[0][0] = fSy;
  column[1][0] = fSxy;
  column[2][0] = fSxxy;

  TMatrixD column2(3, 1);
  column2.Mult(matrix, column);

  // Double_t a, b, c; //[R.K. 01/2017] unused variable
  fita = column2[0][0];
  fitb = column2[1][0];
  fitc = column2[2][0];

  if (fVerbose == 2) {
    std::cout << "1) parabolic parameters:\n";
    std::cout << "a = " << fita << "\n";
    std::cout << "b = " << fitb << "\n";
    std::cout << "c = " << fitc << "\n";
  }
  if (TMath::Abs(fita) < 0.000001)
    return kFALSE;

  return kTRUE;
}

ClassImp(PndTrkFitter)
