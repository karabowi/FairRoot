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
 * PndSdsFESimple.h
 *
 *  Created on: Apr 4, 2011
 *      Author: esch
 *
 *      Class to calculate ToT, TimeWalk, TimeStamp and Time till the signal is back to baseline from a TF1 amplifier model.
 *
 *
 */

#ifndef PndSdsFESIMPLE_H_
#define PndSdsFESIMPLE_H_
#include "TObject.h"
#include "PndSdsFEAmpModelSimple.h"
#include "TMath.h"
#include "TF1.h"
#include "TRandom.h"
#include "Math/Interpolator.h"
#include "Math/InterpolationTypes.h"
#include "PndSdsTotChargeConversion.h"
#include "TVectorT.h"
#include "PndSdsFE.h"

class PndSdsFESimple : public PndSdsFE {
 public:
  PndSdsFESimple();
  PndSdsFESimple(double charsingtime, double constcurrent, double threshold, double frequency);
  ~PndSdsFESimple();

  void SetParameter(double chargingtime, double constcurrent, double threshold, double frequency);

  ClassDef(PndSdsFESimple, 1);
};

#endif /* PndSdsFESIMPLE_H_ */
