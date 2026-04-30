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

#pragma once

#define _USE_MATH_DEFINES

#include <math.h>
#include <tuple>

//#include "PndSttGeometryMap.h"
#include "PndStt2GeoHandler.h"
#include "FairHit.h"
#include "PndSttHit.h"
#include "FairLogger.h"
#include "PndHoughApollonius.h"
#include "PndTrackEvaluatorDetStt.h"
#include "PndSttCA.h"
// general
using std::cout;
using std::endl;

namespace PndApollonius {

/**   @brief      Contains all distance thresholds for different detector hits to be defined as "close to the track and must be added".*/
struct Thresholds {
  double fDistanceThresholdSTTSkewed = 0.8;
  double fDistanceThresholdSTTCombinedSkewed = 1.;

  double fDistanceThresholdSTTNarrow = 0.3;
  double fDistanceThresholdSTTFar = 0.5;

  double fDistanceThresholdMVDNarrow = 0.5;
  double fDistanceThresholdMVDMid1 = 0.7;
  double fDistanceThresholdMVDMid2 = 1.;
  double fDistanceThresholdMVDFar = 2.;
  double fDistanceThresholdGEM = 1.;

  double fSTTArcLengthCut = 26.;
  double fMVDArcLengthCutNarrow = 5.;
  double fMVDArcLengthCutMid = 7.;
  double fMVDArcLengthCutFar = 10.;
};

/**   @brief      A structure that defines a triplet consisting of three STT Hits.*/
struct Triplet {
  std::array<PndSttHit *, 3> fTripletHits;
  friend std::ostream &operator<<(std::ostream &output, const Triplet &t)

  {
    output << "Triplet: ";
    for (auto hit : t.fTripletHits)
      output << hit->GetTubeID() << "(" << hit->GetX() << "," << hit->GetY() << ") /";
    output << std::endl;

    return output;
  }
};

/**   @brief      A structure that defines a solution of a triplet (an Apollonius Circle and all hits close to the circle).*/
struct TripletSolution {

  enum detID { MVDpixel, MVDstrip, STT, GEM, NOTDEFINED };
  std::vector<std::string> detID_Names{"MVD_Pixel", "MVD_Strip", "STT", "GEM", "NOTDEFINED"};

  /**   @brief      Constructor*/
  TripletSolution(){};

  /**   @brief      Constructor
   *
   * @param[in]  track              A TVector3 that defines the circle parameters (x_center, y_center, R) from  the Apollonius Circle
   */
  TripletSolution(TVector3 track) : fTrack(track){};

  /**   @brief      A vector of hits is added
   *
   * @param[in]  detector       ID of the corresponding subdetector (MVD, STT or GEM)
   * @param[in]  data           All hits of one subdetector that belong to the track
   */
  void AddHits(detID detector, std::vector<FairHit *> data)
  {
    fHits[detector] = data;
    fAllHits.insert(fAllHits.end(), data.begin(), data.end());
  }

  /**   @brief      A vector of STT hits is added
   *
   * @param[in]  sttHits        All hits of the STT that belong to the track as PndSttHit.
   */
  void AddHits(std::vector<PndSttHit *> sttHits)
  {
    fHits[STT].insert(fHits[STT].end(), sttHits.begin(), sttHits.end());
    fAllHits.insert(fAllHits.end(), sttHits.begin(), sttHits.end());
  }

  /**   @brief      One hit of a specific subdetector is added
   *
   * @param[in]  detector       ID of the corresponding subdetector (MVD, STT or GEM)
   * @param[in]  data           A pointer to one specific hit of a subdetector that belong to the track
   */
  void AddHit(detID detector, FairHit *data)
  {
    if (fHits.count(detector) == 0) {
      fHits[detector];
    }
    if (std::find(fHits[detector].begin(), fHits[detector].end(), data) == fHits[detector].end()) { // add only if hit is not in dataset
      fHits[detector].push_back(data);
      fAllHits.push_back(data);
    }
  }

  /**   @brief      Returns the total number of hits in a TripletSolution*/
  int GetNHits() { return fHits[MVDpixel].size() + fHits[MVDstrip].size() + fHits[STT].size() + fHits[GEM].size(); }

  /**   @brief      Determins a vector from the circle center to the corresponding hit
   *
   * @param[in]  hit       The corresponding hit
   * @param[out] TVector2  The vector from the circle center to the corresponding hit
   */
  TVector2 HitOnTrack(FairHit *hit) { return TVector2(hit->GetX() - fTrack.X(), hit->GetY() - fTrack.Y()); }

