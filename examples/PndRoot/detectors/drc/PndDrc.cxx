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
// -----                        PndDrc source file                     -----
// -----               Created 11/10/06 by Annalisa Cecchi             -----
// -----               Modified 2006++  by Carsten Schwarz             -----
// -----               Modified 2010++  by Maria Patsyuk               -----
// -----                                                               -----
// -------------------------------------------------------------------------

#include "PndGeoDrc.h"
#include "PndDrcPDPoint.h"
#include "PndDrcBarPoint.h"
#include "PndDrcEVPoint.h"
#include "PndGeoDrcPar.h"
#include "PndDetectorList.h"
#include "PndDrc.h"

#include "TString.h"
#include "TClonesArray.h"
#include "TVirtualMC.h"
#include "TObjArray.h"
#include "TGeoMCGeometry.h"
#include "TLorentzVector.h"
#include "TParticle.h"
#include "TVirtualMC.h"
#include "TGeoPgon.h"
#include "TGeoSphere.h"
#include "TGeoBBox.h"
#include "TGeoArb8.h"
#include "TGeoCone.h"
#include "TGeoTrd2.h"
#include "TGeoCompositeShape.h"
#include "TGeoMatrix.h"
#include "TGeoManager.h"
#include "TObject.h"
#include "TColor.h"

#include "TCanvas.h"
#include "TH1.h"
#include "TH2.h"

#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoNode.h"
#include "FairRootManager.h"
#include "FairVolume.h"
#include "FairGeoMedia.h"
#include "FairGeoMedium.h"
#include "FairGeoRootBuilder.h"
#include "FairLogger.h"
#include "PndStack.h"
#include "PndDetectorList.h"

// add on for debug
// #include "FairGeoG3Builder.h"
#include "FairRun.h"
// #include "FairRunSim.h"
#include "FairRuntimeDb.h"
#include "PndGeoHandling.h"

#include <cmath>

// -----   Default constructor   -------------------------------------------
PndDrc::PndDrc()
  : FairDetector("PndDrcDefault", kTRUE), fPersistency(kTRUE), fpi(TMath::Pi()), fzup(-999.), fzdown(-999.), fradius(-999.), fhthick(-999.), fpipehAngle(-999.), fbbGap(-999.),
    fbbnum(-999.), fbarnum(-999.), fphi0(-999.), fdphi(-999.), fbarwidth(-999.), fGeoH(nullptr), fRunCherenkov(kTRUE), fTrackID(-1), fPos(TLorentzVector(0, 0, 0, 0)),
    fMom(TLorentzVector(0, 0, 0, 0)), fTime(-1), fLength(-1), fAngIn(0), fNBar(0), fPosIndex(-1), volDetector(0), fMass(-1), fDetEffId(0), fDetEffAtProduction(kTRUE),
    fTransportEffAtProduction(kTRUE), fStopTime(kFALSE), fPhoMaxTime(-1), fTakeRealReflectivity(kFALSE), fStopSecondaries(kFALSE), fStopChargedTrackAfterDIRC(kFALSE),
    fSetBlackLens(kFALSE), fOptionForLUT(kFALSE), fGeo(new PndGeoDrc()), fPdgCode(-1), fThetaC(-1), //
    fEventID(0)
{
  fListOfSensitives.push_back("Sensor");
  if (fVerboseLevel > 0) {
    LOG(info) << "PndBarrelDIRC: fListOfSensitives contains:";
    for (size_t k = 0; k < fListOfSensitives.size(); k++)
      LOG(info) << "\n\t" << fListOfSensitives[k];
  }

  if (fGeoH == nullptr)
    fGeoH = PndGeoHandling::Instance();
}

// -----   Standard constructor   ------------------------------------------
PndDrc::PndDrc(const char *name, Bool_t active)
  : FairDetector(name, active), fPersistency(kTRUE), fpi(TMath::Pi()), fzup(-999.), fzdown(-999.), fradius(-999.), fhthick(-999.), fpipehAngle(-999.), fbbGap(-999.), fbbnum(-999.),
    fbarnum(-999.), fphi0(-999.), fdphi(-999.), fbarwidth(-999.), fGeoH(nullptr), fRunCherenkov(kTRUE), fTrackID(-1), fPos(TLorentzVector(0, 0, 0, 0)),
    fMom(TLorentzVector(0, 0, 0, 0)), fTime(-1), fLength(-1), fAngIn(0), fNBar(0), fPosIndex(-1), volDetector(0), fMass(-1), fDetEffId(0), fDetEffAtProduction(kTRUE),
    fTransportEffAtProduction(kTRUE), fStopTime(kFALSE), fPhoMaxTime(-1), fTakeRealReflectivity(kFALSE), fStopSecondaries(kFALSE), fStopChargedTrackAfterDIRC(kFALSE),
    fGeo(new PndGeoDrc()), fPdgCode(-1), fThetaC(-1),      //
    fEventID(0)
{
  fListOfSensitives.push_back("Sensor");
  if (fVerboseLevel > 0) {
    LOG(info) << "PndBarrelDIRC: fListOfSensitives contains:";
    for (size_t k = 0; k < fListOfSensitives.size(); k++)
      LOG(info) << "\n\t" << fListOfSensitives[k];
  }
  if (fGeoH == nullptr)
    fGeoH = PndGeoHandling::Instance();
}

