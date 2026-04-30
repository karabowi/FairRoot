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

#include "TMath.h"
#include "PndTrackCand.h"
#include "PndHoughTrackFinder.h"
// general
using std::cout;
using std::endl;
#include "FairLogger.h"

//#include "PndHoughTransformation.h"
//#include "PndHoughMultipletCreator.h"
//#include "PndHoughPreprocessing.h"

#include "TVector3.h"
#include "PndCircleTools.h"
/**
 * @brief      Adds hits to the data structure of the HoughTrackFinder.
 *
 * @param[in]  hits        An array of hits.
 * @param[in]  branchName  The branch name (detector part) of the hits.
 */
void PndHoughTrackFinder::AddHits(TClonesArray *hits, TString branchName)
{
  fData->AddHits(hits, branchName);

  if (branchName == "STTHit")
    fCATrackFinder->AddHits(hits, branchName);
}

/**
 * @brief      Main function of the HoughTrackFinder, which finds the tracks.
 */
void PndHoughTrackFinder::FindTracks()
{
  fPndHoughUtilities = new PndHoughUtilities(fData);
  fPndHoughPreprocessing = new PndHoughPreprocessing(fData);
  fPndHoughSegment = new PndHoughSegment(fData);

  fPndHoughTrackCorrection = new PndHoughTrackCorrection(fData);
  fPndHoughTrackCorrection->SetUtilities(fPndHoughUtilities);

  fPndHoughTransformation = new PndHoughTransformation(fData);
  fPndHoughTransformation->SetUtilities(fPndHoughUtilities);

  fPndHoughMerge = new PndHoughMerge(fData);
  fPndHoughMerge->SetCutMergeByHoughSpace(fCutMergeByHoughSpace);
  fPndHoughMerge->SetWithGhostReduction(fWithGhostReduction);
  fPndHoughMerge->SetUtilities(fPndHoughUtilities);
  // fPndHoughMerge->SetHoughTransformation(fPndHoughTransformation);
  fPndHoughMerge->SetTrackCorrection(fPndHoughTrackCorrection);

  fMapFairLinktoFairHit = fData->GetMapFairLinktoFairHit();
  fMapFairLinktoIsochrone = fData->GetMapFairLinktoIsochrone();

  fFoundHits.clear();

  if (fUseDefaultPreselection)
    Preselection();

  FindHoughMaxima();

  fPndHoughMerge->mergeByHoughSpace(fPreselectedTrackCircles, fPreselectedTrackCands, fBz);

  for (int i = 0; i < fPndHoughMerge->GetNumMergedTracks(); i++) {
    AddOtherDetectorHits(i, "MVD");
    AddOtherDetectorHits(i, "STT");
    AddOtherDetectorHits(i, "GEM");
  }
  if (fCombinedSkewed != nullptr) {
    for (int i = 0; i < fPndHoughMerge->GetNumMergedTracks(); i++) {
      AddSkewedHits(i);
    }
  }

  std::vector<PndApollonius::TripletSolution> solutions;
  for (int i = 0; i < fPndHoughMerge->GetNumMergedTracks(); i++) {
    PndTrack track = fPndHoughMerge->GetMergedTrack(i);
    PndApollonius::TripletSolution sol = FromPndTrackToTripletSolution(track, fData);

    solutions.push_back(sol);
  }

  Double_t ratioOfSameHits = 0.9;
  std::vector<PndApollonius::TripletSolution> FinalSol = CombineIdenticalSolutionsFinal(solutions, ratioOfSameHits);

  std::vector<PndTrack> vec;
  vec.clear();
  for (int i = 0; i < FinalSol.size(); i++) {
    PndTrack track = FromTripletSolutionToPndTrack(FinalSol[i], fBz);
    vec.push_back(track);
  }
  // std::cout << "vec: " << vec.size() << std::endl;
  fPndHoughMerge->SetMergedTracks(vec);
}

/**
 * @brief      Here all data are preselected in smaller tracklets.
 *             The tracklets are created by the Cellular Automation (for STT hits) and by a Segmentation algorithm for all remaining hits.
 */