  /**   @brief      Determins the rotation of the circle, i.e. if the particle mooves clockwise or not */
  void IsClockwise();

  /**   @brief      Sorts the hits based on the angle from first hit to circle center vs hit to circle center
   *
   * @param[in]  firstHit  The first hit of the track
   * @param[in]  detector  The corresponding subdetector
   */
  void SortHits(TVector2 &firstHit, detID detector);

  /**   @brief      Sorts the STT hits: first based on the STT layer to determine the first hit and after that based on the angle from first hit to circle center vs hit to circle
   * center
   *
   * @param[in]  fGeometryMap  The Geometry map of the detector to return the STT layer
   */
  void SortStt(PndStt2GeoHandler *fGeometryMap);
  //void SortStt(PndSttGeometryMap *fGeometryMap);

  /**   @brief      Sorts the STT hits: first based on the STT layer to determine the first hit and after that based on the angle from first hit to circle center vs hit to circle
   * center
   *
   * @param[in]   fGeometryMap  The Geometry map of the detector
   * @param[out]  FairHit       Returns the first hit of the track
   */
  FairHit *GetFirstHit(PndStt2GeoHandler *fGeometryMap);
  //FairHit *GetFirstHit(PndSttGeometryMap *fGeometryMap);

  /**   @brief      Sorts all hits depending on if the track changed or not
   *
   * @param[in]  fGeometryMap  The Geometry map of the detector to return the STT layer
   */
  void SortAllHits(PndStt2GeoHandler *fGeometryMap);
  //void SortAllHits(PndSttGeometryMap *fGeometryMap);

  /**   @brief      Tests if a specific hit is a reasonable hit of the track (based on the angle and the rotation of the track)
   *
   * @param[in]  CurrentHit   The current hit
   * @param[in]  PriviousHit  The previous hit of the track
   */
  bool IsReasonableHit(FairHit *CurrentHit, FairHit *PriviousHit);

  friend std::ostream &operator<<(std::ostream &output, const TripletSolution &t)
  {
    output << "Track x/y/r " << t.fTrack.X() << "/" << t.fTrack.Y() << "/" << t.fTrack.Z() << " ";
    if (t.fTriplet.fTripletHits[0] != nullptr) {
      output << " trip: " << t.fTriplet.fTripletHits[0]->GetTubeID() << "/" << t.fTriplet.fTripletHits[1]->GetTubeID() << "/" << t.fTriplet.fTripletHits[2]->GetTubeID() << " ";
    }
    if (t.fHits.count(TripletSolution::detID::MVDpixel) > 0) {
      output << t.detID_Names.at(TripletSolution::detID::MVDpixel) << " : ";
      for (auto hit : t.fHits.at(TripletSolution::detID::MVDpixel)) {
        output << hit->GetEntryNr() << "/";
      }
      output << " ";
    }
    if (t.fHits.count(TripletSolution::detID::MVDstrip) > 0) {
      output << t.detID_Names.at(TripletSolution::detID::MVDstrip) << " : ";
      for (auto hit : t.fHits.at(TripletSolution::detID::MVDstrip)) {
        output << hit->GetEntryNr() << "/";
      }
      output << " ";
    }
    if (t.fHits.count(TripletSolution::detID::STT) > 0) {
      output << t.detID_Names.at(TripletSolution::detID::STT) << " : ";
      for (auto hit : t.fHits.at(TripletSolution::detID::STT)) {
        PndSttHit *myHit = static_cast<PndSttHit *>(hit);
        output << myHit->GetTubeID() << "/";
      }
      output << " ";
    }
    if (t.fHits.count(TripletSolution::detID::GEM) > 0) {
      output << t.detID_Names.at(TripletSolution::detID::GEM) << " : ";
      for (auto hit : t.fHits.at(TripletSolution::detID::GEM)) {
        output << hit->GetEntryNr() << "/";
      }
      output << " ";
    }

    output << "MSD: " << t.fMeanSquare;
    return output;
  }

  Triplet fTriplet{nullptr, nullptr, nullptr}; //< triplet used to create solution
  TVector3 fTrack;                             //< circle coordinates of track in x,y,r in [cm]
  std::map<detID, std::vector<FairHit *>> fHits;
  std::vector<FairHit *> fAllHits;
  double fMeanSquare = 1000000.;
  bool fClockwise = true;
};

/**   @brief      A structure that defines the three sets of STT hits (inner, mid, outer STT set), that are used to combine triplets.*/
struct TripletValues {
  std::vector<PndSttHit *> fFirstRow;
  std::vector<PndSttHit *> fMidRow;
  std::vector<PndSttHit *> fLastRow;

