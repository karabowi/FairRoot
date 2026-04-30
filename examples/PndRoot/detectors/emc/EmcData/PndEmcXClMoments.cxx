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
//	Class PndEmcXClMoments
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

#include "PndEmcXClMoments.h"
#include "PndEmcClusterProperties.h"
#include "PndEmcTwoCoordIndex.h"
#include "PndEmcCluster.h"
#include "PndEmcDigi.h"
#include "TClonesArray.h"
#include <vector>
#include <cmath>

using std::endl;
using std::ostream;

PndEmcXClMoments::PndEmcXClMoments(const PndEmcCluster &cluster, const TClonesArray *digiArray)
  : PndEmcAbsClusterProperty(cluster, digiArray), fEnergyDistribution(), fClusterSize(0)
{
  Init();
}

PndEmcXClMoments::PndEmcXClMoments(const PndEmcXClMoments &other)
  : PndEmcAbsClusterProperty(other), fEnergyDistribution(other.fEnergyDistribution), fClusterSize(other.fClusterSize)
{
  Init();
}

//--------------
// Destructor --
//--------------

PndEmcXClMoments::~PndEmcXClMoments()
{
  fEnergyDistribution->clear();
  delete fEnergyDistribution;
  fEnergyDistribution = 0;
}

Double_t PndEmcXClMoments::SecondMoment() const
{
  Double_t r, sum = 0;
  for (int i = 0; i < fClusterSize; i++) {
    r = (*fEnergyDistribution)[i].r;
    sum += (*fEnergyDistribution)[i].deposited_energy * r * r;
  }
  sum /= MyCluster().GetEnergy();

  return sum;
}

Double_t PndEmcXClMoments::SecondMomentPhi() const
{
  Double_t r, sum = 0;
  for (int i = 0; i < fClusterSize; i++) {
    r = (*fEnergyDistribution)[i].r * sin((*fEnergyDistribution)[i].phi);
    sum += (*fEnergyDistribution)[i].deposited_energy * r * r;
  }
  sum /= MyCluster().GetEnergy();

  return sum;
}

Double_t PndEmcXClMoments::SecondMomentTheta() const
{
  Double_t r, sum = 0;
  for (int i = 0; i < fClusterSize; i++) {
    r = (*fEnergyDistribution)[i].r * cos((*fEnergyDistribution)[i].phi);
    sum += (*fEnergyDistribution)[i].deposited_energy * r * r;
  }
  sum /= MyCluster().GetEnergy();

  return sum;
}

Double_t PndEmcXClMoments::AbsZernikeMoment(int n, int m, Double_t R0) const
{
  // 1. Check if n,m are correctly
  if ((m > n) || ((n - m) % 2 != 0) || (n < 0) || (m < 0)) {
    return -1;
  }

  // 2. Check if n,R0 are within validity Range :
  // n>20 or R0<5cm  just makes so sense !
  if ((n > 20) || (R0 <= 5)) {
    return -1;
  }
  if (n <= 5) {
    return Fast_AbsZernikeMoment(n, m, R0);
  } else {
    return Calc_AbsZernikeMoment(n, m, R0);
  }
}

Double_t PndEmcXClMoments::Lat() const
{
  Double_t r, redmoment = 0;
  int n, n1, n2, tmp;
  if (fClusterSize < 3) {
    return 0;
  }

  n1 = 0;
  n2 = 1;
  if ((*fEnergyDistribution)[1].deposited_energy > (*fEnergyDistribution)[0].deposited_energy) {
    tmp = n2;
    n2 = n1;
    n1 = tmp;
  }
  for (int i = 2; i < fClusterSize; i++) {
    n = i;
    if ((*fEnergyDistribution)[i].deposited_energy > (*fEnergyDistribution)[n1].deposited_energy) {
      tmp = n2;
      n2 = n1;
      n1 = i;
      n = tmp;
    } else {
      if ((*fEnergyDistribution)[i].deposited_energy > (*fEnergyDistribution)[n2].deposited_energy) {
        tmp = n2;
        n2 = i;
        n = tmp;
      }
    }
    r = (*fEnergyDistribution)[n].r;
    redmoment += r * r * (*fEnergyDistribution)[n].deposited_energy;
  }
  Double_t e1 = (*fEnergyDistribution)[n1].deposited_energy;
  Double_t e2 = (*fEnergyDistribution)[n2].deposited_energy;
  Double_t r0 = 2.5; // cm, the average distance between crystals
  Double_t lat = redmoment / (redmoment + r0 * r0 * (e1 + e2));

  return lat;
}

