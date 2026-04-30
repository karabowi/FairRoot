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
// -----                PndFtsHitProducerIdeal source file             -----
// -------------------------------------------------------------------------

#include "PndFtsHitProducerIdeal.h"

#include "PndFtsHit.h"
#include "PndFtsHitInfo.h"
#include "PndFtsPoint.h"
#include "PndFtsTube.h"
#include "PndFtsSingleStraw.h"
#include "PndFtsMapCreator.h"

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

// TODO: read this from a configuration file
#define foldResolution 0 // <===== NO SMEARING
#define radialResolutionPolynomialConstant1 0.0150
#define radialResolutionPolynomialConstant2 0.
#define radialResolutionPolynomialConstant3 0.
//#define longitudinalResolutionPolynomialConstant1 3.
#define longitudinalResolutionPolynomialConstant1 0.0001
#define longitudinalResolutionPolynomialConstant2 0.
#define longitudinalResolutionPolynomialConstant3 0.

// TODO: read this from geant initialization
#define innerStrawDiameter 1.

// -----   Default constructor   -------------------------------------------
PndFtsHitProducerIdeal::PndFtsHitProducerIdeal() : PndPersistencyTask("Ideal FTS Hit Producer", 0)
{
  SetPersistency(kTRUE);
  fPointArray = nullptr;
  fHitArray = nullptr;
  fHitInfoArray = nullptr;
  fTubeArray = nullptr;
  fFtsParameters = nullptr;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndFtsHitProducerIdeal::~PndFtsHitProducerIdeal() {}
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
InitStatus PndFtsHitProducerIdeal::Init()
{
  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    cout << "-E- PndFtsHitProducerIdeal::Init: "
         << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  // Get input array
  fPointArray = (TClonesArray *)ioman->GetObject("FTSPoint");
  if (!fPointArray) {
    cout << "-W- PndFtsHitProducerIdeal::Init: "
         << "No FTSPoint array!" << endl;
    return kERROR;
  }

  // Create and register output array
  fHitArray = new TClonesArray("PndFtsHit");
  ioman->Register("FTSHit", "FTS", fHitArray, GetPersistency());

  // Create and register output array
  fHitInfoArray = new TClonesArray("PndFtsHitInfo");
  ioman->Register("FTSHitInfo", "FTS", fHitInfoArray, kFALSE);

  // CHECK added
  // PndFtsMapCreator *fMapper     = new PndFtsMapCreator(fFtsParameters);
  LOG(info) << " PndFtsHitProducerIdeal: Intialisation successfull";

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// CHECK added
void PndFtsHitProducerIdeal::SetParContainers()
{
  FairRuntimeDb *rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fFtsParameters = (PndGeoFtsPar *)rtdb->getContainer("PndGeoFtsPar");
}

// -----   Public method Exec   --------------------------------------------
void PndFtsHitProducerIdeal::Exec(Option_t *)
{
  if (fTubeArray == nullptr) {
    PndFtsMapCreator *mapper = new PndFtsMapCreator(fFtsParameters);
    fTubeArray = mapper->FillTubeArray();
  }
  // Reset output array
  if (!fHitArray)
    Fatal("Exec", "No HitArray");

  fHitArray->Delete();
  fHitInfoArray->Delete();

  // Declare some variables
  PndFtsPoint *point = nullptr;

  Int_t detID = 0, // Detector ID
    trackID = 0;   // Track index

  TVector3 pos, dpos; // Position and error vectors

  // Loop over FtsPoints
  Int_t nPoints = fPointArray->GetEntriesFast();
  Int_t counter = 0;
  for (Int_t iPoint = 0; iPoint < nPoints; iPoint++) {
    point = (PndFtsPoint *)fPointArray->At(iPoint);

    if (!point)
      continue;

    // Detector ID
    detID = point->GetDetectorID();

    // MCTrack ID
    trackID = point->GetTrackID();

    // tubeID  CHECK added
    Int_t skew = 0;
    Int_t tubeID = point->GetTubeID();
    Int_t chamberID = point->GetChamberID();
    Int_t layerID = point->GetLayerID();
    PndFtsTube *tube = (PndFtsTube *)fTubeArray->At(tubeID);

    // if skewed tube: skew==1
    if (layerID >= 3 && layerID <= 6) {
      skew = 1;
    } // skewed tudes fts1
    if (layerID >= 11 && layerID <= 14) {
      skew = 1;
    } // skewed tudes fts2
    if (layerID >= 19 && layerID <= 22) {
      skew = 1;
    } // skewed tudes fts3
    if (layerID >= 27 && layerID <= 30) {
      skew = 1;
    } // skewed tudes fts4
    if (layerID >= 35 && layerID <= 38) {
      skew = 1;
    } // skewed tudes fts5
    if (layerID >= 43 && layerID <= 46) {
      skew = 1;
    } // skewed tudes fts6

    // Determine hit position and isochrone (x,y of wire, measured z  position)
    TVector3 posInLocal(point->GetXInLocal(), point->GetYInLocal(), point->GetZInLocal()), posOutLocal(point->GetXOutLocal(), point->GetYOutLocal(), point->GetZOutLocal());

    TVector3 position = tube->GetPosition();

    Double_t closestDistance, closestDistanceError;

    // GetClostestApproachToWire(closestDistance, closestDistanceError,
    //                        posInLocal, posOutLocal);

    // minimum distance ----------------

    // Double_t fd_in = sqrt(pow(point->GetXInLocal(),2)+pow(point->GetYInLocal(),2)); //[R.K. 01/2017] unused variable?
    // cout<<fd_in<<endl;
    double InOut[6];
    memset(InOut, 0, sizeof(InOut));

    InOut[0] = point->GetXInLocal();
    InOut[1] = point->GetYInLocal();
    InOut[2] = point->GetZInLocal();
    InOut[3] = point->GetXOutLocal();
    InOut[4] = point->GetYOutLocal();
    InOut[5] = point->GetZOutLocal();

    PndFtsSingleStraw fts;
    fts.PutWireXYZ(0., 0., -75., 0., 0., 75.);
    closestDistance = fts.TrueDist(InOut);
    closestDistanceError = 0.;

    Double_t eloss = point->GetEnergyLoss();
    //---------------------

    Double_t zpos = position.Z() + ((posOutLocal.Z() + posInLocal.Z()) / 2.), zposError;

    FoldZPosWithResolution(zpos, zposError, posInLocal, posOutLocal);

    // Create new hit
    //      pos.SetXYZ(position.X(), position.Y(), zpos);
    pos.SetXYZ(position.X(), position.Y(), position.Z()); // CHECK!
    dpos.SetXYZ(innerStrawDiameter / 2., innerStrawDiameter / 2., GetLongitudinalResolution(position.Z()));

    //  if(fabs(fd_in-0.5)>0.001) continue;

    Double_t eventTime = FairRootManager::Instance()->GetEventTime();
    Double_t flightTime = point->GetTime();

    new ((*fHitArray)[counter]) PndFtsHit(detID, tubeID, chamberID, layerID, skew, iPoint, pos, dpos, eventTime + flightTime, closestDistance, closestDistanceError, eloss * 1e6);

    new ((*fHitInfoArray)[counter]) PndFtsHitInfo(0, 0, trackID, iPoint, 0, kFALSE);
    counter++;
  } // Loop over MCPoints

  // Event summary
  LOG(info) << " PndFtsHitProducerIdeal: " << nPoints << " FtsPoints, " << nPoints << " Hits created.";
}
// -------------------------------------------------------------------------

// -----   Private method GetRadialResolution-------------------------------
Double_t PndFtsHitProducerIdeal::GetRadialResolution(Double_t radius)
{
  return radialResolutionPolynomialConstant1 + radius * radialResolutionPolynomialConstant2 + radius * radius * radialResolutionPolynomialConstant3;
}
// -------------------------------------------------------------------------

// -----   Private method GetLongitudinalResolution ------------------------
Double_t PndFtsHitProducerIdeal::GetLongitudinalResolution(Double_t zpos)
{
  return longitudinalResolutionPolynomialConstant1 + zpos * longitudinalResolutionPolynomialConstant2 + zpos * zpos * longitudinalResolutionPolynomialConstant3;
}
// -------------------------------------------------------------------------

// -----   Private method GetClostestApproachToWire ------------------------
void PndFtsHitProducerIdeal::GetClostestApproachToWire(Double_t &closestDistance, Double_t &closestDistanceError, TVector3 localInPos, TVector3 localOutPos)
{
  Double_t a = (localOutPos.X() - localInPos.X()), b = (localOutPos.Y() - localInPos.Y()), c = sqrt(a * a + b * b) / 2.;

  // fold with Gaussian for resolution

  if (c > innerStrawDiameter / 2.) {
    c = innerStrawDiameter / 2.;
  }

  closestDistance = sqrt((innerStrawDiameter / 2.) * (innerStrawDiameter / 2.) - c * c);
  closestDistanceError = 0.;

  if (foldResolution) {
    closestDistanceError = GetRadialResolution(closestDistance);
    closestDistance += gRandom->Gaus(0., closestDistanceError);
  }
}
// -------------------------------------------------------------------------

void PndFtsHitProducerIdeal::FoldZPosWithResolution(Double_t &zpos, Double_t &zposError, TVector3 localInPos, TVector3 localOutPos)
{
  Double_t zPosInStrawFrame = (localOutPos.Z() - localInPos.Z()) / 2.;

  zposError = gRandom->Gaus(0., GetLongitudinalResolution(zPosInStrawFrame));

  zpos += zposError;
  //  cout << "zpos in prod: " << zpos << endl;
}

ClassImp(PndFtsHitProducerIdeal)
