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
// File and Version Information:
// 	$Id:$
//
// Description:
//	Class PndEmcMakeCorr
//      Do an energy and theta corrections
//      (at the moment for photons, 4.02.2010)
//
// Author List:
//      A. Biegun
//      M. Babai
//------------------------------------------------------------------------
//
//-----------------------
// This Class's Header --
//-----------------------
#include "PndEmcMakeCorr.h"
#include "PndEmcCorrection.h"

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "PndEmcStructure.h"
#include "PndEmcDataTypes.h"

#include "PndEmcMapper.h"
#include "PndEmcDigiPar.h"
#include "PndEmcRecoPar.h"
#include "PndEmcDigi.h"
#include "PndEmcCluster.h"
#include "PndEmcBump.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

//#include "TClonesArray.h"
//#include "TObject.h"
//#include "TH2.h"
//#include "TVector3.h"

#include <algorithm>
#include <iostream>

using std::cout;
using std::endl;

//----------------
// Constructors --
//----------------
PndEmcMakeCorr::PndEmcMakeCorr(Int_t, TString transportModel, TString clusterType)
  : f(new TFile()), f0(new TFile()), f1(new TFile()), f2(new TFile()), f3(new TFile()), fClusterIndex(-1), fClusterArray(nullptr), fClusterArrayCorr(nullptr),
    fDigiPar(new PndEmcDigiPar()), fRecoPar(new PndEmcRecoPar()), fVerbose(0), fStoreClustersCorr(kTRUE), fModel(transportModel),
    fClusterType(clusterType) // verbose //[R.K.03/2017] unused variable(s)
{
  cout << "PndEmcMakeCorr  constructor: " << fClusterType << endl;
}

//--------------
// Destructor --
//--------------
PndEmcMakeCorr::~PndEmcMakeCorr()
{
  delete fClusterArray;
  delete fClusterArrayCorr;
  delete fDigiPar;
  delete fRecoPar;

  f->Close();
  f0->Close();
  f1->Close();
  f2->Close();
  f3->Close();
  delete f;
  delete f0;
  delete f1;
  delete f2;
  delete f3;
}

// -----   Public method Intialize   ---------------------------------------
InitStatus PndEmcMakeCorr::Init()
{

  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    cout << "-E- PndEmcMakeCorr::Init: "
         << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  // Get input array - Clusters
  fClusterArray = dynamic_cast<TClonesArray *>(ioman->GetObject(fClusterType));
  cout << "" << endl;
  cout << "-------------> fClusterType is: *****  " << fClusterType << "  *****" << endl;
  if (!fClusterArray) {
    cout << "-W- PndEmcMakeCorr::Init: "
         << "No " << fClusterType << " array!" << endl;
    return kERROR;
  }

  // Create and register output array
  fClusterArrayCorr = new TClonesArray("PndEmcCorrection");

  TString corName;
  corName = fClusterType + "Corr";
  ioman->Register(corName, "Emc", fClusterArrayCorr, GetPersistency());

  // Read 2-dim histograms with shifts: GetMean()
  // of reconstructed clusters compared to MC clusters
  cout << "Used transportModel is " << fModel << endl; //" & Particle " <<fPartId<<endl;

  TString work = getenv("VMCWORKDIR");
  TString work1[4];
  work += "/macro/params/";
  cout << "directory is::   " << work << endl;

  for (Int_t i = 0; i < 4; i++) {
    fPartName[0] = "gamma";
    fPartName[1] = "electron";
    fPartName[2] = "pion";
    fPartName[3] = "other";

    corrFileName[i] = fPartName[i] + "_en_th_corr_" + fModel + ".root";
    work1[i] += work + corrFileName[i];

    // cout<<"   "<<endl;
    // cout<<"name of "<< i <<" particle is: "<<fPartName[i]<<endl;
    // cout<<" & path is: "<<work1[i]<<endl;
  }
  cout << "   " << endl;
  cout << "== PLEASE CHECK if the correction map exists for an appropriate PARTICLE & TRANSPORT MODEL!!! " << endl;
  cout << "============================================================================================= " << endl;
  cout << "" << endl;

  for (int i = 0; i < 4; i++) {

    char buffer[10];
    int myInteger = i;
    sprintf(buffer, "%i", myInteger);

    f += myInteger;
    f = new TFile(work1[i], "READ");
    cout << "File " << f->GetName() << " is read" << endl;

    // *** GetMean() from: E_cluster/E_MC & Theta_MC-Theta_Cluster
    //
    // Target EMC
    nameEn[i] = "hisEnergyDelta";
    nameTh[i] = "hisThetaDiff";
    hEn[i] = (TH2F *)f->Get(nameEn[i]);
    hTh[i] = (TH2F *)f->Get(nameTh[i]);

    // Shashlyk
    nameEn5[i] = "hisEnergy5Delta";
    nameTh5[i] = "hisTheta5Diff";
    hEn5[i] = (TH2F *)f->Get(nameEn5[i]);
    hTh5[i] = (TH2F *)f->Get(nameTh5[i]);

    // cout<<"hists: Target EMC "<<hEn[i]->GetName()<<"\t"<<hTh[i]->GetName()<<endl;
    // cout<<"hists: Shashlyk   "<<hEn5[i]->GetName()<<"\t"<<hTh5[i]->GetName()<<endl;
  }

  return kSUCCESS;
}

