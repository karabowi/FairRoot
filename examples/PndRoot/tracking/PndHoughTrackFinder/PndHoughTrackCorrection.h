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
//  PndHoughTrackCorrection
//  A class to correct a PndTrack: only hits which are close enough to the track are added to the track
/////////////////////////////////////////////////////////////////

/** PndHoughTrackCorrection
 *@author Anna Scholl <a.scholl@fz-juelich.de>
 *@since 29.10.2018
 *@updated 23.11.2021
 *@version 1.1
 **
 ** PANDA  class to correct a PndTrack: only hits which are close enough to the track are added to the track
 ** Task level RECO
 **/

#ifndef PndHoughTrackCorrection_H_
#define PndHoughTrackCorrection_H_

#include "PndHoughUtilities.h"
#include "PndHoughData.h"

#include <vector>

class PndHoughTrackCorrection {

 public:
  PndHoughTrackCorrection(PndHoughData *fData)
  {
    ioman = FairRootManager::Instance();
    fMapFairLinktoFairHit = fData->GetMapFairLinktoFairHit();
    fMapFairLinktoIsochrone = fData->GetMapFairLinktoIsochrone();
  };

  virtual ~PndHoughTrackCorrection() { delete ioman; };
  virtual void SetUtilities(PndHoughUtilities *utilities) { fPndHoughUtilities = utilities; };
  /** @brief Determines the first hit of a track as the one closest to the interaction point. */
  TVector3 FindMinimumHit(PndTrackCand &trackCand, std::vector<double> &circle);
  /** @brief Calculates rho for a hit. It is calculated as the path length of the particle track. */
  double calc_rho(TVector3 &Hit, TVector3 &Track, TVector3 &Hitmin);
  /** @brief Corrects the track candidate: only hits are used that are close to the calculated track (d < 0.5 cm). */
  PndTrackCand calcCorrectTrackCand(PndTrackCand &trackCand, TVector3 &Track, PndTrackCand &trackCandTot);
  /** @brief Returns the corrected track candidate. */
  PndTrackCand GetApolloniusTrackCand() { return fApolloniusTrackCand; };
  /** @brief Sorts a trackCand based on the Rho value */
  PndTrackCand changeHitOrder(PndTrackCand &trackCand, TVector3 &Track, TVector3 &Hitmin);

 private:
  PndHoughUtilities *fPndHoughUtilities = nullptr;
  FairRootManager *ioman = nullptr;
  std::map<FairLink, FairHit *> fMapFairLinktoFairHit;
  std::map<FairLink, Double_t> fMapFairLinktoIsochrone;
  PndTrackCand fApolloniusTrackCand;

  ClassDef(PndHoughTrackCorrection, 1);
};

#endif /*PndHoughTrackCorrection_H_*/
