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

/////////////////////////////////////////////////////////////
//
//  PndEmcApdHitProducer
//
//  Filler of PndEmcApdHit
//
//  Created 14/08/06  by S.Spataro
//
///////////////////////////////////////////////////////////////

#include "PndEmcApdHitProducer.h"

#include "PndEmcStructure.h"
#include "PndEmcMapper.h"
#include "PndEmcApdHit.h"
#include "PndEmcApdPoint.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairDetector.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"

#include "TClonesArray.h"
#include "TROOT.h"
#include "TGeoVolume.h"
#include "TGeoMatrix.h"
#include "TVector3.h"

#include <string>

using std::cout;
using std::endl;
using std::map;

// -----   Default constructor   -------------------------------------------
PndEmcApdHitProducer::PndEmcApdHitProducer()
  : PndPersistencyTask("Ideal EMC APD hit Producer"), fPointArray(nullptr), fDigiArray(nullptr), fVolumeArray(new TObjArray), fMapVersion(0), emcX(), emcY(), emcZ(), fEmcStr()
{
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndEmcApdHitProducer::~PndEmcApdHitProducer()
{
  delete fEmcStr;
}
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
InitStatus PndEmcApdHitProducer::Init()
{

  cout << " INITIALIZATION *********************" << endl;

  // FairDetector::Initialize();
  // FairRun* sim = FairRun::Instance();
  // FairRuntimeDb* rtdb=sim->GetRuntimeDb();

  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    cout << "-E- PndEmcApdHitProducer::Init: "
         << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  // Get input array
  fPointArray = (TClonesArray *)ioman->GetObject("EmcApdPoint");
  if (!fPointArray) {
    cout << "-W- PndEmcApdHitProducer::Init: "
         << "No EmcApdPoint array!" << endl;
    return kERROR;
  }

  // Create and register output array
  fDigiArray = new TClonesArray("PndEmcApdHit");

  ioman->Register("EmcApdHit", "Emc", fDigiArray, GetPersistency());
 
  // Geometry loading
  // 	TFile *infile = ioman->GetInFile();
  // 	TGeoManager *geoMan = (TGeoManager*) infile->Get("FAIRGeom");
  //	TGeoManager *geoMan = (TGeoManager*) gROOT->FindObject("FAIRGeom");

  // fMapVersion=1;
  // PndEmcMapper::Init(fMapVersion);
  fEmcStr = PndEmcStructure::Instance();

  emcX = fEmcStr->GetEmcX();
  emcY = fEmcStr->GetEmcY();
  emcZ = fEmcStr->GetEmcZ();
  ;

  LOG(info) << " PndEmcApdHitProducer: Intialization successfull";

  return kSUCCESS;
}

// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndEmcApdHitProducer::Exec(Option_t *)
{
  // cout << " DIGI EXECUTION *********************" << endl;
  // Reset output array
  if (!fDigiArray)
    Fatal("Exec", "No DigiArray");
  fDigiArray->Delete();

  // Declare some variables
  PndEmcApdPoint *point = nullptr;
  map<Int_t, Float_t> fTrackEnergy;
  map<Int_t, Float_t> fTrackTime;  // time of first point
  map<Int_t, TArrayI> fPointIndex; // index of MC points

  fTrackEnergy.clear();
  fTrackTime.clear();
  fPointIndex.clear();

  map<Int_t, Float_t>::const_iterator p;

  std::vector<PndEmcApdPoint *> fPointList; // to pass to EmcApdHit

  // Loop over EmcApdPoints
  Int_t nPoints = fPointArray->GetEntriesFast();

  // Double_t point_time; //[R.K. 01/2017] unused variable
  for (Int_t iPoint = 0; iPoint < nPoints; iPoint++) {
    point = (PndEmcApdPoint *)fPointArray->At(iPoint);
    fTrackEnergy[point->GetDetectorID()] += point->GetEnergyLoss();
    // point_time=point ->GetTime(); //[R.K. 01/2017] unused variable
    if (point->GetFlag() == -1) {
      fPointIndex[point->GetDetectorID()].Set(fPointIndex[point->GetDetectorID()].GetSize() + 1);
      fPointIndex[point->GetDetectorID()].AddAt(point->GetTrackID(), fPointIndex[point->GetDetectorID()].GetSize() - 1);
    }
  }
  // Loop over EmcApdPoint

  // Loop to register EmcApdHit
  for (p = fTrackEnergy.begin(); p != fTrackEnergy.end(); ++p) {
    if ((*p).second > 0.) {
      Int_t index[10] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
      for (Int_t ii = 0; ii < (fPointIndex[(*p).first].GetSize()); ii++)
        if (ii < 10)
          index[ii] = fPointIndex[(*p).first].At(ii);
      AddHit(1, (*p).first, (*p).second, fTrackTime[(*p).first], fPointIndex[(*p).first].GetSize(), index);
    }
  }
}
// -------------------------------------------------------------------------

// -----   Private method AddDigi   --------------------------------------------
PndEmcApdHit *PndEmcApdHitProducer::AddHit(Int_t trackID, Int_t detID, Float_t energy, Float_t time, Int_t npoint, Int_t pointIndex[10])
{
  // It fills the PndEmcApdHit category
  // cout << "PndEmcApdHitProducer: track " << trackID << " evt " << eventID << " sec " << sec << " plane " << pla << " strip " << strip << "box " << box << " tube " << tub <<
  // endl;
  TClonesArray &clref = *fDigiArray;
  Int_t size = clref.GetEntriesFast();
  return new (clref[size]) PndEmcApdHit(trackID, detID, energy, time, emcX[detID], emcY[detID], emcZ[detID], npoint, pointIndex);
}
// ----

ClassImp(PndEmcApdHitProducer)
