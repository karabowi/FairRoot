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
// pnddrcreco.cxx
//
// Created on: 04.03.2016
// Author: R.Dzhygadlo at gsi.de
// -----------------------------------------

#include "PndDrcReco.h"

#include "FairRootManager.h"
#include "FairLogger.h"
#include "PndMCTrack.h"
#include "PndDrcPDPoint.h"
#include "PndDrcHit.h"
#include "PndDrcPDHit.h"
#include "PndDrcLutNode.h"

#include "PndGeoHandling.h"
#include "TSystem.h"

#include <TLine.h>

#include "TStyle.h"
#include "TCanvas.h"

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndDrcReco::PndDrcReco() : FairTask("PndDrcReco") {}

PndDrcReco::PndDrcReco(TString outFile, TString lutFile, TString pdfFile, int verbose, double r1, double r2)
  : FairTask("PndDrcReco", verbose), fVerbose(verbose), fOutFile(outFile), fLutFile(lutFile), fPdfFile(pdfFile), fMethod(0)
{
  fR1 = r1;
  fR2 = r2;
}

// -----   Initialization   ------------------------------------------------
InitStatus PndDrcReco::Init()
{
  LOG(info) << "PndDrcReco: initialization started";
  nevents = -1;
  fHist_flag = true;
  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    LOG(error) << "PndDrcReco: RootManager not instantiated";
    return kFATAL;
  }

  // Get input array
  fMCArray = (TClonesArray *)ioman->GetObject("MCTrack");
  if (!fMCArray) {
    LOG(error) << "PndDrcReco: no MCTrack array";
    return kERROR;
  }

  // Get bar points array
  fBarPointArray = (TClonesArray *)ioman->GetObject("DrcBarPoint");
  if (!fBarPointArray) {
    LOG(error) << "PndDrcReco: no DrcBarPoint array";
    return kERROR;
  }

  // Get ev points array
  fEVPointArray = (TClonesArray *)ioman->GetObject("DrcEVPoint");
  if (!fEVPointArray) {
    LOG(error) << "PndDrcReco: no DrcEVPoint array";
    return kERROR;
  }

  // Get Photon point array
  fPDPointArray = (TClonesArray *)ioman->GetObject("DrcPDPoint");
  if (!fPDPointArray) {
    LOG(error) << "PndDrcReco: no DrcPDPoint array";
    return kERROR;
  }

  // Get hits array
  fPDHitArray = (TClonesArray *)ioman->GetObject("DrcPDHit");
  if (!fPDHitArray) {
    LOG(error) << "PndDrcReco: no DrcPDHit array";
    return kERROR;
  }

  TString name = fLutFile;
  name.Remove(0, name.Last('/') + 1);
  sscanf(name, "lut_e%d_b%d_l%d", &fEvType, &fRadType, &fLensType);
  fRadType = 3;

  fFile = new TFile(fLutFile);
  fTree = (TTree *)fFile->Get("dircsim");
  for (int l = 0; l < 3; l++) {
    fLut[l] = new TClonesArray("PndDrcLutNode");
    fTree->SetBranchAddress(Form("LUT%d", l), &fLut[l]);
  }
  fTree->GetEntry(0);

  for (int m = 0; m < 40; m++) {
    for (int t = 0; t < 120; t++) {
      for (int h = 0; h < 5; h++) {
        for (int i = 0; i < 1100; i++) {
          fhPdf[h][m][t][i] = nullptr;
        }
      }
      for (int p = 0; p < 9; p++) {
        fCorr_mean[p][m][t] = 0;
        fCorr_spr[p][m][t] = 0;
      }
    }
  }

  // read corrections
  fCorrFile = fOutFile + "corrlut.root";
  if (!gSystem->AccessPathName(fCorrFile)) {
    LOG(info) << "PndDrcReco: reading corrections from " << fCorrFile;
    int pmt, mom, theta;
    double mean, spr;
    TChain ch("corr");
    ch.Add(fCorrFile);
    ch.SetBranchAddress("pmt", &pmt);
    ch.SetBranchAddress("mom", &mom);
    ch.SetBranchAddress("theta", &theta);
    ch.SetBranchAddress("mean", &mean);
    ch.SetBranchAddress("spr", &spr);
    for (int i = 0; i < ch.GetEntries(); i++) {
      ch.GetEvent(i);
      fCorr_mean[pmt][mom][theta] = (fabs(mean) < 0.011) ? mean : 0.00001;
      fCorr_spr[pmt][mom][theta] = (fabs(spr) < 0.015) ? spr + 0.001 : 0.010;
      LOG(info) << "pmt " << pmt << " mom " << mom << " theta " << theta << "  " << mean << " " << spr;
    }
    fStoreCorr = false;
  } else {
    fStoreCorr = true;
    LOG(warning) << "PndDrcReco: corr file not found " << fCorrFile;
  }

  if (!gSystem->AccessPathName(fPdfFile)) {
    LOG(info) << "PndDrcReco: reading PDFs from " << fPdfFile;
    TFile fpdf(fPdfFile);
    TIter nextkey(fpdf.GetListOfKeys());
    TKey *key;
    while ((key = (TKey *)nextkey())) {
      TH1F *fun = (TH1F *)key->ReadObj();
      int p, m, t, i;
      sscanf(fun->GetName(), "pdf_%d_%d_%d_%d", &p, &m, &t, &i);
      fhPdf[p][m][t][i] = fun;
      fhPdf[p][m][t][i]->SetDirectory(0);
      if (p == 2)
        fhPdf[p][m][t][i]->SetLineColor(kBlue);
      if (p == 3)
        fhPdf[p][m][t][i]->SetLineColor(kRed);
    }
    fpdf.Close();
  } else {
    LOG(warning) << "PndDrcReco: PDF's file not found " << fPdfFile;
  }

  // //nph lut
  // name = fLutFile;
  // name.Remove(name.Last('/')+1);
  // TFile fn_r(name+"nph.root");
  // TIter nextkey(fn_r.GetListOfKeys());
  // TKey *key;

  // while ((key = (TKey*)nextkey())) {
  //   TF1 *fun = (TF1*)key->ReadObj();
  //   int i,m,t;
  //   sscanf(fun->GetName(), "%d_%d_%d", &i,&m,&t);
  //   fhNphArr[i][m][t]=fun;
  // }
  // fn_r.Close();

  fFileOut = new TFile(fOutFile, "recreate");
  fTreeOut = new TTree("barreldirc", "SPR");

  fTreeOut->Branch("fMom", &fMom, "fMom/D");
  fTreeOut->Branch("fTheta", &fTheta, "fTheta/D");
  fTreeOut->Branch("fPhi", &fPhi, "fPhi/D");
  fTreeOut->Branch("fPidTrue", &fPidTrue, "fPidTrue/I");
  fTreeOut->Branch("fPidDist", &fPidDist, "fPidDist/I");

  fTreeOut->Branch("fMissId", &fMissId, "fMissId[5]/D");
  fTreeOut->Branch("fEfficiency", &fEfficiency, "fEfficiency[5]/D");

  fTreeOut->Branch("fPidLike", &fPidLike, "fPidLike[2]/I");
  fTreeOut->Branch("fLikelihood", &fLikelihood, "fLikelihood[3]/D");
  fTreeOut->Branch("fSeparation", &fSeparation, "fSeparation[3]/D");
  fTreeOut->Branch("fSpr", &fSpr, "fSpr[5]/D");
  fTreeOut->Branch("fNph0", &fNph[0], "fNph0[5]/D");
  fTreeOut->Branch("fNph1", &fNph[1], "fNph1[5]/D");
  fTreeOut->Branch("fNph2", &fNph[2], "fNph2[5]/D");
  fTreeOut->Branch("fCangle", &fCangle, "fCangle[5]/D");
  fTreeOut->Branch("fR1", &fR1, "fR1/D");
  fTreeOut->Branch("fR2", &fR2, "fR2/D");

  fFile->cd();

  fGeo = new PndGeoDrc();
  fPipehAngle = fGeo->PipehAngle();
  double barwidth = fGeo->BarWidth();
  if (fRadType == 3)
    barwidth = 5.41333;
  fBarPhi = 2 * atan(((barwidth + fGeo->barhGap()) / 2.) / fGeo->radius()) * 180 / TMath::Pi();
  fDphi = 2. * (180. - 2 * fPipehAngle) / (double)fGeo->BBoxNum();

  fHist = new TH1F("cherenkov_angle_hist", ";#theta_{c} [rad];entries [#]", 100, 0.6, 0.9);
  fFit = new TF1("fgaus", "[0]*exp(-0.5*((x-[1])/[2])*(x-[1])/[2]) + x*[3]+ [4]", 0.35, 0.85);
  fSpect = new TSpectrum(10);

  fNx = TVector3(1, 0, 0);
  fNy = TVector3(0, 1, 0);
  fCriticalAngle = asin(1.00028 / fGeo->nQuartz());

  int pdg[] = {11, 13, 211, 321, 2212};
  double mass[] = {0.000511, 0.1056584, 0.139570, 0.49368, 0.9382723};

  gStyle->SetOptStat(1);
  gStyle->SetOptTitle(1);

  for (int i = 0; i < 9; i++) {
    TString nid = Form("corr_%d", i);
    fhCorr[i] = new TH1F(nid, nid + ";#theta_{C reco} - #theta_{C theor} [rad];entries [#]", 50, -0.05, 0.05);
  }

  for (int i = 0; i < 5; i++) {
    fParticleArray[pdg[i]] = i;
    fPdg[i] = pdg[i];
    fMass[i] = mass[i];
    fHits[i] = 0;
    fHits1[i] = 0;
    fEvents[i] = 0;
    fEventsEff[i] = 0;
    fEventsMis[i] = 0;
    fMissId[i] = 0;
    fEfficiency[i] = 0;
    fAngle[i] = 0;
    fFunc[i] = new TF1(Form("f_%d", i), "gaus(0)", 0.4, 0.9);
    fFunc[i]->SetParameter(0, 1);
    fFunc[i]->SetParameter(2, 0.0085);

    for (int m = 0; m < 3; m++) {
      fhLk[m][i] = new TH1F(Form("fhLk_%d_%d", m, i), ";ln L(K) - ln L(#pi);entries [#]", 200, -400, 400);
      fhNph[m][i] = new TH1F(Form("fhNph_%d_%d", m, i), ";detected photons [#];entries [#]", 140, 0, 140);
    }
    fhTang[i] = new TH1F(Form("cherenkov_angle_hist_%d", i), ";#theta_{c} [rad];entries [#]", 200, 0.4, 0.9);
    fhTang[i]->SetMinimum(0);
    fhTang[i]->SetStats(0);
    fhDiff[i] = new TH1F(Form("fhDiff_%d", i), ";t_{calc} - t_{measured} [ns];entries [#]", 200, -10, 10);
    fhTime[i] = new TH1F(Form("fhTime_%d", i), ";time [ns];entries [#]", 1000, 0, 100);

    for (int c = 0; c < 800; c++) {
      fhTimeA[i][c] = new TH1F(Form("fhTimeA_%d_%d", i, c), ";time [ns];entries [#]", 250, 10, 35);
    }
  }

  fCanvasList = new TList();
  gg_c = new TCanvas("gg_c", "gg_c", 0, 0, 800, 600);
  gg_c->Divide(2, 1);

  LOG(info) << "PndDrcReco: intialization successfull";
  return kSUCCESS;
}

