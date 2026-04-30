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
//	Class PndEmcXtal
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
#include "TObject.h"
#include "PndEmcXtal.h"
#include "PndEmcTwoCoordIndex.h"
#include "TGeoArb8.h"
#include "TVector3.h"
#include "TRotation.h"
#include "TGeoMatrix.h"
#include "TMath.h"

//#include <vector>
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

//----------------
// Constructors --
//----------------
PndEmcXtal::PndEmcXtal()
  : fTCIIndex(new PndEmcTwoCoordIndex()), fTrap(TGeoTrap()), fRotation(TGeoRotation()), fCentre(0, 0, 0), fLength(0), fFrontCentre(0, 0, 0), fNormalToFrontFace(0, 0, 0),
    fAxis(0, 0, 0)
{
}
PndEmcXtal::PndEmcXtal(const PndEmcTwoCoordIndex *id, const TGeoTrap &trap, const TVector3 &pos, const TGeoRotation &rot)
  : fTCIIndex(id), fTrap(TGeoTrap(trap.GetName(), trap.GetDz(), trap.GetTheta(), trap.GetPhi(), trap.GetH1(), trap.GetBl1(), trap.GetTl1(), trap.GetAlpha1(), trap.GetH2(),
                                  trap.GetBl2(), trap.GetTl2(), trap.GetAlpha2())),
    fRotation(rot), fCentre(pos), fLength(0), fFrontCentre(0, 0, 0), fNormalToFrontFace(0, 0, 0), fAxis(0, 0, 0)
{

  // calculate length of the crystal
  fLength = trap.GetDz() * 2;

  // Obtain vector to trapezoid front face center.

  Double_t tx = TMath::Tan(trap.GetTheta() * TMath::DegToRad()) * TMath::Cos(trap.GetPhi() * TMath::DegToRad());
  Double_t ty = TMath::Tan(trap.GetTheta() * TMath::DegToRad()) * TMath::Sin(trap.GetPhi() * TMath::DegToRad());

  Double_t axis[3] = {trap.GetDz() * tx, trap.GetDz() * ty, trap.GetDz()};
  Double_t axis_rotated[3];
  rot.LocalToMaster(axis, axis_rotated);
  TVector3 axis_vector(axis_rotated);

  // Obtain unitary vector normal to trapezoid front face.

  Double_t normal[3] = {0.0, 0.0, 1.0};
  Double_t normal_rotated[3];
  rot.LocalToMaster(normal, normal_rotated);
  TVector3 normal_vector(normal_rotated);

  if (pos.Dot(axis_vector) < 0.0) {
    // cout << "Crystal orientation in module " << id->Index() / 100000000
    //     << " +dz" << endl;

    fFrontCentre = pos + axis_vector;
    fNormalToFrontFace = -1.0 * normal_vector;
    fAxis = -1.0 * axis_vector.Unit();
  } else {
    // cout << "Crystal orientation in module " << id->Index() / 100000000
    //     << " -dz" << endl;

    fFrontCentre = pos - axis_vector;
    fNormalToFrontFace = +1.0 * normal_vector;
    fAxis = +1.0 * axis_vector.Unit();
  }
}

//--------------
// Destructor --
//--------------

PndEmcXtal::~PndEmcXtal() {}

//		----------------------------------------
// 		-- Public Function Member Definitions --
//		----------------------------------------

bool PndEmcXtal::operator==(const PndEmcXtal &compare) const
{
  bool answer = false;

  if (*fTCIIndex == *compare.fTCIIndex)
    answer = true;

  return answer;
}

bool PndEmcXtal::operator<(const PndEmcXtal &compare) const
{
  bool answer = false;

  if (*fTCIIndex < *compare.fTCIIndex)
    answer = true;
  else if (*fTCIIndex == *compare.fTCIIndex && fLength < compare.fLength)
    answer = true;

  return answer;
}

const PndEmcTwoCoordIndex *PndEmcXtal::myIndex() const
{
  return fTCIIndex;
}

const TVector3 &PndEmcXtal::centre() const
{
  return fCentre;
}

const TVector3 &PndEmcXtal::frontCentre() const
{
  return fFrontCentre;
}

const TVector3 &PndEmcXtal::normalToFrontFace() const
{
  return fNormalToFrontFace;
}

const TVector3 &PndEmcXtal::axisVector() const
{
  return fAxis;
}

const TGeoTrap &PndEmcXtal::geometry() const
{
  return fTrap;
}

const TGeoRotation &PndEmcXtal::rotation() const
{
  return fRotation;
}

double PndEmcXtal::npAngle() const
{
  return fNormalToFrontFace.Theta() - fCentre.Theta();
}

ClassImp(PndEmcXtal)
