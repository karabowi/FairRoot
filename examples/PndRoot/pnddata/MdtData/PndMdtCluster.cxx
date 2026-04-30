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

#include "PndMdtCluster.h"

// -----   Default constructor   -------------------------------------------
PndMdtCluster::PndMdtCluster() : fDigiList() {}

PndMdtCluster::PndMdtCluster(std::vector<Int_t> list) : fDigiList(list) {}

/** Destructor **/
PndMdtCluster::~PndMdtCluster() {}

Bool_t PndMdtCluster::DigiBelongsToCluster(Int_t digiIndex)
{
  for (unsigned int i = 0; i < fDigiList.size(); i++)
    if (fDigiList[i] == digiIndex)
      return true;

  return false;
}

void PndMdtCluster::Print(const Option_t *opt)
{
  std::cout << *this << " opt = " << opt << std::endl;
}

ClassImp(PndMdtCluster)