void PndHoughTrackFinder::Preselection()
{
  int minNumberOfHits;
  minNumberOfHits = 2;

  PndTrackCand RemainingHitsTemp;
  std::vector<FairLink> AllLinks = fData->GetLinks();

  // Cellular Automaton (CA)
  fCATrackFinder->FindTracks();

  LOG(debug1) << "CA has " << fCATrackFinder->NumFirstTrackCands() << " Tracks" << endl;

  for (int i = 0; i < fCATrackFinder->NumFirstTrackCands(); i++) {
    PndTrackCand trackCand_temp = fCATrackFinder->GetFirstTrackCand(i);

    if (trackCand_temp.GetNHits() <= minNumberOfHits)
      continue;
    for (int j = 0; j < trackCand_temp.GetNHits(); j++)
      fFoundHits.push_back(trackCand_temp.GetSortedHit(j));
    PndTrackCand trackCandCleanedWithoutSkewed = fPndHoughPreprocessing->CleanAndRejectSkewed(trackCand_temp);
    if (trackCandCleanedWithoutSkewed.GetNHits() <= minNumberOfHits)
      continue;
    fPreselectedTrackCands.push_back(trackCandCleanedWithoutSkewed);
  }
  LOG(debug1) << "fPreselectedTrackCands after CA " << fPreselectedTrackCands.size() << endl;

  // determine all hits not found by the CA
  double phi, r, rho;
  FairHit *myHit;
  FairLink link;
  for (int i = 0; i < AllLinks.size(); i++) {
    link = AllLinks[i];
    myHit = fMapFairLinktoFairHit[link];
    TString branchName = ioman->GetBranchName(link.GetType());
    if (branchName == "STTHit") {
      link = myHit->GetEntryNr();
      if (std::find(fFoundHits.begin(), fFoundHits.end(), link) == fFoundHits.end()) {
        phi = TMath::ATan2(myHit->GetY(), myHit->GetX()) * 180 / TMath::Pi();
        r = sqrt((myHit->GetX() * myHit->GetX()) + (myHit->GetY() * myHit->GetY()));
        rho = r * phi * TMath::Pi() / 180;
        RemainingHitsTemp.AddHit(link, rho);
      }
    } else {
      phi = TMath::ATan2(myHit->GetY(), myHit->GetX()) * 180 / TMath::Pi();
      r = sqrt((myHit->GetX() * myHit->GetX()) + (myHit->GetY() * myHit->GetY()));
      rho = r * phi * TMath::Pi() / 180;
      RemainingHitsTemp.AddHit(link, rho);
    }
  }

  // segmentation algorithm for all hit not found by CA
  TClonesArray *RemainingSegmentedCands;
  fPndHoughSegment->SetNBins(90);
  RemainingSegmentedCands = fPndHoughSegment->CreateTrackCands(RemainingHitsTemp);
  LOG(debug1) << "RemainingSegmentedCands->GetEntriesFast(): " << RemainingSegmentedCands->GetEntriesFast() << endl;
  for (int i = 0; i < RemainingSegmentedCands->GetEntriesFast(); i++) {
    PndTrackCand *trackCandRemaining = (PndTrackCand *)RemainingSegmentedCands->At(i);
    if (trackCandRemaining->GetNHits() <= minNumberOfHits)
      continue;
    PndTrackCand RemainingHitsCleanedWithoutSkewed = fPndHoughPreprocessing->CleanAndRejectSkewed(*trackCandRemaining);
    if (RemainingHitsCleanedWithoutSkewed.GetNHits() <= minNumberOfHits)
      continue;
    fPreselectedTrackCands.push_back(RemainingHitsCleanedWithoutSkewed);
  }
  RemainingSegmentedCands->Delete();
  LOG(debug1) << "fPreselectedTrackCands after all " << fPreselectedTrackCands.size() << endl;
}

/**
 * @brief        Performs a Hough transformation for all preselected tracklets. The found track parameters are stored in fPreselectedTrackCircles.
 */
