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
 * PndSttTubeMapCreatorRoot.h
 *
 *
 *  Created on: 02.08.2018
 *      Author: tstockmanns
 */

#ifndef DETECTORS_STT_PndSttTubeMapCreatorRoot_H_
#define DETECTORS_STT_PndSttTubeMapCreatorRoot_H_

#include "PndSttTubeMap.h"
#include <PndSttTubeMapCreatorAbs.h>

class PndSttTubeMapCreatorRoot : public PndSttTubeMapCreatorAbs {
 public:
  PndSttTubeMapCreatorRoot();
  virtual ~PndSttTubeMapCreatorRoot();
  bool IsEdgeStraw(int tubeId) const;
  int IsSectorBorderStraw(int tubeId) const;
  PndSttTubeMap *CreateTubeMap();

 protected:
  void AssignNeighbors(PndSttTube *tube);

  int sRowTubeID[6][29];             //[sector][row]
  int eRowTubeID[6][29];             //[sector][row]
  int additionalSkewedTubeMap[4576]; // check for additional tube in skewed layers
  ClassDef(PndSttTubeMapCreatorRoot, 1);
};

#endif /* DETECTORS_STT_PndSttTubeMapCreatorRoot_H_ */
