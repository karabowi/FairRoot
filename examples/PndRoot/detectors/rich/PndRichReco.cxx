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

#include "PndRichReco.h"
#include "FairGeoNode.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"

#include "PndMCTrack.h"
#include "TLorentzVector.h"

#include "PndRichGeo.h"
#include "PndRichPhoton.h"
#include "PndRichPDHit.h"
#include "PndRichMirrorSegment.h"
#include "PndRichHitFinder.h"

#include "TH1F.h"
#include "TF1.h"
#include "TProfile.h"
#include <cmath>

#include "Math/GSLMinimizer.h"
#include "Math/Functor.h"

#include "Math/Minimizer.h"
#include "Math/Factory.h"
#include "Math/Functor.h"
#include "TRandom.h"
#include "TError.h"
#include <iostream>
#include <algorithm>

#include <TVectorD.h>

namespace {

// Show usage with all the possible minimizers.
// Minimize the Rosenbrock function (a 2D -function)
// This example is described also in
// http://root.cern.ch/drupal/content/numerical-minimization#multidim_minim
// input : minimizer name + algorithm name
// randomSeed: = <0 : fixed value: 0 random with seed 0; >0 random with given seed
//
// Author: L. Moneta Dec 2010

//------------------------------------------------------------------

std::vector<double> ti;
std::vector<double> fi;
std::vector<size_t> it;
double nopt_, beta_, nnz_;
double dbeta_; //, dnopt_, dnnz_; //[R.K.03/2017] unused variable
double chi2_;

// //[R.K.03/2017] unused function?
// double thc_old(const double phic,
// const double nopt,
// const double beta,
// const double nnz0)
//{
// Double_t cthc = 1.0/nopt/beta;
// Double_t nnz = nnz0;
// if (nnz>1) nnz = 1;
// if (cthc>1) cthc = 1;
// Double_t sthc = std::sqrt(1-cthc*cthc);
// Double_t nng = nnz*cthc+std::sqrt(1-nnz*nnz)*sthc*std::cos(phic);
// Double_t cdthc = nopt*(1-nng*nng);
// cdthc = cdthc + std::sqrt(std::fabs(1-nopt*cdthc))*nng;
// if (cdthc>1) cdthc = 1;
// return std::acos(cdthc)+std::acos(cthc);
//}
//[R.K.03/2017] unused function
// double thc_v1(const double phic,
// const double nopt,
// const double beta,
// const double nnz0)
//{
// Double_t cphc_ = std::cos(phic);
// Double_t sphc_ = std::sin(phic);
////Double_t nopt = par[0];
////Double_t beta = par[1];
// Double_t ctht = nnz0;
// Double_t cthc = 1.0/nopt/beta;
// if (ctht>1) ctht = 1;
// if (cthc>1) cthc = 1;
// Double_t stht = std::sqrt(1-ctht*ctht);
// Double_t sthc = std::sqrt(1-cthc*cthc);
// Double_t cphc = cphc_;
// Double_t nng, cthc_, sthc_, sphc;
// for(Int_t i=0; i<5; i++) {
// nng = ctht*cthc + stht*sthc*cphc;
// cthc_ = 1.0/beta + ctht*(std::sqrt(std::fabs(1-nopt*nopt*(1-nng*nng)))-nopt*nng);
// if (cthc_>1) cthc_ = 1;
// sthc_ = std::sqrt(1-cthc_*cthc_);
// sphc = sphc_/nopt*sthc_/sthc;
// Double_t cphc2 = 1-sphc*sphc;
// Double_t cphcp, cphcm;
// if (cphc2>=0) {
// cphcp = std::sqrt(cphc2);
// cphcm = -cphcp;
//} else {
// cphcp = 0*cphc_;
// cphcm = 0*cphc_;
//}
// Double_t np = cphcp*cphc_+sphc*sphc_;
// Double_t nm = cphcm*cphc_+sphc*sphc_;
// cphc = np>nm ? cphcp : cphcm;
//}
// return std::acos(cthc_);
//}

double thc0(double thc_, double phc_, double nopt, double beta, double nnz)
{
  // Double_t cphc_ = std::cos(phc_); //[R.K. 01/2017] unused variable?
  Double_t sphc_ = std::sin(phc_);
  Double_t sphc2_ = sphc_ * sphc_;
  Double_t nopt2 = nopt * nopt;
  Double_t ctht = nnz;
  Double_t cthc = 1.0 / nopt / beta;
  if (ctht > 1)
    ctht = 1;
  if (cthc > 1)
    cthc = 1;
  Double_t stht2 = 1 - ctht * ctht;
  Double_t sthc2 = 1 - cthc * cthc;
  // Double_t stht = std::sqrt(stht2); //[R.K. 01/2017] unused variable?
  Double_t sthc = std::sqrt(sthc2);
  Double_t cthc_ = cthc, sthc_ = sthc;
  Double_t p;
  Double_t A0, A1; //, A2; //[R.K. 01/2017] unused variable?
  Double_t B0, B1; //, B2; //[R.K. 01/2017] unused variable?
  Double_t C0;
  Double_t F0, F1; //, F2; //[R.K. 01/2017] unused variable?
  for (Int_t i = 0; i < 10; i++) {
    p = (cthc_ - nopt * cthc) / ctht;
    A0 = (1 - nopt2) / p - p - 2 * nopt * ctht * cthc;
    A1 = ((1 - nopt2) / p / p + 1) * sthc_ / ctht;
    // A2 = ((1-nopt2)*(cthc_+2*sthc_*sthc_/p/ctht)/p/p+cthc_)/ctht;
    B0 = 4 * stht2 * sphc2_ * sthc_ * sthc_;
    B1 = 8 * stht2 * sphc2_ * sthc_ * cthc_;
    // B2 = 8*stht2*sphc2_*(cthc_*cthc_-sthc_*sthc_);
    C0 = 4 * nopt2 * stht2 * sthc2;
    F0 = A0 * A0 + B0 - C0;
    F1 = 2 * A0 * A1 + B1;
    // F2 = 2*A0*A2+2*A1*A1+B2;
    thc_ -= F0 / F1;
    cthc_ = std::cos(thc_);
    sthc_ = std::sin(thc_);
  }
  return thc_;
}

double fNopt, fBeta, fNnz;
double thcMid, thcMin, thcMax;

double thc(const double phic, const double nopt, const double beta, const double nnz)
{
  if ((nopt != fNopt) || (beta != fBeta) || (nnz != fNnz)) {
    fNopt = nopt;
    fBeta = beta;
    fNnz = nnz;

    Double_t cthc = 1.0 / fNopt / fBeta;
    Double_t ctht = fNnz;
    Double_t thc = acos(cthc);
    Double_t tht = acos(ctht);
    Double_t dthp = tht + thc;
    Double_t dthm = tht - thc;
    Double_t cdthp = cos(dthp), sdthp = sin(dthp);
    Double_t cdthm = cos(dthm), sdthm = sin(dthm);
    Double_t rp = 1 - fNopt * fNopt * sdthp * sdthp;
    Double_t rm = 1 - fNopt * fNopt * sdthm * sdthm;
    thcMax = acos(fNopt * cthc - ctht * (fNopt * cdthp - sqrt(rp > 0 ? rp : 0)));
    thcMin = acos(fNopt * cthc - ctht * (fNopt * cdthm - sqrt(rm > 0 ? rm : 0)));
    thcMid = ((thcMin + thcMax) / 2 + thc) / 2;
  }
  Double_t thc_ = std::cos(phic) > 0 ? 0.7 * thcMid : 1.3 * thcMid;
  // Double_t thc_ = std::cos(phic)>0 ? 0.9*thcMin : 1.1*thcMax;
  // Double_t cphc_ = std::cos(phic);
  // Double_t thc_ = thcMid;
  // thc_ += cphc_*( cphc_>0 ? thcMin-thcMid : thcMid-thcMax );
  // thc_ += thcMid * ( cphc_>0 ? -0.2 : 0.2 );
  // thc_ = 1.1*( cphc_>0 ? thcMin : thcMax );
  // Double_t thc_ = acos(1.0/fNopt) - 0.1*cos(phic);
  return thc0(thc_, phic, nopt, beta, nnz);
}

//[R.K.03/2017] unused function
// double Chi2(const double *xx )
//{
// const double nopt = xx[0];
// const double beta = xx[1];
// const double nnz  = xx[2];
// double Chi2_ = 0;
// size_t n = fi.size();
// for(size_t i=0;i<n;i++) {
// double chi = (ti.at(i) - thc(fi.at(i),nopt,beta,nnz))/0.05;
// Chi2_ += chi*chi;
//}
// return Chi2_;
//}

//[R.K.03/2017] unused function
// int Minimizer(const char * minName = "Minuit2",
// const char *algoName = "" ,
// int randomSeed = -1)
//{
//// create minimizer giving a name and a name (optionally) for the specific
//// algorithm
//// possible choices are:
////     minName                  algoName
//// Minuit /Minuit2             Migrad, Simplex,Combined,Scan  (default is Migrad)
////  Minuit2                     Fumili2
////  Fumili
////  GSLMultiMin                ConjugateFR, ConjugatePR, BFGS,
////                              BFGS2, SteepestDescent
////  GSLMultiFit
////   GSLSimAn
////   Genetic
// ROOT::Math::Minimizer* min =
// ROOT::Math::Factory::CreateMinimizer(minName, algoName);

//// set tolerance , etc...
// min->SetMaxFunctionCalls(1000000); // for Minuit/Minuit2
// min->SetMaxIterations(10000);  // for GSL
// min->SetTolerance(1e-12);
// min->SetPrintLevel(0);

//// create funciton wrapper for minmizer
//// a IMultiGenFunction type
// ROOT::Math::Functor f(&Chi2,3);
// double step[3] = {0.00,0.001,0.00};

//// starting point

// double variable[3] = { nopt_, beta_, nnz_ };

// min->SetFunction(f);

//// Set the free variables to be minimized!
// min->SetVariable(0,"nopt",variable[0], step[0]);
// min->SetVariable(1,"beta",variable[1], step[1]);
// min->SetVariable(2,"nnz",variable[2], step[2]);

//// do the minimization
// min->Minimize();

// const double *xs = min->X();
// const double *dxs = min->Errors();

// nopt_ = xs[0];
// beta_ = xs[1];
// nnz_  = xs[2];
// dnopt_ = dxs[0];
// dbeta_ = dxs[1];
// dnnz_  = dxs[2];
// chi2_  = min->MinValue();

// return 0;
//}

//---------------------------------------------------------------------------

double Chi2_v1(const double *xx)
{
  const double nopt = nopt_;
  const double beta = xx[0];
  const double nnz = nnz_;
  double Chi2_ = 0;
  size_t n = fi.size();
  for (size_t i = 0; i < n; i++) {
    double chi = (ti.at(i) - thc(fi.at(i), nopt, beta, nnz)) / 0.0025;
    Chi2_ += chi * chi;
  }
  return Chi2_;
}

int Minimizer_v1(const char *minName = "Minuit2", const char *algoName = "")
//, int randomSeed = -1)  //  //[R.K.03/2017] unused variable(s)
{
  // create minimizer giving a name and a name (optionally) for the specific
  // algorithm
  // possible choices are:
  //     minName                  algoName
  // Minuit /Minuit2             Migrad, Simplex,Combined,Scan  (default is Migrad)
  //  Minuit2                     Fumili2
  //  Fumili
  //  GSLMultiMin                ConjugateFR, ConjugatePR, BFGS,
  //                              BFGS2, SteepestDescent
  //  GSLMultiFit
  //   GSLSimAn
  //   Genetic
  ROOT::Math::Minimizer *min = ROOT::Math::Factory::CreateMinimizer(minName, algoName);

  // set tolerance , etc...
  min->SetMaxFunctionCalls(1000000); // for Minuit/Minuit2
  min->SetMaxIterations(10000);      // for GSL
  min->SetTolerance(1e-12);
  min->SetPrintLevel(0);

  // create funciton wrapper for minmizer
  // a IMultiGenFunction type
  ROOT::Math::Functor f(&Chi2_v1, 1);
  min->SetFunction(f);

  // Set the beta variable to be minimized!
  min->SetVariable(0, "beta", beta_, 0.001);

  // do the minimization
  min->Minimize();

  const double *xs = min->X();
  const double *dxs = min->Errors();

  beta_ = xs[0];
  dbeta_ = dxs[0];
  chi2_ = min->MinValue();

  return 0;
}

//---------------------------------------------------------------------------

TMatrixT<double> gInvCovMatr;
PndRichBarPoint *track;
std::vector<PndRichPhoton> photons;
double xTrack, yTrack, zTrack, tTrack, fTrack;
double xTrack_, yTrack_, tTrack_, fTrack_;
double dxTrack_, dyTrack_, dtTrack_, dfTrack_;
TVector3 dir_;

double Chi2_v2(const double *xx)
{
  const double nopt = nopt_;
  const double beta = xx[0];
  const double nnz = nnz_;
  TVectorT<double> dTrackPars(4);
  dTrackPars[0] = xx[1] - xTrack;
  dTrackPars[1] = xx[2] - yTrack;
  dTrackPars[2] = xx[3] - tTrack;
  dTrackPars[3] = xx[4] - fTrack;
  double Chi2_ = dTrackPars * (gInvCovMatr * dTrackPars);
  size_t n = it.size();
  for (size_t i = 0; i < n; i++) {
    dir_.SetMagThetaPhi(1.0, xx[3], xx[4]);
    track->SetMomentum0(dir_);
    track->SetPosition0(TVector3(xx[1], xx[2], zTrack));
    double theta = photons[it.at(i)].GetTheta();
    double phi = photons[it.at(i)].GetPhi();
    double chi = (theta - thc(phi, nopt, beta, nnz)) / 0.0025;
    Chi2_ += chi * chi;
  }

  return Chi2_;
}

int Minimizer_v2(const char *minName = "Minuit2", const char *algoName = "")
//, int randomSeed = -1) //  //[R.K.03/2017] unused variable(s)
{
  // create minimizer giving a name and a name (optionally) for the specific
  // algorithm
  // possible choices are:
  //     minName                  algoName
  // Minuit /Minuit2             Migrad, Simplex,Combined,Scan  (default is Migrad)
  //  Minuit2                     Fumili2
  //  Fumili
  //  GSLMultiMin                ConjugateFR, ConjugatePR, BFGS,
  //                              BFGS2, SteepestDescent
  //  GSLMultiFit
  //   GSLSimAn
  //   Genetic
  ROOT::Math::Minimizer *min = ROOT::Math::Factory::CreateMinimizer(minName, algoName);

  // set tolerance , etc...
  min->SetMaxFunctionCalls(1000000); // for Minuit/Minuit2
  min->SetMaxIterations(10000);      // for GSL
  min->SetTolerance(1e-12);
  min->SetPrintLevel(0);

  // create funciton wrapper for minmizer
  // a IMultiGenFunction type
  ROOT::Math::Functor f(&Chi2_v2, 5);
  min->SetFunction(f);

  // Set the beta variable to be minimized!
  min->SetVariable(0, "beta", beta_, 0.001);
  min->SetVariable(1, "xt", xTrack_, dxTrack_);
  min->SetVariable(2, "yt", yTrack_, dyTrack_);
  min->SetVariable(3, "theta", tTrack_, dtTrack_);
  min->SetVariable(4, "phi", fTrack_, dfTrack_);

  // do the minimization
  min->Minimize();

  const double *xs = min->X();
  const double *dxs = min->Errors();

  beta_ = xs[0];
  xTrack_ = xs[1];
  yTrack_ = xs[2];
  tTrack_ = xs[3];
  fTrack_ = xs[4];
  dbeta_ = dxs[0];
  dxTrack_ = dxs[1];
  dyTrack_ = dxs[2];
  dtTrack_ = dxs[3];
  dfTrack_ = dxs[4];
  chi2_ = min->MinValue();

  return 0;
}

//------------------------------------------------------------------
} // namespace