TH1F *hEnergy = new TH1F("hEnergy", ";p [eV];entries [#]", 200, 0, 10);
TH1F *hPathAll = new TH1F("hPathAll", ";pathid [#];entries [#]", 30, 0, 30);
TH1F *hPath = new TH1F("hPath", ";pathid [#];entries [#]", 30, 0, 30);
TH2F *hSD = new TH2F("hSD", ";#theta_{c} [rad];t_{calc} - t_{measured} [ns]", 200, 0.8, 0.85, 200, -2, 2);
TH2F *h2Time = new TH2F("h2Time", ";propagation time [ns];t_{calc} - t_{measured} [ns]", 5000, 0, 50, 200, -5, 5);
TH1F *hAngAna = new TH1F("hAngAna", ";#theta_{c} [rad];entries [#]", 500, 0.6, 0.9);

// -----   Execution of Task   ---------------------------------------------
void PndDrcReco::Exec(Option_t *)
{
  nevents++;
  int nHits = fPDHitArray->GetEntriesFast();
  if (fVerbose > 1)
    LOG(info) << "Event # " << nevents << " has " << nHits << " hits";
  else if (fVerbose == 1 && nevents % 100 == 0)
    LOG(info) << "Event # " << nevents << " has " << nHits << " hits";

  for (int itrack = 0; itrack < fMCArray->GetEntriesFast(); itrack++) {

    fMCTrack = (PndMCTrack *)fMCArray->At(itrack);
    if (fMCTrack->GetMotherID() != -1)
      continue;
    fMcTrackId = itrack;

    int mcBoxId(-1), barId; // mcBarId,  //[R.K.03/2017] unused variable
    for (int i = 0; i < fBarPointArray->GetEntriesFast(); i++) {
      fBarPoint = (PndDrcBarPoint *)fBarPointArray->At(i);
      if (itrack == fBarPoint->GetTrackID()) {
        mcBoxId = fBarPoint->GetBoxId();
        break;
      }
    }
    if (mcBoxId == -1)
      continue;

    fBarPoint->Momentum(fMomInBar);
    fBarPoint->Position(fPosInBar);
    fTimeInBar = fBarPoint->GetTime();
    fPidTrue = fBarPoint->GetPdgCode();
    fBarPoint->GetBarId(); // mcBarId =  //[R.K.03/2017] unused variable

    // tracking smearing
    TVector3 zz = fMomInBar;
    fMomInBar.SetTheta(gRandom->Gaus(fMomInBar.Theta(), 0.002));
    fMomInBar.Rotate(gRandom->Uniform(2 * TMath::Pi()), zz);

    fMom = fMomInBar.Mag();
    fTheta = fMCTrack->GetMomentum().Theta() * 180 / TMath::Pi();
    // fPhi=fMomInBar.Phi()*180/TMath::Pi();
    fPhi = fMCTrack->GetMomentum().Phi() * 180 / TMath::Pi();

    fBin_mom = fMom * 10 + 0.5;
    fBin_theta = fTheta + 0.5;

    if (fHist_flag && itrack == 0) {
      for (int m = 0; m < 3; m++) {
        int range = 400;
        if (fMom > 2)
          range = 200;
        if (fMom > 2.5)
          range = 100;
        if (fMom > 3.0)
          range = 50;
        if (m == 1)
          range += 50;

        fhLk[m][2] = new TH1F(Form("fhLkr_%d_2", m), ";ln L(K) - ln L(#pi);entries [#]", 250, -range, range);
        fhLk[m][3] = new TH1F(Form("fhLkr_%d_3", m), ";ln L(K) - ln L(#pi);entries [#]", 250, -range, range);
      }
      fHist_flag = false;
    }

    double boxPhi;
    DetermineBarId(boxPhi, barId);
    fMomInBar.RotateZ(-boxPhi / 180. * TMath::Pi());

    DetermineCherenkov(mcBoxId, barId);
  }
}

