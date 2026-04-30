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

//
// ============================================================================
//
// Straw tube response simulation
// Corrections to StrawCharge and Eject for delta electrons (dec 2007)
//
//  version with one threshold only Julich signal
//
//  mandatory call at the beginning
//   once to set constants
//
//  void TConst(Double_t Radius, Double_t pSTP, Double_t ArP, Double_t CO2P);
//
// Pysical (Garfield) values:
//   Radius  Press (pSTP)  % Ar (ArP)      % CO2 (CO2P)
//     0.4     1           0.9              0.1
//     0.5     1           0.9              0.1
//     0.5     2           0.8              0.2
//
// ------------------------------------------------------------------------------
//
// Full simulation: calls at each track for MC applications
//
//  define particle and its in-out hits
//
//  void PutWireXYZ(Double_t w1, Double_t w2, Double_t w3,
//		  Double_t w4, Double_t w5, Double_t w6);     define  the wire
//
//  Double_t PartToTime(Double_t Mass, Double_t Momentum,
//                                     Double_t InOut[]   );   straw time
//
//
//  Double_t TimnsToDiscm(Double_t time);  from time (ns)
//                                         to the reconstructed distance (cm)
//
//  Double_t PartToADC();  ADC charge
//                        signal corresponding to the energy loss of StrawCharge
//                        for energy loss simulation
//
//
//-----------------------------------------------------------------------------
//
// Fast simulation: calls at each track for MC applications
//
//  define particle and its in-out hits
//
//  void PutWireXYZ(Double_t w1, Double_t w2, Double_t w3,
//		  Double_t w4, Double_t w5, Double_t w6);     define  the wire
//
//  Double_t FastRec(Double_t TrueDcm, Int_t Flag);   fast approximation:
//                                                    from the true distance   (cm)
//                                                    to the reconstructed one (cm)
//                                                    Flag=0 standard
//                                                    Flag=1 Julich expt. curve
//                                                    (active with press =2 in TCo
//
//  void TInit(Double_t xPMass, Double_t xPMom, Double_t InOut[]); constant initialization
//
//  Double_t FastPartToADC();  ADC charge
//                        signal corresponding to the energy loss of StrawCharge
//                        with the Urban model for energy loss simulation
//
// ----------------------------------------------------------------------------
// where:
//
// time: measured drift time in ns
//
// pSTP = pressure in bar
// Radius = tube radius
// ArP and CO2P percentages of Argon and CO2
// w1:w6         = coordinates of the extremes of the straw wire (straw axis)
// Mass, Momentm = mass and momentum of the MC particle
// InOut[0:5]    = straw input and output coordinates traversed from the MC particle
//
//
//
// For other applications see the routines listed in PndFtsSingleStraw.h
//
// Author: A. Rotondi (november 2005, revised and extended in  August 2006)
//
// ================================================================================
//

#include <iostream>
#include <string>
#include "TRandom.h"
#include "TMath.h"
#include "PndFtsSingleStraw.h"
#include "TImage.h"
#include "TVector3.h"
#include "TVectorD.h"

#include "TMatrixD.h"
#include "TMatrixDEigen.h"

using namespace std;

ClassImp(PndFtsSingleStraw);

// ============================================================
PndFtsSingleStraw::PndFtsSingleStraw()
  : CDist(), CDistC(), CNele(), CNeleT(), TeleTime(), AmplSig(), Pulse(), PulseT(), WDist(), Wi(0), ArPerc(0), CO2Perc(0), CH4Perc(0), ArWPerc(0), CO2WPerc(0), CH4WPerc(0),
    pSTP(0), Radius(0), AAr(0), ZAr(0), RhoAr(0), NclAr(0), EmedAr(0), EminAr(0), EmpAr(0), CsiAr(0), IAr(0), WiAr(0), Ncl(0), Ecl(0), Lcl(0), Ntote(0), GasGain(0), Cutoff(0),
    EmedCO2(0), EminCO2(0), EmpCO2(0), CsiCO2(0), ACO2(0), ZCO2(0), RhoCO2(0), ICO2(0), WiCO2(0), NclCO2(0), EmedCH4(0), EmpCH4(0), CsiCH4(0), EminCH4(0), ACH4(0), ZCH4(0),
    RhoCH4(0), ICH4(0), WiCH4(0), NclCH4(0), RhoMixCO2(0), RhoMixCH4(0), PZeta(0), piMass(0), PMass(0), PMom(0), Dx(0), eMass(0), prMass(0), Delta(0), CNumb(0), PEn(0), beta(0),
    gamma(0), Emed(0), Emin(0), Csi(0), Emax(0), Emp(0), NNClus(0), Xin(0), Yin(0), Zin(0), Xout(0), Yout(0), Zout(0), Rpath(0), NPolya(0), Xmax(0), bPolya(0), Calpha(0), Cbeta(0),
    Cgamma(0), NUrban(0), SigUrb(0), Eup(0), AvUrb(0), Wx1(0), Wy1(0), Wz1(0), Wx2(0), Wy2(0), Wz2(0), Wp(0), Wq(0), Wr(0), PulseMax(0), PulseTime(0), Thresh1(0), Thresh2(0),
    Nchann(0), Out1(0), Out2(0), Out3(0)
{
  // class constructor

  // clear
  memset(CumClus, 0, sizeof(CumClus));
  memset(CH4Clus, 0, sizeof(CH4Clus));
  memset(PolyaCum, 0, sizeof(PolyaCum));
  memset(Xs, 0, sizeof(Xs));

  CDist.clear();
  CDistC.clear();
  CNele.clear();
  CNeleT.clear();
  TeleTime.clear();
  AmplSig.clear();
  Pulse.clear();
  PulseT.clear();
  WDist.clear();
}

