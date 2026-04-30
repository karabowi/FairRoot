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
#include "PndApolloniusTriplet.h"
//#include "PndSttGeometryMap.h"
#include "PndStt2GeoHandler.h"
#include "FairHit.h"
#include "PndSttHit.h"
#include "FairLogger.h"
#include "PndHoughApollonius.h"
#include "PndCircleTools.h"
#include "PndTrackEvaluatorDetStt.h"
// general
using std::cout;
using std::endl;

void PndApollonius::TripletSolution::IsClockwise()
{
  std::vector<FairHit *> sttHits = fHits[STT];
  if (sttHits.size() > 2) {

    fClockwise = true;
    std::sort(sttHits.begin(), sttHits.end(), [](FairHit *first, FairHit *second) { return (((PndSttHit *)first)->GetTubeID() < ((PndSttHit *)second)->GetTubeID()); });
    TVector2 firstHit(HitOnTrack(sttHits.front()));
    TVector2 secondHit(HitOnTrack(sttHits.back()));
    if (firstHit.DeltaPhi(secondHit) < 0) {
      fClockwise = false;
    }
  }
}

void PndApollonius::TripletSolution::SortHits(TVector2 &firstHit, detID detector)
{
  if (fClockwise) {
    std::sort(fHits[detector].begin(), fHits[detector].end(), [&](FairHit *a, FairHit *b) {
      double phi_a = HitOnTrack(a).DeltaPhi(firstHit);
      double phi_b = HitOnTrack(b).DeltaPhi(firstHit);
      if (phi_a <= 0)
        phi_a += 2 * TMath::Pi();
      if (phi_b <= 0)
        phi_b += 2 * TMath::Pi();

      return (phi_a > phi_b);
    });
  } else {
    std::sort(fHits[detector].begin(), fHits[detector].end(), [&](FairHit *a, FairHit *b) {
      double phi_a = HitOnTrack(a).DeltaPhi(firstHit);
      double phi_b = HitOnTrack(b).DeltaPhi(firstHit);
      if (phi_a < 0)
        phi_a += 2 * TMath::Pi();
      if (phi_b < 0)
        phi_b += 2 * TMath::Pi();
      return (phi_a < phi_b);
    });
  }
}

void PndApollonius::TripletSolution::SortStt(PndStt2GeoHandler *fGeometryMap)
//void PndApollonius::TripletSolution::SortStt(PndSttGeometryMap *fGeometryMap)
{
  std::vector<FairHit *> sttHits = fHits[STT];
  if (sttHits.size() > 2) {

    IsClockwise();
    // std::cout << "fClockwise: " << fClockwise << std::endl;
    std::sort(sttHits.begin(), sttHits.end(), [](FairHit *first, FairHit *second) { return (((PndSttHit *)first)->GetTubeID() < ((PndSttHit *)second)->GetTubeID()); });
    // std::cout << "Sort by tube id done "<< std::endl;
    std::vector<FairHit *> possibleFirstHits;
    int minRow = 100;
    for (auto hit : sttHits) {
      if (fGeometryMap->GetRow(static_cast<PndSttHit *>(hit)->GetTubeID()) <= minRow) {
        minRow = fGeometryMap->GetRow(static_cast<PndSttHit *>(hit)->GetTubeID());
        possibleFirstHits.push_back(hit);
      } else
        break;
    }
    TVector2 firstHit;
    if (fClockwise) {
      firstHit = HitOnTrack(possibleFirstHits.back());
    } else {
      firstHit = HitOnTrack(possibleFirstHits.front());
    }
    SortHits(firstHit, STT);
  }
}

FairHit *PndApollonius::TripletSolution::GetFirstHit(PndStt2GeoHandler *fGeometryMap)
//FairHit *PndApollonius::TripletSolution::GetFirstHit(PndSttGeometryMap *fGeometryMap)
{
  std::vector<FairHit *> mvdPixelHits = fHits[MVDpixel];
  std::vector<FairHit *> mvdStripHits = fHits[MVDstrip];
  std::vector<FairHit *> sttHits = fHits[STT];
  std::vector<FairHit *> gemHits = fHits[GEM];

  if (mvdPixelHits.size() > 0) {
    double dSquare = 100000.;
    FairHit *first = nullptr;
    for (auto hit : mvdPixelHits) {
      if (((hit->GetX() * hit->GetX()) + (hit->GetY() * hit->GetY() + (hit->GetZ() * hit->GetZ()))) < dSquare) {
        dSquare = ((hit->GetX() * hit->GetX()) + (hit->GetY() * hit->GetY()) + (hit->GetZ() * hit->GetZ()));
        first = hit;
      }
    }
    return first;
  } else if (mvdStripHits.size() > 0) {
    double dSquare = 100000.;
    FairHit *first = nullptr;
    for (auto hit : mvdStripHits) {
      if (((hit->GetX() * hit->GetX()) + (hit->GetY() * hit->GetY() + (hit->GetZ() * hit->GetZ()))) < dSquare) {
        dSquare = ((hit->GetX() * hit->GetX()) + (hit->GetY() * hit->GetY()) + (hit->GetZ() * hit->GetZ()));
        first = hit;
      }
    }
    return first;

  } else if (sttHits.size() > 0) {
    return sttHits.front();
  } else if (gemHits.size() > 0) {
    double dSquare = 100000.;
    FairHit *first = nullptr;
    for (auto hit : gemHits) {
      if (((hit->GetX() * hit->GetX()) + (hit->GetY() * hit->GetY())) < dSquare) {
        dSquare = ((hit->GetX() * hit->GetX()) + (hit->GetY() * hit->GetY()));
        first = hit;
      }
    }
    return first;
  } else
    return nullptr;
}

void PndApollonius::TripletSolution::SortAllHits(PndStt2GeoHandler *fGeometryMap)
//void PndApollonius::TripletSolution::SortAllHits(PndSttGeometryMap *fGeometryMap)
{
  SortStt(fGeometryMap);
  std::vector<FairHit *> sttHits = fHits[STT];
  fAllHits.clear();
  if (sttHits.size() > 2) {
    FairHit *first = GetFirstHit(fGeometryMap);

    TVector2 firstHit(HitOnTrack(first));
    TVector2 secondHit(HitOnTrack(sttHits.back()));
    bool hasChanged = false;
    if (firstHit.DeltaPhi(secondHit) < 0 && fClockwise) {
      hasChanged = true;
      fClockwise = false;
    } else if (firstHit.DeltaPhi(secondHit) >= 0 && !fClockwise) {
      hasChanged = true;
      fClockwise = true;
    }

    if (hasChanged) {
      SortHits(firstHit, MVDpixel);
      SortHits(firstHit, MVDstrip);
      SortHits(firstHit, STT);
      SortHits(firstHit, GEM);

      FairHit *PreviousHit = first;
      FairHit PriviousNonSkewedHit;
      for (auto hitIter = fHits[MVDpixel].begin(); hitIter != fHits[MVDpixel].end(); hitIter++) {
        if (IsReasonableHit(*hitIter, PreviousHit)) {
          fAllHits.insert(fAllHits.end(), *hitIter);
          PreviousHit = *hitIter;
        }
      }
      for (auto hitIter = fHits[MVDstrip].begin(); hitIter != fHits[MVDstrip].end(); hitIter++) {
        if (IsReasonableHit(*hitIter, PreviousHit)) {
          fAllHits.insert(fAllHits.end(), *hitIter);
          PreviousHit = *hitIter;
        }
      }
      for (auto hitIter = fHits[STT].begin(); hitIter != fHits[STT].end(); hitIter++) {
        if (IsReasonableHit(*hitIter, PreviousHit)) {
          fAllHits.insert(fAllHits.end(), *hitIter);
          PreviousHit = *hitIter;
          if (!fGeometryMap->IsSkewedStraw(((PndSttHit *)(*hitIter))->GetTubeID())) {
            PriviousNonSkewedHit = **hitIter;
          }
        }
      }
      PreviousHit = &PriviousNonSkewedHit;
      for (auto hitIter = fHits[GEM].begin(); hitIter != fHits[GEM].end(); hitIter++) {
        if (IsReasonableHit(*hitIter, PreviousHit)) {
          fAllHits.insert(fAllHits.end(), *hitIter);
          PreviousHit = *hitIter;
        }
      }
    } else {
      SortHits(firstHit, MVDpixel);
      SortHits(firstHit, MVDstrip);
      FairHit *PreviousHit = first;
      FairHit PriviousNonSkewedHit;
      for (auto hitIter = fHits[MVDpixel].begin(); hitIter != fHits[MVDpixel].end(); hitIter++) {
        if (IsReasonableHit(*hitIter, PreviousHit)) {
          fAllHits.insert(fAllHits.end(), *hitIter);
          PreviousHit = *hitIter;
        }
      }
      for (auto hitIter = fHits[MVDstrip].begin(); hitIter != fHits[MVDstrip].end(); hitIter++) {
        if (IsReasonableHit(*hitIter, PreviousHit)) {
          fAllHits.insert(fAllHits.end(), *hitIter);
          PreviousHit = *hitIter;
        }
      }
      for (auto hitIter = fHits[STT].begin(); hitIter != fHits[STT].end(); hitIter++) {
        if (IsReasonableHit(*hitIter, PreviousHit)) {
          fAllHits.insert(fAllHits.end(), *hitIter);
          PreviousHit = *hitIter;
          if (!fGeometryMap->IsSkewedStraw(((PndSttHit *)(*hitIter))->GetTubeID())) {
            PriviousNonSkewedHit = **hitIter;
          }
        }
      }
      SortHits(firstHit, GEM);
      PreviousHit = &PriviousNonSkewedHit;
      for (auto hitIter = fHits[GEM].begin(); hitIter != fHits[GEM].end(); hitIter++) {
        if (IsReasonableHit(*hitIter, PreviousHit)) {
          fAllHits.insert(fAllHits.end(), *hitIter);
          PreviousHit = *hitIter;
        }
      }
    }
  }
}

