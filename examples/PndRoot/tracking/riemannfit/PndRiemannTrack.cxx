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
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndRiemannTrack
//      see PndRiemannTrack.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndRiemannTrack.h"

// C/C++ Headers ----------------------
#include <iostream>
#include <assert.h>
#include <cmath>
#include <ostream>

// Collaborating Class Headers --------
#include "PndRiemannHit.h"
#include "FairTrackParP.h"
#include "FairRootManager.h"
#include "TGraph.h"
#include "TMath.h"
#include "TMatrixTSym.h"
#include "TF1.h"
// Class Member definitions -----------

void MatrixOutput(TMatrixD mat)
{
  for (int row = 0; row < mat.GetNrows(); row++) {
    for (int col = 0; col < mat.GetNcols(); col++) {
      std::cout << mat[row][col] << " ";
    }
    std::cout << std::endl;
  }
}

ClassImp(PndRiemannTrack);

PndRiemannTrack::PndRiemannTrack()
  : fn(3), fav(3), fc(0), fm(0), ft(0), fmError(0), ftError(0), fChi2(0), fcovPlane(4, 4), fjacRXY(3, 4), fcovRXY(3, 3), fVerbose(0), fFitDone(false), fSZFitDone(false),
    fErrorCalcDone(false), fweight(0), ftrefit(false), fVertexCut(0.5), fStartAlpha(0), fStopAlpha(0)
{
  fHits.clear();
}

PndRiemannTrack::PndRiemannTrack(PndTrackCand *trackCand)
  : fn(3), fav(3), fc(0), fm(0), ft(0), fmError(0), ftError(0), fChi2(0), fcovPlane(4, 4), fjacRXY(3, 4), fcovRXY(3, 3), fVerbose(0), fFitDone(false), fSZFitDone(false),
    fErrorCalcDone(false), fweight(0), ftrefit(false), fVertexCut(0.5), fStartAlpha(0), fStopAlpha(0)
{
  fHits.clear();
  addPndTrackCand(trackCand);
}

void PndRiemannTrack::addPndTrackCand(PndTrackCand *trackCand)
{
  FairRootManager *ioman = FairRootManager::Instance();
  std::set<FairLink> linksToHits = trackCand->GetLinks();
  for (std::set<FairLink>::iterator iter = linksToHits.begin(); iter != linksToHits.end(); iter++) {
    PndRiemannHit riemannHit((FairHit *)ioman->GetCloneOfLinkData(*iter));
    addHit(riemannHit);
  }
}

PndRiemannTrack::~PndRiemannTrack() {}

void PndRiemannTrack::init(double x0_, double y0_, double R_, double mydip, double)
{ // z0 //[R.K.03/2017] unused variable(s)

  fn[1] = TMath::Sqrt(4 * y0_ * y0_ / (4 * y0_ * y0_ + 4 * x0_ * x0_ + 1));
  fn[0] = x0_ / y0_ * fn[1];
  fn[2] = TMath::Sqrt(1 - fn[0] * fn[0] - fn[1] * fn[1]);
  double fn2 = fn[2] * fn[2];
  fc = fn2 * 4 * R_ * R_ - 1 + fn2 / (-4 * fn[2]);

  fm = tan(acos(mydip));
}

void PndRiemannTrack::addHit(PndRiemannHit &hit)
{
  //	SetVerbose(3);
  // int nbefore=fHits.size(); //[R.K. 01/2017] unused variable?
  fHits.push_back(hit);
  //  std::cout << "fweight before addHit: " << fweight << std::endl;
  // ADDED by ME//
  if (ftrefit) {
    fav *= fweight;
    ftrefit = false;
  }

  if (fVerbose > 1)
    std::cout << "-I- PndRiemannTrack::addHit " << fHits.size() - 1 << ": " << hit.x().X() << " " << hit.x().Y() << " " << hit.sigmaX() << " " << hit.sigmaY() << " "
              << hit.sigmaXY() << std::endl;
  // fav*=(double)nbefore;
  fav[0] += hit.x().X() / (hit.sigmaXY() * hit.sigmaXY());
  fav[1] += hit.x().Y() / (hit.sigmaXY() * hit.sigmaXY());
  ;
  fav[2] += hit.x().Z() / (hit.sigmaXY() * hit.sigmaXY());
  ;
  // fav*=1./(double)(nbefore+1);
  fweight += 1 / (hit.sigmaXY() * hit.sigmaXY());

  //  std::cout << "fweight after addHit: " << fweight << std::endl;
  fFitDone = false;
  fSZFitDone = false;
  fErrorCalcDone = false;
}

double PndRiemannTrack::dist(PndRiemannHit *hit)
{
  double d2 = fc;
  d2 += hit->x().X() * fn[0];
  d2 += hit->x().Y() * fn[1];
  d2 += hit->x().Z() * fn[2];

  if (fVerbose > 1) {
    std::cout << "PndRiemannTrack::dist: c " << fc << " n: " << fn[0] << "/" << fn[1] << "/" << fn[2] << " hit: " << hit->x().X() << "/" << hit->x().Y() << "/" << hit->x().Z()
              << " Dist: " << d2 << std::endl;
  }
  return d2;
}

double PndRiemannTrack::distError(PndRiemannHit *hit)
{
  // for the time being the plane errors are ignored!
  return TMath::Sqrt(TMath::Power(hit->sigmaX(), 2) + TMath::Power(hit->sigmaY(), 2) + TMath::Power(hit->sigmaW(), 2));
}

double PndRiemannTrack::distCircle(PndRiemannHit *hit)
{
  TVector2 pos;
  TVectorD origin(2);
  origin = orig();
  TVector2 diff;

  pos.Set(hit->x().x(), hit->x().y());
  diff.Set(pos.X() - origin[0], pos.Y() - origin[1]);

  return (diff.Mod() - r());
}

double PndRiemannTrack::ChiSquareDistCircle()
{
  double sum = 0;
  for (size_t i = 0; i < fHits.size(); i++) {
    double distance = distCircle(&(fHits[i]));
    sum += TMath::Power(distance / fHits[i].sigmaXY(), 2);
  }
  return sum;
}