void PndHoughTrackFinder::FindHoughMaxima()
{
  // fPndHoughTransformation = new PndHoughTransformation(fData);

  for (int i = 0; i < fPreselectedTrackCands.size(); i++) {
    PndTrackCand trackCand = fPreselectedTrackCands[i];
    LOG(debug1) << "trackCand: " << trackCand.GetNHits() << endl;

    vector<TVector3> TrackCircleVec = fPndHoughTransformation->FindMaximaWithHoughTransformation(trackCand);

    TVector3 Circle;
    if (TrackCircleVec.empty())
      continue;
    if (TrackCircleVec.size() > 1)
      Circle = fPndHoughTransformation->MaximumSelectiondCandToTrack(trackCand, TrackCircleVec);
    else
      Circle = TrackCircleVec[0];
    LOG(debug1) << "circle parameter for track ending with hit (" << fMapFairLinktoFairHit[trackCand.GetSortedHit(trackCand.GetNHits() - 1)]->GetX() << ","
                << fMapFairLinktoFairHit[trackCand.GetSortedHit(trackCand.GetNHits() - 1)]->GetY() << "): (" << Circle.X() << "," << Circle.Y() << "," << Circle.Z() << ")" << endl;

    fPreselectedTrackCircles.push_back(Circle);
  }
}

/**
 * @brief        Determines a phi range for each trackCand, in which possible further hits can be.
 */
TVector2 PndHoughTrackFinder::calcPhiRange(PndTrackCand &trackCand)
{
  std::vector<int> sectors;
  sectors.clear();
  for (int j = 0; j < trackCand.GetNHits(); j++) {
    if (ioman->GetBranchName(trackCand.GetSortedHit(j).GetType()) != "STTHit")
      continue;
    PndSttHit *sttHit = (PndSttHit *)(fData->GetMapFairLinktoFairHit()[(FairLink)trackCand.GetSortedHit(j)]);
    int sectorId = fData->GetSttGeoH()->GetSector(sttHit->GetTubeID());
    //int sectorId = fData->GetStrawMap()->GetSector(sttHit->GetTubeID());
    if (std::find(sectors.begin(), sectors.end(), sectorId) == sectors.end()) {
      sectors.push_back(sectorId);
    }
  }

  double phiHigh = 0;
  double phiLow = 0;
  for (int j = 0; j < sectors.size(); j++) {
    vector<int> FirstSectorRow = fData->GetSttGeoH()->GetStrawRow(sectors[j], 0);
    //vector<int> FirstSectorRow = fData->GetStrawMap()->GetStrawRow(sectors[j], 0);
    double phiLowTemp = TMath::ATan2(fData->GetSttGeoH()->GetTube(FirstSectorRow[0])->GetPosition().Y(), fData->GetSttGeoH()->GetTube(FirstSectorRow[0])->GetPosition().X());
    //double phiLowTemp = TMath::ATan2(fData->GetStrawMap()->GetTube(FirstSectorRow[0])->GetPosition().Y(), fData->GetStrawMap()->GetTube(FirstSectorRow[0])->GetPosition().X());
    double phiHighTemp = TMath::ATan2(fData->GetSttGeoH()->GetTube(FirstSectorRow[FirstSectorRow.size() - 1])->GetPosition().Y(),
                                      fData->GetSttGeoH()->GetTube(FirstSectorRow[FirstSectorRow.size() - 1])->GetPosition().X());
    //double phiHighTemp = TMath::ATan2(fData->GetStrawMap()->GetTube(FirstSectorRow[FirstSectorRow.size() - 1])->GetPosition().Y(),
                                      //fData->GetStrawMap()->GetTube(FirstSectorRow[FirstSectorRow.size() - 1])->GetPosition().X());

    if (phiLowTemp < 0)
      phiLowTemp = 2 * TMath::Pi() + phiLowTemp;
    if (phiHighTemp < 0)
      phiHighTemp = 2 * TMath::Pi() + phiHighTemp;

    if (phiHigh == 0 || phiHigh < phiHighTemp)
      phiHigh = phiHighTemp;
    if (phiLow == 0 || phiLow > phiLowTemp)
      phiLow = phiLowTemp;
  }
  return TVector2(phiLow, phiHigh);
}

/**
 * @brief        Is a hit with a possible phi range of a track? Here a phi +-30 deg is assumed
 */
bool PndHoughTrackFinder::IsInPhiRange(FairHit *hit, double phiLow, double phiHigh)
{

  double phi = TMath::ATan2(hit->GetY(), hit->GetX());
  if (phi < 0)
    phi = 2 * TMath::Pi() + phi;

  if (phiHigh < 60 * TMath::Pi() / 180. && phiLow > 300 * TMath::Pi() / 180.) {
    if (phi < (phiHigh + (30 * TMath::Pi() / 180.)) || phi > (phiLow - (30 * TMath::Pi() / 180.))) {
      return kTRUE;
    }
  } else {
    if (phi < (phiHigh + (30 * TMath::Pi() / 180.)) && phi > (phiLow - (30 * TMath::Pi() / 180.))) {
      return kTRUE;
    }
  }
  return kFALSE;
}

