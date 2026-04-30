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
 *@updated 02.02.2021
 *@version 1.0
 **
 ** PANDA  class to correct a PndTrack: only hits which are close enough to the track are added to the track
 ** Task level RECO
 **/

#include "TVector3.h"
#include "TMath.h"
#include "PndTrackCand.h"
#include "PndHoughTrackCorrection.h"

ClassImp(PndHoughTrackCorrection)

  /**
   * @brief      Determines the first hit of a track as the one closest to the interaction point.
   *
   * @param[in]  trackCand  The track candidate
   * @param[in]  circle     The circle corresponding to the track.
   *
   * @return     The first hit of the track candidate.
   */
  TVector3 PndHoughTrackCorrection::FindMinimumHit(PndTrackCand &trackCand, std::vector<double> &circle)
{
  TVector3 Hitmin;
  double dmin;

  for (int i = 0; i < trackCand.GetNHits(); i++) {
    FairLink link = trackCand.GetSortedHit(i);
    FairHit *hit = fMapFairLinktoFairHit[link];
    double rIso = fMapFairLinktoIsochrone[link];
    TVector3 myHit(hit->GetX(), hit->GetY(), rIso);
    TVector2 HitOnCircle = fPndHoughUtilities->calcPointOnCircle(i, trackCand, circle);

    double d = sqrt(HitOnCircle.X() * HitOnCircle.X() + HitOnCircle.Y() * HitOnCircle.Y());
    if (i == 0) {
      Hitmin = myHit;
      dmin = d;
    }

    if (d < dmin) {
      Hitmin = myHit;
      dmin = d;
    }
  }
  return Hitmin;
}

/**
 * @brief      Calculates rho for a hit. It is calculated as the path length of the particle track.
 *
 * @param[in]  Hit     The hit
 * @param[in]  Track   The coresponding track
 * @param[in]  Hitmin  The first hit of the track candidate.
 *
 * @return     The rho value for the hit.
 */
double PndHoughTrackCorrection::calc_rho(TVector3 &Hit, TVector3 &Track, TVector3 &Hitmin)
{
  Double_t r = Track.Z(); // radius of apollonius circle

  // Phi calculation

  // slope of straight line from InteractionPoint to center of Track
  double m = Track.Y() / Track.X();
  // determin hit with the minimum distance to interaction point

  // check on wich side the hit with the minimum distance to interaction point is
  int check;
  if (Hitmin.Y() > Hitmin.X() * m)
    check = -1;
  else
    check = 1;

  bool same_plane = kFALSE;
  int check2;
  if (Hit.Y() > Hit.X() * m)
    check2 = -1;
  else
    check2 = 1;

  if (check == check2)
    same_plane = kTRUE;

  TVector2 u(-Track.X(), -Track.Y());
  TVector2 v(Hit.X() - Track.X(), Hit.Y() - Track.Y());
  Double_t phi;
  if (same_plane) {
    phi = TMath::ACos((u.X() * v.X() + u.Y() * v.Y()) / (sqrt(u.X() * u.X() + u.Y() * u.Y()) * sqrt(v.X() * v.X() + v.Y() * v.Y())));
  } else {
    phi = 2 * TMath::Pi() - TMath::ACos((u.X() * v.X() + u.Y() * v.Y()) / (sqrt(u.X() * u.X() + u.Y() * u.Y()) * sqrt(v.X() * v.X() + v.Y() * v.Y())));
  }
  Double_t rho = abs(r) * phi;

  return rho;
}

/**
 * @brief      Corrects the track candidate: only hits are used that are close to the calculated track (d < 0.5 cm).
 *
 * @param[in]  trackCand     The track candidate
 * @param[in]  Track         The determined circle corresponding to the track
 * @param[in]  trackCandTot  The track candidate (without cleaning and with skewed hits)
 *
 * @return     All hits NOT(!) belonging to the track.
 */
