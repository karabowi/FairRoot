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
// 	+ Implementation of online clustering algorithm
// 	+ Mapping of digis into virtual data concentrators (more closely resembles real readout chain)
//
// Environment:
//	Software developed for the PANDA Detector at GSI.
//

#include "PndEmcDistributedClustering.h"

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
#include "TVector3.h"

#include <iostream>
#include <fstream>
#include <vector>

using std::cout;
using std::endl;

// std::vector<Int_t> tagSingleClusArray;
// *** create an output file
// ofstream ofile;
// ofstream ofileClus;

PndEmcDistributedClustering::PndEmcDistributedClustering(Int_t verbose, Bool_t storeclusters)
  : FairTask("EmcClusteringTask", verbose), fDigiArray(nullptr), fPreclusterArray(nullptr), fClusterArray(nullptr), fDigiFunctor(nullptr), fDigiEnergyTresholdBarrel(0.),
    fDigiEnergyTresholdFWD(0.), fDigiEnergyTresholdBWD(0.), fDigiEnergyTresholdShashlyk(0.), fTimebunchCutTime(0.), fGeoPar(new PndEmcGeoPar()), fRecoPar(new PndEmcRecoPar()),
    fStoreClusters(storeclusters), fStoreClusterBase(kTRUE), fClusterPosParam(), fNrOfEvents(0), fClusterActiveTime(5.), fClusterEnergyCut(0.030), fRemoveLowEclus(kTRUE),
    fPosMethod(0), fNbMethod(0), evtCounter(0), digiCounter(0), fNrOfDigis(0), nDistProg(0), fRemovedClusters(0), DCtotRtime(0), DCtotCtime(0), CNtotRtime(0), CNtotCtime(0),
    tagSingleClus(false), printClusters(false), fAutoDetermine(kFALSE), fFactor()
{
  fClusterPosParam.clear();
}

//--------------
// Destructor --
//--------------

