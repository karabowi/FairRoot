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
// PndOtHitProducerRealFast
//
// Class for digitalization for OT
//
// authors: Radoslaw Karabowicz, GSI, 2024
//
// modified from PndFtsHitProducerRealFast by Nafija Ibrišimović in 2023
////////////////////////////////////////////////////////////////////////////

#include "PndOtHitProducerRealFast.h"
// from PandaRoot, this library
#include "PndOtHitInfo.h"
#include "PndOtSingleStraw.h"
#include "PndGeoOtPar.h"
#include "PndOtTube.h"
#include "PndOtMapCreator.h"
#include "PndOtSignalOverlap.h"
// from PandaRoot/pnddata
#include <PndOtHit.h>
#include <PndOtPoint.h>
// from FairRoot
#include <FairRootManager.h>
#include <FairRunAna.h>
#include <FairRuntimeDb.h>
#include <FairGeoNode.h>
#include <FairGeoTransform.h>
#include <FairGeoRotation.h>
#include <FairGeoVector.h>
#include <FairLogger.h>
// from ROOT
#include <TGeoManager.h>
#include <TClonesArray.h>
#include <TVector3.h>
#include <TRandom.h>
// standard
#include <iostream>
#include <cmath>

using std::cout;
using std::endl;
using std::sqrt;

