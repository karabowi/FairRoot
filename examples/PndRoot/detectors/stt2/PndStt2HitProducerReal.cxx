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
 
#include "PndStt2HitProducerReal.h"

#include "PndStt2Digi.h"
//#include "PndStt2Hit.h"
#include "PndSttHit.h"
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
PndStt2HitProducerReal::PndStt2HitProducerReal() : PndPersistencyTask("Real STT2 Hit Producer", 0), 
  //fPointArray(nullptr), 
  fDigiArray(nullptr), fHitArray(nullptr), fSttDigiPar(nullptr), fDigitizer(nullptr) //, fGeoH(PndGeoHandling::Instance())
{
  SetPersistency(kTRUE);
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndStt2HitProducerReal::~PndStt2HitProducerReal() 
{
  delete fDigitizer;
}
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
InitStatus PndStt2HitProducerReal::Init()
{
  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();

  if (!ioman) {
    cout << "-E- PndStt2HitProducerReal::Init: "
         << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  // Get input array
  fDigiArray = (TClonesArray *)ioman->GetObject("STT2Digi");

  // Create and register output array
  fHitArray = new TClonesArray("PndSttHit");
  ioman->Register("STTHit", "STT", fHitArray, GetPersistency());

  // create STT geo handler with tube infos
  //fSttGeoH = new PndStt2GeoHandler(fSttGeoPar);
  fSttGeoH = PndStt2GeoHandler::Instance(fSttGeoPar); 

  // STT digitizer
  vector<string>  vsfml(6);
  vsfml[0] = fSttDigiPar->GetFcnTime2RadMean();
  vsfml[1] = fSttDigiPar->GetFcnTime2RadSigL();
  vsfml[2] = fSttDigiPar->GetFcnTime2RadSigH();
  vsfml[3] = fSttDigiPar->GetFcnRad2TimeMean();
  vsfml[4] = fSttDigiPar->GetFcnRad2TimeSigL();
  vsfml[5] = fSttDigiPar->GetFcnRad2TimeSigH();
  
  fDigitizer = new PndStt2Digitizer(vsfml);
  if (fVerbose>0) {
    cout <<"[INFO] PndStt2HitProducerReal - (t_drift -> r_iso)  mean : ";
    PrintFcn(fDigitizer->GetFcnR2Tmean(), fVerbose<2);
    cout <<"[INFO] PndStt2HitProducerReal - (t_drift -> r_iso)  siglo: ";
    PrintFcn(fDigitizer->GetFcnR2TsigL(), fVerbose<2);
    cout <<"[INFO] PndStt2HitProducerReal - (t_drift -> r_iso)  sighi: ";
    PrintFcn(fDigitizer->GetFcnR2TsigH(), fVerbose<2);
  }

  LOG(info) << " PndStt2HitProducerReal: Intialization successful";
  return kSUCCESS;
}
// -------------------------------------------------------------------------

// CHECK added
void PndStt2HitProducerReal::SetParContainers()
{
  FairRuntimeDb *rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fSttDigiPar = (PndStt2DigiPar *)rtdb->getContainer("PndStt2DigiPar");
  fSttGeoPar  = (PndGeoSttPar*) rtdb->getContainer("PndGeoSttPar");  
  //fSttGeoPar  = (PndStt2GeoPar*) rtdb->getContainer("PndStt2GeoPar");  
}

// -----   Public method Exec   --------------------------------------------
void PndStt2HitProducerReal::Exec(Option_t *)
{
  // Reset output array
  if (!fHitArray)
    Fatal("Exec", "No HitArray");

  fHitArray->Delete();

  // Loop over SttPoints
  Int_t nDigis = fDigiArray->GetEntriesFast();
  Int_t counter = 0;
  
  //cout <<"ndigis="<<nDigis<<"  "<<flush;
  
  for (Int_t iDigi = 0; iDigi < nDigis; iDigi++) {
    PndStt2Digi *digi = (PndStt2Digi *)fDigiArray->At(iDigi);

    if (!digi) continue;
    
    // MC index
    int McIdx = digi->GetMcIndex();
    
    // Detector ID
    int detID = digi->GetDetID();

    // tubeID  CHECK added
    Int_t tubeID = digi->GetTubeID();
    //PndStt2Tube *tube = fSttGeoH->GetTube(tubeID);
    PndSttTube *tube = fSttGeoH->GetTube(tubeID);
    TVector3 tubePos = tube->GetPosition();
    double tlen = 2*tube->GetHalfLength();
    double tdia = 2*tube->GetRadOut();
    
    // true dist
    Double_t d2w = digi->GetDist2Wire();
    
    // isochrone radius and uncertainties
    Double_t thit   = digi->GetHitTime();
    Double_t tdrift = digi->GetDriftTime();
    Double_t resfac = fSttDigiPar->GetResFactRad();
    
    vector<Double_t> vriso = fDigitizer->IsochroneRadius(tdrift, resfac);
    Double_t riso   = vriso[0]/10.; // isochrone radius (mm -> cm)
    Double_t risoel = vriso[1]/10.; // isochrone radius error low (mm -> cm)
    Double_t risoeh = vriso[2]/10.; // isochrone radius error high (mm -> cm)
    
    if (riso<0) riso = 0.0;
    if (riso>tdia/2.) riso = tdia/2.;
    
    TVector3 pos(tubePos.X(), tubePos.Y(), tubePos.Z()); 
    TVector3 dpos(tdia/sqrt(12), tdia/sqrt(12.), tlen/sqrt(12.));
    
    //PndStt2Hit(Int_t mcindex, Int_t detID, Int_t tubeID, TVector3 &pos, TVector3 &dpos, Double_t p, Double_t dist2wire, Double_t isorad, Double_t isoraderrlo, Double_t isoraderrhi, Double_t chDep);
    //new ((*fHitArray)[counter]) PndStt2Hit(McIdx, detID, tubeID, pos, dpos, thit, d2w, riso, risoel, risoeh, digi->GetEloss());
    new ((*fHitArray)[counter]) PndSttHit(detID, tubeID, McIdx, pos, dpos, 0, riso, 0.5*(risoel+risoeh), digi->GetEloss()*1e6);
    
    PndSttHit *hit = (PndSttHit*) fHitArray->At(counter);
    hit->SetHitTime(thit);
    hit->SetIsochroneErrorLo(risoel);
    hit->SetIsochroneErrorHi(risoeh);
    hit->SetDist2Wire(d2w);
    
    counter++;
    
  } // Loop over Digis

  // Event summary
  if (fVerbose > 1)
    LOG(info) << " PndStt2HitProducerReal: " << nDigis << " SttDigis, " << counter << " Hits created.";
}

ClassImp(PndStt2HitProducerReal)
