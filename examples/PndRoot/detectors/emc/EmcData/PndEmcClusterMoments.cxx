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
// 	$Id: PndEmcClusterMoments.cc,v 1.3 2005/11/15 01:20:20 steinke Exp $
//
// Description:
//	Class PndEmcClusterMoments
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

#include "PndEmcClusterMoments.h"
#include "PndEmcClusterProperties.h"

#include "PndEmcTwoCoordIndex.h"
#include "PndEmcCluster.h"
#include "PndEmcDigi.h"
#include "TClonesArray.h"
#include <iostream>
#include <cmath>
#include <map>

using std::endl;
using std::ostream;

//----------------
// Constructors --
//----------------

PndEmcClusterMoments::PndEmcClusterMoments(const PndEmcCluster &cluster, const TClonesArray *digiArray) : PndEmcAbsClusterProperty(cluster, digiArray) {}

PndEmcClusterMoments::PndEmcClusterMoments(const PndEmcClusterMoments &other) : PndEmcAbsClusterProperty(other) {}

//--------------
// Destructor --
//--------------

PndEmcClusterMoments::~PndEmcClusterMoments() {}

//-------------
// Methods   --
//-------------

Double_t PndEmcClusterMoments::SecondMoment() const
{
  Double_t sum = 0;

  std::map<Int_t, Int_t>::const_iterator iter = Members().begin();

  TVector3 clusterLocation(MyCluster().where());
  while (iter != Members().end()) {
    PndEmcDigi *digi = (PndEmcDigi *)DigiArray()->At(iter->second);
    TVector3 digiLocation(digi->where());
    Double_t dx = digiLocation.x() - clusterLocation.x();
    Double_t dy = digiLocation.y() - clusterLocation.y();
    Double_t dz = digiLocation.z() - clusterLocation.z();
    sum += digi->GetEnergy() * (dx * dx + dy * dy + dz * dz);
    ++iter;
  }

  sum /= MyCluster().GetEnergy();
  return sum;
}

Double_t PndEmcClusterMoments::SecondMomentTP() const
{
  TVector3 cluster = MyCluster().where();

  Double_t sum = 0;
  std::map<Int_t, Int_t>::const_iterator iter = Members().begin();
  while (iter != Members().end()) {
    PndEmcDigi *digi = (PndEmcDigi *)DigiArray()->At(iter->second);
    Double_t da = cluster.Angle(digi->where());
    sum += digi->GetEnergy() * da * da;
    ++iter;
  }

  sum /= MyCluster().GetEnergy();
  return sum;
}

Double_t PndEmcClusterMoments::Theta1() const
{
  Double_t t = 0;

  std::map<Int_t, Int_t>::const_iterator iter = Members().begin();

  Double_t clusterTheta(MyCluster().theta());

  while (iter != Members().end()) {
    PndEmcDigi *digi = (PndEmcDigi *)DigiArray()->At(iter->second);
    t += digi->GetEnergy() * (digi->GetTheta() - clusterTheta);
    ++iter;
  }

  t /= MyCluster().GetEnergy();

  return t;
}

Double_t PndEmcClusterMoments::Phi1() const
{
  Double_t p = 0;
  Double_t clus_phi = MyCluster().phi();

  std::map<Int_t, Int_t>::const_iterator iter = Members().begin();

  while (iter != Members().end()) {
    PndEmcDigi *digi = (PndEmcDigi *)DigiArray()->At(iter->second);
    p += digi->GetEnergy() * PndEmcCluster::FindPhiDiff(digi->GetPhi(), clus_phi);
    ++iter;
  }

  p /= MyCluster().GetEnergy();

  return (p);
}

Double_t PndEmcClusterMoments::Theta2() const
{
  Double_t t = 0;

  std::map<Int_t, Int_t>::const_iterator iter = Members().begin();
  Double_t clusterTheta(MyCluster().theta());

  while (iter != Members().end()) {
    PndEmcDigi *digi = (PndEmcDigi *)DigiArray()->At(iter->second);
    Double_t theta_diff = digi->GetTheta() - clusterTheta;
    t += digi->GetEnergy() * theta_diff * theta_diff;
    ++iter;
  }

  t /= MyCluster().GetEnergy();

  return (t);
}

Double_t PndEmcClusterMoments::Phi2() const
{
  Double_t p = 0;
  Double_t clus_phi = MyCluster().phi();

  std::map<Int_t, Int_t>::const_iterator iter = Members().begin();

  while (iter != Members().end()) {
    PndEmcDigi *digi = (PndEmcDigi *)DigiArray()->At(iter->second);
    Double_t phi_diff = PndEmcCluster::FindPhiDiff(digi->GetPhi(), clus_phi);
    p += digi->GetEnergy() * phi_diff * phi_diff;
    ++iter;
  }

  p /= MyCluster().GetEnergy();

  return (p);
}

Double_t PndEmcClusterMoments::Major1() const
{
  Double_t moment = 0;
  PndEmcClusterProperties *properties = new PndEmcClusterProperties(MyCluster(), DigiArray());
  Double_t axis = properties->Major_axis();
  Double_t clusT = MyCluster().theta(), clusP = MyCluster().phi();

  if (axis == -999.)
    return (0.);

  TVector2 maj(cos(axis), sin(axis));

  std::map<Int_t, Int_t>::const_iterator iter = Members().begin();

  while (iter != Members().end()) {
    PndEmcDigi *digi = (PndEmcDigi *)DigiArray()->At(iter->second);
    TVector2 coord(PndEmcCluster::FindPhiDiff(digi->GetPhi(), clusP), digi->GetTheta() - clusT);

    moment += fabs(coord * maj) * digi->GetEnergy();
    ++iter;
  }

  moment /= MyCluster().GetEnergy();

  return (moment);
}

