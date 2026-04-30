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

/*
 * PndHelixPropagator.cxx
 *
 *  Created on: Sep 23, 2013
 *  Updated: April 2021
 *      Author: stockman, Gaiser
 */

#include <iostream>
#include <cmath>
#include "TMath.h"
#include "TF1.h"
#include "Math/GSLRootFinder.h"
#include "Math/RootFinderAlgorithms.h"

#include "FairTrackPar.h"
#include "FairRunAna.h"
#include "FairLogger.h"

#include "PndPropagator.h"
#include "PndConstField.h"
#include "PndHelixPropagator.h"

ClassImp(PndHelixPropagator)

PndHelixPropagator::PndHelixPropagator()
  : PndPropagator("PndHelixPropagator", "helix propagator"), fFieldStrength(0),
    fCharge(0), fBackPropagate(kFALSE), fDeltaPhi(0)
{
  fPcaMode = 1;
  fMagneticField = FairRunAna::Instance()->GetField();
  if (fMagneticField == nullptr) {
    fMagneticField = new PndConstField();
  }
};

PndHelixPropagator::PndHelixPropagator(Double_t fieldStrength, TVector3 origin,
                                       TVector3 momentum, Double_t charge)
  : PndPropagator("PndHelixPropagator", "helix propagator"), fFieldStrength(fieldStrength),
    fCharge(charge), fBackPropagate(kFALSE), fDeltaPhi(0)
{
  fPcaMode = 1;
  fInitialPosition = origin;
  fInitialMomentum = momentum;
  fMagneticField = FairRunAna::Instance()->GetField();
  if (fMagneticField == nullptr) fMagneticField = new PndConstField("constfield", -100000, 100000, -100000, 100000, -100000, 100000, 0, 0, fFieldStrength);
};

Bool_t PndHelixPropagator::Propagate(FairTrackPar* TStart, FairTrackPar* TEnd, Int_t) {
  if (!FindPCA(TStart)) return kFALSE;

  if (fInitialMomentum.Mag() == 0) {
    LOG(info) << "PndHelixPropagator::Propagate: Momentum zero. No propagation.";
    *TEnd = FairTrackPar(TStart->GetX(), TStart->GetY(), TStart->GetZ(),
                         TStart->GetPx(), TStart->GetPy(), TStart->GetPz(), TStart->GetQ());

    return kTRUE;
  }
  
  if (fInitialMomentum.Pt() == 0 || fFieldStrength == 0) 
    *TEnd = FairTrackPar(fFinalPosition.X(), fFinalPosition.Y(), fFinalPosition.Z(),
                         fInitialMomentum.Px(), fInitialMomentum.Py(), fInitialMomentum.Pz(), TStart->GetQ());
  else {
    TVector3 newMomentum = fInitialMomentum;
    newMomentum.RotateZ(fDeltaPhi);
    *TEnd = FairTrackPar(fFinalPosition.X(), fFinalPosition.Y(), fFinalPosition.Z(),
                         newMomentum.Px(), newMomentum.Py(), newMomentum.Pz(), TStart->GetQ());
  }

  return kTRUE;
}

void PndHelixPropagator::Init(FairTrackPar* TStart) {
  fFieldStrength = 0.1 * fMagneticField->GetBz(TStart->GetX(), TStart->GetY(), TStart->GetZ());
  // LOG(info) << "Magnetic field: Bz = " << fFieldStrength;
  fInitialPosition = TStart->GetPosition();
  fInitialMomentum = TStart->GetMomentum();
  fCharge = TStart->GetQ();
}

TVector3 PndHelixPropagator::FindCircleCenter() {
  TVector3 magField(0, 0, fFieldStrength);
  TVector3 dirRadius = fInitialMomentum.Cross(magField).Unit();

  return fInitialPosition + (fCharge * Radius() * dirRadius);
}

Double_t PndHelixPropagator::FindDistanceToPCAXYProjection(FairTrackPar *TStart)
{
  Init(TStart);
  TVector3 circleCenter = FindCircleCenter();
  Double_t radius = Radius();
  return abs((fPoint - circleCenter).Perp() - radius);
}

Bool_t PndHelixPropagator::FindPCA(FairTrackPar* TStart) {
  Init(TStart);
  if (fInitialMomentum.Mag() == 0) {
    LOG(info) << "PndHelixPropagator::FindPCA: Momentum zero. Returning last point of track as PCA.";
    fFinalPosition = fInitialPosition;
    fPcaOutput.OnTrackPCA = fFinalPosition;
    return kTRUE;
  }
  
  if (fInitialMomentum.Pt() == 0 || fFieldStrength == 0) 
    return FindPCAOnStraightLine();

  else
    return FindPCAOnHelix();
}

