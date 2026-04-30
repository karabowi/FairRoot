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
// -----                   PndDrcTimeDigiTask source file              -----
// -----         HARPHOOL KUMAWAT h.kumawat@gsi.de                     -----
// -----                                                               -----
// -------------------------------------------------------------------------
#include <fstream>
#include <iostream>
#include <map>
#include "stdio.h"

#include "TGeoManager.h"

#include "FairRootManager.h"
#include "PndDrcDigiWriteoutBuffer.h"
#include "PndDrcTimeDigiTask.h"
#include "PndDrcDigi.h"
#include "PndDrcPDPoint.h"
#include "PndDrcBarPoint.h"
#include "PndMCTrack.h"
#include "FairHit.h"

#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairGeoVector.h"
#include "FairGeoNode.h"
#include "FairGeoVolume.h"
#include "FairLogger.h"

#include "TVector3.h"
#include "TRandom.h"
#include "TString.h"
#include "TGeoBBox.h"
#include "TCanvas.h"

#include "FairBaseParSet.h"
#include "FairGeoTransform.h"

#include "PndGeoDrcPar.h"

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndDrcTimeDigiTask::PndDrcTimeDigiTask() : PndPersistencyTask("PndDrcTimeDigiTask")
{
  fGeo = new PndGeoDrc();
  fGeoH = nullptr;
  SetParameters();
  fisDetEff = kTRUE;
  fisPixel = kTRUE;
  fChargeSharing = kTRUE;
  fTimeSmearing = kTRUE;

  fPDPointArray = nullptr;
  fTimeOrderedDigi = kFALSE; // kTRUE;
  fDrcDigiArray = nullptr;
  fDrcTimeDigiArray = nullptr;
  Reset();
}
// -------------------------------------------------------------------------

// -----   Standard constructor with verbosity level  -------------------------------------------