PndDrc::PndDrc(const PndDrc& rhs)
    : FairDetector(rhs), fPersistency(kTRUE), fpi(TMath::Pi()), fzup(-999.), fzdown(-999.), fradius(-999.), fhthick(-999.), fpipehAngle(-999.), fbbGap(-999.), fbbnum(-999.),
    fbarnum(-999.), fphi0(-999.), fdphi(-999.), fbarwidth(-999.), fGeoH(nullptr), fRunCherenkov(kTRUE), fTrackID(-1), fPos(TLorentzVector(0, 0, 0, 0)),
    fMom(TLorentzVector(0, 0, 0, 0)), fTime(-1), fLength(-1), fAngIn(0), fNBar(0), fPosIndex(-1), volDetector(0), fMass(-1), fDetEffId(0), fDetEffAtProduction(kTRUE),
    fTransportEffAtProduction(kTRUE), fStopTime(kFALSE), fPhoMaxTime(-1), fTakeRealReflectivity(kFALSE), fStopSecondaries(kFALSE), fStopChargedTrackAfterDIRC(kFALSE),
    fGeo(new PndGeoDrc()), fPdgCode(-1), fThetaC(-1),      //
    fEventID(0)
{
  fListOfSensitives.push_back("Sensor");
  if (fGeoH == nullptr)
    fGeoH = PndGeoHandling::Instance();
}

// -----   Destructor   ----------------------------------------------------
PndDrc::~PndDrc()
{
  if (fGeo)
    delete fGeo;
}

