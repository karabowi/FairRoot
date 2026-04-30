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
 * PndSttTubeCreator.cxx
 *
 *  Created on: 02.08.2018
 *      Author: tstockmanns
 */

#include "PndSttTubeCreator.h"
#include <PndSttTubeIdMap.h>
#include <PndGeoHandling.h>

ClassImp(PndSttTubeCreator);

PndSttTubeCreator::PndSttTubeCreator()
{
  // TODO Auto-generated constructor stub
}

PndSttTubeCreator::~PndSttTubeCreator()
{
  // TODO Auto-generated destructor stub
}

PndSttTube *PndSttTubeCreator::GetTube(int tubeId)
{
  PndGeoHandling *geoH = PndGeoHandling::Instance();

  PndSttTubeParameters *params = new PndSttTubeParameters(tubeId, geoH->GetSensorDimensionsShortId(tubeId).Z());
  TGeoHMatrix *matrix = geoH->GetMatrixShortId(tubeId);

  Double_t const *translation = matrix->GetTranslation();
  Double_t const *rotation = matrix->GetRotationMatrix();

  double r[3][3];
  int irot = 0, i = 0, j = 0;
  for (i = 0; i < 3; i++) {
    for (j = 0; j < 3; j++) {
      r[i][j] = rotation[irot];
      irot++;
    }
  }

  PndSttTube *theTube = new PndSttTube(params, translation[0], translation[1], translation[2], r[0][0], r[0][1], r[0][2], r[1][0], r[1][1], r[1][2], r[2][0], r[2][1], r[2][2],
                                       geoH->GetSensorDimensionsShortId(tubeId).X(), geoH->GetSensorDimensionsShortId(tubeId).X() + 0.001);

  std::pair<int, int> SectorRow = PndSttTubeIdMap::Instance()->GetSectorRowFromTubeId(tubeId);
  theTube->SetSectorID(SectorRow.first);
  theTube->SetLayerID(SectorRow.second);

  // std::cout << "-I- PndSttTubeCreator::GetTube " << tubeId << " : " << SectorRow.first << "/" << SectorRow.second << std::endl;

  return theTube;
}
