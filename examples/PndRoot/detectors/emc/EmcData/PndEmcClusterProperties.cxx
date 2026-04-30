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

//---------------------------------------------------------------------
//	 Software developed for the BaBar Detector at the SLAC B-Factory.
// Adapted for the PANDA experiment at GSI
//
// Author List:
//	Xiaorong Shi            Lawrence Livermore National Lab
//	Steve Playfer           University of Edinburgh
//	Stephen Gowdy           University of Edinburgh
//	Helmut Marsiske         SLAC
//---------------------------------------------------------------------

#include "PndEmcClusterProperties.h"

#include "PndEmcClusterMoments.h"
#include "PndEmcCluster.h"
#include "PndEmcDigi.h"
#include "PndEmcXtal.h"
#include "PndEmcStructure.h"
#include "TVector3.h"
#include "TClonesArray.h"

#include <iostream>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <cfloat>
#include <vector>
#include "assert.h"

using std::endl;
using std::vector;

//----------------
// Constructors --
//----------------

PndEmcClusterProperties::PndEmcClusterProperties(const PndEmcCluster &cluster, const TClonesArray *digiArray) : PndEmcAbsClusterProperty(cluster, digiArray) {}

//--------------
// Destructor --
//--------------

PndEmcClusterProperties::~PndEmcClusterProperties() {}

Double_t PndEmcClusterProperties::Energy() const
{
  Double_t sum = 0;
  std::vector<Int_t>::const_iterator digi_iter;
  const std::vector<Int_t> digiList = MyCluster().DigiList();

  for (digi_iter = digiList.begin(); digi_iter != digiList.end(); ++digi_iter) {
    PndEmcDigi *digi = (PndEmcDigi *)DigiArray()->At(*digi_iter);
    sum += digi->GetEnergy();
  }

  return sum;
}

Double_t PndEmcClusterProperties::Mass() const
{
  Double_t mass2;
  TVector3 clusterMomentum(0, 0, 0);
  vector<Int_t>::const_iterator digi_iter;
  TVector3 digiDirection;
  Double_t digiEnergy;
  std::vector<Int_t> digiList = MyCluster().DigiList();

  for (digi_iter = digiList.begin(); digi_iter != digiList.end(); ++digi_iter) {
    PndEmcDigi *digi = (PndEmcDigi *)DigiArray()->At(*digi_iter);
    digiDirection = digi->where().Unit();
    digiEnergy = digi->GetEnergy();
    clusterMomentum = clusterMomentum + digiDirection * digiEnergy;
  }

  Double_t clEnergy = Energy();

  mass2 = clEnergy * clEnergy - clusterMomentum.Mag2();

  if (mass2 < 0.0)
    return 0.0;
  else
    return sqrt(mass2);
}

Double_t PndEmcClusterProperties::Major_axis() const
{
  // Angle of major axis wrt phi

  Double_t maj = 0., st2 = 0.;

  std::vector<Int_t>::const_iterator current;
  std::vector<Int_t> digiList = MyCluster().DigiList();

  Double_t n = digiList.size();
  if (n == 1)
    return (-999.);

  PndEmcClusterMoments *moments = new PndEmcClusterMoments(MyCluster(), DigiArray());

  Double_t phi_wtd = moments->Phi1();
  Double_t phi_clu = MyCluster().phi();
  Double_t theta_clu = MyCluster().theta();
  Double_t theta_wtd = moments->Theta1();

  for (current = digiList.begin(); current != digiList.end(); ++current) {
    PndEmcDigi *digi = (PndEmcDigi *)DigiArray()->At(*current);
    Double_t xx = 0., yy = 0., e = 0., t = 0.;
    xx = PndEmcCluster::FindPhiDiff(digi->GetPhi(), phi_clu);
    yy = digi->GetTheta() - theta_clu;
    e = digi->GetEnergy();
    e *= e; // Will use e squared
    t = (xx - phi_wtd) * e;
    st2 += t * t;
    maj += t * (yy - theta_wtd) * e;
  }

  // The major axis is found by the slope of a line through
  // the coordinates or the Digis with respect to the centre
  // of the cluster. (Or just the coordinates, but I'll be consistent.)

  if (!st2)
    return (TMath::Pi() / 2.0);

  maj /= st2;

  return (atan(maj));
}

