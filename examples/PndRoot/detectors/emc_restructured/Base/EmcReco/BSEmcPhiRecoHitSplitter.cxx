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
//	Class BSEmcPhiRecoHitSplitter
//      Implementation of PhiRecoHitSplitter which splits clusters based on
//      local maxima in the Phi Direction for use with Bremstrahlung correction
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Adapted for the PANDA experiment at GSI
//
// Author List:
//      Phil Strother
//
// Copyright Information:
//	Copyright (C) 1997               Imperial College
//
// Modified:
// Binsong Ma, Ermias Atomssa
//------------------------------------------------------------------------

// Path of file:
// ----- $pandaroot/emc/EmcReco

//-----------------------
// This Class's Header --
//-----------------------
#include "BSEmcPhiRecoHitSplitter.h"

#include <algorithm>
//#include <bits/exception.h>
#include <math.h>
#include <stddef.h>
#include <string>

#include "TMath.h"
#include "TString.h"
#include "TVector3.h"

#include "fairlogger/Logger.h"

//-------------------------------
// Collaborating Class Headers --
//-------------------------------

#include "FairLink.h"
#include "FairParSet.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"

#include "PndConstContainerI.h"
#include "PndMutableContainerI.h"
#include "PndPersistencyTask.h"

#include "BSEmcCluster.h"
#include "BSEmcCrystalPositionPar.h"
#include "BSEmcDataBranchNames.h"
#include "BSEmcDetectorID.h"
#include "BSEmcDigi.h"
#include "BSEmcRecoHit.h"

//----------------
// Constructors --
//----------------

BSEmcPhiRecoHitSplitter::BSEmcPhiRecoHitSplitter(Int_t t_verbose) : PndPersistencyTask("EmcPhiRecoHitSplitter", t_verbose)
{
  SetPersistency(kTRUE);
}

//--------------
// Destructor --
//--------------

BSEmcPhiRecoHitSplitter::~BSEmcPhiRecoHitSplitter()
{
  delete fBarrelDigiArray;
  delete fFwEndcapDigiArray;
  delete fBwEndcapDigiArray;
  delete fShashlikDigiArray;
  delete fBarrelClusterArray;
  delete fFwEndcapClusterArray;
  delete fBwEndcapClusterArray;
  delete fShashlikClusterArray;
}

/**
 * @brief Init Task
 *
 * @return InitStatus
 * @retval kSUCCESS success
 */