Bool_t PndHelixPropagator::FindPCAOnStraightLine() {
  if (fInitialMomentum.Mag2() == 0) {
    LOG(info) << "PndHelixPropagator::FindPCAOnStraightLine: Momentum zero. Returning last point of track as PCA.";
    fFinalPosition = fInitialPosition;
    fPcaOutput.OnTrackPCA = fFinalPosition;
    fPcaOutput.Distance = 0;
    fPcaOutput.TrackLength = 0;
    return kTRUE;
  }
  
  if (fPcaMode == 1) {
    Double_t t_min = ((fPoint.X()-fInitialPosition.X()) * fInitialMomentum.Px()
                      + (fPoint.Y()-fInitialPosition.Y()) * fInitialMomentum.Py()
                      + (fPoint.Z()-fInitialPosition.Z()) * fInitialMomentum.Pz())
                     * 1./fInitialMomentum.Mag2();

    fFinalPosition = fInitialPosition + t_min * fInitialMomentum;
    fPcaOutput.OnTrackPCA = fFinalPosition;
    fPcaOutput.Distance = (fFinalPosition - fInitialPosition).Mag();
    fPcaOutput.TrackLength = fPcaOutput.Distance;
  }
  else {
    LOG(error) << "PndHelixPropagator::FindPCAOnStraightLine: Only propagation to point implemented at the moment!";
    return kFALSE;
  }
  return kTRUE;
}

Bool_t PndHelixPropagator::FindPCAOnHelix() {
  if (fInitialMomentum.Pt() == 0) {
    LOG(warn) << "PndHelixPropagator::FindPCAOnHelix: no transverse momentum, helix propagation not possible";
    return kFALSE;
  }
  TVector3 circleCenter = FindCircleCenter();
  TVector3 origin_shifted = fInitialPosition - circleCenter;
  TVector3 point_shifted = fPoint - circleCenter;
  Double_t momentum_fraction = fInitialMomentum.Pz()/fInitialMomentum.Pt();
  Double_t radius = Radius();
  
  if (fPcaMode == 1) {
    fA = point_shifted.X() * origin_shifted.X() + point_shifted.Y() * origin_shifted.Y();
    fB = point_shifted.X() * origin_shifted.Y() - origin_shifted.X() * point_shifted.Y();
    fC = (momentum_fraction * radius) * (momentum_fraction * radius);
    fD = -1. * momentum_fraction * radius * point_shifted.Z();

    Double_t start_value = -1;
    Bool_t propagate_forward_on_helix = (!fBackPropagate && fCharge < 0) || (fBackPropagate && fCharge > 0);
    if (propagate_forward_on_helix) {
      start_value = 1;
    }
   
    Double_t final_value, fn, dfn, sin_val, cos_val;
    Int_t n = 0;
    sin_val = std::sin(start_value);
    cos_val = std::cos(start_value);
    do {
        n++;
        fn = fA*sin_val + fB*cos_val + fC*start_value + fD;
        dfn = fA*cos_val - fB*sin_val + fC;
        final_value = start_value - fn/dfn;
        start_value = final_value;
        sin_val = std::sin(final_value);
        cos_val = std::cos(final_value);
      } while (abs(fA*sin_val + fB*cos_val + fC*final_value + fD) > 0.001 && n < 20);
    if (n > 19) return kFALSE;

    fDeltaPhi = final_value;

    if (propagate_forward_on_helix && fDeltaPhi < 0) {
      return kFALSE;
    }
    else if (!propagate_forward_on_helix && fDeltaPhi > 0) {
      return kFALSE;
    }

    Int_t dir = 1;
    if ((fDeltaPhi < 0 && !fBackPropagate) || (fDeltaPhi > 0 && fBackPropagate)) dir = -1;

    fFinalPosition.SetX(cos_val * origin_shifted.X() - sin_val * origin_shifted.Y());
    fFinalPosition.SetY(sin_val * origin_shifted.X() + cos_val * origin_shifted.Y());
    fFinalPosition.SetZ(momentum_fraction * radius * dir * fDeltaPhi);

    fFinalPosition += circleCenter;

    fPcaOutput.OnTrackPCA = fFinalPosition;
    fPcaOutput.Distance = (fFinalPosition - fInitialPosition).Mag();
    fPcaOutput.TrackLength = TMath::Abs(radius * fDeltaPhi * TMath::Sqrt(1 + momentum_fraction * momentum_fraction));
  }
  else {
    LOG(error) << "PndHelixPropagator::FindPCAOnHelix: Only propagation to point implemented at the moment!";
    return kFALSE;
  }
  return kTRUE;
}