TVector3 PndEmcClusterProperties::Where(TString method, std::vector<Double_t> params)
{
  TVector3 pos;
  if (!strcmp(method, "lilo")) {
    pos = LiloWhere(params);
  } else if (!strcmp(method, "linear")) {
    pos = LinearWhere();
  } else {
    std::cout << "Incorrect cluster position method: " << method.Data() << std::endl;
    abort();
  }
  return pos;
}

TVector3 PndEmcClusterProperties::GravWhere()
{
  TVector3 aVector(0, 0, 0);

  std::vector<Int_t>::const_iterator current;
  std::vector<Int_t> digiList = MyCluster().DigiList();

  for (current = digiList.begin(); current != digiList.end(); ++current) {
    PndEmcDigi *digi = (PndEmcDigi *)DigiArray()->At(*current);
    aVector += digi->where() * digi->GetEnergy();
  }

  aVector *= 1. / MyCluster().GetEnergy();

  PndEmcTwoCoordIndex *theTCI = PndEmcStructure::Instance()->locateIndex(aVector.Theta(), aVector.Phi());

  assert(theTCI != nullptr);

  PndEmcTciXtalMap const &tciXtalMap = PndEmcStructure::Instance()->GetTciXtalMap();
  const PndEmcXtal *theGeom = tciXtalMap.find(theTCI)->second;

  const TVector3 normal = theGeom->normalToFrontFace();

  TVector3 centre(theGeom->frontCentre() - TVector3(0.0, 0.0, 0.0));

  double distanceOfPlane = normal.Dot(centre);

  double amplitude = distanceOfPlane / normal.Dot(aVector.Unit());

  aVector.SetMag(amplitude);

  return TVector3(aVector.x(), aVector.y(), aVector.z());
}

TVector3 PndEmcClusterProperties::LinearWhere()
{
  const double lClusEnergy = Energy();

  assert(lClusEnergy != 0);

  TVector3 lLinSum(0, 0, 0);

  PndEmcTciXtalMap const &tciXtalMap = PndEmcStructure::Instance()->GetTciXtalMap();

  std::vector<Int_t>::const_iterator current;
  std::vector<Int_t> digiList = MyCluster().DigiList();

  for (current = digiList.begin(); current != digiList.end(); ++current) {
    PndEmcDigi *lDigi = (PndEmcDigi *)DigiArray()->At(*current);
    // PndEmcTwoCoordIndex *tci = lDigi->GetTCI();  //[R.K. 03/2017] unused variable?
    // PndEmcXtal* xtal = tciXtalMap.find(tci)->second; //[R.K. 01/2017] unused variable?

    const TVector3 lDigiWhere = lDigi->where();

    const double lDigiEnergy = lDigi->GetEnergy();
    const double lLinWeight = lDigiEnergy / lClusEnergy;

    lLinSum += lLinWeight * lDigiWhere;
  }

  PndEmcTwoCoordIndex *lTCI = PndEmcStructure::Instance()->locateIndex(lLinSum.Theta(), lLinSum.Phi());

  assert(lTCI != nullptr);

  const PndEmcXtal *lGeom = tciXtalMap.find(lTCI)->second;

  // First, find out if the point is outside the crystal.

  const TVector3 lCentre = lGeom->frontCentre();
  const TVector3 lNormal = lGeom->normalToFrontFace();
  const TVector3 lVector = lCentre - lLinSum;
  const double lLength = lNormal.Dot(lVector);

  if (lLength < 0.0) {
    // Point is outside crystal
    // Project point back onto front-face.
    const TVector3 lUnit = lLinSum.Unit();
    const double lMag = (lNormal.Dot(lCentre) / lNormal.Dot(lUnit));
    lLinSum.SetMag(lMag);
  }

  return lLinSum;
}