void PndRiemannTrack::refit(bool withErrorCalc)
{

  TMatrixT<double> my_av(3, 1);
  if (fVerbose > 1)
    std::cout << "fweight: " << fweight << std::endl;
  // fav *= TMath::Power(fweight,-1); //TS
  /////////////<-------------------------
  if (!ftrefit) {
    fav *= TMath::Power(fweight, -1); // TS
  }
  ////////////
  my_av[0][0] = fav[0]; // *fweight; //TS *fweight added
  my_av[1][0] = fav[1]; // *fweight;
  my_av[2][0] = fav[2]; // *fweight;

  if (fVerbose > 1)
    std::cout << "fav: " << fav[0] << " " << fav[1] << " " << fav[2] << std::endl;

  TMatrixD sampleCov(3, 3);

  int nh = fHits.size();
  for (int i = 0; i < nh; ++i) {
    TMatrixD h(3, 1);
    h[0][0] = fHits[i].x().X();
    h[1][0] = fHits[i].x().Y();
    h[2][0] = fHits[i].x().Z();
    TMatrixD d(3, 1);
    d = h - my_av;
    TMatrixD dt(TMatrixD::kTransposed, d);
    TMatrixD ddt(d, TMatrixD::kMult, dt);
    if (fHits[i].sigmaXY() != 0)
      ddt *= 1 / (fHits[i].sigmaXY() * fHits[i].sigmaXY()); // TS
    else
      LOG(error) << " PndRiemannTrack::refit sigmaXY() == 0";
    sampleCov += ddt;
  }

  if (nh != 0)
    sampleCov *= 1. / (double)nh;
  else {
    LOG(error) << " PndRiemannTrack::refit nh == 0";
    return;
  }

  if (fVerbose > 1)
    std::cout << "sampleCov: " << std::endl;
  if (fVerbose > 1)
    MatrixOutput(sampleCov);

  TVectorD eigenValues(3);
  TMatrixD eigenVec = sampleCov.EigenVectors(eigenValues);

  // find smallest eigenvalue
  //  double val=eigenValues[0]; int g=0;
  //  for(int k=1;k<3;++k){
  //    if(eigenValues[k]<val){
  //      val=eigenValues[k];
  //      g=k;
  //    }
  //  }

  int g = 0;
  double smallestDistance = -1;
  int smallestVec = 0;
  TVectorD correctN(3);
  double correctC;
  double val;
  for (int i = 0; i < 3; i++) {
    g = i;
    fn = TMatrixDColumn(eigenVec, g);

    if (fVerbose > 1)
      std::cout << "eigenVec: " << std::endl;
    if (fVerbose > 1)
      MatrixOutput(eigenVec);

    if (fVerbose > 1)
      std::cout << "eigenValues: ";
    for (int j = 0; j < 3; j++) {
      if (fVerbose > 1)
        std::cout << eigenValues[j] << " ";
    }
    if (fVerbose > 1)
      std::cout << std::endl;

    double norm = 1;
    if (fn.Norm2Sqr() != 0)
      norm = 1. / TMath::Sqrt(fn.Norm2Sqr());

    fn *= norm;
    fc = -1. * fn * fav;
    if (smallestDistance < 0) {
      smallestDistance = ChiSquareDistCircle();
      smallestVec = i;
      correctN = fn;
      correctC = fc;
      val = eigenValues[i];
    } else if (smallestDistance > ChiSquareDistCircle()) {
      smallestDistance = ChiSquareDistCircle();
      smallestVec = i;
      correctN = fn;
      correctC = fc;
      val = eigenValues[i];
    }

    if (fVerbose > 1)
      std::cout << "fn: " << fn[0] << " " << fn[1] << " " << fn[2] << std::endl;
    if (fVerbose > 1)
      std::cout << "fc: " << fc << std::endl;
    if (fVerbose > 1) {
      TVectorD origin = orig();
      std::cout << "Origin: " << origin[0] << " " << origin[1] << std::endl;
    }
    if (fVerbose > 1)
      std::cout << "r: " << r() << std::endl;
    if (fVerbose > 1)
      std::cout << "ChiSquareCircleDistance: " << ChiSquareDistCircle() << std::endl;
  }

  g = smallestVec;
  fn = correctN;
  fc = correctC;
  //------------ Calculation of the full covariance matrix -----------

  // 1. Calculation of the covarianz matrix of the normal vector
  if (withErrorCalc) {
    TMatrixD CovNorm(3, 3);

    for (int i = 0; i < 3; i++) {
      if (i != g) {
        TMatrixD res = eigenVec.GetSub(0, 2, i, i) * eigenVec.GetSub(0, 2, i, i).T();
        CovNorm += res * (val * eigenValues[i] / TMath::Power(val - eigenValues[i], 2));
      }
    }
    //  if (fHits.size() > 5)
    //	  CovNorm *= TMath::Power(fHits.size()-5,-1);  //-5 is very strange. According to the paper this value has to be fixed with simulation???
    //  else
    CovNorm *= TMath::Power(fHits.size(), -1);
    if (fVerbose > 1)
      std::cout << "CovNorm: " << std::endl;
    if (fVerbose > 1)
      MatrixOutput(CovNorm);

    // 2. Calculation of the covarianz matrix of fav

    TMatrixD covAv(3, 3);
    for (size_t i = 0; i < fHits.size(); i++) {
      covAv[0][0] += fHits[i].covX(0, 0) / (TMath::Power(fHits[i].sigmaXY(), 4));
      covAv[1][1] += fHits[i].covX(1, 1) / (TMath::Power(fHits[i].sigmaXY(), 4));
      covAv[1][0] += fHits[i].covX(1, 0) / (TMath::Power(fHits[i].sigmaXY(), 4));
      covAv[2][0] += 2 * (fHits[i].covX(0, 0) * fHits[i].x().X() + fHits[i].covX(1, 0) * fHits[i].x().Y()) / (TMath::Power(fHits[i].sigmaXY(), 4));
      covAv[2][1] += 2 * (fHits[i].covX(1, 1) * fHits[i].x().Y() + fHits[i].covX(1, 0) * fHits[i].x().X()) / (TMath::Power(fHits[i].sigmaXY(), 4));
      covAv[2][2] += 4 *
                     (fHits[i].covX(0, 0) * fHits[i].x().X() * fHits[i].x().X() + 2 * fHits[i].covX(1, 0) * fHits[i].x().X() * fHits[i].x().Y() +
                      fHits[i].covX(1, 1) * fHits[i].x().Y() * fHits[i].x().Y()) /
                     (TMath::Power(fHits[i].sigmaXY(), 4));
    }
    covAv[0][1] = covAv[1][0];
    covAv[0][2] = covAv[2][0];
    covAv[1][2] = covAv[2][1];

    covAv *= TMath::Power(fweight, -2);

    if (fVerbose > 1)
      std::cout << "covAv: " << std::endl;
    if (fVerbose > 1)
      MatrixOutput(covAv);

    // 3. Calculation of var(fc)

    double nCrn = (fn * (covAv * fn));
    double rCnr = (fav * (CovNorm * fav));

    TMatrixD CnCr(CovNorm, TMatrixD::kMult, covAv);
    double trCnCr = 0;
    for (int i = 0; i < CnCr.GetNcols(); i++) {
      trCnCr += CnCr[i][i];
    }
    if (fVerbose > 1)
      std::cout << "nCrn: " << nCrn << " rCnr: " << rCnr << " trCnCr: " << trCnCr << std::endl;
    double varc = nCrn + rCnr + trCnCr;

    TVectorD corr_cn = CovNorm * fav;
    corr_cn *= -1;

    // 4. Calculation of the covarianz matrix of c,n

    fcovPlane[0][0] = varc;
    fcovPlane[1][1] = CovNorm[0][0];
    fcovPlane[2][1] = CovNorm[1][0];
    fcovPlane[2][2] = CovNorm[1][1];
    fcovPlane[3][1] = CovNorm[2][0];
    fcovPlane[3][2] = CovNorm[2][1];
    fcovPlane[3][3] = CovNorm[2][2];
    fcovPlane[1][0] = corr_cn[0];
    fcovPlane[2][0] = corr_cn[1];
    fcovPlane[3][0] = corr_cn[2];
    /////////////////ADDED by me

    fcovPlane[1][2] = CovNorm[1][0];

    fcovPlane[1][3] = CovNorm[2][0];
    fcovPlane[2][3] = CovNorm[2][1];

    fcovPlane[0][1] = corr_cn[0];
    fcovPlane[0][2] = corr_cn[1];
    fcovPlane[0][3] = corr_cn[2];
    ///////////////////////////////

    // 5. Convert plane covariance in start parameter(r,x0,y0) covariances
    calcJacRXY();

    if (fVerbose > 1)
      std::cout << "jacRXY: " << std::endl;
    if (fVerbose > 1)
      MatrixOutput(fjacRXY);

    // fcovRXY = fjacRXY * fcovPlane * fjacRXY.T();

    TMatrixD temp(fjacRXY, TMatrixD::kMult, fcovPlane);
    if (fVerbose > 1)
      std::cout << "temp: " << temp.GetNrows() << "x" << temp.GetNcols() << std::endl;

    if (fVerbose > 1)
      std::cout << "temp: " << std::endl;
    if (fVerbose > 1)
      MatrixOutput(temp);

    fcovRXY = temp * fjacRXY.T(); // J * fcovPlane * J.T()

    if (fVerbose > 1)
      std::cout << "covPlane: " << std::endl;
    if (fVerbose > 1)
      MatrixOutput(fcovPlane);

    if (fVerbose > 1)
      std::cout << "covRXY: " << std::endl;
    if (fVerbose > 1)
      MatrixOutput(fcovRXY);
    fErrorCalcDone = true;
  }

  fFitDone = true;
  fSZFitDone = false;
  ftrefit = true;

  calcSForHits();

  sortHits();
  calcStartStopAlpha();
}

