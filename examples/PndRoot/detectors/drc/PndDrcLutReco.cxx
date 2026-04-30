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

// -----------------------------------------
// PndDrcLutReco.cpp
//
// Created on: 13.07.2013
// Author: R.Dzhygadlo at gsi.de
// -----------------------------------------

#include "PndDrcLutReco.h"

#include "FairRootManager.h"
#include "FairLogger.h"
#include "PndMCTrack.h"
#include "PndDrcPDPoint.h"
#include "PndDrcHit.h"
#include "PndDrcPDHit.h"
#include "PndDrcLutNode.h"

#include "PndGeoHandling.h"
#include "TRandom.h"
#include "TSystem.h"

#include "TCanvas.h"

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndDrcLutReco::PndDrcLutReco() : PndPersistencyTask("PndDrcLutReco")
{
  fInputFile = "luttab.root";
}

// -----   Standard constructors   -----------------------------------------
PndDrcLutReco::PndDrcLutReco(Int_t verbose) : PndPersistencyTask("PndDrcLutReco", verbose)
{
  fVerbose = verbose;
  fInputFile = "luttab.root";
}

PndDrcLutReco::PndDrcLutReco(Int_t verbose, TString infilename) : PndPersistencyTask("PndDrcLutReco", verbose)
{
  fVerbose = verbose;
  fInputFile = infilename;
}

// -----   Destructor   ----------------------------------------------------
PndDrcLutReco::~PndDrcLutReco() {}

// -----   Initialization   ------------------------------------------------
InitStatus PndDrcLutReco::Init()
{
  cout << " ---------- INITIALIZATION ------------" << endl;
  nevents = -1;
  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    cout << "-E- PndDrcLutReco::Init: "
         << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  // Get input array
  fMCArray = (TClonesArray *)ioman->GetObject("MCTrack");
  if (!fMCArray) {
    cout << "-W- PndDrcLutReco::Init: "
         << "No MCTrack array!" << endl;
    return kERROR;
  }

  // Get bar points array
  fBarPointArray = (TClonesArray *)ioman->GetObject("DrcBarPoint");
  if (!fBarPointArray) {
    cout << "-W- PndDrcLutReco::Init: "
         << "No DrcBarPoint array!" << endl;
    return kERROR;
  }

  // Get ev points array
  fEVPointArray = (TClonesArray *)ioman->GetObject("DrcEVPoint");
  if (!fEVPointArray) {
    cout << "-W- PndDrcLutReco::Init: "
         << "No DrcEVPoint array!" << endl;
    return kERROR;
  }

  // Get Photon point array
  fPDPointArray = (TClonesArray *)ioman->GetObject("DrcPDPoint");
  if (!fPDPointArray) {
    cout << "-W- PndDrcLutReco::Init: "
         << "No DrcPDPoint array!" << endl;
    return kERROR;
  }
  // // Get digi array
  // fDigiArray = (TClonesArray*) ioman->GetObject("DrcDigi");
  // if ( ! fDigiArray ) {
  //   cout << "-W- PndDrcLutReco::Init: " << "No DrcDigi array!" << endl;
  //   return kERROR;
  // }
  // Get input array
  fPDHitArray = (TClonesArray *)ioman->GetObject("DrcPDHit");
  if (!fPDHitArray) {
    cout << "-W- PndDrcLutReco::Init: "
         << "No DrcPDHit array!" << endl;
    return kERROR;
  }

  TString name = fInputFile;
  name.Remove(0, name.Last('/') + 1);
  sscanf(name, "lut_e%d_b%d_l%d", &fEvType, &fRadType, &fLensType);

  fFile = new TFile(fInputFile);
  fTree = (TTree *)fFile->Get("dircsim");
  for (Int_t l = 0; l < 5; l++) {
    fLut[l] = new TClonesArray("PndDrcLutNode");
    fTree->SetBranchAddress(Form("LUT%d", l), &fLut[l]);
  }
  fTree->GetEntry(0);

  // Create and register output array
  fDrcTrackInfoArray = new TClonesArray("PndDrcTrackInfo");
  ioman->Register("DrcTrackInfo", "Drc", fDrcTrackInfoArray, GetPersistency());

  fGeo = new PndGeoDrc();
  fBboxNum = fGeo->BBoxNum();
  fPipehAngle = fGeo->PipehAngle();
  Double_t barwidth = fGeo->BarWidth();
  if (fRadType == 3)
    barwidth = 5.41333;
  fBarPhi = 2 * atan(((barwidth + fGeo->barhGap()) / 2.) / fGeo->radius()) * 180 / TMath::Pi();
  fDphi = 2. * (180. - 2 * fPipehAngle) / (Double_t)fGeo->BBoxNum();

  fHist = new TH1F("chrenkov_angle_hist", "chrenkov_angle_hist", 100, 0.4, 0.9);
  // fHist2 = new TH1F("chrenkov_angle_hist2","chrenkov_angle_hist2", 100,0.35,0.85);
  fHist2 = new TH1F("chrenkov_angle_hist2", "", 400, -30, 30);
  fFit = new TF1("fgaus", "[0]*exp(-0.5*((x-[1])/[2])*(x-[1])/[2])", 0.35, 0.85);
  fSpect = new TSpectrum(10);

  LOG(info) << " PndDrcLutReco: Intialization successfull";
  return kSUCCESS;
}

