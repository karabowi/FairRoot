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
// -----                    PndGemDigiPar header file                 -----
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

#ifndef PNDGEMDIGIPAR_H
#define PNDGEMDIGIPAR_H

#include <TVector2.h>
#include <TArrayD.h>
#include <TMath.h>

#include "FairParGenericSet.h"
#include "FairParamList.h"
#include "PndGemStation.h"
#include "PndGemSensor.h"

//! Digitization Parameter Class for GEM part
class PndGemDigiPar : public FairParGenericSet {
 public:
  PndGemDigiPar(const char *name = "PndGemParTest", const char *title = "PndGem digi parameter", const char *context = "TestDefaultContext");
  ~PndGemDigiPar(void);
  void clear(void){};
  void putParams(FairParamList *list);
  Bool_t getParams(FairParamList *list);

  void Print();

  /** Accessors **/
  Int_t GetNStations() { return fStations->GetEntries(); }
  Int_t GetNSensors() { return fNSensors; }
  Int_t GetNChannels() { return fNChannels; }
  TObjArray *GetStations() { return fStations; }
  PndGemStation *GetStation(Int_t iStation);                /** Station by index **/
  PndGemStation *GetStationByNr(Int_t stationNr);           /** Station by number **/
  PndGemSensor *GetSensor(Int_t stationNr, Int_t sensorNr); /** Sensor  by number **/
  PndGemSensor *GetSensor(Int_t sensorId) { return GetSensor(GetStationNr(sensorId), GetSensorNr(sensorId)); }
  TString GetNodeName(Int_t sensorId);
  Int_t GetStationNr(Int_t sensorId) { return (sensorId & (15 << 8)) >> 8; }
  Int_t GetSensorNr(Int_t sensorId) { return (sensorId & (15 << 4)) >> 4; }
  Int_t GetSegmentNr(Int_t sensorId) { return (sensorId & (15 << 0)) >> 0; }

  Int_t GetDetectorIdByName(TString sensorName);
  PndGemSensor *GetSensorByName(TString sensorName);

  Double_t GetTrackFinderOnHits_ParThetaA() { return fTrackFinderOnHits_ParThetaA; }
  Double_t GetTrackFinderOnHits_ParThetaB() { return fTrackFinderOnHits_ParThetaB; }

  Double_t GetTrackFinderOnHits_ParTheta0() { return fTrackFinderOnHits_ParTheta0; }
  Double_t GetTrackFinderOnHits_ParTheta1() { return fTrackFinderOnHits_ParTheta1; }
  Double_t GetTrackFinderOnHits_ParTheta2() { return fTrackFinderOnHits_ParTheta2; }
  Double_t GetTrackFinderOnHits_ParTheta3() { return fTrackFinderOnHits_ParTheta3; }

  Double_t GetTrackFinderOnHits_ParRadPhi0() { return fTrackFinderOnHits_ParRadPhi0; }
  Double_t GetTrackFinderOnHits_ParRadPhi2() { return fTrackFinderOnHits_ParRadPhi2; }

  Double_t GetTrackFinderOnHits_ParMat0(Int_t in) { return fTrackFinderOnHits_ParMat0[in]; }
  Double_t GetTrackFinderOnHits_ParMat1(Int_t in) { return fTrackFinderOnHits_ParMat1[in]; }

 private:
  void CreateStations();

  TArrayD fGemParameters;

  TObjArray *fStations; /** Array of FairStsStation **/
  Int_t fNSensors;      /** Total number of sensors **/
  Int_t fNChannels;     /** Total number of channels **/

  Double_t fTrackFinderOnHits_ParThetaA;
  Double_t fTrackFinderOnHits_ParThetaB;

  Double_t fTrackFinderOnHits_ParTheta0;
  Double_t fTrackFinderOnHits_ParTheta1;
  Double_t fTrackFinderOnHits_ParTheta2;
  Double_t fTrackFinderOnHits_ParTheta3;

  Double_t fTrackFinderOnHits_ParRadPhi0;
  Double_t fTrackFinderOnHits_ParRadPhi2;

  TArrayD fTrackFinderOnHits_ParMat0;
  TArrayD fTrackFinderOnHits_ParMat1;

  std::map<Int_t, PndGemStation *> fStationMap; //! Map from number to station
  std::map<TString, Int_t> fDetIdByName;
  std::map<TString, PndGemSensor *> fSensorByName;

  ClassDef(PndGemDigiPar, 2);
};

#endif /*!GEMSTRIPDIGIPAR_H*/
