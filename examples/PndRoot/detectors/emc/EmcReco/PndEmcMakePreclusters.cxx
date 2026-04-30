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
// 	+ Mapping of digis into virtual DC (more closely resembles real readout chain)
//
// Environment:
//	Software developed for the PANDA Detector at GSI.
//

#include "PndEmcMakePreclusters.h"

#include "PndEmcClusterProperties.h"
#include "PndEmcXClMoments.h"
#include "PndEmcStructure.h"
#include "PndEmcMapper.h"
#include "PndEmcGeoPar.h"
#include "PndEmcDigiPar.h"
#include "PndEmcRecoPar.h"
#include "PndEmcCluster.h"
#include "PndEmcPrecluster.h"
#include "PndEmcDigi.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"
#include "FairRootFileSink.h"

#include "TClonesArray.h"
#include "TROOT.h"
#include "TLeaf.h"

#include <iostream>
#include <vector>

using std::cout;
using std::endl;

PndEmcMakePreclusters::PndEmcMakePreclusters(Int_t verbose, Bool_t storeclusters)
  : FairTask("EmcClusteringTask", verbose), fDigiArray(nullptr), fPreclusterArray(nullptr), fDigiFunctor(nullptr), fDigiEnergyTresholdBarrel(0.), fDigiEnergyTresholdFWD(0.),
    fDigiEnergyTresholdBWD(0.), fDigiEnergyTresholdShashlyk(0.), fTimebunchCutTime(0.), fGeoPar(new PndEmcGeoPar()), fRecoPar(new PndEmcRecoPar()), fStoreClusters(storeclusters),
    fStoreClusterBase(kTRUE), fClusterPosParam(), fClusterActiveTime(5.), fPosMethod(1), evtCounter(0), digiCounter(0), nTotDigisPassed(0), fNrOfDigis(0), fNrOfEvents(0),
    nPrecProg(0), DCtotRtime(0), DCtotCtime(0), fAutoDetermine(kFALSE)
{
  fClusterPosParam.clear();
}

//--------------
// Destructor --
//--------------

PndEmcMakePreclusters::~PndEmcMakePreclusters() {}

// -----   Public method Init   -------------------------------
InitStatus PndEmcMakePreclusters::Init()
{

  LOG(info) << " PndEmcMakePreclusters::Init(): Start Initialization";

  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    LOG(error) << " PndEmcMakePreclusters::Init: "
               << "RootManager not instantiated!";
    return kFATAL;
  }

  // Get nr of events	and digis (for progress counter)
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
      SetClusterActiveTime(dtau_cut + 50);
    }
  }

  // Get input array
  if (FairRunAna::Instance()->IsTimeStamp())
    fDigiArray = (TClonesArray *)ioman->GetObject("EmcDigiSorted"); // for timebased, use sorted digis
  else
    fDigiArray = (TClonesArray *)ioman->GetObject("EmcDigi"); // for eventbased, use "normal" digis
  if (!fDigiArray) {
    LOG(warn) << " PndEmcMakePreclusters::Init: "
              << "No PndEmcDigi array!";
    return kERROR;
  }

  // Create and register output array
  fPreclusterArray = ioman->Register("EmcPrecluster", "PndEmcPrecluster", "Emc", kTRUE);

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
  if (fPosMethod == 0)
    cout << "Using DEFAULT precluster position and radius calculation method (from fRecoPar)" << endl;
  else if (fPosMethod == 1)
    cout << "Using SIMPLIFIED position and radius calculation method (REAL x,y position)." << endl;
  else if (fPosMethod == 2)
    cout << "Using SIMPLIFIED position and radius calculation method (MAPPED x,y position)." << endl;
  else {
    cout << "[ERROR\t] Invalid position method specified. Selecting recommended default instead. " << endl;
    fPosMethod = 1;
  }

  cout << "=> " << fNrOfDigis << " digis in " << fNrOfEvents << " events." << endl;

  fNrOfDigis /= 100; // to save a calculation step for the progress counter

  cout << "\nIMPORTANT: " << endl;
  cout << " -> Did you remember to change PndEmcClusterRingSorter.cxx to use PndEmcPrecluster instead of PndEmcCluster objects?" << endl;
  cout << "    Also, don't forget to compile again after making these changes ;)" << endl;
  cout << " -> Make sure that you set PndEmcPrecluster to use the digi position method you want (using PndEmcMakePreclusters->SetDigiPositionMethod())." << endl;
  cout << "    You're using the ";
  if (fDigiPosType == 0)
    cout << "MAPPED XPAD,YPAD position.\n" << endl;
  else if (fDigiPosType == 1)
    cout << "REAL x,y position.\n" << endl;
  else {
    cout << "\n-E- PndEmcMakePreclusters::Init: "
         << "Unknown digi position method/digi position method not set!" << endl;
    return kFATAL;
  }

  // hNdigis = new TH1I("hNdigis","Number of digis per DC per timebunch",100,0,100);

  LOG(info) << " PndEmcMakePreclusters: Intialization successful";
  return kSUCCESS;
}

