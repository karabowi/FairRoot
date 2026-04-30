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
//
// Description:
//	Class PndEmcClusterDistances
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	Stephen J. Gowdy	Originator
//
// Copyright Information:
//	Copyright (C) 1998	University of Edinburgh
//
// Dima Melnichuk, adaption for pandaroot
//------------------------------------------------------------------------

#include "PndEmcClusterDistances.h"
#include "PndEmcStructure.h"
#include "PndEmcDataTypes.h"
#include "PndEmcTwoCoordIndex.h"
#include "PndEmcCluster.h"
#include "PndEmcDigi.h"
#include "PndEmcXtal.h"

#include "assert.h"

PndEmcClusterDistances::PndEmcClusterDistances(const PndEmcCluster &toUse, const TClonesArray *digiArray)
  : PndEmcAbsClusterProperty(toUse, digiArray), fShowerMaxLocation(new TVector3()), fClusterLocation(new TVector3()), fTheGeom(new PndEmcXtal()),
    fTheNormalToFrontFace(new TVector3()), fDistanceOfPlane(0), fClusPhi(0), fClusTheta(0)
{
  TVector3 clusLocation(toUse.where());

  fClusterLocation = new TVector3(clusLocation.x(), clusLocation.y(), clusLocation.z());

  fShowerMaxLocation = new TVector3(*fClusterLocation);

  Double_t showerMax = (log(toUse.GetEnergy() / 0.0145) + 0.5) * 1.86;

  fShowerMaxLocation->SetMag(fClusterLocation->Mag() + showerMax);

  PndEmcTwoCoordIndex *theTCI = PndEmcStructure::Instance()->locateIndex(fShowerMaxLocation->Theta(), fShowerMaxLocation->Phi());

  assert(theTCI != nullptr);

  PndEmcTciXtalMap const &tciXtalMap = PndEmcStructure::Instance()->GetTciXtalMap();
  fTheGeom = tciXtalMap.find(theTCI)->second;

  fTheNormalToFrontFace = new TVector3(fTheGeom->normalToFrontFace());

  fDistanceOfPlane = fTheNormalToFrontFace->Dot(*fShowerMaxLocation);

  fClusPhi = toUse.phi();
  fClusTheta = toUse.theta();
}

PndEmcClusterDistances::PndEmcClusterDistances(const PndEmcClusterDistances &theDistance)
  : PndEmcAbsClusterProperty(theDistance), fShowerMaxLocation(new TVector3(*(theDistance.fShowerMaxLocation))), fClusterLocation(new TVector3(*(theDistance.fClusterLocation))),
    fTheGeom(theDistance.fTheGeom), fTheNormalToFrontFace(theDistance.fTheNormalToFrontFace), fDistanceOfPlane(theDistance.fDistanceOfPlane), fClusPhi(theDistance.fClusPhi),
    fClusTheta(theDistance.fClusTheta)
{
}
//--------------
// Destructor --
//--------------

PndEmcClusterDistances::~PndEmcClusterDistances()
{
  delete fClusterLocation;
  delete fShowerMaxLocation;
}

//-------------
// Methods   --
//-------------

Double_t PndEmcClusterDistances::RadialDistance(const PndEmcDigi *const theDigi) const
{

  TVector3 digiWhere(theDigi->where());
  TVector3 vecDigiWhere(digiWhere.x(), digiWhere.y(), digiWhere.z());

  PndEmcTciXtalMap const &tciXtalMap = PndEmcStructure::Instance()->GetTciXtalMap();
  PndEmcXtal *theXtalGeom = tciXtalMap.find(theDigi->GetTCI())->second;

  const TVector3 digiNormal = theXtalGeom->normalToFrontFace();

  Double_t normalProduct = fTheNormalToFrontFace->Dot(digiNormal);

  Double_t distToFF = fTheNormalToFrontFace->Dot(vecDigiWhere);

  Double_t digiDepth = (fDistanceOfPlane - distToFF) / normalProduct;

  TVector3 digiPerp(vecDigiWhere + digiDepth * digiNormal - *(fShowerMaxLocation));

  return digiPerp.Mag();
}

Double_t PndEmcClusterDistances::AngularDistance(const PndEmcDigi *const theDigi) const
{

  TVector3 digiWhere(theDigi->where());
  TVector3 vecDigiWhere(digiWhere.x(), digiWhere.y(), digiWhere.z());

  return fClusterLocation->Angle(vecDigiWhere);
}

Double_t PndEmcClusterDistances::AngularSeparation(const PndEmcDigi *const theDigi) const
{
  Double_t phi = theDigi->GetPhi();
  Double_t theta = theDigi->GetTheta();
  Double_t phiDiff = phi - fClusPhi;
  while (phiDiff > TMath::Pi())
    phiDiff -= 2 * TMath::Pi();
  while (phiDiff < -TMath::Pi())
    phiDiff += 2 * TMath::Pi();

  Double_t thetaDiff = theta - fClusTheta;

  return sqrt(phiDiff * phiDiff + thetaDiff * thetaDiff);
}

ClassImp(PndEmcClusterDistances)