// ----------------------------------------------------------------------
void PndFtsSingleStraw::TConst(Double_t R1, Double_t P1, Double_t A1, Double_t C1)
{

  // set constants for the simulation
  // Input
  // P1    = tube absolute pressure pSTP
  // R1  = tube radius Radius
  // A1 = argon percentage ArPerc
  // C1 = C=2 percentage C=2Perc
  //

  Radius = R1;
  pSTP = P1;
  // Input for the media (volume percentages)
  ArPerc = A1;
  CO2Perc = C1;

  // cluster dimensions in Ar and CO2 (experimantal values)

  //   Double_t PClus[20] =
  //      {0., .656,   .150,   .064,  .035,  .0225, .0155, .0105, .0081,
  //       .0061,  .0049, .0039, .0030, .0025, .0020, .0016, .0012,
  //       .00095, .00075, .00063};      // Fischle fit

  Double_t PClus[20] = {0., .802, .0707, .020, .013, .008, .006, .005, .006, .008, .009, .007, .0050, .0040, .0033, .0029, .0025, .0023, .0022, .002}; // Lapique 1st calculation

  // Double_t PClus[20] =
  //     {0., .841,   .0340,   .021,  .013,  .008, .006, .004, .003,
  //            .008,  .013, .008, .0050, .004, .0030, .0028, .0025,
  //            .0023, .0022, .002}; // Lapique 2nd calculation

  // Double_t PClus[20] =
  //     {0., .656, .150,   .064,  .035,  .0225, .0155, .0105, .0081,
  //            .0061,  .0049, .0039, .0030, .0025, .0020, .0016, .0012,
  //            .00080, .00059, .00045}; // Fischle empirical

  // PDouble_t Clus[20] =
  //     {0., .656,   .148,   .0649,  .0337,  .0244, .0141, .0078, .0095,
  //         .0063,  .0062, .0042, .0028, .0018, .0023, .0017, .0014,
  // 	   .00060, .00050, .00063};    // Fischle exp

  Double_t CO2Clus[20] = {0., .730, .162, .038, .020, .0110, .0147, .0060, .0084, .0052, .0020, .0042, .0021, .0025, .0038, .0021, .0009, .00013, .00064, .00048}; // Fischle exp

  //    Double_t CH4Clus[20] =
  //     {0., .786, .120, .032,  .013,  .0098, .0055, .0057, .0027,
  //            .0029,  .0020, .0016, .0013, .0010, .0012, .0006, .0005,
  //            .00042, .00037, .00033};  // Fischle exp

  CH4Perc = 0.07;

  // -----------------------------------------------------
  // gain of the avalanche
  // Ar/CO2 90/10 1 bar (NTP) MAGY GARFIELD 20-7-2006
  GasGain = 100000.;

  // argon ----------------------------------------------------
  AAr = 39.948;                 // Argon (39.948)
  ZAr = 18.0;                   // Argon (18)
  RhoAr = pSTP * 1.78 * 1.e-03; // g/cm3  (1.78 mg/cm3)
  IAr = 188 * 1.e-09;           // ionization potential (GeV) (188 eV)
  WiAr = 27.0;                  // energy to create an ion pair  (standard 26.7 eV)
  NclAr = 25.;                  // cluster/cm in Argon
  //  CO2 -----------------------------------------------------
  ACO2 = 44;                     // CO2
  ZCO2 = 22.;                    // CO2
  RhoCO2 = pSTP * 1.98 * 1.e-03; // g/cm3  CO2 (1.98 mg/cm3)
  ICO2 = 95.8 * 1.e-09;          // ionization potential (GeV) (96 eV)
  WiCO2 = 33.0;                  // energy to create an ion pair (33 eV)
  NclCO2 = 35.5;                 // clusters/cm CO2  35.5
  // Methane CH4 ---------------------------------------------------------
  ACH4 = 16;                     // CO2 (39.948)
  ZCH4 = 10.;                    // CO2 (18)
  RhoCH4 = pSTP * 0.71 * 1.e-03; // g/cm3  CO2 (0.71 mg/cm3)
  ICH4 = 40.6 * 1.e-09;          // ionization potential (GeV) (45 eV)
  WiCH4 = 28.0;                  // energy to create an ion pair
  NclCH4 = 25.0;
  // Input for the media (weight percentages) ----------------------------
  ArWPerc = ArPerc * AAr / (ArPerc * AAr + CO2Perc * ACO2);
  CO2WPerc = CO2Perc * ACO2 / (ArPerc * AAr + CO2Perc * ACO2);

  // mixture densiies ----------------------------------------------------
  RhoMixCO2 = 1. / ((ArWPerc / RhoAr) + (CO2WPerc / RhoCO2));
  RhoMixCH4 = 1. / ((ArWPerc / RhoAr) + (CH4WPerc / RhoCH4));

  //----------------------------------------------------------------------
  //  particles  (Gev, energy losses in Kev)

  PZeta = 1;             // projectile charge
  piMass = 0.139;        // particle mass (GeV)
  eMass = 0.511 / 1000.; // electron mass (GeV) (0.511 MeV)
  prMass = 0.93827;      // proton mass (GeV)

  // ---------------------------------------------------------------------
  // thresholds for the straw tubes  (default values) see TInit for current values
  Thresh1 = 10;
  Thresh2 = 30;
  // channels for the signal
  Nchann = 500;

  // ---------------------------------------------Emin------------------------

  NPolya = 100; // steps for the calculation of the Polya distributions
  Xmax = 0.;    // Polya istribution is calculated between o and Xmax (see Polya)
  bPolya = 0.5;
  Polya(bPolya); // cumulative of the Polya distribution
  // -----------------------------------------------------------------------

  // cumulative for the number of electron per cluster

  Double_t Wnorm = (ArPerc * NclAr + CO2Perc * NclCO2);
  CumClus[0] = (ArPerc * NclAr * PClus[0] + CO2Perc * NclCO2 * CO2Clus[0]) / Wnorm;

  for (Int_t i = 1; i < 20; i++) {
    CumClus[i] = (ArPerc * NclAr * PClus[i] + CO2Perc * NclCO2 * CO2Clus[i]) / Wnorm + CumClus[i - 1];
  }
  CumClus[20] = 1.;

  Double_t sum = 0.;
  for (Int_t i = 0; i < 20; i++) {
    sum += PClus[i];
    //    cout<<" PClus["<<i<<"] = "<<PClus[i]<<endl;
  }
  //  for (Int_t i = 0; i <= 20; i++) { // cout<<"CumClus["<<i<<"] = "<<CumClus[i]<<endl;
  //  }
  // cout<<" Sum of Probabilities = "<<sum<<endl;
}

// ==============================================================
void PndFtsSingleStraw::PutWireXYZ(Double_t w1, Double_t w2, Double_t w3, Double_t w4, Double_t w5, Double_t w6)
{
  // get wire coordinates

  Wx1 = w1;
  Wy1 = w2;
  Wz1 = w3;

  Wx2 = w4;
  Wy2 = w5;
  Wz2 = w6;
}
// =============================================================

