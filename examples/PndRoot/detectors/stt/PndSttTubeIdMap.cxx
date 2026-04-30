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
 * PndSttTubeIdMap.cxx
 *
 *  Created on: 02.08.2018
 *      Author: tstockmanns
 */

#include "PndSttTubeIdMap.h"
#include <utility>
#include <iostream>

ClassImp(PndSttTubeIdMap);

PndSttTubeIdMap *PndSttTubeIdMap::fInstance = nullptr;

PndSttTubeIdMap::PndSttTubeIdMap()
{
  fSkewedRows.push_back(10);
  fSkewedRows.push_back(11);
  fSkewedRows.push_back(12);
  fSkewedRows.push_back(13);
  fSkewedRows.push_back(14);
  fSkewedRows.push_back(15);
  fSkewedRows.push_back(16);
  fSkewedRows.push_back(17);
}

PndSttTubeIdMap::~PndSttTubeIdMap()
{
  // TODO Auto-generated destructor stub
}

PndSttTubeIdMap *PndSttTubeIdMap::Instance()
{
  if (!fInstance) {
    fInstance = new PndSttTubeIdMap();
  }
  return fInstance;
}

void PndSttTubeIdMap::AddTube(int sector, int row, int tubeId)
{
  if (!(sector < fTubeMap.size())) {
    fTubeMap.resize(sector + 1);
  }
  if (!(row < fTubeMap[sector].size())) {
    fTubeMap[sector].resize(row + 1);
  }
  fTubeMap[sector][row].push_back(tubeId);
  fMapTubeIdSectorRow[tubeId] = std::make_pair(sector, row);
  fFlatTubeMap.push_back(tubeId);
}