bool PndApollonius::TripletSolution::IsReasonableHit(FairHit *CurrentHit, FairHit *PriviousHit)
{
  TVector2 Hit2(HitOnTrack(CurrentHit));
  TVector2 Hit1(HitOnTrack(PriviousHit));
  if ((Hit1.DeltaPhi(Hit2) >= 0 && fClockwise) || (Hit1.DeltaPhi(Hit2) <= 0 && !fClockwise)) {
    return kTRUE;
  } else
    return kFALSE;
}

std::map<int, std::vector<PndSttHit *>>
PndApollonius::ApolloniusTripletFunctions::GetAllTubesByRow(std::vector<PndSttHit *> hits, PndStt2GeoHandler *fGeometryMap, int &fAllHitsCounter)
//PndApollonius::ApolloniusTripletFunctions::GetAllTubesByRow(std::vector<PndSttHit *> hits, PndSttGeometryMap *fGeometryMap, int &fAllHitsCounter)
{
  std::map<int, std::vector<PndSttHit *>> result;
  fAllHitsCounter = 0;
  for (auto myHit : hits) {
    auto it = std::find_if(result[fGeometryMap->GetRow(myHit->GetTubeID())].begin(), result[fGeometryMap->GetRow(myHit->GetTubeID())].end(),
                           [&myHit](const PndSttHit *hit) { return myHit->GetTubeID() == hit->GetTubeID(); });
    if (it == result[fGeometryMap->GetRow(myHit->GetTubeID())].end()) {
      result[fGeometryMap->GetRow(myHit->GetTubeID())].push_back(myHit);
      fAllHitsCounter += 1;
    } else {
      if ((*it)->GetTimeStamp() > myHit->GetTimeStamp())
        result[fGeometryMap->GetRow(myHit->GetTubeID())][it - result[fGeometryMap->GetRow(myHit->GetTubeID())].begin()] = myHit;
    }
  }
  return result;
}

std::map<int, std::vector<std::vector<PndSttHit *>>>
PndApollonius::ApolloniusTripletFunctions::GetTubeStructure(std::vector<PndSttHit *> hits, PndStt2GeoHandler *fGeometryMap, int &fAllHitsCounter, bool &fIsStrongCurling)
//PndApollonius::ApolloniusTripletFunctions::GetTubeStructure(std::vector<PndSttHit *> hits, PndSttGeometryMap *fGeometryMap, int &fAllHitsCounter, bool &fIsStrongCurling)
{

  // fIsCurling = false;
  fIsStrongCurling = false;

  std::map<int, std::vector<std::vector<PndSttHit *>>> result;
  std::map<int, std::vector<PndSttHit *>> tubesByRow = GetAllTubesByRow(hits, fGeometryMap, fAllHitsCounter);

  int curlingRows = 0;

  for (auto tubes : tubesByRow) {
    std::sort(tubes.second.begin(), tubes.second.end(), [](PndSttHit *first, PndSttHit *second) { return (first->GetTubeID() < second->GetTubeID()); });
    std::vector<PndSttHit *> group;
    int oldTubeId = -1;
    if (tubes.second.size() > 0)
      oldTubeId = tubes.second[0]->GetTubeID();
    std::vector<std::vector<PndSttHit *>> rowGroups;

    for (auto sttHit : tubes.second) {
      if (TMath::Abs(sttHit->GetTubeID() - oldTubeId) > 1) {
        if (group.size() > 5 && (tubes.first != tubesByRow.rbegin()->first)) { // more than five tubes adjacent to each other in one row but not the last row
          curlingRows++;
        }
        rowGroups.push_back(group);
        group.clear();
      }
      group.push_back(sttHit);
      oldTubeId = sttHit->GetTubeID();
    }
    rowGroups.push_back(group);

    if (group.size() > 5 && (tubes.first != tubesByRow.rbegin()->first)) { // more than five tubes adjacent to each other in one row but not the last row
      // fIsCurling = true;
      curlingRows++;
    }

    result[tubes.first] = (rowGroups); // todo: better condition to identify strong curling tracks needed

    if (result[tubes.first].size() > 5) // more than 5 separated groups in one row
      fIsStrongCurling = true;
  }

  if (curlingRows > 2)
    fIsStrongCurling = true;
  // todo: The condition for a track being strongly curved is not good. Use number of neighbors to discard strongly curved tracks
  // use number of hit tubes ṕer row to determine curved or curling tracks (no second circle in stt)
  // for curved or curling tracks we can try to use one inner tube as last tube or we can merge produced clones
  // std::cout << "curling track fIsCurling: " << fIsCurling << " fIsStrongCurling: " << fIsStrongCurling << " curlingRows: " << curlingRows << std::endl;

  return result;
}

PndApollonius::ReductionMaps PndApollonius::ApolloniusTripletFunctions::CreateMaps(TripletValues &triplets, std::map<FairLink, int> &fMapHitstoCATracklet)
{
  ReductionMaps maps;
  maps.Reset();
  for (auto InnerSttHit : triplets.fFirstRow) {
    auto CAId_it = fMapHitstoCATracklet.find(InnerSttHit->GetEntryNr());

    if (CAId_it != fMapHitstoCATracklet.end()) {
      // Is a ca tracklet
      FairLink CALink = InnerSttHit->GetEntryNr();
      maps.fMapInnerTubeToCA[InnerSttHit] = fMapHitstoCATracklet[CALink];
    }
  }
  for (auto MidSttHit : triplets.fMidRow) {
    auto CAId_it = fMapHitstoCATracklet.find(MidSttHit->GetEntryNr());

    if (CAId_it != fMapHitstoCATracklet.end()) {
      // Is a ca tracklet
      FairLink CALink = MidSttHit->GetEntryNr();
      maps.fMapCAToMidTube[fMapHitstoCATracklet[CALink]] = MidSttHit;
      maps.fMapMidTubeToCA[MidSttHit] = fMapHitstoCATracklet[CALink];
    }
  }
  for (auto OuterSttHit : triplets.fLastRow) {
    auto CAId_it = fMapHitstoCATracklet.find(OuterSttHit->GetEntryNr());

    if (CAId_it != fMapHitstoCATracklet.end()) {
      // Is a ca tracklet
      FairLink CALink = OuterSttHit->GetEntryNr();
      maps.fMapCAToOuterTube[fMapHitstoCATracklet[CALink]] = OuterSttHit;
    }
  }
  return maps;
}