// -----   Execution of Task   ---------------------------------------------
void PndDrcLutReco::Exec(Option_t *)
{
  nevents++;
  fDetectorID = 0;
  if (!fDrcTrackInfoArray)
    Fatal("Exec", "No fDrcTrackInfoArray");
  fDrcTrackInfoArray->Clear();
  Int_t nHits = fPDHitArray->GetEntriesFast();
  if (fVerbose > 1)
    std::cout << "Event # " << nevents << " has " << nHits << " hits." << std::endl;
  else if (fVerbose == 1 && nevents % 100 == 0)
    std::cout << "Event # " << nevents << " has " << nHits << " hits." << std::endl;

  LoopOverMcTracks();
}

void PndDrcLutReco::LoopOverMcTracks()
{
  TVector3 momInBar, posInBar;

  for (Int_t itrack = 0; itrack < fMCArray->GetEntriesFast(); itrack++) {
    fMCTrack = (PndMCTrack *)fMCArray->At(itrack);
    if (fMCTrack->GetMotherID() != -1)
      continue;

    Int_t mcBoxId(-1); // mcBarId,  //[R.K.03/2017] unused variable
    for (int i = 0; i < fBarPointArray->GetEntriesFast(); i++) {
      fBarPoint = (PndDrcBarPoint *)fBarPointArray->At(i);
      if (itrack == fBarPoint->GetTrackID()) {
        mcBoxId = fBarPoint->GetBoxId();
        break;
      }
    }
    if (mcBoxId == -1) {
      std::cout << "mcBoxId  " << mcBoxId << std::endl;
      continue;
    }
    fBarPoint->Momentum(momInBar);
    fBarPoint->Position(posInBar);
    // mcBarId = fBarPoint->GetBarId(); //[R.K.03/2017] unused variable

    PndDrcTrackInfo trackinfo;
    trackinfo.SetMcMomentum(fMCTrack->GetMomentum());
    trackinfo.SetMcPdg(fMCTrack->GetPdgCode());
    trackinfo.SetMcCherenkov(fBarPoint->GetThetaC());
    trackinfo.SetMcMomentumInBar(momInBar);
    trackinfo.SetMcPositionInBar(posInBar);
    trackinfo.SetMcTimeInBar(fBarPoint->GetTime());

    if (fVerbose < 2)
      gROOT->SetBatch(kTRUE);
    DetermineCherenkov(&trackinfo, mcBoxId);
    if (fVerbose < 2)
      gROOT->SetBatch(kFALSE);
    // std::cout<<"dCh = "<< trackinfo.GetMcCherenkov()-trackinfo.GetCherenkov()
    // 	     << "   pdg = "<< trackinfo.GetPdg()<<std::endl;

    new ((*fDrcTrackInfoArray)[fDrcTrackInfoArray->GetEntriesFast()]) PndDrcTrackInfo(trackinfo);
  }
}