// -----   Public method Intialize   ---------------------------------------
void PndDrc::Initialize()
{
  LOG(info) << "PndDrc: intialization started";
  FairDetector::Initialize();
  // FairRun       *sim  = FairRun::Instance();
  // FairRuntimeDb *rtdb = sim->GetRuntimeDb();
  // PndGeoDrcPar *par  = (PndGeoDrcPar*)(rtdb->getContainer("PndGeoDrcPar"));

  if (0 == gGeoManager)
    LOG(warning) << "PndDrc: gGeoManager is not initialized";

  fGeoH->CreateUniqueSensorId("", fListOfSensitives);
  if (fVerboseLevel > 0)
    fGeoH->PrintSensorNames();

  if (!fRunCherenkov)
    LOG(info) << "PndDrc: Cherenkov Propagation is OFF";

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
  fbarwidth = fGeo->BarWidth();

  // create a detector efficiency function:
  {
    // quantum efficiency data from Alex Britting, Jan 25, 2011
    // unit is percent
    // first value is at 200 nm, last at 700 nm
    // credible range start around 250nm, >= 280nm to be safe

    std::array<double, 502> eff_std = {
      0,     231.84, 615.36, 657.4, 258.78, 9839.92, 44.67, 67.87, 51.01, 41.49, 5.36,  49.4,  2.13,  35.49, 8.66,  5.03,  7.51,  13.27, 18.71, 3.92,  3.66,  8.2,   0.56,  7.68,
      2.87,  10.06,  3.47,   3.39,  6.99,   6.01,    4.92,  6.25,  5.97,  6.92,  8.29,  10.45, 8.68,  8.6,   9.79,  11.76, 9.53,  10.98, 9.9,   10.97, 11.31, 10.88, 10.78, 12.16,
      12.38, 12.37,  13.04,  12.36, 13.18,  13.7,    13.85, 13.66, 13.98, 14.55, 14.93, 14.82, 14.97, 14.98, 15.14, 15.35, 15.37, 15.43, 15.49, 15.59, 15.84, 15.84, 15.92, 16.01,
      16.22, 16.41,  16.42,  16.52, 16.86,  17.1,    17.17, 17.22, 17.46, 17.79, 17.99, 18.13, 18.33, 18.34, 18.53, 18.72, 18.95, 19.02, 19.15, 19.28, 19.45, 19.66, 19.69, 19.77,
      19.73, 19.95,  19.98,  20.17, 20.29,  20.33,   20.37, 20.47, 20.48, 20.57, 20.75, 20.8,  20.84, 20.86, 20.88, 21.0,  21.06, 21.0,  21.06, 21.06, 21.04, 21.1,  21.14, 21.08,
      21.17, 21.3,   21.38,  21.49, 21.58,  21.69,   21.77, 21.87, 22.02, 22.13, 22.29, 22.35, 22.45, 22.53, 22.55, 22.64, 22.67, 22.73, 22.74, 22.71, 22.79, 22.76, 22.77, 22.76,
      22.75, 22.78,  22.7,   22.68, 22.72,  22.66,   22.64, 22.7,  22.67, 22.71, 22.67, 22.75, 22.77, 22.83, 22.84, 22.93, 22.97, 23.0,  23.08, 23.16, 23.27, 23.25, 23.37, 23.44,
      23.49, 23.55,  23.52,  23.58, 23.64,  23.63,   23.58, 23.64, 23.63, 23.62, 23.64, 23.63, 23.66, 23.59, 23.59, 23.56, 23.58, 23.63, 23.57, 23.66, 23.62, 23.67, 23.64, 23.54,
      23.57, 23.51,  23.53,  23.45, 23.3,   23.41,   23.25, 23.21, 23.08, 23.01, 22.92, 22.9,  22.76, 22.76, 22.61, 22.53, 22.48, 22.39, 22.29, 22.24, 22.2,  22.12, 22.07, 21.96,
      21.89, 21.87,  21.76,  21.74, 21.58,  21.49,   21.48, 21.37, 21.29, 21.2,  21.17, 21.03, 20.98, 20.92, 20.85, 20.76, 20.69, 20.58, 20.56, 20.47, 20.37, 20.32, 20.24, 20.13,
      20.08, 19.9,   19.84,  19.77, 19.69,  19.63,   19.51, 19.41, 19.27, 19.06, 19.01, 18.87, 18.7,  18.49, 18.41, 18.17, 17.98, 17.84, 17.69, 17.5,  17.25, 17.15, 16.98, 16.79,
      16.66, 16.48,  16.32,  16.19, 16.02,  15.88,   15.77, 15.67, 15.5,  15.39, 15.23, 15.09, 15.04, 14.92, 14.75, 14.7,  14.5,  14.45, 14.34, 14.25, 14.16, 14.13, 14.0,  13.92,
      13.84, 13.76,  13.73,  13.61, 13.54,  13.52,   13.45, 13.41, 13.39, 13.31, 13.22, 13.17, 13.13, 13.06, 13.2,  13.09, 12.97, 12.92, 12.73, 12.65, 12.4,  12.22, 12.02, 11.79,
      11.59, 11.33,  11.03,  10.68, 10.46,  10.14,   9.88,  9.62,  9.36,  9.14,  8.87,  8.63,  8.51,  8.24,  8.07,  7.88,  7.77,  7.65,  7.52,  7.35,  7.27,  7.21,  7.1,   6.92,
      6.89,  6.79,   6.74,   6.56,  6.54,   6.5,     6.39,  6.33,  6.25,  6.27,  6.14,  6.06,  6.04,  6.01,  5.91,  5.89,  5.79,  5.75,  5.75,  5.67,  5.61,  5.51,  5.52,  5.43,
      5.43,  5.34,   5.31,   5.35,  5.23,   5.2,     5.14,  5.11,  5.11,  5.01,  4.98,  4.93,  4.99,  4.89,  4.82,  4.87,  4.8,   4.7,   4.65,  4.65,  4.61,  4.49,  4.56,  4.44,
      4.42,  4.44,   4.35,   4.35,  4.27,   4.29,    4.19,  4.13,  4.08,  4.02,  4.07,  3.92,  3.95,  3.88,  3.82,  3.86,  3.74,  3.71,  3.66,  3.72,  3.62,  3.55,  3.56,  3.57,
      3.45,  3.38,   3.36,   3.36,  3.28,   3.25,    3.19,  3.26,  3.13,  3.17,  3.15,  3.04,  2.98,  2.93,  2.98,  2.9,   2.89,  2.9,   2.81,  2.74,  2.81,  2.68,  2.73,  2.7,
      2.57,  2.58,   2.55,   2.55,  2.37,   2.39,    2.39,  2.44,  2.37,  2.26,  2.27,  2.27,  2.23,  2.26,  2.14,  2.08,  2.15,  2.06,  2.09,  2.04,  2.0,   1.95,  2.02,  1.87,
      1.9,   1.8,    1.87,   1.85,  1.87,   1.81,    1.86,  1.74,  1.74,  1.63,  1.59,  1.5,   1.5,   1.44,  1.47,  1.32,  1.24,  1.28,  1.19,  1.21,  1.21,  1.1,   1.1,   1.05,
      1.06,  0.94,   0.92,   0.87,  0.92,   0.81,    0.86,  0.78,  0.77,  0.8,   0.67,  0.7,   0.81,  0.61,  0.64,  0.71,  0.66,  0.67,  0.68,  0.69,  0.68,  0.73};
    std::array<double, 251> eff_400 = {
      0,    0,    14.0, 14.8, 14.5, 14.9, 14.4, 14.2, 13.9, 14.6, 15.2, 15.7, 16.4, 16.9, 17.5, 17.7, 18.1, 18.8, 19.3, 19.8, 20.6, 21.4, 22.4, 23.1, 23.6, 24.1, 24.2, 24.6,
      24.8, 25.2, 25.7, 26.5, 27.1, 28.2, 29.0, 29.9, 30.8, 31.1, 31.7, 31.8, 31.6, 31.5, 31.5, 31.3, 31.0, 30.8, 30.8, 30.4, 30.2, 30.3, 30.2, 30.1, 30.1, 30.1, 29.8, 29.9,
      29.8, 29.7, 29.7, 29.7, 29.8, 29.8, 29.9, 29.9, 29.8, 29.9, 29.8, 29.9, 29.8, 29.7, 29.8, 29.7, 29.8, 29.6, 29.5, 29.7, 29.7, 29.8, 30.1, 30.4, 31.0, 31.3, 31.5, 31.8,
      31.8, 31.9, 32.0, 32.0, 32.0, 32.0, 32.2, 32.2, 32.1, 31.8, 31.8, 31.8, 31.7, 31.6, 31.6, 31.7, 31.5, 31.5, 31.4, 31.3, 31.3, 31.2, 30.8, 30.7, 30.5, 30.3, 29.9, 29.5,
      29.3, 29.2, 28.6, 28.2, 27.9, 27.8, 27.3, 27.0, 26.6, 26.1, 25.9, 25.5, 25.0, 24.6, 24.2, 23.8, 23.4, 23.0, 22.7, 22.4, 21.9, 21.4, 21.2, 20.7, 20.3, 19.8, 19.6, 19.3,
      18.9, 18.7, 18.3, 17.9, 17.8, 17.8, 16.7, 16.5, 16.4, 16.0, 15.6, 15.6, 15.2, 14.9, 14.6, 14.4, 14.1, 13.8, 13.6, 13.3, 13.0, 12.8, 12.6, 12.3, 12.0, 11.9, 11.7, 11.5,
      11.2, 11.1, 10.9, 10.7, 10.4, 10.3, 9.9,  9.8,  9.6,  9.3,  9.1,  9.0,  8.8,  8.5,  8.3,  8.3,  8.2,  7.9,  7.8,  7.7,  7.5,  7.3,  7.1,  6.9,  6.7,  6.6,  6.3,  6.2,
      6.0,  5.8,  5.7,  5.6,  5.4,  5.2,  5.1,  4.9,  4.8,  4.6,  4.5,  4.4,  4.2,  4.1,  4.0,  3.8,  3.7,  3.5,  3.3,  3.2,  3.1,  3.0,  2.9,  2.5,  2.4,  2.4,  2.3,  2.3,
      2.1,  1.8,  1.6,  1.5,  1.5,  1.6,  1.8,  1.9,  1.4,  0.8,  0.9,  0.8,  0.7,  0.6,  0.3,  0.3,  0.5,  0.3,  0.4,  0.3,  0.1,  0.2,  0.1,  0.2,  0.3,  0.0};

    std::array<std::array<double, 1000>, 2> lambda{};
    std::array<std::array<double, 1000>, 2> efficiency{};
    double collectionEff;

    // still need to convert from percent and cut values below credible limit
    for (int i = 1; i < 1000; i++) {
      // Std QE (Photonis 2)
      collectionEff = 0.95;
      lambda[0][i] = 200 + i - 1;
      efficiency[0][i] = (i > 40 && i < 500) ? eff_std[i] * 0.01 * collectionEff : 0;

      // HIQ 400 (Photonis 1)
      collectionEff = 0.95;
      lambda[1][i] = 180 + (i - 1) * 2;
      efficiency[1][i] = (i < 251) ? eff_400[i] * 0.01 * collectionEff : 0;
    }

    fDetEff[0] = TGraph(1000, lambda[0].data(), efficiency[0].data());
    fDetEff[1] = TGraph(1000, lambda[1].data(), efficiency[1].data());
  }

  if (fDetEffId > 10) {
    fDetEffAtProduction = kFALSE;
    fDetEffId = fDetEffId - 10;
  } else {
    fDetEffAtProduction = kTRUE;
  }
  LOG(info) << "PndDrc: intialization successfull";
}

