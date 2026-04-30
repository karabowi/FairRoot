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
//      Conformal Mapping of a PndTpc Cluster (x,y)->(r,phi)-> riemann sphere
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

#ifndef PNDRIEMANNHIT_HH
#define PNDRIEMANNHIT_HH

// Base Class Headers ----------------
#include "TObject.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op
#include "TVector3.h"
#include "TMatrixD.h"
#include "FairHit.h"
#include "TMath.h"

// Collaborating Class Declarations --
// class PndTpcCluster;
class PndRiemannTrack;

class PndRiemannHit : public TObject {
 public:
  // Constructors/Destructors ---------
  PndRiemannHit() : fX(0., 0., 0.), fSigmaX(0., 0., 0.), fCovX(3, 3), fHit(0), fHitID(-1), fS(-1.), fZ(-1.), fDeltaZ(-1.), fAlpha(-1.), fVerbose(0){};
  PndRiemannHit(double x, double y, double z, double dx, double dy, double dz);
  PndRiemannHit(FairHit *cl, int hitID = -1);
  ~PndRiemannHit();

  // Copy-/Assignment-Operator
  PndRiemannHit(const PndRiemannHit &myHit)
    : TObject(myHit), fX(myHit.fX), fSigmaX(myHit.fSigmaX), fCovX(myHit.fCovX), fHit(myHit.fHit), fHitID(myHit.fHitID), fS(myHit.fS), fZ(myHit.fZ), fDeltaZ(myHit.fDeltaZ),
      fAlpha(myHit.fAlpha), fVerbose(myHit.fVerbose)
  {
  }

  PndRiemannHit &operator=(const PndRiemannHit &myHit)
  {
    if (this != &myHit) {
      fX = myHit.fX;
      fSigmaX = myHit.fSigmaX;
      fCovX = myHit.fCovX;
      fHit = myHit.fHit;
      fHitID = myHit.fHitID;
      fS = myHit.fS;
      fZ = myHit.fZ;
      fDeltaZ = myHit.fDeltaZ;
      fAlpha = myHit.fAlpha;
      fVerbose = myHit.fVerbose;
    }
    return *this;
  }

  // Accessors -----------------------
  void setXYZ(double x, double y, double z);
  void setDXYZ(double dx, double dy, double dz);
  void setHit(FairHit *cl);
  const TVector3 &x() const { return fX; }
  const FairHit *hit() const { return fHit; }
  int hitID() const { return fHitID; }
  double s() const { return fS; }
  double z() const;
  double alpha() const { return fAlpha; }
  double sigmaXY() const;
  double sigmaX() const { return TMath::Sqrt(fCovX[0][0]); }
  double sigmaY() const { return TMath::Sqrt(fCovX[1][1]); }
  double sigmaW() const { return TMath::Sqrt(TMath::Power(2 * x().x() * sigmaX(), 2) + TMath::Power(2 * x().y() * sigmaY(), 2)); } // error of z-Coordinate (x2+y2) in RiemannSpace
  const TMatrixD &covX() const { return fCovX; }
  double covX(int row, int col) const { return fCovX[row][col]; }
  bool operator<(const PndRiemannHit &aHit) const
  { ///< Sort hits by arclength, fails if track curls
    if (s() >= 0 && aHit.s() >= 0) {
      return s() < aHit.s();
    } else if (s() <= 0 && aHit.s() <= 0) {
      return -s() < -aHit.s();
    } else
      return s() < aHit.s();
  }

  // Modifiers -----------------------
  // Operations ----------------------
  void calcPosOnTrk(PndRiemannTrack *trk);

 private:
  // Private Data Members ------------
  TVector3 fX;
  TVector3 fSigmaX; ///< Sigma Error of fX (should be replaced by covariance matrix)
  TMatrixD fCovX;   ///< Covariance Matrix of the hit
  FairHit *fHit;    //->  //no ownership over this pointer!
  int fHitID;
  double fS;      ///< pathlength along track
  double fZ;      ///< z-coordinate of hit
  double fDeltaZ; ///< error in z
  double fAlpha;  ///< angle along track
  int fVerbose;

  // Private Methods -----------------

 public:
  ClassDef(PndRiemannHit, 2)
};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