  TripletValues &operator+=(const TripletValues &right)
  {
    fFirstRow.insert(fFirstRow.end(), right.fFirstRow.begin(), right.fFirstRow.end());
    fMidRow.insert(fMidRow.end(), right.fMidRow.begin(), right.fMidRow.end());
    fLastRow.insert(fLastRow.end(), right.fLastRow.begin(), right.fLastRow.end());

    return *this;
  }

  friend std::ostream &operator<<(std::ostream &output, const TripletValues &t)
  {
    output << "FirstRow: ";
    std::for_each(t.fFirstRow.begin(), t.fFirstRow.end(), [](PndSttHit *hit) { std::cout << hit->GetTubeID() << "(" << hit->GetX() << "," << hit->GetY() << ") /"; });
    output << std::endl;
    output << "MidRow: ";
    std::for_each(t.fMidRow.begin(), t.fMidRow.end(), [](PndSttHit *hit) { std::cout << hit->GetTubeID() << "(" << hit->GetX() << "," << hit->GetY() << ") /"; });
    output << std::endl;
    output << "LastRow: ";
    std::for_each(t.fLastRow.begin(), t.fLastRow.end(), [](PndSttHit *hit) { std::cout << hit->GetTubeID() << "(" << hit->GetX() << "," << hit->GetY() << ") /"; });
    output << std::endl;

    return output;
  }
};

/**   @brief      A structure that defines all maps that are used to reduce to number of possible triplet combinations.*/
struct ReductionMaps {
  std::map<int, PndSttHit *> fMapCAToMidTube;
  std::map<int, PndSttHit *> fMapCAToOuterTube;
  std::map<PndSttHit *, int> fMapInnerTubeToCA;
  std::map<PndSttHit *, int> fMapMidTubeToCA;

  void Reset()
  {
    fMapCAToMidTube.clear();
    fMapCAToOuterTube.clear();
    fMapInnerTubeToCA.clear();
    fMapMidTubeToCA.clear();
  }
};

/**   @brief      A structure that defines all functions used for the Apollonius Triplet track finder.*/
struct ApolloniusTripletFunctions {
  /**
   * @brief      Sorts all STT hits by row and returns a map that connects each row with the hits in that row.
   *
   * @param[in]  hits             STTHits of one preselected group
   * @param[in]  fGeometryMap     The Geometry of the detector to get the STT row
   * @param[in]  fAllHitsCounter  Counts all hits in the group to set a maximum number of hits, i.e. if too many hits are in a group no tracking is performed for that group
   * @param[out] result           A map that connects each row with the hits in that row.
   */
  std::map<int, std::vector<PndSttHit *>> GetAllTubesByRow(std::vector<PndSttHit *> hits, PndStt2GeoHandler *fGeometryMap, int &fAllHitsCounter);
  //std::map<int, std::vector<PndSttHit *>> GetAllTubesByRow(std::vector<PndSttHit *> hits, PndSttGeometryMap *fGeometryMap, int &fAllHitsCounter);

  /**
   * @brief      Creates a map that connects each STT row with a set of groups. All STT hits in one group are directly adjacent to each other.
   *
   * @param[in]  hits             STTHits of one preselected group
   * @param[in]  fGeometryMap     The Geometry of the detector to get the STT row
   * @param[in]  fAllHitsCounter  Counts all hits in the group to set a maximum number of hits, i.e. if too many hits are in a group no tracking is performed for that group
   * @param[in]  fIsStrongCurling A group is defined as strongly curling if it has more than 5 separated groups in one row and more than 2 "curling rows". However this definition
   * has to be improved
   * @param[out] result           A map connecting each row with groups of STT hits. If there are hits in one row that are adjacent these hits are declared as a group
   */
  std::map<int, std::vector<std::vector<PndSttHit *>>>
  GetTubeStructure(std::vector<PndSttHit *> hits, PndStt2GeoHandler *fGeometryMap, int &fAllHitsCounter, bool &fIsStrongCurling);
  //GetTubeStructure(std::vector<PndSttHit *> hits, PndSttGeometryMap *fGeometryMap, int &fAllHitsCounter, bool &fIsStrongCurling);