std::vector<PndApollonius::Triplet> PndApollonius::ApolloniusTripletFunctions::ReduceCombinatorics(TripletValues &triplets, std::map<FairLink, int> &fMapHitstoCATracklet,
                                                                                                   TClonesArray *sttHits, PndSttCA *fCATrackFinder, PndStt2GeoHandler *fGeometryMap,
                                                                                                   //TClonesArray *sttHits, PndSttCA *fCATrackFinder, PndSttGeometryMap *fGeometryMap,
                                                                                                   double &fMinDistance, bool &fWithCombiReduction)
{
  std::vector<PndApollonius::Triplet> result;
  if (fWithCombiReduction) {
    ReductionMaps maps = CreateMaps(triplets, fMapHitstoCATracklet);
    std::vector<int> RemovedTubeIdsLast;
    RemovedTubeIdsLast.clear();
    std::vector<int> RemovedTubeIdsMid;
    RemovedTubeIdsMid.clear();
    for (PndSttHit *iTube : triplets.fFirstRow) {
      LOG(debug) << "FirstRow: ";
      std::for_each(triplets.fFirstRow.begin(), triplets.fFirstRow.end(), [](PndSttHit *hit) { LOG(debug) << hit->GetEntryNr() << "/"; });
      LOG(debug) << "iTube: " << iTube->GetEntryNr() << " has tubeId: " << iTube->GetTubeID() << " (" << iTube->GetX() << "," << iTube->GetY() << ")";
      LOG(debug) << "first hit " << *iTube;

      if (maps.fMapCAToMidTube.find(maps.fMapInnerTubeToCA[iTube]) == maps.fMapCAToMidTube.end() || fMapHitstoCATracklet.find(iTube->GetEntryNr()) == fMapHitstoCATracklet.end()) {
        // inner tube has no connection to mid tube
        for (auto mTube : triplets.fMidRow) {

          LOG(debug) << "MidRow: ";
          std::for_each(triplets.fMidRow.begin(), triplets.fMidRow.end(), [](PndSttHit *hit) { LOG(debug) << hit->GetEntryNr() << "/"; });
          LOG(debug) << "mTube: " << mTube->GetEntryNr() << " has tubeId: " << mTube->GetTubeID() << " (" << mTube->GetX() << "," << mTube->GetY() << ")";
          LOG(debug) << "mid hit " << *mTube;
          if (iTube->GetTubeID() == mTube->GetTubeID() || std::find(RemovedTubeIdsMid.begin(), RemovedTubeIdsMid.end(), mTube->GetTubeID()) !=
                                                            RemovedTubeIdsMid.end()) //||
                                                                                     // fGeometryMap->GetRow(iTube->GetTubeID()) >= fGeometryMap->GetRow(mTube->GetTubeID()))
            continue;
          LOG(debug) << "mid hit " << *mTube;

          if (maps.fMapCAToOuterTube.find(maps.fMapMidTubeToCA[mTube]) == maps.fMapCAToOuterTube.end() ||
              fMapHitstoCATracklet.find(mTube->GetEntryNr()) == fMapHitstoCATracklet.end()) {
            // mid tube has no connection to outer tube

            for (auto oTube : triplets.fLastRow) {
              LOG(debug) << "LastRow: ";
              std::for_each(triplets.fLastRow.begin(), triplets.fLastRow.end(), [](PndSttHit *hit) { LOG(debug) << hit->GetEntryNr() << "/"; });
              LOG(debug) << "oTube: " << oTube->GetEntryNr() << " has tubeId: " << oTube->GetTubeID() << " (" << oTube->GetX() << "," << oTube->GetY() << ")";
              LOG(debug) << "last hit " << *oTube;
              if (mTube->GetTubeID() == oTube->GetTubeID() || iTube->GetTubeID() == oTube->GetTubeID() ||
                  std::find(RemovedTubeIdsLast.begin(), RemovedTubeIdsLast.end(), oTube->GetTubeID()) != RemovedTubeIdsLast.end())
                continue;
              LOG(debug) << "last hit " << *oTube;
              if (sqrt((iTube->GetX() - oTube->GetX()) * (iTube->GetX() - oTube->GetX()) + (iTube->GetY() - oTube->GetY()) * (iTube->GetY() - oTube->GetY())) < fMinDistance)
                continue;
              Triplet t{iTube, mTube, oTube};
              result.push_back(t);
            }
          } else {
            // mid tube has a connection to outer tube
            LOG(debug) << "mid tube has connection to outer tube";
            PndSttHit *connectedOuterTube = maps.fMapCAToOuterTube[maps.fMapMidTubeToCA[mTube]];
            LOG(debug) << "connected outer tube:" << *connectedOuterTube;
            if (mTube->GetTubeID() == connectedOuterTube->GetTubeID() || iTube->GetTubeID() == connectedOuterTube->GetTubeID()) {
              PndTrackCand trackCand_temp = fCATrackFinder->GetFirstTrackCand(maps.fMapMidTubeToCA[mTube]);
              // search for the first hit that is not the same hit
              for (int i = trackCand_temp.GetNHits() - 1; i >= trackCand_temp.GetNHits() / 2; i--) {
                FairLink OuterLink = trackCand_temp.GetSortedHit(i);
                LOG(debug) << "since hits are equal test OuterLink:" << OuterLink;
                LOG(debug) << "sttHits: " << sttHits->GetEntriesFast() << " entry: " << OuterLink.GetIndex();
                if (sttHits->GetEntriesFast() <= OuterLink.GetIndex())
                  break;
                PndSttHit *OuterSttHit = (PndSttHit *)sttHits->At(OuterLink.GetIndex());

                if (!fGeometryMap->IsSkewedStraw(OuterSttHit->GetTubeID()) && OuterSttHit->GetTubeID() != mTube->GetTubeID()) {
                  connectedOuterTube = OuterSttHit;
                  break;
                }
              }

              LOG(debug) << *mTube << "," << *connectedOuterTube;
            }
            if (mTube->GetTubeID() == connectedOuterTube->GetTubeID() || iTube->GetTubeID() == connectedOuterTube->GetTubeID()) {
              LOG(debug) << "mTube == connectedOuterTube";
              continue;
            }
            LOG(debug) << "last hit connected" << *connectedOuterTube;

            if (sqrt((iTube->GetX() - connectedOuterTube->GetX()) * (iTube->GetX() - connectedOuterTube->GetX()) +
                     (iTube->GetY() - connectedOuterTube->GetY()) * (iTube->GetY() - connectedOuterTube->GetY())) < fMinDistance)
              continue;

            Triplet t{iTube, mTube, connectedOuterTube};
            result.push_back(t);
            RemovedTubeIdsLast.push_back(connectedOuterTube->GetTubeID());
          }
        }
      } else {
        // inner tube has a connection to mid tube
        LOG(debug) << "inner tube has a connection to mid tube";
        PndSttHit *connectedMidTube = maps.fMapCAToMidTube[maps.fMapInnerTubeToCA[iTube]];
        PndSttHit *connectedMidTubeForCA = maps.fMapCAToMidTube[maps.fMapInnerTubeToCA[iTube]];

        if (iTube->GetTubeID() == connectedMidTube->GetTubeID()) {
          PndTrackCand trackCand_temp = fCATrackFinder->GetFirstTrackCand(maps.fMapInnerTubeToCA[iTube]);
          for (int i = trackCand_temp.GetNHits() / 2; i >= 0; i--) {
            FairLink linkMid = trackCand_temp.GetSortedHit(i);
            if (sttHits->GetEntriesFast() <= linkMid.GetIndex())
              break;

            PndSttHit *MidSttHit = (PndSttHit *)sttHits->At(linkMid.GetIndex());

            if (!fGeometryMap->IsSkewedStraw(MidSttHit->GetTubeID()) && MidSttHit->GetTubeID() != iTube->GetTubeID()) {
              connectedMidTube = MidSttHit;
              break;
            }
          }
        }

        if (iTube->GetTubeID() == connectedMidTube->GetTubeID()) {
          continue;
        }
        LOG(debug) << "mid hit connected " << *connectedMidTube;
        if (maps.fMapCAToOuterTube.find(maps.fMapInnerTubeToCA[iTube]) == maps.fMapCAToOuterTube.end()) {
          // mid tube has no connection to outer tube
          LOG(debug) << "mid tube has no connection to outer tube ";

          for (auto oTube : triplets.fLastRow) {
            LOG(debug) << "outer hit " << *oTube;

            if (oTube->GetTubeID() == connectedMidTube->GetTubeID() || iTube->GetTubeID() == oTube->GetTubeID() ||
                std::find(RemovedTubeIdsLast.begin(), RemovedTubeIdsLast.end(), oTube->GetTubeID()) != RemovedTubeIdsLast.end())
              continue;
            if (sqrt((iTube->GetX() - oTube->GetX()) * (iTube->GetX() - oTube->GetX()) + (iTube->GetY() - oTube->GetY()) * (iTube->GetY() - oTube->GetY())) < fMinDistance)
              continue;

            Triplet t{iTube, connectedMidTube, oTube};
            result.push_back(t);
          }
        } else {
          // mid tube has a connection to outer tube
          PndSttHit *connectedOuterTube = maps.fMapCAToOuterTube[maps.fMapMidTubeToCA[connectedMidTubeForCA]];
          LOG(debug) << "mid tube has a connection to outer tube ";
          LOG(debug) << "outer hit connected " << *connectedOuterTube;

          if (connectedOuterTube->GetTubeID() == connectedMidTube->GetTubeID()) {
            PndTrackCand trackCand_temp = fCATrackFinder->GetFirstTrackCand(maps.fMapMidTubeToCA[connectedMidTubeForCA]);

            for (int i = trackCand_temp.GetNHits() - 1; i >= 0; i--) {
              FairLink OuterLink = trackCand_temp.GetSortedHit(i);
              if (sttHits->GetEntriesFast() <= OuterLink.GetIndex())
                break;

              PndSttHit *OuterSttHit = (PndSttHit *)sttHits->At(OuterLink.GetIndex());
              LOG(debug) << "try outer hit " << *OuterSttHit;
              if (!fGeometryMap->IsSkewedStraw(OuterSttHit->GetTubeID()) && OuterSttHit->GetTubeID() != connectedMidTube->GetTubeID()) {
                connectedOuterTube = OuterSttHit;
                break;
              }
            }
          }
          if (connectedMidTube->GetTubeID() == connectedOuterTube->GetTubeID() || iTube->GetTubeID() == connectedOuterTube->GetTubeID()) {
            continue;
          }
          LOG(debug) << "outer hit connected " << *connectedOuterTube;

          if (sqrt((iTube->GetX() - connectedOuterTube->GetX()) * (iTube->GetX() - connectedOuterTube->GetX()) +
                   (iTube->GetY() - connectedOuterTube->GetY()) * (iTube->GetY() - connectedOuterTube->GetY())) < fMinDistance)
            continue;

          Triplet t{iTube, connectedMidTube, connectedOuterTube};
          result.push_back(t);
          RemovedTubeIdsLast.push_back(connectedOuterTube->GetTubeID());
        }
        RemovedTubeIdsMid.push_back(connectedMidTube->GetTubeID());
      }
    }
  } else {
    for (auto iTube : triplets.fFirstRow) {
      for (auto mTube : triplets.fMidRow) {
        // if (fGeometryMap->GetRow(iTube->GetTubeID()) >= fGeometryMap->GetRow(mTube->GetTubeID()))
        //  continue;
        for (auto oTube : triplets.fLastRow) {
          if (iTube->GetTubeID() == mTube->GetTubeID() || mTube->GetTubeID() == oTube->GetTubeID() || iTube->GetTubeID() == oTube->GetTubeID())
            continue;
          if (sqrt((iTube->GetX() - oTube->GetX()) * (iTube->GetX() - oTube->GetX()) + (iTube->GetY() - oTube->GetY()) * (iTube->GetY() - oTube->GetY())) < fMinDistance)
            continue;
          Triplet t{iTube, mTube, oTube};
          result.push_back(t);
        }
      }
    }
  }

  return result;
}

