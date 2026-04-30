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
 * PndSttHitCorrector.cxx
 *
 *  Created on: May 20, 2014
 *      Author: schumann
 */

#include "PndSttHitCorrector.h"
#include "PndSttHit.h"
#include "TVector3.h"
#include "PndStt2GeoHandler.h"

using namespace std;

ClassImp(PndSttHitCorrector);

void PndSttHitCorrector::CorrectHits()
{

  if (fVerbose > 1)
    cout << "PndSttHitCorrector::CorrectHits" << endl;

  // for each tube with 2 active neighbors
  for (size_t i = 0; i < fSeparations[2].size(); ++i) {
    int actualTubeId = fSeparations[2][i];

    // continue if straw is skewed
    if (fSttGeoH->IsSkewedStraw(actualTubeId))
    //if (fStrawMap->IsSkewedStraw(actualTubeId))
      continue;

    if (fVerbose > 2)
      cout << "Calculation for tube: " << actualTubeId << endl;

    // vector of tangent angles for each neighbor
    vector<vector<double>> angles;

    // calculate if both neighbors are unskewed
    if (!fSttGeoH->IsSkewedStraw(fHitNeighbors[actualTubeId][0]) && !fSttGeoH->IsSkewedStraw(fHitNeighbors[actualTubeId][1])) {
    //if (!fStrawMap->IsSkewedStraw(fHitNeighbors[actualTubeId][0]) && !fStrawMap->IsSkewedStraw(fHitNeighbors[actualTubeId][1])) {

      // for each neighbor of actual tube
      for (size_t j = 0; j < fHitNeighbors[actualTubeId].size(); j++) {
        int neighborTubeId = fHitNeighbors[actualTubeId][j];

        if (fVerbose > 3)
          cout << "NeighborTube: " << neighborTubeId << " : " << endl;

        // calculate angles of tangents between the isochrones
        angles.push_back(CalculateTangentAngles((PndSttHit *)fHits[fMapTubeIdToHit[actualTubeId]], (PndSttHit *)fHits[fMapTubeIdToHit[neighborTubeId]]));
      }

      // search for the best combination of tangents and store the angles of the best one
      set<double> bestPhi = GetBestCombinatedPhi(angles);

      if (bestPhi.size() == 2) {
        double absDiff = abs(*bestPhi.begin() - *(++bestPhi.begin()));
        if (absDiff < fDeltaDiff) {

          set<double> tmp;
          tmp.insert(GetAverageOfAngles(*bestPhi.begin(), *(++bestPhi.begin())));
          fTangentAngles[actualTubeId] = tmp;
        } else {
          // store both values
          fTangentAngles[actualTubeId] = bestPhi;
        }
      } else if (bestPhi.size() == 1) {
        fTangentAngles[actualTubeId] = bestPhi;

      } else if (fVerbose > 2)
        cout << "Found no good combination of tangent angles for tube " << actualTubeId << endl;
    }
  } // Initialization of possible tangent angles for tubes with two active neighbors done

  // calculate unambiguous tangent angles of cells with 3 active neighbors
  if (fVerbose > 2)
    cout << "Search for special positioning of 3 active neighbors" << endl;
  map<int, set<double>> tmpTangentAngles;

  int midNeighbor;
  double phi1, phi2;
  for (size_t i = 0; i < fSeparations[3].size(); ++i) {
    int currentTube = fSeparations[3].at(i);

    // if currentTube is not in fTangentAngles
    if (fTangentAngles.find(currentTube) == fTangentAngles.end()) {

      // check constellation of neighbors
      midNeighbor = GetMiddleHitNeighbor(currentTube);
      if (midNeighbor != 0) {
        // calculate angle of the connecting vector between the tubes
        phi1 = fSttGeoH->GetAngleBetweenTubes(currentTube, midNeighbor);
        //phi1 = fGeometryMap->GetAngleBetweenTubes(currentTube, midNeighbor);
        fTangentAngles[currentTube].insert(phi1);

        phi2 = phi1 + TMath::Pi();
        if (phi2 > 2 * TMath::Pi())
          phi2 -= 2 * TMath::Pi();
        // update the neighbor
        fTangentAngles[midNeighbor].insert(phi2);
        if (fVerbose > 3)
          cout << "current tube: " << currentTube << ", midNeighbor: " << midNeighbor << ", angles: " << phi1 * TMath::RadToDeg() << ", " << phi2 * TMath::RadToDeg() << endl;
      }
    }
  }

  if (fVerbose > 2) {
    cout << "Initialization done. Current fTangentAngles: " << endl;
    PrintTangentAngles();
    cout << "Adaption of angles: " << endl;
  }

  /* Adapt angles of the unambiguous tubes. Approach: take tubes with 2, 3 or 4 neighbors into account
   * and search for unambiguous neighbors, that form a straight line*/

  set<int> tubesToCheck;

  for (int i = 2; i < 5; ++i) {
    for (size_t j = 0; j < fSeparations[i].size(); ++j) {
      int actualTubeId = fSeparations[i].at(j);
      if (fTangentAngles.find(actualTubeId) == fTangentAngles.end() || fTangentAngles[actualTubeId].size() != 1) {
        // if two neighbors and ambiguous or if more neighbors and angles were not calculated yet
        tubesToCheck.insert(actualTubeId);
      }
    }
  }

  set<int> tmpTubesToCheck;
  bool anglesChanged = true;

  // while unambiguous tangent angles were found
  while (anglesChanged) {

    if (fVerbose > 3) {
      cout << "start loop - adaption of angles" << endl;
      cout << "tubes to Check: " << tubesToCheck.size() << endl;
    }

    anglesChanged = false;
    tmpTubesToCheck.clear();

    for (set<int>::iterator it = tubesToCheck.begin(); it != tubesToCheck.end(); ++it) {

      // must be checked because neighbor of actual tube could be changed in one situation
      if (fTangentAngles.find(*it) == fTangentAngles.end() || fTangentAngles[*it].size() != 1) {

        int actualTubeId = *it;
        pair<int, int> straightNeighbors = GetStraightNeighbors(actualTubeId);

        if (fVerbose > 3)
          cout << "currentTube: " << actualTubeId << endl;

        // found straight neighbors?
        if (straightNeighbors.first != 0) {
          // get unambiguous neighbor
          // search for unambiguous neighbor
          int unambiguousNeighbor = 0;

          if (fHitNeighbors[actualTubeId].size() == 3) {
            // get neighbor in straight line that lies next to the other neighbor (which is not in straight line)
            int thirdNeighbor;
            for (size_t j = 0; j < fHitNeighbors[actualTubeId].size(); ++j) {
              if (fHitNeighbors[actualTubeId].at(j) != straightNeighbors.first && fHitNeighbors[actualTubeId].at(j) != straightNeighbors.second)
                thirdNeighbor = fHitNeighbors[actualTubeId].at(j);
            }

            // set unambiguous neighbor
            if (AreHitNeihbors(straightNeighbors.first, thirdNeighbor) && HasUnambiguousAngle(straightNeighbors.first)) {
              unambiguousNeighbor = straightNeighbors.first;
            } else if (AreHitNeihbors(straightNeighbors.second, thirdNeighbor) && HasUnambiguousAngle(straightNeighbors.second)) {
              unambiguousNeighbor = straightNeighbors.second;
            }

          } else {
            // acutalTube has 2 or 4 hit-neighbors

            // track passed the isochrones of the straight neighbors on the same side
            if (HasUnambiguousAngle(straightNeighbors.first)) {

              unambiguousNeighbor = straightNeighbors.first;

            } else if (HasUnambiguousAngle(straightNeighbors.second)) {

              unambiguousNeighbor = straightNeighbors.second;
            }
          }

          if (unambiguousNeighbor != 0) {
            if (fVerbose > 3)
              cout << "unambiguous neighbor: " << unambiguousNeighbor << endl;

            // for checking the radius of the isochrones
            PndSttHit *actualHit = (PndSttHit *)fHits[fMapTubeIdToHit[actualTubeId]];
            PndSttHit *firstNeigh = (PndSttHit *)fHits[fMapTubeIdToHit[straightNeighbors.first]];
            PndSttHit *secondNeigh = (PndSttHit *)fHits[fMapTubeIdToHit[straightNeighbors.second]];

            // double keep; //[R.K. 01/2017] unused variable?
            double similarAngle;

            if (actualHit->GetIsochrone() > firstNeigh->GetIsochrone() || actualHit->GetIsochrone() > secondNeigh->GetIsochrone()) {
              // isochrone of actual tube is not the smallest one
              // assumption: track passes the isochrones along the same side --> keep same/similar angle
              similarAngle = *fTangentAngles[unambiguousNeighbor].begin();
              if (fVerbose > 3)
                cout << "isochrone is not the smallest one --> keep similar" << endl;

            } else if (actualHit->GetIsochrone() / min(secondNeigh->GetIsochrone(), firstNeigh->GetIsochrone()) >= fIsochroneEquality) {
              // isochrones of actual tube and neighbors are nearly equal, track does not change the side
              // --> keep similar

              similarAngle = *fTangentAngles[unambiguousNeighbor].begin();

              if (fVerbose > 3)
                cout << "nearly equal isochrones for tube " << actualTubeId << ": actual " << actualHit->GetIsochrone() << ", second neigh " << secondNeigh->GetIsochrone()
                     << ", first neigh: " << firstNeigh->GetIsochrone() << "--> keep similar" << endl;
            } else {
              // isochrone of actual tube is the smallest one
              // assumption: track changes the side. Straight neighbor with the biggest isochrone must have
              // the same/similar angle. The other neighbor the opposing one.

              int biggestNeigh, otherNeigh;
              if (firstNeigh->GetIsochrone() > secondNeigh->GetIsochrone()) {
                biggestNeigh = straightNeighbors.first;
                otherNeigh = straightNeighbors.second;
              } else {
                biggestNeigh = straightNeighbors.second;
                otherNeigh = straightNeighbors.first;
              }

              if (fVerbose > 3) {
                cout << "Isochrones (actual, first, second): " << actualHit->GetIsochrone() << ", " << firstNeigh->GetIsochrone() << ", " << secondNeigh->GetIsochrone() << endl;
                cout << "isochrone is the smallest one, biggest neighbor: " << biggestNeigh << endl;
              }

              if (unambiguousNeighbor == biggestNeigh) {
                // kepp same/similar angle
                similarAngle = *fTangentAngles[unambiguousNeighbor].begin();

                // update angle of the other neighbor (if neccessary)
                if (fTangentAngles.find(otherNeigh) == fTangentAngles.end() || fTangentAngles[otherNeigh].size() != 1) {

                  if (fVerbose > 3)
                    cout << "keep similar, change neighbor " << otherNeigh << " to opposite" << endl;

                  double otherAngle = similarAngle + TMath::Pi();
                  if (otherAngle >= 2 * TMath::Pi())
                    otherAngle -= 2 * TMath::Pi();
                  KeepBestAngle(otherNeigh, otherAngle);
                }

              } else {
                // keep opposing angle
                similarAngle = *fTangentAngles[unambiguousNeighbor].begin() + TMath::Pi();
                if (similarAngle >= 2 * TMath::Pi())
                  similarAngle -= 2 * TMath::Pi();

                if (fTangentAngles.find(otherNeigh) == fTangentAngles.end() || fTangentAngles[otherNeigh].size() != 1) {
                  if (fVerbose > 3)
                    cout << "keep opposite, change neighbor " << otherNeigh << " to similar" << endl;
                  KeepBestAngle(otherNeigh, *fTangentAngles[unambiguousNeighbor].begin());
                }
              }
            }

            // store angle for actualTube in fTangentAngles
            KeepBestAngle(actualTubeId, similarAngle);

            // start next loop!
            anglesChanged = true;
            if (fVerbose > 3)
              cout << "angles changed for " << actualTubeId << endl;

          } else {
            // try in next loop
            tmpTubesToCheck.insert(actualTubeId);
            if (fVerbose > 3)
              cout << "try " << actualTubeId << " in next loop" << endl;
          }
        } else if (fTangentAngles.find(actualTubeId) != fTangentAngles.end() && fHitNeighbors[actualTubeId].size() == 2 &&
                   !AreHitNeihbors(fHitNeighbors[actualTubeId].at(0), fHitNeighbors[actualTubeId].at(1))) {
          // tube has two neighbors and two possible angles

          if (fVerbose > 3)
            cout << "handle zigzag for tube " << actualTubeId << endl;

          int firstNeigh = fHitNeighbors[actualTubeId].at(0);
          int secondNeigh = fHitNeighbors[actualTubeId].at(1);
          int unambiguousNeighor = 0;

          if (HasUnambiguousAngle(firstNeigh)) {
            unambiguousNeighor = firstNeigh;
          } else if (HasUnambiguousAngle(secondNeigh)) {
            unambiguousNeighor = secondNeigh;
          }

          if (unambiguousNeighor != 0) {
            double similarAngle = *fTangentAngles[unambiguousNeighor].begin();

            // check if angles lie close to each other, compare with first
            if (GetDiffBetweenAngles(similarAngle, *fTangentAngles[actualTubeId].begin()) > TMath::Pi() / 2) {

              // opposing angle is similar --> change
              similarAngle = (similarAngle + TMath::Pi());

              if (similarAngle >= 2 * TMath::Pi())
                similarAngle -= 2 * TMath::Pi();
            }

            double best = GetBestFittingAngle(similarAngle, fTangentAngles[actualTubeId]);
            KeepBestAngle(actualTubeId, best);

            if (fVerbose > 3)
              cout << "unambiguous neighbor: " << unambiguousNeighor << ", angle: " << *fTangentAngles[unambiguousNeighor].begin() * TMath::RadToDeg()
                   << ", similar: " << similarAngle * TMath::RadToDeg() << ", keep:" << best * TMath::RadToDeg() << endl;

          } else {
            // try in next loop
            tmpTubesToCheck.insert(actualTubeId);
            if (fVerbose > 3)
              cout << "try " << actualTubeId << " in next loop" << endl;
          }

        } else if (fVerbose > 3)
          cout << "found no straight neighbors" << endl;
      }
    }

    tubesToCheck.clear();
    tubesToCheck = tmpTubesToCheck;

    if (fVerbose > 3) {
      cout << "new fTangentAngles: " << endl;
      PrintTangentAngles();
    }
  }

  // If the isochrone is very small --> track passes in the middle of the tube.
  // Set fTangentAngles to -1.
  for (vector<FairHit *>::iterator it = fHits.begin(); it != fHits.end(); ++it) {
    PndSttHit *actualHit = (PndSttHit *)(*it);

    // no tangent angles found and isochrone is smaller than fMinIsochrone?
    if (!fSttGeoH->IsSkewedStraw(actualHit->GetTubeID()) && !HasUnambiguousAngle(actualHit->GetTubeID()) && actualHit->GetIsochrone() < fMinIsochrone) {
    //if (!fStrawMap->IsSkewedStraw(actualHit->GetTubeID()) && !HasUnambiguousAngle(actualHit->GetTubeID()) && actualHit->GetIsochrone() < fMinIsochrone) {
      set<double> tmp;
      tmp.insert(-1.0);
      fTangentAngles[actualHit->GetTubeID()] = tmp;
    }
  }

  if (fVerbose > 2) {
    cout << "Result:" << endl;
    PrintTangentAngles();
  }

  CorrectIsochrones();
}

