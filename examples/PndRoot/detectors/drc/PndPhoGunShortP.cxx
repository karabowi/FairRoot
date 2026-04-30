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
// -----                   PndPhoGunShortP source file                 -----
// -----               Created 11/09/13  by Harphool Kumawat           -----
// -----                                                               -----
// -----                                                               -----
// -------------------------------------------------------------------------
#include <fstream>
#include <iostream>
#include "stdio.h"

#include "PndGeoDrc.h"
#include "PndPhoGunShortP.h"
#include "FairRootManager.h"
#include "PndMCTrack.h"
#include "TVector3.h"
#include "PndDrcBarPoint.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairBaseParSet.h"
#include "FairGeoVolume.h"
#include "TString.h"
#include "FairGeoTransform.h"
#include "FairGeoVector.h"
#include "FairGeoMedium.h"
#include "FairGeoNode.h"
#include "FairLogger.h"
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
PndPhoGunShortP::PndPhoGunShortP() : FairTask("PndPhoGunShortP")
{
  fGeo = new PndGeoDrc();
  fGeoH = nullptr;
  //  fTableName = "PndPhoGunShortP_table1.par";
  //  fOutputName = "PndPhoGunShortP_output1.root";
}
// -----   Standard constructor with verbosity level  -------------------------------------------

PndPhoGunShortP::PndPhoGunShortP(Int_t verbose) : FairTask("PndPhoGunShortP")
{
  fVerbose = verbose;
  fGeo = new PndGeoDrc();
  fGeoH = nullptr;
  //  fTableName = "PndPhoGunShortP_table1.par";
  //  fOutputName = "PndPhoGunShortP_output1.root";
}
// -----   Destructor   ----------------------------------------------------
PndPhoGunShortP::~PndPhoGunShortP()
{
  if (fGeo)
    delete fGeo;
  if (fGeoH)
    delete fGeoH;
  fHistoList->Delete();
  delete fHistoList;
}