void PndFtsSingleStraw::TInit(Double_t xPMass, Double_t xPMom, Double_t InOut[])
{

  // initialization of the constants for each track and each straw

  // transfer of data
  // track geometrical quantities
  Xin = InOut[0];
  Yin = InOut[1];
  Zin = InOut[2];
  Xout = InOut[3];
  Yout = InOut[4];
  Zout = InOut[5];
  PMass = xPMass;
  PMom = xPMom;

  // path into the straw
  Dx = sqrt((Xout - Xin) * (Xout - Xin) + (Yout - Yin) * (Yout - Yin) + (Zout - Zin) * (Zout - Zin));

  // clear
  CNumb = 0;
  PulseMax = 0;
  PulseTime = 0;
  CDist.clear();
  CDistC.clear();
  CNele.clear();
  CNeleT.clear();
  WDist.clear();
  TeleTime.clear();
  AmplSig.clear();
  Pulse.clear();
  PulseT.clear();

  // ---------------------------------------------------------------------

  // initialization of the constants
  // maximum energy transfer Emax (GeV) and related quantities

  PEn = sqrt(PMom * PMom + PMass * PMass); // particle energy GeV
  beta = PMom / PEn;
  gamma = 1 / sqrt(1. - beta * beta);
  Double_t begam = beta * gamma;   // local
  Double_t mratio = eMass / PMass; // local

  // calculation of the polarization Sternheimer factor  for Argon
  Double_t ms = 2.80;
  Double_t Xst = log10(begam * sqrt(pSTP));
  Double_t Cs = -11.92;
  Double_t X1 = 4;
  Double_t Xo = 1.96;
  Double_t as = 0.389;
  Delta = 0;
  if (Xo <= Xst && Xst <= X1)
    Delta = 4.6051702 * Xst + Cs + as * pow(X1 - Xst, ms);
  else if (X1 < Xst)
    Delta = 4.6051702 * Xst + Cs;
  if (Delta < 0)
    Delta = 0;

  // calculation of other typical quantites

  Emax = (1.022 * begam * begam / (1. + 2. * gamma * mratio + mratio * mratio)) / 1000.; // GeV

  CsiAr = 0.5 * 0.3071 * PZeta * PZeta * ZAr * RhoAr / (beta * beta * AAr) * 1e-03; // GeV
  Double_t fact = 2. * eMass * beta * beta * gamma * gamma * Emax / (IAr * IAr);
  EmedAr = 2. * CsiAr * (0.5 * TMath::Log(fact) - beta * beta - 0.5 * Delta); // GeV/cm
  EminAr = pSTP * 2.70 * 1.e-06;                                              // GeV/cm  (2.5 keV)
  // most prob energy (GeV)
  EmpAr = EmedAr + CsiAr * (0.422784 + beta * beta + log(CsiAr / Emax));

  CsiCO2 = 0.5 * 0.3071 * PZeta * PZeta * ZCO2 * RhoCO2 / (beta * beta * ACO2) * 1e-03; // GeV
  fact = 2. * eMass * beta * beta * gamma * gamma * Emax / (ICO2 * ICO2);
  EmedCO2 = 2. * CsiCO2 * (0.5 * TMath::Log(fact) - beta * beta - 0.5 * Delta); // GeV/cm
  EminCO2 = pSTP * 3.60 * 1.e-06;                                               // GeV/cm  (2.5 keV)
  // most prob energy (GeV)
  EmpCO2 = EmedCO2 + CsiCO2 * (0.422784 + beta * beta + log(CsiCO2 / Emax));

  Csi = RhoMixCO2 * ((ArWPerc * CsiAr / RhoAr) + (CO2WPerc * CsiCO2 / RhoCO2));
  ;
  Emed = RhoMixCO2 * ((ArWPerc * EmedAr / RhoAr) + (CO2WPerc * EmedCO2 / RhoCO2));
  Emin = RhoMixCO2 * ((ArWPerc * EminAr / RhoAr) + (CO2WPerc * EminCO2 / RhoCO2));
  Emp = RhoMixCO2 * ((ArWPerc * EmpAr / RhoAr) + (CO2WPerc * EmpCO2 / RhoCO2));

  // mean weighted interaction
  Wi = (ArPerc * NclAr * WiAr + CO2Perc * NclCO2 * WiCO2) / (ArPerc * NclAr + CO2Perc * NclCO2);
  // number of clusters
  Double_t nAr = ArWPerc * RhoMixCO2 / AAr;
  Double_t nCO2 = CO2WPerc * RhoMixCO2 / ACO2;
  // Ncl=pSTP*((ArPerc*NclAr)+(CO2Perc*NclCO2))*Emed/Emin; // <--- Cluster/cm
  Ncl = pSTP * ((nAr * NclAr + nCO2 * NclCO2) / (nAr + nCO2)) * Emed / Emin; // <--- Cluster/cm

  Lcl = 1. / Ncl;          // mean free path between clusters (cm)
  Ecl = 2.8;               // mean number of electrons per clusters (def 2.8)
  Cutoff = Ncl * Ecl * 25; // limit to the number of primary electrons (*20)
  Ntote = Ecl * Ncl;       // total mean number of electrons per cm

  // ---------------------------------------------------------------------
  // thresholds for the straw tubes  (current values)
  // total number of electrons * scaling factor (max of signal x electron=1)

  Thresh1 = Ncl * Ecl * 0.10;
  Thresh2 = Ncl * Ecl * 0.15;

  // -----------------------------------------------------------------------

  TDirCos(); // director cosine of the track

  // control prints

  //  cout<<" Dx "<<Dx<<" Csi  "<<Csi*1.e+09<<" Emax MeV "<<
  //    Emax*1.e+03<<" PMass  "<<PMass<<" gamma "<<
  //    gamma<<endl;
  //   cout<<" RRise "<<RRise(gamma)<<endl;
  //   cout<<" Thresh1 = "<<Thresh1<<" Thresh2 = "<<Thresh2<<endl;
}

// =============================================================

// energy loss in GeV
Double_t PndFtsSingleStraw::STEloss()
{

  return gRandom->Landau(Emp * Dx, Csi * Dx); // in GeV
}
// =============================================================