TVectorD PndRiemannTrack::orig() const
{
  TVectorD o(2);
  if (fn[2] == 0)
    return o;                                 // RK is that good to return here?
  double den = TMath::Power((2 * fn[2]), -1); // modified for paraboloid
  o[0] = -fn[0] * den;
  o[1] = -fn[1] * den;
  return o;
}

double PndRiemannTrack::r() const
{
  //  if(fc==0)return 0;
  // if(fabs(fc)>100)return 0;???????????????????????????
  double a = 2. * fn[2]; // modified for paraboloid
  // if(a==0){
  //  if (fVerbose > 0) std::cout<<"PndRiemannTrack:: a==0 cannot calc r! set r=1E4"<<std::endl;
  //  return 1.E-4;
  //}
  // if (fVerbose > 0) std::cout<<fn[0]<<"   "<<fn[1]<<"   "<<fc<<"   "<<a<<std::endl;
  double nom = 1 - fn[2] * fn[2] - 4 * fc * fn[2]; // modified for paraboloid
  if (nom < 0.0) {
    nom = 1.E-4;
    if (fVerbose > 1)
      std::cout << "PndRiemannTrack::nom<0! set r=1E-4!" << std::endl;
  }
  if (TMath::Abs(a) > 0)
    return sqrt(nom) / TMath::Abs(a);
  else
    return 0;
}

double PndRiemannTrack::dR()
{
  return TMath::Sqrt(fcovRXY[0][0]);
}

void PndRiemannTrack::szFit(bool withErrorCalc)
{
  if (fFitDone == false)
    refit(withErrorCalc);
  unsigned int num = getNumHits();
  if (fVerbose > 1)
    std::cout << "szFit() for " << num << " Points!" << std::endl;
  if (r() > 0) {

    TGraph g(num);
    // get s'es and zs
    int j = 0;
    for (unsigned int i = 0; i < num; ++i) {
      if (fVerbose > 1)
        if (fHits[i].hit() != nullptr)
          std::cout << "Point: " << i << ": " << fHits[i].hit()->GetEntryNr() << " ";
      if (fHits[i].hit() != 0 && fHits[i].hit()->GetEntryNr().GetType() == GetBranchId("STTHit")) {
        continue;
      }
      fHits[i].calcPosOnTrk(this);
      if (fVerbose > 1)
        std::cout << fHits[i].s() << " " << fHits[i].z() << std::endl;
      g.SetPoint(j, fHits[i].s(), fHits[i].z());
      j++;
    }
    if (j > 1) {
      g.Set(j);
      g.Fit("pol1", "Q0"); // << std::endl;
      TF1 *f = g.GetFunction("pol1");
      // std::cout << "f: " << f << std::endl;
      ft = f->GetParameter(0);
      fm = f->GetParameter(1);
      ftError = f->GetParError(0);
      fmError = f->GetParError(1);
      fChi2 = f->GetChisquare() / f->GetNDF();
      fSZFitDone = true;
    } else {
      ft = 0;
      fm = 0;
      ftError = 0;
      fmError = 0;
      fChi2 = -1;
      fSZFitDone = true;

      if (fVerbose > 0) {
        LOG(warn) << " PndRiemannTrack::calcSZ less than 2 points with z-Info: ";
        std::cout << *this << std::endl;
      }
    }
  } else {
    ft = 0;
    fm = 0;
    ftError = 0;
    fmError = 0;
    fChi2 = -1;
    fSZFitDone = true;

    if (fVerbose > 0) {
      LOG(warn) << " PndRiemannTrack::calcSZ r == 0: ";
      std::cout << *this << std::endl;
    }
  }
  if (fVerbose > 1)
    std::cout << "t, m: " << ft << " +/- " << ftError << " / " << fm << " +/- " << fmError << " Chi2: " << fChi2 << std::endl;
  return;
}

double PndRiemannTrack::calcChi2Plane()
{
  double chiSquare = 0;
  if (fFitDone == false)
    refit();
  if (r() > 0) {
    for (size_t i = 0; i < getNumHits(); i++) {
      PndRiemannHit *actualHit = getHit(i);
      //			chiSquare += TMath::Power((dist(actualHit)/actualHit->sigmaXY()),2);
      //			std::cout << "Dist: " << " "<< dist(actualHit) << std::endl;
      chiSquare += TMath::Power((dist(actualHit) / distError(actualHit)), 2);
    }
    if (getNumHits() > 3)
      chiSquare /= (getNumHits() - 3);
    else
      chiSquare = 0;
  }
  return chiSquare;
}

double PndRiemannTrack::calcSZChi2(PndRiemannHit *hit)
{
  // get s'es and zs
  TF1 *f = 0;
  if (fFitDone == false)
    refit();
  if (r() > 0) {
    unsigned int num = getNumHits();
    if (fVerbose > 1)
      std::cout << "szChi2Fit(hit) for " << num + 1 << " Points!" << std::endl;
    TGraph g(num + 1);
    int j = 0;
    for (unsigned int i = 0; i < num; ++i) {
      if (fHits[i].hit() != 0 && fHits[i].hit()->GetEntryNr().GetType() == GetBranchId("STTHit")) {
        continue;
      }
      fHits[i].calcPosOnTrk(this);
      if (fVerbose > 1) {

        std::cout << "Point: " << j << ": ";
        if (fHits[i].hit() != 0)
          std::cout << fHits[i].hit()->GetEntryNr() << " ";
        std::cout << fHits[i].s() << " " << fHits[i].z() << std::endl;
      }
      g.SetPoint(j, fHits[i].s(), fHits[i].z());
      j++;
    }
    if (fVerbose > 1)
      std::cout << "Additional hit: ";
    hit->calcPosOnTrk(this);
    if (fVerbose > 1)
      std::cout << hit->s() << " " << hit->z() << std::endl;
    g.SetPoint(j, hit->s(), hit->z());
    g.Set(j + 1);
    g.Fit("pol1", "Q0");
    f = g.GetFunction("pol1");

    //  fChi2 = chis;
    if (fVerbose > 1)
      std::cout << "t, m: " << f->GetParameter(0) << " +/- " << f->GetParError(0) << " / " << f->GetParameter(1) << " +/- " << f->GetParError(1) << "Chi2: " << f->GetChisquare()
                << std::endl;

    return f->GetChisquare() / f->GetNDF();
    //  delete(f);
  } else {
    LOG(warn) << " PndRiemannTrack::calcSZChi2 r == 0: ";
    std::cout << *this << std::endl;
    return -1;
  }
}

double PndRiemannTrack::calcZPosByS(double s)
{
  //	TVectorD o=orig();
  //	const PndRiemannHit* firstHit=getHit(0);
  //	assert(firstHit!=nullptr);
  //	TVector2 k(firstHit->x().X()-o[0],firstHit->x().Y()-o[1]);
  //	TVector2 l = k.Rotate(s/r());
  Double_t zCoord = s * m() + t();
  //	TVector3 result(l.X()+o[0], l.Y()+o[1], zCoord);

  //	if (fVerbose > 0) std::cout<< "PosByS: s:" << s << " Vector: " << result.x() << " " << result.y() << " " << result.z() << std::endl;
  //	return result;
  return zCoord;
}

