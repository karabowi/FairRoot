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

#include "PndMvdSimplePidAlgo.h"
#include "PndMvdPidCand.h"
// public

void PndMvdSimplePidAlgo::CalcLikelihood(PndMvdPidCand *cand)
{
  // computes pid likelihoods for pi, k, p, mu and stores them in the likelihood map
  // with the map key used as pdg id. This is basically done by integrating the
  // respective energy loss distribution within the chosen selector limits

  double weightP;
  double weightK;
  double weightPi;
  double weightE;

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
      if (momentum < 0.4) {
        weightP = 0.92;
        weightK = 0.008;
      } else {
        weightP = 0.97;
        weightK = 1 - LandauIntegral((LowerProtonBoundary(momentum) - LowerBoundary(momentum, fkMass) - fkShift) / fkScale);
      }
      weightPi = 1 - LandauIntegral((LowerProtonBoundary(momentum) - LowerBoundary(momentum, fpiMass) - fpiShift) / fpiScale);
      weightE = weightPi;

      // Kaon selector
    } else if (energyloss >= LowerKaonBoundary(momentum)) {
      if (momentum < 0.4) {
        weightP = 0.06;
        weightK = 0.98;
      } else {
        weightP = 0.03;
        weightK = LandauIntegral((LowerProtonBoundary(momentum) - LowerBoundary(momentum, fkMass) - fkShift) / fkScale) - 0.01;
      }
      weightPi = LandauIntegral((LowerProtonBoundary(momentum) - LowerBoundary(momentum, fpiMass) - fpiShift) / fpiScale) -
                 LandauIntegral((LowerKaonBoundary(momentum) - LowerBoundary(momentum, fpiMass) - fpiShift) / fpiScale);
      weightE = LandauIntegral((LowerProtonBoundary(momentum) - feShift) / feScale) - LandauIntegral((LowerKaonBoundary(momentum) - feShift) / feScale);

      // Pion selector
    } else if (energyloss >= LowerMuonBoundary(momentum)) {
      weightK = 0.005;
      weightP = 0.002;
      weightPi = LandauIntegral((LowerKaonBoundary(momentum) - LowerBoundary(momentum, fpiMass) - fpiShift) / fpiScale) - 0.001;
      weightE = LandauIntegral((LowerKaonBoundary(momentum) - feShift) / feScale) - LandauIntegral((LowerMuonBoundary(momentum) - feShift) / feScale);

      // Electron selector
    } else {
      weightP = 0;
      weightPi = 0.001;
      weightK = 0;
      weightE = LandauIntegral((LowerMuonBoundary(momentum) - feShift) / feScale) + 0.004;
    }

  } else {
    weightP = 1;
    weightK = 1;
    weightPi = 1;
    weightE = 1;
  }

  // Muons have approximately the same distribution as pions.
  double sum = weightP + weightK + 2 * weightPi + weightE;
  weightP /= sum;
  weightK /= sum;
  weightPi /= sum;
  weightE /= sum;

  cand->SetLikelihood(211, weightPi);
  cand->SetLikelihood(2212, weightP);
  cand->SetLikelihood(321, weightK);
  cand->SetLikelihood(13, weightPi);
  cand->SetLikelihood(11, weightE);
}

// private

double PndMvdSimplePidAlgo::LowerProtonBoundary(double p)
{
  return LowerBoundary(p + 0.02, fpMass) - 5e-4;
};

double PndMvdSimplePidAlgo::LowerKaonBoundary(double p)
{
  return LowerBoundary(p + 0.02, fkMass) - 3e-4;
};

double PndMvdSimplePidAlgo::LowerMuonBoundary(double p)
{
  return LowerBoundary(p + 0.01, fpiMass) - 3e-4;
};

double PndMvdSimplePidAlgo::LowerBoundary(double p, double m)
{
  // Calculate the lower boundary of the energy loss distribution.

  double sqrfBeta = 1 / (1 + pow(m / p, 2));
  return 4.9312e-05 * (log(2 * feMass * fc * fc / feb * sqrfBeta / (1 - sqrfBeta)) - sqrfBeta) / sqrfBeta;
};

double PndMvdSimplePidAlgo::LandauIntegral(double x)
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
float PndMvdSimplePidAlgo::fpiMass = 0.1396;
float PndMvdSimplePidAlgo::fmuMass = 0.1058;
float PndMvdSimplePidAlgo::fkMass = 0.4937;
float PndMvdSimplePidAlgo::fpMass = 0.9383;
float PndMvdSimplePidAlgo::feMass = 0.511e-3;
//[GeV]
float PndMvdSimplePidAlgo::feb = 0.14e-6;
//[m/s]
float PndMvdSimplePidAlgo::fc = 2.99792458e8;

// Landau distribution parameters; these parameters have been
// obtained by fitting the Landau-p.d.f on the energy loss shifted
// by the Bethe-Bloch energy loss at momentum >= 0.4 GeV
double PndMvdSimplePidAlgo::fkShift = 3.38598e-4;
double PndMvdSimplePidAlgo::fkScale = 1.36362e-4;

double PndMvdSimplePidAlgo::fpiShift = 3.09159e-4;
double PndMvdSimplePidAlgo::fpiScale = 1.58696e-4;

double PndMvdSimplePidAlgo::feShift = 2.96566e-3;
double PndMvdSimplePidAlgo::feScale = 1.88056e-4;