Double_t PndFtsSingleStraw::STUrban()
{

  //
  // energy loss in GeV from the Urbam distribution NIM A 362(1995)416
  // Author: A. Rotondi December 2007
  //

  Double_t Sig1Ar, Sig2Ar, Sig3Ar;
  Double_t Sig1Mix, Sig2Mix, Sig3Mix;
  Double_t Sig1CO2, Sig2CO2, Sig3CO2, IMix, nAr, nCO2;
  Double_t f1, f2, e1, e2, e2f, ru, Cu, Cuc;
  Double_t N1Mix, N2Mix, N3Mix, E1Mix, E2Mix, E3Mix, ZMix;
  Double_t Sig21, Sig22, Sig23, Medd;

  Double_t eVGeV = 1.e-09;

  // ru= 0.6;
  ru = 0.55;

  Cuc = 2. * PMass * beta * beta * gamma * gamma;

  // calculation for Argon

  Cu = EmedAr;
  f2 = 2. / ZAr;
  f1 = 1. - f2;
  e2 = 10. * ZAr * ZAr * eVGeV;
  e2f = TMath::Power(e2, f2);
  e1 = TMath::Power(IAr / e2f, 1. / f1);
  Sig1Ar = (1. - ru) * Cu * (f1 / e1) * (TMath::Log(Cuc / e1) - beta * beta) / (TMath::Log(Cuc / IAr) - beta * beta);
  Sig2Ar = (1. - ru) * Cu * (f2 / e2) * (TMath::Log(Cuc / e2) - beta * beta) / (TMath::Log(Cuc / IAr) - beta * beta);
  Sig3Ar = Cu * Emax * ru / (IAr * (Emax + IAr) * TMath::Log((Emax + IAr) / IAr));

  // Calculation for CO2

  Cu = EmedCO2;
  f2 = 2. / ZCO2;
  f1 = 1. - f2;
  e2 = 10. * ZCO2 * ZCO2 * eVGeV;
  e2f = TMath::Power(e2, f2);
  e1 = TMath::Power(ICO2 / e2f, 1. / f1);

  Sig1CO2 = (1. - ru) * Cu * (f1 / e1) * (TMath::Log(Cuc / e1) - beta * beta) / (TMath::Log(Cuc / ICO2) - beta * beta);
  Sig2CO2 = (1. - ru) * Cu * (f2 / e2) * (TMath::Log(Cuc / e2) - beta * beta) / (TMath::Log(Cuc / ICO2) - beta * beta);
  Sig3CO2 = Cu * Emax * ru / (ICO2 * (Emax + ICO2) * TMath::Log((Emax + ICO2) / ICO2));

  // calculation for the mixture

  nAr = ArWPerc * RhoMixCO2 / AAr;
  nCO2 = CO2WPerc * RhoMixCO2 / ACO2;
  IMix = TMath::Exp((nAr * ZAr * TMath::Log(IAr) + nCO2 * ZCO2 * TMath::Log(ICO2)) / (nAr * ZAr + nCO2 * ZCO2));

  ZMix = ArWPerc * ZAr + CO2WPerc * ZCO2;
  f2 = 2. / ZMix;
  f1 = 1. - f2;
  E2Mix = 10. * ZMix * ZMix * eVGeV;
  e2f = TMath::Power(E2Mix, f2);
  E1Mix = TMath::Power(IMix / e2f, 1. / f1);

  Sig1Mix = Dx * RhoMixCO2 * (ArWPerc * Sig1Ar / RhoAr + CO2WPerc * Sig1CO2 / RhoCO2);
  Sig2Mix = Dx * RhoMixCO2 * (ArWPerc * Sig2Ar / RhoAr + CO2WPerc * Sig2CO2 / RhoCO2);
  Sig3Mix = Dx * RhoMixCO2 * (ArWPerc * Sig3Ar / RhoAr + CO2WPerc * Sig3CO2 / RhoCO2);

  NUrban = Sig1Mix + Sig2Mix + Sig3Mix; // total number of collisions (mean value)

  // Urban distribution calculation

  N1Mix = gRandom->Poisson(Sig1Mix);
  N2Mix = gRandom->Poisson(Sig2Mix);
  N3Mix = gRandom->Poisson(Sig3Mix);

  Int_t N3 = TMath::Nint(N3Mix + gRandom->Uniform());

  E3Mix = 0.;
  for (Int_t j = 1; j <= N3; j++) {
    E3Mix += ((Emax + IMix) * IMix) / (IMix + Emax * (1. - gRandom->Uniform()));
  }

  // variance calculation

  Eup = IMix / (1. - 0.98 * Emax / (Emax + IMix)); // 98% of the delta electron area (GeV)
  Sig21 = IMix * (Emax + IMix) / Emax;
  Medd = Sig21 * TMath::Log(Eup / IMix);
  Sig22 = Sig21 * Eup - Medd * Medd;
  Sig23 = Sig1Mix * E1Mix * E1Mix + Sig2Mix * E2Mix * E2Mix + Sig3Mix * Medd * Medd + Sig3Mix * Sig22;

  AvUrb = Sig1Mix * E1Mix + Sig2Mix * E2Mix + Sig3Mix * Medd;

  SigUrb = TMath::Sqrt(Sig23);

  // return the energy lost in keV

  return N1Mix * E1Mix + N2Mix * E2Mix + E3Mix; // in GeV
}
// =============================================================

Double_t PndFtsSingleStraw::StrawCharge()
{

  // sampling of the ionization energy loss, number of
  // primary electrons and their wire distances for each track
  // energy loss in GeV

  // clear
  Double_t TotEnEle = 0., ClusEner = 0., Ecurr;
  CNeleT.clear();
  WDist.clear();

  // threshold for delta rays (>1.5 KeV from NIM A301(1991)202)
  // total energy released by the electrons
  // Number of cluster in the length
  NNClus = Cluster(); // set vectors CDist CDistC e CNele
  // calculation of total energy released by the electrons
  for (Int_t j = 1; j <= NNClus; j++) {
    ClusEner = 0.;
    for (Int_t jj = 1; jj <= (Int_t)(CNele.at(j - 1)); jj++) {
      Ecurr = Wi;
      TotEnEle += Ecurr;
      ClusEner += Ecurr;
    }

    // effective number of electrons per cluster

    CNeleT.push_back(int(ClusEner / Wi));
  }

  // record the distance from the wire of any electron
  // adding the diffusion effects
  Int_t owfl = 0;
  for (Int_t k = 1; k <= NNClus; k++) {
    for (Int_t kk = 0; kk < (Int_t)CNeleT.at(k - 1); kk++) {
      if (owfl > (Int_t)Cutoff)
        break;
      owfl++;
      WDist.push_back(WDistCalc(CDistC.at(k - 1)));
    }
  }

  return TotEnEle * 1.e-09; // in Gev
}

// =============================================================

Int_t PndFtsSingleStraw::Cluster()
{

  // calculate the number of clusters CNumb
  // their distance CDist and
  // their number of electrons CNele

  CNumb = 0;
  CDist.clear();
  CDistC.clear();
  CNele.clear();

  Double_t DisTot = 0;

  for (Int_t k = 1; k <= 1000; k++) {
    // distance

    Double_t path = -Lcl * log(1 - gRandom->Uniform() + 0.000001);
    DisTot += path;
    if (DisTot > Dx)
      break;
    CNumb = k;
    CDist.push_back(path);
    CDistC.push_back(DisTot);
    CNele.push_back((Double_t)Eject());
  }

  return CNumb;
}

// =============================================================

Int_t PndFtsSingleStraw::Eject()
{
  // find the number of electrons in a cluster
  Int_t Inelect;
  Double_t nelect = 0.;

  nelect = TMath::BinarySearch(21, CumClus, gRandom->Uniform());
  if (nelect < 19) {
    return Inelect = (Int_t)(nelect) + 1;
  } else {
    // long delta electron tail
    return Inelect = (Int_t)(20. / (1.03 - gRandom->Uniform()));
  }
}

// =============================================================

Double_t PndFtsSingleStraw::RRise(Double_t gamma2)
{

  // interpolate the relativisic rise of the
  // number of cluster per cm, starting from the one
  // measured at the ionization minimum

  Double_t Rise;
  Double_t lg = log10(gamma2);

  if (1. <= gamma2 && gamma2 <= 2.2) {
    Rise = -2.159 * lg + 1.7;
  } else if (2.2 <= gamma2 && gamma2 <= 6.) {
    Rise = 1.;
  } else if (6. <= gamma2 && gamma2 <= 200.) {
    Rise = 0.302 * lg + 0.765;
  } else if (200. <= gamma2 && gamma2 <= 1000.) {
    Rise = 0.1431 * lg + 1.131;
  } else if (1000. <= gamma2) {
    Rise = 1.54;
  } else {
    Rise = 1.7;
  }

  return Rise;
}

// ==========================================================================