FairTrackPar PndHelixPropagator::PropagateToZ(Double_t zpos)
{
  TVector3 magField(0, 0, fFieldStrength);
  TVector3 dirRadius = fInitialMomentum.Cross(magField).Unit();
  TVector3 circleCenter = fInitialPosition + (fCharge * Radius() * dirRadius);

  Double_t ratioPtPl = fInitialMomentum.Pt() / fInitialMomentum.z();
  TVector3 zeroCircle = fInitialPosition - circleCenter;

  Double_t zLength = zpos - fInitialPosition.z();
  Double_t arcLength = ratioPtPl * zLength;
  Double_t deltaPhi = arcLength / Radius();
  Double_t newPhi = deltaPhi + zeroCircle.Phi();

  TVector3 newOrigin = zeroCircle;
  newOrigin.SetPhi(newPhi);
  newOrigin += circleCenter;
  newOrigin.SetZ(zpos);
  TVector3 newMomentum = fInitialMomentum;
  newMomentum.RotateZ(deltaPhi);

  FairTrackPar result(newOrigin.x(), newOrigin.y(), newOrigin.z(), newMomentum.Px(), newMomentum.Py(), newMomentum.Pz(), fCharge);

  return result;
}

FairTrackPar PndHelixPropagator::PropagateToXYPos(TVector2) // xyPos //[R.K.03/2017] unused variable(s)
{                                                           // FIXME The TVector2 is not utilized!
  TVector3 magField(0, 0, fFieldStrength);
  TVector3 dirRadius = fInitialMomentum.Cross(magField).Unit();
  TVector3 circleCenter = fInitialPosition + (fCharge * Radius() * dirRadius);
  circleCenter.SetZ(0);

  Double_t ratioPtPl = fInitialMomentum.Pt() / fInitialMomentum.z();
  TVector3 zeroCircle = fInitialPosition - circleCenter;
  TVector3 negCircleCenter = -circleCenter;
  Double_t deltaPhi = negCircleCenter.DeltaPhi(zeroCircle);

  TVector3 newOrigin = negCircleCenter.Unit() * Radius() + circleCenter;
  Double_t zpos = deltaPhi * Radius() / ratioPtPl - fInitialPosition.Z();
  newOrigin.SetZ(zpos);
  TVector3 newMomentum = fInitialMomentum;
  newMomentum.RotateZ(deltaPhi);

  FairTrackPar result(newOrigin.x(), newOrigin.y(), newOrigin.z(), newMomentum.Px(), newMomentum.Py(), newMomentum.Pz(), fCharge);

  return result;
}

FairTrackPar PndHelixPropagator::PropagateToS(Double_t) // arcLength //[R.K.03/2017] unused variable(s)
{
  // Not yet implemented!
  std::cout << "*** -E- PndHelixPropagator::PropagateToS not yet implemented!" << std::endl;
  return FairTrackPar();
}

FairTrackPar PndHelixPropagator::PropagateByAngle(Double_t step)
{
  TVector3 magField(0, 0, fFieldStrength);
  TVector3 dirRadius = fInitialMomentum.Cross(magField).Unit();
  TVector3 circleCenter = fInitialPosition + (fCharge * Radius() * dirRadius);
  Double_t stepInRad = step / 180 * TMath::Pi();

  Double_t ratioPtPl = 0;
  if (fInitialMomentum.Pz() != 0)
    ratioPtPl = fInitialMomentum.Pt() / fInitialMomentum.Pz();

  TVector3 zeroCircle = fInitialPosition - circleCenter;
  Double_t newPhi = zeroCircle.Phi() + stepInRad;
  TVector3 newOrigin = zeroCircle;
  newOrigin.SetPhi(newPhi);
  newOrigin += circleCenter;
  if (ratioPtPl != 0)
    newOrigin.SetZ((stepInRad * Radius() * -1 * fCharge / ratioPtPl) + fInitialPosition.Z());
  else
    newOrigin.SetZ(fInitialPosition.Z());

  TVector3 newMomentum = fInitialMomentum;
  newMomentum.RotateZ(stepInRad);

  FairTrackPar result(newOrigin.x(), newOrigin.y(), newOrigin.z(), newMomentum.Px(), newMomentum.Py(), newMomentum.Pz(), fCharge);
  return result;
}