ClassImp(PndRichReco)

  //___________________________________________________________
  PndRichReco::~PndRichReco()
{
  //
  FairRootManager *fManager = FairRootManager::Instance();
  fManager->Write();
}

// -----   Default constructor   -------------------------------------------
PndRichReco::PndRichReco()
  : fRichPDHit(0), fGeo(nullptr), fEvent(0), fGeoVersion(0), fGeoVersionMirr(0), fParticleID(0), fMirrorLength(0.), fTrackPosition(0., 0., 0.), fTrackDirection(0., 0., 0.),
    fMirrSegs(), fPhDetAngle(0.), fZamid(0.), gResVect(), gRotMatr()
{
  fGeoVersion = 313;
  Init(); // init geometry parameters
  Register();

  //-----------------------------------------------------
}

// -----   Default constructor   -------------------------------------------
PndRichReco::PndRichReco(UInt_t version)
  : fRichPDHit(0), fGeo(nullptr), fEvent(0), fGeoVersion(version), fGeoVersionMirr(0), fParticleID(0), fMirrorLength(0.), fTrackPosition(0., 0., 0.), fTrackDirection(0., 0., 0.),
    fMirrSegs(), fPhDetAngle(0.), fZamid(0.), gResVect(), gRotMatr()
{
  Init(); // init geometry parameters
  Register();

  //-----------------------------------------------------
}

