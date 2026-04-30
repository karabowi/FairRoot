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
//  PndHoughPreprocessing
//  Prepares the data for the HoughTrackFinder. Here double hitted tubes are
//  deleted and only the first hit is chosen. Additionally all skewed hits are
//  removed.
/////////////////////////////////////////////////////////////////

/** PndHoughPreprocessing
 *@author Anna Alicke <a.alicke@fz-juelich.de>
 *@since 07.08.2020
 *@updated 02.02.2021
 *@version 2.0
 **
 ** PANDA task class for preparing data for the HoughTrackFinder
 ** Task level RECO
 **/
#define _USE_MATH_DEFINES

#include "PndHoughPreprocessing.h"
#include "FairHit.h"

ClassImp(PndHoughPreprocessing)

  /**
   * @brief      Deletes all hits, where a tube is hitted twice or more.
                 These are unrealistic hits, since the STT has a dead time where no further hits can be seen. In the case of several hits per tube only the first (comparing time)
   hit is taken.
   *
   * @param[in]  trackCand  The track candidate to be cleaned.
   *
   * @return     The cleaned track candidate.
   */
  PndTrackCand PndHoughPreprocessing::CleanData(PndTrackCand &trackCand)
{
  PndTrackCandHit hit;

  PndTrackCand trackCandCleaned;

  std::map<int, PndTrackCandHit> cleanedHits;
  std::vector<int> hittedTubes;
  cleanedHits.clear();
  hittedTubes.clear();

  for (int i = 0; i < trackCand.GetNHits(); i++) {

    hit = (PndTrackCandHit)trackCand.GetSortedHit(i);
    TString branchName = ioman->GetBranchName(hit.GetType());

    if (branchName == "STTHit") {

      Int_t tubeID = fMapFairLinktoTubeId[(FairLink)hit];
      std::vector<int>::iterator it = find(hittedTubes.begin(), hittedTubes.end(), tubeID);

      if (it == hittedTubes.end()) {
        hittedTubes.push_back(tubeID);
        cleanedHits[tubeID] = hit;
      } else {
        FairHit *hold = fMapTubetoHit[tubeID];
        double told = hold->GetTimeStamp();
        FairHit *hnew = fMapFairLinktoFairHit[(FairLink)hit];
        double tnew = hnew->GetTimeStamp();

        if (told > tnew) {
          cleanedHits[tubeID] = hit;
        }
      }
    } else {
      trackCandCleaned.AddHit((FairLink)hit, hit.GetRho());
    }
  }
  std::map<int, PndTrackCandHit>::iterator it;
  for (it = cleanedHits.begin(); it != cleanedHits.end(); it++) {
    PndTrackCandHit hit_temp = it->second;
    trackCandCleaned.AddHit((FairLink)it->second, hit_temp.GetRho());
  }

  return trackCandCleaned;
}

/**
 * @brief      Deletes all skewed hits, since they are not used in the 2D case. The z component will be included later.
 *
 * @param      trackCand  The track candidiate to be cleaned.
 *
 * @return     The cleaned track candidate.
 */
PndTrackCand PndHoughPreprocessing::RejectSkewed(PndTrackCand &trackCand)
{
  PndTrackCand trackCandWithoutSkewed;
  PndTrackCand trackCandWithoutSkewed_temp;

  for (int i = 0; i < trackCand.GetNHits(); i++) {
    PndTrackCandHit hit = trackCand.GetSortedHit(i);
    TString branchName = ioman->GetBranchName(hit.GetType());
    if (branchName == "STTHit") {
      Int_t tubeID = fMapFairLinktoTubeId[(FairLink)hit];

      if (!fSttGeoH->IsSkewedStraw(tubeID)) {
      //if (!fStrawMap->IsSkewedStraw(tubeID)) {
        Double_t rho_temp = hit.GetRho();
        trackCandWithoutSkewed_temp.AddHit((FairLink)hit, rho_temp);
      }
    } else {
      PndTrackCandHit hit1_temp = trackCand.GetSortedHit(i);
      Double_t rho_temp = hit1_temp.GetRho();

      trackCandWithoutSkewed_temp.AddHit((FairLink)hit1_temp, rho_temp);
    }
  }

  trackCandWithoutSkewed = trackCandWithoutSkewed_temp;

  return trackCandWithoutSkewed;
}

/**
 * @brief      Deletes all hits, where a tube is hitted twice or more and all skewed hits.
               These are unrealistic hits, since the STT has a dead time where no further hits can be seen. In the case of several hits per tube only the first (comparing time) hit
 is taken. The skewed hits are not taken, since they are not used in the 2D case. The z component will be included later.
 *
 * @param      trackCand  The track candidate to be cleaned
 *
 * @return     The cleaned track candidate.
 */
PndTrackCand PndHoughPreprocessing::CleanAndRejectSkewed(PndTrackCand &trackCand)
{
  PndTrackCandHit hit;

  PndTrackCand trackCandCleaned;

  std::map<int, PndTrackCandHit> cleanedHits;
  std::vector<int> hittedTubes;
  cleanedHits.clear();
  hittedTubes.clear();

  for (int i = 0; i < trackCand.GetNHits(); i++) {

    hit = (PndTrackCandHit)trackCand.GetSortedHit(i);
    TString branchName = ioman->GetBranchName(hit.GetType());

    if (branchName == "STTHit") {
      Int_t tubeID = fMapFairLinktoTubeId[(FairLink)hit];

      if (!fSttGeoH->IsSkewedStraw(tubeID)) {
      //if (!fStrawMap->IsSkewedStraw(tubeID)) {
        std::vector<int>::iterator it = find(hittedTubes.begin(), hittedTubes.end(), tubeID);

        if (it == hittedTubes.end()) {
          hittedTubes.push_back(tubeID);
          cleanedHits[tubeID] = hit;
        } else {
          FairHit *hold = fMapTubetoHit[tubeID];
          double told = hold->GetTimeStamp();
          FairHit *hnew = fMapFairLinktoFairHit[(FairLink)hit];
          double tnew = hnew->GetTimeStamp();

          if (told > tnew) {
            cleanedHits[tubeID] = hit;
          }
        }
      }
    } else {
      trackCandCleaned.AddHit((FairLink)hit, hit.GetRho());
    }
  }

  std::map<int, PndTrackCandHit>::iterator it;
  for (it = cleanedHits.begin(); it != cleanedHits.end(); it++) {
    trackCandCleaned.AddHit((FairLink)it->second, it->second.GetRho());
  }

  return trackCandCleaned;
}