void PndHoughTrackFinder::AddSkewedHits(int i)
{
  PndTrack track = fPndHoughMerge->GetMergedTrack(i);
  PndTrackCand trackCand = track.GetTrackCand();
  std::vector<double> circle = fPndHoughUtilities->getCircleFromPndTrack(track);
  FairLink FirstLink = trackCand.GetSortedHit(0);
  TVector3 Hitmin(fData->GetMapFairLinktoFairHit()[FirstLink]->GetX(), fData->GetMapFairLinktoFairHit()[FirstLink]->GetY(), fData->GetMapFairLinktoFairHit()[FirstLink]->GetZ());
  TVector3 Track(circle[0], circle[1], circle[2]);
  double dMin = sqrt(Hitmin.X() * Hitmin.X() + Hitmin.Y() * Hitmin.Y());
  TVector2 PhiRange = calcPhiRange(trackCand);
  double phiLow = PhiRange.X();
  double phiHigh = PhiRange.Y();
  bool hasChanged = kFALSE;
  bool hasMinHitChanged = kFALSE;
  LOG(debug1) << "AddSkewedHits track " << i << std::endl;

  // FIXME: This protection will silently omit the fact of a missing array, output would be too verbose, though.
  if (fCombinedSkewed != nullptr) {
    for (int j = 0; j < fCombinedSkewed->GetEntriesFast(); j++) {
      PndSttSkewedHit *SkewedSttHit = (PndSttSkewedHit *)fCombinedSkewed->At(j);
      double d =
        sqrt((SkewedSttHit->GetX() - circle[0]) * (SkewedSttHit->GetX() - circle[0]) + (SkewedSttHit->GetY() - circle[1]) * (SkewedSttHit->GetY() - circle[1])) - abs(circle[2]);
      std::pair<Int_t, Int_t> tubeIDs = SkewedSttHit->GetTubeIDs();
      if (abs(d) < fDistanceThresholdSTTCombinedSkewed) {

        PndSttHit *sttHit1 = (PndSttHit *)(fData->GetMapTubetoHit()[tubeIDs.first]);
        PndSttHit *sttHit2 = (PndSttHit *)(fData->GetMapTubetoHit()[tubeIDs.second]);

        FairLink link1 = fData->GetMapTubetoLink()[tubeIDs.first];
        FairLink link2 = fData->GetMapTubetoLink()[tubeIDs.second];

        // check if hits are already in the track
        std::vector<PndTrackCandHit> vec = trackCand.GetSortedHits();
        bool IsIn1 = kFALSE;
        bool IsIn2 = kFALSE;
        for (auto iter = vec.begin(); iter != vec.end(); iter++) {
          if ((FairLink)(*iter) == link1)
            IsIn1 = kTRUE;
        }
        for (auto iter = vec.begin(); iter != vec.end(); iter++) {
          if ((FairLink)(*iter) == link2)
            IsIn2 = kTRUE;
        }

        // check if hits are in the correct phi range
        bool SameSectorRange1 = IsInPhiRange((FairHit *)sttHit1, phiLow, phiHigh);
        bool SameSectorRange2 = IsInPhiRange((FairHit *)sttHit2, phiLow, phiHigh);

        if (SameSectorRange1 && !IsIn1) {
          // check if the hit is closer to the IP as the first hit --> Change order of the hits
          double d = sqrt(SkewedSttHit->GetX() * SkewedSttHit->GetX() + SkewedSttHit->GetY() * SkewedSttHit->GetY());
          TVector3 Hit1(sttHit1->GetX(), sttHit1->GetY(), sttHit1->GetZ());
          LOG(debug1) << "skewed hit (" << Hit1.X() << "," << Hit1.Y() << ") was added" << std::endl;
          if (d < dMin) {
            LOG(debug1) << "Order must be changed new Min Hit: (" << Hit1.X() << "," << Hit1.Y() << ")" << std::endl;
            Hitmin = Hit1;
            dMin = d;
            hasMinHitChanged = kTRUE;
          }
          hasChanged = kTRUE;

          double rho_temp1 = fPndHoughTrackCorrection->calc_rho(Hit1, Track, Hitmin);
          trackCand.AddHit(link1, rho_temp1);
        }
        if (SameSectorRange2 && !IsIn2) {
          double d = sqrt(SkewedSttHit->GetX() * SkewedSttHit->GetX() + SkewedSttHit->GetY() * SkewedSttHit->GetY());
          TVector3 Hit2(sttHit2->GetX(), sttHit2->GetY(), sttHit2->GetZ());
          LOG(debug1) << "skewed hit (" << Hit2.X() << "," << Hit2.Y() << ") was added" << std::endl;

          if (d < dMin) {
            LOG(debug1) << "Order must be changed new Min Hit: (" << Hit2.X() << "," << Hit2.Y() << ")" << std::endl;
            Hitmin = Hit2;
            dMin = d;
            hasMinHitChanged = kTRUE;
          }
          hasChanged = kTRUE;
          double rho_temp2 = fPndHoughTrackCorrection->calc_rho(Hit2, Track, Hitmin);
          trackCand.AddHit(link2, rho_temp2);
        }
      }
    }
    if (hasChanged && !hasMinHitChanged) {
      track.SetTrackCand(trackCand);
      for (int j = 0; j < trackCand.GetNHits(); j++) {
        LOG(debug1) << "AddSkewedHits: track " << i << " (" << ((FairHit *)ioman->GetCloneOfLinkData(trackCand.GetSortedHit(j)))->GetX() << ","
                    << ((FairHit *)ioman->GetCloneOfLinkData(trackCand.GetSortedHit(j)))->GetY() << ")" << std::endl;
      }
    }
    if (hasChanged && hasMinHitChanged) {
      trackCand = fPndHoughTrackCorrection->changeHitOrder(trackCand, Track, Hitmin);

      track = fPndHoughUtilities->getPndTrack(fBz, trackCand, Track);
      for (int j = 0; j < trackCand.GetNHits(); j++) {
        LOG(debug1) << "AddSkewedHits: track " << i << " (" << ((FairHit *)ioman->GetCloneOfLinkData(trackCand.GetSortedHit(j)))->GetX() << ","
                    << ((FairHit *)ioman->GetCloneOfLinkData(trackCand.GetSortedHit(j)))->GetY() << ")" << std::endl;
      }
    }
  }
  // track.SetTrackCand(trackCand);
  fPndHoughMerge->UpdateMergedTrack(i, track);
}