InitStatus BSEmcPhiRecoHitSplitter::Init()
{

  LOG(debug) << "BSEmcPhiRecoHitSplitter::Init()";
  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (ioman == nullptr) {
    LOG(error) << "BSEmcPhiRecoHitSplitter<ParSet>::Init: "
               << "RootManager not instantiated!";
    return kFATAL;
  }
  // Get input array

  fBarrelDigiArray->SetBranchName(BSEmcDataBranchNames::fgDigiBranchName + "Barrel");
  fBarrelDigiArray->SetTCA(dynamic_cast<TClonesArray *>(ioman->GetObject(fBarrelDigiArray->GetBranchName().Data())));

  fBarrelClusterArray->SetBranchName(BSEmcDataBranchNames::fgClusterBranchName + "Barrel");
  fBarrelClusterArray->SetTCA(dynamic_cast<TClonesArray *>(ioman->GetObject(fBarrelClusterArray->GetBranchName().Data())));

  fFwEndcapDigiArray->SetBranchName(BSEmcDataBranchNames::fgDigiBranchName + "FwEndcap");
  fFwEndcapDigiArray->SetTCA(dynamic_cast<TClonesArray *>(ioman->GetObject(fFwEndcapDigiArray->GetBranchName().Data())));

  fFwEndcapClusterArray->SetBranchName(BSEmcDataBranchNames::fgClusterBranchName + "FwEndcap");
  fFwEndcapClusterArray->SetTCA(dynamic_cast<TClonesArray *>(ioman->GetObject(fFwEndcapClusterArray->GetBranchName().Data())));

  // Get input array
  fBwEndcapDigiArray->SetBranchName(BSEmcDataBranchNames::fgDigiBranchName + "BwEndcap");
  fBwEndcapDigiArray->SetTCA(dynamic_cast<TClonesArray *>(ioman->GetObject(fBwEndcapDigiArray->GetBranchName().Data())));

  fBwEndcapClusterArray->SetBranchName(BSEmcDataBranchNames::fgClusterBranchName + "BwEndcap");
  fBwEndcapClusterArray->SetTCA(dynamic_cast<TClonesArray *>(ioman->GetObject(fBwEndcapClusterArray->GetBranchName().Data())));

  // Get input array
  fShashlikDigiArray->SetBranchName(BSEmcDataBranchNames::fgDigiBranchName + "Shashlik");
  fShashlikDigiArray->SetTCA(dynamic_cast<TClonesArray *>(ioman->GetObject(fShashlikDigiArray->GetBranchName().Data())));

  fShashlikClusterArray->SetBranchName(BSEmcDataBranchNames::fgClusterBranchName + "Shashlik");
  fShashlikClusterArray->SetTCA(dynamic_cast<TClonesArray *>(ioman->GetObject(fShashlikClusterArray->GetBranchName().Data())));

  // Create and register output array

  fPhiRecoHitArray->SetBranchName("EmcPhiRecoHit");
  fPhiRecoHitArray->SetTCA(
    dynamic_cast<TClonesArray *>(ioman->Register(fPhiRecoHitArray->GetBranchName().Data(), fPhiRecoHitArray->GetClassName(), "Emc", fPhiRecoHitArray->GetPersistency())));
  LOG(debug) << "BSEmcPhiRecoHitSplitter: Intialization successfull";

  return kSUCCESS;
}

/**
 * @brief Runs the task
 *
 * @param opt unused
 * @return void
 */
void BSEmcPhiRecoHitSplitter::Exec(Option_t * /*unused*/)
{
  fPhiRecoHitArray->Reset();
  CreatePhiHits(fBarrelDigiArray->GetVectorOfPtrToConst(), fBarrelClusterArray->GetVectorOfPtrToConst(), BSEmcDataBranchNames::fgClusterBranchName + "Barrel", fBarrelPosition);
  CreatePhiHits(fFwEndcapDigiArray->GetVectorOfPtrToConst(), fFwEndcapClusterArray->GetVectorOfPtrToConst(), BSEmcDataBranchNames::fgClusterBranchName + "FwEndcap", fFWECPosition);
  CreatePhiHits(fBwEndcapDigiArray->GetVectorOfPtrToConst(), fBwEndcapClusterArray->GetVectorOfPtrToConst(), BSEmcDataBranchNames::fgClusterBranchName + "BwEndcap", fBWECPosition);
  CreatePhiHits(fShashlikDigiArray->GetVectorOfPtrToConst(), fShashlikClusterArray->GetVectorOfPtrToConst(), BSEmcDataBranchNames::fgClusterBranchName + "Shashlik",
                fShashlikPosition);
}