PndEmcDistributedClustering::~PndEmcDistributedClustering() {}

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
InitStatus PndEmcDistributedClustering::Init()
{

  LOG(info) << " PndEmcDistributedClustering::Init(): Start Initialization";

  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    LOG(error) << " PndEmcDistributedClustering::Init: "
               << "RootManager not instantiated!";
    return kFATAL;
  }

  // Get nr of events	and digis (for progress counter)
  TTree *tIn = (ioman->GetInTree());
  fNrOfEvents = tIn->GetEntriesFast();
  fNrOfDigis = tIn->Draw("EmcDigi.fEnergy>>hist", "", "goff");

  // Automatically determine time cuts

  if (fAutoDetermine && FairRunAna::Instance()->IsTimeStamp()) {
    cout << endl;
    std::vector<Double_t> tVector;
    TH1D *hDigiDt = new TH1D("hDigiDt", "Time difference between Digis", 1000, 0, 1000);
    int nProg = 0;
    for (int i = 0; i < fNrOfEvents; i++) {
      // get Digis of this event
      TClonesArray *DigiArray = new TClonesArray("PndEmcDigi");
      tIn->SetBranchAddress("EmcDigiSorted", &DigiArray);
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
    /*		int* index = new int [nDigis];
        double* tArray = new double [nDigis];
        for (int s=0; s<nDigis; s++) tArray[s] = tVector[s];
        TMath::Sort(nDigis,tArray, index, false);*/
    for (int k = 0; k < nDigis; k++) {
      // if ((k+1) <= nDigis) dt = tVector[index[k+1]]-tVector[index[k]];
      dt = tVector[k + 1] - tVector[k];
      hDigiDt->Fill(dt);
      if (k == nProg * nDigis / 100 - 1) {
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
      if (integral > 0.98 * fNrOfEvents)
        break; // assume some small inherent efficiency loss
    }

    if (!dtau_cut) {
      cout << "[WARNING] Automatic time cut setting failed, using default values!" << endl;
      fAutoDetermine = kFALSE;
    }
    if (fAutoDetermine) {
      SetTimebunchCutTime(dtau_cut);
      if (fFactor->Y() == 0)
        SetClusterActiveTime(dtau_cut * fFactor->X());
      else if (fFactor->Y() == 1)
        SetClusterActiveTime(dtau_cut + fFactor->X());
      else {
        cout << "-E- PndEmcDistributedClustering::Init: "
             << "Unknown operational mode set for dt factor. Set to 0 for multiplication, or to 1 for addition. Using default value, 0." << endl;
        SetClusterActiveTime(dtau_cut * fFactor->X());
      }
    }
  }

  // Get input array
  if (FairRunAna::Instance()->IsTimeStamp())
    fDigiArray = (TClonesArray *)ioman->GetObject("EmcDigiSorted"); // for timebased, use sorted digis
  else
    fDigiArray = (TClonesArray *)ioman->GetObject("EmcDigi"); // for eventbased, use "normal" digis
  if (!fDigiArray) {
    LOG(warn) << " PndEmcDistributedClustering::Init: "
              << "No PndEmcDigi array!";
    return kERROR;
  }

  // Create and register output array
  fPreclusterArray = ioman->Register("EmcPrecluster", "PndEmcCluster", "Emc", kTRUE);
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
    cout << "If default is used: use Lilo cluster position method" << endl;
    fClusterPosParam.push_back(fRecoPar->GetOffsetParmA());
    fClusterPosParam.push_back(fRecoPar->GetOffsetParmB());
    fClusterPosParam.push_back(fRecoPar->GetOffsetParmC());
  }

  if (FairRunAna::Instance()->IsTimeStamp()) {
    cout << "Minimum time between timebunches: " << fTimebunchCutTime << " ns" << endl;
    cout << "Maximum time between digis: " << fClusterActiveTime << " ns" << endl;
  }
  if (fRemoveLowEclus)
    cout << "Minimum cluster energy: " << fClusterEnergyCut << " GeV" << endl;
  if (fPosMethod == 0)
    cout << "Using DEFAULT cluster position and radius calculation method (from fRecoPar)" << endl;
  else if (fPosMethod == 1)
    cout << "Using SIMPLIFIED position and radius calculation method for preclusters ONLY, and DEFAULT method for clusters." << endl;
  else if (fPosMethod == 2)
    cout << "Using SIMPLIFIED position and radius calculation method for preclusters AND clusters." << endl;
  else if (fPosMethod == 3)
    cout << "Using SIMPLIFIED position and radius calculation method for preclusters ONLY, and the position of the digi with the highest energy for clusters." << endl;
  else {
    cout << "[ERROR\t] Invalid position method specified. Selecting recommended default instead. " << endl;
    fPosMethod = 1;
  }
  if (fNbMethod == 0)
    cout << "Using DEFAULT precluster radius method. (REAL X,Y CIRCLE)" << endl;
  else if (fNbMethod == 1)
    cout << "Using SIMPLIFIED precluster radius method (CIRCLE)." << endl;
  else if (fNbMethod == 2)
    cout << "Using SIMPLIFIED precluster radius method (RECTANGULAR BOX)." << endl;
  else if (fNbMethod == 3)
    cout << "Using SIMPLIFIED precluster radius method (SQUARE BOX)." << endl;
  else {
    cout << "[ERROR\t] Invalid neighbour method specified. Selecting recommended default instead. " << endl;
    fNbMethod = 0;
  }

  cout << "=> " << fNrOfDigis << " digis in " << fNrOfEvents << " events." << endl;

  fNrOfDigis /= 100; // to save a calculation step for the progress counter
  counter = 0;
  stoptime = 0;

  /*	hNdigis = new TH1I("hNdigis","Number of digis per timebunch",250,0,500);
  //	hRadDiff = new TH1D("hRadDiff","Difference in radius of two methods: simple - precise",100,-50,50);
  //	hPosDiff = new TH1D("hposDiff","Difference in position of two methods: simple - precise",100,-50,50);
    hSingle = new TH1D("hSingle","Energy of single-hit clusters",100,0,1);
    hDistancesToSingleHitClusters = new TH1D("hDistancesToSingleHitClusters","Distances to single-hit clusters",200,0,400);*/

  // ofile.open("diffference.txt");
  // ofileClus.open("DistClusLRsingleHitClus100MeV.txt");

  LOG(info) << " PndEmcDistributedClustering: Intialization successful";
  return kSUCCESS;
}

/**
 * @brief Runs the task.
 *
 * Fetches an array of @ref PndEmcDigi's by reading the current event in the tree for event-based usage,
 * or using @ref TimeGap() to load digis up to a certain time gap for time-based usage.
 * The task then maps  these digis into (virtual) Data Concentrators (sections of the EMC of ~128 crystals)
 * and looks for clusters in these sections. After this, a loop is done over all sections to merge the
 * pre-formed clusters if needed, and build the final PndEmcCluster objects. This approach closely
 * resembles the design of the readout chain of the detector, but does not necessarily deliver the best
 * performance for offline analysis.
 *
 * @param opt unused
 * @return void
 */
