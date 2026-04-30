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
// PndFtsHitProducerRealFull
//
// Class for digitalization for FTS
//
// authors: Pablo Genova - Pavia University
//          Lia Lavezzi  - Pavia University
//
// modified for FTS by Isabella Garzia
/////////////////////////////////////////////////////////////

#include "PndFtsHitProducerRealFull.h"

#include "PndFtsHit.h"
#include "PndFtsHitInfo.h"
#include "PndFtsPoint.h"
#include "PndFtsSingleStraw.h"
#include "PndFtsMapCreator.h"
#include "PndFtsTube.h"

#include "PndFtsHitWriteoutBuffer.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairEventHeader.h"
#include "FairLogger.h"

#include "TGeoManager.h"
#include "TVector3.h"
#include "TRandom.h"
#include "TClonesArray.h"

#include <iostream>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndFtsHitProducerRealFull::PndFtsHitProducerRealFull()
  : PndPersistencyTask("Real FTS Hit Producer", 0), fPointArray(nullptr), // fHitArray(0),
    fHitInfoArray(nullptr), fFtsParameters(new PndGeoFtsPar()), fTimeOrderedDigi(kFALSE)
{
  SetPersistency(kTRUE);
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndFtsHitProducerRealFull::~PndFtsHitProducerRealFull() {}
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
InitStatus PndFtsHitProducerRealFull::Init()
{

  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    cout << "-E- PndFtsHitProducerRealFull::Init: "
         << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  // Get input array
  fPointArray = dynamic_cast<TClonesArray *>(ioman->GetObject("FTSPoint"));
  if (!fPointArray) {
    cout << "-W- PndFtsHitProducerRealFull::Init: "
         << "No FTSPoint array!" << endl;
    return kERROR;
  }

  // Create and register output array
  //  fHitArray = new TClonesArray("PndFtsHit");
  //  ioman->Register("FTSHit","FTS",fHitArray, fPersistence);

  fDataBuffer = new PndFtsHitWriteoutBuffer("FTSHit", "FTS", GetPersistency());
  fDataBuffer = (PndFtsHitWriteoutBuffer *)ioman->RegisterWriteoutBuffer("FTSHit", fDataBuffer);
  fDataBuffer->ActivateBuffering(fTimeOrderedDigi);

  // Create and register output array
  fHitInfoArray = new TClonesArray("PndFtsHitInfo");
  ioman->Register("FTSHitInfo", "FTS", fHitInfoArray, kFALSE);

  // CHECK added
  PndFtsMapCreator *mapper = new PndFtsMapCreator(fFtsParameters);
  fTubeArray = mapper->FillTubeArray();

  LOG(info) << " PndFtsHitProducerRealFull: Intialization successfull";

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// CHECK added
void PndFtsHitProducerRealFull::SetParContainers()
{
  FairRuntimeDb *rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fFtsParameters = (PndGeoFtsPar *)rtdb->getContainer("PndGeoFtsPar");
}

// -----   Public method Exec   --------------------------------------------
void PndFtsHitProducerRealFull::Exec(Option_t *)
{

  //  Int_t evtn=0;
  //  PndFtsPoint *ptemp =(PndFtsPoint*) fPointArray->At(0);
  //  if(ptemp !=nullptr) {
  //    evtn=ptemp->GetEventID();
  //    if(evtn%50==0)cout << "Event Number "<<evtn<<endl;
  //  }

  // Reset output array
  // if ( ! fHitArray ) Fatal("Exec", "No HitArray");

  // fHitArray->Delete();
  fHitInfoArray->Clear();

  Int_t detID = 0;    // detectorID
  TVector3 pos, dpos; // position and error vectors

  // Declare some variables
  PndFtsPoint *point = nullptr;

  // Loop over FtsPoints
  Int_t nPoints = fPointArray->GetEntriesFast();
  for (Int_t iPoint = 0; iPoint < nPoints; iPoint++) {
    point = (PndFtsPoint *)fPointArray->At(iPoint);
    if (point == nullptr)
      continue;

    detID = point->GetDetectorID();

    // tubeID  CHECK added
    Int_t skew = 0;
    Int_t layerID = point->GetLayerID();
    Int_t tubeID = point->GetTubeID();
    Int_t chamberID = point->GetChamberID();
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

    double InOut[6];
    memset(InOut, 0, sizeof(InOut));

    InOut[0] = point->GetXInLocal();
    InOut[1] = point->GetYInLocal();
    InOut[2] = point->GetZInLocal();
    InOut[3] = point->GetXOutLocal();
    InOut[4] = point->GetYOutLocal();
    InOut[5] = point->GetZOutLocal();

    // single straw tube simulation -----------------------
    PndFtsSingleStraw fts;

    // setting the single straw tube simulation constants
    // 3 options currently available:
    // TConst(tube radius (cm), gas pressure (bar), Ar%, CO2%)
    // fts.TConst(0.4, 1, 0.9, 0.1);
    // fts.TConst(0.5, 1, 0.9, 0.1);
    fts.TConst(0.5, 2, 0.8, 0.2);

    // wire positioning
    fts.PutWireXYZ(0., 0., -75., 0., 0., 75.);

    // get particle momentum
    TVector3 momentum(point->GetPxOut(), point->GetPyOut(), point->GetPzOut()); // GeV/c

    Double_t GeV = 1.;
    // position in cm (already in cm); momentum in GeV (already in GeV); mass in GeV (already in GeV)

    // drift time calculation
    Double_t pulset = fts.PartToTime(point->GetMass() / GeV, momentum.Mag() / GeV, InOut);

    // simulated radius (cm)
    double radius = fts.TimnsToDiscm(pulset);
    if (radius < 0.)
      radius = 0.; // CHECK

    // true radius (cm)
    // double true_rad = fts.TrueDist(InOut); //[R.K. 03/2017] unused variable?

    // dE calculation
    double depCharge = fts.PartToADC();

    // dE/dx calculation
    // double dedx = -999; //[R.K. 01/2017] unused variable?

    // fts: detID, pos, dpos, index come from --------------
    // fts (FairHit):
    Double_t closestDistanceError = 0.0150; // per adesso (stessa che in Ideal:
                                            // radialResolution = 0.0150)

    TVector3 position = tube->GetPosition();

    // ----------------
    // TVector3 posInLocal(point->GetXInLocal(), point->GetYInLocal(), point->GetZInLocal());
    // TVector3 posOutLocal(point->GetXOutLocal(), point->GetYOutLocal(), point->GetZOutLocal());
    // Double_t zpos = position.Z() + ((posOutLocal.Z() + posInLocal.Z()) / 2.);
    // Double_t zposError;
    // FoldZPosWithResolution(zpos, zposError, posInLocal, posOutLocal);
    //    pos.SetXYZ(position.X(), position.Y(), zpos); // <--- stt2
    // ----------------

    pos.SetXYZ(position.X(), position.Y(), position.Z()); // <--- stt1

    //    dpos.SetXYZ(innerStrawDiameter / 2., innerStrawDiameter / 2., GetLongitudinalResolution(position.Z()));
    dpos.SetXYZ(0.5, 0.5, 3.); // per adesso (stessi che in Ideal:
                               // innerStrawDiameter/2 = 0.5,
                               // longitudinalResolution = 3.)

    // create hit
    AddHit(detID, tubeID, chamberID, layerID, skew, iPoint, pos, dpos, pulset, radius, closestDistanceError, depCharge, point->GetTime());

    AddHitInfo(0, 0, point->GetTrackID(), iPoint, 0, kFALSE);

  } // Loop over MCPoints

  // Event summary
  LOG(info) << " PndFtsHitProducerRealFull: " << nPoints << " FtsPoints, " << nPoints << " Hits created.";
}
// -------------------------------------------------------------------------
void PndFtsHitProducerRealFull::FoldZPosWithResolution(Double_t &zpos, Double_t &zposError, TVector3, TVector3) // localInPos localOutPos //[R.K.03/2017] unused variable(s)
{
  // Double_t
  // zPosInStrawFrame = (localOutPos.Z() - localInPos.Z()) / 2.; //[R.K. 01/2017] unused variable?

  //  zposError = gRandom->Gaus(0., GetLongitudinalResolution(zPosInStrawFrame));
  zposError = gRandom->Gaus(0., 3.); // per adesso (stesso che in Ideal:
                                     // longitudinalResolution = 3.)

  zpos += zposError;
}

// -----   Private method AddHit   --------------------------------------------
PndFtsHit *PndFtsHitProducerRealFull::AddHit(Int_t detID, Int_t tubeID, Int_t chamberID, Int_t layerID, Int_t skew, Int_t iPoint, TVector3 &pos, TVector3 &dpos, Double_t p,
                                             Double_t rsim, Double_t closestDistanceError, Double_t depcharge, Double_t timeOfFlight)
{
  // see PndFtsHit for hit description

  Double_t EventTime = FairRootManager::Instance()->GetEventTime();

  PndFtsHit *hitnew = new PndFtsHit(detID, tubeID, chamberID, layerID, skew, iPoint, pos, dpos, p + EventTime + timeOfFlight, rsim, closestDistanceError, depcharge);
  if (fTimeOrderedDigi) {
    hitnew->ResetLinks();
    FairEventHeader *evtHeader = (FairEventHeader *)FairRootManager::Instance()->GetObject("EventHeader.");
    hitnew->AddLink(FairLink(evtHeader->GetInputFileId(), evtHeader->GetMCEntryNumber() - 1, "FTSPoint", iPoint));
    hitnew->AddLink(FairLink(-1, FairRootManager::Instance()->GetEntryNr(), "EventHeader.", -1));
  }
  fDataBuffer->FillNewData(hitnew, p + EventTime + timeOfFlight, timeOfFlight + EventTime);
  return hitnew;
}
// ----

// -----   Private method AddHitInfo   --------------------------------------------
PndFtsHitInfo *PndFtsHitProducerRealFull::AddHitInfo(Int_t fileNumber, Int_t eventNumber, Int_t trackID, Int_t pointID, Int_t nMerged, Bool_t isFake)
{
  // see PndSttHitInfo for hit description
  TClonesArray &clref = *fHitInfoArray;
  Int_t size = clref.GetEntriesFast();
  return new (clref[size]) PndFtsHitInfo(fileNumber, eventNumber, trackID, pointID, nMerged, isFake);
}

ClassImp(PndFtsHitProducerRealFull)