TVector3 PndRiemannTrack::calcPosByS(double s)
{
  //	calcSForHits();
  TVectorD o = orig();
  const PndRiemannHit *firstHit = getHit(0);

  TVector2 k(firstHit->x().X() - o[0], firstHit->x().Y() - o[1]);

  Double_t start_phi = k.Phi();
  Double_t delta_phi = s / r();
  TVector2 Res2D(r(), 0);

  TVector2 rotated = Res2D.Rotate(start_phi + delta_phi);

  TVector3 result(rotated.X() + o[0], rotated.Y() + o[1], calcZPosByS(s));

  return result;
}

int PndRiemannTrack::calcIntersection(PndRiemannTrack &track, TVector3 &p1, TVector3 &p2)
{
  const double SMALL = 1E-12;
  const double VERTEX_CUT = fVertexCut;
  if (track.getNumHits() < 3) {
    if (fVerbose > 1)
      LOG(info) << " PndRiemannTrack::clacIntersection: less than 3 hits in track!";
    return 0;
  }

  TVector3 n1(fn[0], fn[1], fn[2]); // normal vector of THIS plane
  TVectorD tempn = track.n();
  TVector3 n2(tempn[0], tempn[1], tempn[2]); // normal vector of track plane
  Double_t c1 = c();                         // offset of THIS plane
  Double_t c2 = track.c();                   // offset of track plane

  if (fVerbose > 1)
    std::cout << "n1: " << n1.X() << " " << n1.Y() << " " << n1.Z() << " c1: " << c1 << std::endl;
  if (fVerbose > 1)
    std::cout << "n2: " << n2.X() << " " << n2.Y() << " " << n2.Z() << " c2: " << c2 << std::endl;

  TVector3 lineNorm = n1.Cross(n2);

  TVector3 dLineNorm = calcErrorLineNorm(track);

  if (lineNorm.Mag() < SMALL)
    return 0;
  // lineNorm.SetMag(1.);				//normalized normal vector of intersection line between the two planes
  if (fVerbose > 1)
    std::cout << "n1 x n2: " << lineNorm.X() << "+/-" << dLineNorm.X() << " " << lineNorm.Y() << "+/-" << dLineNorm.Y() << " " << lineNorm.Z() << "+/-" << dLineNorm.Z()
              << std::endl;

  double denom = n1[0] * n2[1] - n1[1] * n2[0];
  double x0 = (n1[1] * c2 - c1 * n2[1]) / denom;
  double y0 = (c1 * n2[0] - n1[0] * c2) / denom;
  TVector3 lineOffset(x0, y0, 0); // point on the intersection line
  TVector3 dLineOffset = calcErrorLineOffset(track);
  if (fVerbose > 1)
    std::cout << "lineOffset: " << lineOffset.X() << "+/-" << dLineOffset.X() << " " << lineOffset.Y() << "+/-" << dLineOffset.Y() << std::endl;

  //---------- Calculation of intersection of line with parabola ------------
  double denom2 = lineNorm[0] * lineNorm[0] + lineNorm[1] * lineNorm[1];
  double p = (2 * (lineNorm[0] * lineOffset[0] + lineNorm[1] * lineOffset[1]) - lineNorm[2]) / denom2;
  double q = (lineOffset[0] * lineOffset[0] + lineOffset[1] * lineOffset[1] - lineOffset[2]) / denom2;

  double rad = (p / 2) * (p / 2) - q;
  if (rad < 0) {
    if (fVerbose > 1)
      std::cout << "PndRiemannTrack::calcIntersection: No match with parabola" << std::endl;
    return 0;
  }
  double l1 = -(p / 2) + TMath::Sqrt(rad);
  double l2 = -(p / 2) - TMath::Sqrt(rad);

  TVectorD dXY12 = calcErrorXY1XY2(lineNorm, dLineNorm, lineOffset, dLineOffset);

  if (fVerbose > 1)
    std::cout << "l1: " << l1 << "+/-" << dXY12[4] << " l2: " << l2 << "+/-" << dXY12[5] << std::endl;

  TVector3 inter1 = lineNorm * l1 + lineOffset; // intersection point1 of line with parabola
  TVector3 inter2 = lineNorm * l2 + lineOffset; // intersection point2 of line with parabola
  if (fVerbose > 1) {
    std::cout << "intersection parabola 1: " << inter1.X() << "+/-" << dXY12[0] << " " << inter1.Y() << "+/-" << dXY12[1] << " " << inter1.Z() << std::endl;
    std::cout << "intersection parabola  2: " << inter2.X() << "+/-" << dXY12[2] << " " << inter2.Y() << "+/-" << dXY12[3] << " " << inter2.Z() << std::endl;
  }
  PndRiemannHit hit1, hit2;
  hit1.setXYZ(inter1.X(), inter1.Y(), inter1.Z());
  hit2.setXYZ(inter2.X(), inter2.Y(), inter2.Z());

  hit1.calcPosOnTrk(this);
  double s1 = hit1.s(); // arc length of hit1 for THIS track
  TVector2 dummy1(inter1.X(), inter2.Y());
  TVector2 dummy2(dXY12[0], dXY12[1]);
  double dS1 = calcErrorS(dummy1, dummy2, this);
  if (fVerbose > 1)
    std::cout << "S1 for track1: " << s1 << "+/-" << dS1 << std::endl;

  hit1.calcPosOnTrk(&track);
  double s2 = hit1.s(); // arc length of hit1 for track
  double dS2 = calcErrorS(dummy1, dummy2, &track);
  if (fVerbose > 1)
    std::cout << "S1 for track2: " << s2 << "+/-" << dS2 << std::endl;

  TVector3 vertex1; // = calcPosByS(s1);			//backcalculated vertex position from s1 with z-Value
  TVector3 vertex2; // = track.calcPosByS(s2);	//backcalculated vertex position from s2 with z-Value
  TVector3 dVertex1 = calcErrorPosByS(s1, dS1);
  TVector3 dVertex2 = track.calcErrorPosByS(s2, dS2);

  vertex1.SetXYZ(inter1.X(), inter1.Y(), calcZPosByS(s1)); // skip calculation error (from (Xv,Yv) to S and then back from S to (Xv,Yv))
  vertex2.SetXYZ(inter1.X(), inter1.Y(), track.calcZPosByS(s2));

  if (fVerbose > -1) {
    std::cout << "vertex candidate1 for hit1: " << vertex1.X() << " " << vertex1.Y() << " " << vertex1.Z() << std::endl;
    std::cout << "vertex candidate2 for hit1: " << vertex2.X() << " " << vertex2.Y() << " " << vertex2.Z() << std::endl;
  }

  if ((vertex1 - vertex2).Mag() < VERTEX_CUT) {
    if (fVerbose > -1)
      std::cout << "Vertex Found!" << std::endl;
    p1 = vertex1;
    p2 = vertex2;
    return 1;
  }

  hit2.calcPosOnTrk(this);
  double s1b = hit2.s();
  dummy1.Set(inter2.X(), inter2.Y());
  dummy2.Set(dXY12[2], dXY12[3]);
  double dS1b = calcErrorS(dummy1, dummy2, this);
  if (fVerbose > 1)
    std::cout << "S1b for track1: " << s1b << "+/-" << dS1b << std::endl;

  hit2.calcPosOnTrk(&track);
  double s2b = hit2.s();
  double dS2b = calcErrorS(dummy1, dummy2, &track);
  if (fVerbose > 1)
    std::cout << "S1b for track2: " << s2b << "+/-" << dS2b << std::endl;

  //	vertex1 = calcPosByS(s1b);
  //	vertex2 = track.calcPosByS(s2b);

  vertex1.SetXYZ(inter2.X(), inter2.Y(), calcZPosByS(s1b)); // skip calculation error (from (Xv,Yv) to S and then back from S to (Xv,Yv))
  vertex2.SetXYZ(inter2.X(), inter2.Y(), track.calcZPosByS(s2b));

  dVertex1 = calcErrorPosByS(s1b, dS1b);
  dVertex2 = track.calcErrorPosByS(s2b, dS2b);

  if (fVerbose > -1) {
    std::cout << "vertex candidate1 for hit2: " << vertex1.X() << " " << vertex1.Y() << " " << vertex1.Z() << std::endl;
    std::cout << "vertex candidate2 for hit2: " << vertex2.X() << " " << vertex2.Y() << " " << vertex2.Z() << std::endl;
  }
  if ((vertex1 - vertex2).Mag() < VERTEX_CUT) {
    if (fVerbose > -1)
      std::cout << "Vertex Found!" << std::endl;
    p1 = vertex1;
    p2 = vertex2;
    return 1;
  }
  return 0;
}

