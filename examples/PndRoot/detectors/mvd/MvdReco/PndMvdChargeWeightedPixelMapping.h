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

#ifndef PNDMVDCHARGEWEIGHTEDPIXELMAPPING_H
#define PNDMVDCHARGEWEIGHTEDPIXELMAPPING_H

#include "PndSdsChargeWeightedPixelMapping.h"
#include "PndSdsPixelDigiPar.h"
#include "PndSdsTotDigiPar.h"

//! PndMvdChargeWeightedPixelMapping.h
//! @author T.Stockmanns <t.stockmanns@fz-juelich.de>
//!
//! \brief PndSdsChargedWeightedPixelMapping: Gets a vector of DigiHits and calculates the cluster center weighted with the charge

class PndMvdChargeWeightedPixelMapping : public PndSdsChargeWeightedPixelMapping {
 public:
  PndMvdChargeWeightedPixelMapping(Int_t verbose = 0);
  PndMvdChargeWeightedPixelMapping(PndGeoHandling *geo, Int_t verbose = 0);
  PndMvdChargeWeightedPixelMapping(PndGeoHandling *geo, PndSdsPixelDigiPar *digiPar, PndSdsTotDigiPar *totPar);
  PndMvdChargeWeightedPixelMapping(const PndMvdChargeWeightedPixelMapping &) = delete;
  PndMvdChargeWeightedPixelMapping &operator=(const PndMvdChargeWeightedPixelMapping &) = delete;

  void SetChargeConverter();
  PndSdsPixelDigiPar *fDigiPar;
  PndSdsTotDigiPar *fTotDigiPar;
};

#endif
