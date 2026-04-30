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

/*
 * PndMvdRadDamTask.cxx
 *
 *  Created on: Dec 16, 2008
 *      Author: stockman
 */

#include "PndMvdRadDamTask.h"

#include "FairRootManager.h"
#include "FairLogger.h"
#include "PndSdsMCPoint.h"
#include "PndMCTrack.h"
#include "PndMvdRadDamHit.h"
#include "PndStringSeparator.h"

#include <iostream>

PndMvdRadDamTask::PndMvdRadDamTask()
  : fPersistance(kTRUE), fMCTracks(nullptr), fMCHits(nullptr), fRadDamHits(nullptr), fElectronList(nullptr), fProtonList(nullptr), fNeutronList(nullptr), fPionList(nullptr),
    fGeoH(PndGeoHandling::Instance()), fMapDetHistos(), fRadDamHisto(nullptr), fWeightListsMap()
{
}

PndMvdRadDamTask::~PndMvdRadDamTask()
{
  for (std::map<std::string, TH2 *>::iterator it = fMapDetHistos.begin(); it != fMapDetHistos.end(); it++)
    delete (it->second);
  fMapDetHistos.clear();
  delete (fRadDamHisto);
}

void PndMvdRadDamTask::SetParContainers() {}

InitStatus PndMvdRadDamTask::ReInit()
{
  SetParContainers();
  return kSUCCESS;
}

InitStatus PndMvdRadDamTask::Init()
{
  FairRootManager *ioman = FairRootManager::Instance();

  if (!ioman) {
    std::cout << "-E- PndMvdRadDamTask::Init: "
              << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  fMCTracks = (TClonesArray *)ioman->GetObject("MCTrack");
  if (!fMCTracks) {
    std::cout << "-W- PndMvdRadDamTask::Init: "
              << "No MCTrack array!" << std::endl;
    return kERROR;
  }

  fMCHits = (TClonesArray *)ioman->GetObject("MVDPoint");
  if (!fMCHits) {
    std::cout << "-W- PndMvdRadDamTask::Init: "
              << "No MVDPoint array!" << std::endl;
    return kERROR;
  }

  // Create and register output array
  fRadDamHits = new TClonesArray("PndMvdRadDamHit");
  ioman->Register("MVDRadDamHit", "MVD", fRadDamHits, fPersistance);

  InitWeightLists();
  fRadDamHisto = new TH1D("radDamH", "Weight Factors", 1000, 0, 100);

  LOG(info) << " PndMvdRadDamTask: Initialization successful";

  return kSUCCESS;
}

void PndMvdRadDamTask::InitWeightLists()
{
  fElectronList = new PndMvdRadDamList("$VMCWORKDIR/detectors/mvd/MvdTools/MvdRadDamage/electronsWeight.root"); //FIXME: Move to params
  fProtonList = new PndMvdRadDamList("$VMCWORKDIR/detectors/mvd/MvdTools/MvdRadDamage/protonsWeight.root"); //FIXME: Move to params
  fNeutronList = new PndMvdRadDamList("$VMCWORKDIR/detectors/mvd/MvdTools/MvdRadDamage/neutronsWeight.root"); //FIXME: Move to params
  fPionList = new PndMvdRadDamList("$VMCWORKDIR/detectors/mvd/MvdTools/MvdRadDamage/pionsWeight.root"); //FIXME: Move to params

  fWeightListsMap[11] = fElectronList;       // e-
  fWeightListsMap[-11] = fElectronList;      // e+
  fWeightListsMap[2212] = fProtonList;       // p+
  fWeightListsMap[-2212] = fProtonList;      // p-
  fWeightListsMap[2112] = fNeutronList;      // n
  fWeightListsMap[-2112] = fNeutronList;     // n_bar
  fWeightListsMap[111] = fPionList;          // pi0
  fWeightListsMap[211] = fPionList;          // pi+
  fWeightListsMap[-211] = fPionList;         // pi-
  fWeightListsMap[1000010030] = fProtonList; // Tritium
  fWeightListsMap[1000010020] = fProtonList; // Deuterium
}

void PndMvdRadDamTask::Exec(Option_t *)
{
  // Reset output array
  if (!fRadDamHits)
    Fatal("Exec", "No RadDamArray");
  fRadDamHits->Delete();

  PndSdsMCPoint *mcPoint;
  PndMCTrack *mcTrack;

  for (int i = 0; i < fMCHits->GetEntriesFast(); i++) {
    mcPoint = (PndSdsMCPoint *)fMCHits->At(i);
    mcTrack = (PndMCTrack *)(fMCTracks->At(mcPoint->GetTrackID()));
    TVector3 mom;
    mcPoint->Momentum(mom);
    Double_t Ekin = mom.Mag();
    Int_t pid = mcTrack->GetPdgCode();
    Double_t weight = 0;
    if (fWeightListsMap[pid] != nullptr) {
      weight = fWeightListsMap[pid]->GetWeight(Ekin);
    }
    // std::cout << "WeightCalc: pid: " << pid << " Energy: " << Ekin << " Weight: " << weight << std::endl;
    new ((*fRadDamHits)[i]) PndMvdRadDamHit(mcPoint->GetTrackID(), i, mcPoint->GetSensorID(), pid, Ekin, mcPoint->GetPosition(), mom, weight);
    TString detname = fGeoH->GetPath(mcPoint->GetSensorID());
    if (fMapDetHistos[detname.Data()] == 0) {
      PndStringSeparator svec(detname.Data(), "/");
      TVector3 sensDim = fGeoH->GetSensorDimensionsShortId(mcPoint->GetSensorID());
      fMapDetHistos[detname.Data()] =
        new TH2D(svec.Replace("/", "o").c_str(), fGeoH->GetPath(mcPoint->GetSensorID()), (Int_t)(2 * sensDim.X() * 10), -sensDim.X(), sensDim.X(), // point resolution mm^2
                 (Int_t)(2 * sensDim.Y() * 10), -sensDim.Y(), sensDim.Y());
    }
    TVector3 localHit = fGeoH->MasterToLocalShortId(mcPoint->GetPosition(), mcPoint->GetSensorID());
    ((TH2D *)(fMapDetHistos[detname.Data()]))->Fill(localHit.X(), localHit.Y(), weight);
    fRadDamHisto->Fill(weight);
  }
}

void PndMvdRadDamTask::Finish()
{
  for (std::map<std::string, TH2 *>::iterator it = fMapDetHistos.begin(); it != fMapDetHistos.end(); it++)
    it->second->Write();
  fRadDamHisto->Write();
}

ClassImp(PndMvdRadDamTask);
