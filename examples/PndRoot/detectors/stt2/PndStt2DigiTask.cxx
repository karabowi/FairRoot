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
 
#include "PndStt2DigiTask.h"

#include "PndStt2Point.h"
#include "PndStt2Digi.h"
#include "PndStt2Digitizer.h"
#include "PndStt2GeoHandler.h"
#include "PndSttTube.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"

#include "TRandom.h"
#include "TF1.h"

#include <iostream>
#include <cmath>

using std::cout;
using std::endl;
using std::sqrt;
using std::string;

#include "pndtools.h"
using pndtools::PrintFcn;

// -----   Default constructor   -------------------------------------------
PndStt2DigiTask::PndStt2DigiTask() : PndPersistencyTask("Ideal STT2 Digi Producer", 0), 
  fPointArray(nullptr), fDigiArray(nullptr), fSttDigiPar(nullptr), fSttGeoH(0) //fGeoH(PndGeoHandling::Instance())
{
  SetPersistency(kTRUE);
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndStt2DigiTask::~PndStt2DigiTask() {}
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
InitStatus PndStt2DigiTask::Init()
{
  // ##### Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();

  if (!ioman) {
    cout << "-E- PndStt2DigiTask::Init: "
         << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  // ##### Get input array
  fPointArray = (TClonesArray *)ioman->GetObject("STT2Point");

  if (!fPointArray) {
    cout << "-W- PndStt2DigiTask::Init: "
         << "No STT2Point array!" << endl;
    return kERROR;
  }

  // ##### Create and register output array
  fDigiArray = new TClonesArray("PndStt2Digi");
  ioman->Register("STT2Digi", "STT", fDigiArray, GetPersistency());

  // ##### STT digitizer
  vector<string> vsfml(6);
  vsfml[0] = fSttDigiPar->GetFcnTime2RadMean();
  vsfml[1] = fSttDigiPar->GetFcnTime2RadSigL();
  vsfml[2] = fSttDigiPar->GetFcnTime2RadSigH();
  vsfml[3] = fSttDigiPar->GetFcnRad2TimeMean();
  vsfml[4] = fSttDigiPar->GetFcnRad2TimeSigL();
  vsfml[5] = fSttDigiPar->GetFcnRad2TimeSigH();
  
  fDigitizer = new PndStt2Digitizer(vsfml);
  
  if (fVerbose>0) {
    cout <<"[INFO] PndStt2DigiTask - (d -> t_drift)  mean : ";
    PrintFcn(fDigitizer->GetFcnT2Rmean(), fVerbose<2);
    cout <<"[INFO] PndStt2DigiTask - (d -> t_drift)  siglo: ";
    PrintFcn(fDigitizer->GetFcnT2RsigL(), fVerbose<2);
    cout <<"[INFO] PndStt2DigiTask - (d -> t_drift)  sighi: ";
    PrintFcn(fDigitizer->GetFcnT2RsigH(), fVerbose<2);
  }

  // ##### create STT geo handler with tube infos
  //fSttGeoH = new PndStt2GeoHandler(fSttGeoPar);
  fSttGeoH = PndStt2GeoHandler::Instance(fSttGeoPar); 
  if (fVerbose>1) 
    fSttGeoH->CheckTubeMap(5,5);
  
  LOG(info) << "PndStt2DigiTask: Initialization successfull";
  return kSUCCESS;
}
// -------------------------------------------------------------------------

// CHECK added
void PndStt2DigiTask::SetParContainers()
{
  FairRuntimeDb *rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fSttDigiPar = (PndStt2DigiPar*) rtdb->getContainer("PndStt2DigiPar");  
  fSttGeoPar  = (PndGeoSttPar*) rtdb->getContainer("PndGeoSttPar");  
}

// -----   Public method Exec   --------------------------------------------
void PndStt2DigiTask::Exec(Option_t *)
{
  // Reset output array
  if (!fDigiArray)
    Fatal("Exec", "No HitArray");

  fDigiArray->Delete();

  // Loop over SttPoints
  Int_t nPoints = fPointArray->GetEntriesFast();
  Int_t counter = 0;
  
  for (Int_t iPoint = 0; iPoint < nPoints; iPoint++) {
    
    PndStt2Point *point = (PndStt2Point *)fPointArray->At(iPoint);
    if (!point) continue;

    // Detector ID
    Int_t detID = point->GetDetectorID();

    // tubeID  
    Int_t tubeID = point->GetTubeID();
    //PndStt2Tube *tube = fSttGeoH->GetTube(tubeID);
    PndSttTube *tube = fSttGeoH->GetTube(tubeID);
    
    TVector3 gPosIn(point->GetX(), point->GetY(), point->GetZ());             // global entry coordinate 
    TVector3 gPosOut(point->GetXOut(), point->GetYOut(), point->GetZOut());   // global exit coordinate 

    Double_t dist2wire = GetDistToWire(tubeID, gPosIn, gPosOut);
     
    // ##### transform distance to time signal
    TVector3 hitpos   = tube->GetPositionAtZ(0.5*(gPosIn.Z() + gPosOut.Z())); // coordinate of hit on wire
    TVector3 readout  = fSttGeoH->GetTube(tubeID)->GetWireUpStr();            // readout end of wire
    Double_t sigdist  = (hitpos-readout).Mag();
    
    //printf("%4d: avg(%5.1f, %5.1f, %5.1f) / atz(%5.1f, %5.1f, %5.1f)\n", 
        //iPoint, hitpos.X(), hitpos.Y(), hitpos.Z(), hitpos1.X(), hitpos1.Y(), hitpos1.Z());
    
    // ##### for short tubes: check whether the up stream or down stream tube has been hit
    // ##### for downstream tubes, we need to add the length of the conn. up stream tube for signal runtime
    if (tube->GetConnected()>0) {
      //PndStt2Tube *contube = fSttGeoH->GetTube(tube->GetConnected());
      PndSttTube *contube = fSttGeoH->GetTube(tube->GetConnected());
      if (contube->GetWireUpStr().Z()<tube->GetWireUpStr().Z())
        sigdist += contube->GetHalfLength()*2;
    }
     
    //Double_t zhit     = 0.5*(gPosIn.Z() + gPosOut.Z());
    //Double_t zplane   = fSttDigiPar->GetReadoutZ();
    Double_t vsignal  = fSttDigiPar->GetSignalSpeed();    
    Double_t resfac   = fSttDigiPar->GetResFactTime();
    Double_t tdrift   = fDigitizer->DriftTime(dist2wire*10, resfac);
    Double_t tflight  = point->GetTime();
    //Double_t tsignal  = (zhit - zplane)/vsignal;
    Double_t tsignal  = sigdist/vsignal;
    Double_t thit     = tflight + tdrift + tsignal;
    //printf("tf = %4.1f   td = %5.1f   ts = %4.1f  (z=%5.1f, d=%5.1f)\n", tflight, tdrift, tsignal, hitpos.Z(), sigdist);
     
    new ((*fDigiArray)[counter]) PndStt2Digi(iPoint, detID, tubeID, point->GetEnergyLoss(), tdrift, thit, dist2wire, point->GetTime());
    
    counter++;
  } // Loop over MCPoints
  
  //cout <<endl;

  // Event summary
  if (fVerbose > 1)
    LOG(info) << " PndStt2DigiTask: " << nPoints << " SttPoints, " << counter << " Hits created.";
}

// -------------------------------------------------------------------------
// -----   Private method GetClostestApproachToWire ------------------------
// -------------------------------------------------------------------------
Double_t PndStt2DigiTask::GetDistToWire(Int_t tubeID, TVector3 gPosIn, TVector3 gPosOut )
{
  //PndStt2Tube *tube = fSttGeoH->GetTube(tubeID);
  PndSttTube *tube = fSttGeoH->GetTube(tubeID);
  if (tube==nullptr) return -1.;
  
  TVector3 p0 = tube->GetWireUpStr(), d0 = tube->GetWireDownStr() - p0;
  TVector3 p1 = gPosIn, d1 = gPosOut - p1;
  
  // ##### distance of lines g(s)=p0+s*d0, h(r)=p1+r*d1: d = |q*n|/|n| with q=p1-p0, n=d0 x d1
  TVector3 q = p1-p0, n = d0.Cross(d1);
  return fabs(q*n)/n.Mag();
}

ClassImp(PndStt2DigiTask)
