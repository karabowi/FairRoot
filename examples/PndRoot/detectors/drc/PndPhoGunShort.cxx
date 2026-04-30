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
// -----                   PndPhoGunShort source file                       -----
// -----               Created 12/10/10  by Maria Patsyuk              -----
// -----                                                               -----
// -----                                                               -----
// -------------------------------------------------------------------------
#include <fstream>
#include <iostream>
#include "stdio.h"

#include "PndGeoDrc.h"
#include "PndPhoGunShort.h"
#include "FairRootManager.h"
#include "PndMCTrack.h"
#include "TVector3.h"

#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairBaseParSet.h"
#include "FairGeoVolume.h"
#include "FairLogger.h"
#include "TString.h"
#include "FairGeoTransform.h"
#include "FairGeoVector.h"
#include "FairGeoMedium.h"
#include "FairGeoNode.h"
#include "PndGeoDrcPar.h"
#include "TMath.h"
#include "TParticlePDG.h"
#include "TDatabasePDG.h"
#include "TPDGCode.h"
#include "TGeoManager.h"
#include "TFile.h"
#include "TExec.h"

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndPhoGunShort::PndPhoGunShort() : FairTask("PndPhoGunShort")
{
  fGeo = new PndGeoDrc();
  fGeoH = nullptr;
}
// -----   Standard constructor with verbosity level  -------------------------------------------

PndPhoGunShort::PndPhoGunShort(Int_t verbose) : FairTask("PndPhoGunShort")
{
  fVerbose = verbose;
  fGeo = new PndGeoDrc();
  fGeoH = nullptr;
}
// -----   Destructor   ----------------------------------------------------
PndPhoGunShort::~PndPhoGunShort()
{
  if (fGeo)
    delete fGeo;
  if (fGeoH)
    delete fGeoH;
  fHistoList->Delete();
  delete fHistoList;
}

