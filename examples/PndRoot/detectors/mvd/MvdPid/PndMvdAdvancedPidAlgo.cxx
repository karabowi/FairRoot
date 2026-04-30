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

#include "TMath.h"
#include "TRandom.h"
#include "PndMvdAdvancedPidAlgo.h"
#include "PndMvdPidCand.h"

// test

ClassImp(PndMvdAdvancedPidAlgo)

  // public

  void PndMvdAdvancedPidAlgo::CalcLikelihood(PndMvdPidCand *cand)
{
  // computes pid likelihoods for pi, k, p, mu, e and stores them in the likelihood
  // map with the map key used as pdg id. This is basically done by integrating the
  // respective energy loss distribution within the chosen selector limits

  double weight[kNPidType];
  double dE = 0;
  double dx = 0;
  fmomentum = 0;

  for (int k = 0; k < cand->GetMvdHits(); k++) {
    dE += cand->GetMvdHitdE(k);
    dx += cand->GetMvdHitdx(k);
    fmomentum += cand->GetMvdHitMomentum(k);
  }

  if (dx > 0) {
    fmomentum /= cand->GetMvdHits();
    fenergyloss = dE / dx;
  } else
    fmomentum = -1; // whateva

  CalcLikelihood(weight);

  cand->SetLikelihood(2212, weight[kProton]);
  cand->SetLikelihood(211, weight[kPion]);
  cand->SetLikelihood(321, weight[kKaon]);
  cand->SetLikelihood(13, weight[kMuon]);
  cand->SetLikelihood(11, weight[kElectron]);
}

void PndMvdAdvancedPidAlgo::CalcLikelihood(PidType part, double momentum, PndMvdPidCand *cand)
{
  double weight[kNPidType];
  CalcLikelihood(part, momentum, weight);
  cand->SetLikelihood(2212, weight[kProton]);
  cand->SetLikelihood(211, weight[kPion]);
  cand->SetLikelihood(321, weight[kKaon]);
  cand->SetLikelihood(13, weight[kMuon]);
  cand->SetLikelihood(11, weight[kElectron]);
}

// the following is a workaround interface for fastsim
void PndMvdAdvancedPidAlgo::CalcLikelihood(PidType part, double momentum, double *lh)
{
  fmomentum = momentum;
  // Build random energy loss
  fenergyloss = MeanEnergyLoss(part) + mpv(part) + gRandom->Landau(0, width1(part)) + gRandom->Gaus(0, width2(part));
  CalcLikelihood(lh);
}

// same as above plus a pdtid switch
void PndMvdAdvancedPidAlgo::CalcLikelihood(int lundId, double momentum, double *lh)
{
  switch (lundId) {
  case 2212: CalcLikelihood(kProton, momentum, lh); break;
  case 321: CalcLikelihood(kKaon, momentum, lh); break;
  case 211: CalcLikelihood(kPion, momentum, lh); break;
  case 13: CalcLikelihood(kMuon, momentum, lh); break;
  case 11: CalcLikelihood(kElectron, momentum, lh); break;
  }
}

// private

void PndMvdAdvancedPidAlgo::CalcLikelihood(double *lh)
{
  if (fmomentum >= 0 && fmomentum <= 2.5) {
    for (PidType part = kElectron; part <= kProton; part = (PidType)(part + 1))
      lh[part] = LandauGaus(fenergyloss - MeanEnergyLoss(part) - mpv(part), width1(part), width2(part));
  } else
    for (int part = kElectron; part <= kProton; part++)
      lh[part] = 1;
  double sum = 0;
  for (int part = kElectron; part <= kProton; part++)
    sum += lh[part];
  for (int part = kElectron; part <= kProton; part++)
    lh[part] /= sum;
}