void PndSttHitCorrector::PrintTangentAngles()
{
  cout << "Calculated tangent angles: " << endl;

  for (map<int, set<double>>::iterator mapIt = fTangentAngles.begin(); mapIt != fTangentAngles.end(); ++mapIt) {
    cout << "angles for tube " << mapIt->first << ": ";
    for (set<double>::iterator setIt = mapIt->second.begin(); setIt != mapIt->second.end(); ++setIt) {
      if (*setIt == -1.0)
        cout << *setIt << ", ";
      else
        cout << TMath::RadToDeg() * (*setIt) << ", ";
    }
    cout << endl;
  }
}

vector<double> PndSttHitCorrector::CalculateTangentAngles(PndSttHit *tube1, PndSttHit *tube2)
{

  if (fVerbose > 4)
    cout << "PndSttHitCorrector::CalculateTangentAngles" << endl;

  Double_t radius1 = tube1->GetIsochrone();
  Double_t radius2 = tube2->GetIsochrone();

  TVector3 origin1, origin2;
  tube1->Position(origin1);
  tube2->Position(origin2);

  TVector3 directionBetweenTubes = origin2 - origin1;

  if (fVerbose > 4) {
    cout << "---- Input ----" << endl;
    cout << "Origin1: ";
    origin1.Print();
    cout << " Origin2: ";
    origin2.Print();
    cout << " Dir: ";
    directionBetweenTubes.Print();
    cout << endl << "Radius1: " << radius1 << " Radius2: " << radius2 << endl;
  }

  Double_t R, r;
  Bool_t tube1Larger = kTRUE;
  if (radius1 > radius2) {
    R = radius1;
    r = radius2;
    tube1Larger = kTRUE;
  } else {
    R = radius2;
    r = radius1;
    tube1Larger = kFALSE;
  }

  Double_t phi0 = directionBetweenTubes.Phi();
  Double_t phi1 = TMath::ASin((R - r) / directionBetweenTubes.Mag());
  Double_t phi2 = TMath::ASin((R + r) / directionBetweenTubes.Mag());

  if (fVerbose > 4) {
    cout << "Phi0: " << TMath::RadToDeg() * phi0 << " Phi1: " << TMath::RadToDeg() * phi1 << " Phi2: " << TMath::RadToDeg() * phi2 << endl << endl;
  }

  vector<double> results;
  if (tube1Larger) {
    Double_t angle1 = phi0 + phi1 - TMath::Pi() / 2;
    if (angle1 < 0)
      angle1 += TMath::Pi() * 2;
    results.push_back(angle1);

    Double_t angle2 = phi0 - phi1 + TMath::Pi() / 2;
    if (angle2 < 0)
      angle2 += TMath::Pi() * 2;
    results.push_back(angle2);

    Double_t angle3 = phi0 + phi2 - TMath::Pi() / 2;
    if (angle3 < 0)
      angle3 += TMath::Pi() * 2;
    results.push_back(angle3);

    Double_t angle4 = phi0 - phi2 + TMath::Pi() / 2;
    if (angle4 < 0)
      angle4 += TMath::Pi() * 2;
    results.push_back(angle4);

    if (fVerbose > 4) {
      cout << "---- Output ----" << endl;
      cout << "Phi0 + Phi1 - Pi/2 = " << TMath::RadToDeg() * angle1 << endl;
      cout << "Phi0 - Phi1 + Pi/2 = " << TMath::RadToDeg() * angle2 << endl;
      cout << "Phi0 + Phi2 - Pi/2 = " << TMath::RadToDeg() * angle3 << endl;
      cout << "Phi0 - Phi2 + Pi/2 = " << TMath::RadToDeg() * angle4 << endl << endl;
    }
  } else {
    Double_t angle1 = phi0 + phi1 + TMath::Pi() / 2;
    if (angle1 < 0)
      angle1 += TMath::Pi() * 2;
    results.push_back(angle1);

    Double_t angle2 = phi0 - phi1 - TMath::Pi() / 2;
    if (angle2 < 0)
      angle2 += TMath::Pi() * 2;
    results.push_back(angle2);

    Double_t angle3 = phi0 + phi2 - TMath::Pi() / 2;
    if (angle3 < 0)
      angle3 += TMath::Pi() * 2;
    results.push_back(angle3);

    Double_t angle4 = phi0 - phi2 + TMath::Pi() / 2;
    if (angle4 < 0)
      angle4 += TMath::Pi() * 2;
    results.push_back(angle4);

    if (fVerbose > 4) {
      cout << "---- Output ----" << endl;
      cout << "Phi0 + Phi1 + Pi/2 = " << TMath::RadToDeg() * angle1 << endl;
      cout << "Phi0 - Phi1 - Pi/2 = " << TMath::RadToDeg() * angle2 << endl;
      cout << "Phi0 + Phi2 - Pi/2 = " << TMath::RadToDeg() * angle3 << endl;
      cout << "Phi0 - Phi2 + Pi/2 = " << TMath::RadToDeg() * angle4 << endl << endl;
    }
  }

  return results;
}