void PndRichReco::Init()
{
  size_t nlayers = (fGeoVersion % 1000) / 100;
  fGeoVersionMirr = fGeoVersion % 100;
  nlayers = nlayers ? nlayers : 3;

  FairRootManager *fManager = FairRootManager::Instance();
  fGeo = new PndRichGeo();
  fGeo->init(fGeoVersion);
  fRichPDHit = dynamic_cast<TClonesArray *>(fManager->GetObject("RichPDHit"));
  //
  TVector3 richOffset = fGeo->richOffset();
  TVector3 aerogelOffset = fGeo->aerogelOffset();
  TVector3 aerogelSize = fGeo->aerogelSize();
  Double_t zain = richOffset.Z() + aerogelOffset.Z();
  fZamid = zain + 0.5 * aerogelSize.Z(); // midle position of the point of cherenkov photons emission
  // fZamid = zain + 0.69075*aerogelSize.Z(); // optimal position of the point of cherenkov photons emission
  //
  fPhDetAngle = fGeo->phDetAngle();
  std::vector<Double_t> flatMirrorY = fGeo->flatMirrorYGlob();
  std::vector<Double_t> flatMirrorZ = fGeo->flatMirrorZGlob();
  //
  std::vector<Double_t> phDetY = fGeo->phDetY();
  std::vector<Double_t> phDetZ = fGeo->phDetZ();
  TVector3 pPhDet = TVector3(0, phDetY[0], phDetZ[0] + zain);
  TVector3 nPhDet = TVector3(0, phDetZ[0] - phDetZ[1], phDetY[1] - phDetY[0]).Unit();
  Int_t ns = flatMirrorY.size() - 2;
  TVector3 pnt, point;
  for (int i = ns; i >= 0; i--) {
    pnt = TVector3(0, flatMirrorY[i], flatMirrorZ[i]);
    point = pnt - 2 * nPhDet * ((pnt - pPhDet) * nPhDet);
    flatMirrorY.push_back(point.Y());
    flatMirrorZ.push_back(point.Z());
  }
  //
  fMirrorLength = fGeo->mirrorLength();
  for (UInt_t i = 0; i < flatMirrorY.size() - 1; i++) {
    PndRichMirrorSegment mirrSeg;
    // middle point on the mirror segment
    Double_t xm = 0;
    Double_t ym = (flatMirrorY[i + 1] + flatMirrorY[i]) / 2;
    Double_t zm = (flatMirrorZ[i + 1] + flatMirrorZ[i]) / 2;
    mirrSeg.SetPoint(TVector3(xm, ym, zm));
    // size of the flat mirror segment
    Double_t dxm = 3 * fMirrorLength / 2;
    Double_t dym = std::fabs(flatMirrorY[i + 1] - flatMirrorY[i]) / 2;
    Double_t dzm = std::fabs(flatMirrorZ[i + 1] - flatMirrorZ[i]) / 2;
    mirrSeg.SetDimensions(TVector3(dxm, dym, dzm));
    // normal vector of the flat mirror segment
    TVector3 norm = (TVector3(-1, 0, 0).Cross(TVector3(0, flatMirrorY[i + 1] - flatMirrorY[i], flatMirrorZ[i + 1] - flatMirrorZ[i]))).Unit();
    mirrSeg.SetNormal(norm.Unit());
    fMirrSegs.push_back(mirrSeg);
  }
  for (UInt_t i = 0; i < flatMirrorY.size() - 1; i++) {
    PndRichMirrorSegment mirrSeg;
    // middle point on the mirror segment
    Double_t xm = 0;
    Double_t ym = -(flatMirrorY[i + 1] + flatMirrorY[i]) / 2;
    Double_t zm = (flatMirrorZ[i + 1] + flatMirrorZ[i]) / 2;
    mirrSeg.SetPoint(TVector3(xm, ym, zm));
    // size of the flat mirror segment
    Double_t dxm = 3 * fMirrorLength / 2;
    Double_t dym = std::fabs(flatMirrorY[i + 1] - flatMirrorY[i]) / 2;
    Double_t dzm = std::fabs(flatMirrorZ[i + 1] - flatMirrorZ[i]) / 2;
    mirrSeg.SetDimensions(TVector3(dxm, dym, dzm));
    // normal vector of the flat mirror segment
    TVector3 norm = (TVector3(-1, 0, 0).Cross(TVector3(0, -flatMirrorY[i + 1] + flatMirrorY[i], flatMirrorZ[i + 1] - flatMirrorZ[i]))).Unit();
    mirrSeg.SetNormal(norm.Unit());
    fMirrSegs.push_back(mirrSeg);
  }
  fEvent = 0;

  // covariance matrix of track resolutions (for test)
  double scal = TMath::Pi() / 180;
  double sigx = 0.1, sigy = 0.2, rhoxy = 0.3;
  double sigt = 0.3 * scal, sigf = 0.5 * scal, rhotf = -0.5;
  TMatrixT<double> covMatr(4, 4);

  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
      covMatr[i][j] = 0;
  covMatr[0][0] = sigx * sigx;
  covMatr[0][1] = sigx * sigy * rhoxy;
  covMatr[1][0] = covMatr[0][1];
  covMatr[1][1] = sigy * sigy;
  covMatr[2][2] = sigt * sigt;
  covMatr[2][3] = sigt * sigf * rhotf;
  covMatr[3][2] = covMatr[2][3];
  covMatr[3][3] = sigf * sigf;

  gResVect.ResizeTo(4);
  gRotMatr.ResizeTo(4, 4);
  gRotMatr = covMatr.EigenVectors(gResVect);
  covMatr.Print();
  gRotMatr.Print();
  gResVect.Print();

  gInvCovMatr.ResizeTo(4, 4);
  gInvCovMatr = covMatr.Invert();
}