//-------------
// Methods   --
//-------------
Int_t PndEmcMakeCorr::FindTheBin(TH2 *lookup_table, Float_t value_x, Float_t value_y, Int_t &bin_x, Int_t &bin_y)
{
  bin_x = lookup_table->GetXaxis()->FindBin(value_x);
  bin_y = lookup_table->GetYaxis()->FindBin(value_y);

  if ((bin_x < 1) || (bin_x > lookup_table->GetXaxis()->GetNbins())) {
    bin_x = -1;
    bin_y = -1;
    return -1;
  }

  if ((bin_y < 1) || (bin_y > lookup_table->GetYaxis()->GetNbins())) {
    bin_x = -1;
    bin_y = -1;
    return -2;
  }

  return 0; // Success
}

Double_t PndEmcMakeCorr::GetValueInZ(TH2 *lookup_table, Float_t value_x, Float_t value_y, Bool_t use_interpolation)
{
  // We own the EmcLocMaxInfo objects.  Delete from last time.
  // Clean-up res, We need an empty set to store the results.

  if (use_interpolation) {
    // cout<<"use_interpolation = kTRUE "<<endl;
    //
    // Use the interpolarion routine of ROOT:
    // Interpolate approximates the value via bilinear
    // interpolation based on the four nearest bin centers
    // see Wikipedia, Bilinear Interpolation
    // Andy Mastbaum 10/8/2008
    // vaguely based on R.Raja 6-Sep-2008
    //
    // cout<<"value_x = "<< value_x <<", value_y = "<<value_y <<endl;

    return (lookup_table->Interpolate(value_x, value_y));
  } else {
    // cout<<"use_interpolation = kFALSE "<<endl;
    Int_t binx, biny, retval;

    retval = FindTheBin(lookup_table, value_x, value_y, binx, biny);
    if (retval) {
      cout << "<E> Error in FindTheBin, check your table and input values!!!!: " << retval << endl;
      return 0;
    }

    return (lookup_table->GetBinContent(binx, biny));
  }
  return 0;
}