bool PndApollonius::ApolloniusTripletFunctions::IsTripletUsed(std::vector<PndApollonius::TripletSolution> &solutions, PndApollonius::Triplet &triplet)
{
  auto sol = std::find_if(solutions.begin(), solutions.end(), [&](TripletSolution &sol) {
    for (auto trip : triplet.fTripletHits) {
      auto first = std::find_if(sol.fHits[TripletSolution::detID::STT].begin(), sol.fHits[TripletSolution::detID::STT].end(),
                                [&](FairHit *hit) { return (((PndSttHit *)hit)->GetTubeID() == trip->GetTubeID()); });
      if (first == sol.fHits[TripletSolution::detID::STT].end()) {
        return false;
      }
    }
    return true;
  });
  if (sol != solutions.end())
    return true;
  else
    return false;
}

std::vector<PndApollonius::TripletSolution>
PndApollonius::ApolloniusTripletFunctions::GenerateTripletTracks(PndApollonius::Triplet triplet, std::vector<PndSttHit *> &sttHits, PndStt2GeoHandler *fGeometryMap)
//PndApollonius::ApolloniusTripletFunctions::GenerateTripletTracks(PndApollonius::Triplet triplet, std::vector<PndSttHit *> &sttHits, PndSttGeometryMap *fGeometryMap)
{
  Thresholds threshold;
  std::vector<TripletSolution> result;
  std::vector<double *> tmpPoints(3);
  for (int k = 0; k < triplet.fTripletHits.size(); k++) {

    double *pVar = new double[4];
    pVar[0] = (triplet.fTripletHits[k]->GetX());
    pVar[1] = (triplet.fTripletHits[k]->GetY());
    pVar[2] = (triplet.fTripletHits[k]->GetIsochrone());
    pVar[3] = (triplet.fTripletHits[k]->GetIsochroneError());
    tmpPoints[k] = pVar;
  }
  double *apolloniusCircles = new double[6 * 8];
  // std::cout << "event " <<  FairRootManager::Instance()->GetEntryNr() << " apollonius for hit0 (" << tmpPoints[0][0] << "," << tmpPoints[0][1] << "," << tmpPoints[0][2] << "),
  // hit1 (" << tmpPoints[1][0] << "," << tmpPoints[1][1] <<
  // ","
  //           << tmpPoints[1][2] << "), hit2 (" << tmpPoints[2][0] << "," << tmpPoints[2][1] << "," << tmpPoints[2][2] << ")," << std::endl;

  PndHoughApollonius::ApolloniusCudaCalcCPU(1, tmpPoints[0], tmpPoints[1], tmpPoints[2], apolloniusCircles);

  for (int solutionIndex = 0; solutionIndex < 8; solutionIndex++) { // there are 8 solutions
    // std::cout << apolloniusCircles[(solutionIndex * 6) + 0] << " " << apolloniusCircles[(solutionIndex * 6) + 1] << " " << apolloniusCircles[(solutionIndex * 6) + 2] <<
    // std::endl;
    if (apolloniusCircles[(solutionIndex * 6) + 0] == 0 && apolloniusCircles[(solutionIndex * 6) + 1] == 0 && apolloniusCircles[(solutionIndex * 6) + 2] == 0)
      continue;
    // 1. if radius is not 0 (--> a circle could be calculated) and x is not nan
    // 1. if x, y, r are not 0
    else if ((!isnan(apolloniusCircles[(solutionIndex * 6) + 0]) && apolloniusCircles[(solutionIndex * 6) + 2] != 0) ||
             (apolloniusCircles[(solutionIndex * 6) + 0] != 0 && apolloniusCircles[(solutionIndex * 6) + 1] != 0 && apolloniusCircles[(solutionIndex * 6) + 2] != 0)) {

      TVector3 apollonius{apolloniusCircles[(solutionIndex * 6) + 0], apolloniusCircles[(solutionIndex * 6) + 1], TMath::Abs(apolloniusCircles[(solutionIndex * 6) + 2])};
      // std::cout << "apollonius circle: " << apollonius.X() << "," << apollonius.Y() << "," << apollonius.Z() << std::endl;
      TripletSolution solution = FindHitsCloseToCircle(sttHits, apollonius, threshold.fDistanceThresholdSTTFar, fGeometryMap);
      /*
      std::vector<PndSttHit *> hits = FindHitsCloseToCircle(sttHits, apollonius, threshold.fDistanceThresholdSTTFar, fGeometryMap);

      TripletSolution solution(apollonius);

      solution.AddHits(hits);

      solution.SortStt(fGeometryMap);

      solution.fMeanSquare = MeanSquareDistance(solution);
      */
      solution.fTriplet = triplet;
      LOG(debug) << solution;
      result.push_back(solution);

    } else if (apolloniusCircles[(solutionIndex * 6) + 2] == 0) { //
      // case for streight lines
      // apollooniusCircle is then (m,b,0) with m and b the parameters of the line through the hits
      double m = apolloniusCircles[(solutionIndex * 6) + 0];
      double b = apolloniusCircles[(solutionIndex * 6) + 1];
      // assume max momentum of 5 GeV/c B = 2T --> rMax = 100/(0.3*B)*pT
      double rMax = 100 / (0.3 * 2) * 5; // assume B = 2 and pT = 5 GeV/c
      double xCenter;
      double yCenter;
      // determine center of circle as point on line perpendicular to streight line going through fiirst point of track and has a distance of rMax.
      // check if a line could be calculated or if the line has m = 0 oder m = inf (NaN)
      if (std::isnan(m)) { // m = inf
        xCenter = tmpPoints[0][0] + rMax;
        yCenter = tmpPoints[0][1];
      } else if (m == 0) {
        xCenter = tmpPoints[0][0];
        yCenter = tmpPoints[0][1] + rMax;
      } else {
        double mInverse = -1 / m;
        double bInverse = tmpPoints[0][1] - tmpPoints[0][0] * mInverse;
        // find center of circle as intersection point of perpendicular line and circle with center (x_firstHit, y_firstHit) and radius rMax
        std::vector<double> circle{tmpPoints[0][0], tmpPoints[0][1], rMax};
        std::vector<double> IP = calcIntersectionPointCircleLine(circle, mInverse, bInverse, 0);
        xCenter = IP[0];
        yCenter = IP[1];
      }
      TVector3 apollonius{xCenter, yCenter, rMax};
      TripletSolution solution = FindHitsCloseToCircle(sttHits, apollonius, threshold.fDistanceThresholdSTTFar, fGeometryMap);

      /*
      std::vector<PndSttHit *> hits = FindHitsCloseToCircle(sttHits, apollonius, threshold.fDistanceThresholdSTTFar, fGeometryMap);

      TripletSolution solution(apollonius);

      solution.AddHits(hits);

      solution.SortStt(fGeometryMap);

      solution.fMeanSquare = MeanSquareDistance(solution);
      */
      solution.fTriplet = triplet;

      result.push_back(solution);
    }
  }

  for (int m = 0; m < 3; m++) {
    delete[] tmpPoints[m];
  }

  delete[] apolloniusCircles;
  return result;
}