//______________________________________________________
void PndRichReco::RichFullReconstruction(TVector3 pos0, TVector3 dir0, Float_t ts, Float_t &chi2, Float_t &chTh, Float_t &dChTh, Int_t &nph)
{

  if (fRichPDHit->GetEntriesFast() == 0)
    return;

  // randomization with correlation of ideal track parameters (for test)
  TVectorT<double> evt(4), evto(4);
  for (int i = 0; i < 4; i++)
    evt[i] = 0 * gRandom->Gaus(0, sqrt(gResVect[i]));
  evto = gRotMatr * evt;

  // finding position of the track in middle depth of the aerogel bar
  Double_t tam = (fZamid - pos0.Z()) / dir0.Z();
  TVector3 pos = pos0 + tam * dir0 + TVector3(evto[0], evto[1], 0.);

  TVector3 dir(1, 0, 0);
  dir.SetTheta(dir0.Theta() + evto[2]);
  dir.SetPhi(dir0.Phi() + evto[3]);
  dir = dir.Unit();

  fTrackTime = ts;

  track = new PndRichBarPoint(pos, dir, ts);
  xTrack = pos.X();
  yTrack = pos.Y();
  zTrack = pos.Z();
  tTrack = dir.Theta();
  fTrack = dir.Phi();

  // flat mirror
  if (((fGeoVersionMirr >= 11) && (fGeoVersionMirr <= 19)) || ((fGeoVersionMirr >= 21) && (fGeoVersionMirr <= 29))) {
    // std::vector<PndRichPhoton> photons;
    beta_ = 1;
    dbeta_ = 0;
    photons = CherenkovPhotonListFlat(track);
    if (photons.size()) {
      Double_t nnz = dir.Z();
      Double_t nopt = 1.05;
      // beta peak finding
      Double_t beta = BetaPeakFinding(photons, nopt, nnz);
      // hits selection
      HitSelection(it, fi, ti, photons, beta, nopt, nnz, 0.03);
      //
      nopt_ = nopt;
      beta_ = beta;
      nnz_ = nnz;
      Minimizer_v1();

      // hits selection
      HitSelection(it, fi, ti, photons, beta_, nopt_, nnz_, 0.01);
      Minimizer_v1();

      // fit with track parameters
      xTrack_ = xTrack;
      yTrack_ = yTrack;
      tTrack_ = tTrack;
      fTrack_ = fTrack;
      dxTrack_ = 0.001;
      dyTrack_ = 0.001;
      dtTrack_ = 0.001;
      dfTrack_ = 0.001;
      // Minimizer_v2();
    }

    chi2 = chi2_;
    chTh = beta_;
    dChTh = dbeta_;
    nph = fi.size();
  }
  fEvent++;
}

