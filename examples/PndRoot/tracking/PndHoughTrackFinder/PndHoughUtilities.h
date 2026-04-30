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
//  PndHoughUtilities
//  This class contains different methods to handle PndTracks vs Circles
/////////////////////////////////////////////////////////////////

/** PndHoughUtilities
 *@author Anna Alicke <a.alicke@fz-juelich.de>
 *@since 29.10.2018
 *@updated 23.11.2021
 *@version 3.1
 **
 ** PANDA class to handle PndTracks vs Circles
 ** Task level RECO
 **/

#ifndef PndHoughUtilities_H_
#define PndHoughUtilities_H_
#include "FairTask.h"
#include "FairRootManager.h"
//#include "PndGeoSttPar.h"
//#include "PndSttGeometryMap.h"
//#include "PndSttStrawMap.h"
//#include "PndSttCellTrackFinder.h"
#include "PndSttHit.h"
#include "PndMCTrack.h"

//#include "TEllipse.h"
#include "TH2F.h"
#include "TGraph.h"
//#include "TCanvas.h" // DRAW

#include <vector>
#include <tuple>
//#include "TDatabasePDG.h"
#include "PndHoughData.h"

class PndHoughUtilities {

 public:
  PndHoughUtilities(PndHoughData *fData) : fMapFairLinktoFairHit(fData->GetMapFairLinktoFairHit()), fMapFairLinktoIsochrone(fData->GetMapFairLinktoIsochrone())
  {
    ioman = FairRootManager::Instance();
  };

  PndHoughUtilities() { ioman = FairRootManager::Instance(); };

  virtual ~PndHoughUtilities(){};

  /** @brief Determines the circle from a PndTrack. */
  std::vector<double> getCircleFromPndTrack(PndTrack &tr);
  /** @brief Determines the transverse momentum. */
  double Pt(double B, double r);
  /** @brief Determines the charge q and the momentum p of a hit */
  std::tuple<TVector3, Int_t> getPandqforHit(int i, double B, TVector3 &circle, PndTrackCand &myCand);
  /** @brief Helper function to find PndTrack parameters from a circle */
  FairTrackParP getTrackParPForHit(Int_t i, Double_t B, PndTrackCand &myCand, TVector3 &circle);
  /** @brief Determined the PndTrack for a given track candidate and a circle. */
  PndTrack getPndTrack(Double_t B, PndTrackCand &myCand, TVector3 &circle);

  /** @brief Finds the point on a circle closest to the given hit point */
  TVector2 calcPointOnCircle(Int_t i, PndTrackCand &myCand, std::vector<double> &circle);
  /** @brief Finds the correct intersection point. */
  TVector2 findCorrectIntersectionPoint(std::vector<double> &intersectionPoints, TVector2 &point);
  /** @brief Finds the intersection point between a line and a circle. */
  std::vector<double> calcIntersectionPointCircleLine(std::vector<double> &circle, double m, double b, double Ax);

  /** @brief Calculates the sum of distances from each hit of a track to the track circle */
  double calcDistanceTrackCandToTrack(PndTrackCand &trackCand, std::vector<double> &circle);
  /** @brief Calculates the distance from a  hitpoint (MVD hit or nearest point of stt isochrone to track) to a circle from hough space. */
  double calcDistanceFromIsochroneToCircle(double &xi, double &yi, double &ri, double &xT, double &yT, double &rT);

 private:
  FairRootManager *ioman = nullptr;

  std::map<FairLink, FairHit *> fMapFairLinktoFairHit;
  std::map<FairLink, double> fMapFairLinktoIsochrone;

  ClassDef(PndHoughUtilities, 1);
};

#endif /*PndHoughUtilities_H_*/