TVector3 PndRiemannTrack::calcErrorLineNorm(PndRiemannTrack &track)
{
  TVectorD n1 = n();
  TVectorD n2 = track.n();

  TMatrixD covN1 = covPlane();
  TMatrixD covN2 = track.covPlane();

  TVector3 dN1(TMath::Sqrt(covN1[1][1]), TMath::Sqrt(covN1[2][2]), TMath::Sqrt(covN1[3][3]));
  TVector3 dN2(TMath::Sqrt(covN2[1][1]), TMath::Sqrt(covN2[2][2]), TMath::Sqrt(covN2[3][3]));

  Double_t dLineNorm1 = TMath::Sqrt(TMath::Power(dN1[1] * n2[2], 2) + TMath::Power(n1[1] * dN2[2], 2) + TMath::Power(dN1[2] * n2[1], 2) + TMath::Power(n1[2] * dN2[1], 2));

  Double_t dLineNorm2 = TMath::Sqrt(TMath::Power(dN1[2] * n2[0], 2) + TMath::Power(n1[2] * dN2[0], 2) + TMath::Power(dN1[0] * n2[2], 2) + TMath::Power(n1[0] * dN2[2], 2));

  Double_t dLineNorm3 = TMath::Sqrt(TMath::Power(dN1[0] * n2[1], 2) + TMath::Power(n1[0] * dN2[1], 2) + TMath::Power(dN1[1] * n2[0], 2) + TMath::Power(n1[1] * dN2[0], 2));

  return TVector3(dLineNorm1, dLineNorm2, dLineNorm3);
}

TVector3 PndRiemannTrack::calcErrorLineOffset(PndRiemannTrack &track)
{
  TVectorD n1 = n();
  Double_t c1 = c();
  TVectorD n2 = track.n();
  Double_t c2 = track.c();

  TMatrixD covN1 = covPlane();
  TMatrixD covN2 = track.covPlane();

  TVector3 dN1(TMath::Sqrt(covN1[1][1]), TMath::Sqrt(covN1[2][2]), TMath::Sqrt(covN1[3][3]));
  TVector3 dN2(TMath::Sqrt(covN2[1][1]), TMath::Sqrt(covN2[2][2]), TMath::Sqrt(covN2[3][3]));
  Double_t dC1(TMath::Sqrt(covN1[0][0]));
  Double_t dC2(TMath::Sqrt(covN2[0][0]));

  Double_t denom = n1[0] * n2[1] - n1[1] * n2[0];
  //	Double_t dDenom2 = (TMath::Power(dN1[0]*n2[1],2) + TMath::Power(n1[0]*dN2[1],2) +
  //						TMath::Power(dN1[1]*n2[0],2) + TMath::Power(n1[1]*dN2[0],2));

  Double_t qX = (n1[1] * c2 - n2[1] * c1) / (TMath::Power(denom, 2));
  Double_t qY = (n2[0] * c1 - n1[0] * c2) / (TMath::Power(denom, 2));

  Double_t dx = TMath::Sqrt(TMath::Power(n2[1] * dC1 / denom, 2) + TMath::Power(n1[1] * dC2 / denom, 2) + TMath::Power(qX * n2[1] * dN1[0], 2) +
                            TMath::Power(qX * n1[1] * dN2[0], 2) + TMath::Power((c1 / denom + qX * n2[0]) * dN1[1], 2) + TMath::Power((c1 / denom + qX * n1[0]) * dN2[1], 2));

  Double_t dy = TMath::Sqrt(TMath::Power(n2[0] * dC1 / denom, 2) + TMath::Power(n1[0] * dC2 / denom, 2) + TMath::Power(qX * n2[0] * dN1[1], 2) +
                            TMath::Power(qX * n1[0] * dN2[1], 2) + TMath::Power((c1 / denom + qY * n2[1]) * dN1[0], 2) + TMath::Power((c1 / denom + qY * n1[1]) * dN2[0], 2));

  return TVector3(dx, dy, 0);
}

TVectorD PndRiemannTrack::calcErrorXY1XY2(TVector3 &line, TVector3 &dLine, TVector3 &offset, TVector3 &dOffset)
{
  const double SMALL = 1E-12;
  Double_t denom = (TMath::Power(line[0], 2) + TMath::Power(line[1], 2));
  Double_t p = (2 * (offset[0] * line[0] + offset[1] * line[1]) - line[2]) / denom;
  Double_t q = (TMath::Power(offset[0], 2) + TMath::Power(offset[1], 2) - offset[2]) / denom;
  Double_t S1 = -(p / 2) + TMath::Sqrt(TMath::Power(p / 2, 2) - q);
  Double_t S2 = -(p / 2) - TMath::Sqrt(TMath::Power(p / 2, 2) - q);

  //	std::cout << "S1: " << S1 << " S2: " << S2 << std::endl;
  Double_t prod = (2 * (offset[0] * line[0] + offset[1] * line[1]) - line[2]) / TMath::Power(denom, 2);
  //	std::cout << "denom: " << denom << " prod1: " << prod * line[0] * dLine[0] << " prod2: " << prod * line[1] * dLine[1]<< std::endl;

  if ((TMath::Power(p / 2, 2) - q) < SMALL)
    return TVectorD(6);

  Double_t sqrterm = TMath::Sqrt(TMath::Power(p / 2, 2) - q);

  Double_t dP = TMath::Sqrt(TMath::Power(2 * line[0] * dOffset[0] / denom, 2) + TMath::Power(2 * line[1] * dOffset[1] / denom, 2) + TMath::Power(dLine[2] / denom, 2) +
                            TMath::Power((2 * offset[0] / denom - 2 * prod * line[0]) * dLine(0), 2) + TMath::Power((2 * offset[1] / denom - 2 * prod * line[1]) * dLine(1), 2));
  //	std::cout << "dP1: " << TMath::Power(2*line[0]*dOffset[0]/denom,2) << " dP2: " << TMath::Power(2*line[1]*dOffset[1]/denom,2) << std::endl;
  //	std::cout << "dP3: " << TMath::Power(dLine[2]/denom,2) << std::endl;
  //	std::cout << "dP4: " << TMath::Power((2*offset[0]/denom - 2*prod*line[0])*dLine(0),2) << " dP5: " << TMath::Power((2*offset[1]/denom - 2*prod*line[1])*dLine(1),2) <<
  // std::endl;

  //	std::cout << "P: " << p << "+/-" << dP << std::endl;
  Double_t dQ = TMath::Sqrt(TMath::Power(2 * offset[0] / denom * dOffset[0], 2) + TMath::Power(2 * offset[1] / denom * dOffset[1], 2) + TMath::Power(dOffset[2] / denom, 2));
  //	std::cout << "Q: " << q << "+/-" << dQ << std::endl;
  Double_t dS1 = TMath::Sqrt(TMath::Power((-1 / 2 + p / (4 * sqrterm)) * dP, 2) + TMath::Power(1 / (2 * sqrterm) * dQ, 2));
  Double_t dS2 = TMath::Sqrt(TMath::Power((-1 / 2 - p / (4 * sqrterm)) * dP, 2) + TMath::Power(1 / (2 * sqrterm) * dQ, 2));

  Double_t dX1 = TMath::Sqrt(TMath::Power(dOffset[0], 2) + TMath::Power(line[0] * dS1, 2) + TMath::Power(dLine[0] * S1, 2));
  Double_t dY1 = TMath::Sqrt(TMath::Power(dOffset[1], 2) + TMath::Power(line[1] * dS1, 2) + TMath::Power(dLine[1] * S1, 2));

  //	std::cout << "dX1: " << dX1 << " dY1: " << dY1 << std::endl;

  Double_t dX2 = TMath::Sqrt(TMath::Power(dOffset[0], 2) + TMath::Power(line[0] * dS2, 2) + TMath::Power(dLine[0] * S2, 2));
  Double_t dY2 = TMath::Sqrt(TMath::Power(dOffset[1], 2) + TMath::Power(line[1] * dS2, 2) + TMath::Power(dLine[1] * S2, 2));

  //	std::cout << "dX2: " << dX2 << " dY2: " << dY2 << std::endl;
  TVectorD result(6);
  result[0] = dX1;
  result[1] = dY1;
  result[2] = dX2;
  result[3] = dY2;
  result[4] = dS1;
  result[5] = dS2;
  return result;
}

