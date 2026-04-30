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

// -------------------------------------------------------------------------
// -----                   PndDrcDigiTask source file                  -----
// -----               Created 15/04/13  by M.Patsyuk                  -----
// -----         HARPHOOL KUMAWAT h.kumawat@gsi.de                     -----
// -----                                                               -----
// -------------------------------------------------------------------------
#include <fstream>
#include <iostream>
#include <map>
#include "stdio.h"

#include "TGeoManager.h"

#include "FairRootManager.h"
#include "PndDrcDigiTask.h"

#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairGeoVector.h"
#include "FairGeoNode.h"
#include "FairGeoVolume.h"
#include "FairLogger.h"

#include "TVector3.h"
#include "TRandom.h"
#include "TString.h"
#include "TGeoBBox.h"

#include "FairBaseParSet.h"
#include "FairGeoTransform.h"

#include "PndGeoDrcPar.h"

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndDrcDigiTask::PndDrcDigiTask() : PndPersistencyTask("PndDrcDigiTask")
{
  fGeo = new PndGeoDrc();
  fGeoH = nullptr;
  fPDPointArray = nullptr;

  SetParameters();
  Reset();
}

// -----   Standard constructor with verbosity level  -------------------------------------------
PndDrcDigiTask::PndDrcDigiTask(Int_t verbose) : PndPersistencyTask("PndDrcDigiTask", verbose)
{
  fVerbose = verbose;
  fGeo = new PndGeoDrc();
  fGeoH = nullptr;
  fPDPointArray = nullptr;

  SetParameters();
  Reset();
}

void PndDrcDigiTask::SetParameters()
{
  fTimeOrderedDigi = kFALSE;
  fChargeSharing = kFALSE;
  fDeadTime = 5;

  fDetType = 1;             //  Detector Type =1
  fSigmat = 0.1;            // Time Resolution is 100 ps ############################
  fTimeGranularity = 0.001; // [ns] = 98 ps granularity of the time signal
  fPixelSize = fGeo->PixelSize();
  fMcpActiveArea = fGeo->McpActiveArea();
  fNpix = fGeo->Npixels();
  fPixelGap = (fMcpActiveArea - (Double_t)fNpix * fPixelSize) / ((Double_t)fNpix - 1.);
  fPixelStep = fMcpActiveArea / fNpix; // fPixelSize + 0.5*fPixelGap;
  fPixelSigma = fGeo->SigmaCharge();
  fThreshold = 0.20; // threshold to detect charge shared hits
}

// -----   Destructor   ----------------------------------------------------
PndDrcDigiTask::~PndDrcDigiTask()
{
  Reset();
  if (fGeo)
    delete fGeo;
}

// -----   Initialization   -----------------------------------------------
InitStatus PndDrcDigiTask::Init()
{
  cout << " ---------- INITIALIZATION ------------" << endl;
  nevents = 0;

  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    cout << "-E- PndDrcDigiTask::Init: "
         << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  // PndGeoHandling
  if (fGeoH == nullptr)
    fGeoH = PndGeoHandling::Instance();

  fGeoH->SetParContainers();
  fGeoH->SetVerbose(fVerbose);
  if (fVerbose > 1)
    Info("SetParContainers", "done.");

  // // Get input array
  // fMCArray = (TClonesArray*) ioman->GetObject("MCTrack");
  // if ( ! fMCArray ) {
  //   cout << "-W- PndDrcRecoLookupMap::Init: "
  //        << "No MCTrack array!" << endl;
  //   return kERROR;
  // }

  // Get input array
  fBarPointArray = (TClonesArray *)ioman->GetObject("DrcBarPoint");
  if (!fBarPointArray) {
    cout << "-W- PndDrcDigiTask::Init: "
         << "No DrcBarPoint array!" << endl;
    return kERROR;
  }

  // Get Photon point array
  fPDPointArray = (TClonesArray *)ioman->GetObject("DrcPDPoint");
  if (!fPDPointArray) {
    cout << "-W- PndDrcAna::Init: "
         << "No DrcPDPoint array!" << endl;
    return kERROR;
  }

  // Create and register output buffer
  fDataBuffer = new PndDrcDigiWriteoutBuffer("DrcDigi", "PndDrc", GetPersistency());
  fDataBuffer = (PndDrcDigiWriteoutBuffer *)ioman->RegisterWriteoutBuffer("DrcDigi", fDataBuffer);
  fDataBuffer->SetVerbose(fVerbose);
  fDataBuffer->ActivateBuffering(fTimeOrderedDigi);

  LOG(info) << " PndDrcDigiTask: Intialization successfull";

  return kSUCCESS;
}

// -----   Execution of Task   ---------------------------------------------
void PndDrcDigiTask::Exec(Option_t *)
{ // ion //[R.K.03/2017] unused variable(s)
  Reset();
  if (fVerbose > 0)
    LOG(info) << " PndDrcDigiTask: Event # " << nevents << " has " << fPDPointArray->GetEntriesFast() << " PD points";
  ProcessPhotonPoint();
  nevents++;
}