// -----   Initialization   -----------------------------------------------
InitStatus PndPhoGunShort::Init()
{
  cout << " ---------- INITIALIZATION ------------" << endl;
  nevents = 0;
  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    cout << "-E- PndPhoGunShort::Init: "
         << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  // Get input array
  fMCArray = (TClonesArray *)ioman->GetObject("MCTrack");
  if (!fMCArray) {
    cout << "-W- PndPhoGunShort::Init: "
         << "No MCTrack array!" << endl;
    return kERROR;
  }
  // Get Photon point array
  fPDPointArray = (TClonesArray *)ioman->GetObject("DrcPDPoint");
  if (!fPDPointArray) {
    cout << "-W- PndPhoGunShort::Init: "
         << "No DrcPDPoint array!" << endl;
    return kERROR;
  }
  // Get input array
  fPDHitArray = (TClonesArray *)ioman->GetObject("DrcPDHit");
  if (!fPDHitArray) {
    cout << "-W- PndPhoGunShort::Init: "
         << "No DrcPDHit array!" << endl;
    return kERROR;
  }

  fEVPointArray = (TClonesArray *)ioman->GetObject("DrcEVPoint");
  if (!fEVPointArray) {
    cout << "-W- PndPhoGunShort::Init: "
         << "No DrcEVPoint array!" << endl;
    return kERROR;
  }
  fBarPointArray = (TClonesArray *)ioman->GetObject("DrcBarPoint");
  if (!fBarPointArray) {
    cout << "-W- PndDrcLogLikeli::Init: "
         << "No DrcBarPoint array!" << endl;
    return kERROR;
  }
  fDigiArray = (TClonesArray *)ioman->GetObject("DrcDigi");
  if (!fDigiArray) {
    cout << "-W- PndPhoGunShortP::Init: "
         << "No DrcDigi array!" << endl;
    return kERROR;
  }

  // Get parameters:
  fpi = TMath::Pi();
  fR = fGeo->radius();
  fHThick = fGeo->barHalfThick();
  fBboxNum = fGeo->BBoxNum();
  fPipehAngle = fGeo->PipehAngle();
  fBarBoxGap = fGeo->BBoxGap();
  fLength = (180. - 2. * fPipehAngle - fBarBoxGap / fR * (fBboxNum / 2. - 1.) / fpi * 180.) / (fBboxNum / 2.) * fR / 180. * fpi;
  fDphi = fGeo->BBoxAngle(); // 21.65 degrees = 2.*(180. - 2*fPipehAngle)/ fBboxNum; //[degrees]
  fPixelSize = fGeo->PixelSize();

  // Double_t EVlen = gGeoManager->GetVolume("DrcEVSensor")->GetShape()->Dz();
  // fEVlen = fGeo->EVlen();

  fEVdrop = fGeo->EVdrop();
  fEVlen = fEVdz;
  fRBottom = fR - fHThick - fEVdrop + fR * (1. - cos(fDphi / 2. / 180. * fpi)) / cos(fDphi / 2. / 180. * fpi);
  // cout<<"fR = "<<fR<<", fHThick = "<<fHThick<<", fEVdrop = "<<fEVdrop<<", fRBottom = "<<fRBottom<<", dphi = "<<fDphi<<endl;
  //   cout<<"EV depth = "<<fEVlen<<", n mcp = "<<fNmcp<<endl;

  Rin1 = fR - fHThick - fEVdrop - fGeo->boxGap() - fGeo->boxThick();
  Rin2 = fR + fHThick + fGeo->EVoffset() + fGeo->boxGap() + fGeo->boxThick();
  Rout1 = fR - fHThick - fEVdrop - fGeo->boxGap() - fGeo->boxThick() + fNmcp * (fGeo->McpActiveArea() + fGeo->McpSize() - fGeo->McpActiveArea() + fGeo->McpGap()) -
          (fGeo->McpSize() - fGeo->McpActiveArea() + fGeo->McpGap());

  if (fShiftPix) {
    Rin1 = fR - fHThick - fEVdrop - fGeo->boxGap() - fGeo->boxThick() - fPixelSize / 2.;
    Rout1 = fR - fHThick - fEVdrop - fGeo->boxGap() - fGeo->boxThick() + fNmcp * (fGeo->McpActiveArea() + fGeo->McpSize() - fGeo->McpActiveArea() + fGeo->McpGap()) -
            (fGeo->McpSize() - fGeo->McpActiveArea() + fGeo->McpGap()) + fPixelSize / 2.;
    cout << "shifted pixels" << endl;
  }

  Ang_pipe = 180. - 2. * fPipehAngle;
  fNoDD = 0;
  fNoU0 = 0;
  fNoU1 = 0;
  fNoU2 = 0;
  fNoU3 = 0;
  fNoB = 0;
  fNoBU0 = 0;
  fNoBU1 = 0;
  fNoBU2 = 0;
  fNoUB = 0;
  fNoUU0 = 0;
  fNoUU1 = 0;
  fNoUU2 = 0;
  fNoUU3 = 0;
  fNoUUU0 = 0;
  fNoUUU1 = 0;
  fNoUUU2 = 0;
  fNoUUU3 = 0;
  fNoUUU4 = 0;
  fNoBUU0 = 0;
  fNoBUU1 = 0;
  fNoBUU2 = 0;
  fNoUBU = 0;
  fNoBUB = 0;
  fNoTotal = 0;
  fNweirdPhotons = 0;

  fFile = TFile::Open(fOutputName, "RECREATE");
  fTree = new TTree("dircsim", "Look-up table for DIRC");
  for (Int_t iLut = 0; iLut < 5; iLut++) {
    fLut[iLut] = new TClonesArray("PndDrcLutNode");
    fTree->Branch(Form("LUT%d", iLut), &fLut[iLut], 256000, 0);
  }

  InitLut();

  if (fGeoH == nullptr)
    fGeoH = PndGeoHandling::Instance();
  fGeoH->SetParContainers();

  // fGeoH->GetGeoManager();
  // const Double_t *truuu = gGeoManager->GetVolume("DrcEVSensor")->GetShape()->GetTransform()->GetTranslation();
  // fLowZ = truuu[2]-fGeo->EVlen();
  // cout<<"posZ = "<<fLowZ<<endl;

  LOG(info) << " PndPhoGunShort: Intialization successfull";
  return kSUCCESS;
}