// -------------------------------------------------------------------------
void PndDrc::BeginEvent()
{
  if (fVerboseLevel > 0)
    LOG(info) << "PndDrc: new event";
  fEventID++;
}

// TH1F *hSpec = new TH1F("hSpec","hSpec",1000,1,10);

// -----   Public method ProcessHits  --------------------------------------
Bool_t PndDrc::ProcessHits(FairVolume *vol)
{
  TString nam = vol->GetName();
  Int_t num = vol->getMCid();

  fEventID = gMC->CurrentEvent();
  fPdgCode = gMC->TrackPid();
  fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
  fTime = gMC->TrackTime() * 1.0e09;
  fLength = gMC->TrackLength();
  gMC->TrackPosition(fPos);
  gMC->TrackMomentum(fMom);

  // // store data at each reflection
  // if(nam.BeginsWith("DrcBarSensor") && gMC->IsTrackEntering() && fPdgCode == 50000050){
  //   TString path = gMC->CurrentVolPath();
  //   Int_t barId = fGeoH->GetShortID(path);
  //   TVector3 mom = fMom.Vect();// - fGeoH->LocalToMasterShortId(TVector3(0,0,0), barId);
  //   //fGeoH->MasterToLocalShortId(fMom.Vect(), barId);//- fGeoH->MasterToLocalShortId(TVector3(0,0,0), barId);

  //   mom = mom.Unit();
  //   TVector3 pos = fPos.Vect();
  //   //fGeoH->MasterToLocalShortId(fPos.Vect(), barId)- fGeoH->MasterToLocalShortId(TVector3(0,0,0), barId);
  //   AddBarHit(fTrackID, fGeoH->GetShortID(path), pos, mom,
  // 	      fTime, fLength, fPdgCode, 0, 0, fEventID, 0);
  //   PndStack* stack = (PndStack*) gMC->GetStack();
  //   stack->AddPoint(DetectorId::kDRC);
  // }
  // //if(fDrcPDCollection->GetEntriesFast()>0) gMC->StopTrack();

  // if(nam.BeginsWith("DrcBar") && gMC->IsTrackEntering() && fPdgCode == 50000050){
  //   hSpec->Fill(fMom.Vect().Mag()*1e9);
  //   gMC->StopTrack();
  // }

  // // print out info about the charged particle:
  // if(gMC->GetStack()->GetCurrentParentTrackNumber()==0 && fPdgCode == 11){
  //   Int_t nproc = gMC->StepProcesses(fProc);
  //   for(Int_t ii=0; ii<nproc; ii++){
  //     LOG(info)<<"track "<<fPdgCode<<" number "<<gMC->GetStack()->GetCurrentTrackNumber()<<" mother id = "<<gMC->GetStack()->GetCurrentParentTrackNumber()<<": Z pos
  //     "<<fPos.Z()<<", R =  "<<sqrt(pow(fPos.X(),2) + pow(fPos.Y(),2))<<", "<<ii<<" - "<<fProc[ii]<<", "<<TMCProcessName[fProc[ii]];
  //   }
  // }

  // stop secondaries so that they do not produce Cherenkov photons
  if (fStopSecondaries) {
    if (fPdgCode != 50000050) {
      if (gMC->GetStack()->GetCurrentParentTrackNumber() != -1) {
        if (gMC->IsNewTrack())
          gMC->StopTrack();
      }
    }
  }

  // stop the track after the DIRC:
  if (fStopChargedTrackAfterDIRC) {
    if (fPdgCode != 50000050 && gMC->IsTrackExiting() && num == gMC->VolId("DrcBarSensor")) {
      LOG(info) << "PndDrc: track is stopped after the Barrel DIRC";
      gMC->StopTrack();
    }
    if (nam.BeginsWith("DrcEVSensor") && gMC->IsTrackExiting() && fMom.Z() > 0.)
      gMC->StopTrack();
  }

  if (fPdgCode == 50000050) {
    if (fRunCherenkov == kFALSE) { //|| fabs(fMom.Vect().Mag()*1.0E9-3.18)>0.2
      if (fVerboseLevel > 0)
        LOG(info) << "PndDrc: photon killed";
      gMC->StopTrack();
    }

    // apply detector efficiency at the production stage:
    if (fDetEffAtProduction && fDetEffId < 2 && gMC->IsNewTrack()) {
      Double_t lambda = 197.0 * 2.0 * fpi / (fMom.Vect().Mag() * 1.0E9);
      Double_t ra = gRandom->Uniform(0., 1.);
      if (ra > fDetEff[fDetEffId].Eval(lambda))
        gMC->StopTrack();
    }

    // if the photon goes backward through the lens, stop it
    if (gMC->IsTrackExiting() && nam.Contains("LENS")) {
      if (fMom.Z() > 0.)
        gMC->StopTrack();
    }

    if (fOptionForLUT && fMom.Z() > 0.)
      gMC->StopTrack();

    // apply transport efficiency at production stage (Maria Patsyuk 20.04.2012):
    if (fTransportEffAtProduction && gMC->IsNewTrack()) {
      Double_t lam_tr = (197.0 * 2.0 * fpi / (fMom.Vect().Mag() * 1.0E9)) / 1000.;

      // current volume (should be the radiator bar)
      Int_t barId = fGeoH->GetShortID(gMC->CurrentVolPath());
      // calculate the number of bounces:
      Int_t NbouncesX, NbouncesY;
      Double_t angleX, angleY;
      // photon initial direction
      TVector3 PphoInitBar = fGeoH->MasterToLocalShortId(fMom.Vect(), barId) - fGeoH->MasterToLocalShortId(TVector3(0., 0., 0.), barId); // vector
      NumberOfBounces(fPos.Vect(), PphoInitBar, barId, &NbouncesX, &NbouncesY, &angleX, &angleY);
      // calculate the bounce probability
      Double_t n_quartz = sqrt(1. + (0.696 * lam_tr * lam_tr / (lam_tr * lam_tr - pow(0.068, 2))) + (0.407 * lam_tr * lam_tr / (lam_tr * lam_tr - pow(0.116, 2))) +
                               0.897 * lam_tr * lam_tr / (lam_tr * lam_tr - pow(9.896, 2)));
      Double_t bounce_probX = 1. - pow(4. * fpi * cos(angleX) * fGeo->Roughness() * n_quartz / lam_tr, 2);
      Double_t bounce_probY = 1. - pow(4. * fpi * cos(angleY) * fGeo->Roughness() * n_quartz / lam_tr, 2);

      Double_t TotalTrProb = pow(bounce_probX, (Int_t)NbouncesX) * pow(bounce_probY, (Int_t)NbouncesY);
      Double_t ra_tr = gRandom->Uniform(0., 1.);
      if (ra_tr > TotalTrProb)
        gMC->StopTrack();
    }

    // kill photons older than fPhoMaxTime:
    if (fStopTime == kTRUE && gMC->TrackTime() * 1.0e09 > fPhoMaxTime) {
      gMC->StopTrack();
    }

    if (gMC->IsTrackEntering() && num == gMC->VolId("DrcEVSensor")) {
      gMC->TrackMomentum(fMomAtEV);
    }

    if (gMC->IsTrackExiting()) { // IsTrackEntering
      if (gMC->IsNewTrack())
        fTimeStart = fTime; // charged particle time at entrance of radiator bar

      // if(nam.BeginsWith("DrcLENS1Sensor")){  //DrcEntranceBox

      // // save the direction of the photon when it enters EntranceBox (for LUT generation)
      // Double_t nx,ny,nz;
      // bool bres = gMC->CurrentBoundaryNormal(nx,ny,nz);
      // AddEVHit(fTrackID, 9375, fPos.Vect(), fMom.Vect().Unit(),
      // 	fTime, fLength, fPdgCode,
      // 	 fEventID, fTimeStart, fTimeAtEVEntrance, fVeloPhoton, TVector3(nx,ny,nz));
      // }

      if (nam.BeginsWith("DrcEVSensor") || nam.BeginsWith("DrcLENS3Sensor")) {
        if (fTimeAtEVEntrance == 0.) {
          fTimeAtEVEntrance = gMC->TrackTime() * 1.0e09;
          fLengthEV = fLength;
        }
        fVeloPhoton = fLength / (fTime - fTimeStart);

        Double_t nmast[3];
        gMC->CurrentBoundaryNormal(nmast[0], nmast[1], nmast[2]);
        // TVector3 lnorm = fGeoH->MasterToLocalShortId(TVector3(nx,ny,nz), fGeoH->GetShortID(gMC->CurrentVolPath()));

        Double_t result[3];
        gGeoManager->MasterToLocalVect(nmast, result);
        TVector3 lnorm = TVector3(result[0], result[1], result[2]);

        // if(bres)
        AddEVHit(fTrackID, 0, fPos.Vect(), fMom.Vect(), fTime, fLength, fPdgCode, fEventID, fTimeStart, fTimeAtEVEntrance, fVeloPhoton, lnorm);

        fTimeAtEVEntrance = 0.0;
      }
    }

    // if a photon is exiting the readout bar end
    if (gMC->IsTrackExiting() && num == gMC->VolId("DrcBarSensor") && fPos.Z() < -118.99) {
      fTimeAtEV = gMC->TrackTime() * 1.0e09;

      gMC->TrackPosition(fAna_point);
      TVector3 lnorm = fGeoH->MasterToLocalShortId(fAna_point.Vect(), fGeoH->GetShortID(gMC->CurrentVolPath()));
      fAna_point.SetVect(lnorm);
    }

    if (gMC->IsTrackEntering()) {
      if (nam.BeginsWith("DrcCathodeSensor")) {
        if (0 == fGeoH) {
          LOG(fatal) << "PndDrc: no PndGeoHandling loaded." << std::endl;
          abort();
        }
        Int_t sensorId = fGeoH->GetShortID(gMC->CurrentVolPath());
        Int_t mcpId, prismId;
        sscanf(gMC->CurrentVolPath(), "/cave_1/BarrelDIRC_0/DrcPDbase_%d/DrcMCP_%d", &prismId, &mcpId);
        if (fTrackID > -2) {
          bool savehit = true;

          if (!fDetEffAtProduction && fDetEffId != 6) {
            Double_t lambda = 197.0 * 2.0 * fpi / (fMom.Vect().Mag() * 1.0E9);
            Double_t ra = gRandom->Uniform(0, 1);
            int qe = fDetEffId;
            if (fDetEffId == 5) {
              if (mcpId == 4 || mcpId == 5)
                qe = 1;
              else
                qe = 0;
            }
            if (ra > fDetEff[qe].Eval(lambda))
              savehit = false;
          }

          if (savehit || fDetEffId == 6) {
            // AddHit(fTrackID, sensorId, mcpId,
            // 	   fPos.Vect(),fMom.Vect(),fMomAtEV.Vect(), fTimeAtEV,
            // 	   fTime, fLength, fPdgCode, fEventID);
            AddHit(fTrackID, sensorId, mcpId, fPos.Vect(), fMom.Vect(), fAna_point.Vect(), fTimeAtEV, fTime, fLength, fPdgCode, fEventID);
          }
        }
        gMC->StopTrack();
      }
      PndStack *stack = (PndStack *)gMC->GetStack();
      stack->AddPoint(PndDetectorId::kDRC);
    }
  }

  if (gMC->TrackCharge() != 0 || fOptionForLUT) {
    if (nam.BeginsWith("DrcBar") && gMC->IsTrackEntering()) {

      bool bpass = true;
      // if(fDrcBarCollection->GetEntriesFast()>0){
      // 	PndDrcBarPoint *tBarPoint = (PndDrcBarPoint*) fDrcBarCollection->At(fDrcBarCollection->GetEntriesFast()-1);
      // 	if(tBarPoint->GetTrackID()==fTrackID && fLength - tBarPoint->GetLength()<0.1) bpass = false;
      // }

      if (bpass && nam.BeginsWith("DrcBarSensor")) {
        Int_t s = 0, b = 0; // side and bar
        fNBar = 0;
        TString path = gMC->CurrentVolPath();
        if (fVerboseLevel > 1)
          LOG(info) << "PndDrc: volume name is" << nam;
        sscanf(path, "/cave_1/BarrelDIRC_0/DrcBarBox_%d/DrcBarBoxCover_0/DrcBarBoxAir_0/DrcBarSensor_%d", &s, &b);

        if (s < 17)
          fNBar = s * 10 + b;
        else
          LOG(warning) << "PndDrc: wrong BarBox Id" << s;

        TVector3 barMom = fMom.Vect();
        Double_t fP = barMom.Mag();
        fMass = gMC->TrackMass();
        Double_t fEnergy = TMath::Sqrt(fP * fP + fMass * fMass);

        if (fP == 0. || fabs(1. / (fGeo->nQuartz() * (fP / fEnergy))) > 1) {
          fThetaC = -1;
        } else {
          fThetaC = acos(1 / (fGeo->nQuartz() * (fP / fEnergy)));
        }

        AddBarHit(fTrackID, fGeoH->GetShortID(path), fPos.Vect(), barMom, fTime, fLength, fPdgCode, fThetaC, fNBar, fEventID, fMass);

        PndStack *stack = (PndStack *)gMC->GetStack();
        stack->AddPoint(PndDetectorId::kDRC);
      }
    }
  }

  fTrackID = -999;
  fPos.SetXYZT(-999., -999., -999., -999.);
  fMom.SetXYZT(-999., -999., -999., -999.);
  fTime = -999;
  fLength = -999;
  fPdgCode = -999;
  fAngIn = -999;
  fThetaC = -999;
  fNBar = -999;
  fMass = -999;

  return kTRUE;
}

