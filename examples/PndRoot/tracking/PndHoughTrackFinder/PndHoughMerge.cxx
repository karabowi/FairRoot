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
//  PndHoughMerge
//  Class for merging single tracklets and reduce the number of ghost tracks
/////////////////////////////////////////////////////////////////

/** PndHoughMerge
 *@author Anna Alicke <a.alicke@fz-juelich.de>
 *@since 23.06.2019
 *@updated 23.11.2021
 *@version 3.1
 **
 ** PANDA class for merging single tracklets and reducing the number of ghost tracks.
 ** Task level RECO
 **/
#define _USE_MATH_DEFINES

// Includes from ROOT
#include "TF1.h"
#include "TVector2.h"
#include "TMath.h"
#include "FairLogger.h"
#include "PndTrack.h"
#include "PndTrackCand.h"
#include "PndHoughMerge.h"
#include "PndHoughTrackCorrection.h"

// general
using std::cout;
using std::endl;
#include <numeric>
#include <algorithm>

ClassImp(PndHoughMerge)

  /**
   * @brief      Here all tracklets are merged.
   *
   *             This function is structured in three parts:
   *
   *              - add all hits of the tracklets which should be merged in one trackCand
   *              - performe a Hough transformation for all hits in the merged trackCand
   *              - perform ghost reduction based on ghost reduction parameters
   *
   * @param[in]  mergeTracks  A vector containig all track indices per track which has to be merged: e.g. 5 tracklets have to be merged into 3 particle tracks [ [tracklet1,
   * tracklet5],[tracklet3,tracklet4],[tracklet2]]
   * @param[in]  Tracks       Vector of all PndTracks to assign an index to a track.
   * @param[in]  TrackCands   Vector of all PndTrackCands to assign an index to a track candidate.
   * @param[in]  fBz          The z component of the magnetic field.
   */
  void PndHoughMerge::mergeSingle(std::vector<std::vector<int>> &mergeTracks, std::vector<PndTrack> &Tracks, std::vector<PndTrackCand> &TrackCands, double fBz)
{
  fData->CreateSTTNeighborhoodData();
  fData->CreateGEMNeighborhoodData();
  fMergedTracks.clear();
  for (int l = 0; l < mergeTracks.size(); l++) {
    LOG(debug1) << "Track " << l << " has " << mergeTracks[l].size() << " subtracks" << endl;

    PndTrack trackTotal;
    std::vector<double> trackcirc;
    trackcirc.clear();
    if ((TrackCands.size() == 0 && mergeTracks[l].size() > 1) || TrackCands.size() > 0) {

      // merging track cand
      TVector3 TrackCircle;
      PndTrackCand mergedCand;
      std::vector<FairLink> links;
      links.clear();
      for (int m = 0; m < mergeTracks[l].size(); m++) {
        PndTrackCand Cand;

        if (TrackCands.size() == 0) {
          PndTrack track = Tracks[mergeTracks[l][m]];
          Cand = track.GetTrackCand();
        } else {

          Cand = TrackCands[mergeTracks[l][m]];
        }

        LOG(debug1) << "track l = " << l << ", m = " << m << " has " << Cand.GetNHits() << " hits" << endl;
        LOG(debug1) << "x: " << fData->GetMapFairLinktoFairHit()[Cand.GetSortedHit(0)]->GetX() << " y: " << fData->GetMapFairLinktoFairHit()[Cand.GetSortedHit(0)]->GetY() << endl;

        for (int n = 0; n < Cand.GetNHits(); n++) {
          PndTrackCandHit hit_temp = Cand.GetSortedHit(n);
          FairLink link = (FairLink)hit_temp;

          if (std::find(links.begin(), links.end(), link) == links.end()) {
            Double_t rho_temp = hit_temp.GetRho();
            mergedCand.AddHit((FairLink)hit_temp, rho_temp);
            links.push_back((FairLink)hit_temp);
          }
        }
      }

      LOG(debug1) << "track l = " << l << " merged cand has " << mergedCand.GetNHits() << " hits" << endl;

      // Hough transformation for merged track cands
      // vector<TVector3> TrackCircleVec = fPndHoughTransformation->FindMaximaWithHoughTransformation(mergedCand);
      // if (TrackCircleVec.empty())
      //  continue;
      // LOG(debug1) << "Number of maxima: " << TrackCircleVec.size() << endl;
      // TrackCircle = TrackCircleVec[0];
      double x_median = findMedian(fMaxima_x[l]);
      double y_median = findMedian(fMaxima_y[l]);
      double r_median = findMedian(fMaxima_r[l]);
      TrackCircle.SetXYZ(x_median, y_median, r_median);

      LOG(debug1) << "maximum x: " << TrackCircle.X() << " y: " << TrackCircle.Y() << " r: " << TrackCircle.Z() << endl;

      if (TrackCircle.X() == 0 && TrackCircle.Y() == 0 && TrackCircle.Z() == 0)
        break;
      if (std::isnan(TrackCircle.X()) || std::isnan(TrackCircle.Y()) || std::isnan(TrackCircle.Z()))
        break;

      // fPndHoughTrackCorrection = new PndHoughTrackCorrection(fData);

      PndTrackCand trackCand = fPndHoughTrackCorrection->calcCorrectTrackCand(mergedCand, TrackCircle, mergedCand);
      fApolloniusTrackCand = fPndHoughTrackCorrection->GetApolloniusTrackCand();
      // delete fPndHoughTrackCorrection;

      LOG(debug1) << "number of hits in merged cand after selection: " << fApolloniusTrackCand.GetNHits() << endl;

      if (fApolloniusTrackCand.GetNHits() < 2)
        continue;
      trackTotal = fPndHoughUtilities->getPndTrack(fBz, fApolloniusTrackCand, TrackCircle);
      trackcirc.push_back(TrackCircle.X());
      trackcirc.push_back(TrackCircle.Y());
      trackcirc.push_back(TrackCircle.Z());
    } else if ((TrackCands.size() == 0 && mergeTracks[l].size() <= 1)) {

      trackTotal = Tracks[mergeTracks[l][0]];
      fApolloniusTrackCand = trackTotal.GetTrackCand();
      trackcirc = fPndHoughUtilities->getCircleFromPndTrack(trackTotal);

      LOG(debug1) << "number of hits in merged cand: " << fApolloniusTrackCand.GetNHits() << endl;
      LOG(debug1) << "maximum x: " << trackcirc[0] << " y: " << trackcirc[1] << " r: " << trackcirc[2] << endl;
    }

    // reduce ghosts and include functor

    CountHits();

    LOG(debug1) << "Number of MVD Hits: " << fMVDHits << ", SttHits: " << fSTTHits << ", GEMHits: " << fGEMHits << endl;

    if (fMVDHits > 3 || (fMVDHits + fSTTHits + fGEMHits) > 5) {

      if (fWithGhostReduction) {
        if (!IsGhost(trackcirc)) {

          fMergedTracks.push_back(trackTotal);
          // fMergedTrackCands.push_back(fApolloniusTrackCand);

          LOG(debug1) << "number of merged tracks: " << fMergedTracks.size() << endl;
        }
      } else {
        fMergedTracks.push_back(trackTotal);
        // fMergedTrackCands.push_back(fApolloniusTrackCand);

        LOG(debug1) << "number of merged tracks: " << fMergedTracks.size() << endl;
      }
    }
  }
}