void PndDrcLutReco::DetermineCherenkov(PndDrcTrackInfo *trackinfo, Int_t boxId)
{

  TVector3 momInBar = trackinfo->GetMcMomentumInBar();
  TVector3 posInBar = trackinfo->GetMcPositionInBar();
  Double_t barHitTime = trackinfo->GetMcTimeInBar();
  Double_t boxPhi; //, cangle, bartime, lutboxPhi=10.825, window1, window2, angdiv; //[R.K. 01/2017] unused variable?
  Int_t lutboxId(3), barId(-1), evpointcount(0);
  Bool_t reflected;
  Int_t boxId1;

  DetermineBarId(posInBar.Phi(), boxPhi, boxId1, barId);

  if (barId < 0)
    return;
  momInBar.RotateZ(-boxPhi / 180. * TMath::Pi());

  // window1 = (mcboxId)*17-17;
  // window2 = (mcboxId+1)*17+17;

  // Loop over PndDrcPDHits
  for (Int_t ihit = 0; ihit < fPDHitArray->GetEntriesFast(); ihit++) {
    fPDHit = (PndDrcPDHit *)fPDHitArray->At(ihit);
    // Int_t wsensorId = fPDHit->GetSensorId()/100; //[R.K. 01/2017] unused variable?
    //   if(wsensorId < window1 || wsensorId > window2) continue;

    Int_t sensorId = fPDHit->GetSensorId();
    Double_t pdHitTime = fPDHit->GetTime();

    //((FairMultiLinkedData*)fPDHit)->Print();
    Int_t pointID = fPDHit->GetLink(1).GetIndex();
    Int_t eventID = fPDHit->GetLink(1).GetEntry();

    if (eventID != nevents)
      continue;
    if (fPDPointArray->GetEntriesFast() <= pointID) {
      std::cout << " name    " << FairRootManager::Instance()->GetBranchName(fPDHit->GetLink(1).GetType()) << std::endl;
      std::cout << "ERROR 83  " << pointID << " out of  " << fPDPointArray->GetEntriesFast() << "   " << eventID << "/" << nevents << std::endl;
      continue;
    }

    fPDPoint = (PndDrcPDPoint *)fPDPointArray->At(pointID);

    Int_t trackID = fPDPoint->GetTrackID();
    evpointcount = 0;
    for (int i = 0; i < fEVPointArray->GetEntriesFast(); i++) {
      fEVPoint = (PndDrcEVPoint *)fEVPointArray->At(i);
      if (trackID == fEVPoint->GetTrackID())
        evpointcount++;
    }
    if (((PndMCTrack *)fMCArray->At(trackID))->GetMomentum().Z() > 0)
      reflected = kTRUE;
    else
      reflected = kFALSE;

    Int_t recalculatedSensorId = sensorId;
    if (fEvType < 3)
      recalculatedSensorId = (sensorId / 100 - (boxId - lutboxId) * 17) * 100 + sensorId % 100; // for tank EV

    if (sensorId > 27200 || recalculatedSensorId > 27200 || recalculatedSensorId < 0) {
      std::cout << "LUT reco: ignore vertical MCPblock for now.  sensorId =  " << sensorId << " recalculatedSensorId = " << recalculatedSensorId << std::endl;
      continue;
    }

    PndDrcPhotonInfo photoninfo;
    photoninfo.SetMcPrimeMomentumInBar(momInBar);
    photoninfo.SetHitTime(pdHitTime);
    photoninfo.SetReflected(reflected);
    photoninfo.SetEvReflections(evpointcount);
    FillAmbiguities(&photoninfo, barId, recalculatedSensorId, posInBar.Z() + 119, barHitTime);
    trackinfo->AddPhoton(photoninfo);
  }

  Double_t cherenkovreco = FindPeak();
  Int_t pdgreco = FindPdg(momInBar.Mag(), cherenkovreco);

  trackinfo->SetPdg(pdgreco);
  trackinfo->SetCherenkov(cherenkovreco);
}

void PndDrcLutReco::FillAmbiguities(PndDrcPhotonInfo *photoninfo, Int_t barId, Int_t recalculatedSensorId, Double_t lenz, Double_t barHitTime)
{
  TVector3 dird, dir, fnX1 = TVector3(1, 0, 0), fnY1 = TVector3(0, 1, 0), momInBar = photoninfo->GetMcPrimeMomentumInBar();

  Double_t evtime, bartime, luttheta, tangle;
  Double_t reflected = photoninfo->GetReflected();
  Double_t pdHitTime = photoninfo->GetHitTime();
  Double_t criticalAngle = asin(1.00028 / fGeo->nQuartz());
  if (reflected)
    lenz = 2 * 240 - lenz;

  PndDrcLutNode *node = (PndDrcLutNode *)fLut[barId]->At(recalculatedSensorId);
  Int_t size = node->Entries();

  for (int i = 0; i < size; i++) {
    dird = node->GetEntry(i);

    // dird.RotateZ(-lutboxPhi/180.*TMath::Pi());

    evtime = node->GetTime(i);
    for (int u = 0; u < 4; u++) {
      if (u == 0)
        dir = dird;
      if (u == 1)
        dir.SetXYZ(-dird.X(), dird.Y(), dird.Z());
      if (u == 2)
        dir.SetXYZ(dird.X(), -dird.Y(), dird.Z());
      if (u == 3)
        dir.SetXYZ(-dird.X(), -dird.Y(), dird.Z());
      if (reflected)
        dir.SetXYZ(dir.X(), dir.Y(), -dir.Z());

      if (dir.Angle(fnX1) < criticalAngle || dir.Angle(fnY1) < criticalAngle)
        continue;

      luttheta = dir.Theta();
      if (luttheta > TMath::Pi() / 2.)
        luttheta = TMath::Pi() - luttheta;
      bartime = lenz / cos(luttheta) / 19.8;

      if (fabs((bartime + evtime) - (pdHitTime - barHitTime)) > 1.5)
        continue;
      tangle = momInBar.Angle(dir);
      // if(tangle>TMath::Pi()/2.) tangle = TMath::Pi()-tangle;

      PndDrcAmbiguityInfo ambinfo;
      ambinfo.SetBarTime(bartime);
      ambinfo.SetEvTime(evtime);
      ambinfo.SetCherencov(tangle);

      photoninfo->AddAmbiguity(ambinfo);
      if (tangle > 0.4 && tangle < 0.9)
        fHist->Fill(tangle);
    }
  }
}

