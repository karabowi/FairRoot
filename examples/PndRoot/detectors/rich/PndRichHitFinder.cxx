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
// -----                PndRichHitFinder source file                   -----
// -----                created 06 Apr 2017                            -----
// -------------------------------------------------------------------------

#include "TClonesArray.h"
#include "TArrayD.h"
#include "TGeoManager.h"

#include "FairRootManager.h"
#include "PndRichHitFinder.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairGeoNode.h"
#include "FairGeoVector.h"
#include "FairRunAna.h"
#include "FairEventHeader.h"
#include "FairLogger.h"

//#include "PndRichDigi.h"

#include "PndDetectorList.h"

// -----   Default constructor   -------------------------------------------
PndRichHitFinder::PndRichHitFinder() : PndPersistencyTask("RichHitFinder", 1)
{
  SetPersistency(kTRUE);
  fPixelHits = 0;
  fEventNr = 0;
  fPixelFactor = 1;
  fHitNumber = 0;

  if (fVerbose > 0)
    Info("PndRichHitFinder", "RICH Hit Finder created, Parameters will be taken from RTDB");
  fGeoH = nullptr;
  fGeo = new PndRichGeo();
  fDigiArray = nullptr;
  fPdHitArray = nullptr;
  fMCEventHeader = nullptr;
  fStopFunctor = nullptr;

  fInd = 0;
  fTimeGate = 1;
  fTimeStep = 0.5; // ns
  fIBuffer = 0;
  fIBufferPrev = 1;
  fBufferStartTime.resize(2);
  fBufferStartTime.at(0) = 0;
  fBufferStartTime.at(1) = 0;
  fHitsBuffer.resize(2);
  fHitsBuffer.at(0).resize(1000);
  fHitsBuffer.at(1).resize(1000);
  fBufferNumHits.resize(2);
  fBufferNumHits.at(0) = 0;
  fBufferNumHits.at(1) = 0;
  fThreshold = 5;
  petime = -1;

  //  fPixelSize = fGeo->PixelSize();	//pixel size;
  //  fNpix = fGeo->Npixels(); //pixel columns
  //  fMcpActiveArea = fGeo->McpActiveArea();
  //  fPixelGap      =  (fMcpActiveArea - (Double_t)fNpix*fPixelSize) / ((Double_t)fNpix - 1.);
  //  fPixelStep     =  fMcpActiveArea/fNpix;//fPixelSize + 0.5*fPixelGap;
}
// -------------------------------------------------------------------------

PndRichHitFinder::PndRichHitFinder(Int_t iVerbose) : PndPersistencyTask("RichHitFinder", iVerbose)
{
  SetPersistency(kTRUE);
  fPixelHits = 0;
  fEventNr = 0;
  fPixelFactor = 1;
  fGeoH = nullptr;
  fGeo = new PndRichGeo();
  fDigiPixelMCInfo = kFALSE;
  if (fVerbose > 0)
    Info("PndRichHitFinder", "RichHitFinder created, Parameters will be taken from RTDB");

  fDigiArray = nullptr;
  fPdHitArray = nullptr;
  fMCEventHeader = nullptr;
  fStopFunctor = nullptr;

  fInd = 0;
  fTimeGate = 1;
  fTimeStep = 0.5; // ns
  fIBuffer = 0;
  fIBufferPrev = 1;
  fBufferStartTime.resize(2);
  fBufferStartTime.at(0) = 0;
  fBufferStartTime.at(1) = 0;
  fHitsBuffer.resize(2);
  fHitsBuffer.at(0).resize(1000);
  fHitsBuffer.at(1).resize(1000);
  fBufferNumHits.resize(2);
  fBufferNumHits.at(0) = 0;
  fBufferNumHits.at(1) = 0;
  fThreshold = 5;
  petime = -1;

  //  fPixelSize = fGeo->PixelSize();	//pixel size;
  //  fNpix = fGeo->Npixels();              //pixel rows in one FE
  //  fMcpActiveArea = fGeo->McpActiveArea();
  //  fPixelGap      =  (fMcpActiveArea - (Double_t)fNpix*fPixelSize) / ((Double_t)fNpix - 1.);
  //  fPixelStep     =  fPixelSize + 0.5*fPixelGap;
}

