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
// -----                    PndGemMonitor header file                 -----
// -----                  Created 10/04/2013 by R. Karabowicz          -----
// -------------------------------------------------------------------------

/** PndGemMonitor
 *@author Radoslaw Karabowicz <r.karabowicz@gsi.de>
 *@since 10.04.2013
 *@version 1.0
 **
 ** PANDA task class for monitoring GEM
 **/

#ifndef PNDGEMMONITOR_H
#define PNDGEMMONITOR_H

#include <TVector2.h>
#include <TArrayD.h>
#include <TMath.h>
#include <TGeoManager.h>

#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairTask.h"
#include "PndGemSensor.h"
#include "PndGemSensorMonitor.h"
#include "PndGemDigi.h"
#include "PndGemCluster.h"

#include <map>

//! Digitization Parameter Class for GEM part
class PndGemMonitor : public FairTask {
 public:
  PndGemMonitor();

  static PndGemMonitor *Instance();

  static void Destroy()
  {
    if (fInstance) {
      delete fInstance;
      fInstance = nullptr;
    }
  }

  virtual ~PndGemMonitor(){};

  Double_t ChannelLastActiveAt(Int_t statNr, Int_t sensNr, Int_t sideId, Int_t chanNr);
  Int_t CreateSensorMonitor(const PndGemSensor &tempSensor);
  void EnableDigi(Int_t eventNr, Int_t digiNr, PndGemDigi *tempDigi);
  void EnableCluster(Int_t eventNr, Int_t clusterNr, PndGemCluster *tempCluster);

  void Print();

 private:
  static PndGemMonitor *fInstance;
  PndGemMonitor(PndGemMonitor &) : FairTask() // gm //[R.K.03/2017] unused variable(s)
  {
  }

  TGeoManager *fGeoMan;
  FairRuntimeDb *fRtdb;

  TObjArray *fSensorList;                              /** list of sensor monitors **/
  std::map<std::pair<Int_t, Int_t>, Int_t> fSensorMap; /** map of stationNr,sensorNr to sensor **/

  ClassDef(PndGemMonitor, 1);
};

#endif /*!GEMSTRIPDIGIPAR_H*/
