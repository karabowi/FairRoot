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

#ifndef PNDSDSPIXELBACKMAPPING_H
#define PNDSDSPIXELBACKMAPPING_H

#include "PndSdsDigiPixel.h"
#include "PndSdsHit.h"
#include "PndGeoHandling.h"
#include <vector>

class PndSdsPixelBackMapping {
 public:
  PndSdsPixelBackMapping();
  PndSdsPixelBackMapping(PndGeoHandling *geo);
  PndSdsPixelBackMapping(const PndSdsPixelBackMapping &other) : fDigiArray(other.fDigiArray), fGeoH(other.fGeoH), fVerbose(other.fVerbose){};
  PndSdsPixelBackMapping &operator=(PndSdsPixelBackMapping &other)
  {
    fDigiArray = other.fDigiArray;
    fGeoH = other.fGeoH;
    fVerbose = other.fVerbose;
    return *this;
  };
  virtual ~PndSdsPixelBackMapping();

  void SetVerbose(Int_t level) { fVerbose = level; };
  virtual PndSdsHit GetCluster(std::vector<PndSdsDigiPixel> pixelArray) = 0;

  std::vector<PndSdsDigiPixel> fDigiArray;
  PndGeoHandling *fGeoH;
  Int_t fVerbose;
};

#endif