void PndHoughTrackFinder::AddOtherDetectorHits(int i, TString type)
{
  PndTrack track = fPndHoughMerge->GetMergedTrack(i);
  PndTrackCand trackCand = track.GetTrackCand();
  std::vector<double> circle = fPndHoughUtilities->getCircleFromPndTrack(track);
  FairLink FirstLink = trackCand.GetSortedHit(0);
  TVector3 Hitmin(fData->GetMapFairLinktoFairHit()[FirstLink]->GetX(), fData->GetMapFairLinktoFairHit()[FirstLink]->GetY(), fData->GetMapFairLinktoFairHit()[FirstLink]->GetZ());
  TVector3 Track(circle[0], circle[1], circle[2]);
  double dMin = sqrt(Hitmin.X() * Hitmin.X() + Hitmin.Y() * Hitmin.Y());
  bool hasChanged = kFALSE;
  bool hasMinHitChanged = kFALSE;
  TVector2 PhiRange = calcPhiRange(trackCand);
  double phiLow = PhiRange.X();
  double phiHigh = PhiRange.Y();
  std::vector<FairLink> data;
  double distanceThreshold;
  if (type == "MVD") {
    data = fData->GetMVDHits();
    distanceThreshold = fDistanceThresholdMVD;
  } else if (type == "GEM") {
    data = fData->GetGEMHits();
    distanceThreshold = fDistanceThresholdGEM;
  } else if (type == "STT") {
    data = fData->GetSTTHits();
    distanceThreshold = fDistanceThresholdSTT;
  }

  for (int j = 0; j < data.size(); j++) {
    FairLink link = data[j];
    FairHit *Hit = (FairHit *)fData->GetMapFairLinktoFairHit()[link];

    bool SameSectorRange = IsInPhiRange(Hit, phiLow, phiHigh);

    double d;
    if (type == "STT") {
      d = sqrt((Hit->GetX() - circle[0]) * (Hit->GetX() - circle[0]) + (Hit->GetY() - circle[1]) * (Hit->GetY() - circle[1])) - abs(circle[2]) -
          abs(fData->GetMapFairLinktoIsochrone()[link]);
    } else {
      d = sqrt((Hit->GetX() - circle[0]) * (Hit->GetX() - circle[0]) + (Hit->GetY() - circle[1]) * (Hit->GetY() - circle[1])) - abs(circle[2]);
    }

    if (abs(d) < distanceThreshold && SameSectorRange) {

      std::vector<PndTrackCandHit> vec = trackCand.GetSortedHits();
      bool IsIn1 = kFALSE;
      for (auto iter = vec.begin(); iter != vec.end(); iter++) {
        if ((FairLink)(*iter) == link)
          IsIn1 = kTRUE;
      }

      if (!IsIn1) {
        TVector3 Hit1(Hit->GetX(), Hit->GetY(), Hit->GetZ());
        if (d < dMin) {
          Hitmin = Hit1;
          dMin = d;
          hasMinHitChanged = kTRUE;
        }
        hasChanged = kTRUE;
        double rho_temp1 = fPndHoughTrackCorrection->calc_rho(Hit1, Track, Hitmin);
        trackCand.AddHit(link, rho_temp1);
      }
    }
  }
  if (hasChanged && !hasMinHitChanged) {
    track.SetTrackCand(trackCand);
  }
  if (hasChanged && hasMinHitChanged) {
    trackCand = fPndHoughTrackCorrection->changeHitOrder(trackCand, Track, Hitmin);
    track = fPndHoughUtilities->getPndTrack(fBz, trackCand, Track);
  }
  fPndHoughMerge->UpdateMergedTrack(i, track);
}