void BSEmcPhiRecoHitSplitter::CreatePhiHits(const std::vector<const BSEmcDigi *> &t_digiArray, const std::vector<const BSEmcCluster *> &t_clusterArray,
                                            const TString &t_clusterBranchName, BSEmcCrystalPositionPar *t_positionPar)
{
  // loop on each cluster. For each cluster there can be any number of phiRecoHits (atleast one)
  Int_t nClusters = t_clusterArray.size();
  for (Int_t iCluster = 0; iCluster < nClusters; iCluster++) {

    const BSEmcCluster *theCluster = t_clusterArray.at(iCluster);
    if (theCluster->GetEnergy() < 0.03) continue; // skip low energy clusters
    
    const Int_t TotNumOfHitPhi = 160;

    std::vector<Double_t> phiRecoHit(TotNumOfHitPhi, 0);

    for (const BSEmcDigiInfo_t &i_digi : theCluster->GetDigis()) {
      const BSEmcDigi *emcDigi = t_digiArray.at(i_digi.fDigiIdx);
      Double_t emcDigiPhi = t_positionPar->GetPosition(emcDigi->GetDetectorId()).Phi() * TMath::RadToDeg();
      Double_t emcDigiEnergy = emcDigi->GetEnergy();
      if (fabs(emcDigiPhi) <= 180) {
        Int_t iBSEmcDigiPhi = Int_t((emcDigiPhi + 180.) * TotNumOfHitPhi / 360.);
        phiRecoHit.at(iBSEmcDigiPhi) += emcDigiEnergy;
      }
    }

    std::vector<Double_t> vPhiList;
    std::vector<Double_t> vDepoEnergyList;
    std::vector<Int_t> vGapSizeList;

    Int_t i_phi_prev = 0;
    for (Int_t i_phi = 0; i_phi < TotNumOfHitPhi; ++i_phi) {
      Double_t BinValue = phiRecoHit.at(i_phi);
      if (BinValue != 0) {
        vPhiList.push_back(-180. + ((0.5 + i_phi) * 360. / TotNumOfHitPhi));
        vDepoEnergyList.push_back(BinValue);
        vGapSizeList.push_back(i_phi - i_phi_prev);
        i_phi_prev = i_phi;
      }
    }

    // Find start bin number for the phi projection of cluster.
    Int_t StartIndex = 0;
    for (size_t i = 0; i < vGapSizeList.size(); i++) {
      if (vGapSizeList.at(i) > vGapSizeList.at(StartIndex)) {
        StartIndex = i;
      }
    }

    // Rotate to the begining of the cluster in case cluster folds over to index=0
    std::rotate(vDepoEnergyList.begin(), vDepoEnergyList.begin() + StartIndex, vDepoEnergyList.end());
    vDepoEnergyList.push_back(0);
    vDepoEnergyList.push_back(0);
    std::rotate(vDepoEnergyList.begin(), vDepoEnergyList.begin() + (vDepoEnergyList.size() - 1), vDepoEnergyList.end());
    // Do the same for the phi positions
    std::rotate(vPhiList.begin(), vPhiList.begin() + StartIndex, vPhiList.end());
    vPhiList.push_back(0);
    vPhiList.push_back(0);
    std::rotate(vPhiList.begin(), vPhiList.begin() + (vPhiList.size() - 1), vPhiList.end());

    // Loop through deposited energy vector and identify "valley" type bins => -_- and calculate wieghts to split energy
    std::vector<Int_t> ValleyType;
    std::vector<Double_t> Weight;
    Double_t _Weight = 0;
    Weight.push_back(0);
    ValleyType.push_back(0);
    for (size_t n_sel = 1; n_sel < vDepoEnergyList.size() - 1; n_sel++) {
      if (vDepoEnergyList.at(n_sel - 1) > vDepoEnergyList.at(n_sel) && vDepoEnergyList.at(n_sel) < vDepoEnergyList.at(n_sel + 1)) {
        ValleyType.push_back(1);
        Weight.push_back(_Weight);
        _Weight = 0;
      } else {
        _Weight += vDepoEnergyList.at(n_sel);
        ValleyType.push_back(0);
      }
    }
    Weight.push_back(_Weight);
    Weight.push_back(0);

    std::vector<Double_t> enePhiRecoHit, phiPhiRecoHit;
    Int_t ValleyIndex = 0;
    Int_t iWeight = 0;
    for (size_t n_sel = 1; n_sel < vDepoEnergyList.size() - 1; n_sel++) {
      if (ValleyType.at(n_sel) == 1 || n_sel == vDepoEnergyList.size() - 2) {
        iWeight++;

        const Double_t _eneRightEdge = vDepoEnergyList.at(ValleyIndex) * (Weight.at(iWeight) / (Weight.at(iWeight) + Weight.at(iWeight - 1)));
        Double_t _enePhiRecoHit = _eneRightEdge;
        Double_t _phiPhiRecoHit = vPhiList.at(ValleyIndex) * _eneRightEdge;
        for (size_t p = ValleyIndex + 1; p < n_sel; p++) {
          _enePhiRecoHit += vDepoEnergyList.at(p);
          _phiPhiRecoHit += vPhiList.at(p) * vDepoEnergyList.at(p);
        }
        const Double_t _eneLeftEdge = vDepoEnergyList.at(n_sel) * (Weight.at(iWeight) / (Weight.at(iWeight) + Weight.at(iWeight + 1)));
        _enePhiRecoHit += _eneLeftEdge;
        _phiPhiRecoHit += vPhiList.at(n_sel) * _eneLeftEdge;
        _phiPhiRecoHit /= _enePhiRecoHit;
        enePhiRecoHit.push_back(_enePhiRecoHit);
        phiPhiRecoHit.push_back(_phiPhiRecoHit);
        ValleyIndex = n_sel;
      }
    }

    TVector3 posClust = theCluster->GetPosition();
    for (size_t i_phirecoHit = 0; i_phirecoHit < enePhiRecoHit.size(); ++i_phirecoHit) {
      BSEmcRecoHit *theNewPhiRecoHit = AddPhiRecoHit();
      theNewPhiRecoHit->SetClusterIndex(iCluster);
      theNewPhiRecoHit->SetSubClusterIndex(-1);
      const Int_t detId = theCluster->GetDigis()[0].fDetectorId;
      theNewPhiRecoHit->SetModule(BSEmcDetectorID{detId}.GetModule());
      theNewPhiRecoHit->SetLink(FairLink(t_clusterBranchName, iCluster));
      theNewPhiRecoHit->SetRawEnergy(enePhiRecoHit.at(i_phirecoHit));
      theNewPhiRecoHit->SetTimeStamp(theCluster->GetTimeStamp());
      theNewPhiRecoHit->SetTimeStampError(theCluster->GetTimeStampError());
      TVector3 posPhiRecoHit;
      posPhiRecoHit.SetMagThetaPhi(posClust.Mag(), posClust.Theta(), phiPhiRecoHit.at(i_phirecoHit) * TMath::DegToRad());
      theNewPhiRecoHit->SetPosition(posPhiRecoHit);
    }
  }
}