int PndSttHitCorrector::GetMiddleHitNeighbor(int tubeID)
{
  if (fVerbose > 4)
    cout << "PndSttHitCorrctor::GetMiddleHitNeighbor(" << tubeID << ")" << endl;

  int middleTubeID = 0;

  if (fHitNeighbors[tubeID].size() == 3) {
    double phi0 = fSttGeoH->GetAngleBetweenTubes(tubeID, fHitNeighbors[tubeID].at(0));
    double phi1 = fSttGeoH->GetAngleBetweenTubes(tubeID, fHitNeighbors[tubeID].at(1));
    double phi2 = fSttGeoH->GetAngleBetweenTubes(tubeID, fHitNeighbors[tubeID].at(2));
    //double phi0 = fGeometryMap->GetAngleBetweenTubes(tubeID, fHitNeighbors[tubeID].at(0));
    //double phi1 = fGeometryMap->GetAngleBetweenTubes(tubeID, fHitNeighbors[tubeID].at(1));
    //double phi2 = fGeometryMap->GetAngleBetweenTubes(tubeID, fHitNeighbors[tubeID].at(2));

    // Calculate the relative angles between the neighboring tubes
    double diff01 = GetDiffBetweenAngles(phi0, phi1);
    double diff12 = GetDiffBetweenAngles(phi1, phi2);
    double diff02 = GetDiffBetweenAngles(phi0, phi2);

    // consider rounding errors
    double delta = pow(10, -4);

    // two angles must be 60 degree
    if ((diff01 - (TMath::Pi() / 3)) < delta && (diff12 - (TMath::Pi() / 3)) < delta)
      middleTubeID = fHitNeighbors[tubeID].at(1);
    else if ((diff01 - (TMath::Pi() / 3)) < delta && (diff02 - (TMath::Pi() / 3)) < delta)
      middleTubeID = fHitNeighbors[tubeID].at(0);
    else if ((diff12 - (TMath::Pi() / 3)) < delta && (diff02 - (TMath::Pi() / 3)) < delta)
      middleTubeID = fHitNeighbors[tubeID].at(2);

    if (fVerbose > 4) {
      cout << "diff01: " << diff01 * TMath::RadToDeg() << ", diff12: " << diff12 * TMath::RadToDeg() << ", diff02: " << diff02 * TMath::RadToDeg() << endl;
      cout << "middle-tube: " << middleTubeID << endl;
    }
  }

  return middleTubeID;
}

