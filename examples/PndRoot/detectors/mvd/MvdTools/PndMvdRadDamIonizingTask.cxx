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
 * PndMvdRadDamIonizingTask.cxx
 *
 *  Created on: Dec 16, 2008
 *      Author: stockman
 */

#include "PndMvdRadDamIonizingTask.h"

#include "FairRootManager.h"
#include "FairLogger.h"
#include "PndSdsMCPoint.h"
#include "PndMCTrack.h"
#include "PndMvdRadDamHit.h"
#include "PndStringSeparator.h"

#include <iostream>

PndMvdRadDamIonizingTask::PndMvdRadDamIonizingTask()
  : fPersistance(kTRUE), fMCHits(nullptr), fRadDamHits(nullptr), fGeoH(PndGeoHandling::Instance()), fMapDetHistos(), fRadDamHisto(nullptr)
{
}

PndMvdRadDamIonizingTask::~PndMvdRadDamIonizingTask()
{
  for (std::map<std::string, TProfile2D *>::iterator it = fMapDetHistos.begin(); it != fMapDetHistos.end(); it++)
    delete (it->second);
  fMapDetHistos.clear();
  delete (fRadDamHisto);
}

void PndMvdRadDamIonizingTask::SetParContainers() {}

InitStatus PndMvdRadDamIonizingTask::ReInit()
{
  SetParContainers();
  return kSUCCESS;
}

InitStatus PndMvdRadDamIonizingTask::Init()
{
  FairRootManager *ioman = FairRootManager::Instance();

  if (!ioman) {
    std::cout << "-E- PndMvdRadDamIonizingTask::Init: "
              << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  fMCHits = (TClonesArray *)ioman->GetObject("MVDPoint");
  if (!fMCHits) {
    std::cout << "-W- PndMvdRadDamIonizingTask::Init: "
              << "No MVDPoint array!" << std::endl;
    return kERROR;
  }

  // Create and register output array
  //	fRadDamHits = ioman->Register("MVDRadDamHit", "PndMvdRadDamHit", "MVD", kTRUE);

  fRadDamHisto = new TH1D("radDamH", "absorbed dose", 10000000, 1E-12, 1E-4);

  LOG(info) << " PndMvdRadDamIonizingTask: Initialization successful";

  return kSUCCESS;
}

void PndMvdRadDamIonizingTask::Exec(Option_t *)
{
  // Reset output array

  PndSdsMCPoint *mcPoint;

  for (int i = 0; i < fMCHits->GetEntriesFast(); i++) {
    mcPoint = (PndSdsMCPoint *)fMCHits->At(i);
    TVector3 length = mcPoint->GetPosition();
    length -= mcPoint->GetPositionOut();
    TVector3 mom;
    mcPoint->Momentum(mom);
    Double_t energyLoss = mcPoint->GetEnergyLoss();
    Double_t dEdx;
    if (length.Mag() > 0)
      dEdx = energyLoss / length.Mag();
    else
      dEdx = 0;
    Double_t gray = dEdx * 1E9 * 1.6E-19 * 1 / 0.01 * 1 / 2.4 *
                    1000; // dEdx [GeV/cm] * conversion to electrons * conversion to Joule * sensitive Area mm2 * density silicon * conversion 1/g to 1/kg= Joule / kg

    TString detname = fGeoH->GetPath(mcPoint->GetSensorID());

    if (fMapDetHistos[detname.Data()] == 0) {
      PndStringSeparator svec(detname.Data(), "/");
      TVector3 sensDim = fGeoH->GetSensorDimensionsShortId(mcPoint->GetSensorID());
      std::string histoName = "";
      histoName += svec.Replace("/", "o");
      fMapDetHistos[detname.Data()] = new TProfile2D(histoName.c_str(), detname.Data(), (Int_t)(2 * sensDim.X() * 10), -sensDim.X(), sensDim.X(), // point resolution mm^2
                                                     (Int_t)(2 * sensDim.Y() * 10), -sensDim.Y(), sensDim.Y());
    }
    // std::cout << "Damage: " << gray << " gray" << std::endl;
    TVector3 localHit = fGeoH->MasterToLocalShortId(mcPoint->GetPosition(), mcPoint->GetSensorID());
    ((TProfile2D *)(fMapDetHistos[detname.Data()]))->Fill(localHit.X(), localHit.Y(), gray);
    fRadDamHisto->Fill(gray);

    //	  fRadDamHits = FairRootManager::Instance()->GetTClonesArray("MVDRadDamHit");
    //	  PndMvdRadDamHit* tempHit = new ((*fRadDamHits)[fRadDamHits->GetEntries()]) PndMvdRadDamHit(mcPoint->GetTrackID(), i, mcPoint->GetSensorID(), 0, 0, mcPoint->GetPosition(),
    // mom, dEdx);
    //  std::cout << "TempHit: " << *tempHit << std::endl;
  }
}

void PndMvdRadDamIonizingTask::FinishEvent()
{
  // fRadDamHits->Delete();
}

void PndMvdRadDamIonizingTask::FinishTask()
{
  for (std::map<std::string, TProfile2D *>::iterator it = fMapDetHistos.begin(); it != fMapDetHistos.end(); it++)
    it->second->Write();
  fRadDamHisto->Write();

  // fRadDamHisto->Draw();
}

ClassImp(PndMvdRadDamIonizingTask);
