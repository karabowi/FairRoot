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
// -----                PndSttHitProducerIdeal source file             -----
// -------------------------------------------------------------------------

#include "PndSttHitProducerIdeal.h"

#include "PndSttHit.h"
#include "PndSttHitInfo.h"
#include "PndSttPoint.h"
#include "PndSttTube.h"
#include "PndSttSingleStraw.h"
#include "PndSttMapCreator.h"

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
PndSttHitProducerIdeal::PndSttHitProducerIdeal() : PndPersistencyTask("Ideal STT Hit Producer", 0)
{
  SetPersistency(kTRUE);

  fPointArray = nullptr;
  fHitArray = nullptr;
  fHitInfoArray = nullptr;
  fTubeArray = nullptr;

  fSttParameters = nullptr;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndSttHitProducerIdeal::~PndSttHitProducerIdeal() {}
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
InitStatus PndSttHitProducerIdeal::Init()
{
  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();

  if (!ioman) {
    cout << "-E- PndSttHitProducerIdeal::Init: "
         << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  // Get input array
  fPointArray = (TClonesArray *)ioman->GetObject("STTPoint");

  if (!fPointArray) {
    cout << "-W- PndSttHitProducerIdeal::Init: "
         << "No STTPoint array!" << endl;
    return kERROR;
  }

  // Create and register output array
  fHitArray = new TClonesArray("PndSttHit");
  ioman->Register("STTHit", "STT", fHitArray, GetPersistency());

  // Create and register output array
  fHitInfoArray = new TClonesArray("PndSttHitInfo");
  ioman->Register("STTHitInfo", "STT", fHitInfoArray, GetPersistency());

  // CHECK added
  //   PndSttMapCreator *mapper = new PndSttMapCreator(fSttParameters);
  //   fTubeArray = mapper->FillTubeArray();

  LOG(info) << " PndSttHitProducerIdeal: Intialisation successfull";
  return kSUCCESS;
}
// -------------------------------------------------------------------------

// CHECK added
void PndSttHitProducerIdeal::SetParContainers()
{
  FairRuntimeDb *rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fSttParameters = (PndGeoSttPar *)rtdb->getContainer("PndGeoSttPar");
}

// -----   Public method Exec   --------------------------------------------
void PndSttHitProducerIdeal::Exec(Option_t *)
{
  if (fTubeArray == nullptr) {
    PndSttMapCreator mapper(fSttParameters);
    fTubeArray = mapper.FillTubeArray();
  }

  // Reset output array
  if (!fHitArray)
    Fatal("Exec", "No HitArray");

  fHitArray->Delete();
  fHitInfoArray->Delete();

  // Declare some variables
  PndSttPoint *point = nullptr;

  Int_t detID = 0, // Detector ID
    trackID = 0;   // Track index

  TVector3 pos, dpos; // Position and error vectors

  // Loop over SttPoints
  Int_t nPoints = fPointArray->GetEntriesFast();
  Int_t counter = 0;
  for (Int_t iPoint = 0; iPoint < nPoints; iPoint++) {
    point = (PndSttPoint *)fPointArray->At(iPoint);

    if (!point)
      continue;

    // Detector ID
    detID = point->GetDetectorID();

    // MCTrack ID
    trackID = point->GetTrackID();

    // tubeID  CHECK added
    Int_t tubeID = point->GetTubeID();
    PndSttTube *tube = (PndSttTube *)fTubeArray->At(tubeID);

    // Determine hit position and isochrone (x,y of wire, measured z  position)
    TVector3 posInLocal(point->GetXInLocal(), point->GetYInLocal(), point->GetZInLocal()), posOutLocal(point->GetXOutLocal(), point->GetYOutLocal(), point->GetZOutLocal()),
      position;
    position = tube->GetPosition(); // CHECK added

    Double_t closestDistance, closestDistanceError;

    // GetClostestApproachToWire(closestDistance, closestDistanceError,
    //				posInLocal, posOutLocal);

    // minimum distance ----------------

    // Double_t fd_in  = sqrt(point->GetXInLocal()*point->GetXInLocal()+point->GetYInLocal()*point->GetYInLocal()); //[R.K. 01/2017] unused variable?
    // cout<<fd_in<<endl;
    double InOut[6];
    memset(InOut, 0, sizeof(InOut));

    InOut[0] = point->GetXInLocal();
    InOut[1] = point->GetYInLocal();
    InOut[2] = point->GetZInLocal();
    InOut[3] = point->GetXOutLocal();
    InOut[4] = point->GetYOutLocal();
    InOut[5] = point->GetZOutLocal();

    PndSttSingleStraw stt;
    stt.PutWireXYZ(0., 0., -75., 0., 0., 75.);
    closestDistance = stt.TrueDist(InOut);
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

    new ((*fHitArray)[counter]) PndSttHit(detID, tubeID, iPoint, pos, dpos, 0, closestDistance, closestDistanceError, eloss * 1e6);

    new ((*fHitInfoArray)[counter]) PndSttHitInfo(0, 0, trackID, iPoint, 0, kFALSE);
    counter++;
  } // Loop over MCPoints

  // Event summary
  if (fVerbose > 1)
    LOG(info) << " PndSttHitProducerIdeal: " << nPoints << " SttPoints, " << nPoints << " Hits created.";
}
// -------------------------------------------------------------------------

// -----   Private method GetRadialResolution-------------------------------
Double_t PndSttHitProducerIdeal::GetRadialResolution(Double_t radius)
{
  return radialResolutionPolynomialConstant1 + radius * radialResolutionPolynomialConstant2 + radius * radius * radialResolutionPolynomialConstant3;
}
// -------------------------------------------------------------------------

// -----   Private method GetLongitudinalResolution ------------------------
Double_t PndSttHitProducerIdeal::GetLongitudinalResolution(Double_t zpos)
{
  return longitudinalResolutionPolynomialConstant1 + zpos * longitudinalResolutionPolynomialConstant2 + zpos * zpos * longitudinalResolutionPolynomialConstant3;
}
// -------------------------------------------------------------------------

// -----   Private method GetClostestApproachToWire ------------------------
void PndSttHitProducerIdeal::GetClostestApproachToWire(Double_t &closestDistance, Double_t &closestDistanceError, TVector3 localInPos, TVector3 localOutPos)
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

void PndSttHitProducerIdeal::FoldZPosWithResolution(Double_t &zpos, Double_t &zposError, TVector3 localInPos, TVector3 localOutPos)
{
  Double_t zPosInStrawFrame = (localOutPos.Z() - localInPos.Z()) / 2.;

  zposError = gRandom->Gaus(0., GetLongitudinalResolution(zPosInStrawFrame));

  zpos += zposError;
  //  cout << "zpos in prod: " << zpos << endl;
}

ClassImp(PndSttHitProducerIdeal)