void PndEmcXClMoments::Print(const Option_t *) const
{
  TVector3 cl(MyCluster().where());
  TVector3 ClusDirection(cl.x(), cl.y(), cl.z());
  ClusDirection *= 1.0 / ClusDirection.Mag();
  TVector3 theta_axis(ClusDirection.y(), -ClusDirection.x(), 0.0);
  theta_axis *= 1.0 / theta_axis.Mag();
  TVector3 phi_axis = theta_axis.Cross(ClusDirection);

  std::vector<Int_t> digiList = MyCluster().DigiList();
  std::vector<Int_t>::iterator current;

  PndEmcClEnergyDeposition clEdep;
  std::cout << "Dump of PndEmcCluster with CoG at (" << cl.x() << "," << cl.y() << "," << cl.z() << ")" << endl;
  std::cout << "-------------------------------------------------------------" << endl;
  std::cout << " theta - axis : (" << theta_axis.x() << "," << theta_axis.y() << "," << theta_axis.z() << ")" << endl;
  std::cout << " phi - axis : (" << phi_axis.x() << "," << phi_axis.y() << "," << phi_axis.z() << ")" << endl;
  int i = 0;

  for (current = digiList.begin(); current < digiList.end(); ++current) {
    clEdep = (*fEnergyDistribution)[i];
    PndEmcDigi *digi = (PndEmcDigi *)DigiArray()->At(*current);
    digi->Print();
    std::cout << "Polar coords : (" << clEdep.r << "," << clEdep.phi << ")" << endl;
    i++;
  }

  for (int j = 0; j <= 8; j++) {
    std::cout << "f[" << j << "](5) = " << (this->*fFcn[j])(5.0) << endl;
  }

  std::cout << "--------------------------------------------------------------" << endl;
}

Double_t PndEmcXClMoments::f00(Double_t) const
{
  return 1;
} // r   //[R.K.03/2017] unused variable(s)

Double_t PndEmcXClMoments::f11(Double_t r) const
{
  return r;
}

Double_t PndEmcXClMoments::f20(Double_t r) const
{
  return 2.0 * r * r - 1.0;
}

Double_t PndEmcXClMoments::f22(Double_t r) const
{
  return r * r;
}

Double_t PndEmcXClMoments::f31(Double_t r) const
{
  return 3.0 * r * r * r - 2.0 * r;
}

Double_t PndEmcXClMoments::f33(Double_t r) const
{
  return r * r * r;
}

Double_t PndEmcXClMoments::f40(Double_t r) const
{
  return 6.0 * r * r * r * r - 6.0 * r * r + 1.0;
}

Double_t PndEmcXClMoments::f42(Double_t r) const
{
  return 4.0 * r * r * r * r - 3.0 * r * r;
}

Double_t PndEmcXClMoments::f44(Double_t r) const
{
  return r * r * r * r;
}

Double_t PndEmcXClMoments::f51(Double_t r) const
{
  return 10.0 * pow(r, 5) - 12.0 * pow(r, 3) + 3.0 * r;
}

Double_t PndEmcXClMoments::f53(Double_t r) const
{
  return 5.0 * pow(r, 5) - 4.0 * pow(r, 3);
}

Double_t PndEmcXClMoments::f55(Double_t r) const
{
  return pow(r, 5);
}

Double_t PndEmcXClMoments::Fast_AbsZernikeMoment(int n, int m, Double_t R0) const
{
  Double_t r, ph, e, Re = 0, Im = 0, result;
  Double_t TotalEnergy = MyCluster().GetEnergy();
  int index = (n / 2) * (n / 2) + (n / 2) + m;
  for (int i = 0; i < fClusterSize; i++) {
    r = (*fEnergyDistribution)[i].r / R0;
    if (r < 1) {
      ph = ((*fEnergyDistribution)[i]).phi;
      e = (*fEnergyDistribution)[i].deposited_energy;
      Re = Re + e / TotalEnergy * (this->*fFcn[index])(r)*cos((Double_t)m * ph);
      Im = Im - e / TotalEnergy * (this->*fFcn[index])(r)*sin((Double_t)m * ph);
    }
  }
  result = sqrt(Re * Re + Im * Im);

  return result;
}

