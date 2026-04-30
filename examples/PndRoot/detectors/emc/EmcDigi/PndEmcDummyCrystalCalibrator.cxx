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

//---------------------------------------------------------
// Description:
// 	Dummy CrystalCalibrator for Emc Crystals.
//
// 	Christian Hammann
// 	31.7.2012

//---------------------------------------------------------

#include "PndEmcDummyCrystalCalibrator.h"

PndEmcAbsCrystalCalibrator::CalibrationStatus_t PndEmcDummyCrystalCalibrator::Calibrate(Double_t &, Long_t, Int_t)
{ // Energy detId SignalNr //[R.K.03/2017] unused variable(s)
  return kCALOK;
}
ClassImp(PndEmcDummyCrystalCalibrator);