TVector3 PndEmcClusterProperties::LiloWhere(std::vector<Double_t> params)
{
  Double_t offsetParmA = params[0];
  Double_t offsetParmB = params[1];
  Double_t offsetParmC = params[2];

  const double lClusEnergy = Energy();

  assert(lClusEnergy != 0);

  const double lOffset = offsetParmA - offsetParmB * exp(-offsetParmC * pow(lClusEnergy, 1.171)) * pow(lClusEnergy, -0.534);

  TVector3 lLiloPoint(1, 1, 1);

  TVector3 lLinSum(0, 0, 0);
  TVector3 lLogSum(0, 0, 0);

  double lLogWeightSum = 0;
  int lLogNum = 0;

  bool lLogSecondTheta = false;
  bool lLogSecondPhi = false;
  int lLogFirstTheta = -666;
  int lLogFirstPhi = -666;

  std::vector<Int_t>::const_iterator current;
  std::vector<Int_t> digiList = MyCluster().DigiList();
  PndEmcTciXtalMap const &tciXtalMap = PndEmcStructure::Instance()->GetTciXtalMap();

  for (current = digiList.begin(); current != digiList.end(); ++current) {
    PndEmcDigi *lDigi = (PndEmcDigi *)DigiArray()->At(*current);
    PndEmcXtal *xtal = tciXtalMap.find(lDigi->GetTCI())->second;
    const TVector3 tNormal = xtal->normalToFrontFace();

    // TODO Consider forwars endcup separetly
    // TVector3 tmpPoint=lDigi->where();
    //		if(lDigi->theta() > 3000)
    // tmpPoint += *tNormal * 7.0 * 6.2;
    // else
    // tmpPoint += tNormal * 6.2;

    const TVector3 lDigiWhere = lDigi->where();

    const int lDigiTheta = lDigi->GetThetaInt();
    const int lDigiPhi = lDigi->GetPhiInt();
    const double lDigiEnergy = lDigi->GetEnergy();
    const double lLinWeight = lDigiEnergy / lClusEnergy;
    const double lLogWeight = lOffset + log(lLinWeight);

    lLinSum += lLinWeight * lDigiWhere;
    if (lLogWeight > 0) {
      lLogSum += lLogWeight * lDigiWhere;
      lLogWeightSum += lLogWeight;
      lLogNum++;

      if (lLogNum == 1) {
        lLogFirstTheta = lDigiTheta;
        lLogFirstPhi = lDigiPhi;
      } else {
        if (!lLogSecondTheta && lDigiTheta != lLogFirstTheta)
          lLogSecondTheta = true;
        if (!lLogSecondPhi && lDigiPhi != lLogFirstPhi)
          lLogSecondPhi = true;
      }
    }
  }

  if (lLogNum > 0)
    lLogSum *= 1. / lLogWeightSum;

  lLiloPoint.SetTheta(lLogSecondTheta ? lLogSum.Theta() : lLinSum.Theta());
  lLiloPoint.SetPhi(lLogSecondPhi ? lLogSum.Phi() : lLinSum.Phi());

  PndEmcTwoCoordIndex *lTCI = PndEmcStructure::Instance()->locateIndex(lLiloPoint.Theta(), lLiloPoint.Phi());

  assert(lTCI != nullptr);

  const PndEmcXtal *lGeom = tciXtalMap.find(lTCI)->second;

  // First, find out if the point is outside the crystal.

  const TVector3 lLiloVector = lLiloPoint;
  const TVector3 lCentre = lGeom->frontCentre();

  const TVector3 lNormal = lGeom->normalToFrontFace();
  const TVector3 lVector = lCentre - lLiloVector;
  const double lLength = lNormal.Dot(lVector);
  // const Hep3Vector lUnit = lLiloVector.unit();
  // const double lMag = (lNormal->dot(lCentre)/lNormal->dot(lUnit));
  // cout<<" With original code, lilo-position "<<float(lMag)<<endl;

  if (lLength < 0.0) {
    // Point is outside crystal (not sure its correct for all cases?).
    // Anyhow, project point back onto front-face.
    const TVector3 lUnit = lLiloVector.Unit();
    const double lMag = (lNormal.Dot(lCentre) / lNormal.Dot(lUnit));
    lLiloPoint.SetMag(lMag);
  } else {
    // Don't project onto front-face. Keep the centroid inside the crytal.
    // cout<<" Point is inside crystal, keep as it is "<<endl;
    if (lLogNum > 1) {
      // Use logarithmic centroid position
      lLiloPoint.SetMag(lLogSum.Mag());
      // cout<<" With log centroid method, lilo-position "<<lLiloPoint.Mag()<<endl;
    } else {
      // Use linear centroid position
      lLiloPoint.SetMag(lLinSum.Mag());
      // cout<<" Use lin centroid method, lilo-position "<<lLiloPoint.Mag()<<endl;
    }
  }

  return lLiloPoint;
}

ClassImp(PndEmcClusterProperties)