  /**
   * @brief      Created maps that connect the hits with a corresponding CA tracklet
   *
   * @param[in]  triplets               All inner, mid and outer tubes
   * @param[in]  fMapHitstoCATracklet   A map that connects each hit to the corresponding CA tracklet
   */
  ReductionMaps CreateMaps(TripletValues &triplets, std::map<FairLink, int> &fMapHitstoCATracklet);

  /**
   * @brief      Find triplet combinations with higher probability to be a proper one
   *
   * @param[in]  triplets             All inner, mid and outer tubes
   * @param[in]  fMapHitstoCATracklet A map that connects each hit to the corresponding CA tracklet
   * @param[in]  sttHits              All STT hits
   * @param[in]  fCATrackFinder       The Cellular Automaton
   * @param[in]  fGeometryMap         The Geometry of the detector
   * @param[in]  fMinDistance         A minimum distance between the innermost and outermost hit
   * @param[in]  fWithCombiReduction  A bool to test if the number of combinations should be reduced or to use all combinations
   * @param[out] result               A vector of triplets
   */
  std::vector<Triplet> ReduceCombinatorics(TripletValues &triplets, std::map<FairLink, int> &fMapHitstoCATracklet, TClonesArray *sttHits, PndSttCA *fCATrackFinder,
                                           PndStt2GeoHandler *fGeometryMap, double &fMinDistance, bool &fWithCombiReduction);
                                           //PndSttGeometryMap *fGeometryMap, double &fMinDistance, bool &fWithCombiReduction);

  /**
   * @brief      Checks if the triplet is already found in a good solution
   *
   * @param[in]  solutions    All already found good solutions
   * @param[in]  triplet      One Triplet
   * @param[out] bool         is already found (true) or not (false)
   */
  bool IsTripletUsed(std::vector<TripletSolution> &solutions, Triplet &triplet);

  /**
   * @brief      Generate tracks for one triplet
   *
   * @param[in]  triplet      One Triplet
   * @param[in]  sttHits      All STT hits
   * @param[in]  fGeometryMap The geometry of the detector
   * @param[out] result       A vector of all solutions for one triplet
   */
  std::vector<TripletSolution> GenerateTripletTracks(Triplet triplet, std::vector<PndSttHit *> &sttHits, PndStt2GeoHandler *fGeometryMap);
  //std::vector<TripletSolution> GenerateTripletTracks(Triplet triplet, std::vector<PndSttHit *> &sttHits, PndSttGeometryMap *fGeometryMap);

  /**
   * @brief      Determines the intersection points of a Circle and a Line
   *
   * @param[in]  circle      The circle parameters
   * @param[in]  m           The slope of the line (y = m*x + b)
   * @param[in]  b           The y axis intersection of the line
   * @param[in]  Ax          A control parameter that is used if the line is the x-axis
   * @param[out] result      A vector of all solutions for one triplet
   */
  std::vector<double> calcIntersectionPointCircleLine(std::vector<double> circle, double m, double b, double Ax);

  /**
   * @brief      Finds all hits that are close to a specific circle
   *
   * @param[in]  sttHits                  A vector of all stt hits in a preselected group
   * @param[in]  circle                   An apollonius circle
   * @param[in]  fDistanceThresholdSTTFar The distance threshold used to define an STT hit to be close to the circle
   * @param[in]  fGeometryMap             The Geometry of detector to check if hits are skewed layer
   * @param[out] result                   A vector of all STT hits that are close to the circle
   */
  TripletSolution FindHitsCloseToCircle(std::vector<PndSttHit *> &sttHits, TVector3 &circle, double &fDistanceThresholdSTTFar, PndStt2GeoHandler *fGeometryMap);
  //TripletSolution FindHitsCloseToCircle(std::vector<PndSttHit *> &sttHits, TVector3 &circle, double &fDistanceThresholdSTTFar, PndSttGeometryMap *fGeometryMap);

  /**
   * @brief      Determines the mean square distance of the hits to the circle
   *
   * @param[in]  solution    The found track
   * @param[out] result      The mean square distance of the hits to the circle
   */
  double MeanSquareDistance(TripletSolution &solution);

  /**
   * @brief      Determines the distance between a point and a circle
   *
   * @param[in]  circle      The circle parameters
   * @param[in]  hit         The hit
   * @param[out] result      The distance between the point and the circle
   */
  double DistanceCirclePoint(TVector3 &circle, FairHit *hit);

  /**
   * @brief      Determines the distance between a STT Hit and a circle
   *
   * @param[in]  circle      The circle parameters
   * @param[in]  sttHit      The STT hit
   * @param[out] result      The distance between the STT hit and the circle
   */
  double DistanceCircleSttHit(TVector3 &circle, PndSttHit *sttHit);

