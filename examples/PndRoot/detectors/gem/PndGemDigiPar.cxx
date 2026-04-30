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

//* $Id: */

// -------------------------------------------------------------------------
// -----                    PndGemDigiPar source file                 -----
// -----                  Created 12/02/2009 by R. Karabowicz          -----
// -------------------------------------------------------------------------

/** PndGemDigiPar
 *@author Radoslaw Karabowicz <r.karabowicz@gsi.de>
 *@since 12.02.2009
 *@version 1.0
 **
 ** PANDA task class for storing digitization parameters for GEM
 ** Task level RECO
 **/

#include <iostream>
#include "PndGemDigiPar.h"
#include "PndDetectorList.h"

using std::cout;
using std::endl;
using std::flush;
using std::map;
using std::pair;

PndGemDigiPar::PndGemDigiPar(const char *name, const char *title, const char *context)
  : FairParGenericSet(name, title, context), fGemParameters(TArrayD()), fStations(new TObjArray(10)), fNSensors(0), fNChannels(0), fTrackFinderOnHits_ParThetaA(0.),
    fTrackFinderOnHits_ParThetaB(0.), fTrackFinderOnHits_ParTheta0(0.), fTrackFinderOnHits_ParTheta1(0.), fTrackFinderOnHits_ParTheta2(0.), fTrackFinderOnHits_ParTheta3(0.),
    fTrackFinderOnHits_ParRadPhi0(0.), fTrackFinderOnHits_ParRadPhi2(0.), fTrackFinderOnHits_ParMat0(TArrayD()), fTrackFinderOnHits_ParMat1(TArrayD())