void PndFtsSingleStraw::Polya(Double_t bpar)
{

  // calculate the cumulative of the Polya distribution
  // for samplig the gain fluctuations

  Double_t eps = 0.0001;
  Double_t Dxx = 0.05, xx = 0., x1, x2;
  Double_t PMax;

  Double_t k = 1. / bpar - 1.;

  // find Xmax

  PMax = eps * pow(k, k) * exp(-k);
  Double_t value = 1.e+06;
  Xmax = 2 * k;
  while (value > PMax) {
    Xmax += Dxx;
    value = pow(Xmax, k) * exp(-Xmax);
  }
  Xmax += -0.5 * Dxx;

  // calculate the cumulative

  Double_t dx = Xmax / NPolya;
  Xs[0] = 0;
  for (Int_t i = 1; i < NPolya; i++) {
    x1 = xx;
    xx += dx;
    x2 = xx;
    Xs[i] = x2;
    if (i > 1)
      PolyaCum[i] = PolyaCum[i - 1] + 0.5 * dx * (pow(x1, k) * exp(-x1) + pow(x2, k) * exp(-x2)) / TMath::Gamma(k + 1);
    else
      PolyaCum[i] = PolyaCum[i - 1] + 0.5 * dx * pow(x2, k) * exp(-x2) / TMath::Gamma(k + 1);
  }

  // adjust the normalization

  for (Int_t ii = 0; ii < NPolya; ii++)
    PolyaCum[ii] /= PolyaCum[NPolya - 1];
}

// =====================================================================

Double_t PndFtsSingleStraw::PolyaSamp()
{

  // sampling a wire gain fluctuation in the gas

  Double_t xr = gRandom->Uniform();
  Int_t n = TMath::BinarySearch(NPolya, PolyaCum, xr);
  Double_t xsamp = Xmax;
  if (n < NPolya - 1) {
    xsamp = Xs[n] + ((xr - PolyaCum[n]) / (PolyaCum[n + 1] - PolyaCum[n])) * (Xs[n + 1] - Xs[n]);
  }
  return xsamp;
}

// =====================================================================

TVector3 PndFtsSingleStraw::WDistCalc(Double_t DisTot)
{

  // calculation of the distance of the cluster from the wire
  // DisTot is the cluster distance from the track entrance
  // diffusion effects are considered

  // wire director cosines

  Double_t Wlength = sqrt((Wx2 - Wx1) * (Wx2 - Wx1) + (Wy2 - Wy1) * (Wy2 - Wy1) + (Wz2 - Wz1) * (Wz2 - Wz1));
  Wp = (Wx2 - Wx1) / Wlength;
  Wq = (Wy2 - Wy1) / Wlength;
  Wr = (Wz2 - Wz1) / Wlength;

  // current track coordinates
  Double_t xcor = Calpha * DisTot + Xin;
  Double_t ycor = Cbeta * DisTot + Yin;
  Double_t zcor = Cgamma * DisTot + Zin;

  // cross product VV1= (wire x track) for the distance
  Double_t XX1 = Wr * (ycor - Wy1) - Wq * (zcor - Wz1);
  Double_t YY1 = Wp * (zcor - Wz1) - Wr * (xcor - Wx1);
  Double_t ZZ1 = Wq * (xcor - Wx1) - Wp * (ycor - Wy1);

  // vector for the 3-D distance from the  wire (from wire x VV1)
  Double_t XX = Wq * ZZ1 - Wr * YY1;
  Double_t YY = Wr * XX1 - Wp * ZZ1;
  Double_t ZZ = Wp * YY1 - Wq * XX1;
  // cout<<" XYZ  "<<XX<<" "<<YY<<" "<<ZZ<<endl;
  Double_t DDistcm = sqrt(XX * XX + YY * YY + ZZ * ZZ);

  // director cosines of the distance vector
  Double_t cosx = XX / DDistcm;
  Double_t cosy = YY / DDistcm;
  Double_t cosz = ZZ / DDistcm;

  // sampling of the diffusion
  // Double_t DDist = DDistcm*10.;  // distance in mm

  // longitudinal coefficient of diffusion (GARFIELD) cm --> micron
  // MAGY Ar/CO2 90/10 20-7-2006 GARFIELD
  Double_t SigL = DiffLong(DDistcm);
  // ... in cm
  SigL *= 1.e-04; // in cm

  // tranverse coefficient (GARFIELD)  cm--> micron
  // MAGY Ar/CO2 90/10 20-7-2006 GARFIELD
  Double_t SigT = DiffTran(DDistcm);
  SigT *= 1.e-04; // in cm

  // sampling of Longitudinal and Transverse diffusion
  Double_t difL = gRandom->Gaus(0., SigL);
  Double_t difT = gRandom->Gaus(0., SigT);

  // vector addition to the distance
  // the transverse component has the same dir cos of the wire
  XX += difL * cosx + difT * Wp;
  YY += difL * cosy + difT * Wq;
  ZZ += difL * cosz + difT * Wr;
  // cout<<" XYZ+ dif  "<<XX<<" "<<YY<<" "<<ZZ<<endl;
  // cout<<"  --------------------------------------------------- "<<endl;
  TVector3 TDist(XX, YY, ZZ);

  return TDist;
}

// =====================================================================

void PndFtsSingleStraw::TDirCos()
{

  // director cosines of the track (called for each track)

  // path into the straw
  Rpath = sqrt((Xout - Xin) * (Xout - Xin) + (Yout - Yin) * (Yout - Yin) + (Zout - Zin) * (Zout - Zin));

  // director cosines
  Calpha = (Xout - Xin) / Rpath;
  Cbeta = (Yout - Yin) / Rpath;
  Cgamma = (Zout - Zin) / Rpath;
}

// =====================================================================

Int_t PndFtsSingleStraw::TimeEle()
{

  TeleTime.clear();

  // calculate the arrival times (ns) for each electron in TeleTime
  // from cm to ns  <---------------------------------
  // return the number of electrons
  // Author:A. Rotondi 20-7-2006

  Int_t ido = WDist.size();
  Double_t etime;

  // cutoff the total charge to 20 times the average
  if (ido > (Int_t)Cutoff)
    ido = (Int_t)Cutoff;

  for (Int_t k = 0; k < ido; k++) {
    Double_t dst = WDist.at(k).Mag(); // distance in cm

    // space to time calculation from GARFIELD

    if (pSTP < 1.9) {
      if (Radius < 0.5) {
        // V=1600V diameter 4 mm 90/10
        etime = -1.624e-05 + 0.1258 * dst + 0.8079 * pow(dst, 2) - 2.918 * pow(dst, 3) + 10.33 * pow(dst, 4) - 10.84 * pow(dst, 5);
      } else {
        // V=1600V diameter 5 mm   90/10
        etime = -6.763e-05 + 0.1471 * dst + 0.3625 * pow(dst, 2) + 0.3876 * pow(dst, 3) + 1.04 * pow(dst, 4) - 1.693 * pow(dst, 5);
      }
    } else {
      // 2 bar 2000V, 5 mm, 80/20
      etime = -0.0001014 + 0.1463 * dst - 0.1694 * pow(dst, 2) + 2.4248 * pow(dst, 3) - 1.793 * pow(dst, 4);
    }

    etime *= 1000.; // nano seconds
    TeleTime.push_back(etime);
  }

  return TeleTime.size();
}