pair<int, int> PndSttHitCorrector::GetStraightNeighbors(int tubeID)
{

  if (fVerbose > 4)
    cout << "PndSttHitCorrector::GetStraightNeighbors(" << tubeID << ")" << endl;

  pair<int, int> straightNeighbors;
  straightNeighbors.first = 0;
  straightNeighbors.second = 0;

  for (size_t i = 0; i < fHitNeighbors[tubeID].size() - 1; ++i) {

    for (size_t j = i + 1; j < fHitNeighbors[tubeID].size(); ++j) {

      if (fSttGeoH->InStraightLine(tubeID, fHitNeighbors[tubeID].at(i), fHitNeighbors[tubeID].at(j))) {
      //if (fGeometryMap->InStraightLine(tubeID, fHitNeighbors[tubeID].at(i), fHitNeighbors[tubeID].at(j))) {
        straightNeighbors.first = fHitNeighbors[tubeID].at(i);
        straightNeighbors.second = fHitNeighbors[tubeID].at(j);

        if (fVerbose > 4)
          cout << "Found straight neighbors: " << straightNeighbors.first << ", " << straightNeighbors.second << endl;
        break;
      }
    }
  }

  return straightNeighbors;
}

set<double> PndSttHitCorrector::GetBestCombinatedPhi(const vector<vector<double>> &angles)
{

  if (fVerbose > 3)
    cout << "PndSttHitCorrector::GetBestCombinatedPhi()" << endl;

  set<double> ret;

  // Calculate differences and corresponding classification for angles
  vector<vector<vector<double>>> differences;
  vector<vector<double>> smallestDiff;
  vector<vector<pair<int, int>>> smallestPairs;

  CalcDifferencesBetweenAngles(angles, differences, smallestDiff, smallestPairs);

  vector<vector<double>> classification;
  classification = CalcClassification(smallestDiff);

  // get the two best classification-values
  int biggestIndex, secondBiggestIndex;
  double biggest = 0, secondBiggest = 0;

  vector<double> tmp = classification.at(0);

  for (size_t j = 0; j < tmp.size(); ++j) {
    if (tmp[j] > biggest) {
      secondBiggest = biggest;
      secondBiggestIndex = biggestIndex;

      biggestIndex = j;
      biggest = tmp[j];

    } else if (tmp[j] > secondBiggest) {
      secondBiggest = tmp[j];
      secondBiggestIndex = j;
    }
  }

  // get corresponding angles
  pair<int, int> bestPair = smallestPairs.at(0)[biggestIndex];
  double firstAngle = angles[0][bestPair.first];
  double secondAngle = angles[1][bestPair.second];

  if (fVerbose > 3)
    cout << "Best pair of angles: " << firstAngle * TMath::RadToDeg() << ", " << secondAngle * TMath::RadToDeg() << endl;

  if (abs(firstAngle - secondAngle) < fMaxDiffBetweenAngles) {

    double bestPhi = (firstAngle + secondAngle) / 2;

    ret.insert(bestPhi);

    if (fVerbose > 3) {
      cout << "Two best values of classification: " << biggest << ", " << secondBiggest << endl;
      cout << "Best Phi: " << TMath::RadToDeg() * bestPhi << endl;
    }

    // check similarity of the two best (classification-)values
    if (secondBiggest / biggest > 0.90) {
      // found 2 appropriate angles
      pair<int, int> secondBestPair = smallestPairs.at(0)[secondBiggestIndex];
      double secondBestPhi = (angles[0][secondBestPair.first] + angles[1][secondBestPair.second]) / 2;
      ret.insert(secondBestPhi);

      if (fVerbose > 3)
        cout << "Second best Phi: " << TMath::RadToDeg() * secondBestPhi << endl;
    }
  }

  return ret;
}