{
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndGemDigiPar::~PndGemDigiPar(void)
{
  if (fStations) {
    fStations->Delete();
    delete fStations;
  }
}
// -------------------------------------------------------------------------

void PndGemDigiPar::putParams(FairParamList *list)
{
  if (!list)
    return;
  list->add("parameters", fGemParameters);

  list->add("TrackFinderOnHits_ParThetaA", fTrackFinderOnHits_ParThetaA);
  list->add("TrackFinderOnHits_ParThetaB", fTrackFinderOnHits_ParThetaB);

  list->add("TrackFinderOnHits_ParTheta0", fTrackFinderOnHits_ParTheta0);
  list->add("TrackFinderOnHits_ParTheta1", fTrackFinderOnHits_ParTheta1);
  list->add("TrackFinderOnHits_ParTheta2", fTrackFinderOnHits_ParTheta2);
  list->add("TrackFinderOnHits_ParTheta3", fTrackFinderOnHits_ParTheta3);

  list->add("TrackFinderOnHits_ParRadPhi0", fTrackFinderOnHits_ParRadPhi0);
  list->add("TrackFinderOnHits_ParRadPhi2", fTrackFinderOnHits_ParRadPhi2);

  list->add("TrackFinderOnHits_ParMat0", fTrackFinderOnHits_ParMat0);
  list->add("TrackFinderOnHits_ParMat1", fTrackFinderOnHits_ParMat1);
}

Bool_t PndGemDigiPar::getParams(FairParamList *list)
{
  if (!list) {
    std::cout << "list not found" << std::endl;
    return kFALSE;
  }
  if (!list->fill("parameters", &fGemParameters)) {
    std::cout << "par: fGemParameters not found" << std::endl;
    return kFALSE;
  }
  if (!list->fill("TrackFinderOnHits_ParThetaA", &fTrackFinderOnHits_ParThetaA)) {
    std::cout << "par: fTrackFinderOnHits_ParThetaA not found" << std::endl;
    return kFALSE;
  }
  if (!list->fill("TrackFinderOnHits_ParThetaB", &fTrackFinderOnHits_ParThetaB)) {
    std::cout << "par: fTrackFinderOnHits_ParThetaB not found" << std::endl;
    return kFALSE;
  }

  if (!list->fill("TrackFinderOnHits_ParTheta0", &fTrackFinderOnHits_ParTheta0)) {
    std::cout << "par: fTrackFinderOnHits_ParTheta0 not found" << std::endl;
    return kFALSE;
  }
  if (!list->fill("TrackFinderOnHits_ParTheta1", &fTrackFinderOnHits_ParTheta1)) {
    std::cout << "par: fTrackFinderOnHits_ParTheta1 not found" << std::endl;
    return kFALSE;
  }
  if (!list->fill("TrackFinderOnHits_ParTheta2", &fTrackFinderOnHits_ParTheta2)) {
    std::cout << "par: fTrackFinderOnHits_ParTheta2 not found" << std::endl;
    return kFALSE;
  }
  if (!list->fill("TrackFinderOnHits_ParTheta3", &fTrackFinderOnHits_ParTheta3)) {
    std::cout << "par: fTrackFinderOnHits_ParTheta3 not found" << std::endl;
    return kFALSE;
  }

  if (!list->fill("TrackFinderOnHits_ParRadPhi0", &fTrackFinderOnHits_ParRadPhi0)) {
    std::cout << "par: fTrackFinderOnHits_ParRadPhi0 not found" << std::endl;
    return kFALSE;
  }
  if (!list->fill("TrackFinderOnHits_ParRadPhi2", &fTrackFinderOnHits_ParRadPhi2)) {
    std::cout << "par: fTrackFinderOnHits_ParRadPhi2 not found" << std::endl;
    return kFALSE;
  }

  if (!list->fill("TrackFinderOnHits_ParMat0", &fTrackFinderOnHits_ParMat0)) {
    std::cout << "par: fTrackFinderOnHits_ParMat0 not found" << std::endl;
    return kFALSE;
  }
  if (!list->fill("TrackFinderOnHits_ParMat1", &fTrackFinderOnHits_ParMat1)) {
    std::cout << "par: fTrackFinderOnHits_ParMat1 not found" << std::endl;
    return kFALSE;
  }

  CreateStations();

  return kTRUE;
}

void PndGemDigiPar::CreateStations()
{
  if (GetNStations() > 0)
    return;
  Int_t arrayIndex = 0;

  fNSensors = 0;
  fNChannels = 0;

  Int_t stationNr = 0;
  TString stationName = "";
  Int_t nofSensors = 0;
  Int_t sensorNr = 0;
  TString sensorName = "";
  PndGemStation *station = nullptr;
  PndGemSensor *sensor = nullptr;

  while (arrayIndex < fGemParameters.GetSize()) {
    stationNr = (Int_t)fGemParameters[arrayIndex + 0];
    stationName = Form("Gem_Station_%d", stationNr);

    station = new PndGemStation(stationName.Data(), stationNr, fGemParameters[arrayIndex + 1], TMath::Pi() * fGemParameters[arrayIndex + 2] / 180.);

    fStations->Add(station);
    fStationMap[stationNr] = station;

    arrayIndex += 4;

    nofSensors = (Int_t)fGemParameters[arrayIndex - 1];

    for (Int_t isec = 0; isec < nofSensors; isec++) {
      sensorNr = (Int_t)fGemParameters[arrayIndex + 0];
      //      sensorName = Form("Gem_Disk%d_Gem%s_Sensor_kapton",stationNr,(sensorNr==1?"1":"6"));
      sensorName = Form("Gem_Disk%d_Gem%s_Sensor_GEMmixture", stationNr, (sensorNr == 1 ? "1" : "6"));

      sensor = new PndGemSensor(sensorName.Data(), stationNr, sensorNr, (Int_t)fGemParameters[arrayIndex + 1], fGemParameters[arrayIndex + 2], fGemParameters[arrayIndex + 3],
                                fGemParameters[arrayIndex + 4], -TMath::Pi() * fGemParameters[arrayIndex + 5] / 180., fGemParameters[arrayIndex + 6],
                                fGemParameters[arrayIndex + 7], fGemParameters[arrayIndex + 8], fGemParameters[arrayIndex + 9], fGemParameters[arrayIndex + 10],
                                fGemParameters[arrayIndex + 11], fGemParameters[arrayIndex + 12]);

      station->AddSensor(sensor);

      fNSensors++;
      fNChannels += sensor->GetNChannels();

      // put sensor into name/sensor map
      map<TString, PndGemSensor *>::iterator p;
      p = fSensorByName.find(sensorName.Data());
      if (p != fSensorByName.end()) {
        cout << " -E- Sensor \"" << sensorName.Data() << "\" is already inserted " << endl;
      } else {
        fSensorByName.insert(pair<TString, PndGemSensor *>(sensorName.Data(), sensor));
      }
      arrayIndex += 13;
    }
  }
}

// -----   Public method GetStation   --------------------------------------
PndGemStation *PndGemDigiPar::GetStation(Int_t iStation)
{
  return (PndGemStation *)fStations->At(iStation);
}
// -------------------------------------------------------------------------

// -----   Public method GetNodeName   -------------------------------------
TString PndGemDigiPar::GetNodeName(Int_t sensorId)
{
  // TString detN = Form("/cave_1/Gem_Disks_0/Gem_Disk%d_Volume_0/Gem_Disk%d_Seg%d_Gem%c_Sensor_kapton_0",
  TString detN = Form("/cave_1/Gem_Disks_0/Gem_Disk%d_Volume_0/Gem_Disk%d_Seg%d_Gem%c_Sensor_GEMmixture_0", GetStationNr(sensorId), GetStationNr(sensorId), GetSegmentNr(sensorId),
                      (GetSensorNr(sensorId) == 1 ? '1' : '6'));
  return detN;
}
// -------------------------------------------------------------------------

// -------   Public method GetStationById   --------------------------------
PndGemStation *PndGemDigiPar::GetStationByNr(Int_t stationNr)
{
  PndGemStation *station = (PndGemStation *)fStationMap[stationNr];
  if (!station) {
    cout << "-W- PndGemDigiPar::GetStationByNr: "
         << "No parameters found for station " << stationNr << endl;
    return nullptr;
  }
  return station;
}
// -------------------------------------------------------------------------

// -----   Public method GetDetectorIdByName  ------------------------------
Int_t PndGemDigiPar::GetDetectorIdByName(TString sensorName)
{
  map<TString, Int_t>::iterator p;
  p = fDetIdByName.find(sensorName);

  if (p != fDetIdByName.end()) {
    return p->second;
  } else {
    cout << " -E- PndGemDigiPar::GetDetectorIdByName \"" << sensorName.Data() << "\" not found " << endl;
    return -1;
  }
}
// -------------------------------------------------------------------------

// -----   Public method GetSensor   ---------------------------------------
PndGemSensor *PndGemDigiPar::GetSensor(Int_t stationNr, Int_t sensorNr)
{
  PndGemStation *stat = (PndGemStation *)this->GetStation(stationNr - 1);
  return stat->GetSensor(sensorNr - 1);
  //  return ( GetStationByNr(stationNr)->GetSensorByNr(sensorNr) );
}
// -------------------------------------------------------------------------

// -----   Public method GetSensorIdByName  --------------------------------
PndGemSensor *PndGemDigiPar::GetSensorByName(TString sensorName)
{
  map<TString, PndGemSensor *>::iterator p;
  p = fSensorByName.find(sensorName);

  if (p != fSensorByName.end()) {
    return p->second;
  } else {
    cout << " -E- PndGemDigiPar::GetSensorByName \"" << sensorName.Data() << "\" not found " << endl;
    return nullptr;
  }
}
// -------------------------------------------------------------------------

ClassImp(PndGemDigiPar);

void PndGemDigiPar::Print()
{
  cout << "-------------------------------------------------" << endl;
  cout << "GEM Digitization Parameters (" << fGemParameters.GetSize() << "):" << endl;

  for (Int_t iind = 0; iind < fGemParameters.GetSize(); iind++) {
    cout << fGemParameters[iind] << " " << flush;
    if (iind % 30 == 3)
      cout << endl << "   " << flush;
    if (iind % 30 == 16)
      cout << endl << "   " << flush;
    if (iind % 30 == 29)
      cout << endl;
  }
  cout << "-------------------------------------------------" << endl;
}
