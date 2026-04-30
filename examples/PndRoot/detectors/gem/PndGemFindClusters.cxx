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
// -----                    PndGemFindClusters source file                 -----
// -----                  Created 15/02/2009 by R. Karabowicz          -----
// -------------------------------------------------------------------------

/** PndGemFindClusters
 *@author Radoslaw Karabowicz <r.karabowicz@gsi.de>
 *@since 15.02.2009
 *@version 1.0
 **
 ** PANDA task class for finding PndGemClusters from PndGemDigis
 ** Task level RECO
 **/

#include "PndGemFindClusters.h"

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
PndGemFindClusters::PndGemFindClusters() : FairTask("GEMFindClusters", 1)
{
  fDigiPar = nullptr;
  fDigis = nullptr;
  fClusters = nullptr;

  fTNofEvents = 0;
  fTNofDigis = 0;
  fTNofClusters = 0;
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndGemFindClusters::PndGemFindClusters(Int_t iVerbose) : FairTask("GEMFindClusters", iVerbose)
{
  fDigiPar = nullptr;
  fDigis = nullptr;
  fClusters = nullptr;

  fTNofEvents = 0;
  fTNofDigis = 0;
  fTNofClusters = 0;
}
// -------------------------------------------------------------------------

// -----   Constructor with name   -----------------------------------------
PndGemFindClusters::PndGemFindClusters(const char *name, Int_t iVerbose) : FairTask(name, iVerbose)
{
  fDigiPar = nullptr;
  fDigis = nullptr;
  fClusters = nullptr;

  fTNofEvents = 0;
  fTNofDigis = 0;
  fTNofClusters = 0;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndGemFindClusters::~PndGemFindClusters()
{
  if (fClusters) {
    fClusters->Delete();
    delete fClusters;
  }
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndGemFindClusters::Exec(Option_t *)
{
  if (fVerbose > 0)
    LOG(info) << " PndGemFindClusters::Exec()";

  fTNofEvents++;

  fTimer.Start();
  // Bool_t warn = kFALSE; //[R.K. 01/2017] unused variable?

  // Clear output array
  fClusters->Clear();

  // Int_t nDigis    = 0; //[R.K.03/2017] unused variable
  Int_t nClusters = 0;

  // Sort digis with respect to time
  SortDigis(); // nDigis =  //[R.K.03/2017] unused variable

  // FindClusters();

  CreateClusters();

  if (fVerbose > 1) {
    Int_t nofC2W = 0;
    for (size_t idc = 0; idc < fDigiClusters.size(); idc++)
      if (fDigiClusters[idc].cluPos > -0.5)
        nofC2W++;
    cout << ">>>> created  " << fDigiClusters.size() << " clusters, " << nofC2W << " to write" << endl;
  }

  // Skip Analyze... the join is already done in the FindClusters()..
  // AnalyzeClusters();

  //   if ( fVerbose ) {
  //     Int_t nofC2W = 0;
  //     for ( Int_t idc = 0 ; idc < fDigiClusters.size() ; idc++ )
  //       if ( fDigiClusters[idc].cluPos > -0.5 )
  // 	nofC2W++;
  //     cout << ">>>> analyzed " << fDigiClusters.size() << " clusters, " << nofC2W << " to write" << endl;
  //   }

  // ClearClusters();
  // ClearClusters2();//sort and print only..
  SortClusters();

  if (fVerbose > 1) {
    PrintClusters();
  }

  //   if ( fVerbose ) {
  //     Int_t nofC2W = 0;
  //     for ( Int_t idc = 0 ; idc < fDigiClusters.size() ; idc++ )
  //       if ( fDigiClusters[idc].cluPos > -0.5 )
  // 	nofC2W++;
  //     cout << ">>>> cleared  " << fDigiClusters.size() << " clusters, " << nofC2W << " to write" << endl;
  //   }

  nClusters = WriteClusters();

  fTimer.Stop();

  //  cout << "CREATED " << nClusters << " CLUSTERS OUT OF " << nDigis << " DIGIS IN " << fTimer.RealTime() << "s" << endl;

  fTNofClusters += nClusters;
}
// -------------------------------------------------------------------------

// -----   Private method SetParContainers   -------------------------------
void PndGemFindClusters::SetParContainers()
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
InitStatus PndGemFindClusters::Init()
{

  // Get input array
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman)
    Fatal("Init", "No FairRootManager");
  fDigis = (TClonesArray *)ioman->GetObject("GEMDigi");

  // Register output array
  // fClusters = new TClonesArray("PndGemDigi", 1000);
  fClusters = new TClonesArray("PndGemCluster", 1000);
  ioman->Register("GEMCluster", "Cluster in GEM", fClusters, kTRUE);

  LOG(info) << " " << fName.Data() << "::Init(). There are " << fDigiPar->GetNStations() << " GEM stations.";
  LOG(info) << " " << fName.Data() << "::Init(). Initialization succesfull.";

  PndGemStation *station = (PndGemStation *)fDigiPar->GetStation(0);
  PndGemSensor *sensor = (PndGemSensor *)station->GetSensor(0);
  cout << "sensor out rad is " << sensor->GetOuterRadius() << endl;

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Private method ReInit   -----------------------------------------
InitStatus PndGemFindClusters::ReInit()
{

  // Create sectorwise digi sets
  //  MakeSets();

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Private method SortDigis   --------------------------------------
Int_t PndGemFindClusters::SortDigis()
{
  if (fVerbose)
    LOG(info) << "  PndGemFindClusters::SortDigis()";

  // Check input array
  if (!fDigis) {
    LOG(error) << " " << fName << "::SortDigis: No input array!";
    return -1;
  }

  // PndGemDigi* digi; //[R.K. 01/2017] unused variable?
  Int_t nDigis = fDigis->GetEntriesFast();

  fTimeOrderedDigis.clear();

  fTNofDigis += nDigis;

  for (Int_t iDigi = 0; iDigi < nDigis; iDigi++) {
    // digi = (PndGemDigi*) fDigis->At(iDigi); //[R.K. 01/2017] unused variable?
    //    Double_t time = digi->GetTimeStamp();
    Double_t time = gRandom->Gaus(0., 5.);

    fTimeOrderedDigis[time] = iDigi;
  }

  //  cout << "have " << nDigis << " digis ( " << fTimeOrderedDigis.size() << " ) " << endl;

  map<Double_t, Int_t>::iterator mapIt;
  Int_t nd = 0;
  for (mapIt = fTimeOrderedDigis.begin(); mapIt != fTimeOrderedDigis.end(); mapIt++) {
    //    cout << mapIt->first << " --> " << mapIt->second << endl;
    nd++;
  }
  // cout << " ---> " << nd << endl;

  return nDigis;
}
// -------------------------------------------------------------------------

// // -----  Private method FindClusters   --------------------------------------
// //NOT USED..
// void PndGemFindClusters::FindClusters() {
//   if ( fVerbose ) {
//     cout << "-I-  PndGemFindClusters::FindClusters()" <<endl;
//   }

//   PndGemDigi* digi;

//   map<Double_t, Int_t>::iterator mapIt;

//   fDigiClusters.clear();

//   for (mapIt=fTimeOrderedDigis.begin(); mapIt!=fTimeOrderedDigis.end(); mapIt++) {

//     if ( fVerbose ) {
//       cout << fTNofEvents << ". fighting for " << mapIt->second << endl;
//       for ( Int_t idc = 0 ; idc < fDigiClusters.size() ; idc++ ) {
// 	cout << "     cluster at " << fDigiClusters[idc].cluPos << ", h = " << fDigiClusters[idc].cluADC << ", for detId = " << fDigiClusters[idc].detId << " from " <<
// fDigiClusters[idc].digiNr.size() << " digis:" << endl;// (from " << lowChannel << " to " << topChannel << "):" << endl;
//       }
//     }

//     //Bool_t digiUsed = CompareDigiToClusters(mapIt->second);
//     Bool_t digiUsed = CompareDigiToClustersDigis(mapIt->second);

//     if ( digiUsed ) continue;

//     digi = (PndGemDigi*) fDigis->At(mapIt->second);

//     if ( fVerbose > 1 )
//       cout << "creating cluster " << fDigiClusters.size() << " from digi " << mapIt->second << " : " << digi->GetDetectorId() << " / " << digi->GetChannelNr() << endl;
//     DigiCluster tempDC;
//     tempDC.detId  = digi->GetDetectorId();
//     tempDC.digiNr.push_back(mapIt->second);
//     tempDC.chanNr.push_back((Int_t)digi->GetChannelNr());
//     tempDC.sigADC.push_back(digi->GetCharge());
//     tempDC.cluTDC = digi->GetTimeStamp();
//     tempDC.cluADC = digi->GetCharge();
//     tempDC.cluPos = digi->GetChannelNr();
//     tempDC.cluPMn = digi->GetChannelNr();
//     tempDC.cluPMx = digi->GetChannelNr();
//     tempDC.cluMPs = digi->GetChannelNr();
//     tempDC.cluMVl = digi->GetCharge();

//     fDigiClusters.push_back(tempDC);
//   }

// }
// // -------------------------------------------------------------------------

// -------------------------------------------------------------------------
Int_t PndGemFindClusters::CreateClusters()
{
  if (fVerbose > 0) {
    LOG(info) << "  PndGemFindClusters::CreateClusters()";
  }

  fDigiClusters.clear();

  PndGemDigi *digi = nullptr;
  for (Int_t idigi = 0; idigi < fDigis->GetEntries(); idigi++) {
    digi = (PndGemDigi *)fDigis->At(idigi);

    if (fVerbose) {
      //    cout << "digi " << idigi << " in " << digi->GetDetectorId() << ", there are already " << fDigiClusters.size() << " clusters" << endl;
      //    cout << idigi << " ---> " << digi->GetDetectorId() << " @ " << digi->GetChannelNr() << " @ " << digi->GetTimeStamp() << endl;
    }
    if (CompareDigiToClustersDigis(idigi) == kTRUE)
      continue;

    DigiCluster tempDC;
    tempDC.detId = digi->GetDetectorId();
    tempDC.digiNr.push_back(idigi);
    tempDC.chanNr.push_back((Int_t)digi->GetChannelNr());
    tempDC.sigADC.push_back(digi->GetCharge());
    tempDC.cluTDC = digi->GetTimeStamp();
    tempDC.cluADC = digi->GetCharge();
    tempDC.cluPos = digi->GetChannelNr();
    tempDC.cluPMn = digi->GetChannelNr();
    tempDC.cluPMx = digi->GetChannelNr();
    tempDC.cluMPs = digi->GetChannelNr();
    tempDC.cluMVl = digi->GetCharge();
    fDigiClusters.push_back(tempDC);
  }
  return fDigiClusters.size();
}
// -------------------------------------------------------------------------

// // -----  Private method AnalyzeClusters   --------------------------------------
// //CURRENTLY NOT USED...
// void PndGemFindClusters::AnalyzeClusters() {
//   if ( fVerbose > 0 ) {
//     cout << "STARTING CLUSTER ANALYSIS WITH " << fDigiClusters.size() << " CLUSTERS" << endl;
//     for ( Int_t idc = 0 ; idc < fDigiClusters.size() ; idc++ ) {
//       cout << idc << ".th cluster at " << fDigiClusters[idc].cluPos << ", h = " << fDigiClusters[idc].cluADC << ", for detId = " << fDigiClusters[idc].detId << " from " <<
//       fDigiClusters[idc].digiNr.size() << " digis:" << endl;// (from " << lowChannel << " to " << topChannel << "):" << endl;
//     }
//   }

//   Int_t nClusters = fDigiClusters.size();

//   PndGemSensor* sensor;
//   Int_t         side;
//   PndGemDigi*   digi;
//   Double_t      dist;

//   std::vector<Bool_t> digiVect(20000,kFALSE);
//   std::vector<Bool_t> commVect(20000,kFALSE);

//   for ( Int_t idc1 = 0 ; idc1 < nClusters ; idc1++ ) {

//     digiVect.assign(20000,kFALSE);

//     Bool_t jointDigi = kFALSE;

//     for ( Int_t idc2 = idc1+1 ; idc2 < nClusters ; idc2++ ) {
//       if ( fDigiClusters[idc1].detId != fDigiClusters[idc2].detId ) continue;
//       digi   = (PndGemDigi*)fDigis->At(fDigiClusters[idc1].digiNr[0]);
//       sensor = fDigiPar->GetSensor(digi->GetStationNr(),digi->GetSensorNr());
//       side   = digi->GetSide();

//       dist   = sensor->GetDistance(side,fDigiClusters[idc1].cluPos,fDigiClusters[idc2].cluPos);

//       if ( dist <  0 ) continue;
//       if ( dist > 30 ) continue;

//       //      cout << "WILL JOIN cluster " << idc1 << " and " << idc2 << endl;

//       commVect.assign(20000,kFALSE);
//       for ( Int_t id = 0 ; id < fDigiClusters[idc2].digiNr.size() ; id++ ) {
// 	//	cout << "cluster2 id = " << id << " and in fact " << fDigiClusters[idc2].digiNr[id] << endl;
// 	commVect[fDigiClusters[idc2].digiNr[id]] = kTRUE;
//       }

//       if ( fVerbose > 1 )
// 	cout << "CLUSTER " << idc2 << " (" << fDigiClusters[idc2].digiNr.size() << " digis at " << fDigiClusters[idc2].cluPos << ") joined with cluster " << idc1 << " ---> " << flush;
//       for ( Int_t id = 0 ; id < fDigiClusters[idc1].digiNr.size() ; id++ ) {
// 	if ( commVect[fDigiClusters[idc1].digiNr[id]] == kFALSE ) {
// 	  //	  cout << "adding digi id " << id << " and in fact " << fDigiClusters[idc1].digiNr[id] << endl;
// 	  digi   = (PndGemDigi*)fDigis->At(fDigiClusters[idc1].digiNr[id]);
// 	  fDigiClusters[idc2].digiNr.push_back(fDigiClusters[idc1].digiNr[id]);
// 	  fDigiClusters[idc2].chanNr.push_back((Int_t)digi->GetChannelNr());
// 	  fDigiClusters[idc2].sigADC.push_back(digi->GetCharge());
// 	  fDigiClusters[idc2].cluTDC = digi->GetTimeStamp();
// 	  fDigiClusters[idc2].cluPos = sensor->GetMeanChannel(side,fDigiClusters[idc2].cluPos,fDigiClusters[idc2].cluADC,digi->GetChannelNr(),digi->GetCharge());
// 	  if ( digi->GetCharge() > fDigiClusters[idc2].cluMVl ) {
// 	    fDigiClusters[idc2].cluMVl = digi->GetCharge();
// 	    fDigiClusters[idc2].cluMPs = digi->GetChannelNr();
// 	  }

// 	  fDigiClusters[idc2].cluADC =  fDigiClusters[idc2].cluADC+digi->GetCharge();
// 	}
//       }
//       if ( fVerbose > 1 )
// 	cout << "(" << fDigiClusters[idc2].digiNr.size() << " digis at " << fDigiClusters[idc2].cluPos << ")." << endl;

//       jointDigi = kTRUE;
//       if ( fVerbose > 1 )
// 	cout << "comparing cluster at " << fDigiClusters[idc1].cluPos << " with " << fDigiClusters[idc2].cluPos << " gives dist of " << dist << endl;
//     }

//     if ( jointDigi == kTRUE )
//       fDigiClusters[idc1].cluPos = -1.;
//   }

//   if ( fVerbose ) {
//     cout << "FINISHING CLUSTER ANALYSIS WITH " << fDigiClusters.size() << " CLUSTERS" << endl;
//     for ( Int_t idc = 0 ; idc < fDigiClusters.size() ; idc++ ) {
//       cout << idc << ".th cluster at " << fDigiClusters[idc].cluPos << ", h = " << fDigiClusters[idc].cluADC << ", for detId = " << fDigiClusters[idc].detId << " from " <<
//       fDigiClusters[idc].digiNr.size() << " digis:" << endl;// (from " << lowChannel << " to " << topChannel << "):" << endl;
//     }

//     for ( Int_t idc = 0 ; idc < fDigiClusters.size() ; idc++ ) {
//       // if ( fDigiClusters[idc].cluADC < 105 ) continue;
//       cout << "cluster at " << fDigiClusters[idc].cluPos << ", h = " << fDigiClusters[idc].cluADC << ", for detId = " << fDigiClusters[idc].detId << " from " <<
//       fDigiClusters[idc].digiNr.size() << " digis:" << endl;// (from " << lowChannel << " to " << topChannel << "):" << endl; for ( Int_t idigi = 0 ; idigi <
//       fDigiClusters[idc].digiNr.size() ; idigi++ ) {
// 	cout << idigi << " > "
// 	     << fDigiClusters[idc].digiNr[idigi] << " @ "
// 	     << fDigiClusters[idc].chanNr[idigi] << " h "
// 	     << fDigiClusters[idc].sigADC[idigi] << endl;
//       }
//     }
//   }

// }
// // -------------------------------------------------------------------------

// // -----  Private method ClearClusters   --------------------------------------
// //CURRENTLY NOT USED..
// void PndGemFindClusters::ClearClusters() {
//   if ( fVerbose )
//     cout << "CLEAR CLUSTERS STARTING FROM " << fDigiClusters.size() << endl;

//   Int_t intEvent = -1;

//   PndGemDigi*   digi;
//   PndGemSensor* sensor;

//   Int_t nofClustersAtBegin = fDigiClusters.size();
//   for ( Int_t idc = 0 ; idc < nofClustersAtBegin ; idc++ ) {
//     if ( fDigiClusters[idc].cluPos < -0.5 ) continue;

//     if ( fVerbose || fTNofEvents == intEvent )
//       cout << "-------------------------------------------------" << endl;

//     // GET THE LOWEST AND HIGHEST CHANNEL NUMBER
//     Int_t lowChannel = 1000000;
//     Int_t topChannel = -1;
//     for ( Int_t idigi = 0 ; idigi < fDigiClusters[idc].digiNr.size() ; idigi++ ) {
//       if ( fVerbose ) {
// 	cout << idigi << " > "
// 	     << fDigiClusters[idc].digiNr[idigi] << " @ "
// 	     << fDigiClusters[idc].chanNr[idigi] << " h "
// 	     << fDigiClusters[idc].sigADC[idigi] << endl;
//       }
//       if ( fDigiClusters[idc].chanNr[idigi] < lowChannel ) lowChannel = fDigiClusters[idc].chanNr[idigi];
//       if ( fDigiClusters[idc].chanNr[idigi] > topChannel ) topChannel = fDigiClusters[idc].chanNr[idigi];
//     }
//     // ---------------------------------------------------------

//     // MAKE ORDER IN THE CLUSTER
//     Int_t channelPosition = 0;
//     for ( Int_t ich = lowChannel ; ich <= topChannel ; ich++ ) {
//       for ( Int_t idigi = channelPosition ; idigi < fDigiClusters[idc].digiNr.size() ; idigi++ ) {
// 	if ( fDigiClusters[idc].chanNr[idigi] == ich ) {
// 	  Int_t    dN = fDigiClusters[idc].digiNr[idigi];
// 	  Int_t    cN = fDigiClusters[idc].chanNr[idigi];
// 	  Double_t sA = fDigiClusters[idc].sigADC[idigi];
// 	  fDigiClusters[idc].digiNr[idigi] = fDigiClusters[idc].digiNr[channelPosition];
// 	  fDigiClusters[idc].chanNr[idigi] = fDigiClusters[idc].chanNr[channelPosition];
// 	  fDigiClusters[idc].sigADC[idigi] = fDigiClusters[idc].sigADC[channelPosition];
// 	  fDigiClusters[idc].digiNr[channelPosition] = dN;
// 	  fDigiClusters[idc].chanNr[channelPosition] = cN;
// 	  fDigiClusters[idc].sigADC[channelPosition] = sA;
// 	  channelPosition++;
// 	}
//       }
//     }
//     // ---------------------------------------------------------

//     // PRINT
//     if ( fVerbose || fTNofEvents == intEvent ) {
//       cout << fTNofEvents << " cluster at " << fDigiClusters[idc].cluPos << ", h = " << fDigiClusters[idc].cluADC << ", for detId = " << fDigiClusters[idc].detId << " from " <<
//       fDigiClusters[idc].digiNr.size() << " digis (from " << lowChannel << " to " << topChannel << "):" << endl; for ( Int_t idigi = 0 ; idigi < fDigiClusters[idc].digiNr.size()
//       ; idigi++ ) {
// 	cout << idigi << " > "
// 	     << fDigiClusters[idc].digiNr[idigi] << " @ "
// 	     << fDigiClusters[idc].chanNr[idigi] << " h "
// 	     << fDigiClusters[idc].sigADC[idigi] << endl;
//       }
//     }
//     // ---------------------------------------------------------

//     // FIND PEAKS AND HOLES BETWEEN TEHM
//     Bool_t showDets = kFALSE;
//     if ( fVerbose ) {
//       showDets = kTRUE;
//       cout << fTNofEvents << " cluster at " << fDigiClusters[idc].cluPos << ", h = " << fDigiClusters[idc].cluADC << ", for detId = " << fDigiClusters[idc].detId << " from " <<
//       fDigiClusters[idc].digiNr.size() << " digis (from " << lowChannel << " to " << topChannel << "):" << endl;
//       /*for ( Int_t idigi = 0 ; idigi < fDigiClusters[idc].digiNr.size() ; idigi++ ) {
// 	cout << idigi << " > "
// 	     << fDigiClusters[idc].digiNr[idigi] << " @ "
// 	     << fDigiClusters[idc].chanNr[idigi] << " h "
// 	     << fDigiClusters[idc].sigADC[idigi] << endl;
// 	     }*/
//     }
//     Int_t    lastPos = fDigiClusters[idc].chanNr[0];
//     Double_t lastADC = fDigiClusters[idc].sigADC[0];
//     Double_t maxADC  = fDigiClusters[idc].sigADC[0];
//     Double_t gotPeak = 40.;
//     Double_t newPeak = 0;
//     Double_t minADC = 1000.;
//     Int_t    minPos = -1;
//     Int_t nofPeaks = 0;
//     std::vector<Int_t> clusterBorders;
//     if ( showDets )      cout << "0 > "
// 			      << fDigiClusters[idc].digiNr[0] << " @ "
// 			      << fDigiClusters[idc].chanNr[0] << " h "
// 			      << fDigiClusters[idc].sigADC[0] << endl;
//     //    for ( Int_t idigi = 1 ; idigi < fDigiClusters[idc].digiNr.size() ; idigi++ ) {
//     Int_t    thisDigi = 1;
//     Double_t thisADC  = 0.;
//     for ( Int_t ichan = lowChannel+1 ; ichan <= topChannel ; ichan++ ) {
//       thisADC = 0.;
//       if ( fDigiClusters[idc].chanNr[thisDigi] == ichan ) {
// 	thisADC = fDigiClusters[idc].sigADC[thisDigi];
// 	thisDigi++;
//       }
// //       if ( fDigiClusters[idc].chanNr[idigi] > lastPos+2 )
// // 	cout << "will probably make a break here between " << idigi-1 << " and " << idigi << endl;
// //      if ( gotPeak < 0. &&
//       if ( thisADC < minADC ) { // this is min
// 	minADC = thisADC;
// 	minPos = ichan;
// 	//	cout << "minadc " << minADC << " at " << minPos << endl;
//       }
//       if ( thisADC < maxADC*.5 && gotPeak < 0. ) { // this is max
// 	gotPeak = 40.;
// 	if ( showDets ) 		cout << "is less than half of last max" << endl;
//       }
//       if ( gotPeak > 0. ) { // got peak already, adc should be going down
// 	if ( thisADC >= lastADC ) { // but adc not falling down
// 	  if ( thisADC >  lastADC ) newPeak+=1.0; // possibly a new peak
// 	  if ( thisADC == lastADC ) {
// 	    if ( thisADC != 0. ) newPeak+=0.5;
// 	    if ( thisADC == 0. ) newPeak+=1.0;
// 	  }
// 	  if ( newPeak >= 2 ) { // it is a new peak
// 	    nofPeaks++;
// 	    if ( showDets || fTNofEvents == intEvent )
// 	      cout << "NEW PEAK starts from " << minPos << " ( " << minADC << " )." << endl;
// 	    clusterBorders.push_back(minPos);
// 	    minADC = 1000.;
// 	    minPos = -1;
// 	    gotPeak = -1.;
// 	    maxADC = 0.;
// 	  }
// 	}
// 	else if ( thisADC < lastADC ) {
// 	  newPeak = 0;
// 	}
//       }
//       lastPos = ichan;
//       lastADC = thisADC;
//       if ( showDets )
// 	cout << thisDigi << " > " << gotPeak << " "
// 	     << fDigiClusters[idc].digiNr[thisDigi] << " @ "
// 	     << fDigiClusters[idc].chanNr[thisDigi] << " h "
// 	     << fDigiClusters[idc].sigADC[thisDigi] << endl;
//       if ( lastADC > maxADC ) {
// 	maxADC = lastADC;
// 	if ( newPeak == 0 ) {
// 	  minPos = -1;
// 	  minADC = 1000.;
// 	}
//       }
//     }

//     if ( showDets ) {
//       cout << "NOF PEAKS = " << nofPeaks << "  : " << flush;
//       for ( Int_t ipk = 0 ; ipk < nofPeaks ; ipk++ )
// 	cout << "New" << flush;
//       cout << endl;
//       for ( Int_t ipk = 0 ; ipk < nofPeaks ; ipk++ )
// 	cout << "---> " << clusterBorders[ipk] << endl;
//     }
//     // ---------------------------------------------------------

//     // ----> still do not consider strips with 0 charge...

//     if ( nofPeaks < 2 ) continue; // do not have to do anything...
//     // DIVIDE CLUSTER IF nofPeaks > 1
//     digi = (PndGemDigi*) fDigis->At(fDigiClusters[idc].digiNr[0]);

//     Int_t stationNr = digi->GetStationNr();
//     Int_t sensorNr  = digi->GetSensorNr();
//     Int_t iSide     = digi->GetSide();

//     sensor = fDigiPar->GetSensor(stationNr, sensorNr);

//     thisDigi = 0;
//     Int_t ichan = lowChannel;

//     if ( fTNofEvents == intEvent ) {
//       cout << fTNofEvents << " - - - - CLUSTER " << idc << " FROM " << lowChannel << " TO " << topChannel << " SHOULD BE DIVIDED IN " << nofPeaks << " PEAKS" << endl;
//       for ( Int_t idigi = 0 ; idigi < fDigiClusters[idc].digiNr.size() ; idigi++ ) {
// 	cout << " > > > > > > > " << idigi << " > "
// 	     << fDigiClusters[idc].digiNr[idigi] << " @ "
// 	     << fDigiClusters[idc].chanNr[idigi] << " h "
// 	     << fDigiClusters[idc].sigADC[idigi] << endl;
//       }
//     }

//     for ( Int_t inp = 0 ; inp < nofPeaks ; inp++ ) {
//       if ( fTNofEvents == intEvent ) {
// 	cout << "*** p e a k " << inp << " to " << clusterBorders[inp+1] << endl;
//       }

//       DigiCluster tempDC;
//       tempDC.detId  = fDigiClusters[idc].detId;
//       tempDC.cluTDC = fDigiClusters[idc].cluTDC;
//       tempDC.cluADC = 0.;
//       tempDC.cluPos = 0.;
//       tempDC.cluMPs = 0.;
//       tempDC.cluMVl = 0.;

//       /*      for ( ; idigi < fDigiClusters[idc].digiNr.size() ; idigi++ ) {
// 	if ( fDigiClusters[idc].chanNr[idigi] > clusterBorders[inp+1] ) break;
// 	tempDC.digiNr.push_back(fDigiClusters[idc].digiNr[idigi]);
// 	tempDC.chanNr.push_back(fDigiClusters[idc].chanNr[idigi]);
// 	tempDC.sigADC.push_back(fDigiClusters[idc].sigADC[idigi]);
// 	if ( fDigiClusters[idc].sigADC[idigi] > tempDC.cluMVl ) {
// 	  tempDC.cluMVl = fDigiClusters[idc].sigADC[idigi];
// 	  tempDC.cluMPs = fDigiClusters[idc].chanNr[idigi];
// 	}
// 	if ( tempDC.digiNr.size() == 1 ) {
// 	  tempDC.cluPos = fDigiClusters[idc].chanNr[idigi];
// 	  tempDC.cluADC = fDigiClusters[idc].sigADC[idigi];
// 	}
// 	else {
// 	  tempDC.cluPos = sensor->GetMeanChannel(iSide,tempDC.cluPos,tempDC.cluADC,fDigiClusters[idc].chanNr[idigi],fDigiClusters[idc].sigADC[idigi]);
// 	  tempDC.cluADC = tempDC.cluADC+fDigiClusters[idc].sigADC[idigi];
// 	}
// 	}*/
//       for ( ; ichan <= topChannel ; ichan++ ) {
// 	if ( fTNofEvents == intEvent ) {
// 	  cout << " checking channel " << ichan << endl;
// 	}

// 	if ( inp < nofPeaks-1 && ichan > clusterBorders[inp+1] ) break;

// 	if ( fDigiClusters[idc].chanNr[thisDigi] != ichan )
// 	  continue;

// 	//	if ( fDigiClusters[idc].digiNr[thisDigi] > 3000 )
// 	if ( fTNofEvents == intEvent ) {
// 	  cout << fTNofEvents << " > putting digiNr = " << fDigiClusters[idc].digiNr[thisDigi]
// 	       << " (" << fDigiClusters[idc].chanNr[thisDigi] << ") "
// 	       << " from " << thisDigi << " coming from split of cluster " << idc << endl;
// 	}

// 	tempDC.digiNr.push_back(fDigiClusters[idc].digiNr[thisDigi]);
// 	tempDC.chanNr.push_back(fDigiClusters[idc].chanNr[thisDigi]);
// 	tempDC.sigADC.push_back(fDigiClusters[idc].sigADC[thisDigi]);
// 	if ( fDigiClusters[idc].sigADC[thisDigi] > tempDC.cluMVl ) {
// 	  tempDC.cluMVl = fDigiClusters[idc].sigADC[thisDigi];
// 	  tempDC.cluMPs = fDigiClusters[idc].chanNr[thisDigi];
// 	}
// 	if ( tempDC.digiNr.size() == 1 ) {
// 	  tempDC.cluPos = fDigiClusters[idc].chanNr[thisDigi];
// 	  tempDC.cluADC = fDigiClusters[idc].sigADC[thisDigi];
// 	}
// 	else {
// 	  tempDC.cluPos = sensor->GetMeanChannel(iSide,tempDC.cluPos,tempDC.cluADC,fDigiClusters[idc].chanNr[thisDigi],fDigiClusters[idc].sigADC[thisDigi]);
// 	  tempDC.cluADC = tempDC.cluADC+fDigiClusters[idc].sigADC[thisDigi];
// 	}
// 	thisDigi++;
//       }
//       if ( showDets )
// 	cout << "just to confirm, added cluster with ADC = " << tempDC.cluADC << " at " << tempDC.cluPos
// 	     << ", with MAX = " << tempDC.cluMVl << " at " << tempDC.cluMPs
// 	     << " with " << tempDC.digiNr.size() << " digis" << endl;

//       fDigiClusters.push_back(tempDC);
//     }
//     fDigiClusters[idc].cluPos = -1.;

//     // ---------------------------------------------------------
//   }
// }
// // -------------------------------------------------------------------------

// // -----  Private method   --------------------------------------
// // It does only sorting and print
// // no double peak analysis
// void PndGemFindClusters::ClearClusters2() {
//   if ( fVerbose )
//     cout << "CLEAR CLUSTERS STARTING FROM " << fDigiClusters.size() << endl;

//   Int_t intEvent = -1;

//   PndGemDigi*   digi;
//   PndGemSensor* sensor;

//   Int_t nofClustersAtBegin = fDigiClusters.size();
//   for ( Int_t idc = 0 ; idc < nofClustersAtBegin ; idc++ ) {
//     if ( fDigiClusters[idc].cluPos < -0.5 ) continue;

//     if ( fVerbose || fTNofEvents == intEvent )
//       cout << "-------------------------------------------------" << endl;

//     // GET THE LOWEST AND HIGHEST CHANNEL NUMBER
//     Int_t lowChannel = 1000000;
//     Int_t topChannel = -1;
//     for ( Int_t idigi = 0 ; idigi < fDigiClusters[idc].digiNr.size() ; idigi++ ) {
//       if ( fVerbose ) {
// 	cout << idigi << " > "
// 	     << fDigiClusters[idc].digiNr[idigi] << " @ "
// 	     << fDigiClusters[idc].chanNr[idigi] << " h "
// 	     << fDigiClusters[idc].sigADC[idigi] << endl;
//       }
//       if ( fDigiClusters[idc].chanNr[idigi] < lowChannel ) lowChannel = fDigiClusters[idc].chanNr[idigi];
//       if ( fDigiClusters[idc].chanNr[idigi] > topChannel ) topChannel = fDigiClusters[idc].chanNr[idigi];
//     }
//     // ---------------------------------------------------------

//     // MAKE ORDER IN THE CLUSTER
//     Int_t channelPosition = 0;
//     for ( Int_t ich = lowChannel ; ich <= topChannel ; ich++ ) {
//       for ( Int_t idigi = channelPosition ; idigi < fDigiClusters[idc].digiNr.size() ; idigi++ ) {
// 	if ( fDigiClusters[idc].chanNr[idigi] == ich ) {
// 	  Int_t    dN = fDigiClusters[idc].digiNr[idigi];
// 	  Int_t    cN = fDigiClusters[idc].chanNr[idigi];
// 	  Double_t sA = fDigiClusters[idc].sigADC[idigi];
// 	  fDigiClusters[idc].digiNr[idigi] = fDigiClusters[idc].digiNr[channelPosition];
// 	  fDigiClusters[idc].chanNr[idigi] = fDigiClusters[idc].chanNr[channelPosition];
// 	  fDigiClusters[idc].sigADC[idigi] = fDigiClusters[idc].sigADC[channelPosition];
// 	  fDigiClusters[idc].digiNr[channelPosition] = dN;
// 	  fDigiClusters[idc].chanNr[channelPosition] = cN;
// 	  fDigiClusters[idc].sigADC[channelPosition] = sA;
// 	  channelPosition++;
// 	}
//       }
//     }
//     // ---------------------------------------------------------

//     // PRINT
//     if ( fVerbose || fTNofEvents == intEvent ) {
//       cout << fTNofEvents << " cluster at " << fDigiClusters[idc].cluPos << ", h = " << fDigiClusters[idc].cluADC << ", for detId = " << fDigiClusters[idc].detId << " from " <<
//       fDigiClusters[idc].digiNr.size() << " digis (from " << lowChannel << " to " << topChannel << "):" << endl; for ( Int_t idigi = 0 ; idigi < fDigiClusters[idc].digiNr.size()
//       ; idigi++ ) {
// 	cout << idigi << " > "
// 	     << fDigiClusters[idc].digiNr[idigi] << " @ "
// 	     << fDigiClusters[idc].chanNr[idigi] << " h "
// 	     << fDigiClusters[idc].sigADC[idigi] << endl;
//       }
//     }

//   }

// }
// // -------------------------------------------------------------------------

// -----  Private method WriteClusters   --------------------------------------
Int_t PndGemFindClusters::WriteClusters()
{
  if (fVerbose > 0)
    LOG(info) << " PndGemFindClusters::WriteClusters()";

  Int_t nClusters = 0;
  // PndGemDigi* digi; //[R.K. 01/2017] unused variable?

  for (size_t idc = 0; idc < fDigiClusters.size(); idc++) {

    //    if ( fDigiClusters[idc].cluADC < 1. ) continue;
    if (fDigiClusters[idc].cluPos < -0.5)
      continue;

    if (fVerbose > 1) {
      cout << "cluster at " << fDigiClusters[idc].cluPos << ", h = " << fDigiClusters[idc].cluADC << ", for detId = " << fDigiClusters[idc].detId << " from "
           << fDigiClusters[idc].digiNr.size() << " digis (from " << fDigiClusters[idc].cluPMn << " to " << fDigiClusters[idc].cluPMx << "): " << endl;

      for (size_t idigi = 0; idigi < fDigiClusters[idc].digiNr.size(); idigi++) {
        cout << " digiNr=" << fDigiClusters[idc].digiNr[idigi] << " chanNr=" << fDigiClusters[idc].chanNr[idigi] << " sigADC=" << fDigiClusters[idc].sigADC[idigi] << endl;
      }
    }

    new ((*fClusters)[nClusters]) PndGemCluster(fDigiClusters[idc].detId, fDigiClusters[idc].cluPos, fDigiClusters[idc].cluPMn, fDigiClusters[idc].cluPMx,
                                                fDigiClusters[idc].cluADC, fDigiClusters[idc].cluTDC, fDigiClusters[idc].digiNr);

    if (fVerbose > 1)
      cout << "creating cluster at " << fDigiClusters[idc].cluPos << " with height of " << fDigiClusters[idc].cluADC << " /// compare maximum at " << fDigiClusters[idc].cluMPs
           << endl;
    nClusters++;
  }
  return nClusters;
}
// -------------------------------------------------------------------------

// // -----   Private method  ------------------------------------------
// //NOT USED.. keep old structure..
// Bool_t PndGemFindClusters::CompareDigiToClusters(Int_t digiNumber) {
//   PndGemSensor* sensor;
//   PndGemDigi* digi;

//   Bool_t digiUsed = kFALSE;
//   for ( Int_t idc = 0 ; idc < fDigiClusters.size() ; idc++ ) {
//     digi = (PndGemDigi*) fDigis->At(digiNumber);

//     if ( digi->GetDetectorId() != fDigiClusters[idc].detId ) continue;

//     Int_t stationNr = digi->GetStationNr();
//     Int_t sensorNr  = digi->GetSensorNr();
//     Int_t iSide     = digi->GetSide();

//     sensor = fDigiPar->GetSensor(stationNr, sensorNr);

//     Double_t digiClustDist = sensor->GetDistance(iSide,digi->GetChannelNr(),fDigiClusters[idc].cluPos);
//     if ( digiClustDist < 0. ) continue; // there's no connection
//     if ( digiClustDist > 50. ) continue; // they are too far

//     if ( fVerbose > 1 )
//       cout << "digi " << digiNumber << " ( " << digi->GetDetectorId() << " / " << digi->GetChannelNr() << " ) could attach to cluster "
// 	   << idc << " ( " << fDigiClusters[idc].cluPos << " ) with distance " << digiClustDist << " . " << fDigiClusters[idc].digiNr.size() << " digis in cluster" << flush;

//     fDigiClusters[idc].digiNr.push_back(digiNumber);
//     fDigiClusters[idc].chanNr.push_back((Int_t)digi->GetChannelNr());
//     fDigiClusters[idc].sigADC.push_back(digi->GetCharge());
//     fDigiClusters[idc].cluTDC = digi->GetTimeStamp();
//     fDigiClusters[idc].cluPos = sensor->GetMeanChannel(iSide,fDigiClusters[idc].cluPos,fDigiClusters[idc].cluADC,digi->GetChannelNr(),digi->GetCharge());
//     fDigiClusters[idc].cluADC =  fDigiClusters[idc].cluADC+digi->GetCharge();
//     if ( digi->GetCharge() > fDigiClusters[idc].cluMVl ) {
//       fDigiClusters[idc].cluMVl = digi->GetCharge();
//       fDigiClusters[idc].cluMPs = digi->GetChannelNr();
//     }

//     if ( fVerbose > 1 )
//       cout << " >>> new pos = " << fDigiClusters[idc].cluPos << endl;

//     digiUsed = kTRUE;
//   }
//   return digiUsed;

// }
// // -------------------------------------------------------------------------

// -----   Private method   ------------------------------------------
// compares the sample digi and all digis in clusters
Bool_t PndGemFindClusters::CompareDigiToClustersDigis(Int_t digiNumber)
{
  if (fVerbose > 0)
    cout << "PndGemFindClusters::CompareDigiToClustersDigis" << endl;

  PndGemSensor *sensor;
  PndGemDigi *digi;
  digi = (PndGemDigi *)fDigis->At(digiNumber);

  Int_t iDigiUsed = -1;
  for (size_t idc = 0; idc < fDigiClusters.size(); idc++) {
    if (fDigiClusters[idc].cluPos < -0.5)
      continue;
    if (digi->GetDetectorId() != fDigiClusters[idc].detId)
      continue;

    Int_t stationNr = digi->GetStationNr();
    Int_t sensorNr = digi->GetSensorNr();
    Int_t iSide = digi->GetSide();

    sensor = fDigiPar->GetSensor(stationNr, sensorNr);

    Double_t digiDigiDist = 999999999, testDist;
    for (size_t id = 0; id < fDigiClusters[idc].digiNr.size(); id++) {
      testDist = sensor->GetDistance2(iSide, digi->GetChannelNr(), fDigiClusters[idc].chanNr[id]);
      if (testDist < digiDigiDist)
        digiDigiDist = testDist;
    }

    if (digiDigiDist < 0.)
      continue;
    if (digiDigiDist > 1)
      continue; // 1 for next strip to accept

    if (fVerbose > 1)
      cout << "digi " << digiNumber << " ( " << digi->GetDetectorId() << " / " << digi->GetChannelNr() << " ) could attach to cluster " << idc << " ( " << fDigiClusters[idc].cluPos
           << " ) with distance " << digiDigiDist << " . " << fDigiClusters[idc].digiNr.size() << " digis in cluster" << flush;

    AddDigiToCluster(digiNumber, idc);

    if (fVerbose > 1)
      cout << " >>> mean channel = " << fDigiClusters[idc].cluPos << endl;

    if (iDigiUsed >= 0) {
      // cout << "DIGI " << digiNumber << " MATCHES TO CLUSTER " << iDigiUsed << " and " << idc << endl;
      JoinTwoClusters(idc, iDigiUsed);
    }

    iDigiUsed = idc;
  }
  return (iDigiUsed == -1 ? kFALSE : kTRUE);
}
// -------------------------------------------------------------------------

// -----   Private method   ------------------------------------------
void PndGemFindClusters::JoinTwoClusters(Int_t clus1, Int_t clus2)
{
  if (fVerbose > 0)
    cout << "PndGemFindClusters::JointTwoClusters()" << endl;

  // PndGemDigi* digi; //[R.K. 01/2017] unused variable?

  for (size_t id2 = 0; id2 < fDigiClusters[clus2].digiNr.size(); id2++) {
    Int_t digi2 = fDigiClusters[clus2].digiNr[id2];
    Int_t chan2 = fDigiClusters[clus2].chanNr[id2];

    Int_t addDigi = kTRUE;
    for (size_t id1 = 0; id1 < fDigiClusters[clus1].digiNr.size(); id1++) {
      if (chan2 == fDigiClusters[clus1].chanNr[id1])
        addDigi = kFALSE;
    }

    if (addDigi)
      AddDigiToCluster(digi2, clus1);
  }
  fDigiClusters[clus2].cluPos = -1.;
}
// -------------------------------------------------------------------------

// -----  Private method AddDigiToCluster   ---------------------------------
void PndGemFindClusters::AddDigiToCluster(Int_t digiNr, Int_t clusNr)
{
  if (fVerbose > 0)
    cout << "PndGemFindClusters::AddDigiToCluster()" << endl;

  PndGemDigi *digi = (PndGemDigi *)fDigis->At(digiNr);
  Int_t stationNr = digi->GetStationNr();
  Int_t sensorNr = digi->GetSensorNr();
  Int_t iSide = digi->GetSide();

  PndGemSensor *sensor = fDigiPar->GetSensor(stationNr, sensorNr);

  if (fDigiClusters[clusNr].digiNr.size() == 0) {
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
}
// -------------------------------------------------------------------------

// -----   Private method SortClusters   -------------------------------
void PndGemFindClusters::SortClusters()
{
  if (fVerbose > 0)
    cout << "PndGemFindClusters::SortClusters()" << endl;
  for (size_t idc = 0; idc < fDigiClusters.size(); idc++) {
    SortCluster(idc);
  }
}
// -------------------------------------------------------------------------

// -----   Private method SortClusters   -------------------------------
void PndGemFindClusters::SortCluster(Int_t clus)
{
  if (fVerbose > 0)
    cout << "PndGemFindClusters::SortCluster()" << endl;
  Int_t channelPosition = 0;

  for (Int_t ich = fDigiClusters[clus].cluPMn; ich <= fDigiClusters[clus].cluPMx; ich++) {
    for (size_t idigi = channelPosition; idigi < fDigiClusters[clus].digiNr.size(); idigi++) {
      if (fDigiClusters[clus].chanNr[idigi] == ich) {
        Int_t dN = fDigiClusters[clus].digiNr[idigi];
        Int_t cN = fDigiClusters[clus].chanNr[idigi];
        Double_t sA = fDigiClusters[clus].sigADC[idigi];
        fDigiClusters[clus].digiNr[idigi] = fDigiClusters[clus].digiNr[channelPosition];
        fDigiClusters[clus].chanNr[idigi] = fDigiClusters[clus].chanNr[channelPosition];
        fDigiClusters[clus].sigADC[idigi] = fDigiClusters[clus].sigADC[channelPosition];
        fDigiClusters[clus].digiNr[channelPosition] = dN;
        fDigiClusters[clus].chanNr[channelPosition] = cN;
        fDigiClusters[clus].sigADC[channelPosition] = sA;
        channelPosition++;
      }
    }
  }
}
// -------------------------------------------------------------------------

// -----   Private method PrintClusters   -------------------------------
void PndGemFindClusters::PrintClusters()
{
  if (fVerbose > 0)
    cout << "PndGemFindClusters::PrintClusters()" << endl;

  for (size_t idc = 0; idc < fDigiClusters.size(); idc++) {
    //    if ( fDigiClusters[idc].detId != 568329024 ) continue;
    if (fDigiClusters[idc].cluPos < -0.5)
      continue;
    PrintCluster(idc);
  }
}
// -------------------------------------------------------------------------

// -----   Private method PrintCluster   -------------------------------
void PndGemFindClusters::PrintCluster(Int_t clus)
{
  if (fVerbose > 0)
    cout << "PndGemFindClusters::PrintCluster()" << endl;

  cout << "CLUSTER " << clus << " IN " << fDigiClusters[clus].detId << " AT " << fDigiClusters[clus].cluPos << " CREATED FROM " << fDigiClusters[clus].digiNr.size()
       << " DIGIS, FROM " << fDigiClusters[clus].cluPMn << " TO " << fDigiClusters[clus].cluPMx << " WITH MAXIMUM OF " << fDigiClusters[clus].cluMVl << " AT "
       << fDigiClusters[clus].cluMPs << endl;

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

// -----   Private method Finish   ------------------------------------------
void PndGemFindClusters::Finish()
{
  if (fClusters)
    fClusters->Clear();

  cout << "-------------------- " << fName.Data() << " : Summary -----------------------" << endl;
  cout << " Events:        " << setw(10) << fTNofEvents << endl;
  cout << " Digis:         " << setw(10) << fTNofDigis << "    ( " << (Double_t)fTNofDigis / ((Double_t)fTNofEvents) << " per event )" << endl;
  cout << " Clusters:      " << setw(10) << fTNofClusters << "    ( " << (Double_t)fTNofClusters / ((Double_t)fTNofEvents) << " per event )" << endl;
  cout << "                       -->    ( " << (Double_t)fTNofDigis / ((Double_t)fTNofClusters) << " digis per cluster )" << endl;
  cout << "---------------------------------------------------------------------" << endl;
}
// -------------------------------------------------------------------------

ClassImp(PndGemFindClusters)
