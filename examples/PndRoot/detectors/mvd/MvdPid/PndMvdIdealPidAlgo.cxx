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

#include "PndMvdIdealPidAlgo.h"
#include "PndMvdPidCand.h"
// public

void PndMvdIdealPidAlgo::CalcLikelihood(PndMvdPidCand *cand)
{
  // computes pid likelihoods for pi, k, p, mu and stores them in the likelihood map
  // with the map key used as pdg id. This is basically done by integrating the
  // respective energy loss distribution within the chosen selector limits

  double weightP;
  double weightK;
  double weightPi;

  double dE = 0;
  double dx = 0;
  double momentum = 0;

  for (Int_t k = 0; k < cand->GetMvdHits(); k++) {
    dE += cand->GetMvdHitdE(k);
    dx += cand->GetMvdHitdx(k);
    momentum += cand->GetMvdHitMomentum(k);
  }

  if (dx > 0) {
    double energyloss = dE / dx;
    momentum = momentum / cand->GetMvdHits();

    // Proton selector
    if (energyloss >= LowerProtonBoundary(momentum)) {
      weightP = 1 - fPRemainder;
      weightK = 1 - LandauIntegral((LowerProtonBoundary(momentum) - LowerBoundary(momentum, fkMass) - fkShift) / fkScale);
      weightPi = 1 - LandauIntegral((LowerProtonBoundary(momentum) - LowerBoundary(momentum, fpiMass) - fpiShift) / fpiScale);

      // Kaon selector
    } else if (energyloss >= LowerKaonBoundary(momentum)) {
      weightP = fPRemainder;
      weightK = LandauIntegral((LowerProtonBoundary(momentum) - LowerBoundary(momentum, fkMass) - fkShift) / fkScale) - fKRemainder;
      weightPi = LandauIntegral((LowerProtonBoundary(momentum) - LowerBoundary(momentum, fpiMass) - fpiShift) / fpiScale) -
                 LandauIntegral((LowerKaonBoundary(momentum) - LowerBoundary(momentum, fpiMass) - fpiShift) / fpiScale);

      // Pion selector
    } else if (energyloss >= LowerPionBoundary(momentum)) {
      weightP = 0;
      weightK = fKRemainder;
      weightPi = LandauIntegral((LowerKaonBoundary(momentum) - LowerBoundary(momentum, fpiMass) - fpiShift) / fpiScale) - fPiRemainder;

      // Electron selector
    } else {
      weightP = 0;
      weightK = 0;
      weightPi = fPiRemainder;
    }
  } else {
    weightP = 1;
    weightK = 1;
    weightPi = 1;
  }

  // Muons have approximately the same distribution as pions.
  double sum = weightP + weightK + 2 * weightPi;
  weightP /= sum;
  weightK /= sum;
  weightPi /= sum;

  cand->SetLikelihood(211, weightPi);
  cand->SetLikelihood(2212, weightP);
  cand->SetLikelihood(321, weightK);
  cand->SetLikelihood(13, weightPi);
}

// private

double PndMvdIdealPidAlgo::LowerProtonBoundary(double p)
{
  return LowerBoundary(p + 0.02, fpMass) - 5e-4;
};

double PndMvdIdealPidAlgo::LowerKaonBoundary(double p)
{
  return LowerBoundary(p + 0.02, fkMass) - 3e-4;
};

double PndMvdIdealPidAlgo::LowerPionBoundary(double p)
{
  return LowerBoundary(p + 0.01, fpiMass) - 3e-4;
};

double PndMvdIdealPidAlgo::LowerBoundary(double p, double m)
{
  // Calculate the lower boundary of the energy loss distribution.

  double sqrfBeta = 1 / (1 + pow(m / p, 2));
  return 4.9312e-05 * (log(2 * feMass * fc * fc / feb * sqrfBeta / (1 - sqrfBeta)) - sqrfBeta) / sqrfBeta;
};

double PndMvdIdealPidAlgo::LandauIntegral(double x)
{
  // compute integral approximation of a Landau-p.d.f from -INF to x
  // with expected value 0 and deviation 1. Hard coded numbers have
  // been fitted within their respective intervals.

  if (x < -0.8)
    // gauss
    return 0.199785 * exp(-pow(x + 0.149198, 2) * 0.769779);
  else if (x < 0.5)
    // linear
    return 0.177214 * (x + 1.61437);
  else
    // power function
    return 1 - 19.0054 * pow(x + 5.860003, -1.84611);
}

//[GeV/c**2]
float PndMvdIdealPidAlgo::fpiMass = 0.1396;
float PndMvdIdealPidAlgo::fkMass = 0.4937;
float PndMvdIdealPidAlgo::fpMass = 0.9383;
float PndMvdIdealPidAlgo::feMass = 0.511e-3;
//[GeV]
float PndMvdIdealPidAlgo::feb = 0.14e-6;
//[m/s]
float PndMvdIdealPidAlgo::fc = 2.99792458e8;

// Landau distribution parameters; these parameters have been
// obtained by fitting the Landau-p.d.f on the energy loss shifted
// by the Bethe-Bloch energy loss at momentum >= 0.4 GeV
double PndMvdIdealPidAlgo::fkShift = 3.38598e-4;
double PndMvdIdealPidAlgo::fkScale = 1.36362e-4;

double PndMvdIdealPidAlgo::fpiShift = 3.09159e-4;
double PndMvdIdealPidAlgo::fpiScale = 1.58696e-4;

// General remainders of integrals for integrating from -INF to
// below the Bethe-Bloch lower boundary as obtained by MC Simulation
double PndMvdIdealPidAlgo::fPRemainder = 0.015;
double PndMvdIdealPidAlgo::fKRemainder = 0.01;
double PndMvdIdealPidAlgo::fPiRemainder = 0.001;
