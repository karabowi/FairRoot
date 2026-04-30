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
 * PndMQDummyTask.h
 *
 *  Created on: 15.10.2015
 *      Author: Stockmanns
 */

#ifndef MVD_MVDOFFLINETBANALYSIS_TOPIX4_MQ_PNDMQTOPIXHITPRODUCER_H_
#define MVD_MVDOFFLINETBANALYSIS_TOPIX4_MQ_PNDMQTOPIXHITPRODUCER_H_

#include <PndSdsDigiTopix4.h>
#include "PndSdsHit.h"
#include "TGeoMatrix.h"
#include "TVector3.h"
#include <vector>

class PndMQTopixHitProducer {
 public:
  PndMQTopixHitProducer();
  PndMQTopixHitProducer(double dimX, double dimY, int maxcol, int maxrow);
  virtual ~PndMQTopixHitProducer();
  PndSdsHit GetHit(std::vector<PndSdsDigiTopix4> pixelArray);

 protected:
  TGeoHMatrix GetTransformation(Int_t sensorID);
  TVector3 GetSensorDimensions(Int_t sensorID);

 private:
  double fx, fy;
  int fcol, frow;
};

#endif /* MVD_MVDOFFLINETBANALYSIS_TOPIX4_MQ_PNDMQTOPIXHITPRODUCER_H_ */