int gg_pathid = 0;
void PndDrcReco::DetermineCherenkov(int, int barId)
{

  for (int i = 0; i < 5; i++) {
    for (int m = 0; m < 3; m++) {
      fLk[m][i] = 0;
      fHitsE[m][i] = 0;
    }
    fAngle[i] = acos(sqrt(fMom * fMom + fMass[i] * fMass[i]) / fMom / 1.473) + 0.00; // 1.4738 = 370 = 3.35
    fFunc[i]->SetParameter(1, fAngle[i]);
    if (fCorr_spr[8][fBin_mom][fBin_theta] > 0.001)
      fFunc[i]->SetParameter(2, fCorr_spr[8][fBin_mom][fBin_theta]);
  }

  // if(fPDHitArray->GetEntriesFast()<80) return;
  for (int h = 0; h < fPDHitArray->GetEntriesFast(); h++) {
    fPDHit = static_cast<PndDrcPDHit *>(fPDHitArray->At(h));

    int sensorId = fPDHit->GetSensorId();
    fTimeHit = fPDHit->GetTime() - fTimeInBar;
    // fTimeHit+=gRandom->Gaus(0,0.1); // smear time if not smeared during digi

    int pointID = fPDHit->GetLink(1).GetIndex();
    // int eventID = fPDHit->GetLink(1).GetEntry();

    fPDPoint = static_cast<PndDrcPDPoint *>(fPDPointArray->At(pointID));
    fBarPoint = static_cast<PndDrcBarPoint *>(fBarPointArray->At(fPDPoint->GetBarPointID()));

    if (fPDPoint->GetTrackID() < 1)
      continue;
    fMCTrack = static_cast<PndMCTrack *>(fMCArray->At(fPDPoint->GetTrackID()));
    if (fMcTrackId != fMCTrack->GetMotherID())
      continue;

    // double en = 1.2398/(fMCTrack->GetMomentum().Mag()*1E6);
    hEnergy->Fill(fMCTrack->GetMomentum().Mag() * 1E9);

    // if(fBarPoint->GetBoxId() != boxId || fBarPoint->GetBarId() != barId) continue;

    int nev = 0;
    TVector3 vec;
    gg_pathid = 0;

    for (int i = 0; i < fEVPointArray->GetEntriesFast(); i++) {
      fEVPoint = static_cast<PndDrcEVPoint *>(fEVPointArray->At(i));
      if (fPDPoint->GetTrackID() == fEVPoint->GetTrackID()) {
        vec = fEVPoint->GetNormal();
        if (vec.X() > 0.99)
          gg_pathid = gg_pathid * 10 + 1; // right
        if (vec.X() < -0.99)
          gg_pathid = gg_pathid * 10 + 2; // left
        if (vec.Y() < -0.99)
          gg_pathid = gg_pathid * 10 + 3; // bottom
        if (fabs(vec.Y() - 0.844437) < 0.1)
          gg_pathid = gg_pathid * 10 + 4; // top
        if (vec.Y() > 0.99)
          gg_pathid = gg_pathid * 10 + 5; // top of the lens
        if (vec.Z() < -0.99)
          gg_pathid = gg_pathid * 10 + 9; // face
        if (++nev > 10)
          LOG(warning) << "PndDrcReco: too many reflections in Prism";
      }
    }

    if (fMethod == 0 || fMethod == 5)
      LookUpTable(barId, sensorId);
    if (fMethod == 1 || fMethod == 5)
      TimeImaging(sensorId);
    if (fMethod == 2 || fMethod == 5)
      TimeImagingWithAnalyticalPdf(barId, sensorId);
  }

  int pid = fParticleArray[fPidTrue];
  for (int m = 0; m < 3; m++) {
    fLikelihood[m] = fLk[m][3] - fLk[m][2];
    if (fabs(fLikelihood[m]) < 0.01)
      continue;
    // if(fHitsE[m][pid]<50) continue;
    if (fMethod == m || fMethod == 5) {
      fhLk[m][pid]->Fill(fLikelihood[m]);
      fHits[pid] += fHitsE[m][pid];
      if (pid == 3)
        fHitsE[m][pid] += 2;
      fhNph[m][pid]->Fill(fHitsE[m][pid]);
      if (fVerbose > 1)
        LOG(info) << "LK (method " << m << ") = " << fLikelihood[m];
    }
  }

  if (fLikelihood[0] > 0) {
    if (fPidTrue == 321)
      fEventsEff[3]++;
    else
      fEventsMis[3]++;
  } else {
    if (fPidTrue == 211)
      fEventsEff[2]++;
    else
      fEventsMis[2]++;
  }
  fEvents[pid]++;

  if (fVerbose > 2) {
    TCanvas *c = new TCanvas("c", "c", 0, 0, 800, 600);
    // fHist->Scale(1/fHist->GetMaximum());
    fHist->SetTitle(Form("%d", fPidTrue));
    fHist->Draw();

    fFunc[2]->SetLineColor(kBlue);
    fFunc[2]->Draw("same");
    fFunc[3]->SetLineColor(kRed);
    fFunc[3]->Draw("same");

    TLine *line = new TLine(0, 0, 0, 1000);
    line->SetX1(fAngle[2]);
    line->SetX2(fAngle[2]);
    line->SetY1(gPad->GetUymin());
    line->SetY2(fHist->GetMaximum() * 1.05);
    line->SetLineColor(kBlue);
    line->Draw();

    TLine *line1 = new TLine(0, 0, 0, 1000);
    line1->SetX1(fAngle[3]);
    line1->SetX2(fAngle[3]);
    line1->SetY1(gPad->GetUymin());
    line1->SetY2(fHist->GetMaximum() * 1.05);
    line1->SetLineColor(kRed);
    line1->Draw();

    c->Modified();
    c->Update();
    c->WaitPrimitive();
  }

  fHist->Reset();
}

