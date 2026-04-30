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

// -------------------------------------------------------------------------
// -----            PndCorrDistGenerator source file                 -----
// -----          Created 23/05/07 by Galoyan Aida                     -----
// -------------------------------------------------------------------------

#include <iostream>
#include "TClonesArray.h"
#include "TFile.h"
#include "TLorentzVector.h"
#include "TRandom.h"
#include "TMath.h"
#include "TH2F.h"
#include "TF1.h"
#include "TVector3.h"
//#include "THParticle.h"
#include "PndCorrDistGenerator.h"
#include "FairPrimaryGenerator.h"

using namespace std;

// -----   Default constructor   ------------------------------------------
PndCorrDistGenerator::PndCorrDistGenerator()
{
  iEvent = 0;
  fInputFile = nullptr;
  fInputHist = nullptr;
  fPdgType = 0;
  fParam = kFALSE;
  fTheLow = 0.;
  fTheHigh = 0.;
}
// ------------------------------------------------------------------------

//
// -----   Standard constructor   -----------------------------------------
PndCorrDistGenerator::PndCorrDistGenerator(const Char_t *fileName)
{
  iEvent = 0;
  fFileName = fileName;
  fInputFile = new TFile(fFileName);
  if (fParam) {
    cout << "choosing parametrization modell" << endl;
  } else {
    cout << " Random events gerated from a TH2D P(th) dist-->GiBUU Modell" << endl;
    fInputHist = (TH2F *)fInputFile->Get("source");
  }
}
// ------------------------------------------------------------------------

// -----   Destructor   ---------------------------------------------------
PndCorrDistGenerator::~PndCorrDistGenerator()
{
  CloseInput();
}
// ------------------------------------------------------------------------

// -----   Public method ReadEvent   --------------------------------------
Bool_t PndCorrDistGenerator::ReadEvent(FairPrimaryGenerator *primGen)
{

  // Check for input file
  if (!fInputFile) {
    cout << "-E PndCorrDistGenerator: Input file nor open!" << endl;
    return kFALSE;
  }

  // defining kinematics variables, angle is expressed in degrees.

  Double_t phi, p = 0.0, theta = 0.0;
  Double_t pz = 0., py = 0., px = 0.;

  phi = gRandom->Uniform(0, 360) * TMath::DegToRad();

  TF1 *lan; // Landau
  TF1 *exp; // exponential
  TF1 *MBol;
  // relativistic Maxwell Boltzmann dist <v> (Maxwell-Juettner dist), mean velocity
  // For more INFO see, http://en.wikipedia.org/wiki/Maxwell–Jüttner_distribution

  Double_t mean_p = 0.;

  if (fParam) {

    //--> Landau, dsigma/dtheta dist

    // Constant     1.03980e+03 //Peak height (kFALSE-->0), no integral,
    // MPV          1.10093e+01
    // Sigma        4.65284e+00

    lan = new TF1("lan", "1.03980e+03*TMath::Landau(x,[0],[1],0)", fTheLow, fTheHigh);
    lan->SetParameters(1.10093e+01, 4.65284e+00);

    // getting theta
    theta = lan->GetRandom();

    // <p> --- > Max. Boltzmann dist --> p

    // more or less -->exponential <P>(theta)
    // Constant     4.93964e-01
    //  Slope       -1.41427e-02

    mean_p = MeanMomentum(theta); // getting <p>
    p = MaxBoltDistP(mean_p);

    pz = p * TMath::Cos(theta * TMath::DegToRad());
    py = p * TMath::Sin(theta * TMath::DegToRad()) * TMath::Sin(phi);
    px = p * TMath::Sin(theta * TMath::DegToRad()) * TMath::Cos(phi);

  } else {
    // getting p and theta randomly from a TH2F object
    // both variables are correlated to each other
    // phi is independtly generated over the full angular range.

    fInputHist->GetRandom2(theta, p);

    pz = p * TMath::Cos(theta * TMath::DegToRad());
    py = p * TMath::Sin(theta * TMath::DegToRad()) * TMath::Sin(phi);
    px = p * TMath::Sin(theta * TMath::DegToRad()) * TMath::Cos(phi);
  }
  // vertex coordinates given during generation via prim generator

  // Give track to PrimaryGenerator
  // multiplocity equal to 1
  // pdg is -3312, corresponding to a Xi minus

  fPdgType == 3312;

  // cout<<" generating Xi minus "<<px<<" "<<py<<" "<<pz<<endl;

  primGen->AddTrack(3312, px, py, pz, 0., 0., 0.);

  return kTRUE;
}
// ------------------------------------------------------------------------

// -----   Private method CloseInput   ------------------------------------
void PndCorrDistGenerator::CloseInput()
{
  if (fInputFile) {
    cout << "-I PndCorrDistGenerator: Closing input file " << fFileName << endl;
    fInputFile->Close();
    delete fInputFile;
  }
  fInputFile = nullptr;
}
// ------------------------------------------------------------------------
// -----   Private method MeanMomentum   ------------------------------------
Double_t PndCorrDistGenerator::MeanMomentum(Double_t thet)
{

  cout << "-I PndCorrDistGenerator: Calculating Mean Momentum as function of theta" << endl;
  // f(x) = exp(p0+p1*x)
  Double_t fitval = TMath::Exp((4.93964e-01) - (thet * 1.41427e-02));
  return fitval;
}
// -----   Private method MeanMomentum   ------------------------------------
Double_t PndCorrDistGenerator::MaxBoltDistP(Double_t MeanP)
{

  Double_t Et = 0., EK = 0., v = 0., gamma = 0., zet = 0., sigma = 0.;

  Et = TMath::Sqrt((MeanP * MeanP) + (1.321 * 1.321)); // rel.  energy, speed of light c = 1.
  v = MeanP / Et;                                      // v = beta*c/Etotal
  gamma = 1. / TMath::Sqrt(1. - (v * v));              // gamma factor
  EK = gamma * 1.321 - 1.321;                          // rel. kinetic energy, mass of Xi_minus = 1.321 GeV/c2
  zet = EK / 1.321;

  sigma = TMath::Sqrt(3 * zet);
  Double_t limLow, limHigh;
  limLow = MeanP * 0.882;
  limHigh = MeanP * 1.085;
  // Double_t beta =

  TF1 *MBol = new TF1("mb", "TMath::Gaus(x,[0],[1])", limLow, limHigh);
  // TF1* MBol = new TF1("mb","TMath::Exp(-TMath::Sqrt(1.+ (TMath::Power(x/[0],2))))/(4*TMath::Power([0],3)*TMath::Pi()*[1]*TMath::BesselK(2,1/[1]))",-2.,3);
  // TF1* MBol = new TF1("mb","x*x*[1]*TMath::Exp(-x/[0])/([0]*TMath::BesselK(2,1/[0]))",0.,3);
  // MBol->SetParameter(0,1.321);
  // MBol->SetParameter(1,zet);
  // MBol->SetParameter(0,zet);
  // MBol->SetParameter(1,v);
  MBol->SetParameter(0, MeanP);
  MBol->SetParameter(1, sigma);

  cout << "-I PndCorrDistGenerator: Calculating Momentum distribution via MB <v >" << endl;
  Double_t fitval = MBol->GetRandom();

  return fitval;
}

ClassImp(PndCorrDistGenerator)
