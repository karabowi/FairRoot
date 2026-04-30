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

#ifndef PNDEMCSIMCRYSTALCALIBRATOR_HH
#define PNDEMCSIMCRYSTALCALIBRATOR_HH

//---------------------------------------------------------
// Description:
// 	File Calibrator for Emc Crystals.
//
// 	Christian Hammann
// 	31.7.2012

//---------------------------------------------------------

#include "PndEmcAbsCrystalCalibrator.h"
#include "TString.h"
#include <map>

/**
 * @brief Class to simulate a Calibration
 * @author Ch. Hammann <chammann@hiskp.uni-bonn.de>
 * @ingroup PndEmc
 **/
class PndEmcSimCrystalCalibrator : public PndEmcAbsCrystalCalibrator {
 public:
  PndEmcSimCrystalCalibrator(Int_t verbose = 0);

  /**
   * @brief Set Calibration for a Module
   *
   * @param Int_t  Module
   * @param Double_t  Height of 1GeV Pulse
   *
   * @return
   **/
  virtual void SetCalibration(Int_t ModId, Double_t cal, Int_t SignalNr = 1, Double_t overflow = -1);

  /**
   * @brief Creates a random gaussion distributed calibration for all crystals
   * 	Not implemented yet
   *
   * @param Double_t  relative spread of calibration values
   *
   * @return
   **/
  virtual void CreateRandomCalibration(Double_t Sigma);
  virtual Bool_t Init();

  virtual ~PndEmcSimCrystalCalibrator(){};
  virtual CalibrationStatus_t Calibrate(Double_t &Energy, Long_t DetId, Int_t SignalNr = 1);

 private:
  Double_t fRandomCal;
  Double_t fRandomSigma;
  std::vector<std::map<Int_t, std::pair<Double_t, Double_t>>> fModuleCalibrationMaps; // sig nr, modId, calib, overflow
  std::vector<std::map<Long_t, Double_t>> fCrystalCalibrationMaps;                    // sig nr, detId, rel. calib

  Int_t fVerbose;
  ClassDef(PndEmcSimCrystalCalibrator, 2);
};
#endif