double PndMvdAdvancedPidAlgo::MeanEnergyLoss(PidType part)
{
  // Calculate the lower boundary of the energy loss distribution.

  //[GeV]
  static float eb = 0.14e-6;
  //[m/s]
  static float c = 2.99792458e8;
  //[GeV/c**2]
  static float Mass[kNPidType] = {0.511e-3, 0.1058, 0.1396, 0.4937, 0.9383};

  double sqrBeta = 1 / (1 + pow(Mass[part] / fmomentum, 2));
  return 4.9312e-05 * (log(2 * Mass[kElectron] * c * c / eb * sqrBeta / (1 - sqrBeta)) - sqrBeta) / sqrBeta;
};

double PndMvdAdvancedPidAlgo::LandauGaus(double s_mpv, double width1, double width2)
{
  // this is the adapted TF1::Integral function from
  // ROOT 5.14. GOTOs have been removed and interval
  // division has been modified to maximize performance

  if (width1 <= 0)
    return TMath::Gaus(s_mpv, 0, width2, true);
  else if (width2 <= 0)
    return TMath::Landau(s_mpv, 0, width1, true);
  else {
    static double x[12] = {0.96028985649753623, 0.79666647741362674, 0.52553240991632899, 0.18343464249564980, 0.98940093499164993, 0.94457502307323258,
                           0.86563120238783174, 0.75540440835500303, 0.61787624440264375, 0.45801677765722739, 0.28160355077925891, 0.09501250983763744};

    static double w[12] = {0.10122853629037626, 0.22238103445337447, 0.31370664587788729, 0.36268378337836198, 0.02715245941175409, 0.06225352393864789,
                           0.09515851168249278, 0.12462897125553387, 0.14959598881657673, 0.16915651939500254, 0.18260341504492359, 0.18945061045506850};

    double h, bb, aa, c1, c2, u, s8, s16, f1, f2;
    double xx;
    bool redo = true;
    int i;

    h = 0;
    aa = -5.0 * width2;
    bb = -2.5 * width2;

    do {
      c1 = 0.5 * (bb + aa);
      c2 = 0.5 * (bb - aa);

      s8 = 0;
      for (i = 0; i < 4; i++) {
        u = c2 * x[i];
        xx = c1 + u;
        f1 = TMath::Landau(s_mpv + xx, 0, width1, true) * TMath::Gaus(xx, 0, width2, true);
        xx = c1 - u;
        f2 = TMath::Landau(s_mpv + xx, 0, width1, true) * TMath::Gaus(xx, 0, width2, true);
        s8 += w[i] * (f1 + f2);
      }
      s16 = 0;
      for (i = 4; i < 12; i++) {
        u = c2 * x[i];
        xx = c1 + u;
        f1 = TMath::Landau(s_mpv + xx, 0, width1, true) * TMath::Gaus(xx, 0, width2, true);
        xx = c1 - u;
        f2 = TMath::Landau(s_mpv + xx, 0, width1, true) * TMath::Gaus(xx, 0, width2, true);
        s16 += w[i] * (f1 + f2);
      }
      s16 = c2 * s16;
      if (TMath::Abs(s16 - c2 * s8) <= 1e-12 * (s16 + 1)) {
        aa = bb;
        bb += 2 * c2 * 1.5;
        if (bb >= 5 * width2) {
          bb = 5 * width2;
          redo = false;
        }
        h += s16;
      } else
        bb = c1;
    } while (redo);
    return h;
  }
}