//-------- Photon Detector Hit production with efficiency-------------
void PndDrcDigiTask::ProcessPhotonPoint()
{
  for (Int_t k = 0; k < fPDPointArray->GetEntriesFast(); k++) {

    fPpt = (PndDrcPDPoint *)fPDPointArray->At(k);
    // fMCtrk = (PndMCTrack*)fMCArray->At(fPpt->GetTrackID());
    Int_t bpId = fPpt->GetBarPointID();
    if (bpId < 0) {
      std::cout << " Error: bpId = " << bpId << std::endl;

      continue;
    }

    fBarPoint = (PndDrcBarPoint *)fBarPointArray->At(bpId);

    // transform to local sensor system
    TVector3 PptPosition;
    fPpt->Position(PptPosition);
    TVector3 posL = fGeoH->MasterToLocalShortId(PptPosition, fPpt->GetDetectorID());
    TVector3 sensorDim = GetSensorDimensions(fPpt->GetDetectorID());

    // usually sensors have origin in the middle, let's move it to the left lower corner:
    TVector3 posLshifted = posL + sensorDim;

    // calculate the number of the fired pixel
    Int_t Ncol = (Int_t)TMath::Floor(posLshifted.X() / fPixelStep);
    Int_t Nrow = (Int_t)TMath::Floor(posLshifted.Y() / fPixelStep);
    Int_t NpixelLocal = Ncol + Nrow * fNpix;

    fDetectorID = fPpt->GetDetectorID() * 100 + NpixelLocal;
    Int_t mcpId = fPpt->GetMcpId();
    Int_t sensorId = mcpId * 100 + NpixelLocal;

    // time is smeared and digitized = has granularity
    fTime = (Int_t)(fPpt->GetTime() / fTimeGranularity) * fTimeGranularity;
    fTime = fPpt->GetTime();
    if (fSigmat > 0)
      Smear(fTime, fSigmat);

    ActivatePixel(sensorId, k, 0);

    if (fChargeSharing) {
      Double_t distance = 999.;
      TVector3 corner, point;

      // left pixel
      distance = posLshifted.X() - TMath::Floor(posLshifted.X() / fPixelStep) * fPixelStep;
      if (exp(-distance / fPixelSigma) > gRandom->Uniform(0., 1.) && exp(-distance / fPixelSigma) > fThreshold) {
        if ((Ncol - 1) >= 0) {
          ActivatePixel(mcpId * 100 + Ncol - 1 + Nrow * fNpix, k, 1);
        }
      }
      // right pixel
      distance = TMath::Ceil(posLshifted.X() / fPixelStep) * fPixelStep - posLshifted.X();
      if (exp(-distance / fPixelSigma) > gRandom->Uniform(0., 1.) && exp(-distance / fPixelSigma) > fThreshold) {
        if (Ncol + 1 < fNpix) {
          ActivatePixel(mcpId * 100 + Ncol + 1 + Nrow * fNpix, k, 1);
        }
      }
      // lower pixel
      distance = posLshifted.Y() - TMath::Floor(posLshifted.Y() / fPixelStep) * fPixelStep;
      if (exp(-distance / fPixelSigma) > gRandom->Uniform(0., 1.) && exp(-distance / fPixelSigma) > fThreshold) {
        if (Nrow - 1 >= 0) {
          ActivatePixel(mcpId * 100 + Ncol + (Nrow - 1) * fNpix, k, 1);
        }
      }
      // upper pixel
      distance = TMath::Ceil(posLshifted.Y() / fPixelStep) * fPixelStep - posLshifted.Y();
      if (exp(-distance / fPixelSigma) > gRandom->Uniform(0., 1.) && exp(-distance / fPixelSigma) > fThreshold) {
        if (Nrow + 1 < fNpix) {
          ActivatePixel(mcpId * 100 + Ncol + (Nrow + 1) * fNpix, k, 1);
        }
      }

      point.SetXYZ(posLshifted.X(), posLshifted.Y(), 0.);

      // upper left pixel
      corner.SetXYZ(TMath::Floor(posLshifted.X() / fPixelStep) * fPixelStep, TMath::Ceil(posLshifted.Y() / fPixelStep) * fPixelStep, 0.);
      distance = (point - corner).Mag();
      if (exp(-distance / fPixelSigma) > gRandom->Uniform(0., 1.) && exp(-distance / fPixelSigma) > fThreshold) {
        if (Ncol - 1 >= 0 && Nrow + 1 < fNpix) {
          ActivatePixel(mcpId * 100 + Ncol - 1 + (Nrow + 1) * fNpix, k, 1);
        }
      }

      // bottom left pixel
      corner.SetXYZ(TMath::Floor(posLshifted.X() / fPixelStep) * fPixelStep, TMath::Floor(posLshifted.Y() / fPixelStep) * fPixelStep, 0.);
      distance = (point - corner).Mag();
      if (exp(-distance / fPixelSigma) > gRandom->Uniform(0., 1.) && exp(-distance / fPixelSigma) > fThreshold) {
        if (Ncol - 1 >= 0 && Nrow - 1 >= 0) {
          ActivatePixel(mcpId * 100 + Ncol - 1 + (Nrow - 1) * fNpix, k, 1);
        }
      }

      // bottom right pixel
      corner.SetXYZ(TMath::Ceil(posLshifted.X() / fPixelStep) * fPixelStep, TMath::Floor(posLshifted.Y() / fPixelStep) * fPixelStep, 0.);
      distance = (point - corner).Mag();
      if (exp(-distance / fPixelSigma) > gRandom->Uniform(0., 1.) && exp(-distance / fPixelSigma) > fThreshold) {
        if (Ncol + 1 < fNpix && Nrow - 1 >= 0) {
          ActivatePixel(mcpId * 100 + Ncol + 1 + (Nrow - 1) * fNpix, k, 1);
        }
      }

      // upper right pixel
      corner.SetXYZ(TMath::Ceil(posLshifted.X() / fPixelStep) * fPixelStep, TMath::Ceil(posLshifted.Y() / fPixelStep) * fPixelStep, 0.);
      distance = (point - corner).Mag();
      if (exp(-distance / fPixelSigma) > gRandom->Uniform(0., 1.) && exp(-distance / fPixelSigma) > fThreshold) {
        if (Ncol + 1 < fNpix && Nrow + 1 < fNpix) {
          ActivatePixel(mcpId * 100 + Ncol + 1 + (Nrow + 1) * fNpix, k, 1);
        }
      }
    }
  }
}

