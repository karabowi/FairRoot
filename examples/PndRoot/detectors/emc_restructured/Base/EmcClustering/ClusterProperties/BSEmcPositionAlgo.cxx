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

#include "BSEmcPositionAlgo.h"

#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "TVector3.h"

#include "fairlogger/Logger.h"

#include "BSEmcCluster.h"
#include "BSEmcClusterPropertiesPar.h"
#include "BSEmcCrystalPositionPar.h"
#include "BSEmcDetectorID.h"
#include "BSEmcDigi.h"

BSEmcPositionAlgo::BSEmcPositionAlgo() : fClusterPosParam({}), fClusterPosMethod(""), fPositionPar(nullptr), fPositionCalculationMethod({}) {}

BSEmcPositionAlgo::~BSEmcPositionAlgo() {}

void BSEmcPositionAlgo::SetClusterPropertiesPar(const BSEmcClusterPropertiesPar *t_clusteringPar)
{
  fClusterPosMethod = t_clusteringPar->GetEmcClusterPosMethod();

  if (strcmp(fClusterPosMethod.c_str(), "lilo") == 0) {
    LOG(debug) << "Lilo cluster position method";
    fClusterPosParam.push_back(t_clusteringPar->GetOffsetParmA());
    fClusterPosParam.push_back(t_clusteringPar->GetOffsetParmB());
    fClusterPosParam.push_back(t_clusteringPar->GetOffsetParmC());
    fPositionCalculationMethod = std::bind(&BSEmcPositionAlgo::CalculateLiLoPosition, this, std::placeholders::_1, std::placeholders::_2);
  } else if (strcmp(fClusterPosMethod.c_str(), "linear") == 0) {
    fPositionCalculationMethod = std::bind(&BSEmcPositionAlgo::CalculateLinearPosition, this, std::placeholders::_1, std::placeholders::_2);
  } else if (strcmp(fClusterPosMethod.c_str(), "grav") == 0) {
    fPositionCalculationMethod = std::bind(&BSEmcPositionAlgo::CalculateGravPosition, this, std::placeholders::_1, std::placeholders::_2);
  } else {
    LOG(error) << "BSEmcPositionAlgo::CalculatePosition() - Incorrect cluster position method: " << fClusterPosMethod << "! Aborting!";
    abort();
  }
}

TVector3 BSEmcPositionAlgo::CalculatePosition(const BSEmcCluster *t_cluster, const std::vector<const BSEmcDigi *> &t_digis) const
{
  return fPositionCalculationMethod(t_cluster, t_digis);
}

TVector3 BSEmcPositionAlgo::CalculateLinearPosition(const BSEmcCluster *t_cluster, const std::vector<const BSEmcDigi *> &t_digis) const
{
  const Double_t clusEnergy = t_cluster->GetEnergy();

  TVector3 linSum(0, 0, 0);

  for (const auto &digiInfo : t_cluster->GetDigis()) {
    const BSEmcDigi *digi = t_digis.at(digiInfo.fDigiIdx);
    linSum += digi->GetEnergy() / clusEnergy * fPositionPar->GetPosition(digiInfo.fDetectorId) * digiInfo.fWeight;
  }

  const BSEmcCrystalPositionData &crystal = GetClusterCrystal(t_cluster, linSum.Theta(), linSum.Phi());

  // First, find out if the point is outside the crystal.

  const TVector3 center = crystal.GetFrontCentre();
  const TVector3 normal = crystal.GetNormalToFrontFace();
  const Double_t length = normal.Dot(center - linSum);

  if (length < 0.0) {
    // Point is outside crystal
    // Project point back onto front-face.
    const TVector3 unit = linSum.Unit();
    const Double_t mag = (normal.Dot(center) / normal.Dot(unit));
    linSum.SetMag(mag);
  }

  return linSum;
}

TVector3 BSEmcPositionAlgo::CalculateGravPosition(const BSEmcCluster *t_cluster, const std::vector<const BSEmcDigi *> &t_digis) const
{
  TVector3 aVector{0, 0, 0};
  const Double_t clusEnergy = t_cluster->GetEnergy();

  for (const auto &digiInfo : t_cluster->GetDigis()) {
    const BSEmcDigi *digi = t_digis.at(digiInfo.fDigiIdx);
    aVector += fPositionPar->GetPosition(digiInfo.fDetectorId) * digi->GetEnergy() * digiInfo.fWeight;
  }

  aVector *= 1. / clusEnergy;

  const BSEmcCrystalPositionData &crystal = GetClusterCrystal(t_cluster, aVector.Theta(), aVector.Phi());
  const TVector3 normal = crystal.GetNormalToFrontFace();

  TVector3 centre{crystal.GetFrontCentre() - TVector3(0.0, 0.0, 0.0)};

  Double_t distanceOfPlane = normal.Dot(centre);

  Double_t amplitude = distanceOfPlane / normal.Dot(aVector.Unit());

  aVector.SetMag(amplitude);

  return TVector3(aVector.x(), aVector.y(), aVector.z());
}

