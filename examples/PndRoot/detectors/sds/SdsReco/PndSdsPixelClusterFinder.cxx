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

#include "PndSdsPixelClusterFinder.h"
#include "PndSdsIdealChargeConversion.h"
#include "PndSdsTotChargeConversion.h"

ClassImp(PndSdsPixelClusterFinder);

// PndSdsPixelClusterFinder::PndSdsPixelClusterFinder(std::vector<PndSdsDigiPixel> hits){
//     fHits   = hits;
//      fVerbose = 0;
//}

void PndSdsPixelClusterFinder::Print(std::vector<PndSdsDigiPixel> hits)
{
  std::cout << "--------------" << std::endl;
  for (UInt_t i = 0; i < hits.size(); i++) {
    std::cout << hits[i] << std::endl;
  }
}

void PndSdsPixelClusterFinder::Print()
{
  Print(fHits);
}

void PndSdsPixelClusterFinder::PrintResult(std::vector<std::vector<Int_t>> clusters)
{
  for (UInt_t i = 0; i < clusters.size(); i++) {
    std::cout << "PndSdsPixelClusterFinder::PrintResult(): Cluster " << i << ": " << std::endl;
    for (UInt_t j = 0; j < clusters[i].size(); j++)
      fHits[clusters[i][j]].Print();
  }
}