void PndDrcReco::LookUpTable(int barId, int sensorId)
{
  TVector3 dird, dir;
  double evtime, luttime, luttheta, tangle, noise(3);
  int pid = fParticleArray[fPidTrue];
  int pmt = sensorId / 100;

  if (fMCTrack->GetMomentum().Z() > 0)
    fReflected = kTRUE;
  else
    fReflected = kFALSE;

  fLenz = fPosInBar.Z() + 119;
  if (fReflected)
    fLenz = 2 * 240 - fLenz;

  PndDrcLutNode *node = static_cast<PndDrcLutNode *>(fLut[barId]->At(sensorId));
  int size = node->Entries();

  bool isGood(false);
  for (int i = 0; i < size; i++) {
    dird = node->GetEntry(i);
    evtime = node->GetTime(i);
    hPathAll->Fill(node->GetPathId(i));

    if ((int)gg_pathid != (int)node->GetPath(i)) {
      // continue;
    } else {
      hPath->Fill(node->GetPathId(i));
    }

    for (int u = 0; u < 4; u++) {
      if (u == 0)
        dir = dird;
      if (u == 1)
        dir.SetXYZ(-dird.X(), dird.Y(), dird.Z());
      if (u == 2)
        dir.SetXYZ(dird.X(), -dird.Y(), dird.Z());
      if (u == 3)
        dir.SetXYZ(-dird.X(), -dird.Y(), dird.Z());
      if (fReflected)
        dir.SetXYZ(dir.X(), dir.Y(), -dir.Z());

      if (dir.Angle(fNx) < fCriticalAngle || dir.Angle(fNy) < fCriticalAngle)
        continue;

      luttheta = dir.Theta();
      if (luttheta > TMath::PiOver2())
        luttheta = TMath::Pi() - luttheta;
      luttime = fLenz / cos(luttheta) / 19.8 + evtime; // 19.8

      double tdiff = luttime - fTimeHit;
      tangle = fMomInBar.Angle(dir);
      tangle += fCorr_mean[pmt][fBin_mom][fBin_theta];
      if (tangle < 0.4 || tangle > 0.9)
        continue;

      fhTime[pid]->Fill(fTimeHit);
      fhDiff[pid]->Fill(tdiff);

      if (fabs(tdiff) > 0.8 + luttime * 0.03)
        continue;

      double cdiff = tangle - fAngle[pid];
      if (fabs(cdiff) < 0.05) {
        fhCorr[pmt]->Fill(cdiff);
        fhCorr[8]->Fill(cdiff);
      }

      if (fabs(tangle - fAngle[2]) < 0.05 || fabs(tangle - fAngle[3]) < 0.05)
        isGood = true;

      hSD->Fill(tangle, tdiff);
      h2Time->Fill(luttime, tdiff);

      fLk[0][2] += TMath::Log((fFunc[2]->Eval(tangle) + noise)); // 211
      fLk[0][3] += TMath::Log((fFunc[3]->Eval(tangle) + noise)); // 321

      fhTang[pid]->Fill(tangle);
      fHist->Fill(tangle);
    }
  }
  if (isGood)
    fHitsE[0][pid]++;
}