PndDrcTimeDigiTask::PndDrcTimeDigiTask(Int_t verbose) : PndPersistencyTask("PndDrcTimeDigiTask", verbose)
{
  fVerbose = verbose;
  fDetType = 1;
  fisDetEff = kTRUE;
  fisPixel = kTRUE;
  fGeo = new PndGeoDrc();
  fGeoH = nullptr;
  SetParameters();

  fPDPointArray = nullptr;
  fTimeOrderedDigi = kFALSE; // kTRUE;
  fDrcDigiArray = nullptr;
  fDrcTimeDigiArray = nullptr;
  Reset();
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndDrcTimeDigiTask::~PndDrcTimeDigiTask()
{
  if (fGeo)
    delete fGeo;
  if (fDrcDigiArray) {
    fDrcDigiArray->Delete();
    delete fDrcDigiArray;
  }
  Reset();
}

// -----   Initialization   -----------------------------------------------
// -------------------------------------------------------------------------
InitStatus PndDrcTimeDigiTask::Init()
{
  cout << " ---------- INITIALIZATION ------------" << endl;
  nevents = 0;

  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    cout << "-E- PndDrcTimeDigiTask::Init: "
         << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  // PndGeoHandling
  if (fGeoH == nullptr)
    fGeoH = PndGeoHandling::Instance();

  fGeoH->SetParContainers();
  if (fVerbose > 1)
    Info("SetParContainers", "done.");

  // Get input array
  fMCArray = (TClonesArray *)ioman->GetObject("MCTrack");
  if (!fMCArray) {
    cout << "-W- PndDrcRecoLookupMap::Init: "
         << "No MCTrack array!" << endl;
    return kERROR;
  }

  // Get input array
  fBarPointArray = (TClonesArray *)ioman->GetObject("DrcBarPoint");
  if (!fBarPointArray) {
    cout << "-W- PndDrcTimeDigiTask::Init: "
         << "No DrcBarPoint array!" << endl;
    return kERROR;
  }

  // Get Photon point array
  fPDPointArray = (TClonesArray *)ioman->GetObject("DrcPDPoint");
  if (!fPDPointArray) {
    cout << "-W- PndDrcAna::Init: "
         << "No DrcPDPoint array!" << endl;
    return kERROR;
  }
  fDrcDigiArray = new TClonesArray("PndDrcDigi");
  ioman->Register("DrcDigiNormal", "Drc", fDrcDigiArray, kFALSE);

  // Create and register output array
  fDrcTimeDigiArray = new PndDrcDigiWriteoutBuffer("DrcDigi", "PndDrc", GetPersistency());
  fDrcTimeDigiArray = (PndDrcDigiWriteoutBuffer *)ioman->RegisterWriteoutBuffer("DrcDigi", fDrcTimeDigiArray);
  fDrcTimeDigiArray->ActivateBuffering(fTimeOrderedDigi);

  LOG(info) << " PndDrcTimeDigiTask: Intialization successfull";

  return kSUCCESS;
}
// -----   Execution of Task   ---------------------------------------------
void PndDrcTimeDigiTask::Exec(Option_t *) // ion //[R.K.03/2017] unused variable(s)
{
  fTimeMap.clear();
  Reset();
  fDrcDigiArray->Delete();
  nevents++;
  fPileup = 0;
  // Int_t PDHIT = fPDPointArray->GetEntriesFast(); //[R.K. 01/2017] unused variable?
  //  if (fVerbose > 0) if(nevents%10==0)
  //  cout<<"event time "<<FairRootManager::Instance()->GetEventTime()<<endl;
  // Reset output array
  fGeoH->SetVerbose(fVerbose);

  if (fVerbose > 1) {
    cout << " Number of Photon MC Points in Photon Detector Plane : " << fPDPointArray->GetEntries() << endl;
  }

  // Loop over PndDrcPDPoints
  if (fVerbose == 0)
    LOG(info) << " PndDrcTimeDigiTask: PD points " << fPDPointArray->GetEntriesFast();
  for (Int_t k = 0; k < fPDPointArray->GetEntriesFast(); k++) {

    fPpt = (PndDrcPDPoint *)fPDPointArray->At(k);

    Int_t trID = fPpt->GetTrackID();
    if (trID < 0)
      continue;
    fMCtrk = (PndMCTrack *)fMCArray->At(trID);
    Int_t trMrID = fMCtrk->GetMotherID();
    fMrIdPho = trMrID;
    fTimeAtBar = fMCtrk->GetStartTime();

    PndMCTrack *fMCtrk1 = (PndMCTrack *)fMCArray->At(trMrID);
    fPdg = fMCtrk1->GetPdgCode();
    fMrId = fMCtrk1->GetMotherID();
    // Int_t NbouncesX, NbouncesY; //[R.K. 01/2017] unused variable?
    // Double_t angleX, angleY;    //[R.K. 01/2017] unused variable?
    Int_t BarId = -1;
    fTrackId = trID;
    fTrackMom.SetXYZ(fMCtrk1->GetMomentum().X(), fMCtrk1->GetMomentum().Y(), fMCtrk1->GetMomentum().Z());
    if (fPpt->GetBarPointID() != -1) {
      fBarPoint = (PndDrcBarPoint *)fBarPointArray->At(fPpt->GetBarPointID());
      BarId = fBarPoint->GetDetectorID();
      fBarId = fBarPoint->GetBarId();
      fBoxId = fBarPoint->GetBoxId();
      // start vertex of the photon
      TVector3 StartVertex = fMCtrk->GetStartVertex();
      fTrackIniVertex = StartVertex;
      // initial direction of the photon in the bar coord system
      TVector3 PphoInitBar = fGeoH->MasterToLocalShortId(fMCtrk->GetMomentum(), BarId) - fGeoH->MasterToLocalShortId(TVector3(0., 0., 0.), BarId); // vector
    }

    // Double_t PPx= fPpt->GetPx(); //[R.K. 01/2017] unused variable?
    // Double_t PPy= fPpt->GetPy(); //[R.K. 01/2017] unused variable?
    // Double_t PPz= fPpt->GetPz(); //[R.K. 01/2017] unused variable?

    // Double_t etot = sqrt(PPx*PPx + PPy*PPy +PPz*PPz);// in GeV //[R.K. 01/2017] unused variable?
    // Double_t lambda=197.0*2.0*fpi/(etot*1.0E9);//wavelength of photon in nm //[R.K. 01/2017] unused variable?

    //####################################
    // transport efficiency
    if (BarId != -1) {
      fDetection = 1;
    }

    if (fDetection == 1) {

      // transform to local sensor system... (mc point has the ID not the path to the volume)
      TVector3 PptPosition;
      fPpt->Position(PptPosition);
      TVector3 posL = fGeoH->MasterToLocalShortId(PptPosition, fPpt->GetDetectorID()); // point
      TVector3 sensorDim = GetSensorDimensions(fPpt->GetDetectorID());

      // usually sensors have origin in the middle, let's move it to the left lower corner:
      TVector3 posLshifted = posL + sensorDim;

      // calculate the number of the fired pixel
      Int_t Ncol = (Int_t)TMath::Floor(posLshifted.X() / fPixelStep);
      Int_t Nrow = (Int_t)TMath::Floor(posLshifted.Y() / fPixelStep);
      Int_t NpixelLocal = Ncol + Nrow * fNpix;

      fDetectorID = fPpt->GetDetectorID() * 100 + NpixelLocal;
      ;
      Int_t mcpId = fPpt->GetMcpId();

      Int_t sensorId = mcpId * 100 + NpixelLocal;

      // time is smeared and digitized = has granularity
      fTime = (Int_t)(fPpt->GetTime() / fTimeGranularity) * fTimeGranularity;

      fTimeOfFlight = fTime;
      fEventTime = FairRootManager::Instance()->GetEventTime();
      if (fTimeSmearing == kTRUE)
        Smear(fTime, fSigmat);

      ActivatePixel(fDetectorID, sensorId, fTime, k, 0);

      if (fChargeSharing == kTRUE) {
        // find fired pixels:
        Double_t distance = 999.;
        TVector3 corner;
        TVector3 point;

        // left pixel
        distance = posLshifted.X() - TMath::Floor(posLshifted.X() / fPixelStep) * fPixelStep; //[cm]
        if (exp(-distance / fPixelSigma) > gRandom->Uniform(0., 1.) && exp(-distance / fPixelSigma) > fThreshold) {
          if ((Ncol - 1) >= 0) {
            ActivatePixel(fDetectorID, mcpId * 100 + Ncol - 1 + Nrow * fNpix, fTime, k, 1);
          }
        }
        // right pixel
        distance = TMath::Ceil(posLshifted.X() / fPixelStep) * fPixelStep - posLshifted.X();
        if (exp(-distance / fPixelSigma) > gRandom->Uniform(0., 1.) && exp(-distance / fPixelSigma) > fThreshold) {
          if (Ncol + 1 < fNpix) {
            ActivatePixel(fDetectorID, mcpId * 100 + Ncol + 1 + Nrow * fNpix, fTime, k, 1);
          }
        }
        // lower pixel
        distance = posLshifted.Y() - TMath::Floor(posLshifted.Y() / fPixelStep) * fPixelStep;
        if (exp(-distance / fPixelSigma) > gRandom->Uniform(0., 1.) && exp(-distance / fPixelSigma) > fThreshold) {
          if (Nrow - 1 >= 0) {
            ActivatePixel(fDetectorID, mcpId * 100 + Ncol + (Nrow - 1) * fNpix, fTime, k, 1);
          }
        }
        // upper pixel
        distance = TMath::Ceil(posLshifted.Y() / fPixelStep) * fPixelStep - posLshifted.Y();
        if (exp(-distance / fPixelSigma) > gRandom->Uniform(0., 1.) && exp(-distance / fPixelSigma) > fThreshold) {
          if (Nrow + 1 < fNpix) {
            ActivatePixel(fDetectorID, mcpId * 100 + Ncol + (Nrow + 1) * fNpix, fTime, k, 1);
          }
        }

        point.SetXYZ(posLshifted.X(), posLshifted.Y(), 0.);

        // upper left pixel
        corner.SetXYZ(TMath::Floor(posLshifted.X() / fPixelStep) * fPixelStep, TMath::Ceil(posLshifted.Y() / fPixelStep) * fPixelStep, 0.);
        distance = (point - corner).Mag();
        if (exp(-distance / fPixelSigma) > gRandom->Uniform(0., 1.) && exp(-distance / fPixelSigma) > fThreshold) {
          if (Ncol - 1 >= 0 && Nrow + 1 < fNpix) {
            ActivatePixel(fDetectorID, mcpId * 100 + Ncol - 1 + (Nrow + 1) * fNpix, fTime, k, 1);
          }
        }

        // bottom left pixel
        corner.SetXYZ(TMath::Floor(posLshifted.X() / fPixelStep) * fPixelStep, TMath::Floor(posLshifted.Y() / fPixelStep) * fPixelStep, 0.);
        distance = (point - corner).Mag();
        if (exp(-distance / fPixelSigma) > gRandom->Uniform(0., 1.) && exp(-distance / fPixelSigma) > fThreshold) {
          if (Ncol - 1 >= 0 && Nrow - 1 >= 0) {
            ActivatePixel(fDetectorID, mcpId * 100 + Ncol - 1 + (Nrow - 1) * fNpix, fTime, k, 1);
          }
        }

        // bottom right pixel
        corner.SetXYZ(TMath::Ceil(posLshifted.X() / fPixelStep) * fPixelStep, TMath::Floor(posLshifted.Y() / fPixelStep) * fPixelStep, 0.);
        distance = (point - corner).Mag();
        if (exp(-distance / fPixelSigma) > gRandom->Uniform(0., 1.) && exp(-distance / fPixelSigma) > fThreshold) {
          if (Ncol + 1 < fNpix && Nrow - 1 >= 0) {
            ActivatePixel(fDetectorID, mcpId * 100 + Ncol + 1 + (Nrow - 1) * fNpix, fTime, k, 1);
          }
        }

        // upper right pixel
        corner.SetXYZ(TMath::Ceil(posLshifted.X() / fPixelStep) * fPixelStep, TMath::Ceil(posLshifted.Y() / fPixelStep) * fPixelStep, 0.);
        distance = (point - corner).Mag();
        if (exp(-distance / fPixelSigma) > gRandom->Uniform(0., 1.) && exp(-distance / fPixelSigma) > fThreshold) {
          if (Ncol + 1 < fNpix && Nrow + 1 < fNpix) {
            ActivatePixel(fDetectorID, mcpId * 100 + Ncol + 1 + (Nrow + 1) * fNpix, fTime, k, 1);
          }
        }
      } // if charge sharing true
    }   // if detection
  }
}

// -----   Private method ActivatePixel   -------------------------------
void PndDrcTimeDigiTask::ActivatePixel(Int_t detectorId, Int_t sensorId, Double_t signalTime, Int_t k, Int_t csflag)
{
  // in case when the same pixel was fired by two different photons this function takes care of which hits from that pixel to write
  if (fPixelMap.find(sensorId) == fPixelMap.end()) {
    // pixel not yet active, create new digi

    PndDrcDigi *Digi = new PndDrcDigi(k, detectorId, sensorId, 0., fTimeOfFlight, csflag, 0.);
    Digi->SetTimeStamp(signalTime);
    Digi->SetTimeStampError(fSigmat / TMath::Sqrt(fSigmat));
    Digi->SetBarID(fBarId);
    Digi->SetBoxID(fBoxId);
    Digi->SetTrackID(fTrackId);
    Digi->SetTrackIniVertex(fTrackIniVertex);
    Digi->SetMotherID(fMrId);
    Digi->SetMotherIDPho(fMrIdPho);
    Digi->SetPdgCode(fPdg);
    Digi->SetTrackMom(fTrackMom);
    Digi->SetTimeAtBar(fTimeAtBar);
    Digi->SetEvtTim(signalTime);
    Digi->SetEventTim(fEventTime);
    Digi->SetEventNo(nevents);
    fDrcTimeDigiArray->FillNewData(Digi, signalTime, signalTime + fActiveTime);
    fPixelMap[sensorId] = fNDigis;
    fTimeMap[sensorId] = signalTime;
    fNDigis++;
    Digi->SetLink(FairLink("PndDrcPDPoint", k));
  } else {
    // Int_t iDigi = fPixelMap[sensorId]; //[R.K. 01/2017] unused variable?

    if (fabs(fTimeMap.find(sensorId)->second - signalTime) < 5.0) {
      fPileup++;
    }
    {
      PndDrcDigi *digi = new PndDrcDigi(k, detectorId, sensorId, 0., fTimeOfFlight, csflag, 0.);
      digi->SetTimeStamp(signalTime);
      digi->SetTimeStampError(fSigmat / TMath::Sqrt(fSigmat));
      digi->SetBarID(fBarId);
      digi->SetBoxID(fBoxId);
      digi->SetTrackID(fTrackId);
      digi->SetTrackIniVertex(fTrackIniVertex);
      digi->SetMotherID(fMrId);
      digi->SetMotherIDPho(fMrIdPho);
      digi->SetPdgCode(fPdg);
      digi->SetTrackMom(fTrackMom);
      digi->SetTimeAtBar(fTimeAtBar);
      digi->SetEvtTim(signalTime);
      digi->SetEventTim(fEventTime);
      digi->SetEventNo(nevents);
      fDrcTimeDigiArray->FillNewData(digi, signalTime, signalTime + fActiveTime);
      fPixelMap[sensorId] = fNDigis;
      fTimeMap[sensorId] = signalTime;
      fNDigis++;
      fPileup = 0.0;
    }
  }
}

//----------------------------------------------------------------------------------------------
Double_t PndDrcTimeDigiTask::FindPhiRot(Double_t xx, Double_t yy)
{ // returns [degrees]

  TVector3 hit;
  hit.SetXYZ(xx, yy, 0.);
  Double_t startPhi = hit.Phi() / fpi * 180.; // [degrees]
  if (startPhi < 0.) {
    startPhi = 360. + hit.Phi() * 180. / fpi;
  }

  Double_t PhiRot = 0.; //[degrees]
  if (startPhi >= 0. && startPhi < 90.) {
    PhiRot = TMath::Floor(startPhi / fdphi) * fdphi + fdphi / 2.;
  }
  if (startPhi >= 90. && startPhi < 270.) {
    PhiRot = 90. + fpipehAngle + TMath::Floor((startPhi - 90. - fpipehAngle) / fdphi) * fdphi + fdphi / 2.;
  }
  if (startPhi >= 270. && startPhi < 360.) {
    PhiRot = 270. + fpipehAngle + TMath::Floor((startPhi - 270. - fpipehAngle) / fdphi) * fdphi + fdphi / 2.;
  }
  // cout<<"-I- FindPhiRot: PhiRot = "<<PhiRot<<endl;
  return PhiRot; // degrees
}

//----------------------------------------------------------------------------------------------------------
Double_t PndDrcTimeDigiTask::FindOutPoint(Double_t x0, Double_t xEn, Double_t a, Double_t *NN, Bool_t print)
{
  Double_t m = 99.;
  Double_t n = TMath::Floor(x0 / a);
  m = n;
  if (print) {
    std::cout << "n = " << n << ", NN = " << *NN << ", x0 = " << x0 << ", a = " << a << std::endl;
  }
  Double_t x1 = x0 - n * a;
  if (x0 < 0.) {
    x1 = x0 - (n + 1) * a;
  }
  if (print) {
    std::cout << "xy = " << x1 << std::endl;
  }
  Double_t xK = 0.;
  if ((m / 2. - TMath::Floor(m / 2.)) == 0.) { // 4etnoe
    if (print) {
      std::cout << "odd==0" << std::endl;
    }
    if (x0 >= 0. && x1 + xEn <= a) {
      xK = x1 + xEn;
    }
    if (x0 >= 0. && x1 + xEn > a) {
      xK = 2 * a - x1 - xEn;
      n = 1. + n;
    }
    if (x0 < 0. && x1 + xEn >= 0.) {
      xK = a - (x1 + xEn);
      n = -1. - n;
    }
    if (x0 < 0. && x1 + xEn < 0.) {
      xK = a + x1 + xEn;
      n = -n;
    }
    if (print) {
      std::cout << "xK = " << xK << ", n = " << n << std::endl;
    }
  }

  if ((m / 2. - TMath::Floor(m / 2.)) != 0.) { // ne4etnoe
    if (print) {
      std::cout << "even!=0" << std::endl;
    }
    if (x0 >= 0. && x1 + xEn <= a) {
      xK = a - (x1 + xEn);
    }
    if (x0 >= 0. && x1 + xEn > a) {
      xK = x1 + xEn - a;
      n = 1. + n;
    }
    if (x0 < 0. && x1 + xEn >= 0.) {
      xK = x1 + xEn;
      n = -1. - n;
    }
    if (x0 < 0. && x1 + xEn < 0.) {
      xK = -(x1 + xEn);
      n = -n;
    }
    if (print) {
      std::cout << "xK = " << xK << ", n = " << n << std::endl;
    }
  }

  *NN = n;
  return xK;
}

// -----   Add Photon Detector Hit to HitCollection   --------------------------------------

PndDrcDigi *PndDrcTimeDigiTask::AddDrcDigi(Int_t index, Int_t iDetectorId, Double_t charge, Double_t time, Int_t csflag)
{
  TClonesArray &clrefPD = *fDigis;
  Int_t size = clrefPD.GetEntriesFast();
  return new (clrefPD[size]) PndDrcDigi(index, iDetectorId, iDetectorId, charge, time, csflag, 0.);
}

//-------------------------------------------------------------------------------
void PndDrcTimeDigiTask::SetFakeDetEff()
{
  if (fDetType == 1) {

    /** Quantum efficiency taken from old Burle data */
    flambda_min = 300.;
    flambda_max = 700.;
    flambda_step = 400.;

    fDetEfficiency[0] = 1.;
    fDetEfficiency[1] = 1.;
  } else {
    cout << "ERROR:    photocathode type not specified" << endl;

    flambda_min = 300.;
    flambda_max = 700.;
    flambda_step = 400.;

    fDetEfficiency[0] = 0.;
  }
}

//------------------------------------------------------------------------------------

Double_t PndDrcTimeDigiTask::FuncD1(Double_t x)
{

  // refraction index of fused silica as a function of wavelength
  Double_t par0 = 0.696;
  Double_t par1 = 0.068;
  Double_t par2 = 0.407;
  Double_t par3 = 0.116;
  Double_t par4 = 0.897;
  Double_t par5 = 9.896;

  return sqrt(1. + (par0 * x * x / (x * x - par1 * par1)) + (par2 * x * x / (x * x - par3 * par3)) + (par4 * x * x / (x * x - par5 * par5)));
}

Double_t PndDrcTimeDigiTask::FuncD3(Double_t x, Double_t y)
{
  // reflection probability according to the scalar theory (PhD Thesis R.Hohler)
  Double_t par0 = fRoughness;
  return 1. - pow(4. * 3.14159 * cos(y) * par0 * FuncD1(x) / x, 2);
}

//--------------------------------------------------------------
TVector3 PndDrcTimeDigiTask::GetSensorDimensions(Int_t sensorID)
{
  gGeoManager->cd(fGeoH->GetPath(sensorID));
  TGeoVolume *actVolume = gGeoManager->GetCurrentVolume();
  TGeoBBox *actBox = (TGeoBBox *)(actVolume->GetShape());
  TVector3 result(actBox->GetDX(), actBox->GetDY(), actBox->GetDZ());
  return result;
}

//-------------Set Parameter------------------------------------
void PndDrcTimeDigiTask::SetParameters()
{
  fDetType = 1;          //  Detector Type =1
  nRefrac = fGeo->nEV(); // 1.467;  //Refractive index of SOB
  //  fSigmat=0.1;  //Time Resolution is 100 ps ############################
  fCollectionEff = 0.65;    // Collection Efficiency
  fPackingFraction = 0.80;  // Packing Efficiency
  fRoughness = 0.001;       // 10 A
  fTimeGranularity = 0.001; // [ns] = 98 ps granularity of the time signal

  // basic DIRC parameters:
  fpi = TMath::Pi();
  fzup = fGeo->barBoxZUp();
  fzdown = fGeo->barBoxZDown();
  fradius = fGeo->radius();       // radius in the middle of the bar = 50.cm
  fhthick = fGeo->barHalfThick(); // half thickness of the bars=1.7/2 cm
  fpipehAngle = fGeo->PipehAngle();
  fbbGap = fGeo->BBoxGap();
  fbbnum = fGeo->BBoxNum();
  fbarnum = fGeo->barNum();
  fphi0 = (180. - 2. * fpipehAngle) / fbbnum + fpipehAngle;
  fdphi = (180. - 2. * fpipehAngle) / fbbnum * 2.;
  flside = fGeo->Lside();
  fbarwidth = fGeo->BarWidth();
  fPixelSize = fGeo->PixelSize();
  fMcpActiveArea = fGeo->McpActiveArea();
  fNpix = fGeo->Npixels();

  fPixelGap = (fMcpActiveArea - (Double_t)fNpix * fPixelSize) / ((Double_t)fNpix - 1.);
  fPixelStep = fMcpActiveArea / fNpix; // fPixelSize + 0.5*fPixelGap;

  fPixelSigma = fGeo->SigmaCharge();
  fDeadTime = fGeo->DeadTime();

  fThreshold = 0.20; // threshold to detect charge shared hits
}
//-------------Smear Time------------------------------------
void PndDrcTimeDigiTask::Smear(Double_t &time, Double_t sigt)
{
  Double_t EvtTime = FairRootManager::Instance()->GetEventTime();
  time += EvtTime + gRandom->Gaus(0, sigt);
}

// -----   Private method Reset   ------------------------------------------
void PndDrcTimeDigiTask::Reset()
{
  fNDigis = 0;
  fPixelMap.clear();
  if (fDrcDigiArray)
    fDrcDigiArray->Clear();
}

// -------------------------------------------------------------------------

// -----   Finish Task   ---------------------------------------------------
void PndDrcTimeDigiTask::Finish()
{
  if (fDrcDigiArray)
    fDrcDigiArray->Clear();
  LOG(info) << " PndDrcTimeDigiTask: Finish";
}
// -------------------------------------------------------------------------

ClassImp(PndDrcTimeDigiTask)