double PndHoughMerge::findMedian(std::vector<double> a)
{
  // First we sort the array
  std::sort(a.begin(), a.end());
  int n = a.size();
  // check for even case
  if (n % 2 != 0)
    return (double)a[n / 2];

  return (double)(a[(n - 1) / 2] + a[n / 2]) / 2.0;
}

/**
 * @brief      All tracklets are determined that have to be merged.
 *             The method is based on calculation the distance of maxima in the hough space.
 *             A cut value "fCutMergeByHoughSpace" can be set manually. If nothing is set a value of 5 cm is chosen, which was the result of a ROC analysis.
 *
 * @param[in]  PreselectedCircles  The calculated Circles of the preselected tracklets.
 * @param[in]  TrackCands          Vector of all PndTrackCands to assign an index to a track candidate.
 * @param[in]  fBz                 The z component of the magnetic field.
 */
void PndHoughMerge::mergeByHoughSpace(std::vector<TVector3> &PreselectedCircles, std::vector<PndTrackCand> &TrackCands, double fBz)
{
  std::vector<std::vector<int>> mergeTracks;
  std::map<int, int> mapSingleTrackToMergedTrack;
  std::vector<int> filledTracks;
  mergeTracks.clear();
  filledTracks.clear();
  std::vector<double> maxima_x;
  std::vector<double> maxima_y;
  fMaxima_x.clear();
  fMaxima_y.clear();
  fMaxima_r.clear();

  LOG(debug1) << "number of Preselected Tracks: " << PreselectedCircles.size() << endl;

  if (PreselectedCircles.size() > 0) {
    std::vector<int> SingleTracks;
    SingleTracks.push_back(0);
    mergeTracks.push_back(SingleTracks);
    mapSingleTrackToMergedTrack[0] = 0;
  }

  for (int i = 0; i < PreselectedCircles.size(); i++) {

    LOG(debug1) << "analyze preselected Track " << i << " of " << PreselectedCircles.size() << endl;

    TVector3 Circlei = PreselectedCircles[i];
    double xcirclecenterTrack1 = Circlei.X();
    double ycirclecenterTrack1 = Circlei.Y();
    double rcirclecenterTrack1 = Circlei.Z();

    if (i == 0) {
      std::vector<double> SingleX, SingleY, SingleR;
      SingleX.push_back(xcirclecenterTrack1);
      SingleY.push_back(ycirclecenterTrack1);
      SingleR.push_back(rcirclecenterTrack1);
      fMaxima_x.push_back(SingleX);
      fMaxima_y.push_back(SingleY);
      fMaxima_r.push_back(SingleR);
    }

    maxima_x.push_back(xcirclecenterTrack1);
    maxima_y.push_back(ycirclecenterTrack1);

    LOG(debug1) << "found track x: " << Circlei.X() << " y: " << Circlei.Y() << " r: " << Circlei.Z() << endl;

    for (int j = 0; j < maxima_x.size() - 1; j++) {

      LOG(debug1) << "xcirclecenterTrack1: " << xcirclecenterTrack1 << " ycirclecenterTrack1: " << ycirclecenterTrack1 << endl;
      LOG(debug1) << "maxima_x: " << maxima_x[j] << " maxima_y: " << maxima_y[j] << endl;

      double d = sqrt((xcirclecenterTrack1 - maxima_x[j]) * (xcirclecenterTrack1 - maxima_x[j]) + (ycirclecenterTrack1 - maxima_y[j]) * (ycirclecenterTrack1 - maxima_y[j]));

      LOG(debug1) << "d: " << d << endl;
      LOG(debug1) << "does track nr " << i << " fit to track nr " << j << " of the already found tracks?" << endl;

      if (d < fCutMergeByHoughSpace) {
        LOG(debug1) << "mapSingleTrackToMergedTrack[j]: " << mapSingleTrackToMergedTrack[j] << " i: " << i << endl;
        mergeTracks[mapSingleTrackToMergedTrack[j]].push_back(i);
        fMaxima_x[mapSingleTrackToMergedTrack[j]].push_back(xcirclecenterTrack1);
        fMaxima_y[mapSingleTrackToMergedTrack[j]].push_back(ycirclecenterTrack1);
        fMaxima_r[mapSingleTrackToMergedTrack[j]].push_back(rcirclecenterTrack1);

        break;
      } else if (j == maxima_x.size() - 2) {
        LOG(debug1) << "new Merged track nr: " << mergeTracks.size() << endl;
        std::vector<int> SingleTracks;
        SingleTracks.push_back(i);
        std::vector<double> SingleX, SingleY, SingleR;
        SingleX.push_back(xcirclecenterTrack1);
        SingleY.push_back(ycirclecenterTrack1);
        SingleR.push_back(rcirclecenterTrack1);
        mapSingleTrackToMergedTrack[i] = mergeTracks.size();
        mergeTracks.push_back(SingleTracks);
        fMaxima_x.push_back(SingleX);
        fMaxima_y.push_back(SingleY);
        fMaxima_r.push_back(SingleR);
      }
    }
  }

  LOG(debug1) << "Number of Merged tracks; " << mergeTracks.size() << endl;
  std::vector<PndTrack> temp;
  temp.clear();
  mergeSingle(mergeTracks, temp, TrackCands, fBz);
}

