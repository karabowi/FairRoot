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

//---------------------------------------------------------------------
// File and Version Information:
// 	$Id: $
//
// Description:
//	This object constructs a list of clusters from a list of digis.
//	Online version.
//
// Environment:
//	Software developed for the PANDA Detector at GSI.
//

#include "PndEmcMakeClusterOnline.h"

#include "PndEmcClusterProperties.h"
#include "PndEmcXClMoments.h"
#include "PndEmcStructure.h"
#include "PndEmcMapper.h"
#include "PndEmcGeoPar.h"
#include "PndEmcDigiPar.h"
#include "PndEmcRecoPar.h"
#include "PndEmcCluster.h"
#include "PndEmcDigi.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"
#include "FairRootFileSink.h"

#include "TClonesArray.h"
#include "TROOT.h"
#include "TLeaf.h"
#include "TCanvas.h"
#include "TH1.h"

#include <iostream>
#include <fstream>

using std::cout;
using std::endl;

static Int_t evtCounter = 0;
static Int_t digiCounter = 0;
// *** create an output file
// ofstream outfile;

PndEmcMakeClusterOnline::PndEmcMakeClusterOnline(Int_t verbose, Bool_t storeclusters)
  : PndPersistencyTask("EmcClusteringTask", verbose), fDigiArray(nullptr), fClusterArray(nullptr), fGeoPar(new PndEmcGeoPar()), fRecoPar(new PndEmcRecoPar()),
    fDigiEnergyTresholdBarrel(0.), fDigiEnergyTresholdFWD(0.), fDigiEnergyTresholdBWD(0.), fDigiEnergyTresholdShashlyk(0.), fTimebunchCutTime(0.), fDigiFunctor(nullptr),
    fStoreClusters(storeclusters), fStoreClusterBase(kTRUE), fClusterPosParam(), fNrOfEvents(0), fNrOfDigis(0), digiCounter(0), evtCounter(0), nOnlProg(0), OtotRtime(0),
    OtotCtime(0), fClusterActiveTime(5.), fClusterEnergyCut(0.030), fRemoveLowEclus(kTRUE), fAutoDetermine(kFALSE)
{
  SetPersistency(storeclusters);
  fClusterPosParam.clear();
}

//--------------
// Destructor --
//--------------

PndEmcMakeClusterOnline::~PndEmcMakeClusterOnline() {}

/**
 * @brief Init Task
 *
 * Prepares the TClonesArray of PndEmcDigi for reading and PndEmcCluster for writing.
 * Also reads the EMC parameters, sets the energy thresholds for digis, and sets the
 * neighbour and position methods to be used.
 *
 * @return InitStatus
 * @retval kSUCCESS success
 */