void PndDrc::FinishPrimary() {}

//------   Find Nubmer of Bounces     -----------------------------------------
void PndDrc::NumberOfBounces(TVector3 start, TVector3 dir, Int_t barId, Int_t *n1, Int_t *n2, Double_t *alpha1, Double_t *alpha2)
{
  // start - photon production point in global coord system
  // dir - photon direction in bar coord system

  // calculates the number of bounces in x and y direction and reflection angles in these directions.

  // Find coordinates of X0, Y0:
  Double_t Z0, X0, Y0;
  if (dir.Theta() < 3.1415 / 2.) {
    Z0 = -(fabs(fzup) + 2. * fzdown - start.Z());
  }
  if (dir.Theta() >= 3.1415 / 2.) {
    Z0 = -(start.Z() - fzup);
  }
  X0 = Z0 * tan(dir.Theta()) * cos(dir.Phi());
  Y0 = Z0 * tan(dir.Theta()) * sin(dir.Phi());

  // Find the start position of the photon with respect to the middle of the bar:
  TVector3 startLocal = fGeoH->MasterToLocalShortId(start, barId); // point

  // Find the number of bounces in each direction
  Double_t N1, N2;
  FindOutPoint(X0, startLocal.X() + fbarwidth / 2., fbarwidth, &N1, 0);
  FindOutPoint(Y0, startLocal.Y() + fhthick, 2. * fhthick, &N2, 0);

  *n1 = (Int_t)N1;
  *n2 = (Int_t)N2;

  // calculate the reflection angles in x and y directions:
  TVector3 up_down;
  up_down.SetXYZ(0., 1., 0.);
  TVector3 left_right;
  left_right.SetXYZ(1., 0., 0.);
  Double_t angle1 = dir.Angle(left_right);
  if (angle1 > fpi / 2.) {
    angle1 = fpi - dir.Angle(left_right);
  }
  Double_t angle2 = dir.Angle(up_down);
  if (angle2 > fpi / 2.) {
    angle2 = fpi - dir.Angle(up_down);
  }
  *alpha1 = angle1;
  *alpha2 = angle2;
}