PndApollonius::TripletSolution PndHoughTrackFinder::FromPndTrackToTripletSolution(PndTrack &track, PndHoughData *fData)
{

  PndHoughUtilities *fPndHoughUtilities = new PndHoughUtilities();

  std::vector<double> circle = fPndHoughUtilities->getCircleFromPndTrack(track);
  PndTrackCand cand = track.GetTrackCand();
  PndApollonius::TripletSolution sol = PndApollonius::TripletSolution(TVector3(circle[0], circle[1], circle[2]));

  for (int i = 0; i < cand.GetNHits(); i++) {
    FairLink link = (FairLink)cand.GetSortedHit(i);
    TString branchName = FairRootManager::Instance()->GetBranchName(link.GetType());

    if (branchName.Contains("MVDHitsPixel")) {
      FairHit *hit = (FairHit *)(fData->GetMapFairLinktoFairHit()[link]);
      hit->SetEntryNr(link);
      sol.AddHit(PndApollonius::TripletSolution::detID::MVDpixel, hit);
    }
    if (branchName.Contains("MVDHitsStrip")) {
      FairHit *hit = (FairHit *)(fData->GetMapFairLinktoFairHit()[link]);
      hit->SetEntryNr(link);
      sol.AddHit(PndApollonius::TripletSolution::detID::MVDstrip, hit);
    }
    if (branchName.Contains("GEMHit")) {
      FairHit *hit = (FairHit *)(fData->GetMapFairLinktoFairHit()[link]);
      hit->SetEntryNr(link);
      sol.AddHit(PndApollonius::TripletSolution::detID::GEM, hit);
    }
    if (branchName.Contains("STTHit")) {
      PndSttHit *hit = (PndSttHit *)(fData->GetMapFairLinktoFairHit()[link]);
      hit->SetEntryNr(link);
      sol.AddHit(PndApollonius::TripletSolution::detID::STT, hit);
    }
  }

  // sol.SortAllHits(fData->GetGeometryMap());
  return sol;
}

