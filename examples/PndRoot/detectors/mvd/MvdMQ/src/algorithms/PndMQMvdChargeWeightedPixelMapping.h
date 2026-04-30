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

#ifndef PndMQMvdChargeWeightedPixelMapping_H
#define PndMQMvdChargeWeightedPixelMapping_H

#include "PndMQSdsChargeWeightedPixelMapping.h"
#include "PndSdsPixelDigiPar.h"
#include "PndSdsTotDigiPar.h"

//! PndMQMvdChargeWeightedPixelMapping.h
//! @author T.Stockmanns <t.stockmanns@fz-juelich.de>
//!
//! \brief PndSdsChargedWeightedPixelMapping: Gets a vector of DigiHits and calculates the cluster center weighted with the charge

class PndMQMvdChargeWeightedPixelMapping : public PndMQSdsChargeWeightedPixelMapping {
 public:
  PndMQMvdChargeWeightedPixelMapping(Int_t verbose = 0);
  PndMQMvdChargeWeightedPixelMapping(PndGeoHandling *geo, Int_t verbose = 0);
  PndMQMvdChargeWeightedPixelMapping(PndGeoHandling *geo, PndSdsPixelDigiPar *digiPar, PndSdsTotDigiPar *totPar);

  virtual void SetDigiPar(PndSdsPixelDigiPar *par) { fDigiPar = par; }
  virtual void SetTotPar(PndSdsTotDigiPar *par) { fTotDigiPar = par; }

  void SetChargeConverter();
  PndSdsPixelDigiPar *fDigiPar;
  PndSdsTotDigiPar *fTotDigiPar;
};

#endif