PndTrackCand PndHoughTrackCorrection::calcCorrectTrackCand(PndTrackCand &trackCand, TVector3 &Track, PndTrackCand &trackCandTot)
{
  PndTrackCand CorrectedTrackCand_temp;
  PndTrackCand RemainingTrackCand;
  PndTrackCand CorrectedTrackCand;

  double trackX = Track.X();
  double trackY = Track.Y();
  double trackZ = Track.Z();
  for (int i = 0; i < trackCandTot.GetNHits(); i++) {
    FairLink link = trackCandTot.GetSortedHit(i);
    PndTrackCandHit hit_temp = trackCandTot.GetSortedHit(i);
    FairHit *hit = fMapFairLinktoFairHit[link];
    double rIso = fMapFairLinktoIsochrone[link];
    TVector3 myHit(hit->GetX(), hit->GetY(), rIso);
    TString branchName = ioman->GetBranchName(link.GetType());
    Double_t rho_temp = hit_temp.GetRho();

    double hitX = myHit.X();
    double hitY = myHit.Y();
    double hitZ = myHit.Z();
    double d = fPndHoughUtilities->calcDistanceFromIsochroneToCircle(hitX, hitY, hitZ, trackX, trackY, trackZ);

    double tube_diameter = 0.5;

    std::vector<PndTrackCandHit> HitVector = trackCand.GetSortedHits();
    auto it = find_if(HitVector.begin(), HitVector.end(), [&link](const PndTrackCandHit &obj) { return (FairLink)obj == link; });
    if (d < tube_diameter) {
      CorrectedTrackCand_temp.AddHit(link, rho_temp);
    } else if (d >= tube_diameter && it != HitVector.end()) {
      RemainingTrackCand.AddHit(link, rho_temp);
    }
  }
  // change order (rho values for corrected TrackCand)
  std::vector<double> circle;
  circle.push_back(Track.X());
  circle.push_back(Track.Y());
  circle.push_back(Track.Z());
  TVector3 Hitmin = FindMinimumHit(CorrectedTrackCand_temp, circle);
  /*
  for (int i = 0; i < CorrectedTrackCand_temp.GetNHits(); i++) {
    FairLink link = CorrectedTrackCand_temp.GetSortedHit(i);
    FairHit *hit = fMapFairLinktoFairHit[link];
    double rIso = fMapFairLinktoIsochrone[link];
    TVector3 myHit(hit->GetX(), hit->GetY(), rIso);

    double rho = calc_rho(myHit, Track, Hitmin);
    CorrectedTrackCand.AddHit(link, rho);
  }
  fApolloniusTrackCand = CorrectedTrackCand;
  */
  fApolloniusTrackCand = changeHitOrder(CorrectedTrackCand_temp, Track, Hitmin);
  return RemainingTrackCand;
}

/**
 * @brief      Sorts the hits of a track cand based on the rho value depending on a specific start hit (Hitmin)
 *
 * @param[in]  trackCand     The track candidate
 * @param[in]  Track         The determined circle corresponding to the track
 * @param[in]  Hitmin        The start hit of the track
 *
 * @return     The sorted trackCand
 */
PndTrackCand PndHoughTrackCorrection::changeHitOrder(PndTrackCand &trackCand, TVector3 &Track, TVector3 &Hitmin)
{
  PndTrackCand CorrectedTrackCand;
  // change order (rho values for corrected TrackCand)
  for (int i = 0; i < trackCand.GetNHits(); i++) {
    FairLink link = trackCand.GetSortedHit(i);
    FairHit *hit = fMapFairLinktoFairHit[link];
    double rIso = fMapFairLinktoIsochrone[link];
    TVector3 myHit(hit->GetX(), hit->GetY(), rIso);
    LOG(debug1) << "changeHitOrder (" << myHit.X() << "," << myHit.Y() << ") HitMin (" << Hitmin.X() << "," << Hitmin.Y() << ")" << std::endl;
    double rho = calc_rho(myHit, Track, Hitmin);
    CorrectedTrackCand.AddHit(link, rho);
  }

  return CorrectedTrackCand;
}