void PndEmcMakePreclusters::Exec(Option_t *opt)
{

  /*	if (fVerbose>3){
      fTimer.Start();
    }*/

  // Reset output arrays and get all digis up to a certain time gap specified by fTimebunchCutTime
  if (!fPreclusterArray)
    Fatal("Exec", "No Cluster Array");
  fPreclusterArray->Delete();
  if (FairRunAna::Instance()->IsTimeStamp()) {
    fDigiArray->Delete();
    fDigiArray->AbsorbObjects(FairRootManager::Instance()->GetData("EmcDigiSorted", fDigiFunctor, fTimebunchCutTime));
  }

  evtCounter++;

  Int_t nDigis = fDigiArray->GetEntriesFast();

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

  /*if(FairRunAna::Instance()->IsTimeStamp()) {
    dt = 0;
    deltaT = fClusterActiveTime; // threshold for time between digis in a cluster
  }*/

  if (fVerbose > 2) {
    cout << "#digis: " << nDigis << endl;
    cout << "Per DC: ";
  }

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

      if (digiCounter - 1 == nPrecProg * fNrOfDigis) {
        cout << "\r[INFO\t] PndEmcMakePreclusters: " << nPrecProg << "\% completed." << std::flush;
        nPrecProg += 1;
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
      if (theDigi->GetXPad() < 0 && theDigi->GetModule() == 3)
        XPadPassed.push_back(theDigi->GetXPad() + 1); // correct for gaps in FwEndcap
      else
        XPadPassed.push_back(theDigi->GetXPad());
      if (theDigi->GetYPad() < 0 && theDigi->GetModule() == 3)
        YPadPassed.push_back(theDigi->GetYPad() + 1);
      else
        YPadPassed.push_back(theDigi->GetYPad());
      nDigisPassed++;
      // ofile.precision(11);
      // ofile << theDigi->GetTimeStamp() << " " << theDigi->GetEnergy() << " " << theDigi->GetXPad() << " " << theDigi->GetYPad() << endl;
    }

    nTotDigisPassed += nDigisPassed;

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
        // if(FairRunAna::Instance()->IsTimeStamp()) dt = TMath::Abs(TPassed[e] - TPassed[d]);	// also take into account that non-consecutive digi pairs may differ in time more
        // than dtau ns

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

    //----- Finally, use isAdded to merge digis into clusters -----//

    for (Int_t i = 0; i < nDigisPassed; i++) {
      if (isAdded[i] < 0)
        continue;
      if (isAdded[i] < fPreclusterArray->GetEntriesFast()) { // if isAdded[i] is smaller than the current #clusters, the cluster to which this digi belongs already exists, and we
                                                             // should add it to this cluster
        PndEmcPrecluster *precluster = (PndEmcPrecluster *)fPreclusterArray->At(isAdded[i]); // set pointer to the cluster we need, as indicated by isAdded[]
        precluster->SetDigiPositionType(fDigiPosType);
        precluster->addDigi(fDigiArray, DigiPassed[i]); // add digi with index as indicated by DigiPassed, so we add the correct one to the cluster
                                                        // cout << "Added digi\t" << DigiPassed[i] << "\tto precluster\t" << isAdded[i] << endl;
      } else {                                          // if not, make a new cluster for this digi
        PndEmcPrecluster *newPrecluster = new ((*fPreclusterArray)[fPreclusterArray->GetEntriesFast()]) PndEmcPrecluster();
        newPrecluster->SetDigiPositionType(fDigiPosType);
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
}

void PndEmcMakePreclusters::FinishPreclusters()
{
  // cout << endl;
  Int_t nCluster = fPreclusterArray->GetEntriesFast();
  if (fVerbose > 3) {
    cout << nCluster << ")" << endl;
  }
  for (Int_t i = 0; i < nCluster; i++) {
    // cout << "Precluster: " << i << endl;
    // ofileClus << i << " ";
    FinishPrecluster((PndEmcPrecluster *)(fPreclusterArray->At(i)));
  }
  // if (nCluster) //ofileClus << endl;
}

void PndEmcMakePreclusters::FinishPrecluster(PndEmcPrecluster *precluster)
{

  PndEmcClusterProperties clustProperties(*precluster, fDigiArray);
  TVector3 tmpprecpos;

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
    } else if (fPosMethod == 1) { // use simplified method to get estimate for size and position
      if (thedigi->where().x() > xmax)
        xmax = thedigi->where().x();
      if (thedigi->where().x() < xmin)
        xmin = thedigi->where().x();
      if (thedigi->where().y() > ymax)
        ymax = thedigi->where().y();
      if (thedigi->where().y() < ymin)
        ymin = thedigi->where().y();
    } else if (fPosMethod == 2) { // use simplified method with mapping to get estimate for size and position
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

  if (fPosMethod == 0) { // use built-in method to get estimate for position
    tmpprecpos = clustProperties.Where(fRecoPar->GetEmcClusterPosMethod(), fClusterPosParam);
    precluster->SetPosition(tmpprecpos);
  } else if (fPosMethod == 1) {
    Double_t yradius = 0.5 * (ymax - ymin);
    Double_t xradius = 0.5 * (xmax - xmin);
    precluster->SetXRadius(yradius);
    precluster->SetYRadius(xradius);
    yradius > xradius ? maxDist = yradius : maxDist = xradius;
    Double_t zpos = static_cast<PndEmcDigi *>(fDigiArray->UncheckedAt(Emax_idx))->where().z();
    tmpprecpos.SetXYZ(xmax - xradius, ymax - yradius, zpos);
    precluster->SetPosition(tmpprecpos);
  } else if (fPosMethod == 2) { // (to have the radius an int, make everything twice as big)
    Double_t yradius = ymax - ymin;
    Double_t xradius = xmax - xmin;
    precluster->SetXRadius(yradius, 2);
    precluster->SetYRadius(xradius, 2);
    yradius > xradius ? maxDist = yradius : maxDist = xradius;
    Double_t zpos = static_cast<PndEmcDigi *>(fDigiArray->UncheckedAt(Emax_idx))->where().z();
    tmpprecpos.SetXYZ(2 * xmax + 2 * xmin, 2 * ymax + 2 * ymin, zpos);
    precluster->SetPosition(tmpprecpos);
  }

  precluster->SetRadius(maxDist, fPosMethod); // new function added to PndEmcCluster (correction for crystal size already in function): arguments: Double_t calculated radius, Int_t
                                              // multiplier for addition of single-cluster radius (default value: 1, set to 2 to use diameter [automatically set by using fPosMethod
                                              // as input, with multiplier set to 1 if fPosMethod=0], set to higher values to manually increase radius)
  precluster->SetEnergy(Etot);

  precluster->SetTimeStamp(static_cast<PndEmcDigi *>(fDigiArray->UncheckedAt(Emax_idx))->GetTimeStamp());
  precluster->SetModule(static_cast<PndEmcDigi *>(fDigiArray->UncheckedAt(Emax_idx))->GetModule());

  // ofileClus << Etot << " " << precluster->GetTimeStamp() << " " << maxDist << " ";

  if (fVerbose > 3)
    cout << "Assigned precluster properties: radius = " << precluster->GetRadius() << " (maxDist=" << maxDist << "), energy = " << Etot << endl; // check that radius!=maxDist

  // -------------- stop (end of precluster finding) -------------------------------------------------------
}

void PndEmcMakePreclusters::FinishTask()
{

  cout << "\nProcessed in total " << digiCounter << " digis within " << evtCounter << " timebunches (discarded " << digiCounter - nTotDigisPassed
       << " digis because of their energy)." << endl;
  cout << "[INFO\t] Preclustering (per DC): real time: " << DCtotRtime << " s, cpu time: " << DCtotCtime << " s." << endl;

  // store used value of fTimebunchCutTime
  TVector3 *cuttingTime = new TVector3(fTimebunchCutTime, fClusterActiveTime, fNrOfEvents);
  FairRootManager *ioman = FairRootManager::Instance();
  auto sink = ioman->GetSink();
  assert(sink->GetSinkType() == Sink_Type::kFILESINK);
  auto rootFileSink = static_cast<FairRootFileSink *>(sink);
  TTree *tOut = rootFileSink->GetOutTree();
  tOut->GetUserInfo()->Add(cuttingTime);
}

void PndEmcMakePreclusters::SetParContainers()
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

void PndEmcMakePreclusters::SetStorageOfData(Bool_t val)
{
  fStoreClusters = val;
  return;
}

ClassImp(PndEmcMakePreclusters)