std::vector<double> PndApollonius::ApolloniusTripletFunctions::calcIntersectionPointCircleLine(std::vector<double> circle, double m, double b, double Ax)
{

  double x0, y0, r, p, q, intersectionPointX1, intersectionPointY1, intersectionPointX2, intersectionPointY2;
  if (m == 0 && b == 0) {
    // m=0;
    x0 = circle[0];
    y0 = circle[1];
    r = circle[2];
    intersectionPointX1 = Ax;
    intersectionPointX2 = Ax;
    //         cout << "y0: " << y0 << " r "<<r<<" x0 "<<x0<< " Ax "<< Ax <<endl;
    intersectionPointY1 = y0 + sqrt(r * r - (Ax - x0) * (Ax - x0));
    intersectionPointY2 = y0 - sqrt(r * r - (Ax - x0) * (Ax - x0));
    // cout << "intersectionPointY1: " << intersectionPointY1 << endl;
    // cout << "intersectionPointY2: " << intersectionPointY2 << endl;
  } else {
    // m = (Ay-By)/(Ax-Bx);

    // b = Ay-m*Ax;
    x0 = circle[0];
    y0 = circle[1];
    r = circle[2];
    p = (2 * m * (b - y0) - 2 * x0) / (m * m + 1);
    q = (x0 * x0 + (b - y0) * (b - y0) - r * r) / (m * m + 1);
    // cout << "m: " << m << endl;
    // cout << "p: " << p << endl;
    // cout << "q: " << q << endl;

    intersectionPointX1 = -p / 2 + sqrt(p * p / 4 - q);
    intersectionPointY1 = m * intersectionPointX1 + b;

    intersectionPointX2 = -p / 2 - sqrt(p * p / 4 - q);
    intersectionPointY2 = m * intersectionPointX2 + b;
  }
  vector<double> intersectionPoints;
  intersectionPoints.push_back(intersectionPointX1);
  intersectionPoints.push_back(intersectionPointY1);
  intersectionPoints.push_back(intersectionPointX2);
  intersectionPoints.push_back(intersectionPointY2);
  return intersectionPoints;
}

PndApollonius::TripletSolution PndApollonius::ApolloniusTripletFunctions::FindHitsCloseToCircle(std::vector<PndSttHit *> &sttHits, TVector3 &circle,
                                                                                                double &fDistanceThresholdSTTFar, PndStt2GeoHandler *fGeometryMap)
                                                                                                //double &fDistanceThresholdSTTFar, PndSttGeometryMap *fGeometryMap)
{
  TripletSolution solution(circle);
  double MeanSquareDistance = 0.0;
  double threshold;
  std::vector<PndSttHit *> result;
  if (circle.Z() != 0) {
    for (auto sttHit : sttHits) {
      PndSttHit *myHit = sttHit;
      if (fGeometryMap->IsSkewedStraw(myHit->GetTubeID())) {
        continue;
      }

      threshold = fDistanceThresholdSTTFar; // DetermineDistanceThreshold(TripletSolution::STT, temp, (FairHit *)myHit);
      double squaredDistance = (myHit->GetX() - circle.X()) * (myHit->GetX() - circle.X()) + (myHit->GetY() - circle.Y()) * (myHit->GetY() - circle.Y());
      double squaredThreshold = (threshold + circle.Z() + sttHit->GetIsochrone()) * (threshold + circle.Z() + sttHit->GetIsochrone());
      if (squaredDistance >= squaredThreshold) {
        continue;
      }
      double d = DistanceCircleSttHit(circle, myHit, squaredDistance);
      if (sttHit->GetIsochrone() != 0 && TMath::Abs(d) < threshold) {
        result.push_back(myHit);
        MeanSquareDistance += d * d;
      } else if (sttHit->GetIsochrone() == 0 && TMath::Abs(d) < threshold + 0.5) { // workaround for bug in STT digitization (isochrone radius = 0)
        result.push_back(myHit);
        MeanSquareDistance += d * d;
      }
    }
  }
  // streight line
  else {
    for (auto sttHit : sttHits) {
      PndSttHit *myHit = sttHit;
      if (fGeometryMap->IsSkewedStraw(myHit->GetTubeID())) {
        continue;
      }
      double squaredDistance = SquaredDistanceLineSttHit(circle, myHit);
      if (squaredDistance >= (threshold + 0.5) * (threshold + 0.5))
        continue;
      double d = DistanceLineSttHit(circle, myHit, squaredDistance);
      if (sttHit->GetIsochrone() != 0 && TMath::Abs(d) < threshold) {
        result.push_back(myHit);
        MeanSquareDistance += d * d;
      } else if (sttHit->GetIsochrone() == 0 && TMath::Abs(d) < threshold + 0.5) { // workaround for bug in STT digitization (isochrone radius = 0)
        result.push_back(myHit);
        MeanSquareDistance += d * d;
      }
    }
  }
  solution.AddHits(result);
  solution.SortStt(fGeometryMap);
  solution.fMeanSquare = MeanSquareDistance / result.size();

  return solution;
}

double PndApollonius::ApolloniusTripletFunctions::MeanSquareDistance(PndApollonius::TripletSolution &solution)
{
  double result = 0.0;
  if (solution.fTrack.Z() == 0) {
    // for (auto hit : solution.fHits[TripletSolution::STT]) {
    // result += TMath::Power(DistanceLineSttHit(solution.fTrack, static_cast<PndSttHit *>(hit)), 2);
    // double d = DistanceLineSttHit(solution.fTrack, static_cast<PndSttHit *>(hit));
    // result += d * d;
    //}
    for (auto hit : solution.fHits[TripletSolution::MVDpixel]) {
      double d = DistanceLinePoint(solution.fTrack, hit);
      result += d * d;
    }
    for (auto hit : solution.fHits[TripletSolution::MVDstrip]) {
      double d = DistanceLinePoint(solution.fTrack, hit);
      result += d * d;
    }
    for (auto hit : solution.fHits[TripletSolution::GEM]) {
      double d = DistanceLinePoint(solution.fTrack, hit);
      result += d * d;
    }
    result /= solution.GetNHits();
  } else {
    // for (auto hit : solution.fHits[TripletSolution::STT]) {
    // double d = DistanceCircleSttHit(solution.fTrack, static_cast<PndSttHit *>(hit));
    // result += d * d;
    //}
    for (auto hit : solution.fHits[TripletSolution::MVDpixel]) {
      double d = DistanceCirclePoint(solution.fTrack, hit);
      result += d * d;
    }
    for (auto hit : solution.fHits[TripletSolution::MVDstrip]) {
      double d = DistanceCirclePoint(solution.fTrack, hit);
      result += d * d;
    }
    for (auto hit : solution.fHits[TripletSolution::GEM]) {
      double d = DistanceCirclePoint(solution.fTrack, hit);
      result += d * d;
    }
    result /= solution.GetNHits();
  }
  return result;
}

double PndApollonius::ApolloniusTripletFunctions::DistanceCirclePoint(TVector3 &circle, FairHit *hit)
{

  TVector2 hitVec{hit->GetX(), hit->GetY()};
  TVector2 circ{circle.X(), circle.Y()};

  // double distCenters = (hitVec - circ).Mod();
  double distCenters = sqrt((hitVec.X() - circ.X()) * (hitVec.X() - circ.X()) + (hitVec.Y() - circ.Y()) * (hitVec.Y() - circ.Y()));
  return (distCenters - circle.Z());
}

double PndApollonius::ApolloniusTripletFunctions::DistanceCircleSttHit(TVector3 &circle, PndSttHit *sttHit)
{

  TVector2 hit{sttHit->GetX(), sttHit->GetY()};
  TVector2 circ{circle.X(), circle.Y()};

  /// double distCenters = (hit - circ).Mod();
  double distCenters = sqrt((hit.X() - circ.X()) * (hit.X() - circ.X()) + (hit.Y() - circ.Y()) * (hit.Y() - circ.Y()));
  if (distCenters - circle.Z() > 0) {
    return (distCenters - circle.Z() - sttHit->GetIsochrone());
  } else {
    return (distCenters - circle.Z() + sttHit->GetIsochrone());
  }
}

double PndApollonius::ApolloniusTripletFunctions::DistanceCircleSttHit(TVector3 &circle, PndSttHit *sttHit, double &sqaredDistance)
{
  /// double distCenters = (hit - circ).Mod();
  double distCenters = sqrt(sqaredDistance);
  if (distCenters - circle.Z() > 0) {
    return (distCenters - circle.Z() - sttHit->GetIsochrone());
  } else {
    return (distCenters - circle.Z() + sttHit->GetIsochrone());
  }
}

double PndApollonius::ApolloniusTripletFunctions::DistanceLineSttHit(TVector3 &circle, PndSttHit *sttHit)
{
  /*
  double m = circle.X(); // line parameters y = m * x + b
  double b = circle.Y(); // line parameters y = m * x + b

  double x2 = (sttHit->GetY() + sttHit->GetX() / m - b) / (m + 1 / m);
  double y2 = -1 / m * x2 + sttHit->GetY() + sttHit->GetX() / m;

  double d = sqrt((sttHit->GetX() - x2) * (sttHit->GetX() - x2) + (sttHit->GetY() - y2) * (sttHit->GetY() - y2)) - abs(sttHit->GetIsochrone());

  return d;
  */
  return sqrt(SquaredDistanceLineSttHit(circle, sttHit));
}

double PndApollonius::ApolloniusTripletFunctions::SquaredDistanceLineSttHit(TVector3 &circle, PndSttHit *sttHit)
{

  double m = circle.X(); // line parameters y = m * x + b
  double b = circle.Y(); // line parameters y = m * x + b

  double x2 = (sttHit->GetY() + sttHit->GetX() / m - b) / (m + 1 / m);
  double y2 = -1 / m * x2 + sttHit->GetY() + sttHit->GetX() / m;

  return ((sttHit->GetX() - x2) * (sttHit->GetX() - x2) + (sttHit->GetY() - y2) * (sttHit->GetY() - y2)) - abs(sttHit->GetIsochrone());
}