// -----   Execution of Task   ---------------------------------------------
void PndPhoGunShort::Exec(Option_t *ion)
{
  nevents++;

  fGeoH->SetVerbose(fVerbose);

  fDetectorID = 0;
  /*if(nevents%1000==0)*/ cout << "Event # " << nevents << endl;
  ProcessPhotonHit();
}

//--------------Process Photon Hits----------------------------------------------------
void PndPhoGunShort::ProcessPhotonHit()
{
  Int_t EVEntry;
  Int_t SelectionName = 0;
  Int_t PhiSec = 0;
  fmatrixdata.Set(9);
  fmatrixdata.Reset(0);

  // Loop over PndDrcPDHits
  for (Int_t k = 0; k < fPDHitArray->GetEntriesFast(); k++) {

    pdhit = (PndDrcPDHit *)fPDHitArray->At(k);

    Int_t mcPDRef = pdhit->GetRefIndex();
    fDigi = (PndDrcDigi *)fDigiArray->At(mcPDRef);
    Int_t pointID = fDigi->GetIndex(0);
    Ppt = (PndDrcPDPoint *)fPDPointArray->At(pointID);

    Int_t trID = Ppt->GetTrackID();
    tr = (PndMCTrack *)fMCArray->At(trID);

    EVpt = (PndDrcEVPoint *)fEVPointArray->At(mcPDRef);

    if (trID != EVpt->GetTrackID())
      cout << "different track IDs" << endl;
    EVEntry = fEVPointArray->GetEntriesFast();
    if (trID != EVpt->GetTrackID())
      continue;
    PndDrcBarPoint *fBarPoint = (PndDrcBarPoint *)fBarPointArray->At(Ppt->GetBarPointID());
    //    fBarId = fBarPoint->GetDetectorID();
    fBarId = fBarPoint->GetBarId();

    // production point of the photon
    fStartVertex = tr->GetStartVertex();

    // find PHIrot to get to the bar coord. syst:
    fPhiRot = InBarCoordSyst(fStartVertex, &fBBver1, &fBBver2, &fBBver3, &fBBver4);

    // check if the last of the EV points is on the PD plane as the reflection was in the grease layer/window/photocathode, then exclude this point
    if (((PndDrcEVPoint *)fEVPointArray->At(EVEntry - 1))->GetZ() < ((PndDrcEVPoint *)fEVPointArray->At(0))->GetZ() - fEVlen + 0.01) {
      EVEntry = EVEntry - 1;
    }
    // check if there are EVpoints on the PD plane and these points are not the last in the array, then reject the photon
    Bool_t wePho = kFALSE;
    for (Int_t k2 = 1; k2 < fEVPointArray->GetEntriesFast() - 1; k2++) {
      if ((((PndDrcEVPoint *)fEVPointArray->At(k2))->GetZ()) < ((PndDrcEVPoint *)fEVPointArray->At(0))->GetZ() - fEVlen + 0.01) {
        // cout<<"this is a weird photon - has EV points at the PD plane!!!"<<endl;
        fNweirdPhotons += 1;
        wePho = kTRUE;
      }
    }
    if (wePho == kTRUE)
      continue;

    if (EVEntry > 4) {
      fNweirdPhotons += 1;
      continue;
    }
    ReflName = "";
    if (EVEntry == 1)
      ReflName = "DD";
    fZin = ((PndDrcEVPoint *)fEVPointArray->At(0))->GetZ();
    if (EVEntry > 1) {
      for (Int_t k1 = 1; k1 < EVEntry; k1++) {
        Int_t EVS = 0;
        EVt = (PndDrcEVPoint *)fEVPointArray->At(k1);
        fEVSec.SetXYZ(EVt->GetX(), EVt->GetY(), EVt->GetZ());
        fPhiRotEV = InBarCoordSyst(fEVSec, &fBBver1, &fBBver2, &fBBver3, &fBBver4);
        fPhiRotEV = fPhiRotEV * 180. / fpi;
        fEVPhi = fEVSec.Phi() * 180. / fpi;
        // cout<<"EV phi = "<<fEVPhi<<", fPhiRot = "<<fPhiRot<<", fDphi = "<<fDphi<<endl;
        PhiSec = TMath::Nint(fabs(fEVPhi - fPhiRot * 180. / fpi) / fDphi);
        if (fEVPhi < 0.) {
          fEVPhi = 360. + fEVPhi;
          PhiSec = TMath::Nint(fabs(fEVPhi - fPhiRot * 180. / fpi - 360.) / fDphi);
        }
        // cout<<"phi sec = "<<PhiSec<<endl;
        FindReflectionType(EVt->GetX(), EVt->GetY(), EVt->GetZ(), ReflectionType);
        ReflName.Append(ReflectionType);
      }
      if (EVEntry == 2)
        ReflName.Append("1");
      if (EVEntry == 3)
        ReflName.Append("2");
      if (EVEntry == 4)
        ReflName.Append("3");
      if (EVEntry == 5)
        ReflName.Append("4");
      if (EVEntry >= 6)
        ReflName.Append("XX");
      ReflName.Prepend(Form("%d", PhiSec));
    }
    // 24 ambiguities in total:
    if (ReflName == "DD") {
      fNoDD += 1;
      SelectionName = 1;
      ambiguity = 1;
    }
    if (ReflName == "0B1") {
      fNoB += 1;
      SelectionName = 1;
      ambiguity = 2;
    }
    if (ReflName == "0U1") {
      fNoU0 += 1;
      SelectionName = 1;
      ambiguity = 3;
    }
    if (ReflName == "1U1") {
      fNoU1 += 1;
      SelectionName = 1;
      ambiguity = 4;
    }
    if (ReflName == "2U1") {
      fNoU2 += 1;
      SelectionName = 1;
      ambiguity = 5;
    }
    if (ReflName == "3U1") {
      fNoU3 += 1;
      SelectionName = 1;
      ambiguity = 6;
    }
    if (ReflName == "0BU2") {
      fNoBU0 += 1;
      SelectionName = 1;
      ambiguity = 7;
    }
    if (ReflName == "1BU2") {
      fNoBU1 += 1;
      SelectionName = 1;
      ambiguity = 8;
    }
    if (ReflName == "2BU2") {
      fNoBU2 += 1;
      SelectionName = 1;
      ambiguity = 9;
    }
    if (ReflName == "0UB2") {
      fNoUB += 1;
      SelectionName = 1;
      ambiguity = 10;
    }
    if (ReflName == "0UU2") {
      fNoUU0 += 1;
      SelectionName = 1;
      ambiguity = 11;
    }
    if (ReflName == "1UU2") {
      fNoUU1 += 1;
      SelectionName = 1;
      ambiguity = 12;
    }
    if (ReflName == "2UU2") {
      fNoUU2 += 1;
      SelectionName = 1;
      ambiguity = 13;
    }
    if (ReflName == "3UU2") {
      fNoUU3 += 1;
      SelectionName = 1;
      ambiguity = 14;
    }
    if (ReflName == "0UUU3") {
      fNoUUU0 += 1;
      SelectionName = 1;
      ambiguity = 15;
    }
    if (ReflName == "1UUU3") {
      fNoUUU1 += 1;
      SelectionName = 1;
      ambiguity = 16;
    }
    if (ReflName == "2UUU3") {
      fNoUUU2 += 1;
      SelectionName = 1;
      ambiguity = 17;
    }
    if (ReflName == "3UUU3") {
      fNoUUU3 += 1;
      SelectionName = 1;
      ambiguity = 18;
    }
    if (ReflName == "4UUU3") {
      fNoUUU4 += 1;
      SelectionName = 1;
      ambiguity = 19;
    }
    if (ReflName == "0BUU3") {
      fNoBUU0 += 1;
      SelectionName = 1;
      ambiguity = 20;
    }
    if (ReflName == "1BUU3") {
      fNoBUU1 += 1;
      SelectionName = 1;
      ambiguity = 21;
    }
    if (ReflName == "2BUU3") {
      fNoBUU2 += 1;
      SelectionName = 1;
      ambiguity = 22;
    }
    if (ReflName == "BUB3") {
      fNoBUB += 1;
      SelectionName = 1;
      ambiguity = 23;
    }
    if (ReflName == "UBU3") {
      fNoUBU += 1;
      SelectionName = 1;
      ambiguity = 24;
    }
    if (ReflName == "") {
      ambiguity = -99;
    }

    if (SelectionName == 0.) {
      continue;
    }

    fPixIndex = fDigi->GetSensorId();
    //    fPixIndex = pdhit->GetDetectorID();
    ftime = Ppt->GetTime(); // pdhit->GetTime();

    // Photon initial momentum in the bar coord. syst.
    fPphoInit = tr->GetMomentum();
    // fPphoB = (fGeoH->MasterToLocalShortId(fPphoInit, fBarId) - fGeoH->MasterToLocalShortId((0.,0.,0.),fBarId)).Unit();
    // fPphoB.Print();
    // Photon initial momentum in the bar coord. syst.
    fPphoB = (tr->GetMomentum()).Unit();
    fPphoB.RotateZ(-fPhiRot);
    //    fkxBar = -fPphoB.Y();
    //    fkyBar =  fPphoB.X();
    //    fkzBar =  fPphoB.Z();
    fkxBar = fPphoB.X();
    fkyBar = fPphoB.Y();
    fkzBar = fPphoB.Z();
    fPphoB.SetXYZ(fkxBar, fkyBar, fkzBar);

    ((PndDrcLutNode *)(fLut[fBarId]->At(fPixIndex)))->AddEntry(fDigi->GetDetectorId(), fPphoB, ambiguity, ftime, pdhit->GetPosition());
    //    ((PndDrcLutNodeH*)(fLut->At(fPixIndex)))->AddEntry(fPphoB,ambiguity,ftime);
    //    ((PndDrcLutNodeH*)(fLut->At(fPixIndex)))->SetPos(pdhit->GetPosition());

    //    Double_t rrr = sqrt(pow(pdhit->GetX(),2) + pow(pdhit->GetY(),2));

    // if(ambiguity>2){
    //      cout<<"pix "<<fPixIndex<<", reflection type = "<<ReflName<<", ambiguity = "<<ambiguity<<", kx = "<<fkxBar<<", R = "<<rrr<<endl;
    //}

    fNoTotal += 1;

  } // photon hits
}