// =====================================================================

Double_t PndFtsSingleStraw::Signal(Double_t t, Double_t t0)
{

  // electric signal at time  t of a cluster arriving at t0
  Double_t elesig;
  // Double_t A = 1.03e-03; //[R.K. 01/2017] unused variable?
  // Double_t B = 3.95; //[R.K. 01/2017] unused variable?
  // Double_t C = 0.228; //[R.K. 01/2017] unused variable?
  // Double_t D = -3.839e-02; //[R.K. 01/2017] unused variable?
  // Double_t E = 1.148e-03; //[R.K. 01/2017] unused variable?

  Double_t x = t - t0;
  // if(x>0) elesig = A*exp(B*log(x)-C*x)*(1+D*x+E*x*x);  // Sokolov Signal
  if (x > 0)
    elesig = pow(2. * x / 10., 2) * exp(-2. * x / 10.); // Wirtz signal
  else
    elesig = 0.;

  return elesig;
}

// =====================================================================

Int_t PndFtsSingleStraw::StrawSignal(Int_t nsteps)
{

  // creation of nstep values of
  // the straw global Pulse (sum on all clusters)
  // return the number of primary electrons

  PulseMax = 0;
  Pulse.clear();
  PulseT.clear();
  AmplSig.clear();

  Int_t neltot = TimeEle(); // creation and size of TeleTime (electron times)

  Double_t Tmax = 1.e-25;
  for (Int_t k = 0; k < neltot; k++)
    if (Tmax < TeleTime.at(k))
      Tmax = TeleTime.at(k);
  Tmax += 100.;

  Double_t Dt = Tmax / nsteps; // number of steps of the signal

  // AmplSig is the amplitude of each electron

  for (Int_t j = 0; j < neltot; j++) {
    AmplSig.push_back(bPolya * PolyaSamp());
  }

  // creation of the signal Pulse(PulseT) time in ns
  for (Int_t j = 0; j < nsteps; j++) {
    Double_t te = j * Dt;
    Double_t sumele = 0.;
    for (Int_t jj = 0; jj < neltot; jj++) {
      Double_t te0 = TeleTime.at(jj);
      sumele += AmplSig.at(jj) * Signal(te, te0);
    }

    Pulse.push_back(sumele);
    PulseT.push_back(te);
  }

  // add a random noise (3% of maximum) plus
  // a 1% of 200 ns periodic signal

  Double_t Pmax = 1.e-25;
  for (Int_t k = 0; k < (Int_t)Pulse.size(); k++)
    if (Pmax < Pulse.at(k))
      Pmax = Pulse.at(k);

  for (Int_t k = 0; k < (Int_t)Pulse.size(); k++) {
    Pulse.at(k) += 0.03 * Pmax * gRandom->Uniform() + 0.01 * Pmax * (sin(6.28 * PulseT.at(k) / 120.));
    //    if(Pulse[k]<0) Pulse[k] *= -1.;
  }

  PulseMax = Pmax;

  // set variable threshold for the signals (constant fraction)
  // Thresh1=0.05*Pmax;
  // Thresh2=0.15*Pmax;

  return neltot;
}

// =====================================================================

Int_t PndFtsSingleStraw::StrawTime()
{

  // simulate the discrimination of the straw signal
  // and give the time
  // discriminator technique: set 2 threshold, select the first one
  // (the low one) only if the second one is fired (FINUDA system)

  PulseTime = 0.;

  Int_t ind = 0;
  Int_t flag1 = 0;
  Int_t flag2 = 1;

  for (Int_t k = 0; k < (Int_t)Pulse.size(); k++) {
    if (flag1 == 0 && Pulse[k] > Thresh1) {
      flag1 = 1;
      ind = k;
    }

    //     if(Pulse[k]<Thresh1) {flag1=0;  ind=0;} // reset if signal decreases

    //     if(flag1==1 && Pulse[k]>Thresh2) {
    //       flag2=1;
    //       break;
    //    }
  }
  if (flag1 == 1 && flag2 == 1)
    PulseTime = PulseT.at(ind);

  return ind;
}

// =====================================================================

Double_t PndFtsSingleStraw::TrueDist(Double_t Point[])
{
  // service routine that finds the distance in cm from the wire
  // by knowing the wire coordinates (class variables)
  // and the input-output points Point[6]

  Double_t truedist = 0;

  // wire director cosines
  Double_t Wlength = sqrt((Wx2 - Wx1) * (Wx2 - Wx1) + (Wy2 - Wy1) * (Wy2 - Wy1) + (Wz2 - Wz1) * (Wz2 - Wz1));
  Wp = (Wx2 - Wx1) / Wlength;
  Wq = (Wy2 - Wy1) / Wlength;
  Wr = (Wz2 - Wz1) / Wlength;

  // director cosines of the given track
  Double_t Modu = sqrt((Point[3] * Point[0]) * (Point[3] * Point[0]) + (Point[4] * Point[1]) * (Point[4] * Point[1]) + (Point[5] * Point[2]) * (Point[5] * Point[2]));
  Double_t dcx = (Point[3] - Point[0]) / Modu;
  Double_t dcy = (Point[4] - Point[1]) / Modu;
  Double_t dcz = (Point[5] - Point[2]) / Modu;

  // distance formula
  Double_t p1 = (Point[0] - Wx1) * (dcy * Wr - dcz * Wq);
  Double_t p2 = -(Point[1] - Wy1) * (dcx * Wr - dcz * Wp);
  Double_t p3 = (Point[2] - Wz1) * (dcx * Wq - dcy * Wp);
  Double_t Det = p1 + p2 + p3;
  Double_t Disc = sqrt((dcy * Wr - dcz * Wq) * (dcy * Wr - dcz * Wq) + (dcz * Wp - dcx * Wr) * (dcz * Wp - dcx * Wr) + (dcx * Wq - dcy * Wp) * (dcx * Wq - dcy * Wp));
  if (Disc > 0)
    truedist = TMath::Abs(Det / Disc);

  return truedist; // distance in cm
}

// =====================================================================

