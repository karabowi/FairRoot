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
 * PndTrackEvaluatorDetStt.cxx
 *
 *  Created on: 19.08.2021
 *      Author: tstockmanns
 */

#include <PndTrackEvaluatorDetStt.h>
#include "PndStt2GeoHandler.h"

PndTrackEvaluatorDetStt::PndTrackEvaluatorDetStt(PndStt2GeoHandler *geoH) : fSttGeoH(geoH) {}
//PndTrackEvaluatorDetStt::PndTrackEvaluatorDetStt(PndSttGeometryMap *sttMap) : fSttMap(sttMap) {}

PndTrackEvaluatorDetStt::~PndTrackEvaluatorDetStt()
{
  // TODO Auto-generated destructor stub
}

bool PndTrackEvaluatorDetStt::CheckFirstHit(FairLink *hit)
{
  std::unique_ptr<PndSttHit> sttHit(GetSttHit(hit));
  return CheckFirstHit(sttHit.get());
}

bool PndTrackEvaluatorDetStt::CheckFirstHit(PndSttHit *sttHit)
{

  //  return fSttMap->IsEdgeStraw(sttHit->GetTubeID()); // todo: check close to skewed
  return true;
}

bool PndTrackEvaluatorDetStt::CheckLastHit(FairLink *hit)
{
  //  return CheckFirstHit(hit);
  return true;
}

bool PndTrackEvaluatorDetStt::CheckLastHit(PndSttHit *hit)
{
  return CheckFirstHit(hit);
}

bool PndTrackEvaluatorDetStt::CheckTwoHits(FairLink *first, FairLink *second)
{
  std::unique_ptr<PndSttHit> firstSttHit(GetSttHit(first));
  std::unique_ptr<PndSttHit> secondSttHit(GetSttHit(second));

  return CheckTwoHits(firstSttHit.get(), secondSttHit.get());
}

bool PndTrackEvaluatorDetStt::CheckTwoHits(PndSttHit *firstSttHit, PndSttHit *secondSttHit)
{
  TVector3 firstPos;
  TVector3 secondPos;
  firstSttHit->Position(firstPos);
  secondSttHit->Position(secondPos);

  //  std::cout << "Check two hits: " << *firstSttHit << " " << *secondSttHit << std::endl;
  if ((secondPos.X() - firstPos.X()) * (secondPos.X() - firstPos.X()) + (secondPos.Y() - firstPos.Y()) * (secondPos.Y() - firstPos.Y()) +
        (secondPos.Z() - firstPos.Z()) * (secondPos.Z() - firstPos.Z()) <
      4) { // if the distance between the two points is less than 2 cm then they are connected
           // if ((secondPos - firstPos).Mag() < 2) { // if the distance between the two points is less than 2 cm then they are connected
    //    std::cout << "close together" << std::endl;
    return true;
  } else {
    if (fSttGeoH->IsEdgeStraw(firstSttHit->GetTubeID()) && fSttGeoH->IsEdgeStraw(secondSttHit->GetTubeID())) { // if the distance is larger they could be edge straws
    //if (fSttMap->IsEdgeStraw(firstSttHit->GetTubeID()) && fSttMap->IsEdgeStraw(secondSttHit->GetTubeID())) { // if the distance is larger they could be edge straws
      if (fSttGeoH->GetRow(firstSttHit->GetTubeID()) != 0 && fSttGeoH->GetRow(secondSttHit->GetTubeID()) != 0) {
      //if (fSttMap->GetRow(firstSttHit->GetTubeID()) != 0 && fSttMap->GetRow(secondSttHit->GetTubeID()) != 0) {
        //        std::cout << "both edge and not row 0" << std::endl;
        return true;
      }
    }
    if (fSttGeoH->IsSkewedStraw(firstSttHit->GetTubeID()) || fSttGeoH->IsSkewedStraw(secondSttHit->GetTubeID())) { // if the distance is larger one could be a skewed straw
    //if (fSttMap->IsSkewedStraw(firstSttHit->GetTubeID()) || fSttMap->IsSkewedStraw(secondSttHit->GetTubeID())) { // if the distance is larger one could be a skewed straw
      TArrayI neighbors = fSttGeoH->GetNeighborings(firstSttHit->GetTubeID());
      //TArrayI neighbors = fSttMap->FindNeighborings(firstSttHit->GetTubeID());
      bool isneighbor = false;
      for (int i = 0; i < neighbors.GetSize(); i++) {
        if (neighbors[i] == secondSttHit->GetTubeID()) {
          //          std::cout << "neighbors" << std::endl;
          return true;
        }
      }
    }
    if (fSttGeoH->IsAxialStraw(firstSttHit->GetTubeID()) && fSttGeoH->IsAxialStraw(secondSttHit->GetTubeID())) { // both are axial straws
    //if (fSttMap->IsAxialStraw(firstSttHit->GetTubeID()) && fSttMap->IsAxialStraw(secondSttHit->GetTubeID())) { // both are axial straws
      int firstRow(fSttGeoH->GetRow(firstSttHit->GetTubeID()));
      //int firstRow(fSttMap->GetRow(firstSttHit->GetTubeID()));
      int secondRow(fSttGeoH->GetRow(secondSttHit->GetTubeID()));
      //int secondRow(fSttMap->GetRow(secondSttHit->GetTubeID()));
      if ((firstRow == 7 || firstRow == 16) && (secondRow == 7 || secondRow == 16)) { // if the distance is larger they could be next to skewed layers
                                                                                      //        std::cout << "skewed neigbours" << std::endl;
        return true;
      }
    }
  }
  // std::cout << "hits not connected: " << firstSttHit->GetTubeID() << " " << secondSttHit->GetTubeID() << std::endl;
  return false;
}
