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

////////////////////////////////////////////////////////////////////////////
// PndFtsHitProducerRealFast
//
// Class for digitalization for FTS
//
// authors: Pablo Genova - Pavia University
//          Lia Lavezzi  - Pavia University
//
// modified for forward tracking by Isabella Garzia - Ferrara Univertsity
////////////////////////////////////////////////////////////////////////////

#include "PndFtsHitProducerRealFast.h"

#include "PndFtsHit.h"
#include "PndFtsHitInfo.h"
#include "PndFtsPoint.h"
#include "PndFtsSingleStraw.h"
#include "PndGeoFtsPar.h"
#include "PndFtsTube.h"
#include "PndFtsMapCreator.h"
#include "PndFtsSignalOverlap.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairGeoNode.h"
#include "FairGeoTransform.h"
#include "FairGeoRotation.h"
#include "FairGeoVector.h"
#include "FairLogger.h"

#include "TGeoManager.h"
#include "TClonesArray.h"
#include "TVector3.h"
#include "TRandom.h"
#include <iostream>
#include <cmath>

using std::cout;
using std::endl;
using std::sqrt;

// -----   Default constructor   -------------------------------------------
PndFtsHitProducerRealFast::PndFtsHitProducerRealFast()
  : PndPersistencyTask("Ideal FTS Hit Producer", 0), fPointArray(nullptr), fHitArray(nullptr), fVolumeArray(0), fHitInfoArray(nullptr), fevtn(0),
    fFtsParameters(new PndGeoFtsPar()), fOverlap(kFALSE)
{
  SetPersistency(kTRUE);
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndFtsHitProducerRealFast::~PndFtsHitProducerRealFast() {}
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
InitStatus PndFtsHitProducerRealFast::Init()
{
  fevtn = 0;

  std::cout << "#########################################################" << std::endl;
  std::cout << "PndFtsHitProducerRealFast: Init()#######" << std::endl;
  std::cout << "#########################################################" << std::endl;

  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    cout << "-E- PndFtsHitProducerRealFast-wintz::Init: "
         << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  // Get input array
  fPointArray = (TClonesArray *)ioman->GetObject("FTSPoint");
  if (!fPointArray) {
    cout << "-W- PndFtsHitProducerRealFast::Init: "
         << "No FTSPoint array!" << endl;
    return kERROR;
  }

  // Create and register output array: without fOverlap
  // fHitArray = new TClonesArray("PndFtsHit");
  // ioman->Register("FTSHit","FTS",fHitArray, fPersistence);

  ////new part: create and register output array
  if (!fOverlap) {
    fHitArray = new TClonesArray("PndFtsHit");
    ioman->Register("FTSHit", "FTS", fHitArray, GetPersistency());
  } else {
    // if overlap on, save the overlapped hits in regular
    // output TCA (FTSHit) and the "original" hits (non overlapped)
    // in another TCA (FTSOriginalHit)
    fOverlapHitArray = new TClonesArray("PndFtsHit");
    ioman->Register("FTSHit", "FTS", fOverlapHitArray, GetPersistency());
    fHitArray = new TClonesArray("PndFtsHit");
    ioman->Register("FTSOriginalHit", "FTS", fHitArray, GetPersistency());
  }

  // Create and register output array
  fHitInfoArray = new TClonesArray("PndFtsHitInfo");
  ioman->Register("FTSHitInfo", "FTS", fHitInfoArray, kFALSE);

  fVolumeArray = gGeoManager->GetListOfVolumes();

  LOG(info) << " PndFTSHitProducerRealFast: INITIALIZATION SUCCESSFUL";

  // CHECK added
  PndFtsMapCreator *mapper = new PndFtsMapCreator(fFtsParameters);
  fTubeArray = mapper->FillTubeArray();

  return kSUCCESS;
}
// -------------------------------------------------------------------------

void PndFtsHitProducerRealFast::SetParContainers()
{

  FairRuntimeDb *rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fFtsParameters = (PndGeoFtsPar *)rtdb->getContainer("PndGeoFtsPar");
}

// -----   Public method Exec   --------------------------------------------
void PndFtsHitProducerRealFast::Exec(Option_t *)
{

  // std::cout<<"PndFtsHitProducer Exec ########"<<std::endl;
  if (fVerbose && fevtn % 50 == 0)
    cout << "Event Number " << fevtn << endl;
  else if (fVerbose >= 3)
    cout << "Event Number " << fevtn << endl;

  fevtn++;

  // Reset output array
  if (!fHitArray)
    Fatal("Exec", "No HitArray");

  fHitArray->Delete();
  fHitInfoArray->Clear();
  if (fOverlap)
    fOverlapHitArray->Delete();

  Int_t detID = 0;    // detectorID
  TVector3 pos, dpos; // position and error vectors

  // Declare some variables
  PndFtsPoint *point = nullptr;

  // Loop over FtsPoints
  Int_t nPoints = fPointArray->GetEntriesFast();

  //  cout << "------------ " << nPoints << endl;

  for (Int_t iPoint = 0; iPoint < nPoints; iPoint++) {
    point = (PndFtsPoint *)fPointArray->At(iPoint);
    if (point == nullptr)
      continue;

    detID = point->GetDetectorID();

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
    // fts.TConst(0.5, 1, 0.9, 0.1);//1 bar
    fts.TConst(0.5, 2, 0.8, 0.2); // 2 bar

    // wire positioning->controllare bene
    fts.PutWireXYZ(0., 0., -75., 0., 0., 75.);

    // get particle momentum
    TVector3 momentum(point->GetPxOut(), point->GetPyOut(), point->GetPzOut()); // GeV/c

    Double_t GeV = 1.;
    // position in cm (already in cm); momentum in GeV (already in GeV); mass in GeV (already in GeV)

    // drift time calculation

    Double_t pulset = -1;
    // pulset = fts.PartToTime(point->GetMass()/GeV, momentum.Mag()/GeV, InOut);

    // constant initialization
    fts.TInit(point->GetMass() / GeV, momentum.Mag() / GeV, InOut);

    // true radius (cm)
    Double_t true_rad = fts.TrueDist(InOut);

    // simulated radius (cm)
    // Double_t radius = fts.TimnsToDiscm(pulset);
    // if(radius < 0.) radius = 0.; // CHECK
    // if(radius <0. ||radius==0.) radius =-999;

    // fast simulation
    Double_t radius = fts.FastRec(true_rad, 1); //,0) standard curve ,1) Juelich exp curve
                                                // Juelich is at 2 bar pressure
    // dE calculation
    //  double depCharge = fts.PartToADC();

    // dE calculation ------- check
    // charge calculation
    Double_t depcharge = fts.FastPartToADC(); // CHECK   arbitrary units!
    // dE/dx calculation postponed
    // Double_t dedx = -999; //[R.K. 01/2017] unused variable?

    Double_t closestDistanceError = GetError(radius); // calculates the error according to Juelich experimental curves
    // cout<<"radius "<<radius<<" error "<<closestDistanceError<<endl;
    // closestDistanceError = 0.0150; //150 microns check this point!
    // closestDistanceError =TMath::Sqrt(2.)*radius/TMath::Sqrt(12);

    // TVector3 position(point->GetX(), point->GetY(), point->GetZ()); // use this for hits having same coordinates as MC points
    TVector3 position = tube->GetPosition();              // use this for realistic hit production
    pos.SetXYZ(position.X(), position.Y(), position.Z()); // <--- stt1

    //    dpos.SetXYZ(innerStrawDiameter / 2., innerStrawDiameter / 2., GetLongitudinalResolution(position.Z()));
    dpos.SetXYZ(0.5, 0.5, 3.); // per adesso (stessi che in Ideal:
                               // innerStrawDiameter/2 = 0.5,
                               // longitudinalResolution = 3.)

    // create hit
    Double_t eventTime = FairRootManager::Instance()->GetEventTime();
    Double_t flightTime = point->GetTime();
    AddHit(detID, tubeID, chamberID, layerID, skew, iPoint, pos, dpos, pulset + flightTime + eventTime, radius, closestDistanceError, depcharge);
    AddHitInfo(0, 0, point->GetTrackID(), iPoint, 0, kFALSE);

  } // Loop over MCPoints

  if (fOverlap) {
    PndFtsSignalOverlap *myoverlap = new PndFtsSignalOverlap(fHitArray);
    myoverlap->OverlapSimultaneousSignals(fOverlapHitArray);
  }

  // Event summary
  // cout << "-I- PndSttHitProducerRealFast: " << nPoints << " FtsPoints, "
  //<< nPoints << " Hits created." << endl;
}
// -------------------------------------------------------------------------
void PndFtsHitProducerRealFast::FoldZPosWithResolution(Double_t &zpos, Double_t &zposError, TVector3, TVector3) // localInPos localOutPos //[R.K.03/2017] unused variable(s)
{

  // Double_t
  // zPosInStrawFrame = (localOutPos.Z() - localInPos.Z()) / 2.; //[R.K. 01/2017] unused variable?
  // FIXME We have dummy Error calculation
  //  zposError = gRandom->Gaus(0., GetLongitudinalResolution(zPosInStrawFrame));
  zposError = gRandom->Gaus(0., 3.); // per adesso (stesso che in Ideal:
                                     // longitudinalResolution = 3.)

  zpos += zposError;
}

// -----   Private method AddHit   --------------------------------------------
PndFtsHit *PndFtsHitProducerRealFast::AddHit(Int_t detID, Int_t tubeID, Int_t chamberID, Int_t layerID, Int_t skew, Int_t iPoint, TVector3 &pos, TVector3 &dpos, Double_t p,
                                             Double_t rsim, Double_t closestDistanceError, Double_t depcharge)
{

  // see PndFtsHit for hit description
  TClonesArray &clref = *fHitArray;
  Int_t size = clref.GetEntriesFast();

  PndFtsHit *hitnew = new (clref[size]) PndFtsHit(detID, tubeID, chamberID, layerID, skew, iPoint, pos, dpos, p, rsim, closestDistanceError, depcharge);
  return hitnew;
}
// ----

// -----   Private method AddHitInfo   --------------------------------------------
PndFtsHitInfo *PndFtsHitProducerRealFast::AddHitInfo(Int_t fileNumber, Int_t eventNumber, Int_t trackID, Int_t pointID, Int_t nMerged, Bool_t isFake)
{
  // see PndFtsHitInfo for hit description

  TClonesArray &clref = *fHitInfoArray;
  Int_t size = clref.GetEntriesFast();
  return new (clref[size]) PndFtsHitInfo(fileNumber, eventNumber, trackID, pointID, nMerged, isFake);
}

Double_t PndFtsHitProducerRealFast::GetError(Double_t TrueDcm)
{

  // data from julich
  Double_t resmic = -1;
  if (TrueDcm < 0.48) {
    resmic = 20. + 1.48048e+02 - 3.35951e+02 * TrueDcm - 1.87575e+03 * pow(TrueDcm, 2) + 1.92910e+04 * pow(TrueDcm, 3) - 6.90036e+04 * pow(TrueDcm, 4) +
             1.07960e+05 * pow(TrueDcm, 5) - 5.90064e+04 * pow(TrueDcm, 6);
  } else
    resmic = 65.;

  return resmic * 0.0001;
}

ClassImp(PndFtsHitProducerRealFast)
