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
//  PndHoughTrackFinder
//  Finds Track for one event
/////////////////////////////////////////////////////////////////

/** PndHoughTrackFinder
 *@author Anna Alicke <a.alicke@fz-juelich.de>
 *@since 29.10.2018
 *@updated 23.11.2021
 *@version 3.1
 **
 ** PANDA class for finding tracks based on a hough transformation
 ** Task level RECO
 **/

#ifndef PndHoughTrackFinder_H_
#define PndHoughTrackFinder_H_

#include <vector>

#include "PndSttCA.h"

#include "PndHoughData.h"
#include "PndHoughUtilities.h"
#include "PndHoughTransformation.h"
#include "PndHoughSegment.h"
#include "PndHoughTrackCorrection.h"
#include "PndHoughPreprocessing.h"
#include "PndHoughMerge.h"
#include "PndApolloniusTriplet.h"

class PndStt2GeoHandler;

class PndHoughTrackFinder {
 public:
  PndHoughTrackFinder(){};

  PndHoughTrackFinder(PndStt2GeoHandler *geoH) : fNbins1(450), fNbins2(450), fNBinsSeg(90.), fBz(2.), fCutMergeByHoughSpace(5.), fWithGPU(kFALSE), fWithGhostReduction(kTRUE)
  //PndHoughTrackFinder(TClonesArray *tubeArray) : fNbins1(450), fNbins2(450), fNBinsSeg(90.), fBz(2.), fCutMergeByHoughSpace(5.), fWithGPU(kFALSE), fWithGhostReduction(kTRUE)
  {
    // Generate TrackFinderData-Object
    //fData = new PndHoughData(tubeArray);
    //fCATrackFinder = new PndSttCA(tubeArray);
    fData = new PndHoughData(geoH);
    fCATrackFinder = new PndSttCA(geoH);
    ioman = FairRootManager::Instance();
  };

  /** Destructor **/
  virtual ~PndHoughTrackFinder()
  {
    delete fData;
    delete fCATrackFinder;

    delete fPndHoughTransformation;
    delete fPndHoughPreprocessing;
    delete fPndHoughMerge;
    delete fPndHoughSegment;
    delete fPndHoughUtilities;
    delete fPndHoughTrackCorrection;
  }

  /** @brief Sets the number of bins in x direction for the Hough space. */
  virtual void SetBinningX(double bin) { fNbins1 = bin; };
  /** @brief Sets the number of bins in y direction for the Hough space. */
  virtual void SetBinningY(double bin) { fNbins2 = bin; };
  /** @brief Sets the number of bins used for the segmentation preselection algorithm. */
  virtual void SetNBinsSeg(Int_t n) { fNBinsSeg = n; };
  /** @brief Sets the z component of the magnetic field. */
  virtual void SetBz(double B) { fBz = B; };
  /** @brief Sets the cut value for the distance of two maxima in the Hough space which have to be merged. */
  virtual void SetCutMergeByHoughSpace(float cut){ fCutMergeByHoughSpace = cut; }
  /** @brief Sets a bool for deciding to use cuda. */
  virtual void SetWithGPU(bool cuda) { fWithGPU = cuda; }
  /** @brief Sets a bool for deciding to use a ghost reduction. */
  virtual void SetWithGhostReduction(bool ghostred) { fWithGhostReduction = ghostred; }
  virtual void SetPreselectedTrackCands(std::vector<PndTrackCand> cands) { fPreselectedTrackCands = cands; };
  virtual void UseDefaultPreselection(bool pres) { fUseDefaultPreselection = pres; };
  virtual void SetCombinedSkewed(TClonesArray *skewed) { fCombinedSkewed = skewed; };
  /** @brief Initializes the data structure and the Hough space of the HoughtrackFinder. */
  void Init()
  {
    fData->Init(fNbins1, fNbins2);
    fCATrackFinder->SetUseGPU(fWithGPU);
  }

  /** @brief Resets the data for a new event. */
  void Reset()
  {
    fData->clear();

    fCATrackFinder->SetUseGPU(fWithGPU);
    fCATrackFinder->Reset();

    fPreselectedTrackCands.clear();
    fPreselectedTrackCircles.clear();
  }

  /** @brief Adds hits to the data structure of the HoughTrackFinder. */
  void AddHits(TClonesArray *hits, TString branchName);

  /** @brief Creates the neighborhood parameters for all investigated hits */
  void CreateNeighborhood()
  {
    fData->CreateSTTNeighborhoodData();
    fData->CreateGEMNeighborhoodData();
  }
  /** @brief Main function of the HoughTrackFinder, which finds the tracks. */
  void FindTracks();

  /** @brief Here all data are preselected in smaller tracklets. */
  void Preselection();

