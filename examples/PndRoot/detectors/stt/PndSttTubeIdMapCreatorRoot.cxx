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
#include <PndSttTubeIdMapCreatorRoot.h>
#include "PndGeoHandling.h"
#include "PndStringSeparator.h"

#include <vector>
#include <string>
#include <algorithm>
#include <iostream>

ClassImp(PndSttTubeIdMapCreatorRoot);

PndSttTubeIdMapCreatorRoot::PndSttTubeIdMapCreatorRoot()
{
  // TODO Auto-generated constructor stub
}

PndSttTubeIdMapCreatorRoot::~PndSttTubeIdMapCreatorRoot()
{
  // TODO Auto-generated destructor stubPndSttTubeIdMapeMap* PndSttTubeIdMapCreatorRoot::CreateTubeMap()
}

PndSttTubeIdMap *PndSttTubeIdMapCreatorRoot::CreateTubeMap()
{
  PndSttTubeIdMap *result = PndSttTubeIdMap::Instance();
  std::vector<TString> nodeNames = PndGeoHandling::Instance()->GetSensorNamesWithString("ArCO2Sensitive");
  //    std::cout << "PndSttTubeIdMapCreatorRoot::CreateTubeMap " << nodeNames.size() << std::endl;
  for (auto name : nodeNames) {
    //        std::cout << "Prozessing Node: " << name.Data() << std::endl;
    int sector = GetSector(name);
    int row = GetRow(name);
    //        std::cout << "Sector: " << sector << " row " << row << std::endl;
    //        std::cout << "ShortID: " << PndGeoHandling::Instance()->GetShortID(name) << std::endl;
    result->AddTube(sector, row, PndGeoHandling::Instance()->GetShortID(name));
  }

  return result;
}

int PndSttTubeIdMapCreatorRoot::GetSector(TString &name)
{
  std::string nameString(name.Data());
  PndStringSeparator sep(nameString, "/-_");
  std::vector<std::string> stringVector = sep.GetStringVector();
  //    std::cout << "GetSector StringVector.size() " << stringVector.size() << std::endl;
  auto i = std::find_if(stringVector.begin(), stringVector.end(), [&](const std::string &val) {
    auto found = val.find("sector");
    return found != std::string::npos;
  });
  int pos = std::distance(stringVector.begin(), i);
  //    std::cout << "GetSector pos of sector " << pos << std::endl;
  int halfSide = std::stoi(stringVector[pos - 1]);
  int sectorId = std::stoi(stringVector[pos + 1]);
  // int sectorCpy = std::stoi(stringVector[pos+2]);

  int sector = -1;
  // int sector2 = halfSide*3 + sectorCpy*2 + sectorId;

  if (halfSide == 0 && sectorId == 0)
    sector = 0;
  else if (halfSide == 0 && sectorId == 1)
    sector = 1;
  else if (halfSide == 0 && sectorId == 2)
    sector = 2;
  else if (halfSide == 1 && sectorId == 0)
    sector = 3;
  else if (halfSide == 1 && sectorId == 1)
    sector = 4;
  else if (halfSide == 1 && sectorId == 2)
    sector = 5;

  //	std::cout << "halfSide = " << halfSide << " ,SectorID = " << sectorId
  //			<< " ,Sector = " << sector << std::endl;

  return sector;
}

int PndSttTubeIdMapCreatorRoot::GetRow(TString &name)
{
  std::string nameString(name.Data());
  PndStringSeparator sep(nameString, "/-_");
  std::vector<std::string> stringVector = sep.GetStringVector();
  auto i = std::find_if(stringVector.begin(), stringVector.end(), [&](const std::string &val) {
    auto found = val.find("strawLayer");
    return found != std::string::npos;
  });
  int pos = std::distance(stringVector.begin(), i);
  int strawLayer = std::stoi(stringVector[pos + 1]);
  //	std::cout << "strawLayer = " << strawLayer << std::endl;
  return strawLayer;
}
