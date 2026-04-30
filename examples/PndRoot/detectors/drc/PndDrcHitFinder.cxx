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
// -----                PndDrcHitFinder source file             -----
// -------------------------------------------------------------------------

#include "TClonesArray.h"
#include "TArrayD.h"
#include "TGeoManager.h"

#include "FairRootManager.h"
#include "PndDrcHitFinder.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairGeoNode.h"
#include "FairGeoVector.h"
#include "FairRunAna.h"
#include "FairEventHeader.h"
#include "FairLogger.h"

//#include "PndDrcDigi.h"

#include "PndDetectorList.h"

// -----   Default constructor   -------------------------------------------
PndDrcHitFinder::PndDrcHitFinder() : PndPersistencyTask("DrcHitFinder", 1)
{
  SetPersistency(kTRUE);
  fPixelHits = 0;
  fEventNr = 0;
  fPixelFactor = 1;

  if (fVerbose > 0)
    Info("PndDrcHitFinder", "DRC Hit Finder created, Parameters will be taken from RTDB");
  fGeoH = nullptr;
  fGeo = new PndGeoDrc();
  fDigiArray = nullptr;
  fPdHitArray = nullptr;
  fMCEventHeader = nullptr;
  fStopFunctor = nullptr;

  fPixelSize = fGeo->PixelSize(); // pixel size;
  fNpix = fGeo->Npixels();        // pixel columns
  fMcpActiveArea = fGeo->McpActiveArea();
  fPixelGap = (fMcpActiveArea - (Double_t)fNpix * fPixelSize) / ((Double_t)fNpix - 1.);
  fPixelStep = fMcpActiveArea / fNpix; // fPixelSize + 0.5*fPixelGap;
}
// -------------------------------------------------------------------------

PndDrcHitFinder::PndDrcHitFinder(Int_t iVerbose) : PndPersistencyTask("DrcHitFinder", iVerbose)
{
  SetPersistency(kTRUE);
  fPixelHits = 0;
  fEventNr = 0;
  fPixelFactor = 1;
  fGeoH = nullptr;
  fGeo = new PndGeoDrc();
  fDigiPixelMCInfo = kFALSE;
  if (fVerbose > 0)
    Info("PndDrcHitFinder", "DrcHitFinder created, Parameters will be taken from RTDB");

  fDigiArray = nullptr;
  fPdHitArray = nullptr;
  fMCEventHeader = nullptr;
  fStopFunctor = nullptr;

  fPixelSize = fGeo->PixelSize(); // pixel size;
  fNpix = fGeo->Npixels();        // pixel rows in one FE
  fMcpActiveArea = fGeo->McpActiveArea();
  fPixelGap = (fMcpActiveArea - (Double_t)fNpix * fPixelSize) / ((Double_t)fNpix - 1.);
  fPixelStep = fPixelSize + 0.5 * fPixelGap;
}

PndDrcHitFinder::PndDrcHitFinder(const char *name, Int_t iVerbose) : PndPersistencyTask(name, iVerbose)
{
  fPixelHits = 0;
  fEventNr = 0;
  fPixelFactor = 1;
  fGeoH = nullptr;
  fGeo = new PndGeoDrc();
  fDigiPixelMCInfo = kFALSE;
  if (fVerbose > 0)
    Info("PndDrcHitFinder", "%s created, Parameters will be taken from RTDB", name);

  fDigiArray = nullptr;
  fPdHitArray = nullptr;
  fMCEventHeader = nullptr;

  fPixelSize = fGeo->PixelSize(); // pixel size;
  fNpix = fGeo->Npixels();        // pixel rows in one FE
  fMcpActiveArea = fGeo->McpActiveArea();
  fPixelGap = (fMcpActiveArea - (Double_t)fNpix * fPixelSize) / ((Double_t)fNpix - 1.);
  fPixelStep = fPixelSize + 0.5 * fPixelGap;
}

// -----   Destructor   ----------------------------------------------------
PndDrcHitFinder::~PndDrcHitFinder()
{
  if (fGeo)
    delete fGeo;
  if (fGeoH)
    delete fGeoH;
}
// -------------------------------------------------------------------------

// -----   Initialization  of Parameter Containers -------------------------
void PndDrcHitFinder::SetParContainers()
{
  if (fGeoH == nullptr)
    fGeoH = PndGeoHandling::Instance();
  fGeoH->SetParContainers();
  fGeoH->SetVerbose(fVerbose);
  if (fVerbose > 1)
    Info("SetParContainers", "done.");
  return;
}

InitStatus PndDrcHitFinder::ReInit()
{
  SetParContainers();
  return kSUCCESS;
}