double PndRiemannTrack::calcErrorS(TVector2 &XY, TVector2 &dXY, PndRiemannTrack *track)
{
  assert(track != nullptr);
  TVectorD o = track->orig();
  double R = track->r();
  double dr = track->dR();

  const PndRiemannHit *firstHit = track->getHit(0);
  assert(firstHit != nullptr);
  TVector2 k(firstHit->x().X() - o[0], firstHit->x().Y() - o[1]);
  TVector2 l(XY.X() - o[0], XY.Y() - o[1]);

  double alpha = l.DeltaPhi(k);
  double dAlpha = TMath::Sqrt(TMath::Power(XY.X() * dXY.Y(), 2) + TMath::Power(XY.Y() * dXY.X(), 2)) / (TMath::Power(XY.X(), 2) + TMath::Power(XY.Y(), 2));

  double dS = TMath::Sqrt(TMath::Power(dAlpha * R, 2) + TMath::Power(alpha * dr, 2));

  return dS;
}

TVector3 PndRiemannTrack::calcErrorPosByS(Double_t s, Double_t dS)
{
  TVectorD o = orig();
  const PndRiemannHit *firstHit = getHit(0);
  assert(firstHit != nullptr);
  TVector2 k(firstHit->x().X() - o[0], firstHit->x().Y() - o[1]);
  Double_t dPhi = TMath::Sqrt(TMath::Power(dS / r(), 2) + TMath::Power(s / (r() * r()) * dR(), 2));
  Double_t phi = s / r();
  Double_t dL1 = TMath::Sqrt(TMath::Power((-k.X() * TMath::Sin(phi) - k.Y() * TMath::Cos(phi)) * dPhi, 2));
  Double_t dL2 = TMath::Sqrt(TMath::Power((k.X() * TMath::Cos(phi) - k.Y() * TMath::Sin(phi)) * dPhi, 2));
  Double_t dZCoord = TMath::Sqrt(TMath::Power(m() * dS, 2) + TMath::Power(s * mError(), 2) + TMath::Power(tError(), 2));

  Double_t resX = TMath::Sqrt(TMath::Power(dL1, 2) + TMath::Power(dX(), 2));
  Double_t resY = TMath::Sqrt(TMath::Power(dL2, 2) + TMath::Power(dY(), 2));

  TVector3 result(resX, resY, dZCoord);

  if (fVerbose > 1)
    std::cout << "PosBySError: s:" << dS << " Vector: " << result.x() << " " << result.y() << " " << result.z() << std::endl;
  return result;
}

double PndRiemannTrack::szDist(PndRiemannHit *hit)
{

  if (fSZFitDone == false)
    szFit();
  hit->calcPosOnTrk(this);
  double hits = hit->s();
  double predz = hits * fm + ft;
  return predz - hit->z();
}

double PndRiemannTrack::szError(PndRiemannHit *hit)
{
  if (fSZFitDone == false)
    szFit();
  hit->calcPosOnTrk(this);
  double hits = hit->s();
  double result = TMath::Sqrt(TMath::Power((hits * fmError), 2) + TMath::Power(ftError, 2));
  // if (fVerbose > 0) std::cout << "Error on szDist is: " << result;
  return result;
}

void PndRiemannTrack::calcStartStopAlpha()
{
  PndRiemannHit *first = getHit(0);
  PndRiemannHit *last = getLastHit();

  fStartAlpha = calcAlpha(first);
  fStopAlpha = calcAlpha(last);
}

double PndRiemannTrack::calcAlpha(PndRiemannHit *myHit)
{
  TVectorD origin = orig();
  TVector2 myVector(myHit->x().x() - origin[0], myHit->x().y() - origin[1]);
  return myVector.Phi();
}

void PndRiemannTrack::calcSForHits()
{
  for (size_t i = 0; i < fHits.size(); i++) {
    fHits[i].calcPosOnTrk(this);
  }
  sortHits();
}

// only after szFit!
double PndRiemannTrack::dip()
{
  if (fSZFitDone == false)
    szFit();
  return cos(atan(fm));
}

// only after szFit!
double PndRiemannTrack::dipangle()
{
  if (fSZFitDone == false)
    szFit();
  return (atan(fm));
}

double PndRiemannTrack::dDip()
{
  if (fSZFitDone == false)
    szFit();
  // return (fabs(sin(1/(1+fm*fm))) * fmError);
  // WRONG derivative!!!!!!!
  return (fabs(sin(atan(fm)) / (1 + fm * fm)) * fmError);
}

double PndRiemannTrack::Pt(double B)
{
  double result = 0.3 * B * r() * 0.01;
  if (fVerbose > 0)
    std::cout << "Pt: " << result << std::endl;

  return result;
}

double PndRiemannTrack::Pl(double B)
{
  double pl = Pt(B) * m();

  if (getNumHits() > 2) {
    PndRiemannHit *startHit = getHit(0);
    PndRiemannHit *nextHit = getHit(1);
    if (nextHit->z() - startHit->z() > 0) {
      if (pl < 0)
        pl *= -1;
    } else {
      if (pl > 0) {
        pl *= -1;
      }
    }
  }
  return pl;
}

double PndRiemannTrack::P(double B)
{
  double result = TMath::Sqrt(Pl(B) * Pl(B) + Pt(B) * Pt(B));
  return result;
}