  /**
   * @brief      Determines the distance between a STT Hit and a circle if the squared distance between the ceneters of circle and hit are already known
   *
   * @param[in]  circle         The circle parameters
   * @param[in]  sttHit         The STT hit
   * @param[in]  sqaredDistance The squared distance between the center of the circle and the center of the STT Hits
   * @param[out] result         The distance between the STT hit and the circle
   */
  double DistanceCircleSttHit(TVector3 &circle, PndSttHit *sttHit, double &sqaredDistance);

  /**
   * @brief      Determines the distance between a STT Hit and a line if the squared distance between the line and hit are already known
   *
   * @param[in]  circle         The circle parameters
   * @param[in]  sttHit         The STT hit
   * @param[in]  sqaredDistance The squared distance between the center of the circle and the center of the STT Hits
   * @param[out] result         The distance between the STT hit and the circle
   */
  double DistanceLineSttHit(TVector3 &circle, PndSttHit *sttHit, double &squaredDistance) { return sqrt(squaredDistance); };

  /**
   * @brief      Determines the distance between a STT Hit and a line
   *
   * @param[in]  circle         The circle parameters
   * @param[in]  sttHit         The STT hit
   * @param[out] result         The distance between the STT hit and the circle
   */
  double DistanceLineSttHit(TVector3 &circle, PndSttHit *sttHit);

  /**
   * @brief      Determines the squared distance between a STT Hit and a line
   *
   * @param[in]  circle         The circle parameters
   * @param[in]  sttHit         The STT hit
   * @param[out] result         The squred distance between the STT hit and the circle
   */
  double SquaredDistanceLineSttHit(TVector3 &circle, PndSttHit *sttHit);

  /**
   * @brief      Determines the distance between a point and a line
   *
   * @param[in]  circle         The circle parameters
   * @param[in]  hit            The point
   * @param[out] result         The distance between the point and the circle
   */
  double DistanceLinePoint(TVector3 &circle, FairHit *hit);

  /**
   * @brief      Checks if a track candidate is continuous in the STT
   *
   * @param[in]  solutions    A vector of possible track solutions
   * @param[in]  fGeometryMap The geometry of the detector to identify skewed layers
   */
  void CheckContinuitySolutions(std::vector<TripletSolution> &solutions, PndStt2GeoHandler *fGeometryMap);
  //void CheckContinuitySolutions(std::vector<TripletSolution> &solutions, PndSttGeometryMap *fGeometryMap);

  /**
   * @brief      Checks if the found STT hits are continuous
   *
   * @param[in]  solutions    A vector of all solutions for one triplet
   * @param[in]  fGeometryMap The geometry of the detector to identify skewed layers
   * @param[out] bool
   */
  bool IsContinuous(TripletSolution &solution, PndStt2GeoHandler *fGeometryMap);
  //bool IsContinuous(TripletSolution &solution, PndSttGeometryMap *fGeometryMap);

  /**
   * @brief      Add MVD and GEM hits to solutions
   *
   * @param[in]  solutions    A vector of all solutions for one triplet
   * @param[in]  fStrawMap    The geometry of the STT
   * @param[in]  fBranchMap   All hits that are still not assigned to a track
   * @param[in]  sttname      The name of the original STT branch
   */
  void AddOtherDetectors(vector<TripletSolution> &solutions, PndStt2GeoHandler *fStrawMap, std::map<TString, TClonesArray *> &fBranchMap, TString sttname);
  //void AddOtherDetectors(vector<TripletSolution> &solutions, PndSttStrawMap *fStrawMap, std::map<TString, TClonesArray *> &fBranchMap, TString sttname);

  /**
   * @brief      Select best solution(s) out of the 8 generated by triplet
   * @param[in]  solutions    A vector of all possible track candidates for one triplet
   * @param[out] result       A vector of the best candidates for one triplet
   */
  std::vector<TripletSolution> FindBestSolutions(std::vector<TripletSolution> &solutions);

  /**
   * @brief      Combines and Checks all Solutions to generate only the true particle tracks
   *
   * @param[in]  solutions          A vector of the best candidates for one triplet
   * @param[in]  nExpectedTracks    Number of expected tracks.
   * @param[out] result             A vector of found track candidates.
   */
  std::vector<TripletSolution> CheckCombinedSolutions(std::vector<TripletSolution> &solutions, int nExpectedTracks);

