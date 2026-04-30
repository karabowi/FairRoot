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
//	Class PndEmcClusterEnergySums
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	Stephen J. Gowdy	Originator
//	Helmut Marsiske
//
// Copyright Information:
//	Copyright (C) 1998	University of Edinburgh
//
// Dima Melnichuk, adaption for pandaroot
//------------------------------------------------------------------------
#include "PndEmcClusterEnergySums.h"
#include "PndEmcCluster.h"
#include "PndEmcDigi.h"
#include "TClonesArray.h"

#include <iostream>

using std::endl;
using std::ostream;

PndEmcClusterEnergySums::PndEmcClusterEnergySums(const PndEmcCluster &toUse, const TClonesArray *digiArray) : PndEmcAbsClusterProperty(toUse, digiArray) {}

PndEmcClusterEnergySums::PndEmcClusterEnergySums(const PndEmcClusterEnergySums &other) : PndEmcAbsClusterProperty(other) {}

//--------------
// Destructor --
//--------------

PndEmcClusterEnergySums::~PndEmcClusterEnergySums() {}

//-------------
// Methods   --
//-------------

Double_t PndEmcClusterEnergySums::energy(Int_t n) const
{
  Double_t sum = 0;

  // If asked for too many digis summed return the whole energy
  if (n > MyCluster().NumberOfDigis())
    return (MyCluster().GetEnergy());

  std::vector<Int_t>::const_iterator digi_iter;
  std::vector<Int_t> digiList = MyCluster().DigiList();
  for (Int_t i = 0; i < n; n++) {
    PndEmcDigi *digi = (PndEmcDigi *)DigiArray()->At(digiList[i]);
    sum += digi->GetEnergy();
  }

  return (sum);
}

Double_t PndEmcClusterEnergySums::energy(Double_t distance) const
{
  Double_t sum = 0;
  std::vector<Int_t>::const_iterator digi_iter;
  std::vector<Int_t> digiList = MyCluster().DigiList();
  for (digi_iter = digiList.begin(); digi_iter != digiList.end(); ++digi_iter) {
    PndEmcDigi *digi = (PndEmcDigi *)DigiArray()->At(*digi_iter);
    if (MyCluster().DistanceToCentre(digi) < distance)
      sum += digi->GetEnergy();
  }

  return (sum);
}

Double_t PndEmcClusterEnergySums::E1() const
{
  const PndEmcDigi *current = MyCluster().Maxima(DigiArray());
  return (current->GetEnergy());
}

Double_t PndEmcClusterEnergySums::E9() const
{
  PndEmcDigi *maxDigi = const_cast<PndEmcDigi *>(MyCluster().Maxima(DigiArray()));

  Double_t sum = maxDigi->GetEnergy();

  std::vector<Int_t>::const_iterator digi_iter;
  std::vector<Int_t> digiList = MyCluster().DigiList();
  for (digi_iter = digiList.begin(); digi_iter != digiList.end(); ++digi_iter) {
    PndEmcDigi *digi = (PndEmcDigi *)DigiArray()->At(*digi_iter);
    if (digi == maxDigi)
      continue;
    if (digi->isNeighbour(maxDigi)) {
      sum += digi->GetEnergy();
    }
  }
  return sum;
}

Double_t PndEmcClusterEnergySums::E25() const
{
  const PndEmcDigi *maxDigi = MyCluster().Maxima(DigiArray());
  Double_t sum = E9();
  std::vector<Int_t> tmpDigiList;

  std::vector<Int_t>::const_iterator current_i, current_j;
  std::vector<Int_t> digiList = MyCluster().DigiList();

  for (current_i = digiList.begin(); current_i != digiList.end(); ++current_i) {
    PndEmcDigi *digi_i = (PndEmcDigi *)DigiArray()->At(*current_i);
    if (digi_i == maxDigi)
      continue;
    if (digi_i->isNeighbour(maxDigi)) {
      for (current_j = digiList.begin(); current_j != digiList.end(); ++current_j) {
        PndEmcDigi *digi_j = (PndEmcDigi *)DigiArray()->At(*current_j);
        if ((digi_j == maxDigi) || (digi_j->isNeighbour(maxDigi)))
          continue;
        if (digi_j->isNeighbour(digi_i)) {
          std::vector<Int_t>::iterator iter;
          iter = std::find(tmpDigiList.begin(), tmpDigiList.end(), *current_j);
          if (iter == tmpDigiList.end()) {
            sum += digi_j->GetEnergy();
            tmpDigiList.push_back(*current_j);
          }
        }
      }
    }
  }

  return sum;
}

Double_t PndEmcClusterEnergySums::E1E9() const
{
  Double_t e9 = E9();
  // Double_t e1e9 = ( e9 != 0. )? e1e9 = E1()/e9 : -1.;
  /*
  Double_t e1e9 = -1;
  if( e9 != 0.0){
   e1e9 = E1()/e9;
   }
 return( e1e9 );
 */
  return (e9 != 0.) ? E1() / e9 : -1.;
}

Double_t PndEmcClusterEnergySums::E9E25() const
{
  Double_t e25 = E25();
  // Double_t e9e25 = ( e25 != 0. )? e9e25 = E9()/e25 : -1.;
  /*
  Double_t e9e25 = -1;
  if( e25 != 0.0){
    e9e25 = E9()/e25;
    }
  return( e9e25 );
  */
  return (e25 != 0.) ? E9() / e25 : -1.;
}

void PndEmcClusterEnergySums::Print(const Option_t *) const
{
  std::cout << " E1=" << E1() << ", E9=" << E9() << ", E25=" << E25();
  std::cout << ", E1/E9=" << E1E9() << ", E9/E25=" << E9E25();
  std::cout << endl;
}

ClassImp(PndEmcClusterEnergySums)