PndRichHitFinder::PndRichHitFinder(const char *name, Int_t iVerbose) : PndPersistencyTask(name, iVerbose)
{
  SetPersistency(kTRUE);
  fPixelHits = 0;
  fEventNr = 0;
  fPixelFactor = 1;
  fGeoH = nullptr;
  fGeo = new PndRichGeo();
  fDigiPixelMCInfo = kFALSE;
  if (fVerbose > 0)
    Info("PndRichHitFinder", "%s created, Parameters will be taken from RTDB", name);

  fDigiArray = nullptr;
  fPdHitArray = nullptr;
  fMCEventHeader = nullptr;

  fTimeStep = 0.5; // ns
  fIBuffer = 0;
  fHitsBuffer.resize(2);
  fHitsBuffer.at(0).resize(1000);
  fHitsBuffer.at(1).resize(1000);
  petime = -1;

  //  fPixelSize = fGeo->PixelSize();	//pixel size;
  //  fNpix = fGeo->Npixels();              //pixel rows in one FE
  //  fMcpActiveArea = fGeo->McpActiveArea();
  //  fPixelGap      =  (fMcpActiveArea - (Double_t)fNpix*fPixelSize) / ((Double_t)fNpix - 1.);
  //  fPixelStep     =  fPixelSize + 0.5*fPixelGap;
}

// -----   Destructor   ----------------------------------------------------
PndRichHitFinder::~PndRichHitFinder()
{
  if (fGeo)
    delete fGeo;
  if (fGeoH)
    delete fGeoH;
}
// -------------------------------------------------------------------------

// -----   Initialization  of Parameter Containers -------------------------
void PndRichHitFinder::SetParContainers()
{
  if (fGeoH == nullptr)
    fGeoH = PndGeoHandling::Instance();
  fGeoH->SetParContainers();
  fGeoH->SetVerbose(fVerbose);
  if (fVerbose > 1)
    Info("SetParContainers", "done.");
  return;
}

InitStatus PndRichHitFinder::ReInit()
{
  SetParContainers();
  return kSUCCESS;
}

// -----   Public method Init   --------------------------------------------
InitStatus PndRichHitFinder::Init()
{
  // FairRun* ana = FairRun::Instance(); //[R.K. 01/2017] unused variable?
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    std::cout << "-E- PndRichHitFinder::Init: "
              << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  std::cout << "FairRunAna::Instance()->IsTimeStamp() = " << FairRunAna::Instance()->IsTimeStamp() << std::endl;
  if (FairRunAna::Instance()->IsTimeStamp())
    fInBranchName = "RichDigiSorted";
  else
    fInBranchName = "RichDigi";
  // Get input array
  fDigiArray = (TClonesArray *)ioman->GetObject(fInBranchName);
  if (!fDigiArray) {
    std::cout << "-W- PndRichHitFinder::Init: "
              << "No RichDigi array!" << std::endl;
    return kERROR;
  }

  // Create and register output array
  fPdHitArray = new TClonesArray("PndRichPDHit");
  ioman->Register("RichPDHit", "Rich", fPdHitArray, GetPersistency());

  fGapFunctor = new TimeGap();
  fStopFunctor = new StopTime();
  return kSUCCESS;
}