/**
 * @brief      Determines whether the specified track is a ghost.
 *             Check if track is a ghost based on ghost reduction parameters. All cut values were determined by a ROC analysis.
 *
 * @param[in]  trackcirc  The circle parameters of a track stored as [x,y,z].
 *
 * @return     True if the specified trackcirc is ghost, False otherwise.
 */
bool PndHoughMerge::IsGhost(std::vector<double> &trackcirc)
{
  CalculateGhostReductionParameters(trackcirc);

  if ((fSTTHits > 0 && fCounterHittedNeighborsRel / fSTTHits <= 3.65) || fSTTHits == 0) {
    // If a track has a GEM hit with more than 3 neighbored GEM hits, it is defined
    // as a "GEM blob" and therefore defined as a ghost track.
    // GEMNeighborCut = kTRUE means that this track has to be cutted since it is a ghost
    if (!(fGEMNeighborCut)) {
      // --> all tracks consiting of more than 6 GEM Hits and nothing else can be classified as ghost tracks.
      if (!((fMVDHits + fSTTHits == 0) && (fGEMHits > 6))) {
        // the mean distance between the hits of a tracklet needs to be smalle 4.9 cm (otherwise its a ghost)
        if (fmeanD <= 4.9) {
          return kFALSE;
        }
      }
    }
  }
  return kTRUE;
}