Double_t PndEmcXClMoments::Calc_AbsZernikeMoment(int n, int m, Double_t R0) const
{
  Double_t r, ph, e, Re = 0, Im = 0, f_nm, result;
  Double_t TotalEnergy = MyCluster().GetEnergy();
  for (int i = 0; i < fClusterSize; i++) {
    r = (*fEnergyDistribution)[i].r / R0;
    if (r < 1) {
      ph = ((*fEnergyDistribution)[i]).phi;
      e = (*fEnergyDistribution)[i].deposited_energy;
      f_nm = 0;
      for (int s = 0; s <= (n - m) / 2; s++) {
        if (s % 2 == 0) {
          f_nm = f_nm + Fak(n - s) * pow(r, (Double_t)(n - 2 * s)) / (Fak(s) * Fak((n + m) / 2 - s) * Fak((n - m) / 2 - s));
        } else {
          f_nm = f_nm - Fak(n - s) * pow(r, (Double_t)(n - 2 * s)) / (Fak(s) * Fak((n + m) / 2 - s) * Fak((n - m) / 2 - s));
        }
      }
      Re = Re + e / TotalEnergy * f_nm * cos((Double_t)m * ph);
      Im = Im - e / TotalEnergy * f_nm * sin((Double_t)m * ph);
    }
  }
  result = sqrt(Re * Re + Im * Im);

  return result;
}

Double_t PndEmcXClMoments::Fak(int n) const
{
  Double_t res = 1.0;
  for (int i = 2; i <= n; i++) {
    res = res * (Double_t)i;
  }

  return res;
}

void PndEmcXClMoments::Init()
{
  // need to get one digi to ask what kind of digi position method is used
  // or to be more precise what scaling factor has do be used
  Double_t rescaleFactor = 1.;
  PndEmcClusterProperties *properties = new PndEmcClusterProperties(MyCluster(), DigiArray());

  const PndEmcDigi *pDigi = MyCluster().Maxima(DigiArray());
  if (pDigi != 0)
    rescaleFactor = pDigi->getRescaleFactor();

  // make sure we always use gravWhere for the cluster moments
  //  TVector3 cl( MyCluster().where() );
  TVector3 cl(properties->GravWhere());
  TVector3 ClusDirection(cl.x(), cl.y(), cl.z());
  ClusDirection *= 1.0 / ClusDirection.Mag();
  TVector3 theta_axis(ClusDirection.y(), -ClusDirection.x(), 0.0);
  theta_axis *= 1.0 / theta_axis.Mag();
  TVector3 phi_axis = theta_axis.Cross(ClusDirection);

  std::vector<Int_t> digiList = MyCluster().DigiList();
  std::vector<Int_t>::iterator current;

  fClusterSize = digiList.size();
  fEnergyDistribution = new std::vector<PndEmcClEnergyDeposition>(fClusterSize);

  int i = 0;
  PndEmcClEnergyDeposition clEdep;

  for (current = digiList.begin(); current < digiList.end(); ++current) {
    PndEmcDigi *digi = (PndEmcDigi *)DigiArray()->At(*current);
    clEdep.deposited_energy = digi->GetEnergy();

    TVector3 diff = digi->where() - cl;
    TVector3 DigiVect = diff - diff.Dot(ClusDirection) * ClusDirection;
    clEdep.r = DigiVect.Mag() / rescaleFactor;
    clEdep.phi = DigiVect.Angle(theta_axis);
    if (DigiVect.Dot(phi_axis) < 0) {
      clEdep.phi = 2 * M_PI - clEdep.phi;
    }

    (*fEnergyDistribution)[i] = clEdep;
    i++;
  }

  fFcn[0] = &PndEmcXClMoments::f00;
  fFcn[1] = &PndEmcXClMoments::f11;
  fFcn[2] = &PndEmcXClMoments::f20;
  fFcn[3] = &PndEmcXClMoments::f31;
  fFcn[4] = &PndEmcXClMoments::f22;
  fFcn[5] = &PndEmcXClMoments::f33;
  fFcn[6] = &PndEmcXClMoments::f40;
  fFcn[7] = &PndEmcXClMoments::f51;
  fFcn[8] = &PndEmcXClMoments::f42;
  fFcn[9] = &PndEmcXClMoments::f53;
  fFcn[10] = &PndEmcXClMoments::f44;
  fFcn[11] = &PndEmcXClMoments::f55;
}

ClassImp(PndEmcXClMoments)