// -----   Default constructor   -------------------------------------------
PndOtHitProducerRealFast::PndOtHitProducerRealFast()
  : PndPersistencyTask("Ideal OT Hit Producer", 0), fPointArray(nullptr), fHitArray(nullptr), fVolumeArray(0), fHitInfoArray(nullptr), fevtn(0), fOtParameters(new PndGeoOtPar()),
    fOverlap(kFALSE)
{
  SetPersistency(kTRUE);
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndOtHitProducerRealFast::~PndOtHitProducerRealFast() {}
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
InitStatus PndOtHitProducerRealFast::Init()
{
  fevtn = 0;

  //  std::cout << "#########################################################" << std::endl;
  //  std::cout << "PndOtHitProducerRealFast: Init()#######" << std::endl;
  //  std::cout << "#########################################################" << std::endl;

  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    cout << "-E- PndOtHitProducerRealFast-wintz::Init: "
         << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  // Get input array
  fPointArray = (TClonesArray *)ioman->GetObject("OTPoint");
  if (!fPointArray) {
    cout << "-W- PndOtHitProducerRealFast::Init: "
         << "No OTPoint array!" << endl;
    return kERROR;
  }

  // Create and register output array: without fOverlap
  // fHitArray = new TClonesArray("PndOtHit");
  // ioman->Register("OTHit","OT",fHitArray, fPersistence);

  ////new part: create and register output array
  if (!fOverlap) {
    fHitArray = new TClonesArray("PndOtHit");
    ioman->Register("OTHit", "OT", fHitArray, GetPersistency());
  } else {
    // if overlap on, save the overlapped hits in regular
    // output TCA (OTHit) and the "original" hits (non overlapped)
    // in another TCA (OTOriginalHit)
    fOverlapHitArray = new TClonesArray("PndOtHit");
    ioman->Register("OTHit", "OT", fOverlapHitArray, GetPersistency());
    fHitArray = new TClonesArray("PndOtHit");
    ioman->Register("OTOriginalHit", "OT", fHitArray, GetPersistency());
  }

  // Create and register output array
  fHitInfoArray = new TClonesArray("PndOtHitInfo");
  ioman->Register("OTHitInfo", "OT", fHitInfoArray, kFALSE);

  fVolumeArray = gGeoManager->GetListOfVolumes();

  LOG(info) << " PndOTHitProducerRealFast: INITIALIZATION SUCCESSFUL";

  // CHECK added
  PndOtMapCreator *mapper = new PndOtMapCreator(fOtParameters);
  fOtTubeArray = mapper->FillTubeArray();

  cout << "MapCreator did " << fOtTubeArray->GetEntriesFast() << " OT tubes." << endl;

  return kSUCCESS;
}
// -------------------------------------------------------------------------

void PndOtHitProducerRealFast::SetParContainers()
{

  FairRuntimeDb *rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fOtParameters = (PndGeoOtPar *)rtdb->getContainer("PndGeoOtPar");
}

// -----   Public method Exec   --------------------------------------------
void PndOtHitProducerRealFast::Exec(Option_t *)
{
  // std::cout<<"PndOtHitProducer Exec ########"<<std::endl;
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
  PndOtPoint *point = nullptr;

  // Loop over OtPoints
  Int_t nOtPoints = fPointArray->GetEntriesFast();

  //  cout << "------------ " << nPoints << endl;
  for (Int_t iPoint = 0; iPoint < nOtPoints; iPoint++) {

    point = (PndOtPoint *)fPointArray->At(iPoint);
    if (point == nullptr)
      continue;

    detID = point->GetDetectorID();

    // tubeID  CHECK added
    Int_t skew = 0;
    Int_t tubeID = point->GetTubeID();
    Int_t chamberID = point->GetChamberID();
    Int_t layerID = point->GetLayerID();

    PndOtTube *tube = (PndOtTube *)fOtTubeArray->At(tubeID);

    /*  cout << "Point in chamber " << point->GetChamberID() << " layer " << point->GetLayerID()
         << " tube " << point->GetTubeID() << endl;
    cout << "Point at: " << point->GetX() << " " << point->GetY() << " " << point->GetZ() << endl;
    if (tube)
      cout << "Tube at:  " << tube->GetPosition()[0] << " " << tube->GetPosition()[1] << " " << tube->GetPosition()[2] << endl;
    else
      cout << "There is no tube" << endl; */

    // if skewed tube: skew==1
    if (layerID >= 3 && layerID <= 6) {
      skew = 1;
    } // skewed tudes ot1
    if (layerID >= 11 && layerID <= 14) {
      skew = 1;
    } // skewed tudes ot2
    if (layerID >= 19 && layerID <= 22) {
      skew = 1;
    } // skewed tudes ot3
    if (layerID >= 27 && layerID <= 30) {
      skew = 1;
    } // skewed tudes ot4
    if (layerID >= 35 && layerID <= 38) {
      skew = 1;
    } // skewed tudes ot5
    if (layerID >= 43 && layerID <= 46) {
      skew = 1;
    } // skewed tudes ot6

    double InOut[6];
    memset(InOut, 0, sizeof(InOut));

    InOut[0] = point->GetXInLocal();
    InOut[1] = point->GetYInLocal();
    InOut[2] = point->GetZInLocal();
    InOut[3] = point->GetXOutLocal();
    InOut[4] = point->GetYOutLocal();
    InOut[5] = point->GetZOutLocal();

    // single straw tube simulation -----------------------
    PndOtSingleStraw ot;

    // setting the single straw tube simulation constants
    // 3 options currently available:
    // TConst(tube radius (cm), gas pressure (bar), Ar%, CO2%)
    // ot.TConst(0.4, 1, 0.9, 0.1);
    // ot.TConst(0.4903/2., 1, 0.9, 0.1);//1 bar
    ot.TConst(0.4903 / 2., 2, 0.8, 0.2); // 2 bar

    // wire positioning->controllare bene
    ot.PutWireXYZ(0., 0., -230.5 / 2., 0., 0., 230.5 / 2.); // Acceptance straw length of S1 (longer straw)

    // get particle momentum
    TVector3 momentum(point->GetPxOut(), point->GetPyOut(), point->GetPzOut()); // GeV/c

    Double_t GeV = 1.;
    // position in cm (already in cm); momentum in GeV (already in GeV); mass in GeV (already in GeV)

    // drift time calculation

    Double_t pulset = -1;
    // pulset = ot.PartToTime(point->GetMass()/GeV, momentum.Mag()/GeV, InOut);

    // constant initialization
    ot.TInit(point->GetMass() / GeV, momentum.Mag() / GeV, InOut);

    // true radius (cm)
    Double_t true_rad = ot.TrueDist(InOut);

    // simulated radius (cm)
    // Double_t radius = ot.TimnsToDiscm(pulset);
    // if(radius < 0.) radius = 0.; // CHECK
    // if(radius <0. ||radius==0.) radius =-999;

    // fast simulation
    Double_t radius = ot.FastRec(true_rad, 1); //,0) standard curve ,1) Juelich exp curve
                                               // Juelich is at 2 bar pressure
    // dE calculation
    //  double depCharge = ot.PartToADC();

    // dE calculation ------- check
    // charge calculation
    Double_t depcharge = ot.FastPartToADC(); // CHECK   arbitrary units!
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
    dpos.SetXYZ(0.4903 / 2., 0.4903 / 2., 3.); // per adesso (stessi che in Ideal:
                                               // innerStrawDiameter/2 = 0.4903/2.,
                                               // longitudinalResolution = 3.)
    // create hit
    Double_t eventTime = FairRootManager::Instance()->GetEventTime();
    Double_t flightTime = point->GetTime();
    AddHit(detID, tubeID, chamberID, layerID, skew, iPoint, pos, dpos, pulset + flightTime + eventTime, radius, closestDistanceError, depcharge);
    AddHitInfo(0, 0, point->GetTrackID(), iPoint, 0, kFALSE);
  }

  if (fOverlap) {
    PndOtSignalOverlap *myoverlap = new PndOtSignalOverlap(fHitArray);
    myoverlap->OverlapSimultaneousSignals(fOverlapHitArray);
  }

  // Event summary
  // cout << "-I- PndSttHitProducerRealFast: " << nPoints << " OtPoints, "
  //<< nPoints << " Hits created." << endl;
}
// -------------------------------------------------------------------------
void PndOtHitProducerRealFast::FoldZPosWithResolution(Double_t &zpos, Double_t &zposError, TVector3, TVector3) // localInPos localOutPos //[R.K.03/2017] unused variable(s)
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
PndOtHit *PndOtHitProducerRealFast::AddHit(Int_t detID, Int_t tubeID, Int_t chamberID, Int_t layerID, Int_t skew, Int_t iPoint, TVector3 &pos, TVector3 &dpos, Double_t p,
                                           Double_t rsim, Double_t closestDistanceError, Double_t depcharge)
{

  // see PndOtHit for hit description
  TClonesArray &clref = *fHitArray;
  Int_t size = clref.GetEntriesFast();

  PndOtHit *hitnew = new (clref[size]) PndOtHit(detID, tubeID, chamberID, layerID, skew, iPoint, pos, dpos, p, rsim, closestDistanceError, depcharge);
  return hitnew;
}
// ----

// -----   Private method AddHitInfo   --------------------------------------------
PndOtHitInfo *PndOtHitProducerRealFast::AddHitInfo(Int_t fileNumber, Int_t eventNumber, Int_t trackID, Int_t pointID, Int_t nMerged, Bool_t isFake)
{
  // see PndOtHitInfo for hit description

  TClonesArray &clref = *fHitInfoArray;
  Int_t size = clref.GetEntriesFast();
  return new (clref[size]) PndOtHitInfo(fileNumber, eventNumber, trackID, pointID, nMerged, isFake);
}

Double_t PndOtHitProducerRealFast::GetError(Double_t TrueDcm)
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

ClassImp(PndOtHitProducerRealFast)