// -----   Public method Init   -------------------------------
InitStatus PndEmcMakeClusterOnline::Init()
{

  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    LOG(error) << " PndEmcMakeClusterOnline::Init: "
               << "RootManager not instantiated!";
    return kFATAL;
  }

  /*if(!FairRunAna::Instance()->IsTimeStamp()) {
    LOG(error) << " PndEmcMakeClusterOnline::Init: " << "This task can be activated only online" ;
    return kFATAL;
  }*/

  // Get nr of events
  TTree *tIn = (ioman->GetInTree());
  fNrOfEvents = tIn->GetEntriesFast();
  fNrOfDigis = tIn->Draw("EmcDigi.fEnergy>>hist", "", "goff");

  // Automatically determine time cuts

  if (fAutoDetermine) {
    cout << endl;
    std::vector<Double_t> tVector;
    TH1D *hDigiDt = new TH1D("hDigiDt", "Time difference between Digis", 1000, 0, 1000);
    int nProg = 0;
    for (int i = 0; i < fNrOfEvents; i++) {
      // get Digis of this event
      TClonesArray *DigiArray = new TClonesArray("PndEmcDigi");
      tIn->SetBranchAddress("EmcDigi", &DigiArray);
      tIn->GetEntry(i);
      int len = DigiArray->GetEntriesFast();
      for (int j = 0; j < len; j++) {
        PndEmcDigi *myDigi = (PndEmcDigi *)DigiArray->At(j);
        tVector.push_back(myDigi->GetTimeStamp());
      }
      if (i - 1 == nProg * fNrOfEvents / 100) {
        cout << "\r[INFO\t] Automatically determining time cuts: " << nProg << "\% completed." << std::flush;
        nProg += 5;
      }
    }

    Double_t dt = 0;
    int nDigis = tVector.size();
    int *index = new int[nDigis];
    double *tArray = new double[nDigis];
    for (int s = 0; s < nDigis; s++)
      tArray[s] = tVector[s];
    TMath::Sort(nDigis, tArray, index, false);
    for (int k = 0; k < tVector.size(); k++) {
      dt = tVector[index[k + 1]] - tVector[index[k]];
      hDigiDt->Fill(dt);
      if (k == nProg * tVector.size() / 100 - 1) {
        cout << "\r[INFO\t] Automatically determining time cuts: " << nProg << "\% completed." << std::flush;
        nProg += 5;
      }
    }
    cout << endl;

    Int_t integral = hDigiDt->GetBinContent(hDigiDt->GetNbinsX() + 1); // start by including overflow bin
    Int_t dtau_cut = fTimebunchCutTime;
    for (int iDtau = hDigiDt->GetNbinsX(); iDtau >= 0; iDtau--) {
      integral += hDigiDt->GetBinContent(iDtau);
      dtau_cut = iDtau;
      if (integral > 1.02 * fNrOfEvents)
        break; // assume some small inherent efficiency loss
    }

    if (!dtau_cut) {
      cout << "[WARNING] Automatic time cut setting failed, using default values!" << endl;
      fAutoDetermine = kFALSE;
    }
    if (fAutoDetermine) {
      SetTimebunchCutTime(dtau_cut);
      SetClusterActiveTime(dtau_cut * 2);
    }
  }

  // Get input array
  if (FairRunAna::Instance()->IsTimeStamp())
    fDigiArray = (TClonesArray *)ioman->GetObject("EmcDigiSorted"); // for timebased, use sorted digis
  else
    fDigiArray = (TClonesArray *)ioman->GetObject("EmcDigi"); // for eventbased, use "normal" digis
  if (!fDigiArray) {
    LOG(warn) << " PndEmcMakeClusterOnline::Init: "
              << "No PndEmcDigi array!";
    return kERROR;
  }

  // Create and register output array
  if (FairRunAna::Instance()->IsTimeStamp())
    fClusterArray = ioman->Register("EmcClusterTemp", "PndEmcCluster", "Emc", fStoreClusters);
  else
    fClusterArray = ioman->Register("EmcCluster", "PndEmcCluster", "Emc", fStoreClusters);

  // for subsequent methods we need the "event grouping" as given by the TS buffer
  // --> fDigiArray becomes an output array.
  //
  // can be removed once PndEmcCluster object has been rewritten
  if (FairRunAna::Instance()->IsTimeStamp()) {
    // for subsequent methods we need the "event grouping" as given by the TS buffer --> fDigiArray becomes an output array.
    fDigiArray = new TClonesArray(fDigiArray->GetClass()); // still needed to activate array
    ioman->Register("EmcDigiClusterBase", "Emc", fDigiArray, fStoreClusterBase);
    cout << "\n[INFO\t] Running TIMEBASED version.\n" << endl;
  } else
    cout << "\n[INFO\t] Running EVENTBASED version.\n" << endl;

  // searching for time gaps in digi stream
  fDigiFunctor = new TimeGap();

  fGeoPar->InitEmcMapper();
  PndEmcStructure::Instance();

  // get some parameters from the parbase
  fDigiEnergyTresholdBarrel = fRecoPar->GetEnergyThresholdBarrel();
  fDigiEnergyTresholdFWD = fRecoPar->GetEnergyThresholdFWD();
  fDigiEnergyTresholdBWD = fRecoPar->GetEnergyThresholdBWD();
  fDigiEnergyTresholdShashlyk = fRecoPar->GetEnergyThresholdShashlyk();

  // convert from seconds to nanoseconds...in parfile everything is seconds...here we need nanoseconds
  if (fTimebunchCutTime == 0.)
    fTimebunchCutTime = fRecoPar->GetClusterActiveTime() * 1.0e9;

  if (!strcmp(fRecoPar->GetEmcClusterPosMethod(), "lilo")) {
    cout << "Lilo cluster position method" << endl;
    fClusterPosParam.push_back(fRecoPar->GetOffsetParmA());
    fClusterPosParam.push_back(fRecoPar->GetOffsetParmB());
    fClusterPosParam.push_back(fRecoPar->GetOffsetParmC());
  }

  if (FairRunAna::Instance()->IsTimeStamp()) {
    cout << "Minimum Time Between Timebunches: " << fTimebunchCutTime << " ns" << endl;
    cout << "Maximum Time Between Digis: " << fClusterActiveTime << " ns" << endl;
  }
  if (fRemoveLowEclus)
    cout << "Minimum cluster energy: " << fClusterEnergyCut << " GeV" << endl;

  cout << "=> " << fNrOfDigis << " digis in " << fNrOfEvents << " events." << endl;

  fNrOfDigis /= 100; // to save a calculation step for the progress counter

  /*	hClusMultiplicity = new TH1I("hClusMultiplicity","Number of high E (> 30 MeV) clusters per timebunch",100,0,100);
    hEventsPerCluster = new TH1I("hEventsPerCluster","Number of event numbers in a cluster",20,0,20);
    hTimeDifference = new TH1D("hTimeDifference","Time Distribution within a cluster",100,-10,90);*/
  //	outfile.open("digisLRdt100_ONL.txt");

  LOG(info) << " PndEmcMakeClusterOnline: Intialization successful";
  return kSUCCESS;
}