std::vector<double> PndRichReco::GetPhis()
{
  return fi;
};

std::vector<double> PndRichReco::GetThetas()
{
  return ti;
};

std::vector<double> PndRichReco::GetDThetas()
{
  size_t n = fi.size();
  std::vector<double> dth(n);
  for (size_t i = 0; i < n; i++)
    dth.at(i) = ti.at(i) - thc(fi.at(i), nopt_, beta_, nnz_);
  return dth;
}

double PndRichReco::BetaPeakFinding(std::vector<PndRichPhoton> photons, Double_t nopt, Double_t nnz)
{
  // beta peak finding
  Int_t nch = 60;
  Double_t bmin = 1.0 / nopt;
  Double_t bmax = 1 + (1 - bmin) * 0.2;
  // Double_t dbeta = (bmax-bmin)/nch; //[R.K. 01/2017] unused variable?
  Double_t beta = 1.0;
  // Double_t thcmin = 0; //[R.K. 01/2017] unused variable?
  // Double_t thcmax = std::acos(bmin); //[R.K. 01/2017] unused variable?
  Double_t bim = 0;
  Int_t ibm = -1;
  Double_t dtm = 3; // 0.5;
  int nph = 0;
  for (size_t j = 0; j < 2; j++) {
    std::vector<Double_t> bi(nch, 0);
    bim = 0;
    ibm = -1;
    nph = 0;
    for (UInt_t i = 0; i < photons.size(); i++) {
      Double_t thcc = photons.at(i).GetTheta();
      Double_t phcc = photons.at(i).GetPhi();
      Double_t thcm = thc(phcc, nopt, beta, nnz);
      Double_t b = bmin + thcc * (beta - bmin) / thcm;
      Int_t ib = (b - bmin) / (bmax - bmin) * nch;
      Double_t dt = photons.at(i).GetTime() - fTrackTime;
      if ((ib >= 0) && (ib < nch) && std::fabs(dt) < dtm) {
        nph++;
        bi.at(ib)++;
        if (bim < bi.at(ib)) {
          bim = bi.at(ib);
          ibm = ib;
        }
      }
    }
    beta = bmin + ibm * (bmax - bmin) / nch;
  }
  return beta;
}

