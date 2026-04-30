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
// -----                  PndRichHitProducer source file               -----
// -----                  Created 11/06/08  by  S.Spataro              -----
// -------------------------------------------------------------------------

#include <PndPersistencyTask.h>
#include "PndRichHitProducer.h"

#include "PndRichDigi.h"
#include "PndRichHit.h"
#include "PndRichPDHit.h"
#include "PndRichPDPoint.h"
#include "PndRichBarPoint.h"

#include "FairRootManager.h"
#include "FairDetector.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairEventHeader.h"
#include "FairLogger.h"

#include "PndRichHitWriteoutBuffer.h"

#include "TClonesArray.h"
#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TGeoNode.h"
#include "TGeoMatrix.h"
#include "TVector3.h"
#include "TRandom.h"

#include <iostream>
#include <iomanip>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndRichHitProducer::PndRichHitProducer() : PndPersistencyTask("Rich Hit Producer")
{
  fPosResolution = -1.;
  fGeoVersion = 313;
  fPhDetNoise = kFALSE;
  fNumRand = 0;
  SetPersistency(kTRUE);
  fTimeOrderedDigi = kFALSE;
  fPreviousEventTime = -1;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndRichHitProducer::~PndRichHitProducer() {}
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
InitStatus PndRichHitProducer::Init()
{

  cout << "-I- PndRichHitProducer::Init: "
       << "INITIALIZATION *********************" << endl;

  // FairRun* sim = FairRun::Instance(); //[R.K. 01/2017] unused variable?
  // FairRuntimeDb* rtdb=sim->GetRuntimeDb(); //[R.K. 01/2017] unused variable?

  fGeo = new PndRichGeo();
  fGeo->init(fGeoVersion);
  cout << "-I- PndRichHitProducer::Init: "
       << "fGeoVersion = " << fGeoVersion << endl;

  cout << "-I- PndRichHitProducer::Init: ";
  if (fPhDetNoise)
    cout << "Photodetector noise is switched on! " << endl;
  else
    cout << "Photodetector noise is switched off! " << endl;

  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    cout << "-E- PndRichHitProducer::Init: "
         << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  // Get input array
  fPDPointArray = (TClonesArray *)ioman->GetObject("RichPDPoint");
  if (!fPDPointArray) {
    cout << "-W- PndRichHitProducer::Init: "
         << "No RichPDPoint array!" << endl;
    return kERROR;
  }

  fBarPointArray = (TClonesArray *)ioman->GetObject("RichBarPoint");
  if (!fBarPointArray) {
    cout << "-W- PndRichHitProducer::Init: "
         << "No RichBarPoint array!" << endl;
    return kERROR;
  }

  // Create and register output array
  fDataBuffer = new PndRichHitWriteoutBuffer("RichDigi", "PndRich", GetPersistency());
  fDataBuffer = (PndRichHitWriteoutBuffer *)ioman->RegisterWriteoutBuffer("RichDigi", fDataBuffer);
  fDataBuffer->ActivateBuffering(fTimeOrderedDigi);

  fHitArray = new TClonesArray("PndRichHit");
  ioman->Register("RichHit", "PndRich", fHitArray, kTRUE);

  if (fPosResolution > 0.)
    cout << "-I- PndRichHitProducer::Init: "
         << "Hit Position smearing: " << fPosResolution << " [cm]" << endl;

  LOG(info) << " PndRichHitProducer: Intialization successfull";

  fRichResolution = new PndRichResolution();

  /*   for(UInt_t i=0;i<1e5;i++)
     {
        Double_t xl = 2*gRandom->Uniform()*(fGeo->phDetSizeX() + fGeo->phDetGapX());
        Double_t yl = 2*gRandom->Uniform()*(fGeo->phDetSizeY() + fGeo->phDetGapY());
        TVector3 posg = fGeo->PhDetPositionGlobal(TVector3(xl,yl,0.0));
        TVector3 pos = fGeo->PositionDiscretization(posg);
        TVector3 posl = pos.Z() ? fGeo->PhDetPositionLocal(pos) : TVector3(0,0,0);
        std::cout << "PndRichGeo:  " <<
  //         format("%15.6 %15.6 %15.6 %15.6 %15.6\n") % xl % yl % posl.X() % posl.Y() % pos.Z();
           std::setprecision(8) << xl << " " << yl <<
           " " << posl.X() << " " << posl.Y() << " " << pos.Z() << std::endl;
     }
  */
  /*   Double_t xmax = (fGeo->phDetSizeX() + fGeo->phDetGapX())*2;// *fGeo->phDetNumX();
     Double_t ymax = (fGeo->phDetSizeY() + fGeo->phDetGapY())*2;// *fGeo->phDetNumY();
     for(Double_t x=-xmax;x<xmax;x+=0.1)
     {
        for(Double_t y=-ymax;y<ymax;y+=0.1)
        {
           TVector3 pos = fGeo->PhDetPositionGlobal(TVector3(x,y,0));
           Int_t ix = fGeo->IndexX(pos);
           Int_t iy = fGeo->IndexY(pos);
           TVector3 posl = fGeo->PhDetPositionLocal(fGeo->PixelPosition(ix,iy));
           std::cout << "PndRichGeo:  " <<
              std::setprecision(8) << ix << " " << iy << " " << x << " " << y <<
              " " << posl.X() << " " << posl.Y() << " " << pos.Z() << std::endl;
        }
     }*/
  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndRichHitProducer::Exec(Option_t *)
{

  fEventTime = FairRootManager::Instance()->GetEventTime();
  std::cout << "FairRootManager::Instance()->GetEventTime() = " << fEventTime << " " << fPreviousEventTime << " " << fGeo->sensorsPerDevice() << std::endl;

  // Reset output array
  // if ( ! fPDHitArray ) Fatal("Exec", "No HitArray");

  // if (!fTimeOrderedDigi) fPDHitArray->Clear();

  // pixels of PhDet
  UInt_t iXmax = fGeo->phDetNPixelMaxX();
  UInt_t iYmax = fGeo->phDetNPixelMaxY();
  /*   UInt_t map[iXmax][iYmax];
     for (UInt_t ix=0; ix<iXmax; ix++)
        for (UInt_t iy=0; iy<iYmax; iy++)
           map[ix][iy] = 0;
    */
  // Loop over RichPDpoints
  Int_t nPoints = fPDPointArray->GetEntriesFast();
  PndRichPDPoint *point = 0;
  TVector3 pos, mom;
  TVector3 sig(fPosResolution, fPosResolution, fPosResolution);
  //   Double_t k = 2*3.1415927*197.3269602e-9;

  for (Int_t iPoint = 0; iPoint < nPoints; iPoint++) {
    point = (PndRichPDPoint *)fPDPointArray->At(iPoint);
    point->Momentum(mom);
    //     if ( gRandom->Uniform() < fGeo->phDetQEff(k/mom.Mag()) ) {
    point->Position(pos);
    TVector3 posd = fGeo->PositionDiscretization(pos);
    if (posd.Z()) {
      Double_t t = gRandom->Gaus(point->GetTime(), 0.5); // ns
      /*           if (fPhDetNoise) { // add noise
                    std::vector<Double_t> tn = PhDetNoise();
                    for (UInt_t i=0; i<tn.size(); i++)
                       if (t-tn.at(i)<720&&t>tn.at(i)) t = tn.at(i);
                 }*/
      AddDigi(point->GetDetectorID(), fGeo->sensorIndex(), posd, sig, iPoint, t);
      //           UInt_t ix = fGeo->IndexX(posd);
      //           UInt_t iy = fGeo->IndexY(posd);
      //           if ( ix<iXmax && iy<iYmax )
      //              map[ix][iy] = 1;
    }
    //     }
  } // Loop over MCPoints
  if (fPhDetNoise) {
    /*      for (UInt_t ix=0; ix<iXmax; ix++)
             for (UInt_t iy=0; iy<iYmax; iy++)
                if (!map[ix][iy]) {
                   std::vector<Double_t> tn = PhDetNoise();
                   if (tn.size()&&tn.back()>-50) {
                      pos = fGeo->PixelPosition(ix,iy);
                      AddDigi(0, fGeo->sensorIndex(), pos, sig, 0, tn.back() );
                   }
                }*/
    Int_t kcell = 8 / fGeo->sensorsPerDevice();                            // in one direction
    Double_t fnoise = 1e3 * 1e-9 * kcell * kcell;                          // GHz(=1/ns) per pixel
    Double_t dt = fTimeOrderedDigi ? fEventTime - fPreviousEventTime : 50; // ns
    Int_t nn = gRandom->Poisson(iXmax * iYmax * fnoise * dt);              // number of fired pixels
    std::cout << "nn = " << nn << " " << iXmax << " " << iYmax << " " << kcell << std::endl;
    for (Int_t i = 0; i < nn; i++) {
      Int_t gind = gRandom->Integer(iXmax * iYmax);
      Int_t ix = gind % iXmax;
      Int_t iy = gind / iXmax;
      pos = fGeo->PixelPosition(ix, iy);
      Double_t t = (fTimeOrderedDigi ? 0 : 25) - dt * gRandom->Uniform();
      AddDigi(0, fGeo->sensorIndex(), pos, sig, 0, t);
    }
  }

  //   if (!fTimeOrderedDigi)
  {
    fHitArray->Clear();

    // Loop over RichBarPoints
    Int_t nBarPoints = fBarPointArray->GetEntriesFast();
    PndRichBarPoint *hit = 0;

    Int_t detID = -999;    //[R.K.Jan/2017] Variable was not initialized!
    Int_t sensorId = -999; //[R.K.Jan/2017] Variable was not initialized!
    //, index; //[R.K. 01/2017] unused variable?
    TVector3 dpos(0., 0., 0.);
    // thetaC containe value of beta
    Double_t thetaC, errThetaC;
    dbpoint pnt;
    for (Int_t iBarPoint = 0; iBarPoint < nBarPoints; iBarPoint++) {
      hit = (PndRichBarPoint *)fBarPointArray->At(iBarPoint);
      hit->Position(pos);
      hit->Momentum(mom);

      pnt.mass = hit->GetMass();
      pnt.beta = mom.Mag() / sqrt(mom.Mag() * mom.Mag() + pnt.mass * pnt.mass);
      pnt.x = pos.X();
      pnt.y = pos.Y();
      pnt.theta = mom.Theta();
      pnt.phi = mom.Phi();

      if (gRandom->Uniform() < fRichResolution->Efficiency(pnt)) { // efficiency of reconstruction
        errThetaC = fRichResolution->Sigma(pnt);                   // sigma of beta
        thetaC = gRandom->Gaus(hit->GetThetaC(), errThetaC);
        AddHit(detID, sensorId, pos, dpos, thetaC, errThetaC, iBarPoint);
      }
    } // Loop over MCPoints
  }
  fPreviousEventTime = FairRootManager::Instance()->GetEventTime();
}
// -------------------------------------------------------------------------

std::vector<Double_t> PndRichHitProducer::PhDetNoise()
{
  std::vector<Double_t> tn;
  Double_t td = 720;    // ns
  Double_t fn = 0.25e5; // Hz
  Double_t ts = -2000;  // start time of noise simulation, in ns
  Double_t tstop = 50;
  Double_t t = 0;
  while (ts < tstop) {
    t += gRandom->Exp(1e9 / fn - td);
    fNumRand++;
    if (t > td) {
      ts += t;
      t = 0;
      if (ts < tstop)
        tn.push_back(ts);
    }
  }
  return tn;
}

// -----   Private method AddHit   --------------------------------------------
void PndRichHitProducer::AddXPDHit(Int_t detID, Int_t sensorId, TVector3 &pos, TVector3 &dpos, Int_t index, Double_t time)
{
  if (fTimeOrderedDigi)
    AddDigi(detID, sensorId, pos, dpos, index, time);
  else
    AddPDHit(detID, sensorId, pos, dpos, index, time);
  // AddDigi(detID,sensorId,pos,dpos,index,time);
}

PndRichDigi *PndRichHitProducer::AddDigi(Int_t detID, Int_t sensorId, TVector3 &pos, TVector3 &dpos, Int_t index, Double_t time)
{
  Double_t EventTime = FairRootManager::Instance()->GetEventTime();
  fDeadTime = 100; // ns;

  Double_t timeThreshold = -999; //[R.K.Jan/2017] Variable was not initialized!
  PndRichDigi *hitnew = new PndRichDigi(index, detID, sensorId, pos, dpos, time + EventTime, timeThreshold, EventTime + time);
  if (fTimeOrderedDigi) {
    hitnew->ResetLinks();
    FairEventHeader *evtHeader = (FairEventHeader *)FairRootManager::Instance()->GetObject("EventHeader.");
    hitnew->AddLink(FairLink(evtHeader->GetInputFileId(), evtHeader->GetMCEntryNumber() - 1, "RichPDPoint", index));
    hitnew->AddLink(FairLink(-1, FairRootManager::Instance()->GetEntryNr(), "EventHeader.", -1));
    PndRichPDPoint *point = (PndRichPDPoint *)fPDPointArray->At(index);
    if (point)
      hitnew->AddLinks(*(point->GetPointerToLinks()));
  }
  fDataBuffer->FillNewData(hitnew, EventTime + time, EventTime + time + fDeadTime);
  return hitnew;
}
// ----

PndRichPDHit *PndRichHitProducer::AddPDHit(Int_t detID, Int_t sensorId, TVector3 &pos, TVector3 &dpos, Int_t index, Double_t time)
{
  // It fills the PndRichPDHit category

  Double_t timeThreshold = -999; //[R.K.Jan/2017] Variable was not initialized!
  TClonesArray &clref = *fPDHitArray;
  Int_t size = clref.GetEntriesFast();
  return new (clref[size]) PndRichPDHit(index, detID, sensorId, pos, dpos, time, timeThreshold);
}
// ----

// -----   Private method AddHit   --------------------------------------------
PndRichHit *PndRichHitProducer::AddHit(Int_t detID, Int_t sensorId, TVector3 &pos, TVector3 &dpos, Double_t thetaC, Double_t errThetaC, Int_t index)
{

  TClonesArray &clref = *fHitArray;
  Int_t size = clref.GetEntriesFast();
  return new (clref[size]) PndRichHit(detID, sensorId, pos, dpos, thetaC, errThetaC, index);
}
// ----

void PndRichHitProducer::FinishEvent() {}

void PndRichHitProducer::FinishTask() {}

ClassImp(PndRichHitProducer)