//-----------------------------------------------------------------------------
Double_t PndDrc::FindOutPoint(Double_t x0, Double_t xEn, Double_t a, Double_t *NN, Bool_t print)
{
  Double_t m = 99.;
  Double_t n = TMath::Floor(x0 / a);
  m = n;
  if (print)
    LOG(info) << "n = " << n << ", NN = " << *NN << ", x0 = " << x0 << ", a = " << a;

  Double_t x1 = x0 - n * a;
  if (x0 < 0.) {
    x1 = x0 - (n + 1) * a;
  }
  if (print)
    LOG(info) << "xy = " << x1;

  Double_t xK = 0.;
  if ((m / 2. - TMath::Floor(m / 2.)) == 0.) { // 4etnoe
    if (print)
      LOG(info) << "odd==0";
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
    if (print)
      LOG(info) << "xK = " << xK << ", n = " << n;
  }

  if ((m / 2. - TMath::Floor(m / 2.)) != 0.) { // ne4etnoe
    if (print)
      LOG(info) << "even!=0";
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
    if (print)
      LOG(info) << "xK = " << xK << ", n = " << n;
  }

  *NN = n;
  return xK;
}

// -----   Public method EndOfEvent   -----------------------------------------
void PndDrc::EndOfEvent()
{
  if (fVerboseLevel)
    Print();
  Reset();
}