/**
 * @brief      Calculates the ghost reduction parameters.
 *
 * @param[in]  trackcirc  The circle parameters of a track stored as [x,y,z].
 */
void PndHoughMerge::CalculateGhostReductionParameters(std::vector<double> &trackcirc)
{

  /*
  Calculates ghost reduction parameters
  */

  std::vector<double> d;
  d.clear();
  fCounterHittedNeighborsRel = 0.;
  fGEMNeighborCut = kFALSE;
  fmeanD = 0;

  for( int n = 0; n < fApolloniusTrackCand.GetNHits(); n++){
    PndTrackCandHit hit_temp = fApolloniusTrackCand.GetSortedHit(n);
    TString branchName = ioman->GetBranchName(hit_temp.GetType());

    if(branchName=="STTHit"){
      fCounterHittedNeighborsRel += fData->GetNumOfSTTNeighbors((FairLink)hit_temp);
    }
            
    if(n<fApolloniusTrackCand.GetNHits()-1){
      TVector2 PCA1=fPndHoughUtilities->calcPointOnCircle(n, fApolloniusTrackCand, trackcirc);//PointOfClosestApproach
      TVector2 PCA2=fPndHoughUtilities->calcPointOnCircle(n+1, fApolloniusTrackCand, trackcirc);//PointOfClosestApproach
      double dHitToHit = sqrt((PCA1.X()-PCA2.X()) * (PCA1.X()-PCA2.X()) + (PCA1.Y()-PCA2.Y()) * (PCA1.Y()-PCA2.Y()));

      d.push_back(dHitToHit);
    }
        
    if(branchName == "GEMHit"){
      
      int counterGEMNeighbors = fData->GetNumOfGEMNeighbors((FairLink)hit_temp);
      if(counterGEMNeighbors > 3.5) fGEMNeighborCut = kTRUE;

    }
  }
  
  if(d.size()>0) fmeanD = std::accumulate(d.begin(), d.end(), 0.0) / d.size();

  LOG(debug1) << "meanD: " << fmeanD << ", GEMNeighborCut: " << fGEMNeighborCut << ", fCounterHittedNeighborsRel: " << fCounterHittedNeighborsRel
              << ", counterHittedNeighborsrel/STTHits: " << fCounterHittedNeighborsRel / fSTTHits << endl;
}

/**
 * @brief      Counts the number of hits of different detector parts for a specific track candidate.
 */
void PndHoughMerge::CountHits()
{
  fMVDHits = 0;
  fSTTHits = 0.;
  fGEMHits = 0;

  for( int n = 0; n < fApolloniusTrackCand.GetNHits(); n++){
    PndTrackCandHit hit_temp = fApolloniusTrackCand.GetSortedHit(n);
    TString branchName = ioman->GetBranchName(hit_temp.GetType());
    if(branchName == "MVDHitsPixel") fMVDHits+=1;
    if(branchName == "MVDHitsStrip") fMVDHits+=1;
    if(branchName == "STTHit") fSTTHits+=1.;
    if(branchName == "GEMHit") fGEMHits+=1;
  }
}