Double_t PndFtsSingleStraw::TimnsToDiscm(Double_t time)
{

  // distance in cm from time in ns for pSTP =1 and pSTP=2 atm
  // utility routine for the track reconstruction
  // last update: A. Rotondi 3-3-2007

  Double_t drift;

  // 1  absolute atm (NTP) 20-7-2006 GARFIELd Ar/CO2 90/10 MAGY
  //  ns -->  mm

  if (pSTP < 1.9) {
    if (Radius < 0.5) {

      drift = -0.0140 - 1.37281e-01 + 5.13978e-02 * time + 7.65443e-05 * pow(time, 2) - 9.53479e-06 * pow(time, 3) + 1.19432e-07 * pow(time, 4) - 6.19861e-10 * pow(time, 5) +
              1.35458e-12 * pow(time, 6) - 1.10933e-15 * pow(time, 7);
    }

    else {
      // 1600 V 5 mm
      if (time < 120.) {

        drift = 0.0300 - 1.07377e-01 + 3.65134e-02 * time + 1.20909e-03 * pow(time, 2) - 4.56678e-05 * pow(time, 3) + 6.70207e-07 * pow(time, 4) - 4.99204e-09 * pow(time, 5) +
                2.19079e-11 * pow(time, 6) - 8.01791e-14 * pow(time, 7) + 2.16778e-16 * pow(time, 8);
      } else {

        drift = 0.0300 - 8.91701e-01 + 4.68487e-02 * time + 1.00902e-03 * pow(time, 2) - 4.00359e-05 * pow(time, 3) + 6.23768e-07 * pow(time, 4) - 5.20556e-09 * pow(time, 5) +
                2.41502e-11 * pow(time, 6) - 5.85450e-14 * pow(time, 7) + 5.77250e-17 * pow(time, 8);
      }
    }
  }

  else {
    // 2 absolute atm  5 mm   80/20 (1 bar overpressure)
    if (time <= 50.) {

      // on thresh static 10%

      drift = -0.0300 + 1.28551e-02 + 1.44029e-02 * time - 3.67834e-03 * pow(time, 2) + 3.32034e-04 * pow(time, 3) - 6.36592e-06 * pow(time, 4) - 7.82907e-08 * pow(time, 5) +
              3.58931e-09 * pow(time, 6) - 2.93491e-11 * pow(time, 7);

      // one thresh static 3%
      //       drift =    +5.35238e-02
      // 	         -4.25959e-02 *time
      // 	         +7.59448e-03 *pow(time,2)
      // 	         -1.44009e-04 *pow(time,3)
      // 	         -1.76365e-06 *pow(time,4)
      // 	         +3.29531e-08 *pow(time,5)
      // 	         +1.12115e-09 *pow(time,6)
      //                  -1.72919e-11 *pow(time,7) ;

    } else if (50. < time && time < 130.) {

      // on thresh static 10%

      drift = -0.0190 + 4.40993e-01 - 2.91442e-02 * time + 3.06237e-03 * pow(time, 2) - 6.07870e-05 * pow(time, 3) + 5.97431e-07 * pow(time, 4) - 3.09238e-09 * pow(time, 5) +
              7.70537e-12 * pow(time, 6) - 6.49086e-15 * pow(time, 7);

      // one thresh static 3%
      //       drift =    +2.25702e-02
      // 	         +5.17806e-02 *time
      // 	         +2.53060e-04 *pow(time,2)
      // 	         -1.60338e-05 *pow(time,3)
      // 	         +2.14805e-07 *pow(time,4)
      // 	         -1.30249e-09 *pow(time,5)
      // 	         +3.38791e-12 *pow(time,6)
      //                  -2.10503e-15 *pow(time,7) ;

    }

    else {

      // on thresh static 10%
      drift = -0.0100 + 4.28757e-01 - 1.95413e-02 * time + 3.02333e-03 * pow(time, 2) - 6.13920e-05 * pow(time, 3) + 5.93656e-07 * pow(time, 4) - 3.05271e-09 * pow(time, 5) +
              8.05446e-12 * pow(time, 6) - 8.59626e-15 * pow(time, 7);

      // one thresh static 3%
      //       drift =    +1.57217e-01
      // 	         +5.79365e-02*time
      // 	         +2.15636e-04*pow(time,2)
      // 	         -1.66405e-05*pow(time,3)
      // 	         +2.13726e-07*pow(time,4)
      // 	         -1.26888e-09 *pow(time,5)
      // 	         +3.68533e-12 *pow(time,6)
      //                  -4.24032e-15 *pow(time,7) ;
    }
  }

  drift = 0.1 * drift;
  if (drift < 0.)
    drift = 0.;
  return drift;
}

// --------------------------------------------------------------------------------

Double_t PndFtsSingleStraw::PartToTime(Double_t xPMass, Double_t xPMom, Double_t InOut[])
{

  // find the time of a particle of mass xPmass, momentum xPMom, with
  // input-output coordinate InOut[6]
  // Useful for MC pplication after a call to PutWireXYZ

  TInit(xPMass, xPMom, InOut); // start the event
  Out1 = StrawCharge();        // energy loss (GeV) to generate charge
  Out2 = StrawSignal(Nchann);  // generate the straw signal
  Out3 = StrawTime();          // find the straw drift time PulseTime

  return PulseTime;
}

// --------------------------------------------------------------------------------
Double_t PndFtsSingleStraw::PartToADC()
{

  // return the energy loss in the tube as charge signal
  // taking into account the Polya fluctuations
  Double_t ADCsignal = 0.;

  for (Int_t j = 1; j <= NNClus; j++) {

    for (Int_t jc = 1; jc <= (Int_t)CNeleT[j - 1]; jc++)
      ADCsignal += bPolya * GasGain * PolyaSamp();
  }

  return ADCsignal;
}

// --------------------------------------------------------------------------------

Double_t PndFtsSingleStraw::FastPartToADC()
{

  // return the energy loss (from the Urban distribution)
  // in the tube as charge signal
  // taking into account the Polya fluctuations

  Double_t ADCsignal = 0.;

  // number of elecrons. Wi is the mean energy lost per free
  // electrn in eV
  Int_t NtotEle = (Int_t)(1.e+09 * STUrban() / Wi);

  for (Int_t j = 1; j <= NtotEle; j++) {

    ADCsignal += bPolya * GasGain * PolyaSamp();
  }

  return ADCsignal;
}

