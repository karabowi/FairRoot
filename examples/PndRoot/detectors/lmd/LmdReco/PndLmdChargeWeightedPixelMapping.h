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

#ifndef PNDLMDCHARGEWEIGHTEDPIXELMAPPING_H
#define PNDLMDCHARGEWEIGHTEDPIXELMAPPING_H

#include "PndSdsChargeWeightedPixelMapping.h"

//! PndMvdChargeWeightedPixelMapping.h
//! @author T.Stockmanns <t.stockmanns@fz-juelich.de>
//!
//! \brief PndSdsChargedWeightedPixelMapping: Gets a vector of DigiHits and
//! calculates the cluster center weighted with the charge

class PndLmdChargeWeightedPixelMapping : public PndSdsChargeWeightedPixelMapping {
 public:
  PndLmdChargeWeightedPixelMapping(Int_t verbose = 0);
  PndLmdChargeWeightedPixelMapping(PndGeoHandling *geo, Int_t verbose = 0);
};

#endif