void PndEmcDistributedClustering::Exec(Option_t *opt)
{

  /*	if (fVerbose>3){
      fTimer.Start();
    }*/
  fTimer.Start(kFALSE);
  /*	int time = 0; // integer to make estimated time an integer (printing digits doesn't make sense for the estimate)
    double starttime = 0;
    double factor = 0;*/

  // Reset output arrays and get all digis up to a certain time gap specified by fTimebunchCutTime
  if (!fClusterArray)
    Fatal("Exec", "No Cluster Array");
  fClusterArray->Delete();
  fPreclusterArray->Delete();
  if (FairRunAna::Instance()->IsTimeStamp()) {
    fDigiArray->Delete();
    fDigiArray->AbsorbObjects(FairRootManager::Instance()->GetData("EmcDigiSorted", fDigiFunctor, fTimebunchCutTime));
  }

  evtCounter++;

  Int_t nDigis = fDigiArray->GetEntriesFast();
  // hNdigis->Fill(nDigis);

  // -----   Timer for preclustering  -----------------------------------------
  TStopwatch DCtimer;
  DCtimer.Start();

  // --------------------- START SEARCHING FOR PRECLUSTERS ---------------------------

  Int_t dx = 0;
  Int_t dy = 0;
  Int_t nPreclusters = 0;
  Int_t nDigisPassed = 0; // #digis that passed thresholds
  Int_t nDCdigis = 0;
  Int_t deltaD = 1; // neighbour distance to consider
  Int_t nDCs = 0;   // #DC active

  Double_t dt = 0;
  Double_t deltaT = 0;

  if (FairRunAna::Instance()->IsTimeStamp()) {
    dt = 0;
    deltaT = fClusterActiveTime; // threshold for time between digis in a cluster
  }

  if (fVerbose > 2) {
    cout << "\n#digis: " << nDigis << endl;
    cout << "Per DC: ";
  }
  // ofile << "\n// ---> Timebunch " << evtCounter << " (" << nDigis << " digis)" << endl;
  std::vector<Int_t> neighbours;   // array keeping track which digis are neighbours
  std::vector<Int_t> DigiPassed;   // array containing indices of digis that passed the energy cuts
  std::vector<Int_t> XPadPassed;   // array keeping track of the x-coordinate (in pad space) of digis that passed the energy cuts
  std::vector<Int_t> YPadPassed;   // array keeping track of the y-coordinate (in pad space) of digis that passed the energy cuts
  std::vector<Int_t> isAdded;      // array with cluster numbers for each digi
  std::vector<Int_t> similarities; // array keeping track which clusters should be merged
  std::vector<Double_t> TPassed;   // array keeping track of the timestamp of digis that passed the energy cuts
  std::vector<Bool_t> tagArray;    // array to tag which digis to consider

  for (Int_t iDC = 0; iDC < 159; iDC++) { // loop over all virtual Data Concentrators (DCs)

    //----- Tag which digis are in the current virtual DC -----//
    // if (iDC != 103) continue; // for comparison with hardware prototype, only use 1 DC (at random chosen from FwEndcap)
    tagArray.clear();
    nDCdigis = 0;

    for (Int_t i = 0; i < nDigis; i++) {
      PndEmcDigi *myDigi = (PndEmcDigi *)fDigiArray->At(i);
      if (myDigi->GetDCnumber() == iDC) {
        tagArray.push_back(true);
        nDCdigis++;
      } else
        tagArray.push_back(false);
    }

    nDigisPassed = 0;

    if (nDCdigis == 0)
      continue; // skip empty DCs

    nDCs++;

    if (fVerbose > 2)
      cout << nDCdigis << " " << std::flush;

    // reset vectors
    neighbours.clear();
    DigiPassed.clear();
    XPadPassed.clear();
    YPadPassed.clear();
    TPassed.clear();
    isAdded.clear();
    similarities.clear();

    for (Int_t a = 0; a < nDCdigis; a++)
      isAdded.push_back(-1); // initialise all entries of isAdded to -1 (needed by algorithm)

    //----- Loop over all digis to add them to preclusters -----//
    for (Int_t iDigi = 0; iDigi < nDigis; ++iDigi) {

      if (tagArray[iDigi] == false)
        continue; // only process digis that are in the current virtual DC

      if (digiCounter - 1 == nDistProg * fNrOfDigis) {
        nDistProg += 1;
        if (nDistProg < 101) {
          cout << "\r                                                                                                         " << std::flush;
          cout << "\r[INFO\t] PndEmcDistributedClustering: " << nDistProg << "\% completed." << std::flush; //
        }
      }
      digiCounter++;

      /*starttime = fTimer.RealTime();
      fTimer.Continue();
      //if (digiCounter == counter*fNrOfDigis/10) {counter++;}
      if (starttime-stoptime > 1) { // update timer every 1 s
        if (digiCounter) factor=(fNrOfDigis*100-digiCounter)/digiCounter;
        //time = (1000-counter)*starttime/counter;
        time = factor*starttime;
        cout << "\r                                                                                         " << std::flush;
        cout << "\r[INFO\t] PndEmcDistributedClustering: " << nDistProg << "\% completed (about " << time << " seconds remaining)" << std::flush;
        stoptime = starttime;
      }*/

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
      // ofile.precision(11);
      // ofile << theDigi->GetTimeStamp() << " " << theDigi->GetEnergy() << " " << theDigi->GetXPad() << " " << theDigi->GetYPad() << " " << theDigi->GetModule() << endl;
      // ofile << "t:" << theDigi->GetTimeStamp() << "\t";
    }

    if (fVerbose > 4) {
      cout << "Looped over all digis (" << nDigisPassed << " of " << nDCdigis << " digis passed)" << endl;
    }

    if (nDigisPassed > nDCdigis) {
      Fatal("Exec", "Attempt to process more digis than are present in this virtual data concentrator");
    }

    if (nDigisPassed == 0)
      continue; // skip DCs that only have digis with energies below threshold

    // hNdigis->Fill(nDigisPassed); // keep track how many digis there are per TB per DC

    //----- First, construct matrix containing information which digis are neighbouring -----//

    for (Int_t d = 0; d < nDigisPassed - 1; d++) { // check all pairs of digis, so all digis up to the second-to-last one
      Int_t nNeighbours = 0;
      neighbours.push_back(0); // placeholder for nr of neighbours
      for (Int_t e = d + 1; e < nDigisPassed; e++) {
        dx = XPadPassed[e] - XPadPassed[d]; // check euclidian distance between all digis which passed threshold
        dy = YPadPassed[e] - YPadPassed[d]; // this construction ensures all pairs of digis are only checked once
        if (FairRunAna::Instance()->IsTimeStamp())
          dt = TMath::Abs(TPassed[e] - TPassed[d]); // also take into account that non-consecutive digi pairs may differ in time more than dtau ns

        /*				if (dy*dy <= deltaD*deltaD && dx*dx <= deltaD*deltaD && dt <= deltaT) { 	// ONLINE VERSION. If the distance is small enough, the digis are neighbours
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
    }

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

    if (isAdded[nDigisPassed - 1] < 0)
      isAdded[nDigisPassed - 1] = nClusters++;

    // Secondary clustering
    for (Int_t i = 0; i < simLength; i += 2) { // use info from similarities[] to merge preclusters
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
          isAdded[j] = nPreclusters; // info in isAdded: digi "index" belongs to precluster "isAdded[index]"
        }
      }
      if (n > 0)
        nPreclusters++; // precluster nr, don't reset it while searching for preclusters in a timebunch
    }

    // ofileClus << "\n" << nDigisPassed << " ";
    // for (Int_t of=0; of<nDigisPassed; of++) //ofileClus << isAdded[of] << " ";
    // ofileClus << endl;

    //----- Finally, use isAdded to merge digis into clusters -----//

    for (Int_t i = 0; i < nDigisPassed; i++) {
      if (isAdded[i] < 0)
        continue;
      if (isAdded[i] < fPreclusterArray->GetEntriesFast()) { // if isAdded[i] is smaller than the current #clusters, the cluster to which this digi belongs already exists, and we
                                                             // should add it to this cluster
        PndEmcCluster *precluster = (PndEmcCluster *)fPreclusterArray->At(isAdded[i]); // set pointer to the cluster we need, as indicated by isAdded[]
        precluster->addDigi(fDigiArray, DigiPassed[i]);                                // add digi with index as indicated by DigiPassed, so we add the correct one to the cluster
                                                                                       // cout << "Added digi\t" << DigiPassed[i] << "\tto precluster\t" << isAdded[i] << endl;
      } else {                                                                         // if not, make a new cluster for this digi
        PndEmcCluster *newPrecluster = new ((*fPreclusterArray)[fPreclusterArray->GetEntriesFast()]) PndEmcCluster();
        newPrecluster->addDigi(fDigiArray, DigiPassed[i]);
        // cout << "Added digi\t" << DigiPassed[i] << "\tto precluster\t" << isAdded[i] << endl;
      }
    }
  }
  if (fVerbose > 2)
    cout << endl;

  // ----- FINISHED BUILDING PRECLUSTERS -----------------------------------

  if (fVerbose > 3) {
    cout << "Finished digi assignment: " << nPreclusters << " preclusters identified (" << std::flush;
  }
  FinishPreclusters();

  DCtimer.Stop();
  Double_t DCrtime = DCtimer.RealTime();
  Double_t DCctime = DCtimer.CpuTime();
  DCtimer.Reset();

  DCtotRtime += (DCrtime / nDCs); // keep track how much time was spent on preclustering
  DCtotCtime += (DCctime / nDCs);

  // -----   Timer for clustering  -------------------------------------
  TStopwatch CNtimer;
  CNtimer.Start();

  // ----- START MERGING PRECLUSTERS INTO 'REAL' CLUSTERS ------------------

  Int_t clusterNr = 0;
  Int_t nNeighbours = 0; // #neighbouring preclusters

  std::vector<Int_t> neighbours2;   // array keeping track which preclusters are neighbours
  std::vector<Int_t> isAdded2;      // array with cluster numbers for each precluster
  std::vector<Int_t> similarities2; // array keeping track which clusters should be merged

  Int_t nPres = fPreclusterArray->GetEntriesFast();

  for (Int_t a = 0; a < nPres; a++)
    isAdded2.push_back(-1); // initialise all entries of isAdded2 to -1 (needed by algorithm)

  if (fVerbose > 2) {
    cout << "#preclusters: " << nPres << endl;
  }

  // loop over all preclusters to add them to clusters

  for (Int_t iClus = 0; iClus < nPres - 1; ++iClus) {

    nNeighbours = 0; // reset nNeighbours
    neighbours2.push_back(0);

    /* Get a new precluster from the precluster array */
    PndEmcCluster *preclus1 = (PndEmcCluster *)fPreclusterArray->At(iClus);

    for (Int_t j = iClus + 1; j < nPres; j++) {

      PndEmcCluster *preclus2 = (PndEmcCluster *)fPreclusterArray->At(j);

      if (fVerbose > 4) {
        cout << "Distance between preclusters " << iClus << " and " << j << ": " << std::flush;
        cout << preclus1->DistanceToCentre(preclus2->where()) << endl;
      }

      if (FairRunAna::Instance()->IsTimeStamp())
        dt = TMath::Abs(preclus1->GetTimeStamp() - preclus2->GetTimeStamp());
      //																										     ^
      // tag which preclusters should be merged														     |
      //							spatial distance														     time difference    (use the same as for digis)
      //									|																				   |       |
      //									V																	  			   V 	   V
      if (fNbMethod == 0) { //																								(DEFAULT METHOD: REAL POSITION CIRCLE)
        if (preclus1->DistanceToCentre(preclus2->where()) <= (preclus1->GetRadius() + preclus2->GetRadius()) && dt <= deltaT) {
          neighbours2.push_back(j);
          nNeighbours++;
        }
      }

      // !!! ATTENTION !!! HARDWARE METHODS WILL ONLY WORK IF YOU USE THE MAPPED X,Y COORDINATES (XPAD, YPAD) FOR THE PRECLUSTERS !!! //

      else if (fNbMethod == 1) { //																						(HARDWARE METHOD: CIRCLE)
        Double_t dclusx = preclus2->where().x() - preclus1->where().x();
        Double_t dclusy = preclus2->where().y() - preclus1->where().y();
        Double_t rTot = preclus1->GetRadius() + preclus2->GetRadius();
        if ((dclusx * dclusx + dclusy * dclusy) <= (rTot * rTot) && dt <= deltaT) {
          neighbours2.push_back(j);
          nNeighbours++;
        }
      } else if (fNbMethod == 2) { //																						(HARDWARE METHOD: RECTANGULAR BOX)
        if (TMath::Abs(preclus2->where().x() - preclus1->where().x()) <= (preclus1->GetXRadius() + preclus2->GetXRadius()) &&
            TMath::Abs(preclus2->where().y() - preclus1->where().y()) <= (preclus1->GetYRadius() + preclus2->GetYRadius()) && dt <= deltaT) {
          neighbours2.push_back(j);
          nNeighbours++;
        }
      } else if (fNbMethod == 3) { //																						(HARDWARE METHOD: SQUARE BOX)
        if (TMath::Abs(preclus2->where().x() - preclus1->where().x()) <= (preclus1->GetRadius() + preclus2->GetRadius()) &&
            TMath::Abs(preclus2->where().y() - preclus1->where().y()) <= (preclus1->GetRadius() + preclus2->GetRadius()) && dt <= deltaT) {
          neighbours2.push_back(j);
          nNeighbours++;
        }
      }
    }
    neighbours2[neighbours2.size() - (nNeighbours + 1)] = nNeighbours; // write nr of neighbours to the appropiate entry in neighbours2[]
  }

  // Primary Clustering
  Int_t k = 0;
  Int_t l = 0;
  Int_t nClusters = 0; // nr of clusters
  Int_t simLength = 0;
  while (l < neighbours2.size()) {
    if (isAdded2[k] < 0) {                             // if it hasn't been added yet
      isAdded2[k] = nClusters;                         // put cluster nr in isAdded2 array
      for (Int_t j = 1; j < neighbours2[l] + 1; j++) { // for the first neighbouring hit upto #neighbours for this precluster
        Int_t m = neighbours2[l + j];                  // m = hit index
        if (isAdded2[m] < 0)
          isAdded2[m] = nClusters; // if hit hasn't been added, put cluster nr here
        else if (isAdded2[m] != nClusters) {
          if (nClusters > isAdded2[m]) {
            similarities2.push_back(nClusters);   // if it has, put current cluster nr in this array
            similarities2.push_back(isAdded2[m]); // together with its cluster nr (could be different, which is why is being stored here for later comparison)
          } else {
            similarities2.push_back(isAdded2[m]); // different order, so we always have the largest cluster nr first
            similarities2.push_back(nClusters);
          }
          simLength += 2; // increment simLength by 2, as we just wrote 2 elements to similarities
        }
      }
      nClusters++;
    } else { // if isAdded2[k] isn't -1, it means the precluster has been added already and isAdded2[k] contains its cluster nr
      for (Int_t j = 1; j < neighbours2[l] + 1; j++) {
        Int_t m = neighbours2[l + j];
        if (isAdded2[m] < 0)
          isAdded2[m] = isAdded2[k]; // same as before, only use cluster nr found in isAdded2[k]
        else if (isAdded2[m] != isAdded2[k]) {
          if (isAdded2[k] > isAdded2[m]) {
            similarities2.push_back(isAdded2[k]); // similarities2[] keeps tracks of which clusters have to be merged
            similarities2.push_back(isAdded2[m]);
          } else {
            similarities2.push_back(isAdded2[m]); // different order, so we always have the largest cluster nr first
            similarities2.push_back(isAdded2[k]);
          }
          simLength += 2;
        }
      }
    }
    k++;
    l += neighbours2[l] + 1;
  }

  if (nPres != 0)
    if (isAdded2[nPres - 1] < 0)
      isAdded2[nPres - 1] = nClusters++;

  // Secondary clustering
  for (Int_t i = 0; i < simLength; i += 2) { // use info from similarities2[] to merge preclusters
    if (similarities2[i] != similarities2[i + 1]) {
      for (Int_t m = 0; m < nPres; m++) {
        if (isAdded2[m] == similarities2[i])
          isAdded2[m] = similarities2[i + 1];
      }
      for (Int_t j = i + 2; j < simLength; j++)
        if (similarities2[j] == similarities2[i])
          similarities2[j] = similarities2[i + 1];
    }
  }
  for (Int_t i = 0; i < nClusters; i++) {
    Int_t n = 0;
    for (Int_t j = 0; j < nPres; j++) {
      if (isAdded2[j] == i) {
        n++;
        isAdded2[j] = clusterNr; // info in isAdded2: precluster "index" belongs to cluster "isAdded2[index]"
      }
    }
    if (n > 0)
      clusterNr++;
  }

  if (fVerbose > 2) {
    cout << "Finished precluster assignment: " << clusterNr << " clusters identified\n" << endl;
  }

  //--- Finally, use isAdded to merge preclusters into clusters ---//

  for (Int_t i = 0; i < nPres; i++) {
    PndEmcCluster *thisPrecluster = (PndEmcCluster *)fPreclusterArray->At(i);
    if (isAdded2[i] <
        fClusterArray
          ->GetEntriesFast()) { // if isAdded2[i] is smaller than the current #clusters, the cluster to which this precluster belongs already exists, and we should add it to that
      PndEmcCluster *cluster = (PndEmcCluster *)fClusterArray->At(isAdded2[i]); // set pointer to the cluster we need, as indicated by isAdded2[]
      cluster->addCluster(thisPrecluster, fDigiArray);                          // add precluster i to the current cluster
    } else {                                                                    // if not, make a new cluster
      PndEmcCluster *newCluster = new ((*fClusterArray)[fClusterArray->GetEntriesFast()]) PndEmcCluster();
      newCluster->addCluster(thisPrecluster, fDigiArray);
    }
  }

  // ----- FINISHED MERGING PRECLUSTERS ------------------------------------

  // tagSingleClusArray.clear();

  CNtimer.Stop();
  Double_t CNrtime = CNtimer.RealTime();
  Double_t CNctime = CNtimer.CpuTime();
  CNtimer.Reset();

  CNtotRtime += CNrtime; // keep track how much time was spent on clustering
  CNtotCtime += CNctime;

  FinishClusters();

  if (tagSingleClus)
    MapDistancesToSingleHitClusters();
  // cout << " >>>--- Timebunch " << evtCounter << " ---<<<" << endl;
  if (fRemoveLowEclus)
    RemoveLowEnergyClusters(); // !! significantly slows down cluster finding task, but will speed up future processing and reconstruction tasks
  // if (evtCounter > 1) abort();
}

/**
 * @brief Precluster loop
 *
 * Loops over the pre-formed PndEmcCluster objects to assign its properties.
 *
 * @return void
 */
void PndEmcDistributedClustering::FinishPreclusters()
{
  // cout << endl;
  Int_t nCluster = fPreclusterArray->GetEntriesFast();
  if (fVerbose > 3) {
    cout << nCluster << ")" << endl;
  }
  for (Int_t i = 0; i < nCluster; i++) {
    // cout << "Precluster: " << i << endl;
    // ofileClus << i << " ";
    FinishPrecluster((PndEmcCluster *)(fPreclusterArray->At(i)));
  }
  // if (nCluster) //ofileClus << endl;
}

/**
 * @brief Assign precluster properties
 *
 * Assigns precluster properties, such as energy, position, radius, and time.
 *
 * @return void
 */
void PndEmcDistributedClustering::FinishPrecluster(PndEmcCluster *precluster)
{

  PndEmcClusterProperties clustProperties(*precluster, fDigiArray);
  TVector3 tmpprecpos;

  if (fPosMethod == 0) { // use built-in method to get estimate for position
    tmpprecpos = clustProperties.Where(fRecoPar->GetEmcClusterPosMethod(), fClusterPosParam);
    precluster->SetPosition(tmpprecpos);
  }

  Double_t xmax = -50; // smaller then smallest x, so it will always write a correct value
  Double_t xmin = 200;
  Double_t ymax = -50;
  Double_t ymin = 100;

  std::vector<Int_t> list = precluster->DigiList();

  Double_t Etot = 0;
  Double_t Emax = 0;
  Double_t maxDist = 0;
  Int_t Emax_idx = 0;

  for (Int_t iDigi = 0; iDigi < list.size(); ++iDigi) {
    Int_t idx = list[iDigi];
    // cout << "digiList[" << iDigi << "] = " << idx << endl;
    PndEmcDigi *thedigi = (PndEmcDigi *)fDigiArray->At(idx);
    // cout << thedigi->where().x() << ", " << thedigi->where().y() << ", " << thedigi->GetTimeStamp() << ", " << thedigi->GetEnergy() << endl;
    Etot += thedigi->GetEnergy();
    if (thedigi->GetEnergy() > Emax) {
      Emax = thedigi->GetEnergy();
      Emax_idx = idx;
    }
    if (fPosMethod == 0) {                                   // use built-in method to get estimate for size
      if (precluster->DistanceToCentre(thedigi) > maxDist) { // get cluster size (rough estimate)
        maxDist = precluster->DistanceToCentre(thedigi);
      }
    } else if (fPosMethod == 1 || fPosMethod == 2) { // use simplified method to get estimate for size and position
      /*if (thedigi->where().x() > xmax) xmax = thedigi->where().x();
      if (thedigi->where().x() < xmin) xmin = thedigi->where().x();
      if (thedigi->where().y() > ymax) ymax = thedigi->where().y();
      if (thedigi->where().y() < ymin) ymin = thedigi->where().y();*/
      if (thedigi->GetXPad() > xmax)
        xmax = thedigi->GetXPad();
      if (thedigi->GetXPad() < xmin)
        xmin = thedigi->GetXPad();
      if (thedigi->GetYPad() > ymax)
        ymax = thedigi->GetYPad();
      if (thedigi->GetYPad() < ymin)
        ymin = thedigi->GetYPad();
    }
  }
  if (fPosMethod == 1 || fPosMethod == 2) {
    Double_t yradius = ymax - ymin;
    Double_t xradius = xmax - xmin;
    yradius > xradius ? maxDist = 0.5 *yradius : maxDist = 0.5 * xradius;
    Double_t zpos = static_cast<PndEmcDigi *>(fDigiArray->UncheckedAt(Emax_idx))->where().z();
    tmpprecpos.SetXYZ(xmax - 0.5 * xradius, ymax - 0.5 * yradius, zpos);
    precluster->SetPosition(tmpprecpos);
    precluster->SetXRadius(0.5 * xradius);
    precluster->SetYRadius(0.5 * yradius);
  }

  precluster->SetRadius(maxDist); // new function added to PndEmcCluster (correction for crystal size already in function)
  precluster->SetEnergy(Etot);

  precluster->SetTimeStamp(static_cast<PndEmcDigi *>(fDigiArray->UncheckedAt(Emax_idx))->GetTimeStamp());

  // ofileClus << Etot << " " << precluster->GetTimeStamp() << " " << maxDist << " ";

  if (fVerbose > 3)
    cout << "Assigned precluster properties: radius = " << precluster->GetRadius() << ", energy = " << Etot << endl;
}

/**
 * @brief Cluster loop
 *
 * Loops over the final PndEmcCluster objects to assign its properties.
 *
 * @return void
 */
void PndEmcDistributedClustering::FinishClusters()
{
  // cout << endl;
  Int_t nCluster = fClusterArray->GetEntriesFast();
  for (Int_t i = 0; i < nCluster; i++) {
    // cout << "Cluster: " << i << endl;
    FinishCluster((PndEmcCluster *)(fClusterArray->At(i)), i);
  }
}

/**
 * @brief Assign cluster properties
 *
 * Assigns final cluster properties, such as energy, position, Zernike moments, and time.
 *
 * @return void
 */
void PndEmcDistributedClustering::FinishCluster(PndEmcCluster *cluster, int nr)
{

  //----set energy and time etc. to the clusters

  std::vector<Int_t> list = cluster->DigiList();

  Double_t total_energy = 0;
  Double_t max_energy = 0;
  Int_t max_energy_idx = 0;

  TVector3 tmpbumppos;

  for (Int_t iDigi = 0; iDigi < list.size(); ++iDigi) {
    Int_t idx = list[iDigi];
    PndEmcDigi *thedigi = (PndEmcDigi *)fDigiArray->UncheckedAt(idx);
    // cout << thedigi->where().x() << ", " << thedigi->where().y() << ", " << thedigi->GetTimeStamp() << ", " << thedigi->GetEnergy() << endl;
    total_energy += thedigi->GetEnergy();
    if (thedigi->GetEnergy() > max_energy) {
      max_energy = thedigi->GetEnergy();
      max_energy_idx = idx;
    }
  }

  cluster->SetEnergy(total_energy);
  PndEmcClusterProperties clustProperties(*cluster, fDigiArray);
  /*if (list.size() == 1) {
    hSingle->Fill(cluster->GetEnergy());
    tagSingleClus = true;
    tagSingleClusArray.push_back(nr);
  //	if (cluster->GetEnergy() > 0.1)
  printClusters = true;
  }*/
  if (fPosMethod == 2) { // use position of digi with highest energy
    Double_t clusx = static_cast<PndEmcDigi *>(fDigiArray->UncheckedAt(max_energy_idx))->where().x();
    Double_t clusy = static_cast<PndEmcDigi *>(fDigiArray->UncheckedAt(max_energy_idx))->where().y();
    Double_t clusz = static_cast<PndEmcDigi *>(fDigiArray->UncheckedAt(max_energy_idx))->where().z();
    tmpbumppos.SetXYZ(clusx, clusy, clusz);
    cluster->SetPosition(tmpbumppos);
  } else if (fPosMethod == 0 || fPosMethod == 1) { // use default position method from fRecoPar
    tmpbumppos = clustProperties.Where(fRecoPar->GetEmcClusterPosMethod(), fClusterPosParam);
    cluster->SetPosition(tmpbumppos);
  }
  // cout << "clusE=" << total_energy << ", clusX=" << tmpbumppos.X() << ", clusY=" << tmpbumppos.Y() << endl;
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
void PndEmcDistributedClustering::RemoveLowEnergyClusters()
{

  Int_t nClusters = fClusterArray->GetEntriesFast();
  //	Int_t fRemovedClusters = 0;
  // if (printClusters) ofileClus << endl;
  for (Int_t i = 0; i < nClusters; i++) {
    PndEmcCluster *myCluster = (PndEmcCluster *)fClusterArray->At(i);
    /*		if (printClusters) {
          ofileClus << "Cluster " << i << "\tnHits=" << myCluster->NumberOfDigis() << "\tE=" << myCluster->GetEnergy() << "\tt=" << myCluster->GetTimeStamp() << "\tx=" <<
       myCluster->where().X() << "\ty=" << myCluster->where().Y() << endl; std::vector<Int_t> list = myCluster->DigiList();

          for(Int_t iDigi=0; iDigi<list.size(); ++iDigi) {
            Int_t idx = list[iDigi];
            PndEmcDigi* thedigi = (PndEmcDigi*) fDigiArray->UncheckedAt(idx);
            ofileClus << "\tevt=" << thedigi->GetEventNr() << "\tE=" << thedigi->GetEnergy() << "\tt=" << thedigi->GetTimeStamp() << "\tx=" << thedigi->where().X() << "\ty=" <<
       thedigi->where().Y() << " (XPad=" << thedigi->GetXPad() << ", YPad=" << thedigi->GetYPad() << ")" << endl;
          }
        }*/
    if (myCluster->GetEnergy() < fClusterEnergyCut) {
      fClusterArray->RemoveAt(i);
      fRemovedClusters++;
    }
  }
  fClusterArray->Compress();
  //	printClusters = false;
}

void PndEmcDistributedClustering::MapDistancesToSingleHitClusters()
{

  Int_t nClusters = fClusterArray->GetEntriesFast();

  /*	for (Int_t j=0; j<tagSingleClusArray.size(); j++) {
      PndEmcCluster *tagCluster=(PndEmcCluster*)fClusterArray->At(tagSingleClusArray[j]);

      for (Int_t i=0; i<nClusters; i++) {
        if (i == tagSingleClusArray[j]) continue;
        PndEmcCluster *myCluster=(PndEmcCluster*)fClusterArray->At(i);
        hDistancesToSingleHitClusters->Fill(tagCluster->DistanceToCentre(myCluster->where()));
      }
    }*/

  tagSingleClus = false;
}

/**
 * @brief Finish the task
 *
 * Displays the time needed to perform the sub-tasks, and some other statistics.
 *
 * @return void
 */
void PndEmcDistributedClustering::FinishTask()
{

  fTimer.Stop();
  cout << "\nProcessed in total " << digiCounter << " digis within " << evtCounter << " timebunches." << endl;
  cout << "[INFO\t] Preclustering (per DC): real time: " << DCtotRtime << " s, cpu time: " << DCtotCtime << " s." << endl;
  cout << "[INFO\t] Clustering (CN): real time: " << CNtotRtime << " s, cpu time: " << CNtotCtime << " s." << endl;

  cout << "Removed " << fRemovedClusters << "  low-energetic clusters (E < " << fClusterEnergyCut << " GeV)." << endl;

  // store used value of fTimebunchCutTime
  TVector3 *cuttingTime = new TVector3(fTimebunchCutTime, fClusterActiveTime, fNrOfEvents);
  FairRootManager *ioman = FairRootManager::Instance();
  auto sink = ioman->GetSink();
  assert(sink->GetSinkType() == Sink_Type::kFILESINK);
  auto rootFileSink = static_cast<FairRootFileSink *>(sink);
  TTree *tOut = rootFileSink->GetOutTree();
  tOut->GetUserInfo()->Add(cuttingTime);

  /*	TCanvas *cN = new TCanvas("cN","Multiplicity",1);
    cN->cd(1);
    //hDistancesToSingleHitClusters->Draw();
    hNdigis->Draw();

    TCanvas *cE = new TCanvas("cE","Energy",1);
    cE->cd(1);
    hSingle->Draw();*/

  // ofile.close();
  // ofileClus.close();
}

/**
 * @brief Set parameter containers
 *
 * Instruct the parent task where to find (some of) the parameters that are used.
 *
 * @return void
 */
void PndEmcDistributedClustering::SetParContainers()
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
void PndEmcDistributedClustering::SetStorageOfData(Bool_t val)
{
  fStoreClusters = val;
  return;
}

ClassImp(PndEmcDistributedClustering)