//----------------------------------------------------------------------------------------------
Double_t PndPhoGunShort::FindReflectionType(Double_t xev, Double_t yev, Double_t zev, TString ReflType)
{

  // cout<<"fPhiRotEV = "<<fPhiRotEV<< ", fDphi = "<<fDphi<<", Ang_pipe = "<<Ang_pipe<<endl;

  PlanB[0] = (Rin1 / cos((Ang_pipe / 16.) * fpi / 180.)) * cos((fPhiRotEV - fDphi / 2.) * fpi / 180.);
  PlanB[1] = (Rin1 / cos((Ang_pipe / 16.) * fpi / 180.)) * sin((fPhiRotEV - fDphi / 2.) * fpi / 180.);
  PlanB[2] = fZin; //-119.6015;//fGeo->barBoxZUp()-0.30075;
  PlanB[3] = (Rin1 / cos((Ang_pipe / 16.) * fpi / 180.)) * cos((fPhiRotEV - fDphi / 2.) * fpi / 180.);
  PlanB[4] = (Rin1 / cos((Ang_pipe / 16.) * fpi / 180.)) * sin((fPhiRotEV - fDphi / 2.) * fpi / 180.);
  PlanB[5] = fZin - fEVlen; //-149.6015;//fGeo->barBoxZUp()-fGeo->EVlen()-0.30075;
  PlanB[6] = (Rin1 / cos((Ang_pipe / 16.) * fpi / 180.)) * cos((fPhiRotEV + fDphi / 2.) * fpi / 180.);
  PlanB[7] = (Rin1 / cos((Ang_pipe / 16.) * fpi / 180.)) * sin((fPhiRotEV + fDphi / 2.) * fpi / 180.);
  PlanB[8] = fZin - fEVlen; //-149.6015;//fGeo->barBoxZUp()-fGeo->EVlen()-0.30075;

  PlanU[0] = (Rin2 / cos((Ang_pipe / 16.) * fpi / 180.)) * cos((fPhiRotEV - fDphi / 2.) * fpi / 180.);
  PlanU[1] = (Rin2 / cos((Ang_pipe / 16.) * fpi / 180.)) * sin((fPhiRotEV - fDphi / 2.) * fpi / 180.);
  PlanU[2] = fZin; //-119.6015;//fGeo->barBoxZUp()-0.30075;
  PlanU[3] = (Rout1 / cos((Ang_pipe / 16.) * fpi / 180.)) * cos((fPhiRotEV - fDphi / 2.) * fpi / 180.);
  PlanU[4] = (Rout1 / cos((Ang_pipe / 16.) * fpi / 180.)) * sin((fPhiRotEV - fDphi / 2.) * fpi / 180.);
  PlanU[5] = fZin - fEVlen; //-149.6015;//fGeo->barBoxZUp()-fGeo->EVlen()-0.30075;
  PlanU[6] = (Rout1 / cos((Ang_pipe / 16.) * fpi / 180.)) * cos((fPhiRotEV + fDphi / 2.) * fpi / 180.);
  PlanU[7] = (Rout1 / cos((Ang_pipe / 16.) * fpi / 180.)) * sin((fPhiRotEV + fDphi / 2.) * fpi / 180.);
  PlanU[8] = fZin - fEVlen; //-149.6015;//fGeo->barBoxZUp()-fGeo->EVlen()-0.30075;

  // cout<<Rin1<<" "<<Rin2<<" "<<Rout1<<" "<<Rout1/cos((Ang_pipe/16.)*fpi/180.)<<endl;
  ReflectionType = "";
  TMatrixD matrix1;
  fmatrixdata[0] = xev - PlanB[0];
  fmatrixdata[1] = yev - PlanB[1];
  fmatrixdata[2] = zev - PlanB[2];
  fmatrixdata[3] = PlanB[3] - PlanB[0];
  fmatrixdata[4] = PlanB[4] - PlanB[1];
  fmatrixdata[5] = PlanB[5] - PlanB[2];
  fmatrixdata[6] = PlanB[6] - PlanB[0];
  fmatrixdata[7] = PlanB[7] - PlanB[1];
  fmatrixdata[8] = PlanB[8] - PlanB[2];
  matrix1.Use(3, 3, fmatrixdata.GetArray());
  determint1 = matrix1.Determinant();

  if (determint1 > -0.02 && determint1 < 0.02)
    ReflectionType = "B";
  fmatrixdata.Reset(0);
  fmatrixdata[0] = xev - PlanU[0];
  fmatrixdata[1] = yev - PlanU[1];
  fmatrixdata[2] = zev - PlanU[2];
  fmatrixdata[3] = PlanU[3] - PlanU[0];
  fmatrixdata[4] = PlanU[4] - PlanU[1];
  fmatrixdata[5] = PlanU[5] - PlanU[2];
  fmatrixdata[6] = PlanU[6] - PlanU[0];
  fmatrixdata[7] = PlanU[7] - PlanU[1];
  fmatrixdata[8] = PlanU[8] - PlanU[2];
  matrix1.Use(3, 3, fmatrixdata.GetArray());
  determint2 = matrix1.Determinant();
  if (determint2 > -0.02 && determint2 < 0.02)
    ReflectionType = "U";

  // cout<<"determinants: "<<determint1<<",  "<<determint2<<endl;
  // cout<<"rin2 = "<<Rin1<<", rin2 = "<<Rin2<<", rout = "<<Rout1<<endl;
}

