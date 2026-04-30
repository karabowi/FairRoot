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
// -----                     PndGemStation header file                 -----
// -----                  Created 12/02/2009  by R. Karabowicz         -----
// -------------------------------------------------------------------------

/** PndGemStation.h
 *@author R.Karabowicz <r.karabowicz@gsi.de>
 **
 ** This class holds the transport geometry parameters
 ** of one GEM tracking station necessary for digitisation.
 **/

#ifndef PNDGEMSTATION_H
#define PNDGEMSTATION_H 1

#include "TObjArray.h"
#include "TNamed.h"
#include "TString.h"
#include "PndDetectorList.h"

#include <map>

class PndGemSensor;

class PndGemStation : public TNamed {

 public:
  /** Default constructor **/
  PndGemStation();

  /** Standard constructor
   *@param name      Volume name of station
   *@param stationNr stationNr
   *@param z         z position of station centre [cm]
   *@param rotation  Rotation angle in global c.s. [rad]
   **/
  PndGemStation(const char *name, Int_t stationNr, Double_t z, Double_t rotation);

  /** Destructor **/
  virtual ~PndGemStation();

  /** Accessors **/

  void SetDetectorId(Int_t stationNr) { fDetectorId = static_cast<int>(PndDetectorId::kGEM) << 27 | 0 << 21 | stationNr << 8 | 0 << 6; }
  Int_t GetDetectorId() const { return fDetectorId; }
  Int_t GetSystemId() const { return ((fDetectorId & (31 << 27)) >> 27); }
  Int_t GetStationNr() const { return ((fDetectorId & (8191 << 8)) >> 8); }
  Double_t GetZ(Int_t it = 0);
  Int_t GetNSensors() const { return fSensors->GetEntriesFast(); }
  Int_t GetNChannels();
  TObjArray *GetSensors() { return fSensors; }
  PndGemSensor *GetSensor(Int_t iSensor) { return (PndGemSensor *)fSensors->At(iSensor); }
  PndGemSensor *GetSensorByNr(Int_t sensorNr);

  /** Add one sensor to the array **/
  void AddSensor(PndGemSensor *sensor);

  Int_t GetNofZ();

  /** Reset all eventwise counters **/
  void Reset();

  /** Output to screen **/
  virtual void Print(Bool_t kLong);

 protected:
  Int_t fDetectorId;                 // Unique detector ID
  Double32_t fZ;                     // z position of station centre [cm]
  Double32_t fRotation;              // Rotation angle in global c.s [rad]
  TObjArray *fSensors;               // Array of PndGemSensors
  std::map<Int_t, Int_t> fSensorMap; //! Map from sensor number to index
  std::map<Int_t, Int_t> fMcIdMap;   //! Map from McId to index

  Double_t fSensorZ[10];

  ClassDef(PndGemStation, 1);
};

#endif