void PndDrc::FinishRun()
{
  //  hSpec->Draw();
}

// -----   Public method Register   -------------------------------------------
void PndDrc::Register()
{
  FairRootManager::Instance()->RegisterAny("DrcBarPoint", fBarPointVector, kTRUE);
  FairRootManager::Instance()->RegisterAny("DrcEVPoint", fEVPointVector, kTRUE);
  FairRootManager::Instance()->RegisterAny("DrcPDPoint", fPDPointVector, kTRUE);
}

// -----   Public method GetCollection   --------------------------------------
TClonesArray *PndDrc::GetCollection(Int_t iColl) const
{
  return nullptr;
}

// -----   Public method Print   ----------------------------------------------
void PndDrc::Print() const
{
}

// -----   Public method Reset   ----------------------------------------------
void PndDrc::Reset()
{
    fBarPointVector->clear();
    fEVPointVector->clear();
    fPDPointVector->clear();
  fPosIndex = 0;
}

// -----   Public method CopyClones   -----------------------------------------
void PndDrc::CopyClones(TClonesArray *clPD1, TClonesArray *clPD2, TClonesArray *clBar1, TClonesArray *clBar2, Int_t offset)
{
  Int_t nPDEntries = clPD1->GetEntriesFast();
  LOG(info) << "PndDrc: " << nPDEntries << " entries to add";
  TClonesArray &clrefPD = *clPD2;

  Int_t nBarEntries = clBar1->GetEntriesFast();
  LOG(info) << "PndDrc: " << nBarEntries << " entries to add";
  TClonesArray &clrefBar = *clBar2;

  PndDrcPDPoint *oldpointPD = nullptr;
  PndDrcBarPoint *oldpointBar = nullptr;

  for (Int_t i = 0; i < nPDEntries; i++) {
    oldpointPD = (PndDrcPDPoint *)clPD1->At(i);
    Int_t indexPD = oldpointPD->GetTrackID() + offset;
    oldpointPD->SetTrackID(indexPD);
    new (clrefPD[fPosIndex]) PndDrcPDPoint(*oldpointPD);
    fPosIndex++;
  }

  for (Int_t i = 0; i < nBarEntries; i++) {
    oldpointBar = (PndDrcBarPoint *)clBar1->At(i);
    Int_t indexBar = oldpointBar->GetTrackID() + offset;
    oldpointBar->SetTrackID(indexBar);
    new (clrefBar[fPosIndex]) PndDrcBarPoint(*oldpointBar);
    fPosIndex++;
  }

  LOG(info) << " PndDrc: " << clPD2->GetEntriesFast() << " merged entries";
  LOG(info) << " PndDrc: " << clBar2->GetEntriesFast() << " merged entries";
}

// -----   Public method ConstructGeometry  -----------------------------------
void PndDrc::ConstructGeometry()
{
  LOG(info) << " =======  DRC::  ConstructGeometry()  ======== ";

  TString fileName = GetGeometryFileName();
  if (fileName.EndsWith(".root")) {
    ConstructRootGeometry();
  } else {
    LOG(warning) << "geometry format not supported!";
  }
}