// -----   Public method Exec   --------------------------------------------
void PndRichHitFinder::Exec(Option_t *)
{

  if (fVerbose > 3)
    Info("Exec", "Start");
  if (!fPdHitArray)
    Fatal("Exec", "No PdHitArray");
  fPdHitArray->Clear();

  Double_t etime = FairRootManager::Instance()->GetEventTime();
  if (FairRunAna::Instance()->IsTimeStamp()) {
    // fDigiArray = FairRootManager::Instance()->GetData(fInBranchName, fStopFunctor, etime, fStopFunctor, etime+100);
    fDigiArray = FairRootManager::Instance()->GetData(fInBranchName, fGapFunctor, 10);

    Int_t nDigis = fDigiArray->GetEntriesFast();
    std::cout << "nDigis = " << nDigis << " " << FairRunAna::Instance()->IsTimeStamp() << std::endl;
    if (nDigis > 0) {
      Double_t timef = ((PndRichDigi *)fDigiArray->At(0))->GetTime();
      Double_t timel = ((PndRichDigi *)fDigiArray->At(nDigis - 1))->GetTime();
      fBufferNumHits.at(fIBuffer) = (timel - fBufferStartTime.at(fIBuffer)) / fTimeStep + 1;
      Double_t stopTime = fBufferStartTime.at(fIBuffer) + fBufferNumHits.at(fIBuffer) * fTimeStep;
      if ((Int_t)fHitsBuffer.at(fIBuffer).size() < fBufferNumHits.at(fIBuffer))
        fHitsBuffer.at(fIBuffer).resize(fBufferNumHits.at(fIBuffer));
      for (Int_t i = 0; i < fBufferNumHits.at(fIBuffer); i++)
        fHitsBuffer.at(fIBuffer).at(i) = 0;
      for (Int_t iDigi = 0; iDigi < nDigis; iDigi++) {
        Double_t hitTime = ((PndRichDigi *)fDigiArray->At(iDigi))->GetTime();
        Int_t ind = (hitTime - fBufferStartTime.at(fIBuffer)) / fTimeStep;
        if (hitTime > fBufferStartTime.at(fIBuffer)) {
          fHitsBuffer.at(fIBuffer).at(ind)++;
        } else {
          ind += fBufferNumHits.at(fIBufferPrev) - 1;
          fHitsBuffer.at(fIBufferPrev).at(ind)++;
        }
      }
      Int_t nbr = fBufferNumHits.at(fIBuffer);
      Int_t nbrp = fBufferNumHits.at(fIBufferPrev);
      Int_t im = nbr + nbrp - 3 * fTimeGate;
      Int_t imax_;
      Int_t nhitsmax = 0;
      std::vector<Int_t> nmax;
      std::vector<Int_t> imax;
      std::vector<Int_t> istart;
      std::vector<Int_t> istop;
      Int_t nhitsprev = 0;
      Int_t ilast;
      //  for(Int_t i = fInd; i<im; i++)
      //   {
      //      std::cout << i << " " << (i<nbrp) << " "
      //         << ( i<nbrp ?
      //              fHitsBuffer.at(fIBufferPrev).at(i) :
      //              fHitsBuffer.at(fIBuffer).at(i-nbrp) ) << std::endl;
      //   }
      for (Int_t i = fInd; i < im; i++) {
        Int_t nhits = 0;
        for (Int_t j = i - fTimeGate; j <= i + fTimeGate; j++)
          if (j >= 0)
            nhits += j < nbrp ? fHitsBuffer.at(fIBufferPrev).at(j) : fHitsBuffer.at(fIBuffer).at(j - nbrp);
        //      std::cout << i << " " << (i<nbrp) << " "
        //         << ( i<nbrp ?
        //              fHitsBuffer.at(fIBufferPrev).at(i) :
        //             fHitsBuffer.at(fIBuffer).at(i-nbrp) ) << " " << nhits << " " << nhitsmax;
        if ((nhits > fThreshold) && (nhitsprev <= fThreshold))
          istart.push_back(i);
        if ((nhits > fThreshold) && (nhits > nhitsmax)) {
          nhitsmax = nhits;
          imax_ = i;
        }
        //      std::cout << std::endl;
        if ((nhits <= fThreshold) && (nhitsprev > fThreshold)) {
          nmax.push_back(nhitsmax);
          imax.push_back(imax_);
          istop.push_back(i - 1);
          Double_t t = fBufferStartTime.at(fIBufferPrev) + imax_ * fTimeStep;
          std::cout << "peak = " << imax.size() << " " << imax_ << " " << nhitsmax << " " << istop.back() << " " << istart.back() << " " << t << std::endl;
          nhitsmax = 0;
          imax_ = -1;
        }
        if (nhits < 5)
          ilast = i;
        nhitsprev = nhits;
      }
      fInd = ilast - nbrp;

      if (fVerbose > 1)
        LOG(info) << " PndRichHitFinder: Event # " << fEventNr << " has " << nDigis << " digis.";
      else if (fVerbose == 1 && fEventNr % 1000 == 0)
        LOG(info) << " PndRichHitFinder: Event # " << fEventNr << " has " << nDigis << " digis.";
      std::cout << "-I- PndRichHitFinder: Event # " << fEventNr << " has " << nDigis << " digis.   " << FairRunAna::Instance()->IsTimeStamp() << " " << etime << " "
                << timel - timef << " " << etime << " " << timef << " " << timel << " " << petime << std::endl;

      std::cout << "buffer: " << fIBuffer << " " << fBufferNumHits.at(fIBuffer) << " " << fBufferStartTime.at(fIBuffer) << std::endl;

      Int_t detID = 0; //, mcpID = 0, pixelID = 0;
      TVector3 HitPosGlobal, HitPosLocal, dPosHit;
      Double_t hitTime = 0.;
      fHitNumber += nDigis;

      for (Int_t iDigi = 0; iDigi < nDigis; iDigi++) {
        fDigi = (PndRichDigi *)fDigiArray->At(iDigi);
        detID = -999; // fDigi->GetDetectorId();
        //    pixelID = detID - 100*(Int_t)TMath::Floor((Double_t)detID/100.);
        //    mcpID = detID/100;
        hitTime = fDigi->GetTime();
        Int_t sensorId = -999;

        // the pixel number shows local coordinates of the hit:
        //    HitPosLocal.SetXYZ(fPixelStep*((Double_t)(pixelID % fNpix) - (Double_t)(fNpix/2) + 0.5),
        //		       fPixelStep*(TMath::Floor(((Double_t)pixelID)/((Double_t)fNpix)) - (Double_t)(fNpix/2) + 0.5), 0.);
        //    Int_t sensorId = fDigi->GetSensorId()/fPixelFactor;
        //    if(fPixelFactor==2) { //double pixels
        //      sensorId++;
        //      if(fDigi->GetSensorId()%2) HitPosLocal.SetX(HitPosLocal.X()+fPixelSize/2.);
        //      else  HitPosLocal.SetX(HitPosLocal.X()-fPixelSize/2.);
        //    }

        // local coordinates of the hit on the MCP are translated into global ones as the following:
        //    HitPosGlobal = fGeoH->LocalToMasterShortId(HitPosLocal, mcpID);
        //    dPosHit.SetXYZ(fPixelSize/2., fPixelSize/2., 0.);
        //    if(fPixelFactor==2)  dPosHit.SetXYZ(fPixelSize, fPixelSize/2., 0.);

        if (fDigi->GetTimeStamp() != etime + hitTime)
          hitTime = fDigi->GetTimeStamp() - etime;
        PndRichPDHit pdhit = PndRichPDHit(iDigi, detID, sensorId, HitPosGlobal, dPosHit, hitTime, 0.);
        //    pdhit.SetPdgCode(fDigi->GetPdgCode());
        pdhit.SetTimeStamp(fDigi->GetTimeStamp());
        //    pdhit.SetTimeAtBar(fDigi->GetTimeAtBar());
        pdhit.SetLink(fDigi->GetLink(0)); // MCTrack
        pdhit.AddLink(fDigi->GetLink(1)); // RichPDPoint
        pdhit.AddLink(FairLink(-1, fEventNr, "RichDigi", iDigi));
        //((FairMultiLinkedData)pdhit).Print(); std::cout<<std::endl;
        new ((*fPdHitArray)[fPdHitArray->GetEntriesFast()]) PndRichPDHit(pdhit);
      }
      // switch between buffers
      fIBufferPrev = fIBuffer;
      fIBuffer = fIBuffer ? 0 : 1;
      fBufferStartTime.at(fIBuffer) = stopTime;
      petime = etime;
    }
  } else {
    Int_t nDigis = fDigiArray->GetEntriesFast();
    for (Int_t iDigi = 0; iDigi < nDigis; iDigi++) {
      fDigi = (PndRichDigi *)fDigiArray->At(iDigi);
      Int_t detID = -999;
      Int_t sensorId = fDigi->GetSensorId();
      Double_t hitTime = fDigi->GetTime();
      TVector3 pos = fDigi->GetPosition();
      TVector3 dpos(0, 0, 0);
      PndRichPDHit pdhit = PndRichPDHit(iDigi, detID, sensorId, pos, dpos, hitTime, 0.);
      new ((*fPdHitArray)[fPdHitArray->GetEntriesFast()]) PndRichPDHit(pdhit);
    }
    std::cout << "Event = " << fEventNr << "  nDigis = " << nDigis << " " << FairRunAna::Instance()->IsTimeStamp() << std::endl;
  }

  fEventNr++;
  if (fVerbose > 3)
    Info("Exec", "Loop MC points");

  fPdHitArray->Sort();
  fDigiArray->Delete();
}

// -------------------------------------------------------------------------
void PndRichHitFinder::FinishEvent()
{
  FinishEvents();
}

// -------------------------------------------------------------------------
void PndRichHitFinder::FinishTask()
{
  LOG(info) << " PndRichHitFinder: Total number of hits is " << fHitNumber;
}

ClassImp(PndRichHitFinder);