Double_t PndEmcClusterMoments::Major2() const
{
  Double_t moment = 0;
  PndEmcClusterProperties *properties = new PndEmcClusterProperties(MyCluster(), DigiArray());
  Double_t axis = properties->Major_axis();
  Double_t clusT = MyCluster().theta(), clusP = MyCluster().phi();

  if (axis == -999.)
    return (0.);

  TVector2 maj(cos(axis), sin(axis));

  std::map<Int_t, Int_t>::const_iterator iter = Members().begin();

  while (iter != Members().end()) {
    PndEmcDigi *digi = (PndEmcDigi *)DigiArray()->At(iter->second);
    TVector2 coord(PndEmcCluster::FindPhiDiff(digi->GetPhi(), clusP), digi->GetTheta() - clusT);

    moment += (coord * maj) * (coord * maj) * digi->GetEnergy();
    ++iter;
  }

  moment /= MyCluster().GetEnergy();

  return (moment);
}

Double_t PndEmcClusterMoments::Minor1() const
{
  Double_t moment = 0;
  PndEmcClusterProperties *properties = new PndEmcClusterProperties(MyCluster(), DigiArray());
  Double_t axis = properties->Major_axis();
  Double_t clusT = MyCluster().theta(), clusP = MyCluster().phi();

  if (axis == -999.)
    return (0.);

  axis += TMath::Pi() / 2.0;
  TVector2 min(cos(axis), sin(axis));

  std::map<Int_t, Int_t>::const_iterator iter = Members().begin();

  while (iter != Members().end()) {
    PndEmcDigi *digi = (PndEmcDigi *)DigiArray()->At(iter->second);
    TVector2 coOrd(PndEmcCluster::FindPhiDiff(digi->GetPhi(), clusP), digi->GetTheta() - clusT);

    moment += fabs(coOrd * min) * digi->GetEnergy();
    ++iter;
  }

  moment /= MyCluster().GetEnergy();

  return (moment);
}

Double_t PndEmcClusterMoments::Minor2() const
{
  Double_t moment = 0;
  PndEmcClusterProperties *properties = new PndEmcClusterProperties(MyCluster(), DigiArray());
  Double_t axis = properties->Major_axis();
  Double_t clusT = MyCluster().theta(), clusP = MyCluster().phi();

  if (axis == -999.)
    return (0.);

  axis += TMath::Pi() / 2.0;
  TVector2 min(cos(axis), sin(axis));

  std::map<Int_t, Int_t>::const_iterator iter = Members().begin();

  while (iter != Members().end()) {
    PndEmcDigi *digi = (PndEmcDigi *)DigiArray()->At(iter->second);
    TVector2 coOrd(PndEmcCluster::FindPhiDiff(digi->GetPhi(), clusP), digi->GetTheta() - clusT);

    moment += (coOrd * min) * (coOrd * min) * digi->GetEnergy();
    iter++;
  }

  moment /= MyCluster().GetEnergy();

  return (moment);
}

TVector3 PndEmcClusterMoments::Centre1() const
{
  TVector3 aVector(0, 0, 0);
  TVector3 clusterCentre(MyCluster().where());

  std::map<Int_t, Int_t>::const_iterator iter = Members().begin();
  while (iter != Members().end()) {
    PndEmcDigi *digi = (PndEmcDigi *)DigiArray()->At(iter->second);
    aVector += (digi->where() - clusterCentre) * digi->GetEnergy();
    ++iter;
  }
  aVector *= 1. / MyCluster().GetEnergy();

  return aVector;
}

TVector3 PndEmcClusterMoments::Centre2() const
{
  TVector3 aVector(0, 0, 0);
  TVector3 clusterCentre(MyCluster().where());

  std::map<Int_t, Int_t>::const_iterator iter = Members().begin();
  while (iter != Members().end()) {
    PndEmcDigi *digi = (PndEmcDigi *)DigiArray()->At(iter->second);
    TVector3 displacement(digi->where() - clusterCentre);
    aVector += TVector3(displacement.X() * displacement.X(), displacement.Y() * displacement.Y(), displacement.Z() * displacement.Z()) * digi->GetEnergy();
    ++iter;
  }
  aVector *= 1. / MyCluster().GetEnergy();

  return aVector;
}

void PndEmcClusterMoments::Print(const Option_t *) const
{
  //  std::cout << " centre1=" << Centre1() << ", centre2=" << Centre2();
  //  std::cout << endl;
  //  std::cout << " theta1=" << Theta1() << ", phi1=" << Phi1();
  //  std::cout << ", theta2=" << Theta2() << ", phi2=" << Phi2();
  //  std::cout << endl;
  //  std::cout << " major1=" << Major1() << ", minor1=" << Minor1();
  //  std::cout << ", major2=" << Major2() << ", minor2=" << Minor2();
  //  std::cout << endl;
  //  std::cout << " secondMoment=" << SecondMoment() << ", secondMomentTP=" << SecondMomentTP();
  //  std::cout << endl;
}

ClassImp(PndEmcClusterMoments)
