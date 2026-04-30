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

#include "PndEmcMergePreclusters.h"

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
#include "TLine.h"
#include "TCanvas.h"
#include "TH1.h"

#include <iostream>
#include <vector>

using std::cout;
using std::endl;

PndEmcMergePreclusters::PndEmcMergePreclusters(Int_t verbose, Bool_t storeclusters)
  : FairTask("EmcClusteringTask", verbose), fDigiArray(nullptr), fPreclusterArray(nullptr), fClusterArray(nullptr), fClusterFunctor(nullptr), fTimebunchCutTime(0.),
    fGeoPar(new PndEmcGeoPar()), fRecoPar(new PndEmcRecoPar()), fStoreClusters(storeclusters), fStoreClusterBase(kTRUE), fClusterPosParam(), fClusterActiveTime(5.),
    fClusterEnergyCut(0.030), fRemoveLowEclus(kTRUE), fPosMethod(0), fNbMethod(0), evtCounter(0), precCounter(0), fNrOfPres(0), nMrgProg(0), nTotClusters(0), fRemovedClusters(0),
    nTotDigis(0), wrongConnection(0), CNtotRtime(0), CNtotCtime(0), fAutoTime(kTRUE)
{
  fClusterPosParam.clear();
}

//--------------
// Destructor --
//--------------

PndEmcMergePreclusters::~PndEmcMergePreclusters() {}

