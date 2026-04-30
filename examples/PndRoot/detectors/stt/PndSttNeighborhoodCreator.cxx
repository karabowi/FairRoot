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
 * PndSttNeighborhoodCreator.cxx
 *
 *  Created on: 03.08.2018
 *      Author: tstockmanns
 */

#include "PndSttNeighborhoodCreator.h"
#include "PndSttTube.h"
#include "PndSttTubeIdMap.h"
#include "PndSttTubeMap.h"

#include <utility>
#include <algorithm>
#include <iostream>

ClassImp(PndSttNeighborhoodCreator);

PndSttNeighborhoodCreator::PndSttNeighborhoodCreator()
{
  // TODO Auto-generated constructor stub
}

PndSttNeighborhoodCreator::~PndSttNeighborhoodCreator()
{
  // TODO Auto-generated destructor stub
}

std::vector<int> PndSttNeighborhoodCreator::FindNeighbors(PndSttTube *tube)
{
  std::vector<int> result;
  std::vector<std::pair<int, int>> sectorRows = GetListOfSectorRowsToCompare(tube);

  for (auto pairs : sectorRows) {
    std::vector<int> tubesToTest = PndSttTubeIdMap::Instance()->GetRowInSector(pairs.first, pairs.second);
    for (auto tubeId : tubesToTest) {
      PndSttTube *testTube = PndSttTubeMap::Instance()->GetTube(tubeId);
      if (testTube == tube)
        continue;
      double tolerance = GetTolerance(tube, testTube);
      if (Compare(tube, testTube, tolerance) == true) {
        result.push_back(tubeId);
      }
    }
  }
  return result;
}

std::vector<std::pair<int, int>> PndSttNeighborhoodCreator::GetListOfSectorRowsToCompare(PndSttTube *tube)
{
  int sector = tube->GetSectorID();
  int row = tube->GetLayerID();

  std::vector<int> testRows;
  std::vector<int> testSectors;
  std::vector<std::pair<int, int>> result;

  for (int testRow = row - 1; testRow < row + 2 && testRow < 29; testRow++) {
    if (testRow > -1)
      testRows.push_back(testRow);
  }

  int offset = (sector < 3 ? 0 : 3);

  for (int testSector = sector - 1; testSector < 3 + offset; testSector++) {
    if (testSector > (offset - 1))
      testSectors.push_back(testSector);
  }

  for (auto testSector : testSectors) {
    for (auto testRow : testRows) {
      result.push_back(std::make_pair(testSector, testRow));
    }
  }

  //    std::cout << "PndSttNeighborhoodCreator::GetListOfSectorRowsToCompare sector/row " << tube->GetTubeID() << " " << sector << "/" << row << std::endl;
  //    for_each(result.begin(), result.end(), [](std::pair<int, int>& p){std::cout  << p.first << "/" << p.second << std::endl;});

  return result;
}

bool PndSttNeighborhoodCreator::Compare(PndSttTube *tube1, PndSttTube *tube2, double tolerance)
{
  double distance = tube1->GetDistance(tube2);
  return distance < tolerance;
}

double PndSttNeighborhoodCreator::GetTolerance(PndSttTube *tube1, PndSttTube *tube2)
{

  double tolerance0 = 1.2; // parallel/parallel && inner parallel/skew
  double tolerance1 = 1.3; // skew/skew
  double tolerance2 = 1.5; // outer parallel/skew
  double tolerance = 1.5;  // CHECK tolerance

  if (tube2->IsSkew() == kTRUE && tube1->IsSkew() == kTRUE)
    tolerance = tolerance1;
  else if (tube2->IsParallel() == kTRUE && tube1->IsParallel() == kTRUE)
    tolerance = tolerance0;
  else {
    if (tube1->GetLayerID() > 17 || tube2->GetLayerID() > 17)
      tolerance = tolerance2;
    else
      tolerance = tolerance1;
  }
  return tolerance;
}