TVector3 BSEmcPositionAlgo::CalculateLiLoPosition(const BSEmcCluster *t_cluster, const std::vector<const BSEmcDigi *> &t_digis) const
{
  Double_t offsetParmA = fClusterPosParam[0];
  Double_t offsetParmB = fClusterPosParam[1];
  Double_t offsetParmC = fClusterPosParam[2];

  const Double_t lClusEnergy = t_cluster->GetEnergy();

  assert(lClusEnergy != 0);

  const Double_t lOffset = offsetParmA - offsetParmB * exp(-offsetParmC * pow(lClusEnergy, 1.171)) * pow(lClusEnergy, -0.534);

  TVector3 lLiloPoint(1, 1, 1);

  TVector3 lLinSum(0, 0, 0);
  TVector3 lLogSum(0, 0, 0);

  Double_t lLogWeightSum = 0;
  Int_t lLogNum = 0;

  Bool_t lLogSecondTheta = kFALSE;
  Bool_t lLogSecondPhi = kFALSE;
  Int_t lLogFirstTheta = -666;
  Int_t lLogFirstPhi = -666;

  for (const auto &digiInfo : t_cluster->GetDigis()) {
    const BSEmcDigi *digi = t_digis.at(digiInfo.fDigiIdx);
    const TVector3 lDigiWhere = fPositionPar->GetPosition(digi->GetDetectorId());

    const Int_t lDigiTheta = BSEmcDetectorID(digi->GetDetectorId()).GetColumn();
    const Int_t lDigiPhi = BSEmcDetectorID(digi->GetDetectorId()).GetRow();
    const Double_t lDigiEnergy = digi->GetEnergy() * digiInfo.fWeight;
    const Double_t lLinWeight = lDigiEnergy / lClusEnergy;
    const Double_t lLogWeight = lOffset + log(lLinWeight);

    lLinSum += lLinWeight * lDigiWhere;
    if (lLogWeight > 0) {
      lLogSum += lLogWeight * lDigiWhere;
      lLogWeightSum += lLogWeight;
      lLogNum++;

      if (lLogNum == 1) {
        lLogFirstTheta = lDigiTheta;
        lLogFirstPhi = lDigiPhi;
      } else {
        if (!lLogSecondTheta && lDigiTheta != lLogFirstTheta) {
          lLogSecondTheta = kTRUE;
        }
        if (!lLogSecondPhi && lDigiPhi != lLogFirstPhi) {
          lLogSecondPhi = kTRUE;
        }
      }
    }
  }

  if (lLogNum > 0) {
    lLogSum *= 1. / lLogWeightSum;
  }

  lLiloPoint.SetTheta(lLogSecondTheta ? lLogSum.Theta() : lLinSum.Theta());
  lLiloPoint.SetPhi(lLogSecondPhi ? lLogSum.Phi() : lLinSum.Phi());

  const BSEmcCrystalPositionData &theGeom = GetClusterCrystal(t_cluster, lLiloPoint.Theta(), lLiloPoint.Phi());
  // First, find out if the point is outside the crystal.

  const TVector3 lLiloVector = lLiloPoint;
  const TVector3 lCentre = theGeom.GetFrontCentre();

  const TVector3 lNormal = theGeom.GetNormalToFrontFace();
  const TVector3 lVector = lCentre - lLiloVector;
  const Double_t lLength = lNormal.Dot(lVector);
  if (lLength < 0.0) {
    // Point is outside crystal (not sure its correct for all cases?).
    // Anyhow, project point back onto front-face.
    const TVector3 lUnit = lLiloVector.Unit();
    const Double_t lMag = (lNormal.Dot(lCentre) / lNormal.Dot(lUnit));
    lLiloPoint.SetMag(lMag);
  } else {
    // Don't project onto front-face. Keep the centroid inside the crytal.
    if (lLogNum > 1) {
      // Use logarithmic centroid position
      lLiloPoint.SetMag(lLogSum.Mag());
    } else {
      // Use linear centroid position
      lLiloPoint.SetMag(lLinSum.Mag());
    }
  }
  return lLiloPoint;
}

BSEmcCrystalPositionData BSEmcPositionAlgo::GetClusterCrystal(const BSEmcCluster *t_cluster, Double_t t_theta, Double_t t_phi) const
{
  // Find crystal with minimal angular difference with given direction
  TVector3 vec{0, 0, 10};
  vec.SetTheta(t_theta);
  vec.SetPhi(t_phi);
  Double_t diff = 1000;
  BSEmcCrystalPositionData closestcrystal = {};
  Double_t tmpDiff = 0.0;
  for (const auto &digi : t_cluster->GetDigis()) {
    const BSEmcCrystalPositionData &crystal = fPositionPar->GetPositionData(digi.fDetectorId);
    tmpDiff = crystal.GetFrontCentre().Angle(vec);

    if (tmpDiff < diff) {
      closestcrystal = crystal;
      diff = tmpDiff;
    }
  }

  return closestcrystal;
}
