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

#ifndef PndMQSdsPixelBackMapping_H
#define PndMQSdsPixelBackMapping_H

#include "PndSdsDigiPixel.h"
#include "PndSdsHit.h"
#include "PndGeoHandling.h"
#include <vector>

class PndMQSdsPixelBackMapping {
 public:
  PndMQSdsPixelBackMapping();
  PndMQSdsPixelBackMapping(PndGeoHandling *geo);
  virtual ~PndMQSdsPixelBackMapping();

  void SetVerbose(Int_t level) { fVerbose = level; };
  virtual PndSdsHit GetCluster(std::vector<PndSdsDigiPixel *> &pixelArray) = 0;

  virtual void SetGeoHandling(PndGeoHandling *geo) { fGeoH = geo; }

  PndGeoHandling *fGeoH;
  Int_t fVerbose;
};

#endif
