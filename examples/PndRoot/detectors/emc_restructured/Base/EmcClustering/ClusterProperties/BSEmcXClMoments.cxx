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
//	Class BSEmcXClMoments
//      A class for description of shower shapes
//
//      Stefan Christ:
//      Rescaled the lateral distance r by 1/rescaleFactor
//      This keeps the momentens roughly the same although the digi position
//      was changed
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	Thorsten Brandt 	Originator
//
// Copyright Information:
//
// Dima Melnichuk, adaption for pandaroot
//
//------------------------------------------------------------------------

#include "BSEmcXClMoments.h"

#include <cmath>
#include <vector>

#include "TVector3.h"

#include "BSEmcCluster.h"
#include "BSEmcCrystalPositionPar.h"
#include "BSEmcDigi.h"

BSEmcXClMoments::BSEmcXClMoments() : fCurrentCluster(nullptr), fPositionPar(nullptr), fClusterSize(0)
{
  Init();
}
//--------------
// Destructor --
//--------------

BSEmcXClMoments::~BSEmcXClMoments() {}

void BSEmcXClMoments::Init()
{
  fFcn[0] = &BSEmcXClMoments::f00;
  fFcn[1] = &BSEmcXClMoments::f11;
  fFcn[2] = &BSEmcXClMoments::f20;
  fFcn[3] = &BSEmcXClMoments::f31;
  fFcn[4] = &BSEmcXClMoments::f22;
  fFcn[5] = &BSEmcXClMoments::f33;
  fFcn[6] = &BSEmcXClMoments::f40;
  fFcn[7] = &BSEmcXClMoments::f51;
  fFcn[8] = &BSEmcXClMoments::f42;
  fFcn[9] = &BSEmcXClMoments::f53;
  fFcn[10] = &BSEmcXClMoments::f44;
  fFcn[11] = &BSEmcXClMoments::f55;
}

Double_t BSEmcXClMoments::f00(Double_t /*unused*/) const
{
  return 1;
}
Double_t BSEmcXClMoments::f11(Double_t r) const
{
  return r;
}
Double_t BSEmcXClMoments::f20(Double_t r) const
{
  return 2.0 * r * r - 1.0;
}
Double_t BSEmcXClMoments::f22(Double_t r) const
{
  return r * r;
}
Double_t BSEmcXClMoments::f31(Double_t r) const
{
  return 3.0 * r * r * r - 2.0 * r;
}
Double_t BSEmcXClMoments::f33(Double_t r) const
{
  return r * r * r;
}
Double_t BSEmcXClMoments::f40(Double_t r) const
{
  return 6.0 * r * r * r * r - 6.0 * r * r + 1.0;
}
Double_t BSEmcXClMoments::f42(Double_t r) const
{
  return 4.0 * r * r * r * r - 3.0 * r * r;
}
Double_t BSEmcXClMoments::f44(Double_t r) const
{
  return r * r * r * r;
}
Double_t BSEmcXClMoments::f51(Double_t r) const
{
  return 10.0 * pow(r, 5) - 12.0 * pow(r, 3) + 3.0 * r;
}
Double_t BSEmcXClMoments::f53(Double_t r) const
{
  return 5.0 * pow(r, 5) - 4.0 * pow(r, 3);
}
Double_t BSEmcXClMoments::f55(Double_t r) const
{
  return pow(r, 5);
}

Moments BSEmcXClMoments::CalculateMoments(const BSEmcCluster *t_cluster, const std::vector<const BSEmcDigi *> &t_digiArray)
{
  fCurrentCluster = t_cluster;
  fClusterSize = t_cluster->GetDigis().size();
  std::vector<BSEmcClEnergyDeposition> energydeposition = CreateEnergyDeposition(t_cluster, t_digiArray);

  Moments moments;
  moments.Lateral = Lat(energydeposition);
  moments.Z20 = AbsZernikeMoment(2, 0, 15, energydeposition);
  moments.Z53 = AbsZernikeMoment(5, 3, 15, energydeposition);
  return moments;
}

std::vector<BSEmcXClMoments::BSEmcClEnergyDeposition> BSEmcXClMoments::CreateEnergyDeposition(const BSEmcCluster *t_cluster, const std::vector<const BSEmcDigi *> &t_digiArray)
{
  // need to get one digi to ask what kind of digi position method is used
  // or to be more precise what scaling factor has do be used
  Double_t rescaleFactor = 1.;
  if (fPositionPar != nullptr) {
    rescaleFactor = fPositionPar->GetRescaleFactor();
  }

  // make sure we always use gravWhere for the cluster moments
  TVector3 cl(fPositionAlgo->CalculateGravPosition(fCurrentCluster, t_digiArray));
  TVector3 ClusDirection(cl.x(), cl.y(), cl.z());
  ClusDirection *= 1.0 / ClusDirection.Mag();
  TVector3 theta_axis(ClusDirection.y(), -ClusDirection.x(), 0.0);
  theta_axis *= 1.0 / theta_axis.Mag();
  TVector3 phi_axis = theta_axis.Cross(ClusDirection);

  std::vector<BSEmcDigiInfo_t> digiList = t_cluster->GetDigis();
  std::vector<BSEmcClEnergyDeposition> energyDistribution;
  BSEmcClEnergyDeposition clEdep;
  for (const BSEmcDigiInfo_t &digiInfo : digiList) {
    const BSEmcDigi *digi = t_digiArray.at(digiInfo.fDigiIdx);
    clEdep.deposited_energy = digi->GetEnergy() * digiInfo.fWeight;

    TVector3 diff = fPositionPar->GetPosition(digiInfo.fDetectorId) - cl;
    TVector3 DigiVect = diff - diff.Dot(ClusDirection) * ClusDirection;
    clEdep.r = DigiVect.Mag() / rescaleFactor;
    clEdep.phi = DigiVect.Angle(theta_axis);
    if (DigiVect.Dot(phi_axis) < 0) {
      clEdep.phi = 2 * M_PI - clEdep.phi;
    }
    energyDistribution.push_back(clEdep);
  }
  return energyDistribution;
}

