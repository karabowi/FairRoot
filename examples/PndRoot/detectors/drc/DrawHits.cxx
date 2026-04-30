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
// -----                   DrawHits source file               -----
// -----               Created 20/10/09  by Diapnwita Dutta            -----
// -----                                                               -----
// -----                                                               -----
// -------------------------------------------------------------------------
#include <fstream>
#include <iostream>
#include "stdio.h"

#include "PndGeoDrc.h"
#include "DrawHits.h"
#include "FairRootManager.h"
#include "PndMCTrack.h"
#include "PndDrcBarPoint.h"
#include "PndDrcPDPoint.h"
#include "PndDrcHit.h"
#include "PndDrcPDHit.h"
#include "TVector3.h"
#include "TPolyLine.h"
#include "TLine.h"
#include "TLegend.h"
#include "TRandom.h"
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
#include "TFormula.h"
#include "TMath.h"
#include "TParticlePDG.h"
#include "TDatabasePDG.h"
#include "TPDGCode.h"
#include "TGeoManager.h"
#include "TCanvas.h"
#include "TFile.h"

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
DrawHits::DrawHits() : FairTask("DrawHits")
{
  fGeo = new PndGeoDrc();
}
// -----   Standard constructor with verbosity level  -------------------------------------------

DrawHits::DrawHits(Int_t verbose) : FairTask("DrawHits")
{
  fVerbose = verbose;
  fGeo = new PndGeoDrc();
}
// -----   Destructor   ----------------------------------------------------
DrawHits::~DrawHits()
{
  if (fGeo)
    delete fGeo;
  fHistoList->Delete();
  delete fHistoList;
}
// -----   Initialization   -----------------------------------------------
InitStatus DrawHits::Init()
{
  cout << " ---------- INITIALIZATION ------------" << endl;
  nevents = 0;
  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    cout << "-E- DrawHits::Init: "
         << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  // Get input array
  fMCArray = (TClonesArray *)ioman->GetObject("MCTrack");
  if (!fMCArray) {
    cout << "-W- DrawHits::Init: "
         << "No MCTrack array!" << endl;
    return kERROR;
  }
  // Get input array
  fBarPointArray = (TClonesArray *)ioman->GetObject("DrcBarPoint");
  if (!fBarPointArray) {
    cout << "-W- DrawHits::Init: "
         << "No DrcBarPoint array!" << endl;
    return kERROR;
  }
  // Get Photon point array
  fPDPointArray = (TClonesArray *)ioman->GetObject("DrcPDPoint");
  if (!fPDPointArray) {
    cout << "-W- DrawHits::Init: "
         << "No DrcPDPoint array!" << endl;
    return kERROR;
  }
  // Get input array
  fHitArray = (TClonesArray *)ioman->GetObject("DrcHit");
  if (!fHitArray) {
    cout << "-W- DrawHits::Init: "
         << "No DrcHit array!" << endl;
    return kERROR;
  }
  // Get input array
  fPDHitArray = (TClonesArray *)ioman->GetObject("DrcPDHit");
  if (!fPDHitArray) {
    cout << "-W- DrawHits::Init: "
         << "No DrcPDHit array!" << endl;
    return kERROR;
  }

  LOG(info) << " DrawHits: Intialization successfull";
  CreateHisto();
  return kSUCCESS;
}
// -----   Execution of Task   ---------------------------------------------
void DrawHits::Exec(Option_t *) // ion //[R.K.03/2017] unused variable(s)
{

  nevents++;
  fDetectorID = 0;

  // fNHits = 0;
  ProcessBarHit();
  ProcessPhotonHit();
  ProcessPhotonMC();
}
//--------------Process Bar Hits----------------------------------------------------
void DrawHits::ProcessBarHit()
{
  PndDrcBarPoint *pt = nullptr;
  PndDrcHit *hit = nullptr;
  PndMCTrack *tr = nullptr;
  Double_t deg = 180.0 / TMath::Pi();

  // Loop over PndDrcHit
  for (Int_t j = 0; j < fHitArray->GetEntriesFast(); j++) {
    // for(Int_t j=0; j<fBarPointArray->GetEntriesFast(); j++) {
    if (fVerbose > 1)
      printf("\n\n=====> Event No. %d\n", nevents);

    hit = (PndDrcHit *)fHitArray->At(j);

    Double_t xH = hit->GetX();
    Double_t yH = hit->GetY();
    Double_t zH = hit->GetZ();

    Int_t mcRef = hit->GetRefIndex();
    pt = (PndDrcBarPoint *)fBarPointArray->At(mcRef);
    //     pt= (PndDrcBarPoint*)fBarPointArray->At(j);
    Int_t chtrID = pt->GetTrackID();
    tr = (PndMCTrack *)fMCArray->At(chtrID);
    Int_t chtrPdg = tr->GetPdgCode();
    Int_t chtrMid = tr->GetMotherID();

    Double_t trPx = pt->GetPx();
    Double_t trPy = pt->GetPy();
    Double_t trPz = pt->GetPz();
    Double_t trP = sqrt(trPx * trPx + trPy * trPy + trPz * trPz);
    Double_t mass = pt->GetMass();
    // Double_t energy = sqrt(trP*trP + mass*mass); //[R.K. 01/2017] unused variable?

    // for my tree phoTree:
    xEnt = pt->GetX();
    yEnt = pt->GetY();
    zEnt = pt->GetZ();

    xbar = xH;
    ybar = yH;
    zbar = zH;

    // phoTree->Fill();
    TVector3 barmom;
    pt->Momentum(barmom);
    Double_t angIn = barmom.Theta();
    Double_t angDeg = angIn * deg;

    fThetaC = gRandom->Gaus(pt->GetThetaC(), 0.008);
    fErrThetaC = 0.; // rad

    if (abs(chtrPdg) == 321)
      fhThetaCMomK->Fill(trP, fThetaC);
    if (abs(chtrPdg) == 211)
      fhThetaCMomP->Fill(trP, fThetaC);
    if (abs(chtrPdg) == 13)
      fhThetaCMomM->Fill(trP, fThetaC);
    if (abs(chtrPdg) == 11)
      fhThetaCMomE->Fill(trP, fThetaC);
    if (chtrMid == -1) {
      fhMomAng->Fill(angDeg, trP);
      cout << "ang = " << angDeg << ", mom = " << trP << endl;

    } // prim particle
    fhThetaC->Fill(fThetaC);
    fhThetaCMass->Fill(fThetaC, mass);
  }
}
//--------------Process Photon MC Points----------------------------------------------------
void DrawHits::ProcessPhotonMC()
{
  // Double_t deg=180.0/TMath::Pi(); //[R.K. 01/2017] unused variable?

  PndMCTrack *ptr = nullptr;
  PndMCTrack *trMr = nullptr;
  for (Int_t l = 0; l < fPDPointArray->GetEntriesFast(); l++) {
    PndDrcPDPoint *Ppt = (PndDrcPDPoint *)fPDPointArray->At(l);
    Int_t trID = Ppt->GetTrackID();
    ptr = (PndMCTrack *)fMCArray->At(trID);

    Int_t trMID = ptr->GetMotherID();
    trMr = (PndMCTrack *)fMCArray->At(trMID);
    Int_t trMpdg = trMr->GetPdgCode();

    if (trMr->GetMotherID() == -1) {

      Double_t xP = Ppt->GetX();
      Double_t yP = Ppt->GetY();
      // Double_t zP= Ppt->GetZ(); //[R.K. 01/2017] unused variable?

      fhXYPDMCPt->Fill(xP, yP);
      if (nevents == 1 && trMpdg == 321)
        fhXYPDMCPtKp->Fill(xP, yP);
      if (nevents == 1 && trMpdg == -321)
        fhXYPDMCPtKn->Fill(xP, yP);
      if (nevents == 1 && trMpdg == 211)
        fhXYPDMCPtpip->Fill(xP, yP);
      if (nevents == 1 && trMpdg == -211)
        fhXYPDMCPtpin->Fill(xP, yP);
      if (nevents == 1 && trMpdg == 13)
        fhXYPDMCPtmp->Fill(xP, yP);
      if (nevents == 1 && trMpdg == -13)
        fhXYPDMCPtmn->Fill(xP, yP);

      // Momentum of photon at PD   point
      Double_t Pfx = Ppt->GetPx();
      Double_t Pfy = Ppt->GetPy();
      Double_t Pfz = Ppt->GetPz();
      // Double_t Pf = sqrt(Pfx*Pfx + Pfy*Pfy +Pfz*Pfz); //[R.K. 01/2017] unused variable?
      Double_t etot = sqrt(Pfx * Pfx + Pfy * Pfy + Pfz * Pfz);
      // Double_t nRefrac=1.467; //[R.K. 01/2017] unused variable?
      // Double_t lambda=197.0*2.0*TMath::Pi()/nRefrac/(etot*1.0E9);//wavelength of photon in nm
      Double_t lambda = 197.0 * 2.0 * TMath::Pi() / (etot * 1.0E9);

      // if(lambda > 350. && lambda < 600.){
      fhLambdaMC->Fill(lambda);
      TVector3 photon;
      TVector3 mother;
      photon.SetXYZ(ptr->GetMomentum().X(), ptr->GetMomentum().Y(), ptr->GetMomentum().Z());
      mother.SetXYZ(trMr->GetMomentum().X(), trMr->GetMomentum().Y(), trMr->GetMomentum().Z());
      fhCHrealMC->Fill(photon.Angle(mother));
      fhCHlamMC->Fill(lambda, mother.Angle(photon));
      //}
    } // Prim particle
  }   // Photon points
}
//--------------Process Photon Hits----------------------------------------------------
void DrawHits::ProcessPhotonHit()
{
  PndDrcPDPoint *Ppt = nullptr;
  PndDrcPDHit *pdhit = nullptr;
  PndMCTrack *tr = nullptr;
  PndMCTrack *trMr;
  if (fVerbose > 0) {
    cout << " Number of Detected Photon Hits : " << fPDHitArray->GetEntries() << endl;
  }

  // Loop over PndDrcPDHits
  for (Int_t k = 0; k < fPDHitArray->GetEntriesFast(); k++) {

    pdhit = (PndDrcPDHit *)fPDHitArray->At(k);

    Int_t mcPDRef = pdhit->GetRefIndex();
    Ppt = (PndDrcPDPoint *)fPDPointArray->At(mcPDRef);

    Int_t trID = Ppt->GetTrackID();
    tr = (PndMCTrack *)fMCArray->At(trID);

    Int_t trMID = tr->GetMotherID();

    trMr = (PndMCTrack *)fMCArray->At(trMID);
    Int_t trMpdg = trMr->GetPdgCode();

    if (trMr->GetMotherID() == -1) {

      Double_t xPHit = pdhit->GetX();
      Double_t yPHit = pdhit->GetY();
      Double_t zPHit = pdhit->GetZ();
      Double_t time = pdhit->GetTime();

      // for my tree:
      xhit = xPHit;
      yhit = yPHit;
      zhit = zPHit;
      thit = time;

      pxMo = trMr->GetMomentum().X();
      pyMo = trMr->GetMomentum().Y();
      pzMo = trMr->GetMomentum().Z();
      fPMo.SetXYZ(pxMo, pyMo, pzMo);

      fhPhoTheta->Fill(fPMo.Theta() / 3.1415 * 180.);

      pxPho = tr->GetMomentum().X(); // initial momentum of a photon
      pyPho = tr->GetMomentum().Y();
      pzPho = tr->GetMomentum().Z();

      fStartVertex = tr->GetStartVertex();

      // momentum of a photon on the PD Plane
      fPx = Ppt->GetPx();
      fPy = Ppt->GetPy();
      fPz = Ppt->GetPz();

      // extrapolation of momentum on the PD Plane to the bar:
      fXcross = xPHit - fPx / fPz * (120. + zPHit);
      fYcross = yPHit - fPy / fPz * (120. + zPHit);

      // phoTree->Fill();
      // ------------

      // Double_t xP= Ppt->GetX(); //[R.K. 01/2017] unused variable?
      // Double_t yP= Ppt->GetY(); //[R.K. 01/2017] unused variable?
      // Double_t zP= Ppt->GetZ(); //[R.K. 01/2017] unused variable?

      Double_t PPx = Ppt->GetPx();
      Double_t PPy = Ppt->GetPy();
      Double_t PPz = Ppt->GetPz();
      Double_t etot = sqrt(PPx * PPx + PPy * PPy + PPz * PPz);
      // Double_t nRefrac=1.467; //[R.K. 01/2017] unused variable?
      // Double_t lambdah=197.0*2.0*TMath::Pi()/nRefrac/(etot*1.0E9);//wavelength of photon in nm
      Double_t lambdah = 197.0 * 2.0 * TMath::Pi() / (etot * 1.0E9);

      fhPDTime->Fill(time);
      // if(lambdah > 350. && lambdah < 600.){
      fhLambda->Fill(lambdah);
      TVector3 photon1;
      TVector3 mother1;
      photon1.SetXYZ(tr->GetMomentum().X(), tr->GetMomentum().Y(), tr->GetMomentum().Z());
      mother1.SetXYZ(trMr->GetMomentum().X(), trMr->GetMomentum().Y(), trMr->GetMomentum().Z());
      fhCHreal->Fill(photon1.Angle(mother1));
      fhCHlam->Fill(lambdah, photon1.Angle(mother1));
      //}

      fhXYPDHit->Fill(xPHit, yPHit);

      if (nevents == 1 && trMpdg == 321)
        fhXYPDHitKp->Fill(xPHit, yPHit);
      if (nevents == 1 && trMpdg == -321)
        fhXYPDHitKn->Fill(xPHit, yPHit);
      if (nevents == 1 && trMpdg == 211)
        fhXYPDHitpip->Fill(xPHit, yPHit);
      if (nevents == 1 && trMpdg == -211)
        fhXYPDHitpin->Fill(xPHit, yPHit);
      if (nevents == 1 && trMpdg == 13)
        fhXYPDHitmp->Fill(xPHit, yPHit);
      if (nevents == 1 && trMpdg == -13)
        fhXYPDHitmn->Fill(xPHit, yPHit);
    } // photon from primary particle
  }   // photon hits
}

