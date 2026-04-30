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

//--------------------------------------------------------------------------
// Description:
//	Class PndEmcOnlineBumpSplitter.
//      Implementation of OnlineBumpSplitter which splits clusters based on
//      local maxima, determined in a 0th order approximation, i.e. by looking
//		for the two most energetic digis and checking if they are neighbours.
//
// Environment:
//	Software developed for the PANDA experiment at GSI.
//
// Author List:
//      Marcel Tiemens
//------------------------------------------------------------------------

#include "PndEmcViewClusters.h"

#include "PndEmcClusterProperties.h"
#include "PndEmcTwoCoordIndex.h"
#include "PndEmcDigi.h"
#include "PndEmcCluster.h"
#include "PndEmcBump.h"
#include "PndEmcRecoPar.h"
#include "PndEmcGeoPar.h"
#include "PndEmcDigiPar.h"
#include "PndEmcStructure.h"
#include "PndEmcMapper.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"

#include "TClonesArray.h"
#include "TStopwatch.h"

#include <algorithm>
#include <iostream>

using std::cin;
using std::cout;
using std::endl;
std::vector<Double_t> tVector;

//----------------
// Constructors --
//----------------

PndEmcViewClusters::PndEmcViewClusters(Int_t verbose)
  : fClusterArray(0), fGeoPar(new PndEmcGeoPar()), fDigiPar(new PndEmcDigiPar()), fRecoPar(new PndEmcRecoPar()), fPersistance(kTRUE), fClusterPosParam(), Ethres(0), fMode(1),
    FairTask("PndEmcViewClusters", verbose)
{
  fClusterPosParam.clear();
}

//--------------
// Destructor --
//--------------

PndEmcViewClusters::~PndEmcViewClusters() {}

/**
 * @brief Init Task
 *
 * Prepares the TClonesArray of PndEmcCluster for reading.
 *
 * @return InitStatus
 * @retval kSUCCESS success
 */
InitStatus PndEmcViewClusters::Init()
{

  LOG(info) << " PndEmcViewClusters: start Init()";
  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    cout << "-E- PndEmcViewClusters::Init: "
         << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  // Geometry loading
  fGeoPar->InitEmcMapper();
  PndEmcStructure::Instance();

  // Get input array
  fClusterArray = (TClonesArray *)ioman->GetObject("EmcCluster");
  if (!fClusterArray) {
    cout << "-W- PndEmcViewClusters::Init: "
         << "No PndEmcCluster array!" << endl;
    return kERROR;
  }

  // declare histograms to be filled
  h1 = new TH1F("h1", "Within-timebunch cluster energy distibution (GeV)", 80, 0, 4);
  // TH2D h2("h2","Hit distribution (GeV)",2*38.5,-38.5,38.5,2*38.5,-38.5,38.5); // pick this to show only Fw Endcap
  h2 = new TH2D("h2", "Hit distribution", 200, -40, 160, 120, -40, 80); // pick this to show all hits (XPad,YPad)
  // TH2D h2("h2","Hit distribution",300,-150,150,200,-100,100); // pick this to show all hits (x,y)

  c2 = new TCanvas("c2", "Geometry", -1000, 1000);
  c2->Divide(1, 2);
  h1->GetXaxis()->SetTitle("Cluster energy (GeV)");
  h1->GetYaxis()->SetTitle("#Clusters");
  h2->GetXaxis()->SetTitle("Crystal x-coordinate");
  h2->GetYaxis()->SetTitle("Crystal y-coordinate");
  h2->GetXaxis()->CenterTitle();
  h2->GetYaxis()->CenterTitle();
  // h2.GetZaxis()->SetTitle("Cluster energy (GeV)");
  // gStyle->SetPalette(53); // change colour palette
  gStyle->SetOptStat(0); // Draw without statistics shown

  LOG(info) << " PndEmcViewClusters: " << (ioman->GetInTree())->GetEntriesFast() << " entries.";

  LOG(info) << " PndEmcViewClusters: Intialization successful";
  return kSUCCESS;
}

/**
 * @brief Runs the task
 *
 * Draws EmcClusters and displays their content.
 *
 * @param opt unused
 * @return void
 */