/**
 * @brief Adds a new BSEmcRecoHit to fPhiRecoHitArray and returns it.
 *
 * @return BSEmcRecoHit*
 */
BSEmcRecoHit *BSEmcPhiRecoHitSplitter::AddPhiRecoHit()
{
  return fPhiRecoHitArray->CreateCopy({});
}

/**
 * @brief Called at end of task.
 *
 * @return void
 */
void BSEmcPhiRecoHitSplitter::FinishTask()
{
  LOG(debug) << "=================================================";
  LOG(debug) << "BSEmcPhiRecoHitSplitter::FinishTask";
  LOG(debug) << "=================================================";
}

void BSEmcPhiRecoHitSplitter::SetParContainers()
{

  // Get run and runtime database
  FairRun *run = FairRun::Instance();
  if (run == nullptr) {
    LOG(fatal) << "SetParContainers: No analysis run";
  }
  FairRuntimeDb *db = run->GetRuntimeDb();
  if (db == nullptr) {
    LOG(fatal) << "SetParContainers: No runtime database";
  }
  fBarrelPosition = dynamic_cast<BSEmcCrystalPositionPar *>(db->getContainer("EmcCrystalPositionParBarrel"));
  fFWECPosition = dynamic_cast<BSEmcCrystalPositionPar *>(db->getContainer("EmcCrystalPositionParFwEndcap"));
  fBWECPosition = dynamic_cast<BSEmcCrystalPositionPar *>(db->getContainer("EmcCrystalPositionParBwEndcap"));
  fShashlikPosition = dynamic_cast<BSEmcCrystalPositionPar *>(db->getContainer("EmcCrystalPositionParShashlik"));
}

ClassImp(BSEmcPhiRecoHitSplitter)