  /** @brief Performs a Hough transformation for all preselected tracklets. The found track parameters are stored in fPreselectedTrackCircles. */
  void FindHoughMaxima();
  TVector2 calcPhiRange(PndTrackCand &trackCand);
  bool IsInPhiRange(FairHit *hit, double phiLow, double phiHigh);
  void AddSkewedHits(int i);
  void AddOtherDetectorHits(int i, TString type);
  PndApollonius::TripletSolution FromPndTrackToTripletSolution(PndTrack &track, PndHoughData *fData);
  std::vector<PndApollonius::TripletSolution> CombineIdenticalSolutionsFinal(std::vector<PndApollonius::TripletSolution> &solutions, Double_t ratioOfSameHits);
  /**
   * @brief Converts a TripletSolution to a PndTrack
   *
   * @param[in]  sol    The TripletSolution
   * @param[out] result The PndTrack
   */
  PndTrack FromTripletSolutionToPndTrack(PndApollonius::TripletSolution &sol, Double_t B);

  double CalcRatioSameHits(PndApollonius::TripletSolution &sol, std::vector<PndApollonius::TripletSolution> &AlreadyFoundSolutions);

  /** @brief Returns a specific preselected track candidate. */
  PndTrackCand GetPreselectedTrackCand(int i) { return fPreselectedTrackCands[i]; };

  /** @brief Returns the number of preselected track candidates. */
  int GetNumPreselectedTrackCands() { return fPreselectedTrackCands.size(); };

  /** @brief Returns a the corresponding circle for a specific preselected track candidate. */
  TVector3 GetPreselectedTrackCircles(int i) { return fPreselectedTrackCircles[i]; };

  /** @brief Returns the number of preselected circles for track candidates. */
  int GetNumPreselectedTrackCircles() { return fPreselectedTrackCircles.size(); };

  /** @brief Returns a specific found track. */
  PndTrack GetMergedTrack(int i) { return fPndHoughMerge->GetMergedTrack(i); };

  /** @brief Returns the number of found tracks. */
  int GetNumMergedTracks() { return fPndHoughMerge->GetNumMergedTracks(); };

  /** @brief Returns a specific found track candidate. */
  // PndTrackCand GetMergedTrackCand(int i) { return fPndHoughMerge->GetMergedTrackCand(i); };

  /** @brief Returns the number of found track candidates. */
  // int GetNumMergedTrackCands() { return fPndHoughMerge->GetNumMergedTrackCands(); };

  /** @brief Returns a map linking FairLinks to the corresponding FairHits. */
  std::map<FairLink, FairHit *> GetMapFairLinktoFairHit() { return fData->GetMapFairLinktoFairHit(); };

  /** @brief Returns the data class of the HoughTrackFinder. */
  PndHoughData *GetData() { return fData; };

 private:
  FairRootManager *ioman = nullptr;

  PndSttCA *fCATrackFinder = nullptr;

  PndHoughData *fData = nullptr;
  PndHoughTransformation *fPndHoughTransformation = nullptr;
  PndHoughPreprocessing *fPndHoughPreprocessing = nullptr;
  PndHoughMerge *fPndHoughMerge = nullptr;
  PndHoughSegment *fPndHoughSegment = nullptr;
  PndHoughUtilities *fPndHoughUtilities = nullptr;
  PndHoughTrackCorrection *fPndHoughTrackCorrection = nullptr;
  TClonesArray *fCombinedSkewed = nullptr;
  std::vector<std::vector<TVector3>> fHoughSpacesToTracks;
  std::vector<PndTrackCand> fHoughSpacesToTrackCands;
  std::vector<PndTrackCand> fHoughSpacesToTrackCandTots;

  std::vector<FairLink> fFoundHits;

  std::vector<PndTrackCand> fPreselectedTrackCands;
  std::vector<TVector3> fPreselectedTrackCircles;
  std::vector<PndTrack> fApolloniusMergedTracks;
  std::vector<PndTrackCand> fApolloniusMergedTrackCands;

  Double_t fBz;
  float fCutMergeByHoughSpace;
  Int_t fNbins1;
  Int_t fNbins2;
  Int_t fNBinsSeg;
  bool fWithGPU;
  bool fWithGhostReduction;
  bool fUseDefaultPreselection = kTRUE;
  double fDistanceThresholdSTTCombinedSkewed = 1.;
  double fDistanceThresholdSTT = 0.3;
  double fDistanceThresholdMVD = 0.5;
  double fDistanceThresholdGEM = 1.;
  std::map<FairLink, FairHit *> fMapFairLinktoFairHit;
  std::map<FairLink, double> fMapFairLinktoIsochrone;
  ClassDef(PndHoughTrackFinder, 1);
};

#endif /*PndHoughTrackFinder_H_*/