void PndRichReco::HitSelection(std::vector<size_t> &it, std::vector<double> &ph, std::vector<double> &th, std::vector<PndRichPhoton> photons, Double_t beta, Double_t nopt,
                               Double_t nnz, Double_t dthc)
{
  //
  it.resize(photons.size());
  ph.resize(photons.size());
  th.resize(photons.size());
  Double_t hitx, hity = 10;
  Double_t thccc, phccc, dthccc = 10;
  // Double_t dthc = 0.03;
  size_t itccc;
  Double_t dt = 0;
  Int_t ind = 0;
  Double_t dtm = 3;
  for (UInt_t i = 0; i < photons.size(); i++) {
    TVector3 hit = photons.at(i).GetHitPos();
    // th.at(ind) = photons.at(i).GetTheta();
    // ph.at(ind) = photons.at(i).GetPhi();
    // ind++;
    if (((hit.X() != hitx) || (hit.Y() != hity)) && (hity != 10)) {
      Double_t thcm = thc(phccc, nopt, beta, nnz);
      if (std::fabs(thccc - thcm) < dthc && std::fabs(dt) < dtm) {
        it.at(ind) = itccc;
        th.at(ind) = thccc;
        ph.at(ind) = phccc;
        ind++;
      }
      dthccc = 10;
    }
    Double_t thcc = photons.at(i).GetTheta();
    Double_t phcc = photons.at(i).GetPhi();
    Double_t thcm = thc(phcc, nopt, beta, nnz);
    Double_t dthccl = std::fabs(thcc - thcm);
    if (dthccl < dthccc) {
      itccc = i;
      dthccc = dthccl;
      thccc = thcc;
      phccc = phcc;
      dt = photons.at(i).GetTime() - fTrackTime;
    }
    hitx = hit.X();
    hity = hit.Y();
  }
  Double_t thcm = thc(phccc, nopt, beta, nnz);
  if (std::fabs(thccc - thcm) < dthc && std::fabs(dt) < dtm) {
    it.at(ind) = itccc;
    th.at(ind) = thccc;
    ph.at(ind) = phccc;
    ind++;
  }
  //
  /*
  ph.resize(photons.size());
  th.resize(photons.size());
  Double_t dthc = 0.02;
  Int_t ind = 0;
  for(UInt_t i=0; i<photons.size(); i++) {
     Double_t thcc = photons.at(i).GetTheta();
     Double_t phcc = photons.at(i).GetPhi();
     Double_t thcm = thc(phcc,nopt,beta,nnz);
     if (std::fabs(thcc-thcm)<dthc) {
        th.at(ind) = thcc;
        ph.at(ind) = phcc;
        ind++;
     }
  }*/
  it.resize(ind);
  ph.resize(ind);
  th.resize(ind);
}