void PndSttHitCorrector::CalcDifferencesBetweenAngles(const vector<vector<double>> &angles, vector<vector<vector<double>>> &retDifferences, vector<vector<double>> &retSmallestDiff,
                                                      vector<vector<pair<int, int>>> &retPairsOfSmallest)
{

  if (fVerbose > 4) {
    cout << "PndSttHitCorrector::CalcDifferencesBetweenAngles()" << endl;

    cout << "Calculate differences between:";
    for (vector<vector<double>>::const_iterator iterFirst = angles.begin(); iterFirst != angles.end(); ++iterFirst) {
      vector<double> tmp = *iterFirst;
      cout << "(";
      for (size_t i = 0; i < tmp.size(); ++i) {
        cout << TMath::RadToDeg() * tmp[i] << ",";
      }
      cout << ") ";
    }
    cout << endl;
  }
  // for each neighbor X
  for (vector<vector<double>>::const_iterator iterFirst = angles.begin(); iterFirst != angles.end() - 1; ++iterFirst) {

    // calculate distances between angles of X and angles of all the other neighbors Y
    for (vector<vector<double>>::const_iterator iterSecond = iterFirst + 1; iterSecond != angles.end(); ++iterSecond) {

      // create a vector for the pair (X,Y) that will be filled with differences of the angles
      vector<vector<double>> tmp2D;
      retDifferences.push_back(tmp2D);

      // create vector for smallest differences and pairs
      vector<double> tmp1D;
      retSmallestDiff.push_back(tmp1D);
      vector<pair<int, int>> tmpPair;
      retPairsOfSmallest.push_back(tmpPair);

      vector<double> first = *iterFirst;
      vector<double> second = *iterSecond;

      double currentDiff;
      // for all angles k of X
      for (size_t k = 0; k < first.size(); k++) {

        // create a vector for differences between k and all angles of Y
        vector<double> tmp;
        retDifferences.back().push_back(tmp);

        Double_t smallestDifference = 1000;
        pair<int, int> indexOfSmallest;

        // calculate distances between k and all angles j of Y
        for (size_t j = 0; j < second.size(); j++) {

          if (fVerbose > 4) {
            cout << "Phi 0: " << k << " : " << TMath::RadToDeg() * first[k] << " Phi 1: " << j << " : " << TMath::RadToDeg() * second[j] << endl;
          }

          // save difference
          currentDiff = GetDiffBetweenAngles(first[k], second[j]);
          retDifferences.back().at(k).push_back(currentDiff);

          // get smallest difference
          if (currentDiff < smallestDifference) {

            smallestDifference = currentDiff;
            indexOfSmallest = make_pair(k, j);
          }
        }

        // save smallest difference and associated indizes
        retSmallestDiff.back().push_back(smallestDifference);
        retPairsOfSmallest.back().push_back(indexOfSmallest);
      }
    }
  }

  if (fVerbose > 4) {

    cout << "Result of Calculation:" << endl;
    int first = 0, second = 1;
    for (size_t i = 0; i < retDifferences.size(); ++i) {

      cout << "Smallest Difference for vectors (" << first << ", " << second << "): " << endl;

      for (size_t k = 0; k < retSmallestDiff.at(i).size(); ++k) {
        cout << TMath::RadToDeg() * retSmallestDiff.at(i)[k] << " for Phi [" << retPairsOfSmallest.at(i)[k].first << ", " << retPairsOfSmallest.at(i)[k].second << "]" << endl;
      }

      if (second == (int)retDifferences.size() - 1) {
        ++first;
        second = first + 1;
      } else {
        ++second;
      }
    }

    cout << endl << endl;
  }
}

