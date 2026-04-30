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
// -----                     PndGemStation source file                 -----
// -----                  Created 12/02/2009  by R. Karabowicz         -----
// -------------------------------------------------------------------------

#include "PndGemStation.h"

#include "PndGemSensor.h"
#include "FairLogger.h"

#include "TMath.h"

#include <iostream>
#include <map>

using std::cout;
using std::endl;
using std::flush;
using std::map;

// -----   Default constructor   -------------------------------------------
PndGemStation::PndGemStation()
{
  fDetectorId = 0;
  fZ = 0.;
  fSensors = new TObjArray(100);
  fSensorMap.clear();
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndGemStation::PndGemStation(const char *name, Int_t stationNr, Double_t z, Double_t rotation) : TNamed(name, "GEM station")
{
  SetDetectorId(stationNr);
  fZ = z;
  fRotation = rotation;
  fSensors = new TObjArray(100);
  fSensorMap.clear();

  for (Int_t isz = 0; isz < 10; isz++)
    fSensorZ[isz] = -666.;
  fSensorZ[0] = z;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndGemStation::~PndGemStation()
{
  if (fSensors) {
    fSensors->Delete();
    delete fSensors;
  }
}
// -------------------------------------------------------------------------

// -----   Public method GetNChannels   ------------------------------------
Int_t PndGemStation::GetNChannels()
{
  Int_t nChan = 0;
  for (Int_t iSect = 0; iSect < GetNSensors(); iSect++)
    nChan += GetSensor(iSect)->GetNChannels();
  return nChan;
}
// -------------------------------------------------------------------------

// -----   Public method GetSensorByNr   -----------------------------------
PndGemSensor *PndGemStation::GetSensorByNr(Int_t sensorNr)
{
  if (fSensorMap.find(sensorNr) != fSensorMap.end()) {
    Int_t index = fSensorMap[sensorNr];
    return (PndGemSensor *)fSensors->At(index);
  } else {
    LOG(warn) << " PndGemStation::GetSensorByNr: sensor " << sensorNr << " not found (station " << GetStationNr() << ").";
    return nullptr;
  }
}
// -------------------------------------------------------------------------

// -----   Public method AddSensor   ---------------------------------------
void PndGemStation::AddSensor(PndGemSensor *sensor)
{

  Int_t iSensor = sensor->GetSensorNr();
  Int_t nSensors = fSensors->GetEntries();

  fSensors->Add(sensor);

  fSensorMap[iSensor] = nSensors;
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
Double_t PndGemStation::GetZ(Int_t it)
{
  if (fSensorZ[it] < -665.) {
    Int_t knownZPos = 0;
    for (knownZPos = 0; knownZPos < 10; knownZPos++) {
      if (fSensorZ[knownZPos] < -665.)
        break;
    }

    for (Int_t iSect = GetNSensors(); iSect > 0; iSect--) {
      PndGemSensor *sensor = (PndGemSensor *)GetSensor(iSect - 1);
      Bool_t knownAlready = kFALSE;
      for (Int_t isz = 0; isz < knownZPos; isz++)
        if (TMath::Abs(sensor->GetZ0() - fSensorZ[isz]) < 0.0001) {
          knownAlready = kTRUE;
          break;
        }

      if (knownAlready)
        continue;

      fSensorZ[knownZPos] = sensor->GetZ0();
      knownZPos++;
    }
  }
  return fSensorZ[it];
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
Int_t PndGemStation::GetNofZ()
{
  Int_t knownZPos = 0;
  for (knownZPos = 0; knownZPos < 10; knownZPos++) {
    if (fSensorZ[knownZPos] < -665.)
      break;
  }

  for (Int_t iSect = GetNSensors(); iSect > 0; iSect--) {
    PndGemSensor *sensor = (PndGemSensor *)GetSensor(iSect - 1);
    Bool_t knownAlready = kFALSE;
    for (Int_t isz = 0; isz < knownZPos; isz++)
      if (TMath::Abs(sensor->GetZ0() - fSensorZ[isz]) < 0.0001) {
        knownAlready = kTRUE;
        break;
      }

    if (knownAlready)
      continue;

    fSensorZ[knownZPos] = sensor->GetZ0();
    knownZPos++;
  }
  return knownZPos;
}
// -------------------------------------------------------------------------

// -----   Public method Reset   -------------------------------------------
void PndGemStation::Reset()
{
  for (Int_t iSensor = 0; iSensor < GetNSensors(); iSensor++)
    GetSensor(iSensor)->Reset();
}
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void PndGemStation::Print(Bool_t kLong)
{
  cout << "Station Nr. ";
  cout.width(2);
  cout << GetStationNr() << ", z =  ";
  cout.width(3);
  cout << fZ << " cm, angle = ";
  cout.width(3);
  cout << fRotation * 180 / TMath::Pi() << " deg., sensors: ";
  cout.width(4);
  cout << GetNSensors() << ", channels: ";
  cout.width(8);
  cout << GetNChannels() << endl;

  Int_t tempSId = fDetectorId;
  Int_t bc = 0;
  cout << "STATION:                                          " << flush;
  while (tempSId > 0) {
    bc++;
    cout << "\b" << tempSId % 2 << "\b" << flush;
    if (bc == 27 || bc == 21 || bc == 8 || bc == 6 || bc == 5)
      cout << "\b|\b" << flush;
    tempSId = tempSId / 2;
  }
  cout << endl;

  if (kLong)
    for (Int_t iSec = 0; iSec < GetNSensors(); iSec++)
      GetSensor(iSec)->Print();
}
// -------------------------------------------------------------------------

ClassImp(PndGemStation)
