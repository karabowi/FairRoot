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

#pragma once

#include "TClonesArray.h"
#include "TVector3.h"
#include <vector>
#include <iostream>

#include "PndSttHit.h"
#include "PndPreselectSttHits.h"
#include "PndSttCAData.h"
#include "PndSttStrawMap.h"

class PndStt2GeoHandler;

class PndSegmentationPreselectSttHits : public PndPreselectSttHits {
 public:
  PndSegmentationPreselectSttHits(){};

  virtual ~PndSegmentationPreselectSttHits(){};

  std::vector<std::vector<PndSttHit *>> PreselectSttHits();

  void Init(TClonesArray *sttHits, PndStt2GeoHandler *geoH = nullptr);
  //void Init(TClonesArray *sttHits, TClonesArray *tubeArray = nullptr);
  void SetCombinedSkewed(TClonesArray *combinedskewed) { fCombinedSkewedHitArray = combinedskewed; }

 private:
  TClonesArray *fCombinedSkewedHitArray = nullptr;
  //PndSttStrawMap *fStrawMap;
  PndStt2GeoHandler *fSttGeoH;
};