vector<vector<double>> PndSttHitCorrector::CalcClassification(const vector<vector<double>> &smallestDiff)
{

  if (fVerbose > 4)
    cout << "PndSttHitCorrector::CalcClassification(smallestDiff)" << endl;

  vector<vector<double>> classification;
  vector<double> sumOfSmallest;

  for (size_t i = 0; i < smallestDiff.size(); ++i) {
    sumOfSmallest.push_back(0);

    // calculate sum of smallest differences
    for (size_t j = 0; j < smallestDiff[i].size(); ++j) {
      sumOfSmallest[i] += smallestDiff[i][j];
    }

    vector<double> tmp;
    classification.push_back(tmp);

    // calculate classification
    for (size_t j = 0; j < smallestDiff[i].size(); ++j) {
      if (sumOfSmallest[i] != 0) {
        classification.back().push_back(1 - smallestDiff[i][j] / sumOfSmallest[i]);
      } else {
        classification.back().push_back(1);
      }
    }
  }

  if (fVerbose > 4) {
    cout << "Calculated classification: " << endl;

    for (size_t i = 0; i < classification.size(); ++i) {
      cout << "Data for " << i << ". vector of smallest differences: ";
      for (size_t j = 0; j < classification[i].size(); ++j) {
        cout << classification[i][j] << ", ";
      }
      cout << endl;
    }

    cout << endl << endl;
  }

  return classification;
}