void PndDrcLutReco::DetermineBarId(Double_t phi, Double_t &boxPhi, Int_t &, Int_t &barId)
{ // boxId //[R.K.03/2017] unused variable(s)
  Double_t startPhi = phi / TMath::Pi() * 180;
  if (startPhi < 0)
    startPhi = 360 + startPhi;
  if (startPhi >= 0 && startPhi < 90)
    boxPhi = TMath::Floor(startPhi / fDphi) * fDphi + fDphi / 2.;
  if (startPhi >= 90 && startPhi < 270)
    boxPhi = 90 + fPipehAngle + TMath::Floor((startPhi - 90 - fPipehAngle) / fDphi) * fDphi + fDphi / 2.;
  if (startPhi >= 270 && startPhi < 360)
    boxPhi = 270 + fPipehAngle + TMath::Floor((startPhi - 270 - fPipehAngle) / fDphi) * fDphi + fDphi / 2.;

  if (fRadType == 5)
    barId = (int)(2.5 + (boxPhi - startPhi) / fBarPhi);
  if (fRadType == 3)
    barId = (int)(1.5 + (boxPhi - startPhi) / fBarPhi);
  if (barId > 4 || barId < 0) {
    std::cout << "Error in PndDrcLutReco:  Bar Id is wrong. barId = " << barId << std::endl;
    barId = -1;
  }
  std::cout << "phi=" << phi << " boxPhi " << boxPhi << " startPhi " << startPhi << "  8235fBarPhi  " << fBarPhi << std::endl;
}

Int_t g_num = 0;
Double_t PndDrcLutReco::FindPeak()
{
  Double_t cherenkovreco = -1;

  if (fHist->GetEntries() > 20) {
    TCanvas *c = new TCanvas("c", "c", 0, 0, 800, 600);
    Int_t nfound = fSpect->Search(fHist, 1, "", 0.6);
    Float_t *xpeaks = (Float_t *)fSpect->GetPositionX();
    if (nfound > 0)
      cherenkovreco = xpeaks[0];
    fFit->SetParameter(1, cherenkovreco); // peak
    fFit->SetParameter(2, 0.01);          // width
    fHist->Fit("fgaus", "Q", "", cherenkovreco - 0.02, cherenkovreco + 0.02);
    cherenkovreco = fFit->GetParameter(1);
    std::cout << "sigma  " << fFit->GetParameter(2) << std::endl;

    if (cherenkovreco < 0 || cherenkovreco > 1)
      cherenkovreco = 0;

    if (fVerbose > 1) {
      fHist->GetXaxis()->SetTitle("#theta_{C}, [rad]");
      fHist->GetYaxis()->SetTitle("Entries, [#]");
      fHist->Draw();
      // fHist2->SetLineColor(4);
      // fHist2->Draw("same");
      c->Modified();
      c->Update();
      c->WaitPrimitive();
      // c->Print(Form("pic/animpid/animpid_%d.png",g_num++));
    }
  }
  fHist2->Reset();
  fHist->Reset();

  return cherenkovreco;
}

Int_t PndDrcLutReco::FindPdg(Double_t mom, Double_t cangle)
{
  Int_t pdg[] = {11, 13, 211, 321, 2212};
  Double_t mass[] = {0.000511, 0.1056584, 0.139570, 0.49368, 0.9382723};
  Double_t tdiff, diff = 100;
  Int_t minid = 0;
  for (Int_t i = 0; i < 5; i++) {
    tdiff = fabs(cangle - acos(sqrt(mom * mom + mass[i] * mass[i]) / mom / 1.46907)); // 1.46907 - fused silica
    if (tdiff < diff) {
      diff = tdiff;
      minid = i;
    }
  }
  return pdg[minid];
}

// -----   Finish Task   ---------------------------------------------------
void PndDrcLutReco::Finish()
{
  for (Int_t l = 0; l < 5; l++) {
    fLut[l]->Clear();
  }
  LOG(info) << " PndDrcLutReco: Finish";
}

ClassImp(PndDrcLutReco)