Double_t BSEmcXClMoments::Lat(const std::vector<BSEmcClEnergyDeposition> &t_energyDistribution) const
{
  Double_t r = NAN, redmoment = 0;
  Int_t n = 0, n1 = 0, n2 = 0, tmp = 0;
  if (fClusterSize < 3) {
    return 0;
  }

  n1 = 0;
  n2 = 1;
  if (t_energyDistribution[1].deposited_energy > t_energyDistribution[0].deposited_energy) {
    tmp = n2;
    n2 = n1;
    n1 = tmp;
  }
  for (Int_t i = 2; i < fClusterSize; i++) {
    n = i;
    if (t_energyDistribution[i].deposited_energy > t_energyDistribution[n1].deposited_energy) {
      tmp = n2;
      n2 = n1;
      n1 = i;
      n = tmp;
    } else {
      if (t_energyDistribution[i].deposited_energy > t_energyDistribution[n2].deposited_energy) {
        tmp = n2;
        n2 = i;
        n = tmp;
      }
    }
    r = t_energyDistribution[n].r;
    redmoment += r * r * t_energyDistribution[n].deposited_energy;
  }
  Double_t e1 = t_energyDistribution[n1].deposited_energy;
  Double_t e2 = t_energyDistribution[n2].deposited_energy;
  Double_t r0 = 2.5; // cm, the average distance between crystals
  Double_t lat = redmoment / (redmoment + r0 * r0 * (e1 + e2));

  return lat;
}

Double_t BSEmcXClMoments::AbsZernikeMoment(Int_t t_n, Int_t t_m, Double_t t_r0, const std::vector<BSEmcClEnergyDeposition> &t_energyDistribution) const
{
  // 1. Check if n,m are correctly
  if ((t_m > t_n) || ((t_n - t_m) % 2 != 0) || (t_n < 0) || (t_m < 0)) {
    return -1;
  }

  // 2. Check if n,R0 are within validity Range :
  // n>20 or R0<5cm  just makes so sense !
  if ((t_n > 20) || (t_r0 <= 5)) {
    return -1;
  }
  if (t_n <= 5) {
    return Fast_AbsZernikeMoment(t_n, t_m, t_r0, t_energyDistribution);
  } else {
    return Calc_AbsZernikeMoment(t_n, t_m, t_r0, t_energyDistribution);
  }
}

Double_t BSEmcXClMoments::Fast_AbsZernikeMoment(Int_t t_n, Int_t t_m, Double_t t_r0, const std::vector<BSEmcClEnergyDeposition> &t_energyDistribution) const
{
  Double_t r = NAN, ph = NAN, e = NAN, Re = 0, Im = 0, result = NAN;
  Double_t TotalEnergy = fCurrentCluster->GetEnergy();
  Int_t index = (t_n / 2) * (t_n / 2) + (t_n / 2) + t_m; // TODO: recheck (OSA/ANSI index) j = ( n*(n+2)+m ) / 2
  for (Int_t i = 0; i < fClusterSize; i++) {
    r = t_energyDistribution[i].r / t_r0;
    if (r < 1) {
      ph = (t_energyDistribution[i]).phi;
      e = t_energyDistribution[i].deposited_energy;
      Re = Re + e / TotalEnergy * (this->*fFcn[index])(r)*cos((Double_t)t_m * ph);
      Im = Im - e / TotalEnergy * (this->*fFcn[index])(r)*sin((Double_t)t_m * ph);
    }
  }
  result = sqrt(Re * Re + Im * Im);

  return result;
}

Double_t BSEmcXClMoments::Calc_AbsZernikeMoment(Int_t t_n, Int_t t_m, Double_t t_r0, const std::vector<BSEmcClEnergyDeposition> &t_energyDistribution) const
{
  Double_t r = NAN, ph = NAN, e = NAN, Re = 0, Im = 0, f_nm = NAN, result = NAN;
  Double_t TotalEnergy = fCurrentCluster->GetEnergy();
  for (Int_t i = 0; i < fClusterSize; i++) {
    r = t_energyDistribution[i].r / t_r0;
    if (r < 1) {
      ph = (t_energyDistribution[i]).phi;
      e = t_energyDistribution[i].deposited_energy;
      f_nm = 0;
      for (Int_t s = 0; s <= (t_n - t_m) / 2; s++) {
        if (s % 2 == 0) {
          f_nm = f_nm + Fak(t_n - s) * pow(r, (Double_t)(t_n - 2 * s)) / (Fak(s) * Fak((t_n + t_m) / 2 - s) * Fak((t_n - t_m) / 2 - s));
        } else {
          f_nm = f_nm - Fak(t_n - s) * pow(r, (Double_t)(t_n - 2 * s)) / (Fak(s) * Fak((t_n + t_m) / 2 - s) * Fak((t_n - t_m) / 2 - s));
        }
      }
      Re = Re + e / TotalEnergy * f_nm * cos((Double_t)t_m * ph);
      Im = Im - e / TotalEnergy * f_nm * sin((Double_t)t_m * ph);
    }
  }
  result = sqrt(Re * Re + Im * Im);

  return result;
}

Double_t BSEmcXClMoments::Fak(Int_t t_n) const
{
  Double_t res = 1.0;
  for (Int_t i = 2; i <= t_n; i++) {
    res = res * (Double_t)i;
  }

  return res;
}

ClassImp(BSEmcXClMoments)
