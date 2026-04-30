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
 * PndSttTubeMapIdCreatorRoot.cpp
 *
 *  Created on: 02.08.2018
 *      Author: tstockmanns
 */

#include "PndSttTubeIdMap.h"
#include <PndSttTubeMapCreatorRoot.h>
#include "PndGeoHandling.h"
#include "PndStringSeparator.h"
#include "PndSttTubeCreator.h"
#include "PndSttNeighborhoodCreator.h"
#include "PndSttTubeIdMap.h"

#include <vector>
#include <string>
#include <algorithm>
#include <iostream>

ClassImp(PndSttTubeMapCreatorRoot);

PndSttTubeMapCreatorRoot::PndSttTubeMapCreatorRoot()
{
  // TODO Auto-generated constructor stub
}

PndSttTubeMapCreatorRoot::~PndSttTubeMapCreatorRoot()
{
  // TODO Auto-generated destructor stubPndSttTubeIdMapeMap* PndSttTubeMapCreatorRoot::CreateTubeMap()
}

PndSttTubeMap *PndSttTubeMapCreatorRoot::CreateTubeMap()
{

  // create limitation map
  for (int sector = 0; sector < 6; sector++) {
    for (int row = 0; row < 29; row++) {
      int MaxCpyNum = 0;
      std::vector<int> vecRow = PndSttTubeIdMap::Instance()->GetRowInSector(sector, row);
      //  	    std::cout<<" vecRow size = "<<vecRow.size()<<std::endl;

      for (int j = 0; j < vecRow.size(); j++) {
        //				std::cout << "Id = " << vecRow[j] << " pos in row = " << j
        //						<< std::endl;
        //				std::cout << PndGeoHandling::Instance()->GetPath(vecRow[j])
        //						<< std::endl;

        TString path = PndGeoHandling::Instance()->GetPath(vecRow[j]);
        std::string nameString(path.Data());
        PndStringSeparator sep(nameString, "/-_");
        std::vector<std::string> stringVector = sep.GetStringVector();
        auto i = std::find_if(stringVector.begin(), stringVector.end(), [&](const std::string &val) {
          auto found = val.find("tubestt02");
          return found != std::string::npos;
        });
        int pos = std::distance(stringVector.begin(), i);
        int tubeType = std::stoi(stringVector[pos + 1]);
        int cpyNum = std::stoi(stringVector[pos + 2]);
        //		        std::cout << "cpyNum = " << cpyNum << std::endl;

        if (tubeType != 0) {
          if (cpyNum == 0)
            additionalSkewedTubeMap[vecRow[j]] = 1;
          else
            additionalSkewedTubeMap[vecRow[j]] = 2;
          //					std::cout << "additionalSkewedTubeMap ID =" << vecRow[j] << std::endl;
        } else {
          additionalSkewedTubeMap[vecRow[j]] = 0;
          if (cpyNum == 0)
            sRowTubeID[sector][row] = vecRow[j];
          if (cpyNum > MaxCpyNum) {
            MaxCpyNum = cpyNum;
            eRowTubeID[sector][row] = vecRow[j];
          }
        }
        // std::cout<<IsEdgeStraw(tubeId)<<IsSectorBorderStraw(tubeId)<<std::endl;
      }
      //			std::cout<<sRowTubeID[sector][row]<<":"<<eRowTubeID[sector][row]<<std::endl;
    }
  }

  PndSttTubeMap *result = PndSttTubeMap::Instance();
  PndSttTubeCreator tubeCreator;
  std::vector<TString> nodeNames = PndGeoHandling::Instance()->GetSensorNamesWithString("ArCO2Sensitive");
  //    std::cout << "-I- PndSttTubeMapCreatorRoot::CreateTubeMap Nodes: " << nodeNames.size() << std::endl;
  for (auto name : nodeNames) {
    //        std::cout << "Node: " << name.Data() << std::endl;
    int tubeId = PndGeoHandling::Instance()->GetShortID(name);
    PndSttTube *myTube = tubeCreator.GetTube(tubeId);
    myTube->SetSectorLimitFlag(IsEdgeStraw(tubeId));
    myTube->SetLayerLimitFlag(IsSectorBorderStraw(tubeId));
    result->AddTube(tubeId, myTube);
    //        std::cout << *myTube << std::endl;
    //        std::cout << "GeoHandling Matrix: " << std::endl;
    //        TGeoHMatrix* mat = PndGeoHandling::Instance()->GetMatrixPath(name);
    //        mat->Print();
  }

  std::map<int, PndSttTube *> tubeMap = result->GetMap();
  for (auto tubePair : tubeMap) {
    AssignNeighbors(tubePair.second);
  }
  //    std::cout<<"tubeId = "<<176 << " IsEdgeStraw = "<<IsEdgeStraw(176)<<std::endl;
  //    for_each(tubeMap.begin(), tubeMap.end(), [](std::pair<int, PndSttTube*> pair){ std::cout << *pair.second << std::endl;});

  return result;
}

