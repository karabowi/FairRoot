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

// -------------------------------------------------------------------------
// -----                     PndGemSensorMonitor header file           -----
// -----                  Created 12/02/2009  by R. Karabowicz         -----
// -------------------------------------------------------------------------

/** PndGemSensor.h
 *@author Radoslaw Karabowicz <r.karabowicz@gsi.de>
 *@since 25/03/2013
 *@version 1.0
 **
 ** This class monitors the digis in the sensor, on front and back.
 ** It derives from PndGemSensor, so that we have information about
 ** number of digis, positions and so on.
 **/

#ifndef PNDGEMSENSORMONITOR_H
#define PNDGEMSENSORMONITOR_H 1

#include "TNamed.h"
#include "PndDetectorList.h"
#include "PndGemSensor.h"

#include <map>
#include <list>
#include <set>
#include <vector>

class PndGemSensorMonitor : public PndGemSensor {

 public:
  /** Default constructor **/
  PndGemSensorMonitor();

  /** Standard constructor
   *@param fName     Unique sensor name
   *@param detId     Unique detector identifier
   *@param iType     Sensor type (1,2,3)
   *@param pos[]     sensor centre coordinate in global c.s [cm]
   *@param rotation  rotation in global c.s. [rad]
   *@param innerRadius     sensor inner radius in [cm]
   *@param outerRadius     sensor outer radius in [cm]
   *@param d         sensor thickness in [cm]
   *@param stripAngle[]  strip angle, if 0  - radial strips, measuring theta,
                                      if 60 - modified radial strips, joining inner ring with outer rotated by 60 deg.
                                      if 90 - concentric strips, measuring radius
   *@param pitch[]   readout radial/angle strip pitch or pixel width in x/y [cm]
   **/
  PndGemSensorMonitor(TString tempName, Int_t detId, Int_t iType, Double_t x0, Double_t y0, Double_t z0, Double_t rotation, Double_t innerRad, Double_t outerRad, Double_t d,
                      Double_t stripAngle0, Double_t stripAngle1, Double_t pitch0, Double_t pitch1);
  PndGemSensorMonitor(TString tempName, Int_t stationNr, Int_t sectorNr, Int_t iType, Double_t x0, Double_t y0, Double_t z0, Double_t rotation, Double_t innerRad,
                      Double_t outerRad, Double_t d, Double_t stripAngle0, Double_t stripAngle1, Double_t pitch0, Double_t pitch1);
  PndGemSensorMonitor(const PndGemSensor &tempSensor);

  /** Destructor **/
  virtual ~PndGemSensorMonitor();

  /** Modifiers **/

  // Make a channel active
  void ActivateChannel(Int_t channelNr, Int_t sideId, Int_t eventNr, Int_t digiNr, Double_t channelTime, Double_t channelCharge);

  void CheckNeighbours(Int_t channelNr, Int_t sideId, Int_t eventNr, Int_t digiNr, Double_t channelTime, Double_t channelCharge);

  /** Accessors **/

  // Return kTRUE if channel was activated no more than fSensorRecoveryTime nanoseconds before
  Bool_t ChannelIsActive(Int_t channelNr, Int_t sideId, Double_t timeNow);

  // Return number of seconds between now and last activation
  Double_t ChannelLastActiveAt(Int_t channelNr, Int_t sideId, Double_t timeNow, Int_t eventNr = -1);

  // Return last activation time
  Double_t ChannelLastActiveAt(Int_t channelNr, Int_t sideId);

  // Return last activation data
  Bool_t ChannelLastActivation(Int_t channelNr, Int_t sideId, Int_t &eventNr, Int_t &digiNr, Double_t &channelTime, Double_t &channelCharge);

  /** Screen output  **/
  void Print();

 private:
  /** -------------   Data members   --------------------------**/

  // LA: LastActivation
  // Front
  std::vector<Double_t> fFLATime;
  std::vector<Double_t> fFLACharge;
  std::vector<Int_t> fFLADigiNr;
  std::vector<Int_t> fFLAEventNr;
  std::vector<Double_t> fFMeanTimeD;
  std::vector<Int_t> fFNofDigis;
  Int_t fFSLChan;
  // Back
  std::vector<Double_t> fBLATime;
  std::vector<Double_t> fBLACharge;
  std::vector<Int_t> fBLADigiNr;
  std::vector<Int_t> fBLAEventNr;
  std::vector<Double_t> fBMeanTimeD;
  std::vector<Int_t> fBNofDigis;
  Int_t fBSLChan;

  Double_t fSensorRecoveryTime;
  Double_t fSensorFirstTime;
  Double_t fSensorLastTime;

  ClassDef(PndGemSensorMonitor, 1);
};

#endif
