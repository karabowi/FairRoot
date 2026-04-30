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

//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id:$
//
// Description:
//	Class BSEmcXtal
//
//	 Software developed for the BaBar Detector at the SLAC B-Factory.
// Adapted for the PANDA experiment at GSI
//
// Author List:
//	Gautier Hamel de Monchenault - CEN Saclay & Lawrence Berkeley Lab
//      Stephen J. Gowdy               University of Edinburgh
// Dima Melnichuk - adaption for PANDA
//
// Copyright Information:
//	Copyright (C) 1996		Lawrence Berkeley Laboratory
//	Copyright (C) 1996	       CEA - Centre d'Etude de Saclay
//	Copyright (C) 1997	       University of Edinburgh
//------------------------------------------------------------------------
#include "BSEmcXtal.h"

#include "TGeoArb8.h"
#include "TGeoMatrix.h"
#include "TMath.h"
#include "TVector3.h"

#include "fairlogger/Logger.h"

using namespace std;

//----------------
// Constructors --
//----------------
BSEmcXtal::BSEmcXtal()
  : BSEmcGeoItem(), fLength(0), fTrap(TGeoTrap()), fRotation(TGeoRotation()), fCentre(0, 0, 0), fFrontCentre(0, 0, 0), fNormalToFrontFace(0, 0, 0), fAxis(0, 0, 0)
{
}
BSEmcXtal::BSEmcXtal(Int_t t_id, const TGeoTrap &t_trap, const TVector3 &t_pos, const TGeoRotation &t_rot)
  : BSEmcGeoItem(), fLength(0), fTrap(TGeoTrap(t_trap.GetName(), t_trap.GetDz(), t_trap.GetTheta(), t_trap.GetPhi(), t_trap.GetH1(), t_trap.GetBl1(), t_trap.GetTl1(),
                                               t_trap.GetAlpha1(), t_trap.GetH2(), t_trap.GetBl2(), t_trap.GetTl2(), t_trap.GetAlpha2())),
    fRotation(t_rot), fCentre(t_pos), fFrontCentre(0, 0, 0), fNormalToFrontFace(0, 0, 0), fAxis(0, 0, 0)
{
  SetID(t_id);
  // calculate length of the crystal
  fLength = t_trap.GetDz() * 2;

  // Obtain vector to trapezoid front face center.

  Double_t tx = TMath::Tan(t_trap.GetTheta() * TMath::DegToRad()) * TMath::Cos(t_trap.GetPhi() * TMath::DegToRad());
  Double_t ty = TMath::Tan(t_trap.GetTheta() * TMath::DegToRad()) * TMath::Sin(t_trap.GetPhi() * TMath::DegToRad());

  Double_t axis[3] = {t_trap.GetDz() * tx, t_trap.GetDz() * ty, t_trap.GetDz()};
  Double_t axis_rotated[3] = {0, 0, 0};

  LOG(trace) << "Axis is in local space: " << axis[0] << ", " << axis[1] << ", " << axis[2];
  t_rot.LocalToMaster(axis, axis_rotated);
  LOG(trace) << "Axis is in master space: " << axis_rotated[0] << ", " << axis_rotated[1] << ", " << axis_rotated[2];

  TVector3 axis_vector{axis_rotated};

  // Obtain unitary vector normal to trapezoid front face.

  Double_t normal[3] = {0.0, 0.0, 1.0};
  Double_t normal_rotated[3] = {0, 0, 0};
  LOG(trace) << "normal is in local space: " << normal[0] << ", " << normal[1] << ", " << normal[2];

  t_rot.LocalToMaster(normal, normal_rotated);
  TVector3 normal_vector{normal_rotated};
  LOG(trace) << "Axis is in master space: " << normal_rotated[0] << ", " << normal_rotated[1] << ", " << normal_rotated[2];

  if (t_pos.Dot(axis_vector) < 0.0) {
    fFrontCentre = t_pos + axis_vector;
    fNormalToFrontFace = -1.0 * normal_vector;
    fAxis = -1.0 * axis_vector.Unit();
  } else {
    fFrontCentre = t_pos - axis_vector;
    fNormalToFrontFace = +1.0 * normal_vector;
    fAxis = +1.0 * axis_vector.Unit();
  }
}

//--------------
// Destructor --
//--------------

BSEmcXtal::~BSEmcXtal() {}

//----------------------------------------
//-- Public Function Member Definitions --
//----------------------------------------

Bool_t BSEmcXtal::operator==(const BSEmcXtal &t_compare) const
{
  Bool_t answer = kFALSE;

  if (GetID() == t_compare.GetID()) {
    answer = kTRUE;
  }

  return answer;
}

Bool_t BSEmcXtal::operator<(const BSEmcXtal &t_compare) const
{
  Bool_t answer = kFALSE;

  if (GetID() < t_compare.GetID()) {
    answer = kTRUE;
  } else if (GetID() == t_compare.GetID() && fLength < t_compare.fLength) {
    answer = kTRUE;
  }

  return answer;
}

const TVector3 &BSEmcXtal::GetCentre() const
{
  return fCentre;
}

const TVector3 &BSEmcXtal::GetFrontCentre() const
{
  return fFrontCentre;
}

const TVector3 &BSEmcXtal::GetNormalToFrontFace() const
{
  return fNormalToFrontFace;
}

const TVector3 &BSEmcXtal::GetAxisVector() const
{
  return fAxis;
}

const TGeoTrap &BSEmcXtal::GetGeometry() const
{
  return fTrap;
}

const TGeoRotation &BSEmcXtal::GetRotation() const
{
  return fRotation;
}

Double_t BSEmcXtal::GetNPAngle() const
{
  return fNormalToFrontFace.Theta() - fCentre.Theta();
}

ClassImp(BSEmcXtal)