void PndEmcMakeCorr::Exec(Option_t *)
{
  // Reset output array(s)
  if (!fClusterArrayCorr)
    Fatal("Exec", "No Corrected Cluster Array");
  fClusterArrayCorr->Delete();

  // Variables for Energy & Theta Cluster's Corrections
  Bool_t use_interpolation = kTRUE;
  Double_t valzEn[4], valzTh[4], ThCorr[4], EnCorr[4]; // ThCorrRad[4], //[R.K.03/2017] unused variable
  // Int_t ndigi; //[R.K.03/2017] unused variable
  // Int_t particle[5]; //[R.K. 01/2017] unused variable?
  Int_t chosenModule = 0;

  // Loop over Clusters to make the energy and theta correction
  // by dividing and adding the GetMean() values from lookup table
  Int_t clustLength = fClusterArray->GetEntriesFast();
  // cout <<"clustLength " <<clustLength<<endl;

  for (Int_t iCluster = 0; iCluster < clustLength; iCluster++) {

    PndEmcCluster *theCluster = NULL;

    if (fClusterType.Contains("EmcCluster")) {

      // PndEmcCluster* theCluster;
      theCluster = (PndEmcCluster *)fClusterArray->At(iCluster);
      // cout<<"Cluster was taken from ---> "<<fClusterType<<endl;
      // cout<<"theCluster ---> "<<theCluster->GetEnergy()<<endl;

    } else if (fClusterType.Contains("EmcBump")) {

      // PndEmcBump* theCluster;
      theCluster = (PndEmcBump *)fClusterArray->At(iCluster);
      // cout<<"Cluster was taken from ---> "<<fClusterType<<endl;
      // cout<<"Bump ---> "<<theCluster->GetEnergy()<<endl;

    } else {
      cout << "None of the Cluster object is taken !!!" << endl;
    }

    // Check the ID of a crystal, get module number from it and put into a map
    std::map<Int_t, Int_t> digiMap = theCluster->MemberDigiMap();
    std::map<Int_t, Int_t>::iterator iter;
    // ndigi=digiMap.size(); //[R.K.03/2017] unused variable

    Int_t ID, module;
    std::map<int, int> counting;
    std::map<int, int>::iterator iCounting;
    Int_t oldCounting = 0;

    if (digiMap.size() != 0) {
      for (iter = digiMap.begin(); iter != digiMap.end(); ++iter) {
        ID = iter->first;
        module = ID / 100000000;
        counting[module]++;
      }
    }

    // Get an EMC module in which most of the digits are
    for (iCounting = counting.begin(); iCounting != counting.end(); iCounting++) {
      if ((*iCounting).second > oldCounting) {
        chosenModule = (*iCounting).first;
        oldCounting = (*iCounting).second;
      }
    }

    Double_t energy = theCluster->GetEnergy();
    TVector3 position = theCluster->where();
    Double_t theta = position.Theta() * (180. / TMath::Pi());

    // cout << "  "<<endl;
    // cout << "$$$$$ iCluster "<<iCluster<<endl;

    // Different types of particles: 1=gamma, 2=electron, 3=pion, 4=other
    for (Int_t i = 0; i < 4; i++) {
      if (chosenModule == 5) { // separate map for Shaslyk

        valzEn[i] = GetValueInZ(hEn5[i], energy, theta, use_interpolation);
        valzTh[i] = GetValueInZ(hTh5[i], energy, theta, use_interpolation);

        ThCorr[i] = (valzTh[i] + theta);
        // ThCorrRad[i] = ThCorr[i]*(TMath::Pi()/180.); // DegToRad //[R.K.03/2017] unused variable

        EnCorr[i] = energy / valzEn[i];

        /*cout << "*** EMC Module: "<< chosenModule
       <<", Theta= "<<theta <<", valzTh= "<<valzTh[i] <<", ThCorr="<<ThCorr[i]
       <<", Energy= "<<energy <<", valzEn= "<<valzEn[i] <<", EnCorr="<<EnCorr[i]<<endl;
        */
      } else { // separate map for Target EMC
        if (theta < 6.)
          continue; // Avoid the edges for FwEndCap and Shashlyk

        valzTh[i] = GetValueInZ(hTh[i], energy, theta, use_interpolation);
        valzEn[i] = GetValueInZ(hEn[i], energy, theta, use_interpolation);

        ThCorr[i] = (valzTh[i] + theta);
        // ThCorrRad[i] = ThCorr[i]*(TMath::Pi()/180.); //[R.K.03/2017] unused variable

        EnCorr[i] = energy / valzEn[i];

        /*cout << "*** EMC Module: "<< chosenModule
       <<", Theta= "<<theta <<", valzTh= "<<valzTh[i] <<", ThCorr="<<ThCorr[i]
       <<", Energy= "<<energy <<", valzEn= "<<valzEn[i] <<", EnCorr="<<EnCorr[i] <<endl;
        */
      }
    } // End of corrections for 4 different particles

    new ((*fClusterArrayCorr)[iCluster]) PndEmcCorrection(chosenModule, EnCorr[0], EnCorr[1], EnCorr[2], EnCorr[3], ThCorr[0], ThCorr[1], ThCorr[2], ThCorr[3], valzEn[0],
                                                          valzEn[1], valzEn[2], valzEn[3], valzTh[0], valzTh[1], valzTh[2], valzTh[3]);

  } // End of the loop over Clusters
}

void PndEmcMakeCorr::SetParContainers()
{

  // Get run and runtime database
  FairRun *run = FairRun::Instance();
  if (!run)
    LOG(fatal) << "SetParContainers: No analysis run";

  FairRuntimeDb *db = run->GetRuntimeDb();
  if (!db)
    LOG(fatal) << "SetParContainers: No runtime database";

  // Get Emc digitisation parameter container
  fDigiPar = (PndEmcDigiPar *)db->getContainer("PndEmcDigiPar");

  // Get Emc reconstruction parameter container
  fRecoPar = (PndEmcRecoPar *)db->getContainer("PndEmcRecoPar");
}

void PndEmcMakeCorr::SetStorageOfData(Bool_t val)
{
  fStoreClustersCorr = val;
  return;
}

ClassImp(PndEmcMakeCorr)