/**
 * @brief Runs the task.
 *
 * Fetches an array of @ref PndEmcDigi's by reading the current event in the tree for event-based usage,
 * or using @ref TimeGap() to load digis up to a certain time gap for time-based usage.
 * The task loops over all digis, and establishes space-time neighbour relations between them. It then
 * uses this info to tag which digis should be put into which PndEmcCluster. Designed for use in the
 * time-based framework.
 *
 * @param opt unused
 * @return void
 */
void PndEmcMakeClusterOnline::Exec(Option_t *)
{

  // Reset output arrays and get all digis up to a certain time gap specified by fTimebunchCutTime
  if (!fClusterArray)
    Fatal("Exec", "No Cluster Array");
  fClusterArray->Delete();
  if (FairRunAna::Instance()->IsTimeStamp()) {
    fDigiArray->Delete();
    fDigiArray->AbsorbObjects(FairRootManager::Instance()->GetData("EmcDigiSorted", fDigiFunctor, fTimebunchCutTime));
  }

  evtCounter++;
  /*	if (evtCounter%250 == 0) {
      if(FairRunAna::Instance()->IsTimeStamp()) cout << "\r[INFO\t] PndEmcMakeClusterOnline: " << evtCounter << "\ttimebunches processed." << std::flush;
      else cout << "\r[INFO\t] PndEmcMakeClusterOnline: " << evtCounter << "\tevents processed." << std::flush;
    }*/

  // -----   Timer for clustering  -----------------------------------------
  TStopwatch Ctimer;
  Ctimer.Start();

  // --------------------- START OF CLUSTER FINDING ALGORITHM ---------------------------

  Int_t dx = 0;
  Int_t dy = 0;
  Int_t clusterNr = 0;
  Int_t nDigisPassed = 0; // keeps track of the number of digis in this timebunch AND that passed the thresholds, so use as upper limit for loops later on

  Int_t deltaD = 1; // neighbour distance to consider

  Double_t dt = 0;
  Double_t deltaT = 0;
  /*	if(FairRunAna::Instance()->IsTimeStamp()) {
      dt = 0;
      deltaT = fClusterActiveTime; // threshold for time between digis in a cluster
    }	*/

  Int_t nDigis = fDigiArray->GetEntriesFast();
  // outfile << "\n// ---> Timebunch " << evtCounter << " (" << nDigis << " digis)" << endl;
  std::vector<Int_t> neighbours; // make arrays dynamic, as we don't know its size in advance
  std::vector<Int_t> DigiPassed;
  std::vector<Int_t> XPadPassed;
  std::vector<Int_t> YPadPassed;
  std::vector<Double_t> TPassed;
  std::vector<Int_t> isAdded;
  std::vector<Int_t> similarities;

  for (Int_t a = 0; a < nDigis; a++)
    isAdded.push_back(-1); // initialise all entries of isAdded to -1 (needed by algorithm)

  if (fVerbose > 2) {
    cout << "DigiList length: " << nDigis << endl;
  }

  // loop over all digis to add them to clusters

  for (Int_t iDigi = 0; iDigi < nDigis; ++iDigi) {

    if (digiCounter - 1 == nOnlProg * fNrOfDigis) {
      cout << "\r[INFO\t] PndEmcMakeClusterOnline: " << nOnlProg << "\% completed." << std::flush;
      nOnlProg += 1;
    }
    digiCounter++;

    /* Get a new digi from the digi array */
    PndEmcDigi *theDigi = (PndEmcDigi *)fDigiArray->At(iDigi);

    // thresholds: if energy is below the corresponding threshold, digi is not considered in clustering
    Int_t module = theDigi->GetModule();
    if ((module == 1 || module == 2) && (theDigi->GetEnergy() < fDigiEnergyTresholdBarrel))
      continue;
    if ((module == 3) && (theDigi->GetEnergy() < fDigiEnergyTresholdFWD))
      continue;
    if ((module == 4) && (theDigi->GetEnergy() < fDigiEnergyTresholdBWD))
      continue;
    if ((module == 5) && (theDigi->GetEnergy() < fDigiEnergyTresholdShashlyk))
      continue;

    // fill arrays with digi information for neighbour relationship building
    DigiPassed.push_back(iDigi); // keep track which digis in fDigiArray passed the thresholds, needed to correct for gaps in isAdded[] later on caused by hits being skipped when
                                 // they are below threshold
    if (FairRunAna::Instance()->IsTimeStamp())
      TPassed.push_back(theDigi->GetTimeStamp());
    if (theDigi->GetXPad() < 0 && module == 3)
      XPadPassed.push_back(theDigi->GetXPad() + 1); // correct for gaps in FwEndcap
    else
      XPadPassed.push_back(theDigi->GetXPad());
    if (theDigi->GetYPad() < 0 && module == 3)
      YPadPassed.push_back(theDigi->GetYPad() + 1);
    else
      YPadPassed.push_back(theDigi->GetYPad());
    nDigisPassed++;
    // outfile.precision(11);
    // if (theDigi->GetEnergy() > 0.03) outfile << "t:" << theDigi->GetTimeStamp() << "\t";
  }

  if (fVerbose > 4) {
    cout << "Looped over all digis (" << nDigisPassed << " of " << nDigis << " digis passed)" << endl;
  }

  if (nDigisPassed > nDigis) {
    Fatal("Exec", "Attempt to process more digis than are present in this timebunch");
  }

  //----- First, construct matrix containing information which digis are neighbouring -----//

  for (Int_t d = 0; d < nDigisPassed - 1; d++) { // check all pairs of digis, so all digis up to the second-to-last one
    Int_t nNeighbours = 0;
    neighbours.push_back(0); // placeholder for nr of neighbours
    for (Int_t e = d + 1; e < nDigisPassed; e++) {
      //			dx = XPadPassed[e] - XPadPassed[d]; 		// check euclidian distance between all digis which passed threshold
      //			dy = YPadPassed[e] - YPadPassed[d]; 		// this construction ensures all pairs of digis are only checked once
      // if(FairRunAna::Instance()->IsTimeStamp()) dt = TMath::Abs(TPassed[e] - TPassed[d]);	// also take into account that non-consecutive digi pairs may differ in time more than
      // dtau ns

      /*			if (dy*dy <= deltaD*deltaD && dx*dx <= deltaD*deltaD && dt <= deltaT) { 	// ONLINE VERSION. If the distance is small enough, the digis are neighbours
              neighbours.push_back(e); // construct array containing neighbouring digis
              nNeighbours++; // keep track of nr of neighbours
            }*/

      if (static_cast<PndEmcDigi *>(fDigiArray->At(DigiPassed[e]))->isNeighbour(static_cast<PndEmcDigi *>(fDigiArray->At(DigiPassed[d]))) &&
          dt <= deltaT) {        // BUILT-IN PandaRoot VERSION. Check which digis are neighbours
        neighbours.push_back(e); // construct array containing neighbouring digis
        nNeighbours++;           // keep track of nr of neighbours
      }
    }
    neighbours[neighbours.size() - (nNeighbours + 1)] = nNeighbours; // write nr of neighbours to the appropiate entry in neighbours[]
  }                                                                  // after this loop, the stucture of neighbours[] is [#neighbours_1 hit1 ... hitN #neighbours_2 hit2 ... etc]

  // Primary Clustering
  Int_t k = 0;
  Int_t l = 0;
  Int_t nClusters = 0; // nr of preclusters
  Int_t simLength = 0;
  while (l < neighbours.size()) {
    if (isAdded[k] < 0) {                             // if it hasn't been added yet
      isAdded[k] = nClusters;                         // put internal cluster nr in isAdded array
      for (Int_t j = 1; j < neighbours[l] + 1; j++) { // for the first neighbouring hit upto #neighbours for this digi
        Int_t m = neighbours[l + j];                  // m = hit index
        if (isAdded[m] < 0)
          isAdded[m] = nClusters; // if hit hasn't been added, put internal cluster nr here
        else if (isAdded[m] != nClusters) {
          if (nClusters > isAdded[m]) {
            similarities.push_back(nClusters);  // if it has, put current cluster nr in this array
            similarities.push_back(isAdded[m]); // together with its cluster nr (could be different, which is why is being stored here for later comparison)
          } else {
            similarities.push_back(isAdded[m]); // different order, so we always have the largest cluster nr first
            similarities.push_back(nClusters);
          }
          simLength += 2; // increment simLength by 2, as we just wrote 2 elements to similarities
        }
      }
      nClusters++;
    } else { // if isAdded[k] isn't -1, it means the digi has been added already and isAdded[k] contains its cluster nr
      for (Int_t j = 1; j < neighbours[l] + 1; j++) {
        Int_t m = neighbours[l + j];
        if (isAdded[m] < 0)
          isAdded[m] = isAdded[k]; // same as before, only use cluster nr found in isAdded[k]
        else if (isAdded[m] != isAdded[k]) {
          if (isAdded[k] > isAdded[m]) {
            similarities.push_back(isAdded[k]); // similarities[] keeps tracks of which preclusters have to be merged
            similarities.push_back(isAdded[m]);
          } else {
            similarities.push_back(isAdded[m]); // different order, so we always have the largest cluster nr first
            similarities.push_back(isAdded[k]);
          }
          simLength += 2;
        }
      }
    }
    k++;
    l += neighbours[l] + 1;
  }

  if (nDigisPassed != 0)
    if (isAdded[nDigisPassed - 1] < 0)
      isAdded[nDigisPassed - 1] = nClusters++;

  // Secondary clustering
  for (Int_t i = 0; i < simLength; i += 2) { // use info from similarities[] to merge clusters
    if (similarities[i] != similarities[i + 1]) {
      for (Int_t m = 0; m < nDigisPassed; m++) {
        if (isAdded[m] == similarities[i])
          isAdded[m] = similarities[i + 1];
      }
      for (Int_t j = i + 2; j < simLength; j++)
        if (similarities[j] == similarities[i])
          similarities[j] = similarities[i + 1];
    }
  }
  for (Int_t i = 0; i < nClusters; i++) {
    Int_t n = 0;
    for (Int_t j = 0; j < nDigisPassed; j++) {
      if (isAdded[j] == i) {
        n++;
        isAdded[j] = clusterNr; // info in isAdded: digi "index" belongs to cluster "isAdded[index]"
      }
    }
    if (n > 0)
      clusterNr++; // cluster nr, don't reset it while searching for clusters in a timebunch
  }

  //--- Finally, use isAdded to merge digis into clusters ---//

  for (Int_t i = 0; i < nDigisPassed; i++) {
    if (isAdded[i] <
        fClusterArray
          ->GetEntriesFast()) { // if isAdded[i] is smaller than the current #clusters, the cluster to which this digi belongs already exists, and we should add it to this cluster
      PndEmcCluster *cluster = (PndEmcCluster *)fClusterArray->At(isAdded[i]); // set pointer to the cluster we need, as indicated by isAdded[]
      cluster->addDigi(fDigiArray, DigiPassed[i]);                             // add digi with index as indicated by DigiPassed, so we add the correct one to the cluster

      /*			PndEmcDigi* myDigi = (PndEmcDigi*) fDigiArray->At(DigiPassed[i]);
            FairMultiLinkedData hitLinks = myDigi->GetLinksWithType(FairRootManager::Instance()->GetBranchId("EmcHit"));

            for (Int_t j=0; j<hitLinks.GetNLinks(); j++){
              PndEmcHit* hit = (PndEmcHit*)FairRootManager::Instance()->GetCloneOfLinkData(hitLinks.GetLink(j));
              //std::cout << "Hit: " << hit->GetDetectorID() << std::endl;
              if(hit) {
      //						std::cout << "Cluster : " << clustmarker << " Hit to add: " << hitLinks.GetLink(j) << std::endl;
                if (cluster->GetLinks().count(hitLinks.GetLink(j)) == 0){
                  cluster->AddTracksEnteringExiting(hit->GetTrackEntering(), hit->GetTrackExiting());
                  cluster->AddLink(hitLinks.GetLink(j));
      //						std::cout << "Links in Cluster: " << *cluster->GetPointerToLinks() << std::endl;
      //						std::cout << "EnteringExiting for Hit: " << hitLinks.GetLink(j) << std::endl;
      //						std::cout << "TrackEntering: " << hit->GetTrackEntering() << std::endl;
      //						std::cout << "TrackExiting: " << hit->GetTrackExiting() << std::endl;
      //						std::cout << "Resulting Enter: " << cluster->GetTrackEntering() << std::endl;
      //						std::cout << "Resulting Exit: " << cluster->GetTrackExiting() << std::endl;
                } else {
                  //std::cout << "Hit Already exists!" << std::endl;
                }
                delete(hit);

              } else {
                std::cout << "-E in PndEmcMakeCluster::Exec FairLink " << hitLinks.GetLink(j) << "to EmcHit delivers null" << std::endl;
              }
            }*/
    } else { // if not, make a new cluster for this digi
      PndEmcCluster *newCluster = new ((*fClusterArray)[fClusterArray->GetEntriesFast()]) PndEmcCluster();
      newCluster->addDigi(fDigiArray, DigiPassed[i]);

      /*			PndEmcDigi* myDigi = (PndEmcDigi*) fDigiArray->At(DigiPassed[i]);
            FairMultiLinkedData hitLinks = myDigi->GetLinksWithType(FairRootManager::Instance()->GetBranchId("EmcHit"));

            for (Int_t j=0; j<hitLinks.GetNLinks(); j++){
              PndEmcHit* hit = (PndEmcHit*)FairRootManager::Instance()->GetCloneOfLinkData(hitLinks.GetLink(j));
              if(hit) {
      //					std::cout << "NewCluster : " << isAdded[i] << " Hit Added: " << hitLinks.GetLink(j) << std::endl;
      //					std::cout << "TrackEntering: " << hit->GetTrackEntering() << std::endl;
      //					std::cout << "TrackExiting: " << hit->GetTrackExiting() << std::endl;
                newCluster->SetTrackEntering(hit->GetTrackEntering());
                newCluster->SetTrackExiting(hit->GetTrackExiting());
                newCluster->SetInsertHistory(kFALSE);
                newCluster->AddLink(hitLinks.GetLink(j));
      //					std::cout << "Resulting Enter: " << newCluster->GetTrackEntering() << std::endl;
      //					std::cout << "Resulting Exit: " << newCluster->GetTrackExiting() << std::endl;
      //					std::cout << "Links in Cluster: " << *newCluster->GetPointerToLinks() << std::endl;
                delete(hit);
              } else {
                std::cout << "-E in PndEmcMakeCluster::Exec FairLink " << hitLinks.GetLink(j) << "to EmcHit delivers null" << std::endl;
              }
            }*/
    }
  }

  Ctimer.Stop();
  Double_t Crtime = Ctimer.RealTime();
  Double_t Cctime = Ctimer.CpuTime();
  Ctimer.Reset();

  OtotRtime += Crtime; // keep track how much time was spent on preclustering
  OtotCtime += Cctime;

  // --------------------- stop---------------------------------------------

  FinishClusters();

  if (fRemoveLowEclus)
    RemoveLowEnergyClusters(); // !! significantly slows down cluster finding task, but will speed up future processing and reconstruction tasks
}