// -----   Private method ActivatePixel   -------------------------------
void PndDrcDigiTask::ActivatePixel(Int_t sensorId, Int_t k, Int_t csflag)
{
  // in case when the same pixel was fired by two different photons this function takes care of which hits from that pixel to write
  PndDrcDigi *digi = new PndDrcDigi(k, fDetectorID, sensorId, 0., fTime, csflag, 0.);
  Double_t timeStamp = fTime + FairRootManager::Instance()->GetEventTime();

  digi->SetPdgCode(fBarPoint->GetPdgCode());
  digi->SetTimeStamp(timeStamp);
  digi->SetTimeAtBar(fBarPoint->GetTime());
  if (fSigmat > 0)
    digi->SetTimeStampError(fSigmat / TMath::Sqrt(fSigmat));
  else
    digi->SetTimeStampError(0);

  FairEventHeader *evtHeader = (FairEventHeader *)FairRootManager::Instance()->GetObject("EventHeader.");

  digi->SetLink(fPpt->GetLink(0)); // MCTrack
  digi->AddLink(FairLink(evtHeader->GetInputFileId(), evtHeader->GetMCEntryNumber() - 1, "DrcPDPoint", k));

  fDataBuffer->FillNewData(digi, timeStamp, timeStamp + fDeadTime);

  // if ( fPixelMap.find(sensorId) == fPixelMap.end() ){
  //   // pixel not yet active, create new digi
  //   fDataBuffer->FillNewData(digi, fTime, fTime + fDeadTime);
  //   fPixelMap[sensorId] = fNDigis;
  //   fNDigis++;
  // }else{
  //   // // if there was already hit in this pixel...
  //   // PndDrcDigi* ddigi = dynamic_cast<PndDrcDigi*>(fDrcDigiArray->At(fPixelMap[sensorId]));

  //   // // ... check the time difference with another one in the same pixel
  //   // if(fabs(ddigi->GetTimeStamp() - fTime) > fDeadTime){
  //   fDataBuffer->FillNewData(digi, fTime, fTime + fDeadTime);
  //   fPixelMap[sensorId] = fNDigis;
  //   fNDigis++;
  //   // }
  // }
}

//--------------------------------------------------------------
TVector3 PndDrcDigiTask::GetSensorDimensions(Int_t sensorID)
{
  gGeoManager->cd(fGeoH->GetPath(sensorID));
  TGeoVolume *actVolume = gGeoManager->GetCurrentVolume();
  TGeoBBox *actBox = (TGeoBBox *)(actVolume->GetShape());
  TVector3 result(actBox->GetDX(), actBox->GetDY(), actBox->GetDZ());
  return result;
}

//-------------Smear Time---------------------------------------
void PndDrcDigiTask::Smear(Double_t &time, Double_t sigt)
{
  time += gRandom->Gaus(0, sigt);
}

// -----   Private method Reset   ------------------------------
void PndDrcDigiTask::Reset()
{
  fNDigis = 0;
  fPixelMap.clear();
}

// -----   Finish Task   ---------------------------------------
void PndDrcDigiTask::Finish()
{
  LOG(info) << " PndDrcDigiTask: Finish";
}

ClassImp(PndDrcDigiTask)