// -----   Public method Init   --------------------------------------------
InitStatus PndDrcHitFinder::Init()
{
  // FairRun* ana = FairRun::Instance(); //[R.K. 01/2017] unused variable?
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    std::cout << "-E- PndDrcHitFinder::Init: "
              << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  fInBranchName = "DrcDigi";
  // Get input array
  fDigiArray = (TClonesArray *)ioman->GetObject(fInBranchName);
  if (!fDigiArray) {
    std::cout << "-W- PndDrcHitFinder::Init: "
              << "No DrcDigi array!" << std::endl;
    return kERROR;
  }

  // Create and register output array
  fPdHitArray = new TClonesArray("PndDrcPDHit");
  ioman->Register("DrcPDHit", "Drc", fPdHitArray, GetPersistency());

  fGapFunctor = new TimeGap();
  fStopFunctor = new StopTime();
  return kSUCCESS;
}

// -----   Public method Exec   --------------------------------------------
void PndDrcHitFinder::Exec(Option_t *)
{

  if (fVerbose > 3)
    Info("Exec", "Start");
  if (!fPdHitArray)
    Fatal("Exec", "No PdHitArray");
  fPdHitArray->Clear();

  Double_t etime = FairRootManager::Instance()->GetEventTime();
  if (FairRunAna::Instance()->IsTimeStamp()) {
    fDigiArray = FairRootManager::Instance()->GetData(fInBranchName, fStopFunctor, etime, fStopFunctor, etime + 100);
  }

  Int_t nDigis = fDigiArray->GetEntriesFast();
  if (fVerbose > 1)
    LOG(info) << " PndDrcHitFinder: Event # " << fEventNr << " has " << nDigis << " digis.";
  else if (fVerbose == 1 && fEventNr % 1000 == 0)
    LOG(info) << " PndDrcHitFinder: Event # " << fEventNr << " has " << nDigis << " digis.";

  Int_t detID = 0, mcpID = 0, pixelID = 0;
  TVector3 HitPosGlobal, HitPosLocal, dPosHit;
  Double_t hitTime = 0.;

  for (Int_t iDigi = 0; iDigi < nDigis; iDigi++) {
    fDigi = (PndDrcDigi *)fDigiArray->At(iDigi);
    detID = fDigi->GetDetectorId();
    pixelID = detID - 100 * (Int_t)TMath::Floor((Double_t)detID / 100.);
    mcpID = detID / 100;
    hitTime = fDigi->GetTime();

    // the pixel number shows local coordinates of the hit:
    HitPosLocal.SetXYZ(fPixelStep * ((Double_t)(pixelID % fNpix) - (Double_t)(fNpix / 2) + 0.5),
                       fPixelStep * (TMath::Floor(((Double_t)pixelID) / ((Double_t)fNpix)) - (Double_t)(fNpix / 2) + 0.5), 0.);
    Int_t sensorId = fDigi->GetSensorId() / fPixelFactor;
    if (fPixelFactor == 2) { // double pixels
      sensorId++;
      if (fDigi->GetSensorId() % 2)
        HitPosLocal.SetX(HitPosLocal.X() + fPixelSize / 2.);
      else
        HitPosLocal.SetX(HitPosLocal.X() - fPixelSize / 2.);
    }

    // local coordinates of the hit on the MCP are translated into global ones as the following:
    HitPosGlobal = fGeoH->LocalToMasterShortId(HitPosLocal, mcpID);
    dPosHit.SetXYZ(fPixelSize / 2., fPixelSize / 2., 0.);
    if (fPixelFactor == 2)
      dPosHit.SetXYZ(fPixelSize, fPixelSize / 2., 0.);

    if (fDigi->GetTimeStamp() != etime + hitTime)
      hitTime = fDigi->GetTimeStamp() - etime;
    PndDrcPDHit pdhit = PndDrcPDHit(detID, sensorId, HitPosGlobal, dPosHit, hitTime, 0., iDigi);
    pdhit.SetPdgCode(fDigi->GetPdgCode());
    pdhit.SetTimeStamp(fDigi->GetTimeStamp());
    pdhit.SetTimeAtBar(fDigi->GetTimeAtBar());
    pdhit.SetLink(fDigi->GetLink(0)); // MCTrack
    pdhit.AddLink(fDigi->GetLink(1)); // DrcPDPoint
    pdhit.AddLink(FairLink(-1, fEventNr, "DrcDigi", iDigi));
    //((FairMultiLinkedData)pdhit).Print(); std::cout<<std::endl;
    new ((*fPdHitArray)[fPdHitArray->GetEntriesFast()]) PndDrcPDHit(pdhit);
  }

  fEventNr++;
  if (fVerbose > 3)
    Info("Exec", "Loop MC points");

  fPdHitArray->Sort();
  fDigiArray->Delete();
}

// -------------------------------------------------------------------------
void PndDrcHitFinder::FinishEvent()
{
  FinishEvents();
}

// -------------------------------------------------------------------------
void PndDrcHitFinder::FinishTask() {}

ClassImp(PndDrcHitFinder);