/**
 * @brief Cluster loop
 *
 * Loops over the final PndEmcCluster objects to assign its properties.
 *
 * @return void
 */
void PndEmcMakeClusterOnline::FinishClusters()
{

  Int_t nCluster = fClusterArray->GetEntriesFast();
  for (Int_t i = 0; i < nCluster; i++) {
    FinishCluster((PndEmcCluster *)(fClusterArray->At(i)));
  }
}

/**
 * @brief Assign cluster properties
 *
 * Assigns cluster properties, such as energy, position, Zernike moments, and time.
 *
 * @return void
 */
void PndEmcMakeClusterOnline::FinishCluster(PndEmcCluster *cluster)
{

  std::vector<Int_t> list = cluster->DigiList();
  //	std::vector<Int_t> eventVect;
  //	Bool_t unique = true;

  Double_t total_energy = 0;
  Double_t max_energy = 0;
  Int_t max_energy_idx = 0;

  for (size_t iDigi = 0; iDigi < list.size(); ++iDigi) {
    Int_t idx = list[iDigi];
    PndEmcDigi *thedigi = (PndEmcDigi *)fDigiArray->UncheckedAt(idx);

    total_energy += thedigi->GetEnergy();
    if (thedigi->GetEnergy() > max_energy) {
      max_energy = thedigi->GetEnergy();
      max_energy_idx = idx;
    }
    /*unique = true;
    for (Int_t e=0; e<eventVect.size(); e++) {
      if (eventVect[e] == thedigi->GetEventNr()) {
        unique = false;
        break;
      }
    }
    if (unique) {
      eventVect.push_back(thedigi->GetEventNr());
    }*/
  }

  //	hEventsPerCluster->Fill(eventVect.size());

  cluster->SetEnergy(total_energy);

  PndEmcClusterProperties clustProperties(*cluster, fDigiArray);

  TVector3 tmpbumppos = clustProperties.Where(fRecoPar->GetEmcClusterPosMethod(), fClusterPosParam);
  cluster->SetPosition(tmpbumppos);

  Double_t flightTime = tmpbumppos.Mag() / 29.9792458; // flight time in ns = distance to IP / c

  cluster->SetTimeStamp(static_cast<PndEmcDigi *>(fDigiArray->UncheckedAt(max_energy_idx))->GetTimeStamp() - flightTime); // correct cluster time for photon flight time

  PndEmcXClMoments xClMoments(*cluster, fDigiArray);
  cluster->SetZ20(xClMoments.AbsZernikeMoment(2, 0, 15));
  cluster->SetZ53(xClMoments.AbsZernikeMoment(5, 3, 15));
  cluster->SetLatMom(xClMoments.Lat());

  cluster->SetModule(static_cast<PndEmcDigi *>(fDigiArray->UncheckedAt(max_energy_idx))->GetModule());
}

