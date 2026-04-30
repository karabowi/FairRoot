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
// -----                    PndGemFindHits source file                 -----
// -----                  Created 15/02/2009 by R. Karabowicz          -----
// -------------------------------------------------------------------------

/** PndGemFindHits
 *@author Radoslaw Karabowicz <r.karabowicz@gsi.de>
 *@since 15.02.2009
 *@version 1.0
 **
 ** PANDA task class for finding PndGemHits from PndGemDigis
 ** Task level RECO
 **/

#include "PndGemFindHits.h"

#include "PndGemDigi.h"
#include "PndGemCluster.h"
#include "PndGemDigiPar.h"
#include "PndGemHit.h"
#include "PndGemSensor.h"
#include "PndGemStation.h"
#include "PndGemMonitor.h"

#include "PndDetectorList.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"

#include "TClonesArray.h"
#include "TMath.h"
#include "TH2F.h"

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
PndGemFindHits::PndGemFindHits()
  : PndPersistencyTask("GEM Hit Finder", 1), fMonitor(nullptr), fDigiPar(nullptr), fDigis(nullptr), fHits(nullptr), fHitsTemp(nullptr), fMCPointBranchId(-1), fUseClusters(kFALSE),
    fTimeOrderedDigi(kFALSE), fPrepTime(0.), fSortTime(0.), fCreateTime(0.), fConfirmTime(0.), fActivateTime(0.), fAllTime(0.), fHitWindow(1.5), fTNofEvents(0), fTNofDigis(0),
    fTNofHits(0), fTNofHitsTemp(0)
{
  SetPersistency(kTRUE);
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndGemFindHits::PndGemFindHits(Int_t iVerbose)
  : PndPersistencyTask("GEMFindHits", iVerbose), fMonitor(nullptr), fDigiPar(nullptr), fDigis(nullptr), fHits(nullptr), fHitsTemp(nullptr), fMCPointBranchId(-1),
    fUseClusters(kFALSE), fTimeOrderedDigi(kFALSE), fPrepTime(0.), fSortTime(0.), fCreateTime(0.), fConfirmTime(0.), fActivateTime(0.), fAllTime(0.), fHitWindow(1.5),
    fTNofEvents(0), fTNofDigis(0), fTNofHits(0), fTNofHitsTemp(0)
{
  SetPersistency(kTRUE);
}
// -------------------------------------------------------------------------

// -----   Constructor with name   -----------------------------------------
PndGemFindHits::PndGemFindHits(const char *name, Int_t iVerbose)
  : PndPersistencyTask(name, iVerbose), fMonitor(nullptr), fDigiPar(nullptr), fDigis(nullptr), fHits(nullptr), fHitsTemp(nullptr), fMCPointBranchId(-1), fUseClusters(kFALSE),
    fTimeOrderedDigi(kFALSE), fPrepTime(0.), fSortTime(0.), fCreateTime(0.), fConfirmTime(0.), fActivateTime(0.), fAllTime(0.), fHitWindow(1.5), fTNofEvents(0), fTNofDigis(0),
    fTNofHits(0), fTNofHitsTemp(0)
{
  SetPersistency(kTRUE);
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndGemFindHits::~PndGemFindHits()
{
  if (fHits) {
    fHits->Delete();
    delete fHits;
  }
  if (fHitsTemp) {
    fHitsTemp->Delete();
    delete fHitsTemp;
  }
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndGemFindHits::Exec(Option_t *)
{
  if (fVerbose > 0)
    LOG(info) << " PndGemFindHits::Exec()";
  //  cout << "======== PndGemFindHits::Exec(Event = " << fTNofEvents << " ) ====================" << endl;

  fTimer.Start();

  fTNofEvents++;

  Bool_t warn = kFALSE;

  // Clear output array
  fHits->Delete();
  fHitsTemp->Delete();

  fPrepTime += fTimer.RealTime(); // preparation time
  fTimer.Continue();

  // Sort GEM digis with respect to sectors
  SortDigis();

  fSortTime += fTimer.RealTime();
  fTimer.Continue();

  // Find hits in sectors
  Int_t nDigisF = 0;
  Int_t nDigisB = 0;
  Int_t nHits = 0;
  Int_t nHitsTemp = 0;

  //  cout << "GEM found hits: " << endl;

  Int_t nStations = fDigiPar->GetNStations();
  for (Int_t iStation = 0; iStation < nStations; iStation++) {
    PndGemStation *station = (PndGemStation *)fDigiPar->GetStation(iStation);
    Int_t nDigisFInStation = 0;
    Int_t nDigisBInStation = 0;
    Int_t nHitsInStation = 0;
    Int_t nSensors = station->GetNSensors();
    for (Int_t iSensor = 0; iSensor < nSensors; iSensor++) {
      PndGemSensor *sensor = (PndGemSensor *)station->GetSensor(iSensor);

      set<Int_t> fSet, bSet;
      if (fDigiMapF.find(sensor) == fDigiMapF.end()) {
        LOG(error) << " " << fName << "::Exec: sensor " << sensor->GetSensorNr() << " of station " << station->GetStationNr() << "not found in front map!";
        warn = kTRUE;
        continue;
      }
      fSet = fDigiMapF[sensor];
      if (fDigiMapB.find(sensor) == fDigiMapB.end()) {
        LOG(error) << " " << fName << "::Exec: sensor " << sensor->GetSensorNr() << " of station " << station->GetStationNr() << "not found in back map!";
        warn = kTRUE;
        continue;
      }
      bSet = fDigiMapB[sensor];

      Int_t nDigisFInSensor = fSet.size();
      Int_t nDigisBInSensor = bSet.size();

      Int_t nHitsInSensor;
      // Int_t nHitsInSensor   = FindHits(sensor, fSet, bSet);

      // TimeBased use basically same method..
      if (fTimeOrderedDigi) {
        nHitsInSensor = FindHits(sensor, fSet, bSet); // return non-0 now.. why it was 0??
      } else {
        nHitsInSensor = FindHits2(sensor, fSet, bSet); // test version ok
      }

      if (fVerbose > 2)
        cout << "Sensor " << sensor->GetSensorNr() << ", Digis front " << nDigisFInSensor << ", Digis Back " << nDigisBInSensor << ", Hit combinations " << nHitsInSensor << endl;
      nHitsInStation += nHitsInSensor;
      nDigisFInStation += nDigisFInSensor;
      nDigisBInStation += nDigisBInSensor;
    } // Sector loop

    if (fVerbose > 1)
      cout << "Total for station " << station->GetStationNr() << ": Digis front " << nDigisFInStation << ", digis back " << nDigisBInStation << ", hit combinations "
           << nHitsInStation << endl;
    nDigisB += nDigisBInStation;
    nDigisF += nDigisFInStation;
    nHitsTemp += nHitsInStation;

  } // Station loop

  if (fVerbose > 1)
    cout << "Total for GEM "
         << ": Digis front " << nDigisF << ", digis back " << nDigisB << ", hit combinations " << nHitsTemp << endl;

  fCreateTime += fTimer.RealTime();
  fTimer.Continue();

  // can only confirm hits using clusters fromt the same event
  if (fTimeOrderedDigi) {
    if (fUseClusters) {
      ConfirmHits();
      fConfirmTime += fTimer.RealTime();
      fTimer.Continue();
      // there should be another funciton, that will confirm the hits using fired strips in previous events, that could have obscured the cluster/hit finding in this event because
      // of the small time difference activate digis from the clusters in the PndGemMonitor
      ActivateDigis();
      fActivateTime += fTimer.RealTime();
      fTimer.Continue();
      // check the digis in the previous events...
    }
  } else {
    ConfirmHits2(); // test version ok
  }

  if (fVerbose > 1) {
    //    cout << "-I- PndGemFindHits::Exec() Created " << fHits->GetEntriesFast() << " hits from "
    //<< nDigisF << " front and " << nDigisB << " back digis" << endl;
    cout << endl;
    LOG(info) << " " << fName << ":Event summary";
    cout << "    Active channels front side: " << nDigisF << endl;
    cout << "    Active channels back side : " << nDigisB << endl;
    cout << "    Hits created              : " << fHits->GetEntriesFast() << endl;
    cout << "    Real time                 : " << fTimer.RealTime() << endl;
  }
  if (fVerbose == 1) {
    if (warn)
      cout << "- ";
    else
      cout << "+ ";
    cout << setw(15) << left << fName << ": " << setprecision(4) << setw(8) << fixed << right << fTimer.RealTime() << " s, digis " << nDigisF << " / " << nDigisB
         << ", hits: " << nHits << endl;
  }

  //  cout << "         PndGemFindHits::Exec. Created " << fHits->GetEntries() << " hits out of " << fDigis->GetEntries() << "  digis/clusters." << endl;

  fAllTime += fTimer.RealTime();

  fTimer.Stop();
}
// -------------------------------------------------------------------------

// -----   Private method SetParContainers   -------------------------------
void PndGemFindHits::SetParContainers()
{

  // Get run and runtime database
  FairRun *run = FairRun::Instance();
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
InitStatus PndGemFindHits::Init()
{

  // Get input array
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman)
    Fatal("Init", "No FairRootManager");

  if (fUseClusters)
    fDigis = (TClonesArray *)ioman->GetObject("GEMCluster");
  else
    fDigis = (TClonesArray *)ioman->GetObject("GEMDigi");

  fMCPointBranchId = ioman->GetBranchId("GEMPoint");

  // Register output array
  fHits = new TClonesArray("PndGemHit", 1000);
  ioman->Register("GEMHit", "Hit in GEM", fHits, GetPersistency());

  // Test Register output array
  fHitsTemp = new TClonesArray("PndGemHit", 1000);
  ioman->Register("GEMHitTemp", "temp Hit in GEM", fHitsTemp, kFALSE);

  // Create sectorwise digi sets
  MakeSets();

  LOG(info) << " " << fName.Data() << "::Init(). There are " << fDigiPar->GetNStations() << " GEM stations.";
  LOG(info) << " " << fName.Data() << "::Init(). Initialization succesfull.";

  PndGemStation *station = (PndGemStation *)fDigiPar->GetStation(0);
  PndGemSensor *sensor = (PndGemSensor *)station->GetSensor(0);
  cout << "sensor out rad is " << sensor->GetOuterRadius() << endl;

  // fChargeCorrHist = new TH2F("fChargeCorrHist","fChargeCorrHist",1000,0.,100.,1000,0.,100.);
  // fChargeDiffHist = new TH2F("fChargeDiffHist","fChargeDiffHist",1000,0.,100.,1000,-5.,5.);

  // Open GEM Monitor
  fMonitor = PndGemMonitor::Instance();

  for (Int_t istat = 0; istat < fDigiPar->GetNStations(); istat++) {
    station = (PndGemStation *)fDigiPar->GetStation(istat);
    for (Int_t isens = 0; isens < station->GetNSensors(); isens++) {
      sensor = (PndGemSensor *)station->GetSensor(isens);
      fMonitor->CreateSensorMonitor(*sensor);
    }
  }

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Private method ReInit   -----------------------------------------
InitStatus PndGemFindHits::ReInit()
{

  // Create sectorwise digi sets
  MakeSets();

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Private method MakeSets   ---------------------------------------
void PndGemFindHits::MakeSets()
{

  fDigiMapF.clear();
  fDigiMapB.clear();
  Int_t nStations = fDigiPar->GetNStations();
  for (Int_t iStation = 0; iStation < nStations; iStation++) {
    PndGemStation *station = (PndGemStation *)fDigiPar->GetStation(iStation);
    Int_t nSensors = station->GetNSensors();
    for (Int_t iSensor = 0; iSensor < nSensors; iSensor++) {
      PndGemSensor *sensor = (PndGemSensor *)station->GetSensor(iSensor);
      set<Int_t> a;
      fDigiMapF[sensor] = a;
      set<Int_t> b;
      fDigiMapB[sensor] = b;
    }
  }
}
// -------------------------------------------------------------------------

// -----   Private method ActivateDigis   --------------------------------------
void PndGemFindHits::ActivateDigis()
{
  PndGemCluster *cluster;
  for (Int_t iclus = 0; iclus < fDigis->GetEntries(); iclus++) {
    cluster = (PndGemCluster *)fDigis->At(iclus);

    fMonitor->EnableCluster(fTNofEvents, iclus, cluster);
  }
}
// -------------------------------------------------------------------------

// -----   Private method ConfirmHits   --------------------
void PndGemFindHits::ConfirmHits()
{
  if (fVerbose > 0)
    LOG(info) << " PndGemFindHits::ConfirmHits()";
  Bool_t printInfo = kFALSE;
  if (fVerbose > 1)
    printInfo = kTRUE;

  PndGemSensor *sensor = nullptr;
  PndGemHit *hit = nullptr;
  PndGemCluster *cluster = nullptr;

  for (Int_t ihit = 0; ihit < fHits->GetEntriesFast(); ihit++) {
    hit = (PndGemHit *)fHits->At(ihit);

    sensor = (PndGemSensor *)fDigiPar->GetSensor(hit->GetStationNr(), (3 - hit->GetSensorNr()));
    Double_t hitProjX = hit->GetX() * (sensor->GetZ0() / hit->GetZ());
    Double_t hitProjY = hit->GetY() * (sensor->GetZ0() / hit->GetZ());
    // if ( abs(hit->GetX()+3.8)<0.1 &&
    // 	 abs(hit->GetY()-2.0)<0.1 &&
    // 	 hit->GetStationNr()==2 )
    //   printInfo = kTRUE;
    // else
    //   printInfo = kFALSE;

    if (printInfo) {
      cout << "COMPARING HIT " << ihit << " at " << hit->GetX() << "," << hit->GetY() << "," << hit->GetZ() << " @ " << hit->GetTimeStamp() << endl;
    }

    Bool_t hitConfirmed[2] = {kFALSE, kFALSE};
    Int_t channelNr[2] = {sensor->GetChannel(hitProjX, hitProjY, 0), sensor->GetChannel(hitProjX, hitProjY, 1)};

    // check in previous events....
    for (Int_t iside = 0; iside < 2; iside++) {
      Double_t lastChannelActivateTime = fMonitor->ChannelLastActiveAt(hit->GetStationNr(), (3 - hit->GetSensorNr()), iside, channelNr[iside]);
      if (printInfo) {
        cout << " on " << hit->GetStationNr() << "." << 3 - hit->GetSensorNr() << "." << iside << " channel " << channelNr[iside] << " was last active at "
             << lastChannelActivateTime << endl;
        //	fMonitor->Print();
      }
      if (lastChannelActivateTime < 0)
        continue;
      if (hit->GetTimeStamp() - lastChannelActivateTime < 100 && hit->GetTimeStamp() - lastChannelActivateTime > -10) {
        if (printInfo)
          cout << "HIT CONFIRMED ON SIDE " << iside << " IN SOME PREVIOUS EVENT (" << hit->GetTimeStamp() - lastChannelActivateTime << " ns ago)" << endl;
        hitConfirmed[iside] = kTRUE;
      }
    }

    if (hitConfirmed[0] == kFALSE || hitConfirmed[1] == kFALSE) {
      for (Int_t iclus = 0; iclus < fDigis->GetEntries(); iclus++) {
        cluster = (PndGemCluster *)fDigis->At(iclus);

        if (cluster->GetTimeStamp() < hit->GetTimeStamp() - 111. || cluster->GetTimeStamp() > hit->GetTimeStamp() + 11.)
          continue;

        if (cluster->GetStationNr() != hit->GetStationNr())
          continue;
        if (cluster->GetSensorNr() == hit->GetSensorNr())
          continue;

        // should have some function in sensor for that:
        if (channelNr[cluster->GetSide()] <= cluster->GetClusterEnd() && channelNr[cluster->GetSide()] >= cluster->GetClusterBeg()) {
          hitConfirmed[cluster->GetSide()] = kTRUE;
          if (printInfo) {
            cout << "HIT CONFIRMED ON SIDE " << cluster->GetSide() << " BY CLUSTER " << iclus << " at " << (cluster->GetSide() == 0 ? "F" : "B") << "side on "
                 << cluster->GetStationNr() << "." << cluster->GetSensorNr() << " from " << (cluster->GetClusterBeg()) << " to " << (cluster->GetClusterEnd()) << " **** "
                 << sensor->GetChannel(hitProjX, hitProjY, cluster->GetSide()) << ", time_diff = " << hit->GetTimeStamp() << " - " << cluster->GetTimeStamp() << "ns" << endl;
          }
        }
      }
    }

    if (hitConfirmed[0] == kTRUE && hitConfirmed[1] == kTRUE) {
      if (printInfo)
        cout << "GOOD HIT" << endl;
      ((PndGemHit *)fHits->At(ihit))->SetCharge(10.);
    } else {
      if (printInfo)
        cout << "   NO CLUSTER MATCHES TO THIS HIT" << endl;
    }
  }
}
// -------------------------------------------------------------------------

// -----   Private method ConfirmHits   --------------------
// Check corresponding sensor and store hit when both sensors have same location of hits
// Calculate acceptable width of hit matching based on Dx Dy
// So far only for EB.. Channel activation is not considered...
void PndGemFindHits::ConfirmHits2()
{
  if (fVerbose > 0)
    LOG(info) << " PndGemFindHits::ConfirmHits2()";

  PndGemHit *hitTemp = nullptr;  // hit in all combination of clusters to read
  PndGemHit *hitTemp2 = nullptr; // hit in all combination of clusters to compare
  PndGemSensor *sensor = nullptr;

  Double_t xHit[2], yHit[2], zHit[2];
  Double_t dx[2], dy[2], dz[2];
  TVector3 pos, dpos;

  Int_t nHits = fHits->GetEntriesFast();
  Int_t sensorDetId, hitDetId;
  Bool_t hitMatch = kFALSE;
  Bool_t hitMatch2 = kFALSE;

  // window to find corresponding hit on other sensor
  //   Double_t xHitW=1.5; //3rd attempt, 1 seems to be too narrow (7% missed), 2 seems too wide (10% more Hits)
  //   Double_t yHitW=1.5;
  Double_t xHitW = fHitWindow;
  Double_t yHitW = fHitWindow;

  if (!fUseClusters) {
    if (fVerbose > 1)
      cout << " Hit finding window is constant: xHitW=" << xHitW << " yHitW=" << yHitW << endl;
  }

  for (Int_t ihitTemp = 0; ihitTemp < fHitsTemp->GetEntriesFast(); ihitTemp++) {
    hitTemp = (PndGemHit *)fHitsTemp->At(ihitTemp);

    Int_t hitTempDetId = hitTemp->GetDetectorID();
    Int_t testGemHit = hitTempDetId & static_cast<int>(DetectorType::kGemHit) << 21;
    if (fVerbose > 1) {
      cout << "Check DetectorType::kGemHit bit of hitTempDetId=" << hitTempDetId << " DetectorType::kGemHit=" << static_cast<int>(DetectorType::kGemHit)
           << " masked value testGemHit=" << testGemHit << endl;
    }
    if (testGemHit != 0) {
      if (fVerbose > 1) {
        cout << "ihitTemp " << ihitTemp << " : already used... skip" << endl;
      }
      continue;
    }

    Int_t staNr = hitTemp->GetStationNr();
    Int_t senNr = hitTemp->GetSensorNr();

    xHit[0] = hitTemp->GetX();
    yHit[0] = hitTemp->GetY();
    zHit[0] = hitTemp->GetZ();
    dx[0] = hitTemp->GetDx();
    dy[0] = hitTemp->GetDy();
    dz[0] = hitTemp->GetDz();

    sensor = (PndGemSensor *)fDigiPar->GetSensor(hitTemp->GetStationNr(), hitTemp->GetSensorNr());
    sensorDetId = sensor->GetDetectorId();
    Double_t senD = sensor->GetD(); // sensor thickness 1cm (?)

    if (fVerbose > 1) {
      cout << "COMPARING HitTemp ihitTemp " << ihitTemp << " of staNr " << staNr << " senNr " << senNr << " at (" << xHit[0] << "," << yHit[0] << "," << zHit[0] << ") time "
           << hitTemp->GetTimeStamp() << " dx " << dx[0] << " dy " << dy[0] << " sensorDetId " << sensorDetId << " senThick " << senD << endl;
    }

    hitMatch = kFALSE;
    for (Int_t ihitTemp2 = ihitTemp + 1; ihitTemp2 < fHitsTemp->GetEntriesFast(); ihitTemp2++) {
      hitTemp2 = (PndGemHit *)fHitsTemp->At(ihitTemp2);
      if (fVerbose > 1)
        cout << "WITH ihitTemp2 " << ihitTemp2;

      hitMatch2 = kFALSE;
      Int_t hitTemp2DetId = hitTemp2->GetDetectorID();
      Int_t test2GemHit = hitTemp2DetId & static_cast<int>(DetectorType::kGemHit) << 21;
      if (fVerbose > 2) {
        cout << " hitTemp2DetId=" << hitTempDetId << " DetectorType::kGemHit=" << static_cast<int>(DetectorType::kGemHit) << " masked value test2GemHit=" << test2GemHit << endl;
      }
      //       if(  test2GemHit != 0 ) {
      // 	if ( fVerbose > 1 ) cout << " ihitTemp2 " << ihitTemp2 << " : already used... skip" <<endl;
      // 	continue;
      //       }

      Int_t staNr2 = hitTemp2->GetStationNr();
      if (fVerbose > 1)
        cout << " ihitTemp2 " << ihitTemp2 << " staNr " << staNr2;
      if (staNr != staNr2) {
        if (fVerbose > 1)
          cout << " : diff Station .. skip " << endl;
        continue;
      }
      Int_t senNr2 = hitTemp2->GetSensorNr();
      if (fVerbose > 1)
        cout << " of senNr " << senNr2;
      if (senNr == senNr2) {
        if (fVerbose > 1)
          cout << " : same sensor.. skip " << endl;
        continue;
      }
      xHit[1] = hitTemp2->GetX();
      yHit[1] = hitTemp2->GetY();
      zHit[1] = hitTemp2->GetZ();
      dx[1] = hitTemp2->GetDx();
      dy[1] = hitTemp2->GetDy();
      dz[1] = hitTemp2->GetDz();
      if (fVerbose > 1) {
        cout << " at (" << xHit[1] << "," << yHit[1] << "," << zHit[1] << ") time " << hitTemp2->GetTimeStamp() << " dx " << dx[1] << " dy " << dy[1] << endl;
      }

      // calc window to search hit on other sensor based on Dx Dy of Hit
      // assuming straight path (NOT assuming path from target)
      if (fUseClusters) {
        xHitW = TMath::Sqrt(12.) * dx[0] * TMath::Abs(zHit[0] - zHit[1]) / senD + TMath::Sqrt(12.) * dx[0] * 0.5;
        yHitW = TMath::Sqrt(12.) * dy[0] * TMath::Abs(zHit[0] - zHit[1]) / senD + TMath::Sqrt(12.) * dx[0] * 0.5;
        if (fVerbose > 1) {
          cout << " Hit finding window based on dx dy: xHitW=" << xHitW << " yHitW=" << yHitW << endl;
        }
      }

      if (xHit[0] - xHitW < xHit[1] && xHit[1] < xHit[0] + xHitW) {
        if (yHit[0] - yHitW < yHit[1] && yHit[1] < yHit[0] + yHitW) {

          hitTempDetId = hitTemp->GetDetectorID();
          testGemHit = hitTempDetId & static_cast<int>(DetectorType::kGemHit) << 21;
          if (testGemHit != 0) {
            if (fVerbose > 1) {
              cout << "ihitTemp " << ihitTemp << " : already stored... " << endl;
            }
          } else {
            pos.SetXYZ(xHit[0], yHit[0], zHit[0]);
            dpos.SetXYZ(dx[0], dy[0], dz[0]);
            hitDetId = hitTemp->GetDetectorID() | static_cast<int>(DetectorType::kGemHit) << 21;

            TString fromStr = "GEMDigi";
            if (fUseClusters)
              fromStr = "GEMCluster";

            new ((*fHits)[nHits++]) PndGemHit(hitDetId, pos, dpos, hitTemp->GetCharge(), hitTemp->GetTimeStamp(), hitTemp->GetDigiNr(0), hitTemp->GetDigiNr(1), hitTemp->GetDr(),
                                              hitTemp->GetDp(), hitTemp->GetRefIndex(), fromStr);
            if (fVerbose > 1) {
              cout << "ihitTemp " << ihitTemp << "  xHit yHit zHit " << pos.X() << " / " << pos.Y() << " / " << pos.Z() << "  stored as " << nHits << " from " << fromStr.Data()
                   << "." << hitTemp->GetDigiNr(0) << " / " << hitTemp->GetDigiNr(1) << endl;
            }
            ((PndGemHit *)fHitsTemp->At(ihitTemp))->SetDetectorID(hitDetId);
            fTNofHits++;
          }

          if (test2GemHit != 0) {
            if (fVerbose > 1) {
              cout << "ihitTemp2 " << ihitTemp2 << " : already stored... " << endl;
            }
          } else {
            pos.SetXYZ(xHit[1], yHit[1], zHit[1]);
            dpos.SetXYZ(dx[1], dy[1], dz[1]);
            hitDetId = hitTemp2->GetDetectorID() | static_cast<int>(DetectorType::kGemHit) << 21;

            TString fromStr = "GEMDigi";
            if (fUseClusters)
              fromStr = "GEMCluster";

            new ((*fHits)[nHits++]) PndGemHit(hitDetId, pos, dpos, hitTemp2->GetCharge(), hitTemp2->GetTimeStamp(), hitTemp2->GetDigiNr(0), hitTemp2->GetDigiNr(1),
                                              hitTemp2->GetDr(), hitTemp2->GetDp(), hitTemp2->GetRefIndex(), fromStr);

            if (fVerbose > 1) {
              cout << "ihitTemp2 " << ihitTemp << "  xHit yHit zHit " << pos.X() << " / " << pos.Y() << " / " << pos.Z() << "  stored as " << nHits << " from " << fromStr.Data()
                   << "." << hitTemp2->GetDigiNr(0) << " / " << hitTemp2->GetDigiNr(1) << endl;
            }
            ((PndGemHit *)fHitsTemp->At(ihitTemp2))->SetDetectorID(hitDetId);
            fTNofHits++;
          }

          hitMatch = kTRUE;
          hitMatch2 = kTRUE;
        }
      }
      if (hitMatch2 == kTRUE) {
        if (fVerbose > 1)
          cout << "GOOD HIT" << endl;
        // break;
      } else {
        if (fVerbose > 2)
          cout << "NO MATCHE TO THIS HIT ihitTemp2 = " << ihitTemp2 << endl;
      }
    }
    if (hitMatch == kFALSE)
      if (fVerbose > 1)
        cout << "NO MATCHES OF HIT ihitTemp = " << ihitTemp << endl;
  }
}
// -------------------------------------------------------------------------

// // -----   Private method ConfirmHits   --------------------------------------
// void PndGemFindHits::ConfirmHits() {
//   PndGemSensor* sensor = nullptr;
//   PndGemHit* hit = nullptr;
//   PndGemCluster* cluster = nullptr;

//   for ( Int_t ihit = 0 ; ihit < fHits->GetEntriesFast() ; ihit++ ) {
//     hit = (PndGemHit*)fHits->At(ihit);

//     cout << "trying to confirm hit " << ihit << " at "
// 	 << hit->GetX() << ","
// 	 << hit->GetY() << ","
// 	 << hit->GetZ() << " @ "
// 	 << hit->GetTimeStamp() << endl;

//     sensor = (PndGemSensor*)fDigiPar->GetSensor(hit->GetStationNr(),
// 						(3-hit->GetSensorNr ()));
//     Double_t hitProjX = hit->GetX()*(sensor->GetZ0()/hit->GetZ());
//     Double_t hitProjY = hit->GetY()*(sensor->GetZ0()/hit->GetZ());

//     cout << "   with hits at sensor @ z = " << sensor->GetZ0() << endl;
//     Int_t hitConfirmed = 0;

//     }
//     if ( hitConfirmed < 2 )
//       cout << "   NO CLUSTER MATCHES TO THIS HIT" << endl;
//     else
//       ((PndGemHit*)fHits->At(ihit))->SetCharge(10.);
//   }
// }
// -------------------------------------------------------------------------

// -----   Private method SortDigis   --------------------------------------
void PndGemFindHits::SortDigis()
{

  // Check input array
  if (!fDigis) {
    LOG(error) << " " << fName << "::SortDigis: No input array!";
    return;
  }

  // Clear sensor digi sets
  map<PndGemSensor *, set<Int_t>>::iterator mapIt;
  for (mapIt = fDigiMapF.begin(); mapIt != fDigiMapF.end(); mapIt++)
    ((*mapIt).second).clear();
  for (mapIt = fDigiMapB.begin(); mapIt != fDigiMapB.end(); mapIt++)
    ((*mapIt).second).clear();

  // Fill digis into sets
  PndGemDigi *digi = nullptr;
  // PndGemCluster* clust = nullptr; //[R.K.01/2017]unused variable?
  PndGemSensor *sensor = nullptr;
  Int_t stationNr = -1;
  Int_t sensorNr = -1;
  Int_t iSide = -1;
  Int_t nDigis = fDigis->GetEntriesFast();

  fTNofDigis += nDigis;

  for (Int_t iDigi = 0; iDigi < nDigis; iDigi++) {
    digi = (PndGemDigi *)fDigis->At(iDigi);
    // clust = (PndGemCluster*) fDigis->At(iDigi); //[R.K.01/2017]unused variable?
    stationNr = digi->GetStationNr();
    sensorNr = digi->GetSensorNr();
    iSide = digi->GetSide();

    // Strange...this case to do continue..??
    // it doesn't exist in apr13, but exists already on trunk r23148.
    //(this code is taken from r24527). Comment out ok?
    // if ( !( sensorNr == 1 && iSide == 0 ) && clust->GetClusterEnd()-clust->GetClusterBeg() <= 1 ) {
    //  //      cout << "THE DIGI " << iDigi << ", DET ID = " << digi->GetDetectorId() << " in STATION " << stationNr << " SENSOR " << sensorNr << " SIDE " << iSide << " -> " <<
    //  clust->GetClusterBeg() << " : " << clust->GetClusterEnd() << endl; continue;
    //}

    //    cout << "THE DIGI " << iDigi << ", DET ID = " << digi->GetDetectorId() << " in STATION " << stationNr << " SENSOR " << sensorNr << " SIDE " << iSide << endl;
    //    cout << "digi #" << iDigi+1 << " in station " << stationNr << " sensor " << sensorNr << " channel " << digi->GetChannelNr() << endl;

    sensor = fDigiPar->GetSensor(stationNr, sensorNr);
    //    cout << "Looking for station " << stationNr << " sensor " << sensorNr << " side " << iSide << endl;
    if (iSide == 0) {
      if (fDigiMapF.find(sensor) == fDigiMapF.end()) {
        cerr << "-E- " << fName << "::SortDigits:: sensor " << sensorNr << " of station " << stationNr << " not found in digi scheme (F)!" << endl;
        continue;
      }
      fDigiMapF[sensor].insert(iDigi);
    } else if (iSide == 1) {
      if (fDigiMapB.find(sensor) == fDigiMapB.end()) {
        cerr << "-E- " << fName << "::SortDigits:: sensor " << sensorNr << " of station " << stationNr << " not found in digi scheme (B)!" << endl;
        continue;
      }
      fDigiMapB[sensor].insert(iDigi);
    }
  }
}
// -------------------------------------------------------------------------

// -----   Private method FindHits   ---------------------------------------
// one should add case of UseCluster to carry information of cluster Beg and End (or?)
Int_t PndGemFindHits::FindHits(PndGemSensor *sensor, set<Int_t> &fSet, set<Int_t> &bSet)
{

  // Int_t    iType  = sensor->GetType(); //[R.K. 01/2017] unused variable?

  Double_t sigmaX = 1., sigmaY = 2.;

  Int_t iDigiF = -1;
  Int_t iDigiB = -1;
  Double_t iChanF = -1;
  Double_t iChanB = -1;
  Int_t nHits = fHits->GetEntriesFast();
  Int_t nHitsInSensor = 0;
  Double_t xHit;
  Double_t yHit;
  Double_t zHit;
  Double_t dx, dy, dr, dp;
  TVector3 pos, dpos;
  PndGemDigi *digiF = nullptr;
  PndGemDigi *digiB = nullptr;
  // PndGemCluster* clusterB = nullptr; //[R.K.01/2017]unused variable?

  set<Int_t>::iterator it1;
  set<Int_t>::iterator it2;

  for (it1 = fSet.begin(); it1 != fSet.end(); it1++) {
    iDigiF = (*it1);
    digiF = (PndGemDigi *)fDigis->At(iDigiF);
    if (!digiF) {
      LOG(warn) << " " << GetName() << "::FindHits: Invalid digi index " << iDigiF << " in front set of sensor " << sensor->GetDetectorName();
      continue;
    }
    iChanF = digiF->GetChannelNr();
    for (it2 = bSet.begin(); it2 != bSet.end(); it2++) {
      iDigiB = (*it2);
      digiB = (PndGemDigi *)fDigis->At(iDigiB);
      // clusterB = (PndGemCluster*) fDigis->At(iDigiB); //[R.K.01/2017]unused variable?
      if (!digiB) {
        LOG(warn) << " " << GetName() << "::FindHits: Invalid digi index " << iDigiB << " in front set of sensor " << sensor->GetDetectorName();
        continue;
      }
      iChanB = digiB->GetChannelNr();
      Int_t sensorDetId = sensor->Intersect(iChanF, iChanB, xHit, yHit, zHit, dx, dy, dr, dp);
      //      cout << "intersecting channels " << iChanF << " and " << iChanB << " gave " << sensorDetId << endl;
      // 	cout << "got the following position from sensor " << sensorDetId << " : ("
      // 	     << xHit << ", " << yHit << ", " << zHit << ")" << endl;
      if (sensorDetId == -1)
        continue;

      Double_t rad = TMath::Sqrt(xHit * xHit + yHit * yHit);
      if (rad < sensor->GetInnerRadius() || rad > sensor->GetOuterRadius()) {
        cout << " point " << xHit << "," << yHit << " (" << rad << ") is still ok??? at station " << sensor->GetStationNr() << "." << sensor->GetSensorNr() << endl;
      }

      // Don't we use information of Cluster pos Min Max to set error of Hit??
      sigmaX = dx;
      sigmaY = dy;

      //       if ( fUseClusters ) {
      // 	zHit += sensor->GetD()/2.;
      // 	if ( sensor->GetType() )
      // 	  zHit -= sensor->GetD();
      //       }
      //      cout << "what about sensorz0 = " << sensor->GetZ0() << " or zHit = " << zHit << endl;

      pos.SetXYZ(xHit, yHit, zHit);
      dpos.SetXYZ(sigmaX, sigmaY, sensor->GetD());

      //      cout << "SETTING ERROR TO " << sigmaX << " " << sigmaY << " " << sensor->GetD() << " at " << zHit << endl;

      //      if ( TMath::Abs(xHit) < 2. ) continue;

      Int_t refIndex = -1;
      for (Int_t irf1 = digiF->GetNIndices() - 1; irf1 >= 0; irf1--) {
        if (digiF->GetLink(irf1).GetType() != fMCPointBranchId)
          continue;
        for (Int_t irf2 = digiB->GetNIndices() - 1; irf2 >= 0; irf2--) {
          if (digiB->GetLink(irf2).GetType() != fMCPointBranchId)
            continue;
          if (digiF->GetIndex(irf1) == digiB->GetIndex(irf2))
            refIndex = digiF->GetIndex(irf1);
        }
      }

      Int_t hitDetId = sensorDetId | static_cast<int>(DetectorType::kGemHit) << 21;

      //      cout << nHits << " : " << pos.X() << " " << pos.Y() << " " << pos.Z() << endl;

      // fChargeCorrHist->Fill(digiF->GetCharge(),digiB->GetCharge());
      // fChargeDiffHist->Fill((digiF->GetCharge()+digiB->GetCharge())/2.,
      // 			    (digiF->GetCharge()-digiB->GetCharge()));

      //      if ( TMath::Abs(digiF->GetCharge()-digiB->GetCharge()) > 0.02+(digiF->GetCharge()+digiB->GetCharge())/200. ||

      // How critical are they??? may I open more??
      if (TMath::Abs(digiF->GetCharge() - digiB->GetCharge()) > 5. || TMath::Abs(digiF->GetTimeStamp() - digiB->GetTimeStamp()) > 5.)
        continue;

      TString fromStr = "GEMDigi";
      if (fUseClusters)
        fromStr = "GEMCluster";
      new ((*fHits)[nHits++]) PndGemHit(hitDetId, pos, dpos, (digiF->GetCharge() + digiB->GetCharge()) / 2., (digiF->GetTimeStamp() + digiB->GetTimeStamp()) / 2., iDigiF, iDigiB,
                                        dr, dp, refIndex, fromStr);

      fTNofHits++;
      nHitsInSensor++;
    }
  }

  //  return 0;
  return nHitsInSensor;
}
// -------------------------------------------------------------------------

// -----   Private method FindHits   ---------------------------------------
// create hits for all combination and store in fHitsTemp..
// carry information of cluster width to dpos..
Int_t PndGemFindHits::FindHits2(PndGemSensor *sensor, set<Int_t> &fSet, set<Int_t> &bSet)
{
  if (fVerbose > 0)
    LOG(info) << " PndGemFindHits::FindHits2(). Station " << sensor->GetStationNr() << " sensor " << sensor->GetSensorNr();

  // Int_t    iType  = sensor->GetType(); //[R.K. 01/2017] unused variable?

  Double_t sigmaX = 1., sigmaY = 2., sigmaZ;
  Double_t sigmaR, sigmaP;

  Int_t iDigiF = -1;
  Int_t iDigiB = -1;
  Double_t iChanF = -1;
  Double_t iChanB = -1;
  Double_t iChanFMn, iChanFMx;
  Double_t iChanBMn, iChanBMx;
  Int_t nHitsTemp = fHitsTemp->GetEntriesFast();
  Int_t nHitsInSensor = 0;
  Double_t xHit;
  Double_t yHit;
  Double_t zHit;
  Double_t dx, dy, dr, dp;
  TVector3 pos, dpos;
  PndGemDigi *digiF = nullptr;
  PndGemDigi *digiB = nullptr;
  PndGemCluster *clusterF = nullptr;
  PndGemCluster *clusterB = nullptr;

  Double_t xMnMn, xMxMx, xMnMx, xMxMn;
  Double_t yMnMn, yMxMx, yMnMx, yMxMn;

  set<Int_t>::iterator it1;
  set<Int_t>::iterator it2;

  for (it1 = fSet.begin(); it1 != fSet.end(); it1++) {
    iDigiF = (*it1);
    digiF = (PndGemDigi *)fDigis->At(iDigiF);
    clusterF = (PndGemCluster *)fDigis->At(iDigiF);
    if (!digiF) {
      LOG(warn) << " " << GetName() << "::FindHits: Invalid digi index " << iDigiF << " in front set of sensor " << sensor->GetDetectorName();
      continue;
    }
    iChanF = digiF->GetChannelNr();
    if (fUseClusters) {
      iChanFMn = clusterF->GetClusterBeg();
      iChanFMx = clusterF->GetClusterEnd();
    }
    for (it2 = bSet.begin(); it2 != bSet.end(); it2++) {
      iDigiB = (*it2);
      digiB = (PndGemDigi *)fDigis->At(iDigiB);
      clusterB = (PndGemCluster *)fDigis->At(iDigiB);
      if (!digiB) {
        LOG(warn) << " " << GetName() << "::FindHits: Invalid digi index " << iDigiB << " in front set of sensor " << sensor->GetDetectorName();
        continue;
      }
      iChanB = digiB->GetChannelNr();
      if (fUseClusters) {
        iChanBMn = clusterB->GetClusterBeg();
        iChanBMx = clusterB->GetClusterEnd();
      }

      Int_t sensorDetId = sensor->Intersect(iChanF, iChanB, xHit, yHit, zHit, dx, dy, dr, dp);
      if (fVerbose > 1) {
        cout << "Intersect chF " << iChanF << " chB " << iChanB << " detID " << sensorDetId << endl;
        cout << "at (" << xHit << ", " << yHit << ", " << zHit << ")"
             << "dx " << dx << " dy " << dy << " dr " << dr << " dp " << dp << endl;
      }
      if (sensorDetId == -1) {
        if (fVerbose > 2) {
          cout << "no intersect.. skip " << endl;
        }
        continue;
      }

      pos.SetXYZ(xHit, yHit, zHit);
      Double_t rad = TMath::Sqrt(xHit * xHit + yHit * yHit);
      if (rad < sensor->GetInnerRadius() || rad > sensor->GetOuterRadius()) {
        LOG(warn) << " !! point " << xHit << "," << yHit << " (" << rad << ") is still ok??? at station " << sensor->GetStationNr() << "." << sensor->GetSensorNr();
      }
      // store resolution of the hit pos
      sigmaX = dx;             // basically reso of single strip
      sigmaY = dy;             // basically reso of single strip
      sigmaZ = sensor->GetD(); //
      sigmaR = dr;             // basically reso of single strip
      sigmaP = dp;             // basically reso of single strip

      // take channel Beg and End for dpos when Cluster is used
      // Diagonal lines of the overlaped part are used to extract dx dy and longer one is taken
      // no check on dr dp, no comparison with original resolution sigmaX sigmaY.. ok?
      if (fUseClusters) {
        Int_t testMnMn = sensor->Intersect(iChanFMn, iChanBMn, xMnMn, yMnMn, zHit, dx, dy, dr, dp);
        Int_t testMxMx = sensor->Intersect(iChanFMx, iChanBMx, xMxMx, yMxMx, zHit, dx, dy, dr, dp);
        Int_t testMnMx = sensor->Intersect(iChanFMn, iChanBMx, xMnMx, yMnMx, zHit, dx, dy, dr, dp);
        Int_t testMxMn = sensor->Intersect(iChanFMx, iChanBMn, xMxMn, yMxMn, zHit, dx, dy, dr, dp);
        Double_t dx1 = -1;
        Double_t dx2 = -1;
        Double_t dy1 = -1;
        Double_t dy2 = -1;
        if (testMnMn != -1 && testMxMx != -1) {
          dx1 = TMath::Abs(xMnMn - xMxMx);
          dy1 = TMath::Abs(yMnMn - yMxMx);
          if (testMnMx != -1 && testMxMn != -1) {
            dx2 = TMath::Abs(xMnMx - xMxMn);
            dy2 = TMath::Abs(yMnMx - yMxMn);
            sigmaX = TMath::Max(dx1, dx2) / TMath::Sqrt(12.);
            sigmaY = TMath::Max(dy1, dy2) / TMath::Sqrt(12.);
            if (fVerbose > 1) {
              cout << "fUseCluster is TRUE: dx dy of clusters " << sigmaX << " " << sigmaY << " are calculated for Hits" << endl;
            }
          }
        }
      }
      dpos.SetXYZ(sigmaX, sigmaY, sigmaZ);

      Int_t refIndex = -1;
      for (Int_t irf1 = digiF->GetNIndices() - 1; irf1 >= 0; irf1--) {
        if (digiF->GetLink(irf1).GetType() != fMCPointBranchId)
          continue;
        for (Int_t irf2 = digiB->GetNIndices() - 1; irf2 >= 0; irf2--) {
          if (digiB->GetLink(irf2).GetType() != fMCPointBranchId)
            continue;
          if (digiF->GetIndex(irf1) == digiB->GetIndex(irf2))
            refIndex = digiF->GetIndex(irf1);
        }
      }

      // Hit bit DetectorType::kGemHit will be put when the Hit matches on both sensor.. or problem?
      Int_t hitDetId = sensorDetId;
      //      Int_t hitDetId = sensorDetId | DetectorType::kGemHit << 21;

      // How critical are they??? may I open more??
      //   SO FAR COMMENTED !! ok??
      //       if ( TMath::Abs(digiF->GetCharge()-digiB->GetCharge()) > 5. ||
      // 	   TMath::Abs(digiF->GetTimeStamp()-digiB->GetTimeStamp()) > 5. )
      // 	continue;

      TString fromStr = "GEMDigi";
      Double_t hitTimeStamp = (digiF->GetTimeStamp() + digiB->GetTimeStamp()) / 2.;
      Double_t hitCharge = (digiF->GetCharge() + digiB->GetCharge()) / 2.;
      if (fUseClusters) {
        fromStr = "GEMCluster";
        hitTimeStamp = (clusterF->GetTimeStamp() + clusterB->GetTimeStamp()) / 2.;
        hitCharge = (clusterF->GetCharge() + clusterB->GetCharge()) / 2.;
      }
      new ((*fHitsTemp)[nHitsTemp++]) PndGemHit(hitDetId, pos, dpos, hitCharge, hitTimeStamp, iDigiF, iDigiB, sigmaR, sigmaP, refIndex, fromStr);

      fTNofHitsTemp++;
      nHitsInSensor++;
    }
  }

  //  return 0;
  return nHitsInSensor;
}
// -------------------------------------------------------------------------

// -----   Public method Finish   ------------------------------------------
void PndGemFindHits::Finish()
{
  if (fHits)
    fHits->Clear();

  //  fChargeDiffHist->Draw("colz");

  cout << "-------------------- " << fName.Data() << " : Summary -----------------------" << endl;
  cout << " Events:        " << setw(10) << fTNofEvents << endl;
  cout << " Digis:         " << setw(10) << fTNofDigis << "    ( " << (Double_t)fTNofDigis / ((Double_t)fTNofEvents) << " per event )" << endl;
  cout << " HitsTemp:          " << setw(10) << fTNofHitsTemp << "    ( " << (Double_t)fTNofHitsTemp / ((Double_t)fTNofEvents) << " per event )" << endl;
  cout << " Hits:          " << setw(10) << fTNofHits << "    ( " << (Double_t)fTNofHits / ((Double_t)fTNofEvents) << " per event )" << endl;
  cout << "                       -->    ( " << (Double_t)fTNofHits / ((Double_t)fTNofEvents) / ((Double_t)fDigiPar->GetNSensors()) << " per sensor )" << endl;
  cout << "                       -->    ( " << (Double_t)fTNofHits / ((Double_t)fTNofDigis) << " per digi )" << endl;
  cout << "---------------------------------------------------------------------" << endl;

  cout << " >>> HF >>> prep    time = " << fPrepTime << "s      (get data from input)" << endl;
  cout << " >>> HF >>> sort    time = " << fSortTime - fPrepTime << "s    (sort clusters, " << fSortTime << ")" << endl;
  cout << " >>> HF >>> create  time = " << fCreateTime - fSortTime << "s    (create hits, " << fCreateTime << ")" << endl;
  cout << " >>> HF >>> confirm time = " << fConfirmTime - fCreateTime << "s    (confirm hits, " << fConfirmTime << ")" << endl;
  cout << " >>> HF >>> activ.  time = " << fActivateTime - fConfirmTime << "s      (activate digis, " << fActivateTime << ")" << endl;
  cout << " >>> HF >>> all     time = " << fAllTime - fActivateTime << "s       (all time spent in Exec, " << fAllTime << ")" << endl;
  cout << "---------------------------------------------------------------------" << endl;
}
// -------------------------------------------------------------------------

ClassImp(PndGemFindHits)
