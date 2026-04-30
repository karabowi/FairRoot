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
 
#include "PndStt2HitProducerIdeal.h"

#include "PndStt2Point.h"
#include "PndSttHit.h"
//#include "PndStt2Hit.h"
#include "PndStt2GeoHandler.h"
//#include "PndStt2Tube.h"
#include "PndSttTube.h"
#include "PndStt2DigiPar.h"
#include "PndStt2Digitizer.h"
#include "PndGeoSttPar.h"
//#include "PndStt2GeoPar.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"

#include "TClonesArray.h"
#include "TRandom.h"

#include <iostream>
#include <cmath>

using std::cout;
using std::endl;
using std::sqrt;
using std::string;

#include "pndtools.h"
using pndtools::PrintFcn;
//using namespace pndtools;

// TODO: read this from geant initialization
//#define innerStrawDiameter 1.

// -----   Default constructor   -------------------------------------------
PndStt2HitProducerIdeal::PndStt2HitProducerIdeal() : PndPersistencyTask("Ideal STT2 Hit Producer", 0), 
  //fPointArray(nullptr), 
  fPointArray(nullptr), fHitArray(nullptr), fSttDigiPar(nullptr), fResIsoIdeal(0.) //, fGeoH(PndGeoHandling::Instance())
{
  SetPersistency(kTRUE);
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndStt2HitProducerIdeal::~PndStt2HitProducerIdeal() 
{
  //delete fDigitizer;
}
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
InitStatus PndStt2HitProducerIdeal::Init()
{
  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();

  if (!ioman) {
    cout << "-E- PndStt2HitProducerIdeal::Init: "
         << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  // Get input array
  fPointArray = (TClonesArray *)ioman->GetObject("STT2Point");

  // Create and register output array
  fHitArray = new TClonesArray("PndSttHit");
  ioman->Register("STTHit", "STT", fHitArray, GetPersistency());
  
  // create STT geo handler with tube infos
  //fSttGeoH = new PndStt2GeoHandler(fSttGeoPar);
  fSttGeoH = PndStt2GeoHandler::Instance(fSttGeoPar); 

  // absolute resolution of isochrone radius in [cm]
  fResIsoIdeal = fSttDigiPar->GetResIsoIdeal();
  LOG(info) << Form(" PndStt2HitProducerIdeal: ideal isochrone resolution = %f", fResIsoIdeal);

  LOG(info) << " PndStt2HitProducerIdeal: Intialization successful";
  return kSUCCESS;
}
// -------------------------------------------------------------------------

// CHECK added
void PndStt2HitProducerIdeal::SetParContainers()
{
  FairRuntimeDb *rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fSttDigiPar = (PndStt2DigiPar *)rtdb->getContainer("PndStt2DigiPar");  
  fSttGeoPar  = (PndGeoSttPar*) rtdb->getContainer("PndGeoSttPar");  
  //fSttGeoPar  = (PndStt2GeoPar*) rtdb->getContainer("PndStt2GeoPar");  
}

// -----   Public method Exec   --------------------------------------------
void PndStt2HitProducerIdeal::Exec(Option_t *)
{
  // Reset output array
  if (!fHitArray)
    Fatal("Exec", "No HitArray");

  fHitArray->Delete();

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
    TVector3 tubePos = tube->GetPosition();
    double tlen = 2*tube->GetHalfLength();
    double tdia = 2*tube->GetRadOut();
    
    TVector3 gPosIn(point->GetX(), point->GetY(), point->GetZ());             // global entry coordinate 
    TVector3 gPosOut(point->GetXOut(), point->GetYOut(), point->GetZOut());   // global exit coordinate 

    Double_t d2w = GetDistToWire(tubeID, gPosIn, gPosOut);

    //Int_t tubeID = point->GetTubeID();
    //PndStt2Tube *tube = fSttGeoH->GetTube(tubeID);
    
    //TVector3 gPosIn(point->GetX(), point->GetY(), point->GetZ());            // global entry coordinate 
    //TVector3 gPosOut(point->GetXOut(), point->GetYOut(), point->GetZOut());  // global exit coordinate 
    
    //TVector3 lPosIn  = fGeoH->MasterToLocalShortId(gPosIn,  tubeID);         // tube local entry coordinate 
    //TVector3 lPosOut = fGeoH->MasterToLocalShortId(gPosOut, tubeID);         // tube local exit coordinate
    
    // determine track distance to wire (assumed straight line inside tube)
    //Double_t d2w = GetDistToWire(lPosIn, lPosOut);
    
    Double_t riso = -1;
    while (riso<0 || riso>tdia/2) riso = fResIsoIdeal>0 ? gRandom->Gaus(d2w, fResIsoIdeal) : d2w;
        
    TVector3 pos(tubePos.X(), tubePos.Y(), tubePos.Z()); 
    TVector3 dpos(tdia/sqrt(12), tdia/sqrt(12.), tlen/sqrt(12.));
    //TVector3 dpos(tdia/2., tdia/2., 1);

    //PndStt2Hit(Int_t mcindex, Int_t detID, Int_t tubeID, TVector3 &pos, TVector3 &dpos, Double_t p, Double_t dist2wire, Double_t isorad, Double_t isoraderrlo, Double_t isoraderrhi, Double_t chDep);
    //new ((*fHitArray)[counter]) PndStt2Hit(iPoint, detID, tubeID, pos, dpos, 0, d2w, riso, fResIsoIdeal, fResIsoIdeal,  point->GetEnergyLoss());
    //new ((*fHitArray)[counter]) PndStt2Hit(iPoint, detID, tubeID, pos, dpos, 0, d2w, riso, fResIsoIdeal, fResIsoIdeal,  point->GetEnergyLoss());
    new ((*fHitArray)[counter]) PndSttHit(detID, tubeID, iPoint, pos, dpos, 0, riso, fResIsoIdeal, point->GetEnergyLoss() * 1e6);
    
    PndSttHit *hit = (PndSttHit*) fHitArray->At(counter);
    hit->SetIsochroneErrorLo(fResIsoIdeal);
    hit->SetIsochroneErrorHi(fResIsoIdeal);
    hit->SetDist2Wire(d2w);
   
    counter++;
    
  } // Loop over Digis

  // Event summary
  if (fVerbose > 1)
    LOG(info) << " PndStt2HitProducerIdeal: " << nPoints << " SttPoints, " << counter << " Hits created.";
}


// -------------------------------------------------------------------------
// -----   Private method GetClostestApproachToWire ------------------------
// -------------------------------------------------------------------------
Double_t PndStt2HitProducerIdeal::GetDistToWire(Int_t tubeID, TVector3 gPosIn, TVector3 gPosOut )
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

//// -----   Private method GetClostestApproachToWire ------------------------
//Double_t PndStt2HitProducerIdeal::GetDistToWire(TVector3 lPosIn, TVector3 lPosOut)
//{
  //TVector3 x1(0., 0., -75.); // CHECK
  //TVector3 x2(0., 0., 75.);  // CHECK
  //TVector3 x3 = lPosIn;  //(fX_in_local, fY_in_local, fZ_in_local);
  //TVector3 x4 = lPosOut; //(fX_out_local, fY_out_local, fZ_out_local);

  //return fabs((x3 - x1).Dot((x2 - x1).Cross(x4 - x3))) / fabs(((x2 - x1).Cross(x4 - x3)).Mag());
//}


ClassImp(PndStt2HitProducerIdeal)