// -----   Public method Init   -------------------------------
InitStatus PndEmcMergePreclusters::Init()
{

  LOG(info) << " PndEmcMergePreclusters::Init(): Start Initialization";

  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    LOG(error) << " PndEmcMergePreclusters::Init: "
               << "RootManager not instantiated!";
    return kFATAL;
  }

  // Get nr of preclusters ( divide by 100 for progress counter)
  TTree *tIn = ioman->GetInTree();
  fNrOfPres = tIn->Draw("EmcPrecluster.fEnergy>>hist", "", "goff");
  cout << "(" << fNrOfPres << " preclusters)" << endl;
  fNrOfPres /= 100;
  Int_t fNrOfEvents = tIn->GetEntriesFast();

  // Get input array
  fPreclusterArray = (TClonesArray *)ioman->GetObject("EmcPreclusterSorted"); // get sorted preclusters
  if (!fPreclusterArray) {
    LOG(warn) << " PndEmcMergePreclusters::Init: "
              << "No precluster array!";
    return kERROR;
  }
  fDigiArray = (TClonesArray *)ioman->GetObject("EmcDigiSorted"); // for timebased, use sorted digis
                                                                  //	else fDigiArray = (TClonesArray*) ioman->GetObject("EmcDigi"); // for eventbased, use "normal" digis
  if (!fDigiArray) {
    LOG(warn) << " PndEmcMergePreclusters::Init: "
              << "No PndEmcDigi array!";
    return kERROR;
  }

  // Create and register output array
  fClusterArray = ioman->Register("EmcClusterTemp", "PndEmcCluster", "Emc", kTRUE);

  // searching for time gaps in digi stream
  fClusterFunctor = new TimeGap();

  fGeoPar->InitEmcMapper();
  PndEmcStructure::Instance();

  TVector3 *cuttingTime = (TVector3 *)(ioman->GetInTree())->GetUserInfo()->First();
  if (cuttingTime && fAutoTime) {
    fTimebunchCutTime = cuttingTime->X();
    fClusterActiveTime = cuttingTime->Y();
    cout << "\n-I- PndEmcMergePreclusters::Init: Reading time from file." << endl;
  }

  // Automatically determine time cuts

  if (!fAutoTime && !fTimebunchCutTime) {
    cout << endl;
    std::vector<Double_t> tVector;
    TH1D *hPreDt = new TH1D("hPreDt", "Time difference between preclusters", 1000, 0, 1000);
    int nProg = 0;
    for (int i = 0; i < fNrOfEvents; i++) {
      // get Digis of this event
      TClonesArray *PreArray = new TClonesArray("PndEmcPrecluster");
      tIn->SetBranchAddress("EmcPrecluster", &PreArray);
      tIn->GetEntry(i);
      int len = PreArray->GetEntriesFast();
      for (int j = 0; j < len; j++) {
        PndEmcPrecluster *myPre = (PndEmcPrecluster *)PreArray->At(j);
        tVector.push_back(myPre->GetTimeStamp());
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
      hPreDt->Fill(dt);
      if (k == nProg * tVector.size() / 100 - 1) {
        cout << "\r[INFO\t] Automatically determining time cuts: " << nProg << "\% completed." << std::flush;
        nProg += 5;
      }
    }
    cout << endl;

    // get original nr of events simulated
    fNrOfEvents = cuttingTime->Z();

    Int_t integral = hPreDt->GetBinContent(hPreDt->GetNbinsX() + 1); // start by including overflow bin
    Int_t dtau_cut = fTimebunchCutTime;
    for (int iDtau = hPreDt->GetNbinsX(); iDtau >= 0; iDtau--) {
      integral += hPreDt->GetBinContent(iDtau);
      dtau_cut = iDtau;
      if (integral > 0.98 * fNrOfEvents)
        break; // assume some small inherent efficiency loss
    }

    if (!dtau_cut) {
      cout << "[WARNING] Automatic time cut setting failed, using default values!" << endl;
      fAutoTime = kTRUE;
      fTimebunchCutTime = fRecoPar->GetClusterActiveTime() * 1.0e9;
    }
    if (!fAutoTime) {
      SetTimebunchCutTime(dtau_cut);
      SetPreclusterTimeThreshold(dtau_cut + 25);
    }
  }

  if (!strcmp(fRecoPar->GetEmcClusterPosMethod(), "lilo")) {
    cout << "Lilo cluster position method" << endl;
    fClusterPosParam.push_back(fRecoPar->GetOffsetParmA());
    fClusterPosParam.push_back(fRecoPar->GetOffsetParmB());
    fClusterPosParam.push_back(fRecoPar->GetOffsetParmC());
  }

  if (FairRunAna::Instance()->IsTimeStamp()) {
    cout << "Minimum Time Between Timebunches: " << fTimebunchCutTime << " ns" << endl;
    cout << "Maximum Time Between Preclusters: " << fClusterActiveTime << " ns" << endl;
  }
  if (fRemoveLowEclus)
    cout << "Minimum cluster energy: " << fClusterEnergyCut << " GeV" << endl;
  if (fPosMethod == 0)
    cout << "Using LOGARITHMIC ENERGY WEIGHING for the cluster position calculation" << endl;
  else if (fPosMethod == 1)
    cout << "Using LINEAR ENERGY WEIGHING for the cluster position calculation" << endl;
  else if (fPosMethod == 2)
    cout << "Using SIMPLE LINEAR WEIGHING for the cluster position calculation" << endl;
  else if (fPosMethod == 3)
    cout << "Using the REAL x,y position of digi with highest energy for the cluster position." << endl;
  else {
    cout << "[ERROR\t] Invalid position method specified. Selecting recommended default instead. " << endl;
    fPosMethod = 0;
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

  cout << "\nIMPORTANT: Did you remember to change PndEmcClusterRingSorter.cxx back to use PndEmcCluster instead of PndEmcPrecluster objects?" << endl;
  cout << "           Also, don't forget to compile again after making these changes ;)\n" << endl;

  /*hDx = new TH1D("hDx","Difference in x between log E and lin E weighted pos",100,-50,50);
  hDy = new TH1D("hDy","Difference in y between log E and lin E weighted pos",100,-50,50);
  hDz = new TH1D("hDz","Difference in z between log E and lin E weighted pos",100,-50,50);*/
  // hDz = new TH1D("hDz","Precluster multiplicity per timebunch (MRG)",1000,0,1000);
  // hTimeDifference = new TH1D("hTimeDifference","Precluster time distribution within a timebunch (MRG)",200,0,400);

  LOG(info) << " PndEmcMergePreclusters: Intialization successful";
  return kSUCCESS;
}

void PndEmcMergePreclusters::Exec(Option_t *opt)
{

  /*	if (fVerbose>3){
      fTimer.Start();
    }*/

  // Reset output arrays and get all preclusters up to a certain time gap specified by fTimebunchCutTime
  if (!fClusterArray)
    Fatal("Exec", "No Cluster Array");
  fClusterArray->Delete();
  fDigiArray->Clear();
  if (FairRunAna::Instance()->IsTimeStamp()) {
    fPreclusterArray->Delete();
    fPreclusterArray->AbsorbObjects(FairRootManager::Instance()->GetData("EmcPreclusterSorted", fClusterFunctor, fTimebunchCutTime));
  }

  // -----   Timer for clustering  -------------------------------------
  TStopwatch CNtimer;
  CNtimer.Start();

  evtCounter++;

  // ----- START MERGING PRECLUSTERS INTO 'REAL' CLUSTERS ------------------

  Int_t clusterNr = 0;
  Int_t nNeighbours = 0; // #neighbouring preclusters
  Double_t dt = 0;
  Double_t deltaT = 0;
  Int_t digiListStart = 0;
  Int_t digiListEnd = 0;
  Int_t ind = 0;

  /*if(FairRunAna::Instance()->IsTimeStamp()) {
    dt = 0;
    deltaT = fClusterActiveTime; // threshold for time between digis in a cluster
  }*/

  std::vector<Int_t> neighbours2;   // array keeping track which preclusters are neighbours
  std::vector<Int_t> isAdded2;      // array with cluster numbers for each precluster
  std::vector<Int_t> similarities2; // array keeping track which clusters should be merged

  Int_t nPres = fPreclusterArray->GetEntriesFast();
  // Double_t startTime = 0;
  // hDz->Fill(nPres);

  for (Int_t a = 0; a < nPres; a++)
    isAdded2.push_back(-1); // initialise all entries of isAdded2 to -1 (needed by algorithm)

  if (fVerbose > 2) {
    cout << "#preclusters: " << nPres << endl;
  }

  // loop over all preclusters to add them to clusters

  for (Int_t iClus = 0; iClus < nPres; ++iClus) {

    if (precCounter - 1 == nMrgProg * fNrOfPres) {
      cout << "\r[INFO\t] PndEmcMergePreclusters: " << nMrgProg << "\% completed." << std::flush;
      nMrgProg += 1;
    }
    precCounter++;

    /* Get a new precluster from the precluster array */
    PndEmcPrecluster *preclus1 = (PndEmcPrecluster *)fPreclusterArray->At(iClus);

    // if (iClus==0) startTime=preclus1->GetTimeStamp();
    // hTimeDifference->Fill(preclus1->GetTimeStamp()-startTime);

    /* and add its corresponding digis to the digi array */
    /*		FairMultiLinkedData digiLinks = preclus1->GetLinksWithType(FairRootManager::Instance()->GetBranchId("EmcDigiSorted"));
    cout << "Found " << digiLinks.GetNLinks() << " digis for precluster " << iClus << " (precluster is supposed to have: " << preclus1->NumberOfDigis() << ")" << endl;
    if (preclus1->NumberOfDigis() == 1) cout << "Single-digi precluster E= " << preclus1->GetEnergy() << endl;
        digiListStart = digiListEnd;
        for (Int_t iDigi=0; iDigi<digiLinks.GetNLinks(); iDigi++){
    // printing content inside link loop
    cout << "> " << fDigiArray->GetEntriesFast() << " entries in TClonesArray. ";
    if (fDigiArray->GetEntriesFast()>0) {
    //	cout << "Last one: E=";
    //	PndEmcDigi* theDigi = (PndEmcDigi*)fDigiArray->At(fDigiArray->GetEntriesFast()-1);
    //	cout << theDigi->GetEnergy();
    cout << "INSIDE LOOP" << endl;
      for (int tc=0; tc<fDigiArray->GetEntriesFast(); tc++) {
        cout << "\tEntry " << tc << " E = ";
        PndEmcDigi* theDigi = (PndEmcDigi*)fDigiArray->At(tc);
        cout << theDigi->GetEnergy();
      }
    }
    cout << endl;
    //
          PndEmcDigi* myDigi = (PndEmcDigi*)FairRootManager::Instance()->GetCloneOfLinkData(digiLinks.GetLink(iDigi));
          if(myDigi) {
    //				PndEmcDigi* newDigi = new((*fDigiArray)[fDigiArray->GetEntriesFast()]) PndEmcDigi(*myDigi);
            PndEmcDigi* newDigi = new((*fDigiArray)[ind++]) PndEmcDigi(*myDigi);
    //newDigi->Print();
    PndEmcDigi* theDigi = (PndEmcDigi*)fDigiArray->At(ind-1);
    theDigi->Print();
            delete(myDigi);
            digiListEnd++;

          } else {
            std::cout << "-E in PndEmcMergePreclusters::Exec() FairLink " << digiLinks.GetLink(iDigi) << " to EmcDigi delivers null" << std::endl;
          }
    //		TLine* newLine = new((*fDigiArray)[fDigiArray->GetEntriesFast()]) TLine(1+iClus,2+iClus,3+iClus,4+iClus);
    //		}
    cout << "fDigiArray has " << fDigiArray->GetEntriesFast() << " entries." << endl;
        }

    // !!! VERY IMPORTANT: UPDATE LIST WITH DIGI INDICES ACCORDING TO NEW DIGIARRAY !!! Otherwise, future calls to member functions of PndEmcCluster will try to access the wrong
    digis // preclus1->OverwriteDigiList(digiListStart,digiListEnd);

    //printing content outside link loop
    if (digiLinks.GetNLinks() != 0) {
      cout << "OUTSIDE LOOP" << endl;
      for (int tc=0; tc<fDigiArray->GetEntriesFast(); tc++) {
        cout << "\tEntry " << tc << " ";
        PndEmcDigi* theDigi = (PndEmcDigi*)fDigiArray->At(tc);
    theDigi->Print();
    //		TLine* theLine = (TLine*)fDigiArray->At(tc);
    //		theLine->Print();
      }cout << "\n " << endl;
    }
    */
    if (iClus == nPres - 1)
      continue; // don't check the last one (not needed anyway, but it is included in the loop so that its digis are loaded in the new digi array)

    nNeighbours = 0;          // reset nNeighbours
    neighbours2.push_back(0); // placeholder for nr of neighbours

    for (Int_t j = iClus + 1; j < nPres; j++) {

      PndEmcPrecluster *preclus2 = (PndEmcPrecluster *)fPreclusterArray->At(j);

      if (fVerbose > 4) {
        cout << "Distance between preclusters " << iClus << " and " << j << ": " << std::flush;
        cout << preclus1->DistanceToCentre(preclus2->where()) << endl;
      }

      // if(FairRunAna::Instance()->IsTimeStamp()) dt = TMath::Abs(preclus1->GetTimeStamp()-preclus2->GetTimeStamp());
      //																										     ^
      // tag which preclusters should be merged														     |
      //							spatial distance														     time difference    (use the same as for digis)
      //									|																				   |       |
      //									V																	  			   V 	   V
      if (fNbMethod == 0) { //																								(DEFAULT METHOD: REAL POSITION CIRCLE (most accurate)
        if (preclus1->DistanceToCentre(preclus2->where()) <= (preclus1->GetRadius() + preclus2->GetRadius()) && dt <= deltaT) {
          neighbours2.push_back(j);
          nNeighbours++;
        }
      } else if (fNbMethod == 1) { //																						(HARDWARE METHOD: CIRCLE)
        Double_t dx = preclus2->where().x() - preclus1->where().x();
        Double_t dy = preclus2->where().y() - preclus1->where().y();
        Double_t rTot = preclus1->GetRadius() + preclus2->GetRadius();
        if ((dx * dx + dy * dy) <= (rTot * rTot) && dt <= deltaT) {
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
    cout << "Finished precluster assignment: " << clusterNr << " clusters identified:" << endl;
    cout << "isAdded2 = [" << std::flush;
    for (int ia = 0; ia < isAdded2.size(); ia++)
      cout << isAdded2[ia] << " ";
    cout << "]\n" << endl;
  }

  //--- Finally, use isAdded to merge preclusters into clusters ---//

  /*	for (Int_t i=0; i<nPres; i++) {
      PndEmcPrecluster* thisPrecluster = (PndEmcPrecluster*) fPreclusterArray->At(i);
      if (isAdded2[i] < fClusterArray->GetEntriesFast() ) { // if isAdded2[i] is smaller than the current #clusters, the cluster to which this precluster belongs already exists,
    and we should add it to that PndEmcCluster* cluster= (PndEmcCluster*) fClusterArray->At(isAdded2[i]); // set pointer to the cluster we need, as indicated by isAdded2[]
        cluster->addCluster(thisPrecluster,fDigiArray); // add precluster i to the current cluster
      }
      else { // if not, make a new cluster
        PndEmcCluster* newCluster = new((*fClusterArray)[fClusterArray->GetEntriesFast()]) PndEmcCluster();
        newCluster->addCluster(thisPrecluster,fDigiArray);
      }
    }*/

  TVector3 posVector;
  Double_t energyWeighFactor;
  Double_t energyWeightedTime;
  Double_t energyWeightedXPos;
  Double_t energyWeightedYPos;
  Double_t energyWeightedZPos;
  Double_t maxEn = 0;
  Double_t clusEn = 0;
  Double_t En = 0;
  Double_t W0 = 0;
  Double_t logWeight = 0;
  Double_t flightTime = 0;
  Int_t maxEn_idx = 0;
  Short_t clusModule;
  Short_t sumModule;
  Int_t nPrec = 0;
  std::vector<Double_t> currentDigisT;
  std::vector<Double_t> currentDigisE;
  std::vector<Double_t> currentDigisX;
  std::vector<Double_t> currentDigisY;
  std::vector<Double_t> currentDigisZ;
  std::vector<Double_t> memberDigiTimes;
  std::vector<Double_t> memberDigiEnergies;
  std::vector<Double_t> memberDigiXpos;
  std::vector<Double_t> memberDigiYpos;
  std::vector<Double_t> memberDigiZpos;

  for (Int_t iClus = 0; iClus < clusterNr; iClus++) {
    PndEmcCluster *newCluster = new ((*fClusterArray)[iClus]) PndEmcCluster(); // make a new empty cluster
    posVector.Clear();
    memberDigiTimes.clear();
    memberDigiEnergies.clear();
    memberDigiXpos.clear();
    memberDigiYpos.clear();
    memberDigiZpos.clear();
    currentDigisT.clear();
    currentDigisE.clear();
    currentDigisX.clear();
    currentDigisY.clear();
    currentDigisZ.clear();
    clusEn = 0;
    sumModule = 0;
    nPrec = 0;
    for (Int_t i = 0; i < nPres; i++) { // get all preclusters which should be added to the current cluster
      if (isAdded2[i] == iClus) {       // using info from isAdded2[]
        PndEmcPrecluster *thisPrecluster = (PndEmcPrecluster *)fPreclusterArray->At(i);

        currentDigisT = thisPrecluster->GetMemberDigiTimes();
        currentDigisE = thisPrecluster->GetMemberDigiEnergies();
        currentDigisX = thisPrecluster->GetMemberDigiXpos();
        currentDigisY = thisPrecluster->GetMemberDigiYpos();
        currentDigisZ = thisPrecluster->GetMemberDigiZpos();

        memberDigiTimes.insert(memberDigiTimes.end(), currentDigisT.begin(), currentDigisT.end());
        memberDigiEnergies.insert(memberDigiEnergies.end(), currentDigisE.begin(), currentDigisE.end());
        memberDigiXpos.insert(memberDigiXpos.end(), currentDigisX.begin(), currentDigisX.end());
        memberDigiYpos.insert(memberDigiYpos.end(), currentDigisY.begin(), currentDigisY.end());
        memberDigiZpos.insert(memberDigiZpos.end(), currentDigisZ.begin(), currentDigisZ.end());

        clusModule = thisPrecluster->GetModule();
        clusEn += thisPrecluster->GetEnergy();
        sumModule += clusModule;
        nPrec++;
      }
    }
    // cout << "Tsize: " << memberDigiTimes.size() << " Esize: " << memberDigiEnergies.size() << " Xsize: " << memberDigiXpos.size() << " Ysize: " << memberDigiYpos.size() << "
    // Zsize: " << memberDigiZpos.size() << endl;
    if (nPrec)
      if (sumModule / nPrec != clusModule && ((clusModule != 1 || clusModule != 2) && (sumModule / nPrec != 1 || sumModule / nPrec != 2)))
        wrongConnection++;
    En = 0;
    maxEn = 0;
    maxEn_idx = 0;
    if (fPosMethod == 0)
      W0 = 4.071 - 0.678 * TMath::Power(clusEn, -0.534) * TMath::Exp(-TMath::Power(clusEn, 1.171));
    energyWeighFactor = 0;
    energyWeightedTime = 0;
    energyWeightedXPos = 0;
    energyWeightedYPos = 0;
    energyWeightedZPos = 0;
    nTotClusters++;
    // cout << "\ncluster " << iClus << ", energy: " << clusEn << endl;
    for (Int_t j = 0; j < memberDigiTimes.size(); j++) {
      En = memberDigiEnergies[j];
      nTotDigis++;
      if (fPosMethod == 0) { // use logarithmic energy weighing
        logWeight = W0 + TMath::Log(En / clusEn);
        if (logWeight < 0)
          continue; // discard contributions from low-energy hits
        energyWeighFactor += logWeight;
        energyWeightedTime += logWeight * memberDigiTimes[j];
        energyWeightedXPos += logWeight * memberDigiXpos[j];
        energyWeightedYPos += logWeight * memberDigiYpos[j];
        energyWeightedZPos += logWeight * memberDigiZpos[j];
        // cout << "    digi " << j << ": E=" << En << ", x=" << memberDigiXpos[j] << ", y=" << memberDigiYpos[j] << ", logWeight=" << logWeight << endl;
      } else if (fPosMethod == 1) { // use linear energy weighing
        energyWeighFactor += En;
        energyWeightedTime += En * memberDigiTimes[j];
        energyWeightedXPos += En * memberDigiXpos[j];
        energyWeightedYPos += En * memberDigiYpos[j];
        energyWeightedZPos += En * memberDigiZpos[j];
      } else if (fPosMethod == 2) { // use linear weighing
        energyWeighFactor++;
        energyWeightedTime += memberDigiTimes[j];
        energyWeightedXPos += memberDigiXpos[j];
        energyWeightedYPos += memberDigiYpos[j];
        energyWeightedZPos += memberDigiZpos[j];
      }
      if (En > maxEn) {
        maxEn_idx = j;
        maxEn = En;
      }
    }
    newCluster->SetEnergy(clusEn);
    if (fPosMethod == 3)
      posVector.SetXYZ(memberDigiXpos[maxEn_idx], memberDigiYpos[maxEn_idx], memberDigiZpos[maxEn_idx]); // use position of digi with highest energy
    else {
      if (energyWeighFactor)
        posVector.SetXYZ(energyWeightedXPos / energyWeighFactor, energyWeightedYPos / energyWeighFactor, energyWeightedZPos / energyWeighFactor); // use weighted position
      else
        posVector.SetXYZ(0, 0, 0);
    }
    newCluster->SetPosition(posVector);
    flightTime = posVector.Mag() / 29.9792458;                         // flight time in ns = distance to IP (in cm) / c
                                                                       //		newCluster->SetTimeStamp(energyWeightedTime/energyWeighFactor-flightTime); // use energy-weighted time
    newCluster->SetTimeStamp(memberDigiTimes[maxEn_idx] - flightTime); // use time of most energetic hit
    newCluster->SetModule(clusModule);
    // if (evtCounter > 1) abort();
  }

  // ----- FINISHED MERGING PRECLUSTERS ------------------------------------

  // FinishClusters();

  Double_t CNrtime = CNtimer.RealTime();
  Double_t CNctime = CNtimer.CpuTime();

  CNtotRtime += CNrtime; // keep track how much time was spent on clustering
  CNtotCtime += CNctime;
  // cout << CNtotRtime << "s, (added " << CNrtime << " s) - processed " << clusterNr << " clusters" << endl;
  if (fRemoveLowEclus)
    RemoveLowEnergyClusters(); // !! slows down cluster finding task, but will speed up future processing and reconstruction tasks
}

void PndEmcMergePreclusters::FinishClusters()
{
  // cout << endl;
  Int_t nCluster = fClusterArray->GetEntriesFast();
  for (Int_t i = 0; i < nCluster; i++) {
    FinishCluster((PndEmcCluster *)(fClusterArray->At(i)));
    // cout << "Cluster: " << i << endl;
  }
}

void PndEmcMergePreclusters::FinishCluster(PndEmcCluster *cluster)
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

  if (fPosMethod == 3) { // use position of digi with highest energy
    Double_t clusx = static_cast<PndEmcDigi *>(fDigiArray->UncheckedAt(max_energy_idx))->where().x();
    Double_t clusy = static_cast<PndEmcDigi *>(fDigiArray->UncheckedAt(max_energy_idx))->where().y();
    Double_t clusz = static_cast<PndEmcDigi *>(fDigiArray->UncheckedAt(max_energy_idx))->where().z();
    tmpbumppos.SetXYZ(clusx, clusy, clusz);
    cluster->SetPosition(tmpbumppos);
  } else if (fPosMethod == 0) { // use default position method from fRecoPar
    tmpbumppos = clustProperties.Where(fRecoPar->GetEmcClusterPosMethod(), fClusterPosParam);
    cluster->SetPosition(tmpbumppos);
  }

  Double_t flightTime = tmpbumppos.Mag() / 29.9792458; // flight time in ns = distance to IP / c

  cluster->SetTimeStamp(static_cast<PndEmcDigi *>(fDigiArray->UncheckedAt(max_energy_idx))->GetTimeStamp() - flightTime); // correct cluster time for photon flight time

  PndEmcXClMoments xClMoments(*cluster, fDigiArray);
  cluster->SetZ20(xClMoments.AbsZernikeMoment(2, 0, 15));
  cluster->SetZ53(xClMoments.AbsZernikeMoment(5, 3, 15));
  cluster->SetLatMom(xClMoments.Lat());

  cluster->SetModule(static_cast<PndEmcDigi *>(fDigiArray->UncheckedAt(max_energy_idx))->GetModule());

  //-------------------stop-------------------------------------------------------------
}

void PndEmcMergePreclusters::RemoveLowEnergyClusters()
{

  Int_t nClusters = fClusterArray->GetEntriesFast();
  //	Int_t fRemovedClusters = 0;

  for (Int_t i = 0; i < nClusters; i++) {
    PndEmcCluster *myCluster = (PndEmcCluster *)fClusterArray->At(i);
    if (myCluster->GetEnergy() < fClusterEnergyCut) {
      fClusterArray->RemoveAt(i);
      fRemovedClusters++;
      nTotClusters--;
    }
  }
  fClusterArray->Compress();
}

void PndEmcMergePreclusters::FinishTask()
{

  cout << "\n[INFO\t] Clustering (CN): real time: " << CNtotRtime << " s, cpu time: " << CNtotCtime << " s." << endl;
  if (wrongConnection)
    cout << "\nClusters from different modules were combined " << wrongConnection << " times." << endl;
  cout << "[INFO\t] Created " << nTotClusters << " clusters with " << nTotDigis << " digis in " << evtCounter << " timebunches (removed " << fRemovedClusters << " low-E clusters)."
       << endl;

  // store used value of fTimebunchCutTime
  TVector3 *cuttingTime = new TVector3(fTimebunchCutTime, 0, 0);
  FairRootManager *ioman = FairRootManager::Instance();
  auto sink = ioman->GetSink();
  assert(sink->GetSinkType() == Sink_Type::kFILESINK);
  auto rootFileSink = static_cast<FairRootFileSink *>(sink);
  TTree *tOut = rootFileSink->GetOutTree();
  tOut->GetUserInfo()->Add(cuttingTime);

  /*int nPreclusters = 0;
  for (int k=0; k<hDz->GetXaxis()->GetXmax(); k++)
    nPreclusters+=k*(hDz->GetBinContent(k));

  cout << "[INFO\t] Number of preclusters: " << nPreclusters << endl;

  TCanvas *cN = new TCanvas("cN","Position difference",1);
  //cN->Divide(1,3);
  cN->cd(1);
  hDx->Draw();
  cN->cd(2);
  hDy->Draw();
  cN->cd(3);
  hDz->Draw();

  TCanvas *cT = new TCanvas("cT","Time",1);
  cT->cd(1);
  hTimeDifference->Draw("");*/
}

void PndEmcMergePreclusters::SetParContainers()
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

void PndEmcMergePreclusters::SetStorageOfData(Bool_t val)
{
  fStoreClusters = val;
  return;
}

ClassImp(PndEmcMergePreclusters)
