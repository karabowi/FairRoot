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

// A basic clusterfinder for pixel detectors
#include "PndMvdPixelClusterFinder.h"
#include "TMath.h"

#include <iostream>

#include "FairMQLogger.h"

std::vector<std::vector<Int_t>> PndMvdPixelClusterFinder::GetClusters(std::vector<PndSdsDigiPixel *> &hits)
{
  //	fHits = hits;

  //	LOG(info) << "HitsIn Cluster Finder!" << std::endl;
  std::vector<Int_t> posHits;
  for (UInt_t i = 0; i < hits.size(); i++) {
    //		LOG(info) << i << " : " << hits[i];
    //		LOG(info) << i << " : " << hits[i].GetTimeStamp();
    posHits.push_back(i);
  }
  std::vector<std::vector<Int_t>> result;
  Int_t sizeTempHits = posHits.size();
  // Int_t actHit = 0;
  while (sizeTempHits != 0) {
    std::vector<Int_t> tempInt;
    tempInt.push_back(MoveHit(&posHits, 0));
    result.push_back(tempInt);
    Int_t sizeResultI = (result.end() - 1)->size(); // size of last vector in array
    for (Int_t i = 0; i < sizeResultI; i++) {
      sizeTempHits = posHits.size();
      for (Int_t j = 0; j < sizeTempHits; j++) {
        if (hits[(*(result.end() - 1))[i]]->GetSensorID() == hits[posHits[j]]->GetSensorID()) {
          if (IsInRange(hits[(*(result.end() - 1))[i]], hits[posHits[j]])) {
            (result.end() - 1)->push_back(MoveHit(&posHits, j));
            j--;
          }
        }
        sizeTempHits = posHits.size();
      }
      sizeResultI = (result.end() - 1)->size();
    }
  }
  return result;
}

Int_t PndMvdPixelClusterFinder::MoveHit(std::vector<Int_t> *hitVector, Int_t index) const
{
  Int_t result = -1;
  if (index < (Int_t)hitVector->size()) {
    result = (*hitVector)[index];
    hitVector->erase(hitVector->begin() + index);
  }
  return result;
}

bool PndMvdPixelClusterFinder::IsInRange(PndSdsDigiPixel *hit1, PndSdsDigiPixel *hit2) const
{
  Double_t result1, result2;
  Int_t col1 = hit1->GetPixelColumn() + (Int_t)((hit1->GetFE() % 10) * fcols);
  Int_t col2 = hit2->GetPixelColumn() + (Int_t)((hit2->GetFE() % 10) * fcols);
  Int_t row1 = hit1->GetPixelRow() + (Int_t)((hit1->GetFE() / 10) * frows);
  Int_t row2 = hit2->GetPixelRow() + (Int_t)((hit2->GetFE() / 10) * frows);

  result1 = (col1 - col2);
  result1 *= result1;
  result2 = (row1 - row2);
  result2 *= result2;
  result1 += result2;

  //	LOG(info) << "PndMvdPixelClusterFinder:IsInRange";
  //	LOG(info) << "hit1: " << *hit1;
  //	LOG(info) << "hit2: " << *hit2;
  //	LOG(info) << "Distance: " << TMath::Sqrt(result1) << " radius " << fradius;

  return (TMath::Sqrt(result1) < fradius);
}