void PndSttTubeMapCreatorRoot::AssignNeighbors(PndSttTube *tube)
{
  PndSttNeighborhoodCreator creator;
  std::vector<int> neighbors = creator.FindNeighbors(tube);
  //    std::cout << "PndSttTubeMapCreatorRoot::AssignNeighbors size vector " << neighbors.size();
  TArrayI neighborArray;
  for_each(neighbors.begin(), neighbors.end(), [&neighborArray](int &id) {
    int size = neighborArray.GetSize();
    neighborArray.Set(size + 1);
    neighborArray.AddAt(id, size);
  });
  tube->SetNeighborings(neighborArray);
  //    std::cout << " size of Array: " << neighborArray.GetSize() << std::endl;
}

bool PndSttTubeMapCreatorRoot::IsEdgeStraw(int tubeId) const
{

  std::pair<int, int> sectorRow = PndSttTubeIdMap::Instance()->GetSectorRowFromTubeId(tubeId);
  // std::cout << "sector: " << sectorRow.first << " row:" << sectorRow.second 	<< std::endl;

  // if (row == 0) return true;
  if (additionalSkewedTubeMap[tubeId] > 0 && sectorRow.first != 1 && sectorRow.first != 4) {
    if (sectorRow.first == 0 || sectorRow.first == 3) {
      if (additionalSkewedTubeMap[tubeId] == 1)
        return true;
      else
        return false;
    } else if (sectorRow.first == 2 || sectorRow.first == 5) {
      if (additionalSkewedTubeMap[tubeId] == 2)
        return true;
      else
        return false;
    }
  }
  int endstraw = 0;
  switch (sectorRow.first) {
  case 0: endstraw = sRowTubeID[sectorRow.first][sectorRow.second]; break;
  case 2: endstraw = eRowTubeID[sectorRow.first][sectorRow.second]; break;
  case 3: endstraw = sRowTubeID[sectorRow.first][sectorRow.second]; break;
  case 5: endstraw = eRowTubeID[sectorRow.first][sectorRow.second]; break;
  default: return false;
  };
  return (tubeId == endstraw);
}
int PndSttTubeMapCreatorRoot::IsSectorBorderStraw(int tubeId) const
{

  std::pair<int, int> sectorRow = PndSttTubeIdMap::Instance()->GetSectorRowFromTubeId(tubeId);

  if (additionalSkewedTubeMap[tubeId] > 0) {
    if (additionalSkewedTubeMap[tubeId] == 1)
      return 1;
    if (additionalSkewedTubeMap[tubeId] == 2)
      return -1;
  } else {
    int endstrawcw = sRowTubeID[sectorRow.first][sectorRow.second];
    int endstrawccw = eRowTubeID[sectorRow.first][sectorRow.second];
    if (tubeId == endstrawcw)
      return -1;
    if (tubeId == endstrawccw)
      return 1;
  }

  return 0;
}