double PndHoughTrackFinder::CalcRatioSameHits(PndApollonius::TripletSolution &sol, std::vector<PndApollonius::TripletSolution> &AlreadyFoundSolutions)
{
  double MaxFraction = 0.;
  for (PndApollonius::TripletSolution foundSolution : AlreadyFoundSolutions) {
    double counter = 0.;
    for (FairHit *hit : sol.fAllHits) {
      auto it = std::find_if(foundSolution.fAllHits.begin(), foundSolution.fAllHits.end(),
                             [&hit](const FairHit *myHit) { return (myHit->GetX() == hit->GetX() && myHit->GetY() == hit->GetY()); });

      if (it != foundSolution.fAllHits.end())
        counter++;
    }
    if (counter / sol.fAllHits.size() > MaxFraction)
      MaxFraction = counter / sol.fAllHits.size();
  }
  return MaxFraction;
}

std::vector<PndApollonius::TripletSolution> PndHoughTrackFinder::CombineIdenticalSolutionsFinal(std::vector<PndApollonius::TripletSolution> &solutions, Double_t ratioOfSameHits)
{

  std::vector<PndApollonius::TripletSolution> result;
  std::sort(solutions.begin(), solutions.end(),
            [](PndApollonius::TripletSolution &first, PndApollonius::TripletSolution &second) { return (first.GetNHits() > second.GetNHits()); });
  for (PndApollonius::TripletSolution sol : solutions) {
    // cout << "MVD hits: " << sol.fHits[PndApollonius::TripletSolution::detID::MVDpixel].size() + sol.fHits[PndApollonius::TripletSolution::detID::MVDstrip].size()<< ", Gem Hits:
    // " << sol.fHits[PndApollonius::TripletSolution::detID::GEM].size() << std::endl;
    double RatioSameHits = CalcRatioSameHits(sol, result);
    // cout << "fraction of same hits " << RatioSameHits  << ", musst be smaller than ratioOfSameHits: " << ratioOfSameHits << " to be added as track" << std::endl;
    if (RatioSameHits < ratioOfSameHits) {
      result.push_back(sol);
      // cout << sol << std::endl;
    }
  }

  return result;
}

PndTrack PndHoughTrackFinder::FromTripletSolutionToPndTrack(PndApollonius::TripletSolution &sol, Double_t B)
{
  LOG(debug) << "FromTripletSolutionToPndTrack " << std::endl;
  PndTrackCand cand;
  TVector3 hitPos;
  TVector3 hitPosError(0.015, 0.015, 0.015);
  TVector3 mom;
  TVector3 momError(2, 2, 2);
  TVector3 dj(1, 0, 0);
  TVector3 dk(0, 1, 0);
  TVector3 origin(0, 0, 1);
  int i = 0;
  // std::cout << "sol.fAllHits.size(): " << sol.fAllHits.size() << std::endl;
  if (sol.fAllHits.size() < 4)
    return PndTrack();
  LOG(debug) << "sol.fAllHits: " << sol.fAllHits.size() << std::endl;
  for (auto hit : sol.fAllHits) {
    i++;
    LOG(debug) << "hit->GetEntryNr(): " << hit->GetEntryNr() << std::endl;
    cand.AddHit(hit->GetEntryNr(), i);
  }

  // if(!CheckZInfo(cand))
  //  continue;
  TVector3 circle = sol.fTrack;
  TVector2 hit2D = PANDA::CircleTools::PositionOnCircle((PndSttHit *)sol.fAllHits[0], circle);
  int direction = PANDA::CircleTools::RotationDirection((PndSttHit *)sol.fAllHits[0], (PndSttHit *)sol.fAllHits[1], circle);
  TVector2 pt = PANDA::CircleTools::PtOnCircle((PndSttHit *)sol.fAllHits[0], circle, direction, B);

  hitPos.SetXYZ(hit2D.X(), hit2D.Y(), 0);
  mom.SetXYZ(pt.X(), pt.Y(), 0);

  FairTrackParP first(hitPos, mom, hitPosError, momError, direction, origin, dj, dk);

  hit2D = PANDA::CircleTools::PositionOnCircle((PndSttHit *)sol.fAllHits.back(), circle);
  pt = PANDA::CircleTools::PtOnCircle((PndSttHit *)sol.fAllHits.back(), circle, direction, B);

  hitPos.SetXYZ(hit2D.X(), hit2D.Y(), 0);
  mom.SetXYZ(pt.X(), pt.Y(), 0);

  FairTrackParP last(hitPos, mom, hitPosError, momError, direction, origin, dj, dk);

  return PndTrack(first, last, cand);
}

ClassImp(PndHoughTrackFinder)
