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
//      Track on Riemann Sphere
//      Circle parameters can be calculated from plane parameters
//      plane(c,nx,ny,nz);
//
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef PNDRIEMANNTRACK_HH
#define PNDRIEMANNTRACK_HH

// Base Class Headers ----------------
#include "TObject.h"

// Collaborating Class Headers -------
#include <vector>
#include "TVectorD.h"
#include "TVector3.h"
#include "TMatrixD.h"
#include "TMath.h"

// Collaborating Class Declarations --
#include "PndRiemannHit.h"
#include "PndTrack.h"
#include "PndTrackCand.h"

#include "PndSttHit.h"
#include "PndSttTube.h"

#include <iostream>
#include <iomanip>
#include <algorithm>

class PndRiemannTrack : public TObject {
 public:
  // Constructors/Destructors ---------
  PndRiemannTrack();
  PndRiemannTrack(PndTrackCand *trackCand);
  ~PndRiemannTrack();

  // Accessors -----------------------
  const TVectorD &n() const { return fn; }
  double c() const { return fc; }
  const TVectorD &av() const { return fav; }

  TVectorD orig() const;
  double dX() { return TMath::Sqrt(fcovRXY[1][1]); }
  double dY() { return TMath::Sqrt(fcovRXY[2][2]); }
  double r() const;
  double dR();
  double dip();
  double dipangle(); //< dipangle theta
  double dDip();
  double Pt(double B); //< transvers momentum Pt calculated by the magnetic field B [tesla]
  double Pl(double B);
  double P(double B);
  double sign() const;
  double getSZm() const { return fm; }
  double getSZt() const { return ft; }
  unsigned int getNumHits() { return fHits.size(); }
  PndRiemannHit *getHit(unsigned int i)
  {
    PndRiemannHit *myHit = &(fHits[i]);
    return myHit;
  }
  PndRiemannHit *getLastHit() { return getHit(getNumHits() - 1); }
  std::vector<PndRiemannHit> getHits() const { return fHits; };
  TVector3 getPforHit(int i, double B);
  PndTrack getPndTrack(Double_t B);
  FairTrackParP getTrackParPForHit(Int_t i, Double_t B);
  Int_t getCharge(Double_t B);

  double calcZPosByS(double s);
  TVector3 calcPosByS(double s);
  void calcStartStopAlpha();
  double calcAlpha(PndRiemannHit *myHit);

  void calcSForHits();

  int calcIntersection(PndRiemannTrack &track, TVector3 &p1, TVector3 &p2);

  void sortHits() { std::sort(fHits.begin(), fHits.end()); }

  double weight() const { return fweight; };
  TMatrixD covPlane() const { return fcovPlane; };
  TMatrixD jacRXY() const { return fjacRXY; };
  TMatrixD covRXY() const { return fcovRXY; };
  double m() const { return fm; }
  double mError() const { return fmError; }
  double t() const { return ft; }
  double tError() const { return ftError; }

  // Modifiers -----------------------
  void addHit(PndRiemannHit &hit);
  void addPndTrackCand(PndTrackCand *trackCand);
  void init(double x0, double y0, double R, double dip, double z0);

  // Operations ----------------------
  void refit(bool withErrorCalc = true);
  double dist(PndRiemannHit *hit);
  double distError(PndRiemannHit *hit);
  double distCircle(PndRiemannHit *hit);
  double ChiSquareDistCircle();
  void szFit(bool withErrorCalc = true);
  double calcChi2Plane();
  double calcSZChi2(PndRiemannHit *hit); // calculates the chi2 of the track plus the additional hit
  double szDist(PndRiemannHit *hit);
  double szError(PndRiemannHit *hit);
  double szChi2() const { return fChi2; };

  void SetVerbose(int i) { fVerbose = i; }
  void SetVertexCut(double cut) { fVertexCut = cut; }
  TVector3 calcErrorPosByS(Double_t s, Double_t dS);

  void correctSttHits();
  PndRiemannHit correctSttHit(PndSttHit *mySttHit);
  PndRiemannHit correctSttSkewedHit(PndSttHit *mySttHit, PndSttTube *myTube);
  void PrintHits();

  virtual void Print(std::ostream &out = std::cout)
  {

    out << std::setprecision(6) << "Riemann Track: Radius " << r() << " +/- " << dR() << " Origin: " << orig()[0] << " +/- " << dX() << " / " << orig()[1] << " +/- " << dY()
        << std::endl;
    out << "RiemannTrack: Normal: " << n()[0] << "/" << n()[1] << "/" << n()[2] << " c: " << c() << std::endl;
    out << "Dip: " << dip() << " +/- " << dDip() << " StartAlpha: " << fStartAlpha << " StopAlpha: " << fStopAlpha << std::endl;
    out << "Chi2Plane: " << calcChi2Plane() << std::endl;
    PrintHits();
  }

  friend std::ostream &operator<<(std::ostream &out, PndRiemannTrack &track)
  {
    track.Print(out);
    return out;
  }

 private:
  // Private Data Members ------------
  TVectorD fn;  ///< normal vector to plane;
  TVectorD fav; ///< average over all hits
  double fc;    ///< distance of plane to origin

  double fm; ///< parameters of sz-fit
  double ft;
  double fmError;     ///< Error of fit
  double ftError;     ///< Error of fit
  double fChi2;       ///< Chisquare of sz fit
  TMatrixD fcovPlane; ///< full covarince matrix of the plane;
  TMatrixD fjacRXY;   ///< jacobian matrix to transform from c,n1,n2,n2 to r,x,y
  TMatrixD fcovRXY;

  int fVerbose;

  bool fFitDone;
  bool fSZFitDone;
  bool fErrorCalcDone;
  double fweight; ///< sum over all weights (1/(sigmaXY*sigmaXY))
  bool ftrefit;
  double fVertexCut;

  std::vector<PndRiemannHit> fHits;
  Double_t fStartAlpha;
  Double_t fStopAlpha;

  std::map<TString, Int_t> fBranchNameMap;
  /////////////////
  ////////////////

  void calcJacRXY(); ///< calcualtes fjacRXY

  TVector3 calcErrorLineNorm(PndRiemannTrack &track);
  TVector3 calcErrorLineOffset(PndRiemannTrack &track);
  TVectorD calcErrorXY1XY2(TVector3 &line, TVector3 &dLine, TVector3 &offset, TVector3 &dOffset);
  Double_t calcErrorS(TVector2 &XY, TVector2 &dXY, PndRiemannTrack *track);

  Int_t GetBranchId(TString branchName);

  // Private Methods -----------------

 public:
  ClassDef(PndRiemannTrack, 3)
};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