double PndApollonius::ApolloniusTripletFunctions::DistanceLinePoint(TVector3 &circle, FairHit *hit)
{

  double m = circle.X(); // line parameters y = m * x + b
  double b = circle.Y(); // line parameters y = m * x + b

  double x2 = (hit->GetY() + hit->GetX() / m - b) / (m + 1 / m);
  double y2 = -1 / m * x2 + hit->GetY() + hit->GetX() / m;

  double d = sqrt((hit->GetX() - x2) * (hit->GetX() - x2) + (hit->GetY() - y2) * (hit->GetY() - y2));

  return d;
}

void PndApollonius::ApolloniusTripletFunctions::CheckContinuitySolutions(std::vector<PndApollonius::TripletSolution> &solutions, PndStt2GeoHandler *fGeometryMap)
//void PndApollonius::ApolloniusTripletFunctions::CheckContinuitySolutions(std::vector<PndApollonius::TripletSolution> &solutions, PndSttGeometryMap *fGeometryMap)
{
  solutions.erase(std::remove_if(solutions.begin(), solutions.end(), [&](TripletSolution &sol) { return !IsContinuous(sol, fGeometryMap); }), solutions.end());
}

bool PndApollonius::ApolloniusTripletFunctions::IsContinuous(PndApollonius::TripletSolution &solution, PndStt2GeoHandler *fGeometryMap)
//bool PndApollonius::ApolloniusTripletFunctions::IsContinuous(PndApollonius::TripletSolution &solution, PndSttGeometryMap *fGeometryMap)
{
  PndTrackEvaluatorDetStt solutionTest(fGeometryMap);
  // std::cout << "IsContinuous? solution: " << solution <<std::endl;
  int j = 0;
  bool goodSolution = true;

  if (solution.fHits[TripletSolution::detID::STT].size() == 0) {
    // std::cout << "has no stt hits" << std::endl;
    return false;
  }

  solution.SortStt(fGeometryMap);

  // std::cout << "solution after sorting: " << solution <<std::endl;
  goodSolution = solutionTest.CheckFirstHit((PndSttHit *)solution.fHits[TripletSolution::detID::STT].front());
  PndSttHit *previousHit = (PndSttHit *)solution.fHits[TripletSolution::detID::STT].front();
  for (auto hit : solution.fHits[TripletSolution::detID::STT]) {
    PndSttHit *sttHit = (PndSttHit *)hit;
    if (sttHit == previousHit) {
      continue;
    } else {
      bool good = solutionTest.CheckTwoHits(previousHit, sttHit);
      if (good != true) {
        // std::cout << "is not conituous for hit (" << sttHit->GetX() << "," << sttHit->GetY() << ") and (" << previousHit->GetX() << "," << previousHit->GetY() << ")" <<
        // std::endl;
        return false;
      }
      previousHit = sttHit;
    }
  }

  if (goodSolution) {
    // std::cout << "continuous solution found" << std::endl;
    return true;
  } else {
    // std::cout << "is not conituous: goodSolution: " << goodSolution << std::endl;

    return false;
  }
}

void PndApollonius::ApolloniusTripletFunctions::AddOtherDetectors(vector<PndApollonius::TripletSolution> &solutions, PndStt2GeoHandler *fStrawMap,
//void PndApollonius::ApolloniusTripletFunctions::AddOtherDetectors(vector<PndApollonius::TripletSolution> &solutions, PndSttStrawMap *fStrawMap,
                                                                  std::map<TString, TClonesArray *> &fBranchMap, TString sttname)
{
  Thresholds threshold;

  for (int i = 0; i < solutions.size(); i++) {
    std::vector<int> sectors;
    sectors.clear();
    double phiHigh = 0;
    double phiLow = 0;
    for (int n = 0; n < solutions[i].fHits[TripletSolution::detID::STT].size(); n++) {
      PndSttHit *sttHit = (PndSttHit *)solutions[i].fHits[TripletSolution::detID::STT][n];
      int sectorId = fStrawMap->GetSector(sttHit->GetTubeID());
      if (std::find(sectors.begin(), sectors.end(), sectorId) == sectors.end()) {
        sectors.push_back(sectorId);
      }
    }
    
    for (int j = 0; j < sectors.size(); j++) {
      vector<int> FirstSectorRow = fStrawMap->GetStrawRow(sectors[j], 0);

      double phiLowTemp = TMath::ATan2(fStrawMap->GetTube(FirstSectorRow[0])->GetPosition().Y(), fStrawMap->GetTube(FirstSectorRow[0])->GetPosition().X());
      double phiHighTemp = TMath::ATan2(fStrawMap->GetTube(FirstSectorRow[FirstSectorRow.size() - 1])->GetPosition().Y(),
                                        fStrawMap->GetTube(FirstSectorRow[FirstSectorRow.size() - 1])->GetPosition().X());
      if (phiLowTemp < 0)
        phiLowTemp = 2 * TMath::Pi() + phiLowTemp;
      if (phiHighTemp < 0)
        phiHighTemp = 2 * TMath::Pi() + phiHighTemp;

      if (phiHigh == 0 || phiHigh < phiHighTemp)
        phiHigh = phiHighTemp;
      if (phiLow == 0 || phiLow > phiLowTemp)
        phiLow = phiLowTemp;
    }

    for (auto it = fBranchMap.begin(); it != fBranchMap.end(); it++) {
      for (int j = 0; j < it->second->GetEntriesFast(); j++) {
        FairHit *hit = (FairHit *)it->second->At(j);
        bool SameSectorRange = kFALSE;
        double phi = TMath::ATan2(hit->GetY(), hit->GetX());
        if (phi < 0)
          phi = 2 * TMath::Pi() + phi;

        if (phiHigh < 60 * TMath::Pi() / 180. && phiLow > 300 * TMath::Pi() / 180.) {
          if (phi < (phiHigh + (30 * TMath::Pi() / 180.)) || phi > (phiLow - (30 * TMath::Pi() / 180.)))
            SameSectorRange = kTRUE;
        } else {
          if (phi < (phiHigh + (30 * TMath::Pi() / 180.)) && phi > (phiLow - (30 * TMath::Pi() / 180.)))
            SameSectorRange = kTRUE;
        }

        double d;
        if (solutions[i].fTrack.Z() != 0)
          d = DistanceCirclePoint(solutions[i].fTrack, hit);
        else if (solutions[i].fTrack.Z() == 0)
          d = DistanceLinePoint(solutions[i].fTrack, hit);

        if (it->first.Contains("MVDHitsPixel")) {
          if (abs(d) < threshold.fDistanceThresholdMVDMid2 && SameSectorRange) {
            // for MVD choose a valid angle of sector borders +- 30 deg (30 deg is a randomly chosen value)
            solutions[i].AddHit(TripletSolution::detID::MVDpixel, hit);
          }
        }
        if (it->first.Contains("MVDHitsStrip")) {
          if (abs(d) < threshold.fDistanceThresholdMVDMid2 && SameSectorRange) {
            solutions[i].AddHit(TripletSolution::detID::MVDstrip, hit);
          }
        }
        if (it->first.Contains("GEMHit")) {
          if (abs(d) < threshold.fDistanceThresholdGEM && SameSectorRange) {
            solutions[i].AddHit(TripletSolution::detID::GEM, hit);
          }
        }
        if (it->first.Contains("STTHit")) {
          PndSttHit *sttHit = (PndSttHit *)hit;
          if (SameSectorRange) {
            if (fStrawMap->IsSkewedStraw(sttHit->GetTubeID())) { // && std::find(sectors.begin(), sectors.end(), fStrawMap->GetSector(sttHit->GetTubeID())) != sectors.end()) {
              if (abs(d) < threshold.fDistanceThresholdSTTSkewed) {
                PndSttSkewedHit *SkewedSttHit = (PndSttSkewedHit *)hit;
                std::pair<Int_t, Int_t> tubeIDs = SkewedSttHit->GetTubeIDs();

                if (fBranchMap[sttname] == nullptr) {
                  LOG(error) << "-E- PndApolloniusTripletTrackFinderTask::FillPndTrack() Branch does not exist: " << fBranchMap[sttname];
                }

                for (int j = 0; j < fBranchMap[sttname]->GetEntriesFast(); j++) {
                  PndSttHit *sttHit = (PndSttHit *)fBranchMap[sttname]->At(j);
                  if ((sttHit->GetTubeID() == tubeIDs.first || sttHit->GetTubeID() == tubeIDs.second)) { // &&
                    //(std::find(sectors.begin(), sectors.end(), fStrawMap->GetSector(sttHit->GetTubeID())) != sectors.end())) {
                    if (std::find(solutions[i].fHits[TripletSolution::detID::STT].begin(), solutions[i].fHits[TripletSolution::detID::STT].end(), (FairHit *)sttHit) ==
                        solutions[i].fHits[TripletSolution::detID::STT].end()) { // add only if hit is not in dataset
                      solutions[i].AddHit(TripletSolution::detID::STT, (FairHit *)sttHit);
                    }
                  }
                }
              }
            } else {
              if (abs(d) < threshold.fDistanceThresholdSTTFar) {
                solutions[i].AddHit(TripletSolution::detID::STT, hit);
              }
            }
          }
        }
        if (it->first.Contains("STTCombinedSkewedHits")) {
          PndSttSkewedHit *SkewedSttHit = (PndSttSkewedHit *)hit;
          std::pair<Int_t, Int_t> tubeIDs = SkewedSttHit->GetTubeIDs();

          if (abs(d) < threshold.fDistanceThresholdSTTCombinedSkewed && SameSectorRange) {
            if (fBranchMap[sttname] == nullptr) {
              LOG(error) << "-E- PndApolloniusTripletTrackFinderTask::FillPndTrack() Branch does not exist: " << fBranchMap[sttname];
            }
            for (int j = 0; j < fBranchMap[sttname]->GetEntriesFast(); j++) {
              PndSttHit *sttHit = (PndSttHit *)fBranchMap[sttname]->At(j);
              if ((sttHit->GetTubeID() == tubeIDs.first || sttHit->GetTubeID() == tubeIDs.second)) { // &&
                //(std::find(sectors.begin(), sectors.end(), fStrawMap->GetSector(sttHit->GetTubeID())) != sectors.end())) {
                // if (solutions[i].fHits.count(TripletSolution::detID::STT) == 0) {
                //  solutions[i].fHits[TripletSolution::detID::STT];
                //}
                if (std::find(solutions[i].fHits[TripletSolution::detID::STT].begin(), solutions[i].fHits[TripletSolution::detID::STT].end(), (FairHit *)sttHit) ==
                    solutions[i].fHits[TripletSolution::detID::STT].end()) { // add only if hit is not in dataset
                  // solutions[i].fHits[TripletSolution::detID::STT].push_back((FairHit *)sttHit);
                  solutions[i].AddHit(TripletSolution::detID::STT, (FairHit *)sttHit);
                }
              }
            }
          }
        }
      }
    }
    solutions[i].fMeanSquare += MeanSquareDistance(solutions[i]);
  }
}

