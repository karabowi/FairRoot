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
// -----                    PndGemMonitor source file                 -----
// -----                  Created 10/04/2013 by R. Karabowicz          -----
// -------------------------------------------------------------------------

/** PndGemMonitor
 *@author Radoslaw Karabowicz <r.karabowicz@gsi.de>
 *@since 10.04.2013
 *@version 1.0
 **
 ** PANDA task class for monitoring
 **/

#include "PndGemMonitor.h"
#include "PndGemSensor.h"
#include "PndGemSensorMonitor.h"

#include <iostream>

using std::cout;
using std::endl;
using std::flush;
using std::map;
using std::pair;

PndGemMonitor *PndGemMonitor::fInstance = nullptr;
// -------------------------------------------------------------------------

// -----  Get Instance  ----------------------------------------------------
PndGemMonitor *PndGemMonitor::Instance()
{
  if (!fInstance) {
    std::cout << "Info in (PndGemMonitor::Instance): Making a new instance using the framework." << std::endl;
    fInstance = new PndGemMonitor();
  }
  return fInstance;
}
// -------------------------------------------------------------------------

// -----  Constructor  -----------------------------------------------------
PndGemMonitor::PndGemMonitor() : FairTask("GEM Monitor", 0), fGeoMan(), fRtdb(), fSensorList(nullptr)
//,
//  fSensorMap(0)
{
  if (fInstance)
    return;
  fInstance = this;
  FairRun *run = FairRun::Instance();
  if (run) {
    this->SetName("PndGemMonitor");
    this->SetTitle("FairTask");
    run->AddTask((FairTask *)this);
  } else {
    std::cout << "PndGemMonitor. No FairRun object found. If used in a macro take another constructor." << std::endl;
  }
}
// -------------------------------------------------------------------------

// -----  Create Sensor Monitor  -------------------------------------------
Int_t PndGemMonitor::CreateSensorMonitor(const PndGemSensor &tempSensor)
{
  if (fSensorList == 0)
    fSensorList = new TObjArray();

  PndGemSensorMonitor *tempMonitor = new PndGemSensorMonitor(tempSensor);

  pair<Int_t, Int_t> a(tempMonitor->GetStationNr(), tempMonitor->GetSensorNr());
  fSensorMap[a] = fSensorList->GetEntries();
  fSensorList->Add(tempMonitor);
  return 0; // ADDED, not clear the meaning of this Int_t function (Stefano)
}
// -------------------------------------------------------------------------

// -----  Enable Digi  -----------------------------------------------------
void PndGemMonitor::EnableDigi(Int_t eventNr, Int_t digiNr, PndGemDigi *tempDigi)
{
  pair<Int_t, Int_t> a(tempDigi->GetStationNr(), tempDigi->GetSensorNr());
  PndGemSensorMonitor *sensor = (PndGemSensorMonitor *)(fSensorList->At(fSensorMap[a]));

  // sensor->ChannelLastActiveAt(tempDigi->GetChannelNr(),
  //  			      tempDigi->GetSide(),
  //  			      tempDigi->GetTimeStamp(),
  // 			      eventNr);

  sensor->ActivateChannel(tempDigi->GetChannelNr(), tempDigi->GetSide(), eventNr, digiNr, tempDigi->GetTimeStamp(), tempDigi->GetCharge());

  //  // -----   How much time passed since last activation?   ----------------------------------------------------
  //  Double_t PndGemSensorMonitor::ChannelLastActiveAt(Int_t channelNr, Int_t sideId, Double_t timeNow) {
  //  }
  //  // -------------------------------------------------------------------------

  // // -----   Last activation data   ----------------------------------------------------
  // Bool_t PndGemSensorMonitor::ChannelLastActivation(Int_t channelNr, Int_t sideId, Int_t& eventNr, Int_t& digiNr, Double_t& channelTime, Double_t& channelCharge) {
  // }
  // // -------------------------------------------------------------------------
}
// -------------------------------------------------------------------------

// -----  Return the last time the channel was active  -----------------------------------------------------
Double_t PndGemMonitor::ChannelLastActiveAt(Int_t statNr, Int_t sensNr, Int_t sideId, Int_t chanNr)
{
  pair<Int_t, Int_t> a(statNr, sensNr);
  PndGemSensorMonitor *sensor = (PndGemSensorMonitor *)(fSensorList->At(fSensorMap[a]));
  //  cout << "sensor is there " << sensor << " :" << endl;

  return sensor->ChannelLastActiveAt(chanNr, sideId);
}
// -------------------------------------------------------------------------

// -----  Enable Cluster  -----------------------------------------------------
void PndGemMonitor::EnableCluster(Int_t eventNr, Int_t clusterNr, PndGemCluster *tempCluster)
{
  pair<Int_t, Int_t> a(tempCluster->GetStationNr(), tempCluster->GetSensorNr());
  PndGemSensorMonitor *sensor = (PndGemSensorMonitor *)(fSensorList->At(fSensorMap[a]));

  for (Int_t ichan = tempCluster->GetClusterBeg(); ichan <= tempCluster->GetClusterEnd(); ichan++) {
    sensor->ActivateChannel(ichan, tempCluster->GetSide(), eventNr, clusterNr, tempCluster->GetTimeStamp(), tempCluster->GetCharge());
  }
}
// -------------------------------------------------------------------------

// -----  Print  -----------------------------------------------------------
void PndGemMonitor::Print()
{
  std::cout << "--- PndGemMonitor :: Print () ---" << std::endl;
  std::cout << "--- " << fSensorList->GetEntries() << " sensors" << std::endl;
  for (Int_t isens = 0; isens < fSensorList->GetEntries(); isens++) {
    ((PndGemSensorMonitor *)fSensorList->At(isens))->Print();
  }

  std::cout << "---------------------------------" << std::endl;
}
// -------------------------------------------------------------------------