double gg_fpdf(double *x, double *par)
{
  double f = 0;
  int n = int(par[1]);
  for (int i = 0; i < n; i++) {
    double a = (x[0] - par[3 * i + 3]) / par[3 * i + 4];
    f += par[3 * i + 2] * exp(-0.5 * a * a);
  }
  return par[0] * f;
}

void PndDrcReco::BuildPdfs(TF1 &pdfus, TF1 &pdfuf, int ch, int barId, TVector3 mom, double posz, bool reflected)
{

  double mrange = 50;
  std::vector<double> wwf, mmf, ssf, wws, mms, sss;
  TVector3 dir, dird;
  double tangle, luttheta, len, lenz, evtime, tottime, bartime, radiatorL = 240;
  double si, n, ww, dabs, dphi, bcut = -0.024, scut = 0.01, wcut = 0.009;
  double wtime = 1.0; // 0.9
  double mcut = 0.002;
  double lrad = 1.7 / sin(mom.Theta());
  bool add;
  double canglek = fAngle[3] - 0.00;  //-3
  double canglepi = fAngle[2] + 0.00; // 5

  lenz = posz + 119;
  int umin = 0, umax = 4;
  if (reflected) {
    lenz = 2 * radiatorL - lenz;
    umin = 4;
    umax = 8;
  }

  PndDrcLutNode *node = static_cast<PndDrcLutNode *>(fLut[barId]->At(ch));
  int lutsize = node->Entries();
  int pmt = ch / 100;

  for (int i = 0; i < lutsize; i++) {
    // if((int)gg_pathid != (int)node->GetPath(i)) continue;
    dird = node->GetEntry(i);
    evtime = node->GetTime(i);

    for (int u = umin; u < umax; u++) {
      if (u == 0)
        dir = dird;
      if (u == 1)
        dir.SetXYZ(-dird.X(), dird.Y(), dird.Z());
      if (u == 2)
        dir.SetXYZ(dird.X(), -dird.Y(), dird.Z());
      if (u == 3)
        dir.SetXYZ(-dird.X(), -dird.Y(), dird.Z());

      if (u == 4)
        dir.SetXYZ(dird.X(), dird.Y(), -dird.Z());
      if (u == 5)
        dir.SetXYZ(-dird.X(), dird.Y(), -dird.Z());
      if (u == 6)
        dir.SetXYZ(dird.X(), -dird.Y(), -dird.Z());
      if (u == 7)
        dir.SetXYZ(-dird.X(), -dird.Y(), -dird.Z());

      luttheta = dir.Theta();
      if (luttheta > TMath::PiOver2())
        luttheta = TMath::Pi() - luttheta;
      len = fabs(lenz / cos(luttheta));

      dphi = 500 * 0.64 / (len * len);
      bartime = len / 19.8;
      tottime = evtime + bartime;
      tangle = mom.Angle(dir);
      tangle += fCorr_mean[pmt][fBin_mom][fBin_theta];

      hAngAna->Fill(tangle);

      dabs = tangle - canglek;
      if (fabs(dabs) < mcut)
        dabs = mcut;
      if (dabs > bcut && dabs < scut) {

        ww = 0.05 * fabs(wcut / dabs);
        si = TMath::Sin(canglek);
        n = ww * lrad * si * si * dphi / TMath::TwoPi();
        add = true;

        // for(auto const& v: mmf){
        //   if(fabs(v-tottime)<0.0001){
        //     add=false;
        //     break;
        //   }
        // }
        if (add) {
          wwf.push_back(n);
          mmf.push_back(tottime);
          ssf.push_back(wtime * tottime / 50.);
        }
      }

      dabs = tangle - canglepi;
      if (fabs(dabs) < mcut)
        dabs = mcut;
      if (dabs < -bcut && dabs > -scut) {
        si = TMath::Sin(canglepi);
        ww = 0.05 * fabs(wcut / dabs);
        n = ww * lrad * si * si * dphi / TMath::TwoPi();
        add = true;

        // for(auto const& v: mms){
        //   if(fabs(v-tottime)<0.0001){
        //     add=false;
        //     break;
        //   }
        // }
        if (add) {
          wws.push_back(n);
          mms.push_back(tottime);
          sss.push_back(wtime * tottime / 50.);
        }
      }
    }
  }

  pdfuf = TF1("pdff", gg_fpdf, 0, mrange, 3 * mmf.size() + 2);
  pdfus = TF1("pdfs", gg_fpdf, 0, mrange, 3 * mms.size() + 2);

  pdfuf.SetParameter(0, 1);
  pdfus.SetParameter(0, 1);
  pdfuf.SetParameter(1, mmf.size());
  pdfus.SetParameter(1, mms.size());
  for (uint i = 0; i < mmf.size(); i++) {
    pdfuf.SetParameter(3 * i + 2, wwf[i]);
    pdfuf.SetParameter(3 * i + 3, mmf[i]);
    pdfuf.SetParameter(3 * i + 4, ssf[i]);
  }

  for (uint i = 0; i < mms.size(); i++) {
    pdfus.SetParameter(3 * i + 2, wws[i]);
    pdfus.SetParameter(3 * i + 3, mms[i]);
    pdfus.SetParameter(3 * i + 4, sss[i]);
  }

  wwf.clear();
  mmf.clear();
  ssf.clear();
  wws.clear();
  mms.clear();
  sss.clear();
}

