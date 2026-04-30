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

#include "PndStt2Digitizer.h"
#include "TF1.h"
#include "TRandom3.h"

using pndtools::split;
using pndtools::str2d;

// ----------------------------------
//   constructor
// ----------------------------------
PndStt2Digitizer::PndStt2Digitizer(vector<string> vfcn) : fFcnT2Rmean(0), fFcnT2RsigL(0), fFcnT2RsigH(0), fFcnR2Tmean(0), fFcnR2TsigL(0), fFcnR2TsigH(0)
{
  int nfcn = vfcn.size();
  if (nfcn!=6) {
    std::cout <<"[ERROR] - PndStt2Digitizer - Expected 6 functions, but got "<<nfcn<<". Filling up with 'pol1(0),0,1'."<<std::endl;
    while (vfcn.size()<6) vfcn.push_back("pol1(0),0,1");
  }
  
  vector<string> vffml;
  vector<vector<double>> vfpar;

  for (auto sfcn:vfcn) {
    vector<string>  tok = split(sfcn, ",", 1);
    vffml.push_back(tok[0]);
    tok.erase(tok.begin());
    vfpar.push_back(str2d(tok));
  }
  
  fFcnT2Rmean = new TF1("fFcnT2Rmean", vffml[0].c_str());
  fFcnT2RsigL = new TF1("fFcnT2RsigL", vffml[1].c_str());
  fFcnT2RsigH = new TF1("fFcnT2RsigH", vffml[2].c_str());
  fFcnR2Tmean = new TF1("fFcnR2Tmean", vffml[3].c_str());
  fFcnR2TsigL = new TF1("fFcnR2TsigL", vffml[4].c_str());
  fFcnR2TsigH = new TF1("fFcnR2TsigH", vffml[5].c_str());
  
  fFcnT2Rmean->SetParameters(&(vfpar[0][0]));
  fFcnT2RsigL->SetParameters(&(vfpar[1][0]));
  fFcnT2RsigH->SetParameters(&(vfpar[2][0]));
  fFcnR2Tmean->SetParameters(&(vfpar[3][0]));
  fFcnR2TsigL->SetParameters(&(vfpar[4][0]));
  fFcnR2TsigH->SetParameters(&(vfpar[5][0]));
}

// ----------------------------------
//   destructor
// ----------------------------------
PndStt2Digitizer::~PndStt2Digitizer()
{
  if (nullptr != fFcnT2Rmean) delete fFcnT2Rmean;
  if (nullptr != fFcnT2RsigL) delete fFcnT2RsigL;
  if (nullptr != fFcnT2RsigH) delete fFcnT2RsigH;
  if (nullptr != fFcnR2Tmean) delete fFcnR2Tmean;
  if (nullptr != fFcnR2TsigL) delete fFcnR2TsigL;
  if (nullptr != fFcnR2TsigH) delete fFcnR2TsigH;
}

// ----------------------------------
//   random value from BiGaussian
// ----------------------------------
double PndStt2Digitizer::GetRndBiGaus(double mu, double sigl, double sigh)
{
  // ##### drift time
  double rnd = -1; 
  while (rnd<0) 
    rnd = gRandom->Rndm()<sigl/(sigl+sigh) ? mu-fabs(gRandom->Gaus(0,sigl)) : mu+fabs(gRandom->Gaus(0,sigh));
  
  return rnd;
}

// ----------------------------------
//  drift time [ns] from distance in [mm]
// ----------------------------------
double PndStt2Digitizer::DriftTime(double dist, double resfact)
{
  // ##### parametrization of drift time
  double mu   = fFcnT2Rmean->Eval(dist);         // mu of bi-gauss
  double sigl = fFcnT2RsigL->Eval(dist)*resfact; // sigma1 of bi-gauss with resolution factor
  double sigh = fFcnT2RsigH->Eval(dist)*resfact; // sigma2 of bi-gauss
  
  return GetRndBiGaus(mu, sigl, sigh);           // random drift time from distribution t(dist)
}

// ----------------------------------
//  isochrone rad,sig_low,sig_hi [mm] from drift time [ns] 
// ----------------------------------
vector<double> PndStt2Digitizer::IsochroneRadius(double tdrift, double resfact)
{
  // ##### parametrization of isochrone radius
  double mu   = fFcnR2Tmean->Eval(tdrift);         // mu of bi-gauss and scale mm -> cm
  double sig1 = fFcnR2TsigL->Eval(tdrift)*resfact; // sigma1 of bi-gauss with resolution factor
  double sig2 = fFcnR2TsigH->Eval(tdrift)*resfact; // sigma2 of bi-gauss
  
  // ##### isochrone radius
  double r_iso = mu>0 ? mu : 0.0; 
  
  return {r_iso, sig1, sig2};
}

