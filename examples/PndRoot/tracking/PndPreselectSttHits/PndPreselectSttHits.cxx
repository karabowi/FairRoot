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

/////////////////////////////////////////////////////////////
//  PndPreselectSttHits
//  Track Finder for primaries and secondaries in Panda's barrel part
/////////////////////////////////////////////////////////////////

/** PndHoughApollonius
 *@author Anna Alicke <a.alicke@fz-juelich.de>
 *@since 14.09.2021
 *@version 1.0
 **
 ** Track Finder for primaries and Secondaries in Panda's barrel part
 ** Task level RECO
 **/

#include "PndPreselectSttHits.h"

PndPreselectSttHits::~PndPreselectSttHits() {}

std::vector<PndTrackCand> PndPreselectSttHits::GetPndTrackCands(std::vector<std::vector<PndSttHit *>> &solutions)
{
  std::vector<PndTrackCand> AllCands;
  AllCands.clear();
  for (auto solution : solutions) {
    if (solution.size() == 0)
      continue;

    PndTrackCand cand;
    for (auto hit : solution) {
      double phi = TMath::ATan2(hit->GetY(), hit->GetX()) * 180 / TMath::Pi();
      double r = sqrt(pow(hit->GetX(), 2) + pow(hit->GetY(), 2));
      double rho = r * phi * TMath::Pi() / 180;

      cand.AddHit(hit->GetEntryNr(), rho);
    }
    AllCands.push_back(cand);
  }
  return AllCands;
}
