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
//  PndHoughTransformation
//  Hough transformation based on the apollonius problem
/////////////////////////////////////////////////////////////////

/** PndHoughTransformation
 *@author Anna Alicke <a.alicke@fz-juelich.de>
 *@since 29.10.2018
 *@updated 23.11.2021
 *@version 2.0
 **
 ** PANDA class for a hough transformation based on the apollonius problem
 ** Task level RECO
 **/

// Includes from ROOT
#include "TVector3.h"
#include "TMath.h"
#include "PndTrackCand.h"
#include "PndHoughTransformation.h"

// general
using std::cout;
using std::endl;

ClassImp(PndHoughTransformation)

  /**
   * @brief      Main class of the Hough transformation: the Hough space is filled and the maximum is returned
   *
   * @param[in]  trackCand  The track candidate
   *
   * @return     The vector containing all maxima of the Hough space.
   */
  vector<TVector3> PndHoughTransformation::FindMaximaWithHoughTransformation(PndTrackCand &trackCand)
{
  vector<TVector3> TrackCircleVec;
  TrackCircleVec.clear();
  HoughSpace(trackCand);

  if (fData->GetHoughSpace()->GetEntries() > 0)
    fData->GetHoughSpace()->FindMaxima(TrackCircleVec);
  return TrackCircleVec;
}

/**
 * @brief      Here the Hough space is filled by calcualting all Apollonius circles.
 *
 * @param[in]  trackCand  The track candidate.
 */
void PndHoughTransformation::HoughSpace(PndTrackCand &trackCand)
{
  // fHoughSpace->clear();
  fData->GetHoughSpace()->clear();
  std::vector<std::vector<FairLink>> multiplets;
  multiplets.clear();
  multiplets = fPndHoughMultipletCreator->multiplets_creator(trackCand, 2);
  double *apolloniusCircles = new double[multiplets.size() * 6 * 8 + 6 * 8 + 6];

  fPndHoughApollonius->ApolloniusCuda(multiplets, apolloniusCircles);

  Double_t phi, ephi, r, OverR;

  for (int m = 0; m < multiplets.size(); m++) {
    // 8 apollonius circles per multiplet: equal circles are set to a default value of 10000 to avoid double counting
    for (int k = 0; k < 8; k++) {
      if (apolloniusCircles[m * 8 * 6 + k * 6 + 2] != 0) {
        r = abs(apolloniusCircles[m * 8 * 6 + k * 6 + 2]);
        phi = TMath::ATan2(apolloniusCircles[m * 8 * 6 + k * 6 + 1], apolloniusCircles[m * 8 * 6 + k * 6 + 0]) * 180 / TMath::Pi();

        ephi = 180. / TMath::Pi() /
               (1 + (apolloniusCircles[m * 8 * 6 + k * 6 + 1] * apolloniusCircles[m * 8 * 6 + k * 6 + 1] /
                     (apolloniusCircles[m * 8 * 6 + k * 6 + 0] * apolloniusCircles[m * 8 * 6 + k * 6 + 0]))) *
               sqrt((apolloniusCircles[m * 8 * 6 + k * 6 + 1] * apolloniusCircles[m * 8 * 6 + k * 6 + 1] /
                     (apolloniusCircles[m * 8 * 6 + k * 6 + 0] * apolloniusCircles[m * 8 * 6 + k * 6 + 0] * apolloniusCircles[m * 8 * 6 + k * 6 + 0] *
                      apolloniusCircles[m * 8 * 6 + k * 6 + 0]) *
                     apolloniusCircles[m * 8 * 6 + k * 6 + 3] * apolloniusCircles[m * 8 * 6 + k * 6 + 3]) +
                    (apolloniusCircles[m * 8 * 6 + k * 6 + 4] * apolloniusCircles[m * 8 * 6 + k * 6 + 4] /
                     (apolloniusCircles[m * 8 * 6 + k * 6 + 0] * apolloniusCircles[m * 8 * 6 + k * 6 + 0])));
        fData->GetHoughSpace()->Fill(r, phi);
      }
    }
  }

  delete[] apolloniusCircles;
  multiplets.clear();
}

/**
 * @brief      Selects one maximum of many by choosing the one with the smalles RMS to the track
 *
 * @param[in]  trackCand       The track candidate
 * @param[in]  TrackCircleVec  The vector containing all maxima of the Hough space.
 *
 * @return     The maximum of the Hough space.
 */
TVector3 PndHoughTransformation::MaximumSelectiondCandToTrack(PndTrackCand &trackCand, vector<TVector3> &TrackCircleVec)
{
  std::vector<double> dTotsCandToTrack;
  for (int i = 0; i < TrackCircleVec.size(); i++) {

    std::vector<Double_t> circ{TrackCircleVec[i][0], TrackCircleVec[i][1], TrackCircleVec[i][2]};
    double dTot = fPndHoughUtilities->calcDistanceTrackCandToTrack(trackCand, circ);

    dTotsCandToTrack.push_back(dTot);
  }
  int min_element_index = std::min_element(dTotsCandToTrack.begin(), dTotsCandToTrack.end()) - dTotsCandToTrack.begin();

  return TrackCircleVec[min_element_index];
}
