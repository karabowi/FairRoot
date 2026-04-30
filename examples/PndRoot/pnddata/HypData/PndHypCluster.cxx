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

#include "PndHypCluster.h"

PndHypCluster::PndHypCluster(std::vector<Int_t> list) : fClusterList(list), fSide(SensorSide::kTOP) {}

void PndHypCluster::Print()
{
  std::cout << *this;
}

bool PndHypCluster::DigiBelongsToCluster(Int_t digiIndex)
{
  for (unsigned int i = 0; i < fClusterList.size(); i++)
    if (fClusterList[i] == digiIndex)
      return true;

  return false;
}

ClassImp(PndHypCluster)