double PndMvdAdvancedPidAlgo::mpv(PidType part)
{
  double x = fmomentum;
  double x0 = 0;
  double x1 = 0;
  double c0 = 0;
  double c1 = 0;
  double d1 = 0;
  double a3 = 0;
  double a4 = 0;
  double a5 = 0;

  switch (part) {
  case kProton:
    x0 = 0.45;
    x1 = 1.3;
    c0 = 0.383451e-03;
    c1 = -0.126986e-03;
    d1 = -5.21351e-06;
    a3 = -3.05821;
    a4 = 24.6356;
    a5 = -68.632;
    break;
  case kKaon:
    x0 = 0.25;
    x1 = 1.05;
    c0 = 0.326259e-03;
    c1 = -7.68052e-05;
    d1 = 1.51033e-05;
    a3 = -19.6615;
    a4 = 264.382;
    a5 = -1238.09;
    break;
  case kPion:
    x0 = 0.1;
    x1 = 1.0;
    c0 = 0.274692e-03;
    c1 = 3.2571e-05;
    d1 = 9.16527e-06;
    a5 = -6624.05;
    break;
  case kMuon:
    x0 = 0.15;
    x1 = 1.15;
    a3 = 4.33244;
    a4 = -107.686;
    a5 = 699.522;
    c0 = 0.248749e-03;
    c1 = 6.57118e-05;
    d1 = -4.09447e-06;
    break;
  case kElectron:
    x1 = 1.20;
    c0 = 2.93999e-03;
    c1 = 1.76792e-05;
    break;
  }
  if (x >= x1)
    return c0 + c1 * (x1 - x0) + d1 * (x - x1);
  if (x >= x0)
    return c0 + c1 * (x - x0);
  else
    return c0 + c1 * (x - x0) + pow(x0 - x, 3) * (a3 + (x0 - x) * (a4 + (x0 - x) * a5));
}

double PndMvdAdvancedPidAlgo::width1(PidType part)
{
  double x = fmomentum;
  switch (part) {
  case kProton:
    if (x >= 1.10)
      return +3.81174e-04 + x * (-2.25108e-04 + x * +5.45154e-05);
    else
      return -5.28145e-05 + x * (+8.29883e-04 + x * -5.35972e-04);
    break;
  case kKaon:
    if (x >= 1.05)
      return +2.61134e-04 + x * (-1.30818e-04 + x * +3.44165e-05);
    else
      return +3.41858e-04 + x * (-3.21115e-04 + x * +1.37459e-04);
    break;
  case kPion:
    if (x >= 1.00)
      return +1.88718e-04 + x * (-6.38948e-05 + x * +1.78590e-05);
    else
      return +1.82872e-04 + x * (-1.28373e-04 + x * +8.01459e-05);
    break;
  case kMuon:
    if (x >= 1.20)
      return +1.06142e-04 + x * (+3.68777e-05 + x * -1.00190e-05);
    else
      return +1.89374e-04 + x * (-1.46441e-04 + x * +9.10813e-05);
    break;
  case kElectron:
    if (x > 1.2)
      x = 1.2;
    // electrons are constant for momentum > 1.2GeV
    return +1.27955e-04 + x * (-3.15732e-06 + x * +9.64736e-06);
    break;
  }
  // TODO: what to return?
  return -999.;
}

double PndMvdAdvancedPidAlgo::width2(PidType part)
{
  double x = fmomentum;
  switch (part) {
  case kProton:
    if (x >= 1.10)
      return +6.41067e-04 + x * (-3.82507e-04 + x * +9.03732e-05);
    else
      return +6.40328e-04 - 3.21725e-04 * x + 3.17708e-05 * pow(x, -3);
    break;
  case kKaon:
    if (x >= 1.05)
      return +2.22504e-04 + x * (-6.40051e-06 + x * +2.14434e-06);
    else
      return +3.86684e-04 - 1.61873e-04 * x + 7.76586e-06 * pow(x, -3);
    break;
  case kPion:
    if (x >= 1.00)
      return +1.32999e-04 + x * (+1.19714e-04 + x * -3.53302e-05);
    else
      return +2.21603e-04 - 3.21357e-06 * x + 4.64793e-06 * pow(x, -2);
    break;
  case kMuon:
    if (x >= 1.20)
      return +7.84582e-05 + x * (+1.88988e-04 + x * -5.49637e-05);
    else
      return +1.67388e-04 + 5.67991e-05 * x + 3.42702e-06 * pow(x, -2);
    break;
  case kElectron:
    if (x > 1.2)
      x = 1.2;
    // electrons are constant for momentum > 1.2GeV
    return +4.08849e-04 - 3.56548e-05 * x + 1.84825e-08 * pow(x, -3);
    break;
  }
  // TODO: what to return?
  return -999.;
}

double PndMvdAdvancedPidAlgo::fmomentum;
double PndMvdAdvancedPidAlgo::fenergyloss;