/**
 * @brief Remove low energetic clusters
 *
 * Loops over the PndEmcCluster objects, and removes them if their energy is below @ref fClusterEnergyCut.
 * This helps suppress noise hits. This task can be enabled/disabled by calling @ref
 * EnableRemovalOfLowEnergyClusters(), using kTRUE or kFALSE as arguments, respectively. By default,
 * it is enabled with an energy cut of 30 MeV.
 *
 * @return void
 */
void PndEmcMakeClusterOnline::RemoveLowEnergyClusters()
{

  Int_t nClusters = fClusterArray->GetEntriesFast();
  //	Int_t fRemovedClusters = 0;

  /*	for (Int_t i=0; i<nClusters; i++) {
      PndEmcCluster *myCluster=(PndEmcCluster*)fClusterArray->At(i);
      if (myCluster->NumberOfDigis() < 3) continue;
      //Double_t flightTime = myCluster->where().Mag()/29.9792458;
      std::vector<Int_t> list = myCluster->DigiList();

      for (Int_t iDigi=0; iDigi<list.size()-1; ++iDigi) {
        PndEmcDigi* thedigi1 = (PndEmcDigi*) fDigiArray->At(list[iDigi]);
        for (Int_t iDi=iDigi+1; iDi<list.size(); ++iDi) {
          PndEmcDigi* thedigi2 = (PndEmcDigi*) fDigiArray->At(list[iDigi+1]);
          if (thedigi1->isNeighbour(thedigi2)) hTimeDifference->Fill(thedigi2->GetTimeStamp()-thedigi1->GetTimeStamp()); // put time distribution in histo
          //if (thedigi1->GetEnergy() > 0.01 && thedigi2->GetEnergy() > 0.01) hTimeDifference->Fill(thedigi2->GetTimeStamp()-thedigi1->GetTimeStamp()); // put time distribution in
    histo
          //if (thedigi1->GetEnergy() > 0.01) hTimeDifference->Fill(thedigi1->GetTimeStamp()-myCluster->GetTimeStamp()); // put time distribution in histo
        }

      }
    }*/

  for (Int_t i = 0; i < nClusters; i++) {
    PndEmcCluster *myCluster = (PndEmcCluster *)fClusterArray->At(i);
    if (myCluster->GetEnergy() < fClusterEnergyCut) {
      fClusterArray->RemoveAt(i);
      //			fRemovedClusters++;
    }
  }
  fClusterArray->Compress();

  //	hClusMultiplicity->Fill(nClusters-fRemovedClusters);
}

