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

/*
 * PndSttHitCorrector.h
 *
 *  Created on: May 20, 2014
 *      Author: schumann
 */

#ifndef PNDSTTHITCORRECTOR_H_
#define PNDSTTHITCORRECTOR_H_

#include "PndSttCellTrackFinderData.h"
#include "FairHit.h"
#include "FairLink.h"
#include "TMath.h"

//class PndSttStrawMap;
//class PndSttGeometryMap;
class PndStt2GeoHandler;
class PndSttHit;

class PndSttHitCorrector {
 public:
  PndSttHitCorrector(const PndSttCellTrackFinderData *data)
    : fVerbose(0), fHits(data->GetHits()), fSttGeoH(data->GetSttGeoH()), fMapTubeIdToHit(data->GetMapTubeIdToHit()),
    //: fVerbose(0), fHits(data->GetHits()), fStrawMap(data->GetStrawMap()), fGeometryMap(data->GetGeometryMap()), fMapTubeIdToHit(data->GetMapTubeIdToHit()),
      fHitNeighbors(data->GetHitNeighborsWithoutSkewed()), fSeparations(data->GetSeparationsWithoutSkewed()), fMinIsochrone(0.1), fIsochroneEquality(0.8),
      fDeltaDiff(TMath::DegToRad() * 20), fMaxDiffBetweenAngles(TMath::DegToRad() * 15)
  {
  }
  virtual ~PndSttHitCorrector()
  {
    for (std::map<int, FairHit *>::iterator it = fCorrectedIsochrones.begin(); it != fCorrectedIsochrones.end(); ++it) {
      delete (*it).second;
    }
  }

  void SetVerbose(Int_t verbose) { fVerbose = verbose; }

  void PrintTangentAngles();

  /* Method that calculates better hit-positions by means of the isochrones.*/
  void CorrectHits();

  std::map<int, FairHit *> GetCorrectedHits() { return fCorrectedIsochrones; }

 private:
  Int_t fVerbose;

  std::vector<FairHit *> fHits;
  //const PndSttStrawMap *fStrawMap;
  //const PndSttGeometryMap *fGeometryMap;
  PndStt2GeoHandler *fSttGeoH;
  std::map<int, int> fMapTubeIdToHit;
  std::map<int, std::vector<int>> fHitNeighbors;
  std::map<int, std::vector<int>> fSeparations;

  std::map<int, std::set<double>> fTangentAngles; // map<tube-ID, tangent angle of isochrone (suitable for track)>
  std::map<int, FairHit *> fCorrectedIsochrones;  //< hit-index, corrected hit position>

  double fMinIsochrone;         // Isochrone radius off which the corrected point is set to to the midpoint
  double fIsochroneEquality;    //
  double fDeltaDiff;            // max difference of angles (radian) for whom the average is taken
  double fMaxDiffBetweenAngles; // up to this difference tangent angles are rated as equal

  /* Calculates angles of possible tangents between the isochrones of two hits*/
  std::vector<double> CalculateTangentAngles(PndSttHit *tube1, PndSttHit *tube2);

  /* Method returns the tubeID of the hit-neighbor that lies in the middle of the other hit-neighbors
   * of the given tube. Condition: the given tube has 3 hit-neighbors that are adjacent to each other
   * (angles of 60 degree) */
  int GetMiddleHitNeighbor(int tubeID);

  /* Method returns the tubeIDs of the neighbors, that form a straight line with the given tube.*/
  std::pair<int, int> GetStraightNeighbors(int tubeID);

  /* Method searches for the best combination of angles (best match). Parameter: several vectors of angles.
   * Each angle is compared with all angles of the other vectors. Based on the calculation of the classification
   * 2 angles are returned maximally. */
  std::set<double> GetBestCombinatedPhi(const std::vector<std::vector<double>> &angles);

  /* Method calculates the distances between the angles of all vectors. It returns the calculated differences
   * and the smallest difference with the corresponding index-pair (for each pair of angles of different vectors)*/
  void CalcDifferencesBetweenAngles(const std::vector<std::vector<double>> &angles, std::vector<std::vector<std::vector<double>>> &retDifferences,
                                    std::vector<std::vector<double>> &retSmallestDiff, std::vector<std::vector<std::pair<int, int>>> &retPairsOfSmallest);

  /* Methods calculate the classification-values for the assessment of the smallest differences between the angles.
   * classification-value = 1 - (smallest_diffrence / sum_of_all_smallest_diffrences). The higher the
   * classification-value, the better the pair of angles. */
  std::vector<std::vector<double>> CalcClassification(const std::vector<std::vector<double>> &smallestDiff);

  std::vector<std::vector<double>> CalcClassification(const std::vector<std::vector<std::vector<double>>> &differences);

  /* Method creates new FairHits (in fCorrectedIsochrones) based on the calculated tangent angles. (if available)*/
  void CorrectIsochrones();

  void KeepBestAngle(int tubeID, double angle);

  double GetDiffBetweenAngles(double angle1, double angle2);

  double GetAverageOfAngles(double angle1, double angle2);

  double GetBestFittingAngle(double angle, std::set<double> anglesToTest);

  /* Method checks whether the tubes signal a hit and are neighbors.*/
  bool AreHitNeihbors(int tubeID1, int tubeID2);

  /* Method checks if an unambiguous angle was calculated for the tube*/
  bool HasUnambiguousAngle(int tubeID);

  ClassDef(PndSttHitCorrector, 1);
};

#endif /* PNDSTTHITCORRECTOR_H_ */