TVector3 PndRiemannTrack::getPforHit(int i, double B)
{
  double pt = Pt(B);
  // double p = P(B); //[R.K. 01/2017] unused variable?
  double pl = Pl(B);
  TVectorD origin = orig();
  TVector3 result;
  //	PndRiemannHit* startHit = getHit(0);
  //	PndRiemannHit* lastHit = getLastHit();
  //	if (lastHit->z() < startHit->z()){
  //		pl *= -1;
  //	}

  if (i < (int)getNumHits()) {
    PndRiemannHit *myHit = getHit(i);
    // std::cout << "MyHit: " << myHit->x().X() << " " << myHit->x().Y() << std::endl;
    Double_t arclength = myHit->s() / r();

    Double_t phi = fStartAlpha + arclength;
    TVector2 ptVec(0, pt);
    ptVec = ptVec.Rotate(phi);
    if (i > 0) {
      PndRiemannHit *myHitBefore = getHit(i - 1);
      // std::cout << "MyHitBefore: " << myHitBefore->x().X() << " " << myHitBefore->x().Y() << std::endl;
      TVector2 difVec(myHit->x().x() - myHitBefore->x().x(), myHit->x().y() - myHitBefore->x().y());
      // std::cout <<"DifVec: " << difVec.X() << " " << difVec.Y() << std::endl;
      Double_t length = ptVec * difVec;
      // std::cout << "Length: " << length << std::endl;
      if (length < 0) {
        ptVec *= -1;
      }
    } else if (getNumHits() > 0) {
      PndRiemannHit *myHitAfter = getHit(1);
      // std::cout << "MyHitAfter: " << myHitAfter->x().X() << " " << myHitAfter->x().Y() << std::endl;
      TVector2 difVec2(myHitAfter->x().x() - myHit->x().x(), myHitAfter->x().y() - myHit->x().y());
      // std::cout <<"DifVec: " << difVec2.X() << " " << difVec2.Y() << std::endl;
      Double_t length2 = ptVec * difVec2;
      // std::cout << "Length: " << length2 << std::endl;
      if (length2 < 0) {
        ptVec *= -1;
      }
    }
    result.SetXYZ(ptVec.X(), ptVec.Y(), pl);
  }

  if (fVerbose > 0)
    std::cout << "P-Vector for point " << i << " : " << result.X() << " " << result.Y() << " " << result.Z() << std::endl;
  return result;
}

Int_t PndRiemannTrack::getCharge(Double_t B)
{
  TVector3 p = getPforHit(0, B);
  Double_t s = getHit(0)->s();
  TVector3 hitPos = calcPosByS(s);
  TVector2 hitPos2D(hitPos.x(), hitPos.y());

  TVector2 pt(p.x(), p.y());
  TVectorD origin = orig();
  TVector2 orig2(origin[0], origin[1]);

  orig2 -= hitPos2D;

  TVector2 origRotated = orig2.Rotate(-pt.Phi());

  // std::cout << "OrigRotated: " << origRotated.X() << " " << origRotated.Y() << std::endl;

  if (origRotated.Y() < 0)
    return 1;
  else
    return -1;
}

FairTrackParP PndRiemannTrack::getTrackParPForHit(Int_t i, Double_t B)
{
  calcSForHits();

  TVector3 hitPos;
  TVector3 hitPosError;
  TVector3 momError(2, 2, 2);
  TVector3 dj(1, 0, 0);
  TVector3 dk(0, 1, 0);
  TVector3 origin(0, 0, 1);

  if (i < (int)getNumHits()) {
    Double_t s = getHit(i)->s();
    getHit(i)->hit()->Position(hitPos);
    hitPos = calcPosByS(s);
    //	getHit(i)->hit()->Position(hitPos);
    getHit(i)->hit()->PositionError(hitPosError);
    FairTrackParP result(hitPos, getPforHit(i, B), hitPosError, momError, getCharge(B), origin, dj, dk);

    return result;
  } else {
    return FairTrackParP();
  }
}

PndTrack PndRiemannTrack::getPndTrack(Double_t B)
{
  FairTrackParP first, last;
  PndTrackCand myCand;
  if (getNumHits() > 0) {
    first = getTrackParPForHit(0, B);
    last = getTrackParPForHit(getNumHits() - 1, B);
    // FairTrackParP last;
  }
  return PndTrack(first, last, myCand);
}

// Todo: check for backward going tracks!!!
double PndRiemannTrack::sign() const
{
  double s = fHits[3].s();
  return s < 0 ? -1. : 1;
}

void PndRiemannTrack::calcJacRXY()
{
  double val = -1.;
  if ((1 - fn[2] * fn[2] - 4 * fc * fn[2]) > 0)
    val = (TMath::Sqrt(1 - fn[2] * fn[2] - 4 * fc * fn[2]));
  else {
    LOG(error) << " PndRiemannTrack::calcJacRXY val is imaginary: Sqrt of " << (1 - fn[2] * fn[2] - 4 * fc * fn[2]);
    return;
  }
  fjacRXY.Clear();
  fjacRXY.ResizeTo(3, 4);
  /*	fjacRXY[0][0] = -1/val;
    fjacRXY[0][3] = -1/(2*fn[2]) * (fn[2]+2*fc)/val - val/(2*fn[2]*fn[2]);
    fjacRXY[1][1] = -fn[0]/(2*fn[2]);
    fjacRXY[1][3] = fn[0]/(2*fn[2]*fn[2]);
    fjacRXY[2][2] = -fn[1]/(2*fn[2]);
    fjacRXY[2][3] = fn[1]/(2*fn[2]*fn[2]);*/

  if (fn[2] != 0.) {
    fjacRXY[0][0] = -1 / val;
    fjacRXY[0][3] = -1 / (2 * fn[2]) * (fn[2] + 2 * fc) / val - val / (2 * fn[2] * fn[2]);
    fjacRXY[1][1] = -1 / (2 * fn[2]); //<---------
    fjacRXY[1][3] = fn[0] / (2 * fn[2] * fn[2]);
    fjacRXY[2][2] = -1 / (2 * fn[2]); //<---------
    fjacRXY[2][3] = fn[1] / (2 * fn[2] * fn[2]);
  } else
    LOG(error) << " PndRiemannTrack::calcJacRXY fn[2] is zero!";
}

void PndRiemannTrack::PrintHits()
{
  LOG(info) << " PndRiemannTrack::PrintHits: " << fHits.size();
  // bool first = false; //[R.K. 01/2017] unused variable?
  for (size_t i = 0; i < fHits.size(); i++) {
    std::cout << i << ": ";

    if (fHits[i].hit() != 0) {
      //			std::cout << fHits[i].hit()->GetEntryNr() << " : ";
    }
    std::cout << fHits[i].x().X() << " +/- " << fHits[i].sigmaX() << "\t" << fHits[i].x().Y() << " +/- " << fHits[i].sigmaY() << "\t" << fHits[i].z() << " s: " << fHits[i].s()
              << "\t"
              << " dist: " << dist(&fHits[i]);
    //		calcPosByS(fHits[i].s()).Print();
    std::cout << std::endl;
  }
}

