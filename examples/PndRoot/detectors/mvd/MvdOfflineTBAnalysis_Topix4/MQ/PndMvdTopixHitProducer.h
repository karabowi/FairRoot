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

#ifndef PNDMVDTOPIXHITPRODUCER_H
#define PNDMVDTOPIXHITPRODUCER_H

#include "PndSdsChargeConversion.h"
#include "TGeoMatrix.h"
#include "TVector3.h"

#include "PndSdsHit.h"
#include "PndSdsDigiTopix4.h"
//! PndMvdTopixHitProducer.h
//! @author T.Stockmanns <t.stockmanns@fz-juelich.de>
//!
//! \brief PndSdsChargedWeightedPixelMapping: Gets a vector of DigiHits and calculates the cluster center weighted with the charge
//! params: Number of columns in FE, Number of rows in FE, Pixel dimension in x, Pixel dimension in y
//! params are taken from parameter database

class PndMvdTopixHitProducer {
 public:
  PndMvdTopixHitProducer();
  PndMvdTopixHitProducer(double dimX, double dimY, int maxcol, int maxrow);
  virtual ~PndMvdTopixHitProducer(){};
  PndSdsHit GetHit(std::vector<PndSdsDigiTopix4> pixelArray); ///< Main function of class to calculate the PndSdsHit out of the given PndSdsDigis

 protected:
  Double_t flx;
  Double_t fly;
  Int_t fcols;
  Int_t frows;
  Int_t fVerbose;

 private:
  virtual TGeoHMatrix GetTransformation(Int_t sensorID);
  virtual TVector3 GetSensorDimensions(Int_t sensorID);
  std::vector<PndSdsDigiTopix4> fDigiArray;
};

#endif
