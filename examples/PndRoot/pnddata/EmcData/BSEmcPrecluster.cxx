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
// Description:
//     Precluster object that also stores the base digi information.
//
// Author List:
//	Marcel Tiemens	University of Groningen

//---------------------------------------------------------------------

#include "BSEmcPrecluster.h"

#include "Rtypes.h"     // for ClassImp
#include "RtypesCore.h" // for Double_t, Int_t

#include "BSEmcCluster.h"

BSEmcPrecluster::BSEmcPrecluster() : BSEmcCluster(), fDigiTimeArray(), fDigiEnergyArray(), fDigiXposArray(), fDigiYposArray(), fDigiZposArray()
{
  fDigiTimeArray.clear();
  fDigiEnergyArray.clear();
  fDigiXposArray.clear();
  fDigiYposArray.clear();
  fDigiZposArray.clear();
}

BSEmcPrecluster::BSEmcPrecluster(const BSEmcPrecluster &t_other)
  : BSEmcCluster(t_other), fDigiTimeArray(t_other.fDigiTimeArray), fDigiEnergyArray(t_other.fDigiEnergyArray), fDigiXposArray(t_other.fDigiXposArray),
    fDigiYposArray(t_other.fDigiYposArray), fDigiZposArray(t_other.fDigiZposArray)
{
}

BSEmcPrecluster::~BSEmcPrecluster() {}

void BSEmcPrecluster::AddDigiInfo(Int_t t_digiIdx, Int_t t_detectorId, Double_t t_energy, Double_t t_time, Double_t t_x, Double_t t_y, Double_t t_z)
{
  fDigis.push_back({t_digiIdx, t_detectorId, 1});
  fDigiTimeArray.push_back(t_time);
  fDigiEnergyArray.push_back(t_energy);
  fDigiXposArray.push_back(t_x);
  fDigiYposArray.push_back(t_y);
  fDigiZposArray.push_back(t_z); // always use real z position
  invalidateCache(kFALSE);
}

ClassImp(BSEmcPrecluster)
