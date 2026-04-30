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

/* $Id: */

// -------------------------------------------------------------------------
// -----                    PndGemFindClustersTB source file         -----
// -----                  Created 15/06/2013 by R. Karabowicz          -----
// -------------------------------------------------------------------------

/** PndGemFindClustersTB
 *@author Radoslaw Karabowicz <r.karabowicz@gsi.de>
 *@since 15.06.2013
 *@version 1.0
 **
 ** PANDA task class for finding PndGemClusters from PndGemDigis,
 ** after TimeBased digitization
 ** Task level RECO
 **/

#include "PndGemFindClustersTB.h"

#include "PndGemCluster.h"
#include "PndGemDigi.h"
#include "PndGemDigiPar.h"
#include "PndGemSensor.h"
#include "PndGemStation.h"

#include "PndDetectorList.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"

#include "TClonesArray.h"
#include "TMath.h"
#include "TRandom.h"

#include <iomanip>

using std::cerr;
using std::cout;
using std::endl;
using std::fixed;
using std::flush;
using std::left;
using std::map;
using std::right;
using std::set;
using std::setprecision;
using std::setw;

// -----   Default constructor   ------------------------------------------
PndGemFindClustersTB::PndGemFindClustersTB()
  : FairTask("GEMFindClusters", 0), fDigiPar(nullptr), fDigis(nullptr), fClusters(nullptr), fTNofEvents(0), fTNofDigis(0), fTNofClusters(0), fFunctor(nullptr), fPrepTime(0.),
    fCreateTime(0.), fSortTime(0.), fCheckTime(0.), fAnaTime(0.), fWriteTime(0.), fAllTime(0.), fInBranchName("GEMDigi")
{
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndGemFindClustersTB::PndGemFindClustersTB(Int_t iVerbose)
  : FairTask("GEMFindClusters", iVerbose), fDigiPar(nullptr), fDigis(nullptr), fClusters(nullptr), fTNofEvents(0), fTNofDigis(0), fTNofClusters(0), fFunctor(nullptr),
    fPrepTime(0.), fCreateTime(0.), fSortTime(0.), fCheckTime(0.), fAnaTime(0.), fWriteTime(0.), fAllTime(0.), fInBranchName("GEMDigi")
{
}
// -------------------------------------------------------------------------

// -----   Constructor with name   -----------------------------------------
PndGemFindClustersTB::PndGemFindClustersTB(const char *name, Int_t iVerbose)
  : FairTask(name, iVerbose), fDigiPar(nullptr), fDigis(nullptr), fClusters(nullptr), fTNofEvents(0), fTNofDigis(0), fTNofClusters(0), fFunctor(nullptr), fPrepTime(0.),
    fCreateTime(0.), fSortTime(0.), fCheckTime(0.), fAnaTime(0.), fWriteTime(0.), fAllTime(0.), fInBranchName("GEMDigi")
{
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndGemFindClustersTB::~PndGemFindClustersTB()
{
  if (nullptr != fClusters) {
    fClusters->Delete();
    delete fClusters;
  }
  if (0 != fFunctor)
    delete fFunctor;
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndGemFindClustersTB::Exec(Option_t *)
{
  //  cout << endl << "======== PndGemFindClustersTB::Exec(Event = " << fTNofEvents << " ) ====================" << endl;
  fTimer.Start();

  if (fInBranchName.Contains("Sorted")) {
    fDigis->Clear();
    fDigis = FairRootManager::Instance()->GetData(fInBranchName, fFunctor, 20.); // FairRootManager::Instance()->GetEventTime() +
    if (fVerbose > 1)
      LOG(info) << " PndGemFindClustersTB::Exec Digis: " << fDigis->GetEntries();
  } else
    fDigis = (TClonesArray *)FairRootManager::Instance()->GetObject(fInBranchName);

  fClusters = FairRootManager::Instance()->GetTClonesArray("GEMCluster");
  if (!fClusters)
    Fatal("Exec", "No GEM Cluster Array");
  fClusters->Delete();

  fPrepTime += fTimer.RealTime();
  fTimer.Continue();

  Int_t nDigis = fDigis->GetEntries();
  Int_t nClusters = 0;

  CreateClusters();
  fCreateTime += fTimer.RealTime();
  fTimer.Continue();

  Int_t nofC2W = 0;
  for (size_t idc = 0; idc < fDigiClusters.size(); idc++)
    if (fDigiClusters[idc].cluPos > -0.5)
      nofC2W++;
  //  cout << ">>>> created        " << fDigiClusters.size() << " clusters, " << nofC2W << " to write" << endl;

  //  cout << "sorting" << endl;
  SortClusters();
  fSortTime += fTimer.RealTime();
  fTimer.Continue();
  //  cout << "done" << endl;

  //  cout << "checking" << endl;
  CheckClusters();
  fCheckTime += fTimer.RealTime();
  fTimer.Continue();
  //  cout << "done" << endl;

  //  cout << "analyzing" << endl;
  // RK: turn the AnalyzeClusters off, as it causes problems
  /*  AnalyzeClusters();
  fAnaTime += fTimer.RealTime();
  fTimer.Continue();*/
  //  cout << "done" << endl;

  nofC2W = 0;
  for (size_t idc = 0; idc < fDigiClusters.size(); idc++)
    if (fDigiClusters[idc].cluPos > -0.5)
      nofC2W++;
  //  cout << ">>>> after cleaning " << fDigiClusters.size() << " clusters, " << nofC2W << " to write" << endl;

  //  PrintClusters();

  nClusters = WriteClusters();
  fWriteTime += fTimer.RealTime();
  fTimer.Continue();

  /*
  //  cout << "         PndGemFindClustersTB::Exec. Created " << nClusters << " clusters out of " << nDigis << " digis" << flush;
  // in " << fTimer.RealTime() << "s" << flush;
  if ( fDigis->GetEntries() > 0 ) {
    cout << ", ( " << ((PndGemDigi*)fDigis->At(0))->GetTimeStamp()
   << " -- " << ((PndGemDigi*)fDigis->At(nDigis-1))->GetTimeStamp()
   << " )" << endl;
  }
  else {
    cout << "." << endl;
  }
  */

  fTNofEvents += 1;
  fTNofDigis += nDigis;
  fTNofClusters += nClusters;

  fAllTime += fTimer.RealTime();

  fTimer.Stop();
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
Int_t PndGemFindClustersTB::CreateClusters()
{
  fDigiClusters.clear();

  PndGemDigi *digi = nullptr;
  for (Int_t idigi = 0; idigi < fDigis->GetEntries(); idigi++) {
    digi = (PndGemDigi *)fDigis->At(idigi);
    //    cout << "digi " << idigi << " in " << digi->GetDetectorId() << ", there are already " << fDigiClusters.size() << " clusters" << endl;
    //    cout << idigi << " ---> " << digi->GetDetectorId() << " @ " << digi->GetChannelNr() << " @ " << digi->GetTimeStamp() << endl;

    if (CompareDigiToClusters(idigi) == kTRUE)
      continue;

    DigiClusterTB tempDC;
    tempDC.detId = digi->GetDetectorId();
    tempDC.digiNr.push_back(idigi);
    tempDC.chanNr.push_back((Int_t)digi->GetChannelNr());
    tempDC.sigADC.push_back(digi->GetCharge());
    tempDC.sigTDC.push_back(digi->GetTimeStamp());
    tempDC.cluTDC = digi->GetTimeStamp();
    tempDC.cluADC = digi->GetCharge();
    tempDC.cluPos = digi->GetChannelNr();
    tempDC.cluPMn = digi->GetChannelNr();
    tempDC.cluPMx = digi->GetChannelNr();
    tempDC.cluMPs = digi->GetChannelNr();
    tempDC.cluMVl = digi->GetCharge();
    fDigiClusters.push_back(tempDC);
  }

  // if ( fTNofEvents == 9 ) {
  //   for ( Int_t idc = 0 ; idc < fDigiClusters.size() ; idc++ ) {
  //     if ( fDigiClusters[idc].detId == 568328832 || fDigiClusters[idc].detId == 568329088 ) {
  // 	cout << "CRE CLUSTER " << idc << " in " << fDigiClusters[idc].detId << " from " << fDigiClusters[idc].cluPMn << " to " << fDigiClusters[idc].cluPMx << " at " <<
  // fDigiClusters[idc].cluPos << endl;
  //     }
  //   }
  // }

  //  cout << endl;
  return fDigiClusters.size();
}
// -------------------------------------------------------------------------

// -----   Private method CompareDigiToClusters()   ------------------------------------------
Bool_t PndGemFindClustersTB::CompareDigiToClusters(Int_t digiNumber)
{
  Bool_t printInfo = kFALSE;

  PndGemSensor *sensor;
  PndGemDigi *digi = (PndGemDigi *)fDigis->At(digiNumber);

  //  if ( fTNofEvents == 9 && digi->GetDetectorId() == 568328832 && TMath::Abs(digi->GetChannelNr()-3100)<100 ) printInfo = kTRUE;

  if (printInfo)
    cout << "digi [" << digiNumber << "] in detector " << digi->GetDetectorId() << " channel " << digi->GetChannelNr() << " @ " << digi->GetTimeStamp() << endl;

  Int_t digiUsed = -1;
  for (size_t idc = 0; idc < fDigiClusters.size(); idc++) {
    if (fDigiClusters[idc].cluPos < -0.5)
      continue;
    if (digi->GetDetectorId() != fDigiClusters[idc].detId)
      continue;

    //    if ( fDigiClusters[idc].detId == 568329024 ) {
    // if ( fDigiClusters[idc].detId == -1 ) {
    if (printInfo) {
      cout << "Trying to match digi " << digiNumber << " @ " << digi->GetChannelNr() << " to cluster @ " << fDigiClusters[idc].cluPos << " (from " << fDigiClusters[idc].cluPMn
           << " to " << fDigiClusters[idc].cluPMx << ") @ " << fDigiClusters[idc].cluTDC << " with " << fDigiClusters[idc].sigADC.size() << " digis" << endl;
    }

    Int_t stationNr = digi->GetStationNr();
    Int_t sensorNr = digi->GetSensorNr();
    Int_t iSide = digi->GetSide();

    sensor = fDigiPar->GetSensor(stationNr, sensorNr);

    // This GetDistance(side,ca,cb,ct) returns:
    // 0 if ct is between ca and cb
    // -1 if ct is on different plane that ca/cb
    // n the distance of ct to ca or cb (smaller one)
    Int_t digiClustDist = sensor->GetDistance(iSide, fDigiClusters[idc].cluPMn, fDigiClusters[idc].cluPMx, digi->GetChannelNr());
    Int_t clusterLength = sensor->GetDistance(iSide, fDigiClusters[idc].cluPMn, fDigiClusters[idc].cluPMx) + 1;
    if (printInfo)
      cout << "GOT DISTANCE: " << digiClustDist << " WHILE THE CLUSTER LENGTH = " << clusterLength << " CHECK AT " << 19 / clusterLength + 1 << endl;
    if (digiClustDist < 0)
      continue; // there's no connection
    if (digiClustDist > 20)
      continue; // they are too far
    Double_t digiTimeDist = digi->GetTimeStamp() - fDigiClusters[idc].cluTDC;
    //    cout << "TIME DIST: " << digiTimeDist << endl;
    if (TMath::Abs(digiTimeDist) > 11.)
      continue; // they are too far in time

    if (fVerbose > 1)
      cout << "digi " << digiNumber << " ( " << digi->GetDetectorId() << " / " << digi->GetChannelNr() << " ) could attach to cluster " << idc << " ( " << fDigiClusters[idc].cluPos
           << " ) with distance " << digiClustDist << " . " << fDigiClusters[idc].digiNr.size() << " digis in cluster" << flush;
    //    cout << "ADDING DIGI @ " << digi->GetTimeStamp() << " TO CLUSTER[" << idc << "] AT " << fDigiClusters[idc].cluTDC << flush;

    AddDigiToCluster(digiNumber, idc);
    //    cout << " MOVES IT TO " << fDigiClusters[idc].cluTDC << endl;

    if (fVerbose > 1)
      cout << " >>> new pos = " << fDigiClusters[idc].cluPos << endl;

    if (printInfo) {
      //    if ( fDigiClusters[idc].detId == 568329024 ) {
      cout << "DIGI " << digiNumber << " AT " << digi->GetChannelNr() << " MATCHES TO CLUSTER " << endl;
    }

    if (digiUsed >= 0) {
      // cout << "DIGI " << digiNumber << " MATCHES TO CLUSTER " << digiUsed << " and " << idc << endl;
      JoinTwoClusters(idc, digiUsed);
    }
    digiUsed = idc;
  }
  return (digiUsed == -1 ? kFALSE : kTRUE);
}
// -------------------------------------------------------------------------

// -----  Private method JoinTwoClusters   ---------------------------------
void PndGemFindClustersTB::JoinTwoClusters(Int_t clus1, Int_t clus2)
{
  Bool_t printInfo = kFALSE;
  //  if ( fTNofEvents == 9 && fDigiClusters[clus1].detId == 568328832 ) printInfo = kTRUE;

  if (printInfo) {
    cout << "JoinTwoClusters-------------------------------------" << endl;
    SortCluster(clus1);
    PrintCluster(clus1);
    SortCluster(clus2);
    PrintCluster(clus2);
  }

  // go through digis in clus2
  for (size_t id2 = 0; id2 < fDigiClusters[clus2].digiNr.size(); id2++) {
    Int_t digi2 = fDigiClusters[clus2].digiNr[id2];

    Int_t addDigi = kTRUE;
    for (size_t id1 = 0; id1 < fDigiClusters[clus1].digiNr.size(); id1++)
      if (digi2 == fDigiClusters[clus1].digiNr[id1])
        addDigi = kFALSE;
    // if ( addDigi ) cout << "SHOULD " << (addDigi?"":"NOT ") << "ADD DIGI " << digi2 << " ( " << fDigiClusters[clus2].chanNr[id2] << " )" << endl;
    if (addDigi)
      AddDigiToCluster(digi2, clus1);
  }
  fDigiClusters[clus2].cluPos = -1.;
}
// -------------------------------------------------------------------------

// -----  Private method AddDigiToCluster   ---------------------------------
void PndGemFindClustersTB::AddDigiToCluster(Int_t digiNr, Int_t clusNr)
{
  PndGemDigi *digi = (PndGemDigi *)fDigis->At(digiNr);
  Int_t stationNr = digi->GetStationNr();
  Int_t sensorNr = digi->GetSensorNr();
  Int_t iSide = digi->GetSide();

  PndGemSensor *sensor = fDigiPar->GetSensor(stationNr, sensorNr);

  if (fDigiClusters[clusNr].sigTDC.size() == 0) {
    //    cout << "NEW CLUSTER " << clusNr << " CREATED WITH AddDigiToCluster(" << digiNr << "," << clusNr << ") IN DETECTOR " << digi->GetDetectorId() << endl;
    fDigiClusters[clusNr].detId = digi->GetDetectorId();
    fDigiClusters[clusNr].cluPos = digi->GetChannelNr();
    fDigiClusters[clusNr].cluADC = digi->GetCharge();
    fDigiClusters[clusNr].cluTDC = digi->GetTimeStamp();
    fDigiClusters[clusNr].cluPMn = digi->GetChannelNr();
    fDigiClusters[clusNr].cluPMx = digi->GetChannelNr();
    fDigiClusters[clusNr].cluMVl = digi->GetCharge();
    fDigiClusters[clusNr].cluMPs = digi->GetChannelNr();
  } else {
    fDigiClusters[clusNr].cluTDC =
      (fDigiClusters[clusNr].cluTDC * ((Double_t)(fDigiClusters[clusNr].sigADC.size() - 1.)) + digi->GetTimeStamp()) / ((Double_t)(fDigiClusters[clusNr].sigADC.size()));
    fDigiClusters[clusNr].cluPos = sensor->GetMeanChannel(iSide, fDigiClusters[clusNr].cluPos, fDigiClusters[clusNr].cluADC, digi->GetChannelNr(), digi->GetCharge());
    if (fDigiClusters[clusNr].cluPMn > digi->GetChannelNr())
      fDigiClusters[clusNr].cluPMn = digi->GetChannelNr();
    if (fDigiClusters[clusNr].cluPMx < digi->GetChannelNr())
      fDigiClusters[clusNr].cluPMx = digi->GetChannelNr();
    fDigiClusters[clusNr].cluADC = fDigiClusters[clusNr].cluADC + digi->GetCharge();
    if (digi->GetCharge() > fDigiClusters[clusNr].cluMVl) {
      fDigiClusters[clusNr].cluMVl = digi->GetCharge();
      fDigiClusters[clusNr].cluMPs = digi->GetChannelNr();
    }
  }

  fDigiClusters[clusNr].digiNr.push_back(digiNr);
  fDigiClusters[clusNr].chanNr.push_back((Int_t)digi->GetChannelNr());
  fDigiClusters[clusNr].sigADC.push_back(digi->GetCharge());
  fDigiClusters[clusNr].sigTDC.push_back(digi->GetTimeStamp());
}
// -------------------------------------------------------------------------

// -----  Private method AnalyzeClusters   --------------------------------------
void PndGemFindClustersTB::AnalyzeClusters()
{
  PndGemDigi *digi = nullptr;
  PndGemSensor *sensor = nullptr;

  //  cout << "CALLING AnalyzeClusters() in EVENT " << fTNofEvents << ", and have " << fDigiClusters.size() << endl;

  Bool_t printInfo = kFALSE;

  //  if ( fTNofEvents == 496 ) printInfo = kTRUE;

  if (printInfo)
    cout << "AnalyzeClusters" << endl;

  for (size_t idc = 0; idc < fDigiClusters.size(); idc++) {
    if (fDigiClusters[idc].cluPos < -0.5)
      continue;

    //    if ( fTNofEvents == 9 && fDigiClusters[idc].detId == 568329088 ) printInfo = kTRUE;

    if (printInfo) {
      cout << "CLUSTER " << idc << " IN " << fDigiClusters[idc].detId << " AT " << fDigiClusters[idc].cluPos << " FROM " << fDigiClusters[idc].digiNr.size() << " DIGIS ("
           << fDigiClusters[idc].cluPMn << " TO " << fDigiClusters[idc].cluPMx << "), MAX(" << fDigiClusters[idc].cluMPs << ") = " << fDigiClusters[idc].cluMVl << endl;
    }

    digi = (PndGemDigi *)fDigis->At(fDigiClusters[idc].digiNr[0]);

    Int_t stationNr = digi->GetStationNr();
    Int_t sensorNr = digi->GetSensorNr();
    Int_t iSide = digi->GetSide();

    if (printInfo)
      cout << "DIGI [" << fDigiClusters[idc].digiNr[0] << "] in station " << stationNr << " sensor " << sensorNr << " side " << iSide << " channel " << digi->GetChannelNr()
           << endl;

    sensor = fDigiPar->GetSensor(stationNr, sensorNr);

    if (printInfo)
      cout << "CLUSTERS IN " << fDigiClusters[idc].detId << " COVERS " << sensor->GetDistance(iSide, fDigiClusters[idc].cluPMn, fDigiClusters[idc].cluPMx) + 1 << " STRIPS FROM "
           << fDigiClusters[idc].cluPMn << " TO " << fDigiClusters[idc].cluPMx << " @ " << fDigiClusters[idc].cluTDC << "ns, " << fDigiClusters[idc].digiNr.size()
           << " STRIPS FIRED." << endl;

    if (sensor->GetDistance(iSide, fDigiClusters[idc].cluPMn, fDigiClusters[idc].cluPMx) + 1 == fDigiClusters[idc].digiNr.size()) {
      if (printInfo)
        cout << "The cluster has as many digis as possible strips (" << fDigiClusters[idc].digiNr.size() << ")." << endl;
      continue;
    }

    //    PrintCluster(idc);

    Int_t thisChan = -1;
    Int_t lastChan = -1;
    Int_t chanDist = -1;
    for (size_t idigi = 0; idigi < fDigiClusters[idc].digiNr.size(); idigi++) {
      thisChan = fDigiClusters[idc].chanNr[idigi];
      digi = (PndGemDigi *)fDigis->At(fDigiClusters[idc].digiNr[idigi]);

      if (lastChan != -1)
        chanDist = sensor->GetDistance(iSide, thisChan, lastChan);

      if (lastChan == -1 || chanDist > 1) {
        DigiClusterTB tempDC;
        tempDC.detId = digi->GetDetectorId();
        tempDC.digiNr.push_back(fDigiClusters[idc].digiNr[idigi]);
        tempDC.chanNr.push_back((Int_t)digi->GetChannelNr());
        tempDC.sigADC.push_back(digi->GetCharge());
        tempDC.sigTDC.push_back(digi->GetTimeStamp());
        tempDC.cluTDC = digi->GetTimeStamp();
        tempDC.cluADC = digi->GetCharge();
        tempDC.cluPos = digi->GetChannelNr();
        tempDC.cluPMn = digi->GetChannelNr();
        tempDC.cluPMx = digi->GetChannelNr();
        tempDC.cluMPs = digi->GetChannelNr();
        tempDC.cluMVl = digi->GetCharge();
        if (printInfo)
          cout << "CREATING ClUSTER " << fDigiClusters.size() << " IN DETECTOR " << tempDC.detId << " / " << digi->GetSide() << " at " << digi->GetChannelNr() << endl;
        fDigiClusters.push_back(tempDC);
      } else {
        AddDigiToCluster(fDigiClusters[idc].digiNr[idigi], fDigiClusters.size() - 1);
        if (printInfo) {
          cout << " ADDED CHANNEL " << fDigiClusters[idc].chanNr[idigi] << " @ " << fDigiClusters[idc].sigTDC[idigi] << "ns"
               << " TO CLUSTER " << fDigiClusters.size() - 1 << " TO HAVE NOW " << fDigiClusters[fDigiClusters.size() - 1].digiNr.size()
               << " DIGIS MOVED IT TO: " << fDigiClusters[fDigiClusters.size() - 1].cluPos << endl;
        }
      }
      lastChan = thisChan;
    }
    fDigiClusters[idc].cluPos = -1.;
  }

  // if ( fTNofEvents == 9 ) {
  //   for ( Int_t idc = 0 ; idc < fDigiClusters.size() ; idc++ ) {
  //     if ( fDigiClusters[idc].detId == 568328832 || fDigiClusters[idc].detId == 568329088 ) {
  // 	cout << "ANA CLUSTER " << idc << " in " << fDigiClusters[idc].detId << " from " << fDigiClusters[idc].cluPMn << " to " << fDigiClusters[idc].cluPMx << " at " <<
  // fDigiClusters[idc].cluPos << endl;
  //     }
  //   }
  // }
}
// -------------------------------------------------------------------------

// -----  Private method WriteClusters   --------------------------------------
Int_t PndGemFindClustersTB::WriteClusters()
{

  Int_t nClusters = 0;
  std::vector<Int_t> clusterRefs;
  PndGemDigi *digi;

  for (size_t idc = 0; idc < fDigiClusters.size(); idc++) {

    //    if ( fDigiClusters[idc].cluADC < 1. ) continue;
    if (fDigiClusters[idc].cluPos < -0.5)
      continue;

    clusterRefs.clear();

    for (size_t id = 0; id < fDigiClusters[idc].digiNr.size(); id++) {
      digi = (PndGemDigi *)fDigis->At(fDigiClusters[idc].digiNr[id]);

      if (!digi)
        cout << "there is no digi number " << fDigiClusters[idc].digiNr[id] << ", cause there are only " << fDigis->GetEntries() << " of them" << endl;
      for (Int_t irf = digi->GetNIndices() - 1; irf >= 0; irf--) {
        Int_t refIndex = digi->GetIndex(irf);

        Bool_t alreadyKnown = kFALSE;
        for (Int_t icr = clusterRefs.size() - 1; icr >= 0; icr--) {
          if (refIndex == clusterRefs[icr]) {
            alreadyKnown = kTRUE;
            break;
          }
        }
        if (!alreadyKnown)
          clusterRefs.push_back(refIndex);
      }
    }

    //    cout << "CREATING CLUSTER " << idc << " IN DETECTOR " << fDigiClusters[idc].detId << " AT " << fDigiClusters[idc].cluPos << " WITH TIME " << fDigiClusters[idc].cluTDC <<
    //    endl;
    //  PndGemCluster(Int_t iDetectorId, Double_t iChannel, Int_t bChannel, Int_t eChannel, Double_t signal, Double_t time, std::vector<Int_t> index);

    new ((*fClusters)[nClusters]) PndGemCluster(fDigiClusters[idc].detId, fDigiClusters[idc].cluPos, fDigiClusters[idc].cluPMn, fDigiClusters[idc].cluPMx,
                                                fDigiClusters[idc].cluADC, fDigiClusters[idc].cluTDC, clusterRefs);

    // for ( Int_t icr = clusterRefs.size()-1 ; icr > 0 ; icr-- )
    //   digi->AddIndex(clusterRefs[icr]);
    if (fVerbose > 1)
      cout << "creating cluster at " << fDigiClusters[idc].cluPos << " with height of " << fDigiClusters[idc].cluADC << " /// compare maximum at " << fDigiClusters[idc].cluMPs
           << endl;
    nClusters++;
  }
  return nClusters;
}
// -------------------------------------------------------------------------

// -----   Private method CheckClusters   -------------------------------
void PndGemFindClustersTB::CheckClusters()
{
  for (size_t idc = 0; idc < fDigiClusters.size(); idc++) {
    CheckCluster(idc);
  }
}
// -------------------------------------------------------------------------

// -----   Private method CheckCluster   -------------------------------
void PndGemFindClustersTB::CheckCluster(Int_t clus)
{
  //  cout << "CHECKING CLUSTER " << clus << endl;

  PndGemDigi *digi = (PndGemDigi *)fDigis->At(fDigiClusters[clus].digiNr[0]);

  Int_t stationNr = digi->GetStationNr();
  Int_t sensorNr = digi->GetSensorNr();
  Int_t iSide = digi->GetSide();

  PndGemSensor *sensor = fDigiPar->GetSensor(stationNr, sensorNr);

  Int_t minPart = sensor->GetSensorPart(iSide, fDigiClusters[clus].cluPMn);
  if (minPart == -1)
    return;

  Int_t maxPart = sensor->GetSensorPart(iSide, fDigiClusters[clus].cluPMx);
  if (minPart == maxPart)
    return;

  Int_t tempPart = -1;
  for (size_t ichan = 1; ichan < fDigiClusters[clus].digiNr.size() - 1; ichan++) {
    tempPart = sensor->GetSensorPart(iSide, fDigiClusters[clus].chanNr[ichan]);
    if (tempPart != minPart && tempPart != maxPart) {
      // cout << "PARTS ARE DIFFERENT: "
      // 	   << fDigiClusters[clus].cluPMn << " (" << minPart << ") / "
      // 	   << fDigiClusters[clus].chanNr[ichan] << " (" << tempPart << ") / "
      // 	   << fDigiClusters[clus].cluPMx << " (" << maxPart << ")"
      // 	   << endl;
      tempPart = 666;
      break;
    }
  }

  if (tempPart < 600)
    return;

  // the temp was 666
  // it means that the digis are from 3 different parts of the sensor (0,1 and 2),
  // while clusters can be created of digis from at most 2 parts (0,1 or 0,2).
  // Have to create TWO clusters consisting of digis from parts (0,1) and (0,2).

  DigiClusterTB tempDC;
  fDigiClusters.push_back(tempDC);
  fDigiClusters.push_back(tempDC);

  for (size_t ichan = 0; ichan < fDigiClusters[clus].digiNr.size(); ichan++) {
    tempPart = sensor->GetSensorPart(iSide, fDigiClusters[clus].chanNr[ichan]);

    if (tempPart == 0 || tempPart == 1) {
      // cout << "ADDING DIGI " << fDigiClusters[clus].digiNr[ichan]
      // 	   << " TO CLUSTER " << fDigiClusters.size()-1
      // 	   << " WITH " << fDigiClusters[fDigiClusters.size()-1].digiNr.size() << endl;
      AddDigiToCluster(fDigiClusters[clus].digiNr[ichan], fDigiClusters.size() - 1);
    }
    if (tempPart == 0 || tempPart == 2) {
      // cout << "ADDING DIGI " << fDigiClusters[clus].digiNr[ichan]
      // 	   << " TO CLUSTER " << fDigiClusters.size()-2
      // 	   << " WITH " << fDigiClusters[fDigiClusters.size()-2].digiNr.size() << endl;
      AddDigiToCluster(fDigiClusters[clus].digiNr[ichan], fDigiClusters.size() - 2);
    }
  }

  fDigiClusters[clus].cluPos = -1.;
}
// -------------------------------------------------------------------------

// -----   Private method SortClusters   -------------------------------
void PndGemFindClustersTB::SortClusters()
{
  for (size_t idc = 0; idc < fDigiClusters.size(); idc++) {
    SortCluster(idc);
  }
}
// -------------------------------------------------------------------------

// -----   Private method SortClusters   -------------------------------
void PndGemFindClustersTB::SortCluster(Int_t clus)
{
  Int_t channelPosition = 0;

  for (Int_t ich = fDigiClusters[clus].cluPMn; ich <= fDigiClusters[clus].cluPMx; ich++) {
    for (size_t idigi = channelPosition; idigi < fDigiClusters[clus].digiNr.size(); idigi++) {
      if (fDigiClusters[clus].chanNr[idigi] == ich) {
        Int_t dN = fDigiClusters[clus].digiNr[idigi];
        Int_t cN = fDigiClusters[clus].chanNr[idigi];
        Double_t sA = fDigiClusters[clus].sigADC[idigi];
        Double_t sT = fDigiClusters[clus].sigTDC[idigi];
        fDigiClusters[clus].digiNr[idigi] = fDigiClusters[clus].digiNr[channelPosition];
        fDigiClusters[clus].chanNr[idigi] = fDigiClusters[clus].chanNr[channelPosition];
        fDigiClusters[clus].sigADC[idigi] = fDigiClusters[clus].sigADC[channelPosition];
        fDigiClusters[clus].sigTDC[idigi] = fDigiClusters[clus].sigTDC[channelPosition];
        fDigiClusters[clus].digiNr[channelPosition] = dN;
        fDigiClusters[clus].chanNr[channelPosition] = cN;
        fDigiClusters[clus].sigADC[channelPosition] = sA;
        fDigiClusters[clus].sigTDC[channelPosition] = sT;
        channelPosition++;
      }
    }
  }
}
// -------------------------------------------------------------------------

// -----   Private method PrintClusters   -------------------------------
void PndGemFindClustersTB::PrintClusters()
{

  for (size_t idc = 0; idc < fDigiClusters.size(); idc++) {
    //    if ( fDigiClusters[idc].detId != 568329024 ) continue;
    if (fDigiClusters[idc].cluPos < -0.5)
      continue;
    PrintCluster(idc);
  }
}
// -------------------------------------------------------------------------

// -----   Private method PrintCluster   -------------------------------
void PndGemFindClustersTB::PrintCluster(Int_t clus)
{

  cout << "CLUSTER " << clus << " IN " << fDigiClusters[clus].detId << " AT " << fDigiClusters[clus].cluPos << " CREATED FROM " << fDigiClusters[clus].digiNr.size()
       << " DIGIS, FROM " << fDigiClusters[clus].cluPMn << " TO " << fDigiClusters[clus].cluPMx << " WITH MAXIMUM OF " << fDigiClusters[clus].cluMVl << " AT "
       << fDigiClusters[clus].cluMPs << endl;

  // for ( Int_t idigi = 0 ; idigi < fDigiClusters[clus].digiNr.size() ; idigi++ ) {
  //   cout << idigi << " > "
  // 	 << fDigiClusters[clus].digiNr[idigi] << " @ "
  // 	 << fDigiClusters[clus].chanNr[idigi] << " h "
  // 	 << fDigiClusters[clus].sigADC[idigi] << " t "
  // 	 << fDigiClusters[clus].sigTDC[idigi] << endl;
  // }

  for (Int_t icol = 0; icol < fDigiClusters[clus].cluPMx - fDigiClusters[clus].cluPMn + 3; icol++)
    cout << "-" << flush;
  cout << endl;
  for (Double_t fh = 0.99; fh >= 0.; fh -= 0.247475) {
    cout << "|" << flush;
    Int_t thisDigi = 0;
    Double_t thisADC = 0.;
    for (Int_t ichan = fDigiClusters[clus].cluPMn; ichan <= fDigiClusters[clus].cluPMx; ichan++) {
      thisADC = 0.;
      if (fDigiClusters[clus].chanNr[thisDigi] == ichan) {
        thisADC = fDigiClusters[clus].sigADC[thisDigi];
        thisDigi++;
      }
      if (thisADC > fh * fDigiClusters[clus].cluMVl)
        cout << "*" << flush;
      else
        cout << " " << flush;
    }
    cout << "|" << endl;
  }
  for (Int_t icol = 0; icol < fDigiClusters[clus].cluPMx - fDigiClusters[clus].cluPMn + 3; icol++)
    cout << "-" << flush;
  cout << endl;
}
// -------------------------------------------------------------------------

// -----   Private method SetParContainers   -------------------------------
void PndGemFindClustersTB::SetParContainers()
{

  // Get run and runtime database
  FairRunAna *run = FairRunAna::Instance();
  if (!run)
    LOG(fatal) << "SetParContainers: No analysis run";

  FairRuntimeDb *db = run->GetRuntimeDb();
  if (!db)
    LOG(fatal) << "SetParContainers: No runtime database";

  // Get GEM digitisation parameter container
  fDigiPar = (PndGemDigiPar *)db->getContainer("PndGemDetectors");
}
// -------------------------------------------------------------------------

// -----   Private method Init   -------------------------------------------
InitStatus PndGemFindClustersTB::Init()
{

  // Get input array
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman)
    Fatal("Init", "No FairRootManager");
  fDigis = (TClonesArray *)ioman->GetObject(fInBranchName.Data());

  fFunctor = new TimeGap();

  // Register output array
  // fClusters = new TClonesArray("PndGemDigi", 1000);
  // ioman->Register("GEMCluster", "PndGEM", fClusters, kTRUE);
  fClusters = ioman->Register("GEMCluster", "PndGemCluster", "PndGEM", kTRUE);

  LOG(info) << " " << fName.Data() << "::Init(). There are " << fDigiPar->GetNStations() << " GEM stations.";
  LOG(info) << " " << fName.Data() << "::Init(). Initialization succesfull.";

  PndGemStation *station = (PndGemStation *)fDigiPar->GetStation(0);
  PndGemSensor *sensor = (PndGemSensor *)station->GetSensor(0);
  cout << "sensor out rad is " << sensor->GetOuterRadius() << endl;

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Private method ReInit   -----------------------------------------
InitStatus PndGemFindClustersTB::ReInit()
{

  // Create sectorwise digi sets
  //  MakeSets();

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Public method Finish   ------------------------------------------
void PndGemFindClustersTB::Finish()
{
  if (fClusters)
    fClusters->Clear();

  cout << "-------------------- " << fName.Data() << " : Summary -----------------------" << endl;
  cout << " Events:        " << setw(10) << fTNofEvents << endl;
  cout << " Digis:         " << setw(10) << fTNofDigis << "    ( " << (Double_t)fTNofDigis / ((Double_t)fTNofEvents) << " per event )" << endl;
  cout << " Clusters:      " << setw(10) << fTNofClusters << "    ( " << (Double_t)fTNofClusters / ((Double_t)fTNofEvents) << " per event )" << endl;
  cout << "                       -->    ( " << (Double_t)fTNofDigis / ((Double_t)fTNofClusters) << " digis per cluster )" << endl;
  cout << "---------------------------------------------------------------------" << endl;
  cout << " >>> CF >>> prep  time  = " << fPrepTime << "s      (get data from input array)" << endl;
  cout << " >>> CF >>> create time = " << fCreateTime - fPrepTime << "s    (create clusters, " << fCreateTime << ")" << endl;
  cout << " >>> CF >>> sort  time  = " << fSortTime - fCreateTime << "s      (sort clusters, " << fSortTime << ")" << endl;
  cout << " >>> CF >>> check time  = " << fCheckTime - fSortTime << "s     (check if geometrically fine, " << fCheckTime << ")" << endl;
  cout << " >>> CF >>> ana   time  = " << fAnaTime - fCheckTime << "s       (analyze if not splittable, " << fAnaTime << ")" << endl;
  cout << " >>> CF >>> write time  = " << fWriteTime - fAnaTime << "s     (wrote clusters to output, " << fWriteTime << ")" << endl;
  cout << " >>> CF >>> all   time  = " << fAllTime - fWriteTime << "s       (all time spent in Exec, " << fAllTime << ")" << endl;
  cout << "---------------------------------------------------------------------" << endl;
}
// -------------------------------------------------------------------------

ClassImp(PndGemFindClustersTB)