//---------------------------------------------------------------
void DrawHits::CreateHisto()
{
  // Creation of the TTree to hold the data for Cherenkov angle
  phoTree = new TTree(fTreeName, "tree to get cherenkov angle");
  phoTree->Branch("HitX", &xhit, "HitX/D");
  phoTree->Branch("HitY", &yhit, "HitY/D");
  phoTree->Branch("HitZ", &zhit, "HitZ/D");
  phoTree->Branch("HitT", &thit, "HitT/D");
  phoTree->Branch("MotherPx", &pxMo, "MotherPx/D");
  phoTree->Branch("MotherPy", &pyMo, "MotherPy/D");
  phoTree->Branch("MotherPz", &pzMo, "MotherPz/D");
  phoTree->Branch("InitPhoPx", &pxPho, "InitPhoPx/D");
  phoTree->Branch("InitPhoPy", &pyPho, "InitPhoPy/D");
  phoTree->Branch("InitPhoPz", &pzPho, "InitPhoPz/D");
  phoTree->Branch("xEntry", &xEnt, "xEntry/D");
  phoTree->Branch("yEntry", &yEnt, "yEntry/D");
  phoTree->Branch("zEntry", &zEnt, "zEntry/D");
  phoTree->Branch("xBar", &xbar, "xBar/D");
  phoTree->Branch("yBar", &ybar, "yBar/D");
  phoTree->Branch("zBar", &zbar, "zBar/D");

  // Histogram list
  fHistoList = new TList();

  // Histogram for Bar Hits

  fhThetaC = new TH1D("fhThetaC", "Cherenkov Angle", 100, 0, 1);
  fhThetaCMass = new TH2D("fhThetaCMass", " ThetaC vs. Mass", 100, 0, 1, 100, 0, 0.6);
  fhThetaCMomK = new TH2D("fhThetaCMomK", " ThetaC vs. Mom", 100, 0, 8.0, 100, 0, 1.);
  fhThetaCMomP = new TH2D("fhThetaCMomP", " ThetaC vs. Mom", 100, 0, 8.0, 100, 0, 1.);
  fhThetaCMomM = new TH2D("fhThetaCMomM", " ThetaC vs. Mom", 100, 0, 8.0, 100, 0, 1.);
  fhThetaCMomE = new TH2D("fhThetaCMomE", " ThetaC vs. Mom", 100, 0, 8.0, 100, 0, 1.);
  fhMomAng = new TH2D("fhMomAng", " Mom vs. Angle", 100, 0, 140.0, 100, 0.0, 8.0);
  fhPhoTheta = new TH1F("fhPhoTheta", "Number of photons as a function of initial track theta", 150, 0., 150.);
  fhPDPlane = new TH2F("fhPDPlane", "pixels hit by photons", 338, -109.85, 109.85, 338, -109.85, 109.85);

  // Histogram for Hits in Photon Detector

  fhLambda = new TH1D("fhLambda", "No of Photons  vs. Lambda", 200, 100, 1000);
  fhPDTime = new TH1D("fhPDTime", "Time in ns", 100, 0, 500);
  fhXYPDHit = new TH2D("fhXYPDHit", "XY distribution of Photon Hits", 1000, -110, 110, 1000, -110, 110);

  fhCHreal = new TH1D("fhCHreal", "Generated Cherenkov angle, hits", 100, 0.78, 0.88);
  fhCHlam = new TH2D("fhCHlam", "Cherenkov angle as a function of lambda", 1000, 0., 1000., 100, 0.7, 0.9);

  // Histogram for MCPoints in Photon Detector
  fhXYPDMCPt = new TH2D("fhXYPDMCpt", "XY distribution of Photon MCPt", 1000, -110, 110, 1000, -110, 110);
  fhLambdaMC = new TH1D("fhLambdaMC", "No of Photons  vs. Lambda", 200, 100, 1000);

  fhCHrealMC = new TH1D("fhCHrealMC", "Generated Cherenkov angle, MC", 100, 0.78, 0.88);
  fhCHlamMC = new TH2D("fhCHlamMC", "Cherenkov angle as a function of lambda", 1000, 0., 1000., 100, 0.7, 0.9);

  fhCHlamE = new TH2D("fhCHlamE", "CHerenkov angle as a function of lambda", 1000, 0., 1000., 100, 0.7, 0.9);

  // Histogram for visualization
  fhXYPDMCPtKp = new TH2D("fhXYPDMCptKp", "XY distribution of Photon MCPt", 1000, -110, 110, 1000, -110, 110);
  fhXYPDMCPtKn = new TH2D("fhXYPDMCptKn", "XY distribution of Photon MCPt", 1000, -110, 110, 1000, -110, 110);
  fhXYPDMCPtpip = new TH2D("fhXYPDMCptpip", "XY distribution of Photon MCPt", 1000, -110, 110, 1000, -110, 110);
  fhXYPDMCPtpin = new TH2D("fhXYPDMCptpin", "XY distribution of Photon MCPt", 1000, -110, 110, 1000, -110, 110);
  fhXYPDMCPtmp = new TH2D("fhXYPDMCptmp", "XY distribution of Photon MCPt", 1000, -110, 110, 1000, -110, 110);
  fhXYPDMCPtmn = new TH2D("fhXYPDMCptmn", "XY distribution of Photon MCPt", 1000, -110, 110, 1000, -110, 110);

  fhXYPDHitKp = new TH2D("fhXYPDHitKp", "XY distribution of Photon Hit", 1000, -110, 110, 1000, -110, 110);
  fhXYPDHitKn = new TH2D("fhXYPDHitKn", "XY distribution of Photon Hit", 1000, -110, 110, 1000, -110, 110);
  fhXYPDHitpip = new TH2D("fhXYPDHitpip", "XY distribution of Photon Hit", 1000, -110, 110, 1000, -110, 110);
  fhXYPDHitpin = new TH2D("fhXYPDHitpin", "XY distribution of Photon Hit", 1000, -110, 110, 1000, -110, 110);
  fhXYPDHitmp = new TH2D("fhXYPDHitmp", "XY distribution of Photon Hit", 1000, -110, 110, 1000, -110, 110);
  fhXYPDHitmn = new TH2D("fhXYPDHitmn", "XY distribution of Photon Hit", 1000, -110, 110, 1000, -110, 110);

  fHistoList->Add(fhThetaC);
  fHistoList->Add(fhThetaCMass);
  fHistoList->Add(fhThetaCMomK);
  fHistoList->Add(fhThetaCMomP);
  fHistoList->Add(fhThetaCMomM);
  fHistoList->Add(fhThetaCMomE);
  fHistoList->Add(fhMomAng);
  fHistoList->Add(fhPhoTheta);
  fHistoList->Add(fhPDPlane);

  fHistoList->Add(fhLambda);
  fHistoList->Add(fhLambdaMC);
  fHistoList->Add(fhXYPDHit);
  fHistoList->Add(fhXYPDMCPt);
  fHistoList->Add(fhPDTime);
  fHistoList->Add(fhCHreal);
  fHistoList->Add(fhCHrealMC);
  fHistoList->Add(fhCHlam);
  fHistoList->Add(fhCHlamMC);
  fHistoList->Add(fhCHlamE);
}
//------------------Write to File----------------------------------------------
void DrawHits::WriteToFile()
{
  phoTree->Write();
  TIter next(fHistoList);
  while (TH1 *histo = ((TH1 *)next()))
    histo->Write();
}
// -----   Finish Task   ---------------------------------------------------
void DrawHits::Finish()
{
  LOG(info) << " DrawHits: Finish";

  WriteToFile();
  DrawHisto();
}
//----------------------------------------------------------------
void DrawHits::DrawHisto()
{
  Double_t deg = 180.0 / TMath::Pi();
  TCanvas *C1 = new TCanvas("C1", "Cherenkov Angle vs. Mass ", 500, 500);
  C1->Divide(1, 1);
  // Drawing Histo
  C1->cd(1);
  fhThetaCMass->SetMarkerColor(kBlue);
  fhThetaCMass->SetMarkerStyle(20);
  fhThetaCMass->SetMarkerSize(0.8);
  fhThetaCMass->GetYaxis()->SetTitle("Mass (GeV/c^{2})");
  fhThetaCMass->GetYaxis()->SetTitleOffset(1.0);
  fhThetaCMass->GetXaxis()->SetTitle("#theta_{C} (rad)");
  fhThetaCMass->SetTitle(0);
  fhThetaCMass->SetStats(0);
  fhThetaCMass->Draw();
  C1->cd();
  TCanvas *C1A = new TCanvas("C1A", " Mom vs. Cherenkov angle", 500, 500);
  C1A->Divide(1, 2);
  C1A->cd(1);
  fhThetaCMomK->SetMarkerColor(kBlue);
  fhThetaCMomP->SetMarkerColor(kMagenta);
  fhThetaCMomM->SetMarkerColor(kGreen);
  fhThetaCMomE->SetMarkerColor(kBlack);
  fhThetaCMomK->SetMarkerStyle(20);
  fhThetaCMomK->SetMarkerSize(0.6);
  fhThetaCMomP->SetMarkerStyle(20);
  fhThetaCMomP->SetMarkerSize(0.6);
  fhThetaCMomM->SetMarkerStyle(20);
  fhThetaCMomM->SetMarkerSize(0.6);
  fhThetaCMomE->SetMarkerStyle(20);
  fhThetaCMomE->SetMarkerSize(0.6);
  fhThetaCMomK->GetYaxis()->SetTitle("#theta_{C} (rad)");
  fhThetaCMomK->GetYaxis()->SetTitleOffset(1.0);
  fhThetaCMomK->GetXaxis()->SetTitle("p (GeV/c)");
  fhThetaCMomK->SetTitle(0);
  fhThetaCMomK->SetStats(0);
  fhThetaCMomK->Draw();
  fhThetaCMomM->Draw("same");
  fhThetaCMomP->Draw("same");
  fhThetaCMomE->Draw("same");
  C1A->cd(2);
  fhPhoTheta->Draw();

  TCanvas *C1B = new TCanvas("C1B", " Momentum vs. track angle", 500, 500);
  C1B->Divide(1, 1);
  C1B->cd();
  fhMomAng->SetMarkerColor(kBlue);
  fhMomAng->SetMarkerStyle(20);
  fhMomAng->SetMarkerSize(0.3);
  fhMomAng->GetXaxis()->SetTitle("#theta (deg)");
  fhMomAng->GetYaxis()->SetTitleOffset(1.0);
  fhMomAng->GetYaxis()->SetTitle("p (GeV/c)");
  fhMomAng->SetTitle(0);
  //  fhMomAng->SetStats(0);
  fhMomAng->Draw();
  C1B->cd();
  TCanvas *C2 = new TCanvas("C2", "Photon Distribution", 700, 700);
  C2->Divide(2, 3);
  C2->cd(1);
  fhXYPDHit->SetMarkerColor(kBlue);
  fhXYPDHit->SetMarkerStyle(20);
  fhXYPDHit->SetMarkerSize(0.6);
  fhXYPDHit->GetXaxis()->SetTitle("x_{Hit} (cm)");
  fhXYPDHit->GetXaxis()->SetTitleOffset(1.0);
  fhXYPDHit->GetYaxis()->SetTitle("y_{Hit} (cm)");
  fhXYPDHit->SetTitle(0);
  fhXYPDHit->Draw();
  C2->cd(2);
  fhXYPDMCPt->SetMarkerColor(kBlue);
  fhXYPDMCPt->SetMarkerStyle(20);
  fhXYPDMCPt->SetMarkerSize(0.6);
  fhXYPDMCPt->GetXaxis()->SetTitle("x_{MC} (cm)");
  fhXYPDMCPt->GetXaxis()->SetTitleOffset(1.0);
  fhXYPDMCPt->GetYaxis()->SetTitle("y_{MC} (cm)");
  fhXYPDMCPt->SetTitle(0);
  fhXYPDMCPt->Draw();
  C2->cd(3);
  fhLambdaMC->SetLineColor(kBlue);
  fhLambdaMC->SetMarkerStyle(20);
  fhLambdaMC->SetMarkerSize(0.01);
  fhLambdaMC->GetXaxis()->SetTitle("#lambda_{MC} (nm)");
  fhLambdaMC->GetXaxis()->SetTitleOffset(1.0);
  fhLambdaMC->GetYaxis()->SetTitle("Counts");
  fhLambdaMC->SetTitle(0);
  //  fhLambdaMC->SetStats(0);
  fhLambdaMC->Draw();
  C2->cd(4);
  fhLambda->SetLineColor(kRed);
  fhLambda->SetMarkerStyle(20);
  fhLambda->SetMarkerSize(0.01);
  fhLambda->GetXaxis()->SetTitle("#lambda_{Hit} (nm)");
  fhLambda->GetXaxis()->SetTitleOffset(1.0);
  fhLambda->GetYaxis()->SetTitle("Counts");
  fhLambda->SetTitle(0);
  //  fhLambda->SetStats(0);
  fhLambda->Draw();
  C2->cd(5);
  fhCHrealMC->Draw();
  fhCHrealMC->Fit("gaus");
  C2->cd(6);
  fhCHreal->Draw();
  fhCHreal->Fit("gaus");
  C2->cd();
  TCanvas *C3 = new TCanvas("C3", "Time ", 900, 500);
  C3->Divide();
  // Drawing Histo
  C3->cd();
  fhPDTime->SetLineColor(kBlack);
  fhPDTime->SetMarkerStyle(20);
  fhPDTime->SetFillColor(2);
  fhPDTime->GetXaxis()->SetTitle("Time (ns)");
  fhPDTime->GetXaxis()->SetTitleOffset(1.0);
  fhPDTime->GetYaxis()->SetTitle("Counts");
  fhPDTime->SetTitle(0);
  fhPDTime->Draw();
  C3->cd();
  TCanvas *C4 = new TCanvas("C4", " ", 500, 500);
  C4->Divide(1, 1);
  C4->cd(1);
  fhXYPDHitKp->SetMarkerColor(kBlue);
  fhXYPDHitKn->SetMarkerColor(kBlack);
  fhXYPDHitmp->SetMarkerColor(kYellow);
  fhXYPDHitmn->SetMarkerColor(kGreen);
  fhXYPDHitpip->SetMarkerColor(kMagenta);
  fhXYPDHitpin->SetMarkerColor(kRed);
  fhXYPDHitKp->SetMarkerStyle(20);
  fhXYPDHitKp->SetMarkerSize(0.6);
  fhXYPDHitKn->SetMarkerStyle(20);
  fhXYPDHitKn->SetMarkerSize(0.6);
  fhXYPDHitpip->SetMarkerStyle(20);
  fhXYPDHitpin->SetMarkerSize(0.6);
  fhXYPDHitmp->SetMarkerStyle(20);
  fhXYPDHitmn->SetMarkerSize(0.6);
  fhXYPDHitKp->GetXaxis()->SetTitle("x_{Hit} (cm)");
  fhXYPDHitKp->GetXaxis()->SetTitleOffset(1.0);
  fhXYPDHitKp->GetYaxis()->SetTitle("y_{Hit} (cm)");
  fhXYPDHitKp->SetTitle(0);
  fhXYPDHitKp->Draw();
  fhXYPDHitKn->Draw("same");
  fhXYPDHitpip->Draw("same");
  fhXYPDHitpip->Draw("same");
  fhXYPDHitmp->Draw("same");
  fhXYPDHitmn->Draw("same");
  Double_t theta = 11.25 / deg;
  Double_t rad_out = 104.0; // PD outer radius
  Double_t rad_in = 50.53;  // PD outer radius
  Double_t rp_out = rad_out / cos(theta);
  Double_t rp_in = rad_in / cos(theta);
  Double_t xout[17], yout[17], xin[17], yin[17];
  for (Int_t i = 0; i < 16; i++) {
    xout[i] = rp_out * cos(2.0 * i * theta);
    yout[i] = rp_out * sin(2.0 * i * theta);
    xin[i] = rp_in * cos(2.0 * i * theta);
    yin[i] = rp_in * sin(2.0 * i * theta);
    TLine *l1 = new TLine(xout[i], yout[i], xin[i], yin[i]);
    l1->Draw("same");
  }
  xout[16] = xout[0];
  yout[16] = yout[0];
  xin[16] = xin[0];
  yin[16] = yin[0];
  TPolyLine *p1 = new TPolyLine(17, xout, yout);
  p1->SetFillColor(kYellow);
  p1->Draw("same");
  TPolyLine *p2 = new TPolyLine(17, xin, yin);
  p2->SetFillColor(0);
  p2->Draw("same");
  C4->cd();
  TCanvas *C5 = new TCanvas("C5", " ", 500, 500);
  C5->Divide(1, 1);
  C5->cd(1);
  fhXYPDMCPtKp->SetMarkerColor(kBlue);
  fhXYPDMCPtKn->SetMarkerColor(kBlack);
  fhXYPDMCPtmp->SetMarkerColor(kYellow);
  fhXYPDMCPtmn->SetMarkerColor(kGreen);
  fhXYPDMCPtpip->SetMarkerColor(kMagenta);
  fhXYPDMCPtpin->SetMarkerColor(kRed);
  fhXYPDMCPtKp->SetMarkerStyle(20);
  fhXYPDMCPtKp->SetMarkerSize(0.6);
  fhXYPDMCPtKn->SetMarkerStyle(20);
  fhXYPDMCPtKn->SetMarkerSize(0.6);
  fhXYPDMCPtpip->SetMarkerStyle(20);
  fhXYPDMCPtpip->SetMarkerSize(0.6);
  fhXYPDMCPtpin->SetMarkerSize(0.6);
  fhXYPDMCPtpin->SetMarkerSize(0.6);
  fhXYPDMCPtmp->SetMarkerStyle(20);
  fhXYPDMCPtmp->SetMarkerSize(0.6);
  fhXYPDMCPtmn->SetMarkerSize(0.6);
  fhXYPDMCPtmn->SetMarkerSize(0.6);
  fhXYPDMCPtKp->GetXaxis()->SetTitle("x_{MC} (cm)");
  fhXYPDMCPtKp->GetXaxis()->SetTitleOffset(1.0);
  fhXYPDMCPtKp->GetYaxis()->SetTitle("y_{MC} (cm)");
  fhXYPDMCPtKp->SetTitle(0);
  fhXYPDMCPtKp->Draw();
  fhXYPDMCPtKn->Draw("same");
  fhXYPDMCPtpip->Draw("same");
  fhXYPDMCPtpip->Draw("same");
  fhXYPDMCPtmp->Draw("same");
  fhXYPDMCPtmn->Draw("same");
  for (Int_t i = 0; i < 16; i++) {
    TLine *l2 = new TLine(xout[i], yout[i], xin[i], yin[i]);
    l2->Draw("same");
  }
  p1->Draw("same");
  p2->Draw("same");

  SetPlotStyle();
  // TCanvas *C7=  //[R.K.03/2017] unused variable
  new TCanvas("C7", "Full sim study", 500, 500);
  fhPDPlane->SetMarkerStyle(20);
  fhPDPlane->SetMarkerSize(0.3);
  fhPDPlane->SetMarkerColor(8);
  fhPDPlane->Draw();
  // fhPDPlane->Draw("COL2Z");
  for (Int_t i = 0; i < 16; i++) {
    TLine *l2 = new TLine(xout[i], yout[i], xin[i], yin[i]);
    l2->Draw("same");
  }
  p1->Draw("same");
  p2->Draw("same");
  TLegend *leg = new TLegend(0.5, 0.5, 0.75, 0.6);
  leg->AddEntry(fhPDPlane, "other photons", "p");
  leg->SetTextSize(0.03);
  leg->Draw("same");
}
// -------------------------------------------------------------------------
ClassImp(DrawHits)