//----------------------------------------------------------------------------------------------
Double_t PndPhoGunShort::InBarCoordSyst(TVector3 start, TVector3 *v1, TVector3 *v2, TVector3 *v3, TVector3 *v4)
{

  // this function is used with fStartVertex to find the bar from which the photon originated

  Double_t startPhi = start.Phi() / fpi * 180.; // [degrees]
  // cout<<"-I- InBarCoordinateSystem: start phi = "<<startPhi<<endl;
  // cout<<"-I- InBarCoordinateSystem: dphi = "<<Dphi<<endl;
  Double_t PhiRot = 0.; //[degrees]
  if (startPhi < 0.) {
    startPhi = 360. + startPhi;
  }
  if (startPhi > 0. && startPhi < 90.) {
    PhiRot = TMath::Floor(startPhi / fDphi) * fDphi + fDphi / 2.;
  }
  if (startPhi > 90. && startPhi < 270.) {
    PhiRot = 90. + fPipehAngle + TMath::Floor((startPhi - 90. - fPipehAngle) / fDphi) * fDphi + fDphi / 2.;
  }
  if (startPhi > 270. && startPhi < 360.) {
    PhiRot = 270. + fPipehAngle + TMath::Floor((startPhi - 270. - fPipehAngle) / fDphi) * fDphi + fDphi / 2.;
  }
  // cout<<"-I- InBarCoordinateSystem: PhiRot = "<<PhiRot<<endl;

  // create initial barbox:

  TVector3 ver1, ver2, ver3, ver4;
  ver1.SetXYZ(fR - fHThick, fLength / 2., 0.);
  ver2.SetXYZ(fR + fHThick, fLength / 2., 0.);
  ver3.SetXYZ(fR + fHThick, -fLength / 2., 0.);
  ver4.SetXYZ(fR - fHThick, -fLength / 2., 0.);

  ver1.RotateZ(PhiRot / 180. * fpi);
  ver2.RotateZ(PhiRot / 180. * fpi);
  ver3.RotateZ(PhiRot / 180. * fpi);
  ver4.RotateZ(PhiRot / 180. * fpi);

  *v1 = ver1;
  *v2 = ver2;
  *v3 = ver3;
  *v4 = ver4;

  return PhiRot / 180. * fpi;
}
//--------------------------------------------------------------------------------------------
void PndPhoGunShort::InitLut()
{
  Int_t Nnodes = 30000;
  for (Int_t iLut = 0; iLut < 5; iLut++) {
    TClonesArray &fLuta = *fLut[iLut];
    for (Long64_t n = 0; n < Nnodes; n++) {
      new ((fLuta)[n]) PndDrcLutNode(-1);
    }
  }
}

// -----   Finish Task   ---------------------------------------------------
void PndPhoGunShort::Finish()
{
  fTree->Fill();
  fTree->Write();
  fFile->Write();

  for (Int_t iLut = 0; iLut < 5; iLut++) {
    fLut[iLut]->Clear();
  }
  LOG(info) << " PndDrcLutFill: Finish";
}

// -------------------------------------------------------------------------
ClassImp(PndPhoGunShort)
