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
//  Class for merging single tracklets
/////////////////////////////////////////////////////////////////

/** PndHoughMerge
 *@author Anna Alicke <a.alicke@fz-juelich.de>
 *@since 23.06.2019
 *@updated 23.11.2021
 *@version 3.1
 **
 ** PANDA class for merging single tracklets
 ** Task level RECO
 **/

#ifndef PndHoughMerge_H_
#define PndHoughMerge_H_

//#include "PndSttGeometryMap.h"
//#include "PndStt2GeoHandler.h"

#include "PndHoughUtilities.h"
#include "PndHoughTransformation.h"
#include "PndHoughTrackCorrection.h"

#include <vector>

class PndHoughMerge {
 public:
  PndHoughMerge(PndHoughData *data) : fCutMergeByHoughSpace(5.), fWithGhostReduction(kTRUE)

  {

    // fPndHoughUtilities = new PndHoughUtilities(data);
    // fPndHoughTransformation = new PndHoughTransformation(data);

    ioman = FairRootManager::Instance();
    fMapFairLinktoFairHit = data->GetMapFairLinktoFairHit();
    fMergedTracks.clear();
    // fMergedTrackCands.clear();
    fData = data;
  };

  virtual ~PndHoughMerge() { fMergedTracks.clear(); };
  /** @brief Sets the value of the distance in the Hough space within two tracklets have to be merged. */
  virtual void SetCutMergeByHoughSpace(float cut) { fCutMergeByHoughSpace = cut; };
  /** @brief Sets a bool for deciding to use a ghost reduction. */
  virtual void SetWithGhostReduction(bool ghostred) { fWithGhostReduction = ghostred; }

  virtual void SetUtilities(PndHoughUtilities *utilities) { fPndHoughUtilities = utilities; }
  // virtual void SetHoughTransformation(PndHoughTransformation *trafo) { fPndHoughTransformation = trafo; };
  virtual void SetTrackCorrection(PndHoughTrackCorrection *correction) { fPndHoughTrackCorrection = correction; };
  // virtual void SetHoughTransformation(PndHoughTransformation *trafo) { fPndHoughTransformation = trafo; };

  /** @brief All tracklets are determined that have to be merged. */
  void mergeByHoughSpace(std::vector<TVector3> &PreselectedCircles, std::vector<PndTrackCand> &TrackCands, double fBz);
  /** @brief Here all tracklets are merged. */
  void mergeSingle(std::vector<std::vector<int>> &mergeTracks, std::vector<PndTrack> &Tracks, std::vector<PndTrackCand> &TrackCands, double fBz);
  double findMedian(std::vector<double> a);
  /** @brief Determines whether the specified track is a ghost. */
  bool IsGhost(std::vector<double> &trackcirc);
  /** @brief Calculates the ghost reduction parameters. */
  void CalculateGhostReductionParameters(std::vector<double> &trackcirc);
  /** @brief Counts the number of hits of different detector parts for a specific track candidate. */
  void CountHits();
  /** @brief Returns a specific merged track. */
  PndTrack GetMergedTrack(int i) { return fMergedTracks[i]; };
  /** @brief Returns a specific merged track candidate. */
  // PndTrackCand GetMergedTrackCand(int i) { return fMergedTrackCands[i]; };
  /** @brief Returns the number of merged track. */
  int GetNumMergedTracks() { return fMergedTracks.size(); };
  /** @brief Returns the number of merged track candidates. */
  // int GetNumMergedTrackCands() { return fMergedTrackCands.size(); };
  void UpdateMergedTrack(int i, PndTrack &track) { fMergedTracks[i] = track; };
  /** @brief Returns a vector of all merged track. */
  std::vector<PndTrack> GetMergedTracks() { return fMergedTracks; };
  /** @brief Sets a vector of all merged track. */
  void SetMergedTracks(std::vector<PndTrack> vec)
  {
    if (vec.size() < fMergedTracks.size()) {
      fMergedTracks.erase(fMergedTracks.begin() + vec.size(), fMergedTracks.end());
    }
    for (int i = 0; i < vec.size(); i++) {
      vec[i].Print();
      fMergedTracks[i] = (PndTrack)vec[i];
    }
  };

 private:
  FairRootManager *ioman = nullptr;

  PndHoughUtilities *fPndHoughUtilities = nullptr;
  // PndHoughTransformation *fPndHoughTransformation = nullptr;
  PndHoughTrackCorrection *fPndHoughTrackCorrection = nullptr;
  PndHoughData *fData = nullptr;

  std::map<FairLink, FairHit *> fMapFairLinktoFairHit;
  std::vector<PndTrack> fMergedTracks;
  // std::vector<PndTrackCand> fMergedTrackCands;

  std::vector<std::vector<double>> fMaxima_x;
  std::vector<std::vector<double>> fMaxima_y;
  std::vector<std::vector<double>> fMaxima_r;

  PndTrackCand fApolloniusTrackCand;

  float fCutMergeByHoughSpace;

  Int_t fMVDHits;
  Int_t fGEMHits;
  double fSTTHits;
  double fCounterHittedNeighborsRel;
  double fmeanD;

  bool fGEMNeighborCut;
  bool fSTTNeighborCut;
  bool fWithGhostReduction;

  ClassDef(PndHoughMerge, 1);
};

#endif /*PndHoughMerge_H_*/
