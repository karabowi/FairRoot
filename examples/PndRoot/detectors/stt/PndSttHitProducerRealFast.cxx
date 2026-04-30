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
// PndSttHitProducerRealFast
//
// Class for digitalization for STT1
//
// authors: Pablo Genova - Pavia University
//          Lia Lavezzi  - Pavia University
//
/////////////////////////////////////////////////////////////

#include "PndSttHitProducerRealFast.h"

#include "PndSttHit.h"
#include "PndSttHitInfo.h"
#include "PndSttPoint.h"
#include "PndSttSingleStraw.h"
#include "PndGeoSttPar.h"
#include "PndSttTube.h"
#include "PndSttMapCreator.h"
#include "PndSttSignalOverlap.h"
#include "PndSttTubeMap.h"

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
PndSttHitProducerRealFast::PndSttHitProducerRealFast() : PndPersistencyTask("Ideal STT Hit Producer", 0), fSeparate(kFALSE), fSttParameters(nullptr), fGeoType(-1)
{
  SetPersistency(kTRUE);
  fOverlap = kFALSE;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndSttHitProducerRealFast::~PndSttHitProducerRealFast() {}
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
InitStatus PndSttHitProducerRealFast::Init()
{
  fevtn = 0;

  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    cout << "-E- PndSttHitProducerRealFast-wintz::Init: "
         << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  fGeoType = fSttParameters->GetGeometryType();

  // Get input array
  fPointArray = (TClonesArray *)ioman->GetObject("STTPoint");
  if (!fPointArray) {
    cout << "-W- PndSttHitProducerRealFast::Init: "
         << "No STTPoint array!" << endl;
    return kERROR;
  }

  // Create and register output array
  if (!fOverlap) {
    if (fSeparate == kTRUE) {
      fSttParalHitArray = new TClonesArray("PndSttHit");
      ioman->Register("STTParalHit", "STT", fSttParalHitArray, GetPersistency());
      fSttSkewHitArray = new TClonesArray("PndSttHit");
      ioman->Register("STTSkewHit", "STT", fSttSkewHitArray, GetPersistency());
    } else {
      // if there is no overlap save in output the regular hits
      fHitArray = new TClonesArray("PndSttHit");
      ioman->Register("STTHit", "STT", fHitArray, GetPersistency());
    }
  } else {

    // otherwise, with overlap on, save the overlapped hits in
    // regular output TCA (STTHit) and the "original", non overlapped
    // hits in another TCA (STTOriginalHit)
    if (fSeparate == kTRUE) {
      fSttParalOverlapHitArray = new TClonesArray("PndSttHit");
      ioman->Register("STTParalHit", "STT", fSttParalOverlapHitArray, GetPersistency());
      fSttSkewOverlapHitArray = new TClonesArray("PndSttHit");
      ioman->Register("STTSkewHit", "STT", fSttSkewOverlapHitArray, GetPersistency());
      fSttParalHitArray = new TClonesArray("PndSttHit");
      ioman->Register("STTParalOriginalHit", "STT", fSttParalHitArray, GetPersistency());
      fSttSkewHitArray = new TClonesArray("PndSttHit");
      ioman->Register("STTSkewOriginalHit", "STT", fSttSkewHitArray, GetPersistency());
    } else {
      fOverlapHitArray = new TClonesArray("PndSttHit");
      ioman->Register("STTHit", "STT", fOverlapHitArray, GetPersistency());
      fHitArray = new TClonesArray("PndSttHit");
      ioman->Register("STTOriginalHit", "STT", fHitArray, GetPersistency());
    }
  }

  // Create and register output array
  fHitInfoArray = new TClonesArray("PndSttHitInfo");
  ioman->Register("STTHitInfo", "STT", fHitInfoArray, kFALSE);

  fVolumeArray = gGeoManager->GetListOfVolumes();

  // cout << "-I- PndSttHitProducerRealFast: Intialization successfull" << endl;

  // CHECK added
  if (fGeoType == 1) {
    PndSttMapCreator mapper(fSttParameters);
    fTubeArray = mapper.FillTubeArray();
  }

  LOG(info) << " PndSttHitProducerRealFast: Intialization successfull";

  return kSUCCESS;
}
// -------------------------------------------------------------------------

void PndSttHitProducerRealFast::SetParContainers()
{
  FairRuntimeDb *rtdb = FairRun::Instance()->GetRuntimeDb();
  fSttParameters = (PndGeoSttPar *)rtdb->getContainer("PndGeoSttPar");
}

// -----   Public method Exec   --------------------------------------------
void PndSttHitProducerRealFast::Exec(Option_t *)
{

  if (fVerbose && fevtn % 50 == 0)
    cout << "Event Number " << fevtn << endl;
  else if (fVerbose >= 3)
    cout << "Event Number " << fevtn << endl;

  fevtn++;

  if (fSeparate == kTRUE) {
    fSttParalHitArray->Delete();
    fSttSkewHitArray->Delete();
  } else {
    if (!fHitArray)
      Fatal("Exec", "No HitArray");
    fHitArray->Delete();
  }
  if (fOverlap) {
    if (fSeparate == kTRUE) {
      fSttParalOverlapHitArray->Delete();
      fSttSkewOverlapHitArray->Delete();
    } else {
      fOverlapHitArray->Delete();
    }
  }
  fHitInfoArray->Clear();

  Int_t detID = 0;    // detectorID
  TVector3 pos, dpos; // position and error vectors

  // Declare some variables
  PndSttPoint *point = nullptr;

  // Loop over SttPoints
  Int_t nPoints = fPointArray->GetEntriesFast();

  //  cout << "------------ " << nPoints << endl;

  for (Int_t iPoint = 0; iPoint < nPoints; iPoint++) {
    point = (PndSttPoint *)fPointArray->At(iPoint);
    if (point == nullptr)
      continue;

    detID = point->GetDetectorID();

    // tubeID  CHECK added
    Int_t tubeID = point->GetTubeID();
    PndSttTube *tube = nullptr;

    if (fGeoType == 1)
      tube = (PndSttTube *)fTubeArray->At(tubeID);
    else if (fGeoType == 2) {
      tube = PndSttTubeMap::Instance()->GetTube(tubeID);
    }
    if (tube == nullptr) {
      LOG(error) << " PndSttHitProducerRealFast Nullptr of TubeID: " << tubeID;
    }

    double InOut[6];
    memset(InOut, 0, sizeof(InOut));

    InOut[0] = point->GetXInLocal();
    InOut[1] = point->GetYInLocal();
    InOut[2] = point->GetZInLocal();
    InOut[3] = point->GetXOutLocal();
    InOut[4] = point->GetYOutLocal();
    InOut[5] = point->GetZOutLocal();

    // single straw tube simulation -----------------------
    PndSttSingleStraw stt;

    // setting the single straw tube simulation constants
    // 3 options currently available:
    // TConst(tube radius (cm), gas pressure (bar), Ar%, CO2%)
    // stt.TConst(0.4, 1, 0.9, 0.1);
    // stt.TConst(0.5, 1, 0.9, 0.1); // 1 bar
    // stt.TConst(0.5, 2, 0.8, 0.2); // 2 bar
    stt.TConst(0.5, 2, 0.9, 0.1); // 2 bar

    // wire positioning
    stt.PutWireXYZ(0., 0., -75., 0., 0., 75.);

    // get particle momentum
    TVector3 momentum(point->GetPxOut(), point->GetPyOut(), point->GetPzOut()); // GeV/c

    Double_t GeV = 1.;
    // position in cm (already in cm); momentum in GeV (already in GeV); mass in GeV (already in GeV)

    // drift time calculation

    Double_t pulset = -1;
    // pulset = stt.PartToTime(point->GetMass()/GeV, momentum.Mag()/GeV, InOut);

    // constant initialization
    stt.TInit(point->GetMass() / GeV, momentum.Mag() / GeV, InOut);

    // true radius (cm)
    Double_t true_rad = stt.TrueDist(InOut);

    // simulated radius (cm)
    // Double_t radius = stt.TimnsToDiscm(pulset);
    // if(radius < 0.) radius = 0.; // CHECK
    // if(radius <0. ||radius==0.) radius =-999;

    // fast simulation
    Int_t flag = 2; // 0) standard curve, from simulation
                    // 1) Juelich exp curve from COSY-TOF (old) (2 bar, 80/20)
                    // 2) Juelich exp curve from COSY-TOF (Feb 2011) (1.25 bar, 80/20)
                    // 3) Juelich exp curve from prototype (Apr 2010)(2 bar, 90/10)

    Double_t radius = stt.FastRec(true_rad, flag);

    // dE calculation
    // double depCharge = stt.PartToADC();
    // dE calculation ------- check
    // charge calculation
    Double_t depcharge = stt.FastPartToADC(); // CHECK   arbitrary units!
    // dE/dx calculation postponed
    // Double_t dedx = -999; //[R.K. 01/2017] unused variable?

    // error calculation according to the curve chosen by flag
    Double_t closestDistanceError = GetError(radius, flag);

    // cout<<"radius "<<radius<<" error "<<closestDistanceError<<endl;
    // closestDistanceError = 0.0150; //150 microns check this point!
    // closestDistanceError =TMath::Sqrt(2.)*radius/TMath::Sqrt(12);
    TVector3 position = tube->GetPosition(); // CHECK added

    pos.SetXYZ(position.X(), position.Y(), position.Z());

    //    dpos.SetXYZ(innerStrawDiameter / 2., innerStrawDiameter / 2., GetLongitudinalResolution(position.Z()));
    dpos.SetXYZ(1. / TMath::Sqrt(12), 1. / TMath::Sqrt(12), 150. / TMath::Sqrt(12)); // CHECK will be changed in future

    //    cout << "r: " << radius << " err: " << closestDistanceError << endl;
    // cout<<" radius "<<radius<<endl;

    AddHitInfo(0, 0, point->GetTrackID(), iPoint, 0, kFALSE);

    // create hit
    if (fSeparate == kTRUE) {
      if (tube->IsParallel())
        AddHit(fSttParalHitArray, detID, tubeID, iPoint, pos, dpos, pulset, radius, closestDistanceError, depcharge);
      else
        AddHit(fSttSkewHitArray, detID, tubeID, iPoint, pos, dpos, pulset, radius, closestDistanceError, depcharge);
    } else
      AddHit(detID, tubeID, iPoint, pos, dpos, pulset, radius, closestDistanceError, depcharge);

  } // Loop over MCPoints

  if (fOverlap) {

    if (fSeparate == kTRUE) {
      PndSttSignalOverlap *soverlap2 = new PndSttSignalOverlap(fSttParalHitArray);
      soverlap2->OverlapSimultaneousSignals(fSttParalOverlapHitArray);
      PndSttSignalOverlap *soverlap3 = new PndSttSignalOverlap(fSttSkewHitArray);
      soverlap3->OverlapSimultaneousSignals(fSttSkewOverlapHitArray);
    } else {
      PndSttSignalOverlap *soverlap = new PndSttSignalOverlap(fHitArray);
      soverlap->OverlapSimultaneousSignals(fOverlapHitArray);
      // cout << "OVERLAP " << overlap << endl;
    }
  }

  // Event summary
  // cout << "-I- PndSttHitProducerRealFast: " << nPoints << " SttPoints, "
  //      << nPoints << " Hits created." << endl;
}
// -------------------------------------------------------------------------

void PndSttHitProducerRealFast::FoldZPosWithResolution(Double_t &zpos, Double_t &zposError, TVector3, TVector3) // localInPos ,localOutPos //[R.K.03/2017] unused variable(s)
{
  // Double_t
  // zPosInStrawFrame = (localOutPos.Z() - localInPos.Z()) / 2.; //[R.K. 01/2017] unused variable?

  //  zposError = gRandom->Gaus(0., GetLongitudinalResolution(zPosInStrawFrame));
  zposError = gRandom->Gaus(0., 3.); // per adesso (stesso che in Ideal:
                                     // longitudinalResolution = 3.)

  zpos += zposError;
}

// -----   Private method AddHit   --------------------------------------------
PndSttHit *PndSttHitProducerRealFast::AddHit(Int_t detID, Int_t tubeID, Int_t iPoint, TVector3 &pos, TVector3 &dpos, Double_t p, Double_t rsim, Double_t closestDistanceError,
                                             Double_t depcharge)
{
  // see PndSttHit for hit description
  TClonesArray &clref = *fHitArray;
  Int_t size = clref.GetEntriesFast();

  PndSttHit *hitnew = new (clref[size]) PndSttHit(detID, tubeID, iPoint, pos, dpos, p, rsim, closestDistanceError, depcharge);
  return hitnew;
}
// ----

// -----   Private method AddHit   --------------------------------------------
PndSttHit *PndSttHitProducerRealFast::AddHit(TClonesArray *hitarray, Int_t detID, Int_t tubeID, Int_t iPoint, TVector3 &pos, TVector3 &dpos, Double_t p, Double_t rsim,
                                             Double_t closestDistanceError, Double_t depcharge)
{
  // see PndSttHit for hit description
  TClonesArray &clref = *hitarray;
  Int_t size = clref.GetEntriesFast();

  PndSttHit *hitnew = new (clref[size]) PndSttHit(detID, tubeID, iPoint, pos, dpos, p, rsim, closestDistanceError, depcharge);
  return hitnew;
}
// ----

// -----   Private method AddHitInfo   --------------------------------------------
PndSttHitInfo *PndSttHitProducerRealFast::AddHitInfo(Int_t fileNumber, Int_t eventNumber, Int_t trackID, Int_t pointID, Int_t nMerged, Bool_t isFake)
{
  // see PndSttHitInfo for hit description
  TClonesArray &clref = *fHitInfoArray;
  Int_t size = clref.GetEntriesFast();
  return new (clref[size]) PndSttHitInfo(fileNumber, eventNumber, trackID, pointID, nMerged, isFake);
}
// -------------------------------------------------------------------------

// --------- Get Error on radius -------------------------------------------
Double_t PndSttHitProducerRealFast::GetError(Double_t TrueDcm, Int_t rescurve)
{

  Double_t resmic = -1;

  // TrueDcm in cm

  // simulation res curve used
  if (rescurve == 0) {
    //       if(TrueDcm < 0.48){
    resmic = +1.06966e+02 - 4.03073e+03 * TrueDcm + 1.60851e+05 * pow(TrueDcm, 2) - 2.87722e+06 * pow(TrueDcm, 3) + 2.67581e+07 * pow(TrueDcm, 4) - 1.43397e+08 * pow(TrueDcm, 5) +
             4.61046e+08 * pow(TrueDcm, 6) - 8.79170e+08 * pow(TrueDcm, 7) + 9.17095e+08 * pow(TrueDcm, 8) - 4.03253e+08 * pow(TrueDcm, 9);
    //       }
    //       else resmic=30.;
  }

  // Juelich exp curve from COSY-TOF (old) used
  else if (rescurve == 1) {
    //     if (TrueDcm < 0.48) {
    resmic = +1.48048e+02 - 3.35951e+02 * TrueDcm - 1.87575e+03 * pow(TrueDcm, 2) + 1.92910e+04 * pow(TrueDcm, 3) - 6.90036e+04 * pow(TrueDcm, 4) + 1.07960e+05 * pow(TrueDcm, 5) -
             5.90064e+04 * pow(TrueDcm, 6);
    //     }
    //     else resmic=65.;
  }

  // Juelich exp curve from COSY-TOF (Feb 2011) used
  else if (rescurve == 2) {
    // data from COSY-TOF (Feb 2011)
    // the parametrization comes from mm vs mm:
    // => TrueDcm must be in mm
    TrueDcm *= 10.; // cm -> mm
    // and resmic will be given in mm ...

    // old parametriz
    //     resmic = +0.02152
    //       +0.6764*TrueDcm
    //       -1.008*pow(TrueDcm,2)
    //       +0.7421*pow(TrueDcm,3)
    //       -0.3036*pow(TrueDcm,4)
    //       +0.06955*pow(TrueDcm,5)
    //       -0.008327*pow(TrueDcm,6)
    //       +0.0004049*pow(TrueDcm,7);

    // pol5 parametriz
    resmic = 0.188119 + 0.00211993 * TrueDcm + 0.00336004 * pow(TrueDcm, 2) - 0.0103979 * pow(TrueDcm, 3) + 0.0033461 * pow(TrueDcm, 4) - 0.000315764 * pow(TrueDcm, 5);

    // convert resmic to micron and TrueDcm to cm
    resmic *= 1000.;
    TrueDcm *= 0.1;
  }

  // Juelich exp curve from prototype (Apr 2010) used
  else if (rescurve == 3) {
    // TrueDcm needed in mm --> error in mm
    resmic = 4.521331e-01 - 2.087216e-01 * 10. * TrueDcm + 4.911102e-02 * pow(10. * TrueDcm, 2) - 3.934728e-03 * pow(10. * TrueDcm, 3);
    resmic = resmic * 1000.; // error in micron
  }

  // resmic in micron
  return resmic * 0.0001; // --> resmic in cm
}
// -------------------------------------------------------------------------

ClassImp(PndSttHitProducerRealFast)