std::vector<PndApollonius::TripletSolution> PndApollonius::ApolloniusTripletFunctions::FindBestSolutions(std::vector<PndApollonius::TripletSolution> &solutions)
{
  std::vector<TripletSolution> result;
  LOG(debug) << "find best Solution out of " << solutions.size() << " possible solutions";

  std::vector<TripletSolution> groupedTracks(solutions.size());

  int rSize = solutions.size();
  std::sort(solutions.begin(), solutions.end(), [](TripletSolution &first, TripletSolution &second) { return (first.fAllHits.size() > second.fAllHits.size()); });

  int i = 0;
  int nResults = 0;
  while (rSize > 0) {
    LOG(debug) << "TrackGroup: " << i++;
    TripletSolution first = solutions.front();
    LOG(debug) << first;

    // copy all tracks with the same hit tubes into the vector grouped Tracks
    std::copy_if(solutions.begin(), solutions.end(), groupedTracks.begin(), [&](TripletSolution &solution) {
      bool equalTubes = true;
      std::for_each(solution.fHits[TripletSolution::detID::STT].begin(), solution.fHits[TripletSolution::detID::STT].end(), [&](FairHit *hit) {
        if (std::find_if(first.fHits[TripletSolution::detID::STT].begin(), first.fHits[TripletSolution::detID::STT].end(), [&](FairHit *firstHit) {
              return (((PndSttHit *)firstHit)->GetTubeID() == ((PndSttHit *)hit)->GetTubeID());
            }) == first.fHits[TripletSolution::detID::STT].end()) {
          equalTubes = false;
        }
      });
      if (equalTubes == true) {
        nResults++;
      }
      return equalTubes;
      //      if (first.fHits[TripletSolution::detID::STT] == solution.fHits[TripletSolution::detID::STT]) {
      //        nResults++;
      //        return true;
      //      } else
      //        return false;
    });

    LOG(debug) << "GroupedTracks: ";
    std::for_each(groupedTracks.begin(), groupedTracks.end(), [](TripletSolution &sol) { LOG(debug) << sol; });

    solutions.resize(nResults);
    nResults = 0;
    solutions.erase(std::remove_if(solutions.begin(), solutions.end(),
                                   [&](TripletSolution &solution) {
                                     bool equalTubes = true;
                                     std::for_each(solution.fHits[TripletSolution::detID::STT].begin(), solution.fHits[TripletSolution::detID::STT].end(), [&](FairHit *hit) {
                                       if (std::find_if(first.fHits[TripletSolution::detID::STT].begin(), first.fHits[TripletSolution::detID::STT].end(), [&](FairHit *firstHit) {
                                             return (((PndSttHit *)firstHit)->GetTubeID() == ((PndSttHit *)hit)->GetTubeID());
                                           }) == first.fHits[TripletSolution::detID::STT].end()) {
                                         equalTubes = false;
                                       }
                                     });
                                     return equalTubes;
                                   }),
                    solutions.end());
    rSize = solutions.size();

    // save only the track with the smalles mean square
    std::sort(groupedTracks.begin(), groupedTracks.end(), [](TripletSolution &a, TripletSolution &b) { return (a.fMeanSquare < b.fMeanSquare); });
    result.push_back(groupedTracks[0]); // add track with lowest mean square

    // add track with highest number of hits (and lowest mean square if more than one)
    std::sort(groupedTracks.begin(), groupedTracks.end(),
              [](TripletSolution &a, TripletSolution &b) { return (a.fHits[TripletSolution::detID::STT].size() > b.fHits[TripletSolution::detID::STT].size()); });

    int maxNHits = groupedTracks[0].fHits[TripletSolution::detID::STT].size();
    TripletSolution best;
    double lowestMeanSquare = 10000.;
    if (groupedTracks[0].fHits[TripletSolution::detID::STT].size() > result[0].fHits[TripletSolution::detID::STT].size()) {
      for (auto track : groupedTracks) {
        if (track.fHits[TripletSolution::detID::STT].size() < maxNHits)
          break;
        if (track.fMeanSquare < lowestMeanSquare) {
          lowestMeanSquare = track.fMeanSquare;
          best = track;
        }
      }
      result.push_back(best); // add track with largest number of hits if not already in
    }
    LOG(debug) << "GroupedTracks solutions: ";
    std::for_each(result.begin(), result.end(), [](TripletSolution &sol) { LOG(debug) << sol; });
  }

  return result;
}

std::vector<PndApollonius::TripletSolution>
PndApollonius::ApolloniusTripletFunctions::CheckCombinedSolutions(std::vector<PndApollonius::TripletSolution> &solutions, int nExpectedTracks)
{
  LOG(debug) << "Combine tracks with identical hits. Expected tracks: " << nExpectedTracks << ", Found Tracks: " << solutions.size();

  //  if (fIsCurling) {
  //    nExpectedTracks--;
  //  }
  std::vector<TripletSolution> result;
  if (nExpectedTracks < 1) {
    LOG(debug) << "-E- no Track expected: " << nExpectedTracks;
    return solutions;
  }
  int diff = solutions.size() - nExpectedTracks;
  // Combine Track Solutions: combines all found tracks to groups of tracks possibly belonging together
  if (diff < 0) {
    // std::cout << "Less tracks as expected. All returned" << std::endl;
    return solutions; // todo: better to return empty solutions?
  }
  // std::cout << "solutions.size(): " << solutions.size() << " nExpectedTracks:" << nExpectedTracks << " diff: " << diff << std::endl;
  if (diff > 10) {
    // std::cout << "Too many combinations possible. NO combinations done" << std::endl;
    return result; // todo: better to return empty solutions?
  }

  std::vector<std::vector<int>> combinations = GetKOutOfN(nExpectedTracks, solutions.size());
  /*
  for (int i = 0; i < combinations.size(); i++) {
    std::cout << "combination: ";
    for (int j = 0; j < combinations[i].size(); j++) {
      std::cout << combinations[i][j] << " / ";
    }
    std::cout << std::endl;
  }
  */
  // Check Combined Track Solutions if they are a possible solution
  result = CheckSolutions(solutions, combinations);

  return result;
}