void PndDrcReco::TimeImagingWithAnalyticalPdf(int barId, int ch)
{
  double noise = 3e-4;
  int pid = fParticleArray[fPidTrue];
  fhTime[pid]->Fill(fTimeHit);
  fhTimeA[pid][ch]->Fill(fTimeHit);
  bool reflected;
  if (fMCTrack->GetMomentum().Z() > 0)
    reflected = kTRUE;
  else
    reflected = kFALSE;

  TF1 anapdf[5];
  BuildPdfs(anapdf[2], anapdf[3], ch, barId, fMomInBar, fPosInBar.Z(), reflected);

  // gg_c->// cd(1);
  // double fmax = anapdf[2].GetMaximum(1,40);
  // if(fmax<anapdf[3].GetMaximum()){
  //   fmax = anapdf[3].GetMaximum(1,40);
  // }
  // if(anapdf[2].GetMaximum(1,40)>0) anapdf[2].SetParameter(0,1/fmax);
  // if(anapdf[3].GetMaximum(1,40)>0) anapdf[3].SetParameter(0,1/fmax);
  // anapdf[2].SetLineColor(kBlue);
  // anapdf[3].SetNpx(500);
  // anapdf[2].SetNpx(500);
  // anapdf[2].Draw("same");
  // anapdf[3].SetLineColor(kRed);
  // anapdf[3].Draw("same");
  // gg_c->cd(2);
  // hAngAna->Draw();
  // TLine *line = new TLine(0,0,0,1000);
  // line->SetX1(fAngle[2]);
  // line->SetX2(fAngle[2]);
  // line->SetY1(gPad->GetUymin());
  // line->SetY2(fhTang[2]->GetMaximum()*1.05);
  // line->SetLineColor(kBlue);
  // line->Draw();
  // TLine *line1 = new TLine(0,0,0,1000);
  // line1->SetX1(fAngle[3]);
  // line1->SetX2(fAngle[3]);
  // line1->SetY1(gPad->GetUymin());
  // line1->SetY2(fhTang[2]->GetMaximum()*1.05);
  // line1->SetLineColor(kRed);
  // line1->Draw();
  // gg_c->Update();
  // gg_c->WaitPrimitive();
  // hAngAna->Reset();

  fLk[2][2] += TMath::Log(anapdf[2].Eval(fTimeHit) + noise);
  fLk[2][3] += TMath::Log(anapdf[3].Eval(fTimeHit) + noise);
  fHitsE[2][pid]++;
}

void PndDrcReco::TimeImaging(int ch)
{
  double noise = 1e-6;
  int momid = fMom * 10 + 0.5;
  int thetaid = fTheta + 0.5;
  int pid = fParticleArray[fPidTrue];

  fhTime[pid]->Fill(fTimeHit);
  if (fhPdf[2][momid][thetaid][ch] && fhPdf[3][momid][thetaid][ch]) {

    // gg_c->cd(1);
    // double hmax =  fhPdf[2][momid][thetaid][ch]->GetMaximum();
    // if(hmax<fhPdf[3][momid][thetaid][ch]->GetMaximum()) hmax = fhPdf[3][momid][thetaid][ch]->GetMaximum();
    // fhPdf[2][momid][thetaid][ch]->Scale(1/hmax);
    // fhPdf[3][momid][thetaid][ch]->Scale(1/hmax);
    // fhPdf[2][momid][thetaid][ch]->GetXaxis()->SetRangeUser(10, 40);
    // fhPdf[2][momid][thetaid][ch]->Draw();
    // fhPdf[3][momid][thetaid][ch]->Draw("same");

    fLk[1][2] += TMath::Log(fhPdf[2][momid][thetaid][ch]->GetBinContent(fhPdf[2][momid][thetaid][ch]->FindBin(fTimeHit)) + noise);
    fLk[1][3] += TMath::Log(fhPdf[3][momid][thetaid][ch]->GetBinContent(fhPdf[3][momid][thetaid][ch]->FindBin(fTimeHit)) + noise);
    fHitsE[1][pid]++;
  }
}

void PndDrcReco::DetermineBarId(double &boxPhi, int &barId)
{
  double startPhi = fPosInBar.Phi() / TMath::Pi() * 180;

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
  if (fRadType == 4)
    barId = (int)(2 + (boxPhi - startPhi) / fBarPhi);
  if (fRadType == 3)
    barId = (int)(1.5 + (boxPhi - startPhi) / fBarPhi);
  if (fRadType == 2)
    barId = (int)(1 + (boxPhi - startPhi) / fBarPhi);
  if (fRadType == 1)
    barId = 0;
  if (barId > 3 || barId < 0) {
    LOG(error) << "PndDrcReco: bar id is wrong. barId = " << barId;
    exit(0);
  }
}

double PndDrcReco::FindPeak()
{
  double cherenkovreco = -1;

  if (fHist->Integral() > 20) {
    TCanvas *c = new TCanvas("c", "c", 0, 0, 800, 600);
    int nfound = fSpect->Search(fHist, 1, "", 0.6);
    Float_t *xpeaks = (Float_t *)fSpect->GetPositionX();
    if (nfound > 0)
      cherenkovreco = xpeaks[0];
    fFit->SetParameter(1, cherenkovreco); // peak
    fFit->SetParameter(2, 0.01);          // width
    fHist->Fit("fgaus", "Q", "", cherenkovreco - 0.02, cherenkovreco + 0.02);
    cherenkovreco = fFit->GetParameter(1);

    if (cherenkovreco < 0 || cherenkovreco > 1)
      cherenkovreco = 0;

    if (fVerbose > 1) {
      fHist->Draw();
      c->Modified();
      c->Update();
      c->WaitPrimitive();
      // c->Print(Form("pic/animpid/animpid_%d.png",g_num++));
    }
  }
  fHist->Reset();

  return cherenkovreco;
}

int PndDrcReco::FindPdg(double mom, double cangle)
{
  double tdiff, diff = 100;
  int minid = 0;
  for (int i = 0; i < 5; i++) {
    tdiff = fabs(cangle - acos(sqrt(mom * mom + fMass[i] * fMass[i]) / mom / 1.46907)); // 1.46907 - fused silica
    if (tdiff < diff) {
      diff = tdiff;
      minid = i;
    }
  }
  return fPdg[minid];
}

void PndDrcReco::CanvasAdd(TString name, int w, int h)
{
  fCanvasList->Add(new TCanvas(name, name, 0, 0, w, h));
}

