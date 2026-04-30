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

#ifndef PNDSDSIDEALCHARGECONVERSION_H
#define PNDSDSIDEALCHARGECONVERSION_H

//! ideal charge conversion
//! @author D.-L.Pohl <d.pohl@fz-juelich.de>

#include "PndSdsChargeConversion.h"

class PndSdsIdealChargeConversion : public PndSdsChargeConversion {
 public:
  PndSdsIdealChargeConversion(Double_t noise = 0.) : PndSdsChargeConversion(kIdeal), fNoise(noise){};
  ~PndSdsIdealChargeConversion(){};
  virtual Double_t ChargeToDigiValue(Double_t charge) { return charge; };
  virtual Double_t DigiValueToCharge(Double_t digi) { return digi; };
  virtual Double_t GetRelativeError(Double_t charge) { return charge != 0 ? fNoise / charge : 0.; };
  virtual Double_t GetTimeStamp(Double_t time, Double_t, Double_t) { return (time - 1.); }; // charge  mceventtime //[R.K.03/2017] unused variable(s)

 protected:
  Double_t fNoise;

  ClassDef(PndSdsIdealChargeConversion, 1);
};
#endif /* PNDSDSIDEALCHARGECONVERSION_H */