std::vector<PndApollonius::TripletSolution>
PndApollonius::ApolloniusTripletFunctions::CheckSolutions(std::vector<PndApollonius::TripletSolution> &solutions, std::vector<std::vector<int>> combinations)
{
  std::vector<TripletSolution> result;

  // determines all tubes of a combination of tracks
  std::vector<std::pair<std::vector<int>, int>> uniqueTubesPerCombination;
  std::for_each(combinations.begin(), combinations.end(),
                [&](std::vector<int> combi) { uniqueTubesPerCombination.push_back(make_pair(combi, GetUniqueTubeIDs(solutions, combi).size())); });

  // sorts the vector containing the track combinations by the number of found tubes
  std::sort(uniqueTubesPerCombination.begin(), uniqueTubesPerCombination.end(),
            [](std::pair<std::vector<int>, int> &first, std::pair<std::vector<int>, int> &second) { return first.second > second.second; });

  LOG(debug) << "Combinations TubeCounts: " << uniqueTubesPerCombination.size();
  //  std::for_each(uniqueTubesPerCombination.begin(), uniqueTubesPerCombination.end(), [](std::pair<std::vector<int>, int> &combi) {
  //    std::for_each(combi.first.begin(), combi.first.end(), [](int &val) { std::cout << val << "/"; });
  //    std::cout << " counts " << combi.second << std::endl;
  //  });
  int highestCount = uniqueTubesPerCombination.front().second;

  std::vector<TripletSolution> tmpSolutions;
  std::vector<int> solutionsUsed;
  for (auto combi : uniqueTubesPerCombination) {
    if (highestCount - combi.second < 1) { // take the solution with the highest hits found todo: this has to be checked
      for (auto sol : combi.first) {
        if (std::count(solutionsUsed.begin(), solutionsUsed.end(), sol) == 0) {
          tmpSolutions.push_back(solutions[sol]);
          solutionsUsed.push_back(sol);
        }
      }
    } else {
      break;
    }
  }
  result = tmpSolutions;
  return result;
}

std::vector<int> PndApollonius::ApolloniusTripletFunctions::GetUniqueTubeIDs(std::vector<TripletSolution> &solutions, std::vector<int> combinations)
{
  std::vector<int> result;
  std::for_each(combinations.begin(), combinations.end(), [&](int sol) { // saves all tubes of all tracks in combinations
    std::vector<FairHit *> sttHits(solutions[sol].fHits[TripletSolution::STT]);
    std::for_each(sttHits.begin(), sttHits.end(), [&](FairHit *hit) {                     // saves all tubes of a track
      if (std::count(result.begin(), result.end(), ((PndSttHit *)hit)->GetTubeID()) == 0) // is tube in result? -->if not add tube
        result.push_back(((PndSttHit *)hit)->GetTubeID());
    });
  });
  return result;
}

std::vector<std::vector<int>> PndApollonius::ApolloniusTripletFunctions::GetKOutOfN(int k, int n)
{
  std::vector<std::vector<int>> result;
  std::string bitmask(k, 1); // K leading 1's
  bitmask.resize(n, 0);      // N-K trailing 0's

  // print integers and permute bitmask
  do {
    std::vector<int> tmp;
    for (int i = 0; i < n; ++i) // [0..N-1] integers
    {
      if (bitmask[i])
        tmp.push_back(i);
    }
    result.push_back(tmp);
  } while (std::prev_permutation(bitmask.begin(), bitmask.end()));

  LOG(debug) << "Combinations: " << result.size();

  return result;
}

std::vector<PndApollonius::TripletSolution> PndApollonius::ApolloniusTripletFunctions::CombineIdenticalSolutions(std::vector<PndApollonius::TripletSolution> &solutions)
{
  std::vector<TripletSolution> result;
  std::sort(solutions.begin(), solutions.end(), [](TripletSolution &first, TripletSolution &second) { return (first.GetNHits() > second.GetNHits()); });

  if (solutions.size() == 0)
    return result;
  for (auto solution : solutions) {
    if (std::none_of(result.begin(), result.end(), [&](TripletSolution &res) { return ContainsTriplet(solution.fTriplet, res); })) {
      result.push_back(solution);
    }
  }
  return result;
}

bool PndApollonius::ApolloniusTripletFunctions::ContainsTriplet(PndApollonius::Triplet &triplet, PndApollonius::TripletSolution &solution)
{
  bool result = true;
  for (auto hit : triplet.fTripletHits) {
    auto found = std::find(solution.fHits[TripletSolution::detID::STT].begin(), solution.fHits[TripletSolution::detID::STT].end(), hit);
    if (found == solution.fHits[TripletSolution::detID::STT].end()) {
      return false;
    }
  }
  return result;
}

std::vector<PndSttHit *> PndApollonius::ApolloniusTripletFunctions::TubeReduction(std::vector<std::pair<int, int>> &Rows,
                                                                                  std::map<int, std::vector<std::vector<PndSttHit *>>> &tubeStructure,
                                                                                  std::map<FairLink, int> &fMapHitstoCATracklet, int position)
{
  std::vector<PndSttHit *> result;
  std::map<int, std::vector<PndSttHit *>> FoundCAs;
  result.clear();
  FoundCAs.clear();
  for (auto row : Rows) {
    for (auto hitGroup : tubeStructure[row.first]) {
      if (hitGroup.size() > 5)
        continue;
      for (auto sttHit : hitGroup) {
        auto it = fMapHitstoCATracklet.find(sttHit->GetEntryNr());
        if (it == fMapHitstoCATracklet.end()) {
          // the hit doesnt belong to a CA tracklet
          result.push_back(sttHit);
        } else {
          if (FoundCAs.find(fMapHitstoCATracklet[sttHit->GetEntryNr()]) == FoundCAs.end()) {
            // std::cout << "the hit belongs to a CA tracklet" << std::endl;
            // CA track was still not found
            std::vector<PndSttHit *> temp;
            temp.push_back(sttHit);
            FoundCAs[fMapHitstoCATracklet[sttHit->GetEntryNr()]] = {temp};
          } else {
            FoundCAs[fMapHitstoCATracklet[sttHit->GetEntryNr()]].push_back(sttHit);
          }
        }
      }
    }
  }
  std::map<int, std::vector<PndSttHit *>>::iterator FoundCAsIter;

  for (FoundCAsIter = FoundCAs.begin(); FoundCAsIter != FoundCAs.end(); FoundCAsIter++) {
    PndSttHit *CASttHit;
    FairLink CALink;
    std::sort(FoundCAsIter->second.begin(), FoundCAsIter->second.end());
    if (position == 0) {
      CASttHit = FoundCAsIter->second[0];
      // fMapInnerTubeToCA[CASttHit] = fMapHitstoCATracklet[CALink];
    } else if (position == 1) {
      CASttHit = FoundCAsIter->second[FoundCAsIter->second.size() / 2];
      // fMapCAToOuterTube[fMapHitstoCATracklet[CALink]] = CASttHit;
    } else if (position == 2) {
      CASttHit = FoundCAsIter->second[FoundCAsIter->second.size() - 1];
      // fMapCAToOuterTube[fMapHitstoCATracklet[CALink]] = CASttHit;
    }
    result.push_back(CASttHit);
  }
  return result;
}

std::map<FairLink, int> PndApollonius::ApolloniusTripletFunctions::GetHitsToCAMap(PndSttCA *fCATrackFinder)
{
  std::map<FairLink, int> fMapHitstoCATracklet;
  fMapHitstoCATracklet.clear();
  fCATrackFinder->FindTracks();

  for (int i = 0; i < fCATrackFinder->NumFirstTrackCands(); i++) {
    PndTrackCand trackCand_temp = fCATrackFinder->GetFirstTrackCand(i);
    if (trackCand_temp.GetNHits() < 2)
      continue;

    for (int j = 0; j < trackCand_temp.GetNHits(); j++) {
      fMapHitstoCATracklet[trackCand_temp.GetSortedHit(j)] = i;
      // std::cout << "fMapHitstoCATracklet. Hit " << j << " belongs to ca tracklet " << i << std::endl;
    }
  }
  return fMapHitstoCATracklet;
}

double PndApollonius::ApolloniusTripletFunctions::CalcRatioSameHits(PndApollonius::TripletSolution &sol, std::vector<PndApollonius::TripletSolution> &AlreadyFoundSolutions)
{
  double MaxFraction = 0.;
  for (TripletSolution foundSolution : AlreadyFoundSolutions) {
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

std::vector<PndApollonius::TripletSolution>
PndApollonius::ApolloniusTripletFunctions::CombineIdenticalSolutionsFinal(std::vector<PndApollonius::TripletSolution> &solutions, Double_t ratioOfSameHits)
{

  std::vector<TripletSolution> result;
  std::sort(solutions.begin(), solutions.end(), [](TripletSolution &first, TripletSolution &second) { return (first.GetNHits() > second.GetNHits()); });
  for (TripletSolution sol : solutions) {
    double RatioSameHits = CalcRatioSameHits(sol, result);
    // std::cout << "maximum fraction of same hits " << RatioSameHits << std::endl;
    if (RatioSameHits < ratioOfSameHits) {
      result.push_back(sol);
    }
  }

  return result;
}

PndTrack PndApollonius::ApolloniusTripletFunctions::FromTripletSolutionToPndTrack(PndApollonius::TripletSolution &sol, Double_t B)
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

PndTrack PndApollonius::ApolloniusTripletFunctions::FromTripletSolutionToPndTrack(PndApollonius::TripletSolution &sol, Double_t B, PndTrackCand &cand)
{
  LOG(debug) << "FromTripletSolutionToPndTrack PndTrackCand" << std::endl;

  TVector3 hitPos;
  TVector3 hitPosError(0.015, 0.015, 0.015);
  TVector3 mom;
  TVector3 momError(2, 2, 2);
  TVector3 dj(1, 0, 0);
  TVector3 dk(0, 1, 0);
  TVector3 origin(0, 0, 1);

  int i = 0;
  if (sol.fAllHits.size() < 4)
    return PndTrack();

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