// ----------------------------------------------------------------------------------
Double_t PndFtsSingleStraw::FastRec(Double_t TrueDcm, Int_t Flag)
{

  // having the true distance TrueDcm (cm) as input,
  // return the reconstructed distance (cm), in a fast
  // and approximated way
  // by sampling on the simulated reconstruction curve
  // When Press =2 and Flag=1 one uses the julich experimental data
  // A. Rotondi March 2007

  Double_t resmic;

  // 1 atm pressure
  if (pSTP < 1.9) {
    if (Radius < 0.45) {
      if (TrueDcm < 0.38) {
        resmic =
          1.24506e+02 - 1.80117e+02 * TrueDcm + 3.76905e+03 * pow(TrueDcm, 2) - 4.63251e+04 * pow(TrueDcm, 3) + 1.80068e+05 * pow(TrueDcm, 4) - 2.21094e+05 * pow(TrueDcm, 5);
      } else
        resmic = 57.;
    }
    // radius > 0.4 cm
    else {
      if (TrueDcm < 0.48) {
        resmic = 1.53656e+02 - 5.07749e+03 * TrueDcm + 1.73707e+05 * pow(TrueDcm, 2) - 2.72285e+06 * pow(TrueDcm, 3) + 2.28719e+07 * pow(TrueDcm, 4) -
                 1.12921e+08 * pow(TrueDcm, 5) + 3.39427e+08 * pow(TrueDcm, 6) - 6.12741e+08 * pow(TrueDcm, 7) + 6.12041e+08 * pow(TrueDcm, 8) - 2.60444e+08 * pow(TrueDcm, 9);
      } else
        resmic = 72.;
    }
  }
  // 2 atm pressure radius 5 cm

  else {
    if (Flag == 0) {
      if (TrueDcm < 0.48) {
        resmic = +1.06966e+02 - 4.03073e+03 * TrueDcm + 1.60851e+05 * pow(TrueDcm, 2) - 2.87722e+06 * pow(TrueDcm, 3) + 2.67581e+07 * pow(TrueDcm, 4) -
                 1.43397e+08 * pow(TrueDcm, 5) + 4.61046e+08 * pow(TrueDcm, 6) - 8.79170e+08 * pow(TrueDcm, 7) + 9.17095e+08 * pow(TrueDcm, 8) - 4.03253e+08 * pow(TrueDcm, 9);
      } else
        resmic = 30.;
    } else {
      // data from julich
      if (TrueDcm < 0.48) {
        resmic = 20. + 1.48048e+02 - 3.35951e+02 * TrueDcm - 1.87575e+03 * pow(TrueDcm, 2) + 1.92910e+04 * pow(TrueDcm, 3) - 6.90036e+04 * pow(TrueDcm, 4) +
                 1.07960e+05 * pow(TrueDcm, 5) - 5.90064e+04 * pow(TrueDcm, 6);
      } else
        resmic = 65.;
    }
  }

  // real distance in cm
  Double_t rsim = gRandom->Gaus(TrueDcm, resmic * 0.0001);
  if (rsim < 0.)
    rsim = TrueDcm - TrueDcm * gRandom->Uniform(0., 1.);
  else if (rsim > 0.5)
    rsim = TrueDcm + (0.5 - TrueDcm) * gRandom->Uniform(0., 1.);

  return rsim;
}
// --------------------------------------------------------------------------------
Double_t PndFtsSingleStraw::DiffLong(Double_t Distcm)
{

  // return the longitudinal diffusion
  // from cm to microns
  // MAGY GARFIELD Ar/CO2 90/10 1 bar (NTP) 20-7-2006
  //
  Double_t DiffMic;

  if (pSTP < 1.9) {
    if (Radius < 0.5) {
      // 4 mm 1600 V   90/10
      DiffMic = 0.896 + 1387. * Distcm - 1.888e+04 * pow(Distcm, 2) + 1.799e+05 * pow(Distcm, 3) - 9.848e+05 * pow(Distcm, 4) + 3.009e+06 * pow(Distcm, 5) -
                4.777e+06 * pow(Distcm, 6) + 3.074e+06 * pow(Distcm, 7);
    } else {
      // 5 mm 1600 V
      DiffMic = 1.537 + 1246. * Distcm - 1.357e+04 * pow(Distcm, 2) + 1.049e+05 * pow(Distcm, 3) - 4.755e+05 * pow(Distcm, 4) + 1.211e+06 * pow(Distcm, 5) -
                1.6e+06 * pow(Distcm, 6) + 8.533e+05 * pow(Distcm, 7);
    }
  }

  else {
    // 2000V 5 mm 2 bar 80/20
    DiffMic = 2.135 + 818. * Distcm - 1.044e+04 * pow(Distcm, 2) + 8.31e+04 * pow(Distcm, 3) - 3.492e+05 * pow(Distcm, 4) + 7.959e+05 * pow(Distcm, 5) -
              9.378e+05 * pow(Distcm, 6) + 4.492e+05 * pow(Distcm, 7);
  }
  return DiffMic;
}
// --------------------------------------------------------------------------------
Double_t PndFtsSingleStraw::DiffTran(Double_t Distcm)
{

  // return the transverse diffusion in microns
  // from cm to microns
  // MAGY GARFIELD Ar/CO2 90/10 1 bar (NTP)  20-7-2006
  //
  Double_t DiffMic;

  if (pSTP < 1.9) {
    if (Radius < 0.5) {
      // 4 mm 1600 V   90/10
      //       DiffMic = + 0.8513 + 1648.*Distcm   - 1.085e+04*pow(Distcm,2)
      // 	+ 7.38e+04*pow(Distcm,3) - 3.025e+05*pow(Distcm,4)
      // 	+ 6.067e+05*pow(Distcm,5) - 4.643e+04*pow(Distcm,6);
      DiffMic =
        +1.482 + 1529. * Distcm - 6755. * pow(Distcm, 2) + 2.924e+04 * pow(Distcm, 3) - 0.9246e+05 * pow(Distcm, 4) + 1.548e+05 * pow(Distcm, 5) - 1.002e+05 * pow(Distcm, 6);
    } else {
      // 5 mm 1600 V  90/10
      DiffMic =
        +1.482 + 1529. * Distcm - 6755. * pow(Distcm, 2) + 2.924e+04 * pow(Distcm, 3) - 0.9246e+05 * pow(Distcm, 4) + 1.548e+05 * pow(Distcm, 5) - 1.002e+05 * pow(Distcm, 6);
    }
  } else {

    // 5 mm   2000 V  2 bar  80/20
    DiffMic = +2.094 + 1138. * Distcm - 7557. * pow(Distcm, 2) + 2.968e+04 * pow(Distcm, 3) - 6.577e+04 * pow(Distcm, 4) + 7.581e+04 * pow(Distcm, 5) - 3.497e+04 * pow(Distcm, 6);
  }
  return DiffMic;
}
// --------------------------------------------------------
Double_t PndFtsSingleStraw::DistEle(Double_t time)
{

  // dist  in cm  from time in ns for pSTP =1 and pSTP=2
  // utility routine for the >SINGLE< electron reconstruction
  // last update: A. Rotondi 20-7-2006

  Double_t drift;

  // 1  absolute atm (NTP) 20-7-2006 GARFIELd Ar/CO2 90/10 MAGY
  //  ns -->  cm

  time *= 0.001; // time in micro s

  if (pSTP < 2.) {
    if (Radius < 0.5) {
      // 1600 V 4 mm  90/10
      drift = 0.001629 + 6.194 * time - 56.55 * pow(time, 2) + 355.8 * pow(time, 3) - 903.2 * pow(time, 4);
    } else {
      // 1600 V 5 mm  90/10
      drift = 0.003365 + 5.734 * time - 41.88 * pow(time, 2) + 191.2 * pow(time, 3) - 333.4 * pow(time, 4);
    }
  }

  else {

    // 2 absolute atm 2000 V 5 mm 80/20 (1 bar overpressure)

    drift = 0.003365 + 7.056 * time - 62.28 * pow(time, 2) + 306.1 * pow(time, 3) - 558.7 * pow(time, 4);
  }
  return drift; // in cm
}
