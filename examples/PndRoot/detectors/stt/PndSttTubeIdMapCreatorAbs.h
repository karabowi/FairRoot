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
 * PndSttTubeIdMapCreatorAbs.h
 *
 *  Created on: 02.08.2018
 *      Author: tstockmanns
 */

#ifndef DETECTORS_STT_PNDSTTTUBEIDMAPCREATORABS_H_
#define DETECTORS_STT_PNDSTTTUBEIDMAPCREATORABS_H_

#include "TObject.h"

class PndSttTubeIdMap;

class PndSttTubeIdMapCreatorAbs : public TObject {
 public:
  PndSttTubeIdMapCreatorAbs();
  virtual ~PndSttTubeIdMapCreatorAbs();

  virtual PndSttTubeIdMap *CreateTubeMap() = 0;

  ClassDef(PndSttTubeIdMapCreatorAbs, 1);
};

#endif /* DETECTORS_STT_PNDSTTTUBEIDMAPCREATORABS_H_ */