// -----   Public Method Construct Optical Geometry ---------------------------
void PndDrc::ConstructOpGeometry()
{
  LOG(info) << "PndDrc: constructing optical geometry";

  std::array<double, 2> ephoton_i = {1.907e-09, 6.199e-09}; // 1-10 eV
  std::array<double, 2> reflectivity_i = {1, 1};            // ideal reflectivity
  std::array<double, 2> reflectivity_b = {0, 0};            // no reflectivity

  // real reflectivity (added 18.05.2011, measured by Jerry for BABAR):
  std::array<double, 46> ephoton_r = {1.907e-09, 1.937e-09, 1.968e-09, 2.000e-09, 2.033e-09, 2.066e-09, 2.101e-09, 2.138e-09, 2.175e-09, 2.214e-09, 2.254e-09, 2.296e-09,
                                      2.339e-09, 2.384e-09, 2.431e-09, 2.480e-09, 2.530e-09, 2.583e-09, 2.638e-09, 2.695e-09, 2.755e-09, 2.818e-09, 2.883e-09, 2.952e-09,
                                      3.024e-09, 3.100e-09, 3.179e-09, 3.263e-09, 3.351e-09, 3.444e-09, 3.542e-09, 3.647e-09, 3.757e-09, 3.875e-09, 3.999e-09, 4.133e-09,
                                      4.275e-09, 4.428e-09, 4.592e-09, 4.769e-09, 4.959e-09, 5.166e-09, 5.391e-09, 5.636e-09, 5.904e-09, 6.199e-09};

  std::array<double, 46> reflectivity_r = {0.870, 0.880, 0.885, 0.890, 0.895, 0.900, 0.905, 0.910, 0.915, 0.920, 0.923, 0.925, 0.926, 0.928, 0.930, 0.935,
                                           0.936, 0.937, 0.938, 0.940, 0.940, 0.939, 0.938, 0.938, 0.937, 0.937, 0.936, 0.935, 0.934, 0.932, 0.930, 0.928,
                                           0.926, 0.924, 0.922, 0.920, 0.910, 0.905, 0.895, 0.890, 0.885, 0.860, 0.840, 0.820, 0.800, 0.780};

  gMC->DefineOpSurface("LensSurface", kGlisur, kDielectric_dielectric, kGround, 0.0);
  gMC->DefineOpSurface("MirrSurface", kGlisur, kDielectric_metal, kPolished, 0.0);
  gMC->DefineOpSurface("EVSurface", kGlisur, kDielectric_metal, kPolished, 0.0);
  gMC->DefineOpSurface("BlackSurface", kGlisur, kDielectric_dielectric, kPolished, 0.0);
  gMC->DefineOpSurface("DrcChamferSurface", kUnified, kDielectric_dielectric, kGround, 1.0);

  gMC->SetMaterialProperty("BlackSurface", "REFLECTIVITY", ephoton_i.size(), ephoton_i.data(), reflectivity_b.data());

  if (fTakeRealReflectivity == kFALSE) {
    gMC->SetMaterialProperty("MirrSurface", "REFLECTIVITY", ephoton_i.size(), ephoton_i.data(), reflectivity_i.data());
  } else {
    gMC->SetMaterialProperty("MirrSurface", "REFLECTIVITY", ephoton_r.size(), ephoton_r.data(), reflectivity_r.data());
  }

  gMC->SetBorderSurface("BarMirrorSurface", "DrcMirror", 0, "DrcBarBoxAir", 0, "MirrSurface");

  for (Int_t i = 0; i < 3; i++) {
    gMC->SetBorderSurface("DrcBar_ChamferSurface", "DrcBarSensor", i, "DrcBarChamfer", i, "DrcChamferSurface");
  }

  if (fSetBlackLens == kTRUE) {
    for (Int_t i = 0; i < 3; i++) { // fGeo->barNum()
      gMC->SetBorderSurface("Lens1AirSurface", "DrcLENS1Sensor", i, "DrcEntrance", 0, "BlackSurface");
      gMC->SetBorderSurface("Lens2AirSurface", "DrcLENS2Sensor", i, "DrcEntrance", 0, "BlackSurface");
      // gMC->SetBorderSurface("Lens3AirSurface", "DrcLENS3Sensor", i, "DrcEntrance", 0, "BlackSurface");
    }
    // gMC->SetBorderSurface("BarboxWindowAirSurface", "DrcBarboxWindowSensor", 0, "BarrelDIRC", 0, "EVSurface");
    // gMC->SetBorderSurface("EVGreaseAirSurface", "DrcEVgrease", 0, "BarrelDIRC", 0, "EVSurface");
  }

  // only direct
  // gMC->SetBorderSurface("EVAirSurface", "DrcEVSensor",  0, "DrcEVCoverSensor", 0, "BlackSurface");

  gMC->SetSkinSurface("AirMirrorSurface", "DrcMirror", "MirrSurface");

  LOG(info) << "PndDrc: geometry construction finished";
}

// -----   Public method CheckIfSensitive   --------------------------------------
bool PndDrc::CheckIfSensitive(std::string name)
{
  for (size_t i = 0; i < fListOfSensitives.size(); i++) {
    if (name.find(fListOfSensitives[i]) != std::string::npos)
      return true;
  }
  return false;
}

// -----   Private method AddHit   --------------------------------------------
void PndDrc::AddHit(Int_t trackID, Int_t copyNo, Int_t mcpId, TVector3 pos, TVector3 mom, TVector3 momAtEV, Double_t timeAtEV, Double_t time, Double_t length,
                              Int_t pdgCode, Int_t eventID)
{
    fPDPointVector->push_back(PndDrcPDPoint(trackID, copyNo, mcpId, fBarPointVector->size() - 1, pos, mom, momAtEV, timeAtEV, time, length, pdgCode, eventID));
}

void PndDrc::AddEVHit(Int_t trackID, Int_t copyNo, TVector3 pos, TVector3 mom, Double_t time, Double_t length, Int_t pdgCode, Int_t eventID, Double_t timestart,
                                Double_t timestartEV, Double_t VeloPhoton, TVector3 normal)
{
    fEVPointVector->push_back(PndDrcEVPoint(trackID, copyNo, pos, mom, time, length, pdgCode, eventID, timestart, timestartEV, VeloPhoton, normal));
}

void PndDrc::AddBarHit(Int_t trackID, Int_t copyNo, TVector3 pos, TVector3 mom, Double_t time, Double_t length, Int_t pdgCode, Double_t thetaC, Int_t BarId,
                                  Int_t eventID, Double_t mass)
{
    fBarPointVector->push_back(PndDrcBarPoint(trackID, copyNo, pos, mom, time, length, pdgCode, thetaC, BarId, eventID, mass));
}


Bool_t PndDrc::IsSensitive(const std::string& name)
{
    if (name.find("Drc") == std::string::npos)
        return false;
  for (size_t i = 0; i < fListOfSensitives.size(); i++) {
    if (name.find(fListOfSensitives[i]) != std::string::npos)
      return true;
  }
  return false;
}


FairModule* PndDrc::CloneModule() const
{
    return new PndDrc(*this);
}


ClassImp(PndDrc)