void PndDrcReco::CanvasSave(TString path)
{
  gROOT->SetBatch(1);
  TIter next(fCanvasList);
  TCanvas *c = 0;
  gSystem->mkdir(path, kTRUE);
  while ((c = (TCanvas *)next())) {
    TString name = c->GetName();
    TCanvas *cc = new TCanvas(name + "exp", "cExport", 0, 0, 800, 400);
    cc = (TCanvas *)c->DrawClone();
    cc->SetCanvasSize(800, 400);
    cc->SetBottomMargin(0.12);
    TIter nexth(cc->GetListOfPrimitives());
    TObject *obj;
    while ((obj = nexth())) {
      if (obj->InheritsFrom("TH1")) {
        TH1F *hh = (TH1F *)obj;
        hh->GetXaxis()->SetTitleSize(0.06);
        hh->GetYaxis()->SetTitleSize(0.06);

        hh->GetXaxis()->SetLabelSize(0.05);
        hh->GetYaxis()->SetLabelSize(0.05);

        hh->GetXaxis()->SetTitleOffset(0.85);
        hh->GetYaxis()->SetTitleOffset(0.85);
      }
    }
    cc->Modified();
    cc->Update();
    TString uid("");
    if (fOutFile.Contains("/")) {
      TString tname = fOutFile;
      path = tname.Remove(fOutFile.Last('/')) + "/";
      tname = fOutFile;
      uid = tname.Remove(0, fOutFile.Last('/') + 1);
    }
    cc->Print(path + uid + name + ".png");
    cc->Print(path + uid + name + ".C");
  }
  gROOT->SetBatch(0);
}