std::vector<PndRichPhoton> PndRichReco::CherenkovPhotonListFlat(PndRichBarPoint *track)
{
  size_t nHits = fRichPDHit->GetEntriesFast();
  PndRichPDHit *richPDHit = nullptr;
  std::vector<PndRichPhoton> ph;
  for (size_t ih = 0; ih < nHits; ih++) {
    //
    richPDHit = (PndRichPDHit *)fRichPDHit->At(ih);
    TVector3 hit = richPDHit->GetPosition();
    Double_t hitTime = richPDHit->GetTime();
    AppendFlatMirrorReflections(ph, hit, hitTime, track);
    //
    if ((fGeoVersionMirr >= 21) && (fGeoVersionMirr <= 29)) {
      Double_t x_hit = hit.X();
      // left mirror
      TVector3 hitLeft = richPDHit->GetPosition();
      hitLeft.SetX(fMirrorLength - x_hit);
      AppendFlatMirrorReflections(ph, hitLeft, hitTime, track);
      // right mirror
      TVector3 hitRight = richPDHit->GetPosition();
      hitRight.SetX(-fMirrorLength - x_hit);
      AppendFlatMirrorReflections(ph, hitRight, hitTime, track);
    }
  }
  return ph;
}

void PndRichReco::AppendFlatMirrorReflections(std::vector<PndRichPhoton> &ph, TVector3 hit, Double_t hitTime, PndRichBarPoint *track)
{
  PndRichPhoton phot;
  phot.SetHitPos(hit);
  phot.SetHitTime(hitTime);
  phot.SetTrack(track);
  // phot.SetTrackPos(pos);
  // phot.SetTrackDir(dir);
  // phot.SetDTime(time);
  vector<PndRichMirrorSegment *> segs;
  // direct photon tracks
  if (phot.TrackCalc())
    ph.push_back(phot);
  // single reflection photon tracks
  for (UInt_t i = 0; i < fMirrSegs.size(); i++) {
    segs.clear();
    segs.push_back(&fMirrSegs[i]);
    phot.SetMirror(segs);
    if (phot.TrackCalc())
      ph.push_back(phot);
  }
  // double reflection photon tracks
  for (UInt_t i = 0; i < fMirrSegs.size() - 1; i++) {
    for (UInt_t j = i + 1; j < fMirrSegs.size(); j++) {
      segs.clear();
      segs.push_back(&fMirrSegs[i]);
      segs.push_back(&fMirrSegs[j]);
      phot.SetMirror(segs);
      if (phot.TrackCalc())
        ph.push_back(phot);
    }
  }
}

void PndRichReco::Register()
{

  /** This will create a branch in the output tree called
      PndRichPDPoint, setting the last parameter to kFALSE means:
      this collection will not be written to the file, it will exist
      only during the simulation.
  */
}

// -------------------------------------------------------------------------