/**
 * @brief Finish the task
 *
 * Displays the time needed to perform the sub-task, and some other statistics.
 *
 * @return void
 */
void PndEmcMakeClusterOnline::FinishTask()
{

  std::cout << "\nProcessed in total " << digiCounter << " digis within " << evtCounter << " timebunches." << std::endl;
  cout << "\n[INFO\t] Clustering: real time: " << OtotRtime << " s, cpu time: " << OtotCtime << " s." << endl;

  // store used value of fTimebunchCutTime
  TVector3 *cuttingTime = new TVector3(fTimebunchCutTime, fClusterActiveTime, fNrOfEvents);
  FairRootManager *ioman = FairRootManager::Instance();
  auto sink = ioman->GetSink();
  assert(sink->GetSinkType() == Sink_Type::kFILESINK);
  auto rootFileSink = static_cast<FairRootFileSink *>(sink);
  TTree *tOut = rootFileSink->GetOutTree();
  tOut->GetUserInfo()->Add(cuttingTime);
  /*	outfile.close();

    TCanvas *cN = new TCanvas("cN","Multiplicity",1);
    cN->cd(1);
    hClusMultiplicity->Draw("");

    TCanvas *c1 = new TCanvas("c1","Multiplicity",1);
    c1->cd(1);
    hEventsPerCluster->Draw("");

    TCanvas *cT = new TCanvas("cT","Time",1);
    cT->cd(1);
    hTimeDifference->Draw("");*/
}

/**
 * @brief Set parameter containers
 *
 * Instruct the parent task where to find (some of) the parameters that are used.
 *
 * @return void
 */
void PndEmcMakeClusterOnline::SetParContainers()
{

  // Get run and runtime database
  FairRun *run = FairRun::Instance();
  if (!run)
    LOG(fatal) << "SetParContainers: No analysis run";

  FairRuntimeDb *db = run->GetRuntimeDb();
  if (!db)
    LOG(fatal) << "SetParContainers: No runtime database";

  // Get Emc geometry parameter container
  fGeoPar = (PndEmcGeoPar *)db->getContainer("PndEmcGeoPar");

  // Get Emc reconstruction parameter container
  fRecoPar = (PndEmcRecoPar *)db->getContainer("PndEmcRecoPar");
}

/**
 * @brief Storage
 *
 * Set whether or not to store the final cluster objects in the output tree (enabled by default).
 *
 * @return void
 */
void PndEmcMakeClusterOnline::SetStorageOfData(Bool_t val)
{
  SetPersistency(val);
  return;
}

ClassImp(PndEmcMakeClusterOnline)