// -----   Finish Task   ---------------------------------------------------
void PndDrcReco::Finish()
{

  double step_mom = 0.1; //[0,4]
  double step_theta = 1; //[22,140]
  double step_phi = 0.4; //[0,22]

  int id_mom = fMom / step_mom + 0.1;
  int id_theta = fTheta / step_theta + 0.2;
  int id_phi = fPhi / step_phi + 0.2;
  TString strrun = Form("_%d_%d_%d_%f1.2", id_mom, id_theta, id_phi, fR1);
  for (int m = 0; m < 3; m++) {
    if (fMethod == m || fMethod == 5) {
      CanvasAdd(Form("hLh_%d", m) + strrun);
      TF1 *ff;
      double m1 = 0, m2 = 0, s1 = 0, s2 = 0;

      if (fhLk[m][2]->Integral() > 10) {
        fhLk[m][2]->Fit("gaus", "S");
        ff = fhLk[m][2]->GetFunction("gaus");
        m1 = ff->GetParameter(1);
        s1 = ff->GetParameter(2);
      }
      if (fhLk[m][3]->Integral() > 10) {
        fhLk[m][3]->Fit("gaus", "S");
        ff = fhLk[m][3]->GetFunction("gaus");
        m2 = ff->GetParameter(1);
        s2 = ff->GetParameter(2);
      }

      fSeparation[m] = (fabs(m2 - m1)) / (0.5 * (s1 + s2));
      LOG(info) << "separation " << m << " " << fSeparation[m];

      fhLk[m][2]->SetTitle(Form("S = %2.2f", fSeparation[m]));
      fhLk[m][2]->SetLineColor(4);
      fhLk[m][3]->SetLineColor(2);
      fhLk[m][2]->Draw();
      fhLk[m][3]->Draw("same");

      if (m == 0) {
        CanvasAdd("hDiff" + strrun);
        fhDiff[2]->SetLineColor(4);
        fhDiff[2]->Draw();
        fhDiff[3]->SetLineColor(2);
        fhDiff[3]->Draw("same");

        // CanvasAdd("hPathAll"+strrun);
        // hPathAll->Draw();
        // hPath->SetLineColor(2);
        // hPath->Draw("same");

        CanvasAdd("hAngle" + strrun);
        for (int i = 2; i < 4; i++) {
          fFit->SetParameter(1, fAngle[i]); // peak
          fFit->SetParameter(2, 0.01);      // width
          if (fhTang[i]->Integral() > 10) {
            fhTang[i]->Fit("fgaus", "Q", "", fAngle[i] - 0.05, fAngle[i] + 0.05);
            fhTang[i]->Fit("fgaus", "QM", "", fAngle[i] - 0.05, fAngle[i] + 0.05);
            fSpr[i] = fFit->GetParameter(2);
            fCangle[i] = fFit->GetParameter(1);
          }
        }
        fhTang[2]->SetTitle(Form("#theta_{C} = %2.3f  #sigma = %2.4f      #theta_{C} = %2.3f  #sigma = %2.4f", fCangle[3], fSpr[3], fCangle[2], fSpr[2]));

        fhTang[2]->SetLineColor(4);
        fhTang[2]->Draw();

        fhTang[3]->SetLineColor(2);
        fhTang[3]->Draw("same");

        TLine *line = new TLine(0, 0, 0, 1000);
        line->SetX1(fAngle[2]);
        line->SetX2(fAngle[2]);
        line->SetY1(gPad->GetUymin());
        line->SetY2(fhTang[2]->GetMaximum() * 1.05);
        line->SetLineColor(kBlue);
        line->Draw();

        TLine *line1 = new TLine(0, 0, 0, 1000);
        line1->SetX1(fAngle[3]);
        line1->SetX2(fAngle[3]);
        line1->SetY1(gPad->GetUymin());
        line1->SetY2(fhTang[2]->GetMaximum() * 1.05);
        line1->SetLineColor(kRed);
        line1->Draw();

        CanvasAdd("h2Time" + strrun);
        h2Time->Draw("colz");

        // CanvasAdd("hSD"+strrun);
        // hSD->Draw("colz");
      }
    }
  }

  // store lut corrections
  if (fStoreCorr) {
    LOG(info) << "PndDrcReco: storing corrections in  " << fCorrFile;

    TFile fc(fCorrFile, "recreate");
    TTree *tc = new TTree("corr", "corr");
    int pmt;
    double mean, spr;
    tc->Branch("pmt", &pmt, "pmt/I");
    tc->Branch("mom", &fBin_mom, "mom/I");
    tc->Branch("theta", &fBin_theta, "theta/I");
    tc->Branch("mean", &mean, "mean/D");
    tc->Branch("spr", &spr, "spr/D");
    TF1 *fit = new TF1("fgaus", "[0]*exp(-0.5*((x-[1])/[2])*(x-[1])/[2]) +x*[3]+[4]", 0.35, 0.9);
    fit->SetParameter(1, 0); // mean
    fit->SetParLimits(1, -0.012, 0.012);
    fit->SetParLimits(2, 0.006, 0.015); // width
    for (int i = 0; i < 9; i++) {
      mean = 0;
      spr = 0;
      pmt = i;
      if (fhCorr[i]->GetEntries() > 100) {
        gg_c->cd();
        fhCorr[i]->Fit("fgaus", "MQ", "", -0.03, 0.03);
        mean = -fit->GetParameter(1);
        spr = fit->GetParameter(2);
        if (spr < 0.003 || spr > 0.015 || fabs(mean) > 0.01) {
          spr = 0.010;
          mean = 0;
        }
        fhCorr[i]->Draw();
        gg_c->Update();
        gg_c->WaitPrimitive();
      }
      tc->Fill();
      LOG(info) << "pmt " << pmt << " mean=" << mean << " spr=" << spr;
    }
    tc->Write();
    fc.Write();
    fc.Close();
  }

  CanvasAdd("hTime" + strrun);
  fhTime[2]->SetLineColor(4);
  fhTime[2]->Draw();
  fhTime[3]->SetLineColor(2);
  fhTime[3]->Draw("same");

  // CanvasAdd("hEnergy"+strrun);
  // hEnergy->Draw();

  gStyle->SetOptStat(0);
  CanvasAdd("hNph" + strrun);
  for (int m = 0; m < 3; m++) {
    for (int i = 0; i < 5; i++) {
      if (fEvents[i] < 1)
        continue;
      if (fhNph[m][i]->Integral() > 10) {
        fhNph[m][i]->Fit("gaus", "Q");
        TF1 *ff = fhNph[m][i]->GetFunction("gaus");
        ff->SetLineColor(kBlack);
        ff->SetLineWidth(2);
        fNph[m][i] = ff->GetParameter(1);
      }

      fEfficiency[i] = fEventsEff[i] / (double)fEvents[i];
      fMissId[i] = fEventsMis[i] / (double)fEvents[i];
    }

    LOG(info) << "N" << m << " pi " << fNph[m][2] << "  K " << fNph[m][3];
  }

  for (int m = 0; m < 3; m++) {
    fhNph[m][2]->SetLineColor(4);
    fhNph[m][2]->Draw((m == 0) ? "" : "same");
    fhNph[m][3]->SetLineColor(2);
    fhNph[m][3]->Draw("same");
  }

  LOG(info) << "Eff_K  " << fEfficiency[3] << "  Mis_K  " << fMissId[3];
  LOG(info) << "Eff_pi  " << fEfficiency[2] << "  Mis_pi  " << fMissId[2];

  fFileOut->cd();
  fhTang[2]->Write();
  fhTang[3]->Write();
  fTreeOut->Fill();
  fTreeOut->Write();
  fFileOut->Write();

  CanvasSave(Form("data/reco/%d/", id_mom));

  CanvasAdd("hPdf" + strrun);
  for (int c = 0; c < 0; c++) {
    if (fhTimeA[2][c]->Integral() < 10 || fhTimeA[2][c]->Integral() < 10)
      continue;
    TF1 anapdf[5];
    bool reflected;
    if (fMCTrack->GetMomentum().Z() > 0)
      reflected = kTRUE;
    else
      reflected = kFALSE;
    BuildPdfs(anapdf[2], anapdf[3], c, 1, fMomInBar, fPosInBar.Z(), reflected);

    double hmax = fhTimeA[2][c]->GetMaximum();
    double fmax = anapdf[2].GetMaximum(1, 40);

    if (hmax < fhTimeA[3][c]->GetMaximum()) {
      hmax = fhTimeA[3][c]->GetMaximum();
      fmax = anapdf[3].GetMaximum(1, 40);
    }

    fhTimeA[2][c]->Scale(1 / hmax);
    fhTimeA[3][c]->Scale(1 / hmax);

    if (anapdf[2].GetMaximum(1, 40) > 0)
      anapdf[2].SetParameter(0, 1 / fmax);
    if (anapdf[3].GetMaximum(1, 40) > 0)
      anapdf[3].SetParameter(0, 1 / fmax);

    // fhTimeA[2][c]->GetYaxis()->SetRangeUser(0,1.1);
    // fhTimeA[3][c]->GetYaxis()->SetRangeUser(0,1.1);
    fhTimeA[2][c]->SetLineColor(kBlue + 2);
    fhTimeA[2][c]->Draw("hist");
    fhTimeA[3][c]->SetLineColor(kRed + 2);
    fhTimeA[3][c]->Draw("histsame");
    anapdf[2].SetNpx(1000);
    anapdf[3].SetNpx(1000);
    anapdf[2].SetLineColor(kBlue);
    anapdf[2].Draw("same");
    anapdf[3].SetLineColor(kRed);
    anapdf[3].Draw("same");

    // int momid=fMom*10+0.5;
    // int thetaid=fTheta+0.5;
    // hmax =  fhPdf[2][momid][thetaid][c]->GetMaximum();
    // if(hmax<fhPdf[3][momid][thetaid][c]->GetMaximum()) hmax = fhPdf[3][momid][thetaid][c]->GetMaximum();
    // fhPdf[2][momid][thetaid][c]->Scale(1/hmax);
    // fhPdf[3][momid][thetaid][c]->Scale(1/hmax);
    // fhPdf[2][momid][thetaid][c]->Draw("hist same");
    // fhPdf[3][momid][thetaid][c]->Draw("hist same");

    gPad->Update();
    gPad->WaitPrimitive();
  }

  for (int l = 0; l < 3; l++)
    fLut[l]->Clear();
  LOG(info) << "PndDrcReco: reconstruction finished";
}

ClassImp(PndDrcReco)
