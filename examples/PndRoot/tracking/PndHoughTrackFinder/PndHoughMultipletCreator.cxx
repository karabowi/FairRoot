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

//------------------------------------------------------------------------------
// PndHoughMultipletCreator Creates Multiplets: all possible hit combination
// ///////////////////////////////////////////////////////////////

/** PndHoughMultipletCreator
 *@author Anna Alicke <a.alicke@fz-juelich.de>
 *@since 29.10.2018
 *@updated 02.02.2021
 *@version 3.0
 **
 ** PANDA class for creating all possible hit combinations of two or three hits in a track candidate
 ** Task level RECO
 **/

#include "PndHoughMultipletCreator.h"

/**
 * @brief      creates doublets or triplets from a track Canddidate.
 *
 * @param[in]  trackCandWithoutSkewed  The track candidate without skewed hits.
 * @param[in]  num                     2 if you want to combine two hits (doublets), 3 if you want to combine three hits (triplets).
 *
 * @return     vector of all possible multiplets.
 */
std::vector<std::vector<FairLink>> PndHoughMultipletCreator::multiplets_creator(PndTrackCand trackCandWithoutSkewed, int num)
{
  std::vector<std::vector<FairLink>> multiplet_vec;
  std::vector<FairLink> multiplet(num, FairLink());
  multiplet_vec.clear();

  for (int i = 0; i < trackCandWithoutSkewed.GetNHits(); i++) {
    FairLink hit1 = trackCandWithoutSkewed.GetSortedHit(i);

    for (int j = i + 1; j < trackCandWithoutSkewed.GetNHits(); j++) {
      FairLink hit2 = trackCandWithoutSkewed.GetSortedHit(j);

      if (num > 2) {
        for (int k = j + 1; k < trackCandWithoutSkewed.GetNHits(); k++) {
          FairLink hit3 = trackCandWithoutSkewed.GetSortedHit(k);

          multiplet[0] = hit1;
          multiplet[1] = hit2;
          multiplet[2] = hit3;
          multiplet_vec.push_back(multiplet);
        }
      } else {

        multiplet[0] = hit1;
        multiplet[1] = hit2;
        multiplet_vec.push_back(multiplet);
      }
    }
  }

  return multiplet_vec;
}

ClassImp(PndHoughMultipletCreator)