  /**
   * @brief      Check Combined Track Solutions if they are a possible solution
   *
   * @param[in]  solutions            A vector of the best candidates for one triplet
   * @param[in]  combinations         A vector of a vector containing all tracks that might belong together
   * @param[out] result               A vector of found track candidates.
   */
  std::vector<TripletSolution> CheckSolutions(std::vector<TripletSolution> &solutions, std::vector<std::vector<int>> combinations);

  /**
   * @brief      returns a vector of all tubes of all tracks in one combination
   *
   * @param[in]  solutions            A vector of possible track soluutions
   * @param[in]  combinations         A possible combination for tracks that might be the same track
   * @param[out] result               A vector of all tubes of all tracks in one combination.
   */
  std::vector<int> GetUniqueTubeIDs(std::vector<TripletSolution> &solutions, std::vector<int> combinations);

  /**
   * @brief      Determines all possible combinations of tracks
   *
   * @param[in]  k            The number of expected tracks
   * @param[in]  n            The number of existing tracks
   * @param[out] result       A vector containing all possible index combinations
   */
  std::vector<std::vector<int>> GetKOutOfN(int k, int n);

  /**
   * @brief      Combines identical solutions to one solution
   *
   * @param[in]  solutions    A vector of all track solutions before merging
   * @param[out] result       A vector of all track solutions after merging of identical solutions
   */
  std::vector<TripletSolution> CombineIdenticalSolutions(std::vector<TripletSolution> &solutions);

  /**
   * @brief      Determines if a solution already contains a triplet
   *
   * @param[in]  triplet      The triplet that is investigated
   * @param[in]  solution     The solution that is investigated if the triplet is alraedy contained
   */
  bool ContainsTriplet(Triplet &triplet, TripletSolution &solution);

  /**
   * @brief      Reduces the number of tubes chosen for combination--> if several tubes in one region (inner, mid or outer) belong to the same CA tracklet only the last tube is
   chosen

   * @param[in]  Rows                 The row frum which to choose the tubes
   * @param[in]  tubeStructure        A map connecting each row with groups of STT hits. If there are hits in one row that are adjacent these hits are declared as a group
   * @param[in]  fMapHitstoCATracklet A map that connects hits to the corresponding CA tracklet
   * @param[in]  position             An index that indicates if the first (0), mid (1) od last (2) row is reduced.
   * @param[out] result               Stt hits chosen for combination
   */
  std::vector<PndSttHit *> TubeReduction(std::vector<std::pair<int, int>> &Rows, std::map<int, std::vector<std::vector<PndSttHit *>>> &tubeStructure,
                                         std::map<FairLink, int> &fMapHitstoCATracklet, int position);
  /**
   * @brief      Returns a map that connects hits to the corresponding CA tracklet

   * @param[in]  fCATrackFinder      The Cellular Automaton
   */
  std::map<FairLink, int> GetHitsToCAMap(PndSttCA *fCATrackFinder);

  /**
   * @brief      Determins how many hits of two solutions are identical

   * @param[in]  sol                   The solution of interest
   * @param[in]  AlreadyFoundSolutions All already found solutions
   * @param[out] result                maximum ratio of identical hits
   */

  double CalcRatioSameHits(TripletSolution &sol, std::vector<TripletSolution> &AlreadyFoundSolutions);
  /**
   * @brief      Combines identical solutions

   * @param[in]  solutions            The solution of interest
   * @param[in]  ratioOfSameHits      Ratio of same hits used as threshold to Combine the solutions
   * @param[out] result               A vector of found solutions
   */
  std::vector<TripletSolution> CombineIdenticalSolutionsFinal(std::vector<TripletSolution> &solutions, Double_t ratioOfSameHits = 0.6);

  /**
   * @brief Converts a TripletSolution to a PndTrack
   *
   * @param[in]  sol    The TripletSolution
   * @param[out] result The PndTrack
   */
  PndTrack FromTripletSolutionToPndTrack(PndApollonius::TripletSolution &sol, Double_t B);

  /**
   * @brief Converts a TripletSolution to a PndTrack
   *
   * @param[in]  sol    The TripletSolution
   * @param[in]  cand   A PndTrackCand that is different from the one chosen in the sol.
   * @param[out] result The PndTrack
   */
  PndTrack FromTripletSolutionToPndTrack(PndApollonius::TripletSolution &sol, Double_t B, PndTrackCand &cand);
};

} // namespace PndApollonius

/** @} */