vector<vector<double>> PndSttHitCorrector::CalcClassification(const vector<vector<vector<double>>> &differences)
{

  vector<vector<double>> smallestValues;
  vector<vector<pair<int, int>>> pairsOfSmallestValues;
  vector<vector<double>> classification;

  if (fVerbose > 4)
    cout << "PndSttHitCorrector::CalcClassification: classification" << endl;

  vector<double> sumOfSmallestValues;

  int out = 0;

  for (vector<vector<vector<double>>>::const_iterator outerIter = differences.begin(); outerIter != differences.end(); outerIter++) {
    sumOfSmallestValues.push_back(0);
    vector<double> tmp;
    smallestValues.push_back(tmp);
    classification.push_back(tmp);

    vector<pair<int, int>> tmp2;
    pairsOfSmallestValues.push_back(tmp2);

    for (size_t i = 0; i < outerIter->size(); i++) {
      double smallestValue = 10000;
      pair<int, int> pairOfSmallest;

      for (size_t j = 0; j < outerIter->at(i).size(); j++) {
        if (fVerbose > 4)
          cout << "Pairs: " << i << "/" << j << " : " << outerIter->at(i)[j] << endl;
        if (outerIter->at(i)[j] < smallestValue) {
          smallestValue = outerIter->at(i)[j];
          pairOfSmallest = make_pair(i, j);
        }
      }

      cout << endl;

      smallestValues[out].push_back(smallestValue);
      pairsOfSmallestValues[out].push_back(pairOfSmallest);
      sumOfSmallestValues[out] += smallestValue;
    }
    for (size_t i = 0; i < smallestValues[out].size(); i++) {
      if (sumOfSmallestValues[out] != 0) {
        classification[out].push_back(1 - smallestValues[out][i] / sumOfSmallestValues[out]);
      } else {
        classification[out].push_back(1);
      }
      if (fVerbose > 4)
        cout << "Pair: " << pairsOfSmallestValues[out][i].first << "/" << pairsOfSmallestValues[out][i].second << " Value: " << TMath::RadToDeg() * smallestValues[out][i]
             << " Classification: " << classification[out][i] << endl;
    }
    out++;
  }
  return classification;
}