void PndEmcViewClusters::Exec(Option_t *opt)
{

  int nClusters = fClusterArray->GetEntriesFast();
  int fEventCounter = 0;

  PndEmcMapper *fEmcMap = PndEmcMapper::Instance();

  if (fMode == 1) { // view clusters
    cout << endl;
    cout << "[input] => Press 'Enter' to show the next timebunch. ";
    if (cin.get() == '\n') {
      h1->Reset();
      h2->Reset();
      if (Ethres > 0) {
        int nClusAboveThreshold = 0;
        for (int k = 0; k < nClusters; k++) {
          PndEmcCluster *theCluster = (PndEmcCluster *)fClusterArray->At(k);
          if (theCluster->GetEnergy() > Ethres)
            nClusAboveThreshold++;
        }
        cout << "[info] => " << nClusAboveThreshold << " out of " << nClusters << " clusters have an energy above threshold." << endl;
      } else
        cout << "[info] => There are " << nClusters << " clusters in this timebunch." << endl;
      for (int j = 0; j < nClusters; j++) {
        PndEmcCluster *myCluster = (PndEmcCluster *)fClusterArray->At(j);
        if (myCluster->GetEnergy() < Ethres)
          continue;                       //																							NOTE: Corrected for flight time (cluster, not digis)
        h1->Fill(myCluster->GetEnergy()); //																																		|
        cout.precision(10);               //																																					V
        cout << "Cluster " << j + 1 << ": " << myCluster->GetEnergy() << " GeV, Position: [" << myCluster->where().x() << ", " << myCluster->where().y()
             << "], t = " << myCluster->GetTimeStamp() << endl;

        FairMultiLinkedData digiLinks = myCluster->GetLinksWithType(FairRootManager::Instance()->GetBranchId("EmcDigiSorted"));

        for (int iDigi = 0; iDigi < digiLinks.GetNLinks(); iDigi++) {
          PndEmcDigi *theDigi = (PndEmcDigi *)FairRootManager::Instance()->GetCloneOfLinkData(digiLinks.GetLink(iDigi));
          if (theDigi) {
            h2->Fill(theDigi->GetXPad(), theDigi->GetYPad());
            cout << "\tDigi " << iDigi + 1 << ", t = " << theDigi->GetTimeStamp() << ", (x,y) = (" << theDigi->where().x() << ", " << theDigi->where().y()
                 << "), EvtNo = " << theDigi->GetEventNr() << endl;
          } else {
            cout << "-E in PndEmcViewClusters::Exec FairLink: " << digiLinks.GetLink(iDigi) << " to EmcDigi delivers null" << std::endl;
          }
        }
      }
      cout << "[info] => Drawing plots for timebunch " << fEventCounter << "..." << std::flush;
      c2->cd(1); // Draw geometrical distribution in pad 1
      gPad->SetGridx();
      gPad->SetGridy();
      // gPad->SetLogz();
      h2->Draw("colz");
      c2->cd(2); // Draw energy distribution in pad 2
      h1->Draw();
      c2->Update();
      cout << " done." << endl;
      fEventCounter++;
    } else {
      cout << "[error] => You typed something else, didn't you? Fine. Terminating visualisation process." << endl;
      abort();
    }
  }

  else if (fMode == 2) { // get digi time distribution
    for (int j = 0; j < nClusters; j++) {
      PndEmcCluster *myCluster = (PndEmcCluster *)fClusterArray->At(j);
      if (myCluster->GetEnergy() < Ethres)
        continue;

      FairMultiLinkedData digiLinks = myCluster->GetLinksWithType(FairRootManager::Instance()->GetBranchId("EmcDigiSorted"));

      for (int iDigi = 0; iDigi < digiLinks.GetNLinks(); iDigi++) {
        PndEmcDigi *theDigi = (PndEmcDigi *)FairRootManager::Instance()->GetCloneOfLinkData(digiLinks.GetLink(iDigi));
        if (theDigi) {
          tVector.push_back(theDigi->GetTimeStamp());
        }
      }
    }
  }
}

void PndEmcViewClusters::FinishTask()
{
  TH1D *hTimes = new TH1D("hTimes", "Time difference between digis", 800, 0, 400);
  Double_t dt = 0;

  for (int k = 0; k < tVector.size(); k++) {
    dt = tVector[k + 1] - tVector[k];
    hTimes->Fill(dt);
  }

  if (fMode == 2) {
    TCanvas *cTimes = new TCanvas("cTimes", "Digi times", 1);
    cTimes->cd(1);
    hTimes->GetXaxis()->SetTitle("Time between digis (ns)");
    hTimes->Draw("");
  }
}

void PndEmcViewClusters::SetParContainers()
{

  // Get run and runtime database
  FairRun *run = FairRun::Instance();
  if (!run) {
    std::cout << ("-E- PndEmcViewClusters::SetParContainers: No analysis run") << std::endl;
  }

  FairRuntimeDb *db = run->GetRuntimeDb();
  if (!db) {
    std::cout << ("-E- PndEmcViewClusters::SetParContainers: No runtime database") << std::endl;
  }
  // Get Emc digitisation parameter container
  fGeoPar = (PndEmcGeoPar *)db->getContainer("PndEmcGeoPar");
  // Get Emc digitisation parameter container
  fDigiPar = (PndEmcDigiPar *)db->getContainer("PndEmcDigiPar");
  // Get Emc reconstruction parameter container
  fRecoPar = (PndEmcRecoPar *)db->getContainer("PndEmcRecoPar");
}

ClassImp(PndEmcViewClusters)