// -----   Initialization   -----------------------------------------------
InitStatus PndPhoGunShortP::Init()
{
  cout << " ---------- INITIALIZATION ------------" << endl;
  nevents = 0;
  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    cout << "-E- PndPhoGunShortP::Init: "
         << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  // Get input array
  fMCArray = (TClonesArray *)ioman->GetObject("MCTrack");
  if (!fMCArray) {
    cout << "-W- PndPhoGunShortP::Init: "
         << "No MCTrack array!" << endl;
    return kERROR;
  }
  // Get Photon point array
  fPDPointArray = (TClonesArray *)ioman->GetObject("DrcPDPoint");
  if (!fPDPointArray) {
    cout << "-W- PndPhoGunShortP::Init: "
         << "No DrcPDPoint array!" << endl;
    return kERROR;
  }
  // Get input array
  fPDHitArray = (TClonesArray *)ioman->GetObject("DrcPDHit");
  if (!fPDHitArray) {
    cout << "-W- PndPhoGunShortP::Init: "
         << "No DrcPDHit array!" << endl;
    return kERROR;
  }
  fBarPointArray = (TClonesArray *)ioman->GetObject("DrcBarPoint");
  if (!fBarPointArray) {
    cout << "-W- PndDrcLogLikeli::Init: "
         << "No DrcBarPoint array!" << endl;
    return kERROR;
  }

  fEVPointArray = (TClonesArray *)ioman->GetObject("DrcEVPoint");
  if (!fEVPointArray) {
    cout << "-W- PndPhoGunShortP::Init: "
         << "No DrcEVPoint array!" << endl;
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

  // Double_t EVlen = gGeoManager->GetVolume("DrcEVSensor")->GetShape()->Dz();
  fEVlen = fGeo->EVlen();

  fEVdrop = fGeo->EVdrop();
  fRBottom = fR - fHThick - fEVdrop + fR * (1. - cos(fDphi / 2. / 180. * fpi)) / cos(fDphi / 2. / 180. * fpi);
  // cout<<"fR = "<<fR<<", fHThick = "<<fHThick<<", fEVdrop = "<<fEVdrop<<", fRBottom = "<<fRBottom<<", dphi = "<<fDphi<<endl;

  Rin1 = fR - fHThick - fEVdrop - fGeo->boxGap() - fGeo->boxThick();
  Rin2 = fR + fHThick + fGeo->EVoffset() + fGeo->boxGap() + fGeo->boxThick();
  //   Rout1     =  fR + fHThick + fGeo->EVoffset()+ fGeo->boxGap() + fGeo->boxThick() + 30.*tan(30.*fpi/180.);
  Rout1 = fR - fHThick - fEVdrop - fGeo->boxGap() - fGeo->boxThick() + 5. * (fGeo->McpActiveArea() + fGeo->McpSize() - fGeo->McpActiveArea() + fGeo->McpGap()) -
          (fGeo->McpSize() - fGeo->McpActiveArea() + fGeo->McpGap());

  Ang_pipe = 180. - 2. * fPipehAngle;

  fFile = TFile::Open(fOutputName, "RECREATE");
  //   fLut = new TClonesArray("PndDrcLutNode");
  fTree = new TTree("dircsim", "Look-up table for DIRC");
  //   fTree->Branch("LUT",&fLut,256000,0);
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

  LOG(info) << " PndPhoGunShortP: Intialization successfull";
  return kSUCCESS;
}

// -----   Execution of Task   ---------------------------------------------
void PndPhoGunShortP::Exec(Option_t *ion)
{
  nevents++;

  fGeoH->SetVerbose(fVerbose);

  fDetectorID = 0;
  if (nevents % 1000 == 0)
    cout << "Event # " << nevents << endl;
  ProcessPhotonHit();
}

//--------------Process Photon Hits----------------------------------------------------
void PndPhoGunShortP::ProcessPhotonHit()
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

    //    Ppt = (PndDrcPDPoint*)fPDPointArray->At(mcPDRef);

    Int_t trID = Ppt->GetTrackID();
    tr = (PndMCTrack *)fMCArray->At(trID);

    EVpt = (PndDrcEVPoint *)fEVPointArray->At(mcPDRef);

    if (trID != EVpt->GetTrackID())
      cout << "different track IDs" << endl;
    EVEntry = fEVPointArray->GetEntriesFast();
    if (trID != EVpt->GetTrackID())
      continue;

    //    if(Ppt->GetBarPointID() < 0) continue;
    PndDrcBarPoint *fBarPoint = (PndDrcBarPoint *)fBarPointArray->At(Ppt->GetBarPointID());
    //    fBarId = fBarPoint->GetDetectorID();
    fBarId = fBarPoint->GetBarId();
    TVector3 PdPoint;
    PdPoint.SetXYZ(pdhit->GetX(), pdhit->GetY(), pdhit->GetZ());
    Double_t PdPhi = InBarCoordSyst(PdPoint);
    //    if(fBarId<=0)cout<<"bar name - "<<fBarId<<endl;
    //    if(fBarId <= 0)continue;
    //    cout<<"boxname - "<<fBarPoint->GetBoxId()<<endl;
    // production point of the photon
    fStartVertex = tr->GetStartVertex();

    // find PHIrot to get to the bar coord. syst:
    fPhiRot = InBarCoordSyst(fStartVertex);
    fZin = ((PndDrcEVPoint *)fEVPointArray->At(0))->GetZ();
    if (TMath::Nint(fabs(PdPhi - fPhiRot) * 180. / fpi / fDphi) > 0)
      continue;

    // check if the last of the EV points is on the PD plane as the reflection was in the grease layer/window/photocathode, then exclude this point
    if (((PndDrcEVPoint *)fEVPointArray->At(EVEntry - 1))->GetZ() < fZin - fEVlen + 0.01) {
      EVEntry = EVEntry - 1;
    }
    // check if there are EVpoints on the PD plane and these points are not the last in the array, then reject the photon
    Bool_t wePho = kFALSE;
    for (Int_t k2 = 1; k2 < fEVPointArray->GetEntriesFast() - 1; k2++) {
      if ((((PndDrcEVPoint *)fEVPointArray->At(k2))->GetZ()) < fZin - fEVlen + 0.01) {
        //	cout<<"this is a weird photon - has EV points at the PD plane!!!"<<endl;
        fNweirdPhotons += 1;
        wePho = kTRUE;
      }
    }
    if (wePho == kTRUE)
      continue;

    if (EVEntry > 5) {
      fNweirdPhotons += 1;
      continue;
    }

    if (EVEntry == 1)
      ReflName = "DD";
    if (EVEntry > 1) {
      ReflName = "";
      for (Int_t k1 = 1; k1 < EVEntry; k1++) {
        Int_t EVS = 0;
        EVt = (PndDrcEVPoint *)fEVPointArray->At(k1);
        fEVSec.SetXYZ(EVt->GetX(), EVt->GetY(), EVt->GetZ());
        fPhiRotEV = InBarCoordSyst(fEVSec);
        fPhiRotEV = fPhiRotEV * 180. / fpi;
        fEVPhi = fEVSec.Phi() * 180. / fpi;
        //	cout<<"EV phi = "<<fEVPhi<<", fPhiRot = "<<fPhiRot*180./fpi<<", fDphi = "<<fDphi<<endl;
        PhiSec = TMath::Nint(fabs(fEVPhi - fPhiRot * 180. / fpi) / fDphi);
        if (fEVPhi < 0.) {
          fEVPhi = 360. + fEVPhi;
          PhiSec = TMath::Nint(fabs(fEVPhi - fPhiRot * 180. / fpi - 360.) / fDphi);
        }
        if (PhiSec > 0)
          cout << "phi sec = " << PhiSec << endl;
        FindReflectionType(EVt->GetX(), EVt->GetY(), EVt->GetZ(), ReflectionType);
        ReflName.Append(ReflectionType);
      }
      if (PhiSec > 0)
        continue;
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
      //      ReflName.Prepend(Form("%d",PhiSec));
    }
    // 24 ambiguities in total:
    if (ReflName == "DD") {
      fNoDD += 1;
      SelectionName = 1;
      ambiguity = 1;
    }
    if (ReflName == "B1") {
      fNoB += 1;
      SelectionName = 1;
      ambiguity = 2;
    }
    if (ReflName == "L1") {
      fNoL += 1;
      SelectionName = 1;
      ambiguity = 3;
    }
    if (ReflName == "R1") {
      fNoR += 1;
      SelectionName = 1;
      ambiguity = 4;
    }
    if (ReflName == "U1") {
      fNoU += 1;
      SelectionName = 1;
      ambiguity = 5;
    }
    if (ReflName == "BU2") {
      fNoBU += 1;
      SelectionName = 1;
      ambiguity = 6;
    }
    if (ReflName == "BL2") {
      fNoBL += 1;
      SelectionName = 1;
      ambiguity = 7;
    }
    if (ReflName == "BR2") {
      fNoBR += 1;
      SelectionName = 1;
      ambiguity = 8;
    }
    if (ReflName == "UL2") {
      fNoUL += 1;
      SelectionName = 1;
      ambiguity = 9;
    }
    if (ReflName == "LU2") {
      fNoLU += 1;
      SelectionName = 1;
      ambiguity = 10;
    }
    if (ReflName == "UR2") {
      fNoUR += 1;
      SelectionName = 1;
      ambiguity = 11;
    }
    if (ReflName == "RU2") {
      fNoRU += 1;
      SelectionName = 1;
      ambiguity = 12;
    }
    if (ReflName == "LR2") {
      fNoLR += 1;
      SelectionName = 1;
      ambiguity = 13;
    }
    if (ReflName == "LB2") {
      fNoLB += 1;
      SelectionName = 1;
      ambiguity = 14;
    }
    if (ReflName == "RB2") {
      fNoRB += 1;
      SelectionName = 1;
      ambiguity = 15;
    }
    if (ReflName == "RL2") {
      fNoRL += 1;
      SelectionName = 1;
      ambiguity = 16;
    }
    if (ReflName == "BLR3") {
      fNoBLR += 1;
      SelectionName = 1;
      ambiguity = 17;
    }
    if (ReflName == "BUR3") {
      fNoBUR += 1;
      SelectionName = 1;
      ambiguity = 18;
    }
    if (ReflName == "BUL3") {
      fNoBUL += 1;
      SelectionName = 1;
      ambiguity = 19;
    }
    if (ReflName == "BLU3") {
      fNoBLU += 1;
      SelectionName = 1;
      ambiguity = 20;
    }
    if (ReflName == "BRL3") {
      fNoBRL += 1;
      SelectionName = 1;
      ambiguity = 21;
    }
    if (ReflName == "BRU3") {
      fNoBRU += 1;
      SelectionName = 1;
      ambiguity = 22;
    }
    if (ReflName == "URL3") {
      fNoURL += 1;
      SelectionName = 1;
      ambiguity = 23;
    }
    if (ReflName == "ULR3") {
      fNoULR += 1;
      SelectionName = 1;
      ambiguity = 24;
    }
    if (ReflName == "RBL3") {
      fNoRBL += 1;
      SelectionName = 1;
      ambiguity = 25;
    }
    //    if(ReflName == "RBU3"){fNoRBU +=1; SelectionName=1;ambiguity =30;}
    if (ReflName == "RLU3") {
      fNoRLU += 1;
      SelectionName = 1;
      ambiguity = 26;
    }
    //    if(ReflName == "RLB3"){fNoRLB +=1; SelectionName=1;ambiguity =32;}
    if (ReflName == "RUL3") {
      fNoRUL += 1;
      SelectionName = 1;
      ambiguity = 27;
    }
    //    if(ReflName == "LRB3"){fNoLRB +=1;  SelectionName=1;ambiguity =34;}
    if (ReflName == "LRU3") {
      fNoLRU += 1;
      SelectionName = 1;
      ambiguity = 28;
    }
    if (ReflName == "LBR3") {
      fNoLBR += 1;
      SelectionName = 1;
      ambiguity = 29;
    }
    if (ReflName == "LBU3") {
      fNoLBU += 1;
      SelectionName = 1;
      ambiguity = 30;
    }
    if (ReflName == "LUR3") {
      fNoLUR += 1;
      SelectionName = 1;
      ambiguity = 31;
    }
    //    if(ReflName == "LUB3"){fNoLUB +=1; SelectionName=1;ambiguity =39;}
    if (ReflName == "URB3") {
      fNoURB += 1;
      SelectionName = 1;
      ambiguity = 32;
    }
    if (ReflName == "ULB3") {
      fNoULB += 1;
      SelectionName = 1;
      ambiguity = 33;
    }
    //    if(ReflName == "UBR3"){fNoUBR +=1;  SelectionName=1;ambiguity =42;}
    //    if(ReflName == "UBL3"){fNoUBL +=1; SelectionName=1;ambiguity =43;}
    if (ReflName == "BULR4") {
      fNoBULR += 1;
      SelectionName = 1;
      ambiguity = 34;
    }
    if (ReflName == "BURL4") {
      fNoBURL += 1;
      SelectionName = 1;
      ambiguity = 35;
    }
    if (ReflName == "BLUR4") {
      fNoBLUR += 1;
      SelectionName = 1;
      ambiguity = 36;
    }
    if (ReflName == "BRUL4") {
      fNoBRUL += 1;
      SelectionName = 1;
      ambiguity = 37;
    }
    if (ReflName == "LBRU4") {
      fNoLBRU += 1;
      SelectionName = 1;
      ambiguity = 38;
    }
    if (ReflName == "LBUR4") {
      fNoLBUR += 1;
      SelectionName = 1;
      ambiguity = 39;
    }
    if (ReflName == "RBUL4") {
      fNoRBUL += 1;
      SelectionName = 1;
      ambiguity = 40;
    }
    if (ReflName == "BLRU4") {
      fNoBLRU += 1;
      SelectionName = 1;
      ambiguity = 41;
    }
    if (ReflName == "BRLU4") {
      fNoBRLU += 1;
      SelectionName = 1;
      ambiguity = 42;
    }
    if (ReflName == "BURBL5") {
      fNoBURBL += 1;
      SelectionName = 1;
      ambiguity = 43;
    }
    if (ReflName == "BRULB5") {
      fNoBRULB += 1;
      SelectionName = 1;
      ambiguity = 44;
    }
    if (ReflName == "BULBR5") {
      fNoBULBR += 1;
      SelectionName = 1;
      ambiguity = 45;
    }
    if (ReflName == "BURLB5") {
      fNoBURLB += 1;
      SelectionName = 1;
      ambiguity = 46;
    }
    if (ReflName == "BLURB5") {
      fNoBLURB += 1;
      SelectionName = 1;
      ambiguity = 47;
    }
    if (ReflName == "BULRB5") {
      fNoBULRB += 1;
      SelectionName = 1;
      ambiguity = 48;
    }
    //    if(ReflName == "BULB4"){fNoBULB +=1; SelectionName=1;ambiguity =46;}
    //    if(ReflName == "BURB4"){fNoBURB +=1; SelectionName=1;ambiguity =47;}
    //    if(ReflName == "UBLR4"){fNoUBLR +=1; SelectionName=1;ambiguity =48;}
    //    if(ReflName == "UBRL4"){fNoUBRL +=1; SelectionName=1;ambiguity =49;}
    //    if(ReflName == "ULBR4"){fNoULBR +=1; SelectionName=1;ambiguity =50;}
    //    if(ReflName == "URBL4"){fNoURBL +=1; SelectionName=1;ambiguity =51;}
    //    if(ReflName == "ULRB4"){fNoULRB +=1; SelectionName=1;ambiguity =52;}
    //    if(ReflName == "URLB4"){fNoURLB +=1; SelectionName=1;ambiguity =53;}
    //    if(ReflName == "RULR4"){fNoRULR +=1; SelectionName=1;ambiguity =54;}
    //    if(ReflName == "UBRL4"){fNoUBRL +=1; SelectionName=1;ambiguity =55;}
    //    if(ReflName == "RULB4"){fNoRULB +=1; SelectionName=1;ambiguity =56;}
    //    if(ReflName == "RBUL4"){fNoRBUL +=1; SelectionName=1;ambiguity =57;}
    //    if(ReflName == "RBLU4"){fNoRBLU +=1; SelectionName=1;ambiguity =58;}
    //    if(ReflName == "LURB4"){fNoLURB +=1; SelectionName=1;ambiguity =61;}
    //    if(ReflName == "LUBR4"){fNoLUBR +=1; SelectionName=1;ambiguity =62;}
    //    if(ReflName == "LRUL4"){fNoLRUL +=1; SelectionName=1;ambiguity =63;}
    //    if(ReflName == "LRLU4"){fNoLRLU +=1; SelectionName=1;ambiguity =64;}
    //    if(ReflName == "LURL4"){fNoLURL +=1; SelectionName=1;ambiguity =65;}
    //    if(ReflName == "RULR4"){fNoRULR +=1; SelectionName=1;ambiguity =66;}
    //    if(ReflName == "RBULR5"){fNoRBULR +=1; SelectionName=1;ambiguity =67;}
    //    if(ReflName == "RBLUR5"){fNoRBLUR +=1; SelectionName=1;ambiguity =68;}
    //    if(ReflName == "RBULB5"){fNoRBULB +=1; SelectionName=1;ambiguity =69;}
    //    if(ReflName == "RBRUL5"){fNoRBRUL +=1; SelectionName=1;ambiguity =70;}
    //    if(ReflName == "BRUBL5"){fNoBRUBL +=1; SelectionName=1;ambiguity =71;}
    //    if(ReflName == "BLRUL5"){fNoBLRUL +=1; SelectionName=1;ambiguity =74;}
    //    if(ReflName == "BLUBR5"){fNoBLUBR +=1; SelectionName=1;ambiguity =75;}
    //    if(ReflName == "LBURL5"){fNoLBURL +=1; SelectionName=1;ambiguity =80;}
    //    if(ReflName == "LBLUR5"){fNoLBLUR +=1; SelectionName=1;ambiguity =81;}
    //    cout<<"reflection type = "<<ReflName<<endl;
    if (SelectionName == 0.) {
      continue;
    }

    fPixIndex = fDigi->GetSensorId();
    //    fPixIndex = pdhit->GetSensorId();
    //    fPixIndex = pdhit->GetDetectorID();
    ftime = Ppt->GetTime();
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
    //    cout<<"pix "<<fPixIndex<<", reflection type = "<<ReflName<<", ambiguity = "<<ambiguity<<" barid "<<fBarId<<endl;
    if (fPixIndex > 30000)
      continue;
    ((PndDrcLutNode *)(fLut[fBarId]->At(fPixIndex)))->AddEntry(fDigi->GetDetectorId(), fPphoB, ambiguity, ftime, pdhit->GetPosition());

    //    ((PndDrcLutNodeH*)(fLut->At(fPixIndex)))->SetBarID(fBarId);
    //    ((PndDrcLutNodeH*)(fLut->At(fPixIndex)))->AddEntry(fPphoB,ambiguity,ftime);
    //    ((PndDrcLutNodeH*)(fLut->At(fPixIndex)))->SetPos(pdhit->GetPosition());

    fNoTotal += 1;

  } // photon hits
}