void PndSttHitCorrector::CorrectIsochrones()
{
  if (fVerbose > 2)
    cout << "void PndSttHitCorrector::CorrectIsochrones()" << endl;
  int actualTubeId;
  double bestPhi;

  for (map<int, set<double>>::iterator it = fTangentAngles.begin(); it != fTangentAngles.end(); ++it) {

    if (it->second.size() == 1) {
      actualTubeId = it->first;
      bestPhi = *it->second.begin();

      PndSttHit *actualHit = (PndSttHit *)fHits[fMapTubeIdToHit[actualTubeId]];
      TVector3 origin(actualHit->GetX(), actualHit->GetY(), actualHit->GetZ());
      TVector3 hitError(0.01, 0.01, 100);

      if (bestPhi != -1) {

        TVector3 correctedPosition(origin.x() + actualHit->GetIsochrone() * TMath::Cos(bestPhi), origin.y() + actualHit->GetIsochrone() * TMath::Sin(bestPhi), origin.z());

        if (fVerbose > 4) {
          cout << "Corrected Position for " << actualTubeId << ": ";
          correctedPosition.Print();
          cout << endl;
        }
        fCorrectedIsochrones[fMapTubeIdToHit[actualTubeId]] = new FairHit(-1, correctedPosition, hitError, -1);
      } else {
        // small isochrone --> take midpoint
        fCorrectedIsochrones[fMapTubeIdToHit[actualTubeId]] = new FairHit(-1, origin, hitError, -1);
      }

      fCorrectedIsochrones[fMapTubeIdToHit[actualTubeId]]->SetLink(FairLink("STTHit", fMapTubeIdToHit[actualTubeId]));
    } else {
      if (fVerbose > 2)
        cout << "[warning: ambiguous tangent angles for tube " << it->first << "]" << endl;
    }
  }
  int indexCounter = 0;
  for (map<int, FairHit *>::iterator it = fCorrectedIsochrones.begin(); it != fCorrectedIsochrones.end(); ++it) {
    it->second->SetEntryNr(FairLink("CorrectedIsochrones", indexCounter++));
  }
}

void PndSttHitCorrector::KeepBestAngle(int tubeID, double angle)
{

  if (fVerbose > 3)
    cout << "void PndSttHitCorrector::KeepBestAngle" << endl;

  double keep;

  // tangent angles were already calculated for this tube?
  if ((fTangentAngles.find(tubeID) != fTangentAngles.end()) && (fTangentAngles.size() != 1)) {
    //--> keep the angle that is most similar to the angle of the unambiguous neighbor
    keep = GetBestFittingAngle(angle, fTangentAngles[tubeID]);

  } else {
    // no tangent angle calculated
    //--> store the angle of the unambiguous neighbor
    keep = angle;
  }

  fTangentAngles[tubeID].clear();
  fTangentAngles[tubeID].insert(keep);

  if (fVerbose > 3)
    cout << "Result for tube: " << tubeID << ", keep " << keep * TMath::RadToDeg() << endl;
}

double PndSttHitCorrector::GetDiffBetweenAngles(double angle1, double angle2)
{
  double diff = abs(angle1 - angle2);
  if (diff > TMath::Pi())
    return 2 * TMath::Pi() - diff;
  else
    return diff;
}

double PndSttHitCorrector::GetAverageOfAngles(double angle1, double angle2)
{

  double diff = abs(angle1 - angle2);

  // return the middle of the smaller angle between both angles
  if (diff > TMath::Pi()) {
    double tmp = max(angle1, angle2) + (2 * TMath::Pi() - diff) / 2;
    if (tmp >= 2 * TMath::Pi())
      tmp -= 2 * TMath::Pi();
    return tmp;
  } else
    return (angle1 + angle2) / 2;
}

double PndSttHitCorrector::GetBestFittingAngle(double angle, set<double> anglesToTest)
{

  double minDiff = 1000;
  double tmpDiff;
  double best = -1;

  for (set<double>::iterator it = anglesToTest.begin(); it != anglesToTest.end(); ++it) {
    tmpDiff = GetDiffBetweenAngles(angle, *it);
    if (tmpDiff < minDiff) {
      minDiff = tmpDiff;
      best = *it;
    }
  }

  return best;
}

bool PndSttHitCorrector::AreHitNeihbors(int tubeID1, int tubeID2)
{

  // if both signal a hit
  if (fHitNeighbors.find(tubeID1) == fHitNeighbors.end() || fHitNeighbors.find(tubeID2) == fHitNeighbors.end()) {
    return false;
  }

  for (size_t i = 0; i < fHitNeighbors[tubeID1].size(); ++i) {
    if (fHitNeighbors[tubeID1].at(i) == tubeID2) {
      return true;
    }
  }

  return false;
}

bool PndSttHitCorrector::HasUnambiguousAngle(int tubeID)
{

  if (fTangentAngles.find(tubeID) != fTangentAngles.end() && fTangentAngles[tubeID].size() == 1)
    return true;
  else
    return false;
}