PndRiemannHit PndRiemannTrack::correctSttHit(PndSttHit *mySttHit)
{

  TVectorD normalVector = n();
  if (fVerbose > 0)
    std::cout << "MySttHit: " << mySttHit << std::endl;
  Double_t x_val = mySttHit->GetX();
  Double_t y_val = mySttHit->GetY();

  Double_t phi = TMath::ATan2((normalVector[1] + 2 * normalVector[2] * y_val), (normalVector[0] + 2 * normalVector[2] * x_val));

  Double_t correctedX = x_val + TMath::Cos(phi) * mySttHit->GetIsochrone();
  Double_t correctedY = y_val + TMath::Sin(phi) * mySttHit->GetIsochrone();

  Double_t correctedX1 = x_val + TMath::Cos(phi + TMath::Pi()) * mySttHit->GetIsochrone();
  Double_t correctedY1 = y_val + TMath::Sin(phi + TMath::Pi()) * mySttHit->GetIsochrone();

  PndRiemannHit myRiemannHit(mySttHit);
  PndRiemannHit myRiemannHit1(mySttHit);
  myRiemannHit1.setXYZ(correctedX, correctedY, 0);
  PndRiemannHit myRiemannHit2(mySttHit);
  myRiemannHit2.setXYZ(correctedX1, correctedY1, 0);

  Double_t finalX, finalY;

  if (fabs(dist(&myRiemannHit)) < fabs(dist(&myRiemannHit1))) {
    if (fabs(dist(&myRiemannHit)) < fabs(dist(&myRiemannHit2))) {
      finalX = x_val;
      finalY = y_val;
    } else {
      finalX = correctedX1;
      finalY = correctedY1;
    }
  } else {
    if (fabs(dist(&myRiemannHit1)) < fabs(dist(&myRiemannHit2))) {
      finalX = correctedX;
      finalY = correctedY;
    } else {
      finalX = correctedX1;
      finalY = correctedY1;
    }
  }

  if (fVerbose > 1) {
    std::cout << "TrackNormal + Length: " << normalVector[0] << "/" << normalVector[1] << "/" << normalVector[2] << " o: " << c() << std::endl;
    std::cout << "CalculateCorrectedSTTHit: TubeCenter(" << x_val << "/" << y_val << ") Radius: " << mySttHit->GetIsochrone() << " Distance: " << dist(&myRiemannHit) << std::endl;
    std::cout << "CalculateCorrectedSTTHit: Corrected1(" << correctedX << "/" << correctedY << ") Distance: " << dist(&myRiemannHit1) << " phi: " << phi << std::endl;
    std::cout << "CalculateCorrectedSTTHit: Corrected2(" << correctedX1 << "/" << correctedY1 << ") Distance: " << dist(&myRiemannHit2) << " phi: " << phi << std::endl;

    std::cout << "Result: " << finalX << "/" << finalY << std::endl;
  }
  PndRiemannHit result(mySttHit);
  if (fVerbose > 1) {
    std::cout << "result: " << mySttHit << " " << result.x().X() << "/" << result.x().Y() << "/" << result.x().Z() << " " << result.z() << std::endl;
  }
  result.setXYZ(finalX, finalY, 0);
  result.setDXYZ(0.01, 0.01, 200);

  if (fVerbose > 1) {
    std::cout << "result: " << result.x().X() << "/" << result.x().Y() << "/" << result.x().Z() << " " << result.z() << std::endl;
  }
  return result;
}

PndRiemannHit PndRiemannTrack::correctSttSkewedHit(PndSttHit *mySttHit, PndSttTube *myTube)
{
  TVectorD nVec = n();
  if (fVerbose > 0)
    std::cout << "MySttHit: " << mySttHit << std::endl;

  TVector3 tubeDir = myTube->GetWireDirection();
  TVector3 tubeMidPos = myTube->GetPosition();

  Double_t nominator = nVec[0] * tubeDir.x() + nVec[1] * tubeDir.y() + 2 * nVec[2] * (tubeDir.x() + tubeDir.y());
  Double_t denominator = 2 * nVec[2] * myTube->GetHalfLength() * (tubeDir.x() * tubeDir.x() + tubeDir.y() * tubeDir.y());

  Double_t posOnTube = nominator / denominator;
  if (posOnTube > 1) {
    posOnTube = 1;
  } else if (posOnTube < -1) {
    posOnTube = -1;
  }

  PndRiemannHit result(mySttHit);
  result.setXYZ(tubeMidPos.x() + posOnTube * myTube->GetHalfLength() * tubeDir.x(), tubeMidPos.y() + posOnTube * myTube->GetHalfLength() * tubeDir.y(),
                tubeMidPos.z() + posOnTube * myTube->GetHalfLength() * tubeDir.z());
  return result;
}

void PndRiemannTrack::correctSttHits()
{

  for (size_t i = 0; i < fHits.size(); i++) {
    if (fHits[i].hit()->GetEntryNr().GetType() == GetBranchId("STTHit")) {

      PndRiemannHit myHit = fHits[i];
      PndSttHit *mySttHit = (PndSttHit *)fHits[i].hit();

      PndRiemannHit correctedHit = correctSttHit(mySttHit);
      //			TVectorD normalVector = n();
      //			Double_t phi = TMath::ATan2((normalVector[1]+2*normalVector[2]*myHit.x().Y()), (normalVector[0]+2*normalVector[2]*myHit.x().X()));
      //
      //			Double_t correctedX = myHit.x().X()+TMath::Cos(phi)*mySttHit->GetIsochrone();
      //			Double_t correctedY = myHit.x().Y()+TMath::Sin(phi)*mySttHit->GetIsochrone();
      //
      //			Double_t correctedX1 = myHit.x().X()+TMath::Cos(phi+TMath::Pi())*mySttHit->GetIsochrone();
      //			Double_t correctedY1 = myHit.x().Y()+TMath::Sin(phi+TMath::Pi())*mySttHit->GetIsochrone();
      //
      //			PndRiemannHit myRiemannHit(myHit);
      //			PndRiemannHit myRiemannHit1(myHit);
      //			myRiemannHit1.setXYZ(correctedX, correctedY, 0);
      //			PndRiemannHit myRiemannHit2(myHit);
      //			myRiemannHit2.setXYZ(correctedX1, correctedY1, 0);
      //
      //			Double_t finalX, finalY;
      //
      //			if (fabs(dist(&myRiemannHit)) < fabs(dist(&myRiemannHit1))){
      //				if (fabs(dist(&myRiemannHit)) < fabs(dist(&myRiemannHit2))){
      //					finalX = myHit.x().X();
      //					finalY = myHit.x().Y();
      //				}
      //				else {
      //					finalX = correctedX1;
      //					finalY = correctedY1;
      //				}
      //			} else {
      //				if (fabs(dist(&myRiemannHit1)) < fabs(dist(&myRiemannHit2))){
      //					finalX = correctedX;
      //					finalY = correctedY;
      //				} else {
      //					finalX = correctedX1;
      //					finalY = correctedY1;
      //				}
      //
      //			}
      //
      //			if (fVerbose > 1){
      //				std::cout << "TrackNormal + Length: " << normalVector[0] << "/" << normalVector[1] << "/" << normalVector[2] << " o: " << c() << std::endl;
      //				std::cout << "CalculateCorrectedSTTHit: TubeCenter(" << myHit.x().X() << "/" << myHit.x().Y() << ") Radius: " << mySttHit->GetIsochrone() << " Distance: " <<
      // dist(&myRiemannHit) << std::endl; 				std::cout << "CalculateCorrectedSTTHit: Corrected1(" << correctedX << "/" << correctedY << ") Distance: " << dist(&myRiemannHit1) <<
      // " phi: " << phi << std::endl; 				std::cout << "CalculateCorrectedSTTHit: Corrected2(" << correctedX1 << "/" << correctedY1 << ") Distance: " << dist(&myRiemannHit2) <<
      // " phi: "
      //<< phi << std::endl;
      //
      //				std::cout << "Result: " << finalX << "/" << finalY << std::endl;
      //			}
      fHits[i].setXYZ(correctedHit.x().X(), correctedHit.x().Y(), 0);
      fHits[i].setDXYZ(0.001, 0.001, 200);
    }
  }

  //	return result;
}

Int_t PndRiemannTrack::GetBranchId(TString branchName)
{
  if (fBranchNameMap.count(branchName) == 0) {
    fBranchNameMap[branchName] = FairRootManager::Instance()->GetBranchId(branchName);
  }
  return fBranchNameMap[branchName];
}