//----------------------------------------------------------------------------------------------
Double_t PndPhoGunShortP::FindReflectionType(Double_t xev, Double_t yev, Double_t zev, TString ReflType)
{

  //  cout<<"fPhiRotEV = "<<fPhiRotEV<< ", fDphi = "<<fDphi<<", Ang_pipe = "<<Ang_pipe<<endl;

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

  PlanR[0] = (Rin2 / cos((Ang_pipe / 16.) * fpi / 180.)) * cos((fPhiRotEV + fDphi / 2.) * fpi / 180.);
  PlanR[1] = (Rin2 / cos((Ang_pipe / 16.) * fpi / 180.)) * sin((fPhiRotEV + fDphi / 2.) * fpi / 180.);
  PlanR[2] = fZin;
  PlanR[3] = (Rin1 / cos((Ang_pipe / 16.) * fpi / 180.)) * cos((fPhiRotEV + fDphi / 2.) * fpi / 180.);
  PlanR[4] = (Rin1 / cos((Ang_pipe / 16.) * fpi / 180.)) * sin((fPhiRotEV + fDphi / 2.) * fpi / 180.);
  PlanR[5] = fZin - fEVlen;
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

  if (determint1 > -0.1 && determint1 < 0.1)
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
  if (determint2 > -0.1 && determint2 < 0.1)
    ReflectionType = "U";

  fmatrixdata.Reset(0);
  fmatrixdata[0] = xev - PlanB[0];
  fmatrixdata[1] = yev - PlanB[1];
  fmatrixdata[2] = zev - PlanB[2];
  fmatrixdata[3] = PlanU[3] - PlanB[0];
  fmatrixdata[4] = PlanU[4] - PlanB[1];
  fmatrixdata[5] = PlanU[5] - PlanB[2];
  ;
  fmatrixdata[6] = PlanB[3] - PlanB[0];
  fmatrixdata[7] = PlanB[4] - PlanB[1];
  fmatrixdata[8] = PlanB[5] - PlanB[2];
  matrix1.Use(3, 3, fmatrixdata.GetArray());
  determint3 = matrix1.Determinant();

  fmatrixdata.Reset(0);
  fmatrixdata[0] = xev - PlanR[0];
  fmatrixdata[1] = yev - PlanR[1];
  fmatrixdata[2] = zev - PlanR[2];
  fmatrixdata[3] = PlanR[3] - PlanR[0];
  fmatrixdata[4] = PlanR[4] - PlanR[1];
  fmatrixdata[5] = PlanR[5] - PlanR[2];
  fmatrixdata[6] = PlanU[6] - PlanR[0];
  fmatrixdata[7] = PlanU[7] - PlanR[1];
  fmatrixdata[8] = PlanU[8] - PlanR[2];
  matrix1.Use(3, 3, fmatrixdata.GetArray());
  determint4 = matrix1.Determinant();

  if (ReflectionType != "U" && ReflectionType != "B" && determint3 > determint4)
    ReflectionType = "R";
  if (ReflectionType != "U" && ReflectionType != "B" && determint3 < determint4)
    ReflectionType = "L";
  //	cout<<"determinants: "<<determint1<<",  "<<determint2<<endl;
  //	cout<<"determinants: "<<determint3<<",  "<<determint4<<endl;
  //	cout<<"rin2 = "<<Rin1<<", rin2 = "<<Rin2<<", rout = "<<Rout1<<endl;
}

//----------------------------------------------------------------------------------------------
Double_t PndPhoGunShortP::InBarCoordSyst(TVector3 start)
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

  return PhiRot / 180. * fpi;
}
//--------------------------------------------------------------------------------------------
void PndPhoGunShortP::InitLut()

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
void PndPhoGunShortP::Finish()
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
ClassImp(PndPhoGunShortP)
