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
// -----                PndMdtParamDigi source file                  -----
// -----                  Created 29/03/11  by  S.Spataro              -----
// -------------------------------------------------------------------------

#include "PndMdtParamDigi.h"
#include "TVector3.h"
#include "TRandom.h"
#include "TSystem.h"
#include "TFile.h"
#include <assert.h>
#include <iostream>
#include "TStopwatch.h"
#include "PndMdtIGeometry.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TMath.h"
#include <algorithm>
//#include <CLHEP/Random/RandExponential.h>
//#include <CLHEP/Random/RandGeneral.h>
//#include <CLHEP/Random/RandLandau.h>
//#include <CLHEP/Random/RandGauss.h>
#include "TStopwatch.h"

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndMdtParamDigi::PndMdtParamDigi()
  : TNamed("PndMdtParamDigi", "parameterized simulation of MDT"), fSamplingRate(1e8), fTrF(0), fUseNoise(kTRUE), fGaussianAmp(kTRUE), fNoiseSigmaAnode(1e-3),
    fNoiseSigmaStrip(1e-5), fNumofTruncation(1.), fParamsChanged(kTRUE), fDetailedSim(kFALSE), fUseAvaHist(kFALSE), fUsePlot(kTRUE), fVerbose(0)
{
  cWIRERADIUS = 0.0025001; // unit: cm
  cCELLSIZE = 0.5;         // unit: cm
  cSTRIPWIDTH = 1.0;       // unit: cm
  cMAXRADIUS = 0.00525;    // not changeable
  cMAPFACTOR = 1000.;      // not changeable

  cUNITLOGDR = TMath::Log(1. + cMAPFACTOR * (cMAXRADIUS - cWIRERADIUS)) / NRAD;
  cUNITPHI = TMath::Pi() * 2. / NPHI;
  fSamplingInterval = 1e9 / fSamplingRate;
  fRestMass[0] = 0.00510998928;
  fRestMass[1] = 0.1056583715;
  fRestMass[2] = 0.13957018;
  fRestMass[3] = 0.493667;
  fRestMass[4] = 0.938272046; // GeV

  SPEEDOFLIGHT = 299792458; // m/s
}
Bool_t PndMdtParamDigi::Init()
{
  fTrF = new TF1("fTrF", "(x*6./0.015)**6*exp(-x*6./0.015)", 0., 2.);
  if (!fTrF)
    return kFALSE;

  TString dir(gSystem->Getenv("VMCWORKDIR"));
  TString mdtParamsFile(dir + "/macro/params/MdtDigiParams.root");
  TFile *file = TFile::Open(mdtParamsFile);
  if (!file->IsOpen())
    return kFALSE;

  gFreePath[0] = (TGraphErrors *)file->Get("gElec");
  gFreePath[1] = (TGraphErrors *)file->Get("gMuon");
  gFreePath[2] = (TGraphErrors *)file->Get("gPion");
  gFreePath[3] = (TGraphErrors *)file->Get("gKaon");
  gFreePath[4] = (TGraphErrors *)file->Get("gProton");
  hAvaSize = (TH1F *)file->Get("hava");
  // hAnodeI = (TH1F*)file->Get("hAnodeI");
  // hCathodeIx= (TH1F*)file->Get("hCathodeIx");
  // hCathodeIy= (TH1F*)file->Get("hCathodeIy");
  cout << "PndMdtParamDigi::PndMdtParamDigi(), test" << endl;
  cout << "mean frem path @1 GeV of electron " << gFreePath[0]->Eval(1.) << endl;
  cout << "mean frem path @1 GeV of muon " << gFreePath[1]->Eval(1.) << endl;
  cout << "mean frem path @1 GeV of pion " << gFreePath[2]->Eval(1.) << endl;
  cout << "mean frem path @1 GeV of kaon " << gFreePath[3]->Eval(1.) << endl;
  cout << "mean frem path @1 GeV of proton " << gFreePath[4]->Eval(1.) << endl;
  // cout<<"avalanche size #"<<hAvaSize->GetRandom()<<endl;
  // cout<<"induced current on anode @t = 0.05us, "<<hAnodeI->GetBinContent(5)<<endl;
  // cout<<"induced current on cathode @t = 0.05us, theta 0, "<<hCathodeIx->GetBinContent(5)<<endl;
  // cout<<"induced current on cathode @t = 0.05us, theta PI/2, "<<hCathodeIy->GetBinContent(5)<<endl;
  // file->Close();
  fUseAvaHist = kTRUE;
  // double* lProbData = new double[hAvaSize->GetNbinsX()];
  // for(Int_t ib=0; ib < hAvaSize->GetNbinsX(); ++ ib)
  //  lProbData[ib] = hAvaSize->GetBinContent(ib+1);
  // fRandAva = new CLHEP::RandGeneral(lProbData, hAvaSize->GetNbinsX());

  TString mdtParamsFileI(dir + "/macro/params/MdtDigiParamsI.root");
  TFile *file2 = TFile::Open(mdtParamsFileI);
  if (!file2->IsOpen())
    return kFALSE;

  for (Int_t ir = 0; ir < NRAD; ++ir) {
    TString hisName("Ir");
    hisName += ir;
    hAnodeI1d[ir] = (TH1F *)file2->Get(hisName);
  }
  for (Int_t ir = 0; ir < NRAD; ++ir) {
    for (Int_t ip = 0; ip < NPHI; ++ip) {
      TString hisName("Ir");
      hisName += ir;
      hisName += "t";
      hisName += ip;
      hCathodeI2d[ir][ip] = (TH1F *)file2->Get(hisName);
    }
  }
  hAva1D = (TH1F *)file2->Get("hAva1D");
  hAva2D = (TH2F *)file2->Get("hAva2D");
  cout << "1D avalanche PDF " << hAva1D->GetTitle() << endl;
  cout << "2D avalanche PDF " << hAva2D->GetTitle() << endl;

  for (Int_t ir = 0; ir < hAva1D->GetNbinsX(); ++ir)
    if (hAva1D->GetBinContent(ir + 1) > 0) {
      fProbFunc1D.push_back(AvaBinType(ir, hAva1D->GetBinContent(ir + 1)));
    }

  for (Int_t ir = 0; ir < hAva2D->GetNbinsX(); ++ir)
    for (Int_t ip = 0; ip < hAva2D->GetNbinsY(); ++ip)
      if (hAva2D->GetBinContent(ir + 1, ip + 1) > 0) {
        fProbFunc2D.push_back(AvaBinType(ir + ip * NRAD, hAva2D->GetBinContent(ir + 1, ip + 1)));
      }

  if (fUsePlot) {
    hIonNum = new TH1F("hIonizationNum", "number of primary ionization", 20, 0, 20);
    hTrack = new TH2F("hTrack", "trajectory of primary track", 100, -0.5, 0.5, 100, -0.5, 0.5);
    hAvaPos = new TH2F("hAvaPos", "avalanche profile", 100, -0.01, 0.01, 100, -0.01, 0.01);
    hAvaNum = new TH1F("hAvaNum", "avalanche", 20000, 0, 20000);
    hIndex = new TH1F("hIndex", "raidal avlanche structure", 100, 0, 100);
    hIndex->GetXaxis()->SetTitle("Start bin of Drift");
    h2dIndex = new TH2F("h2dIndex", "raidal avlanche structure", 100, 0, 100, 100, 0, 100);
    h2dIndex->GetXaxis()->SetTitle("Start bin of Drift");

    hExp1 = new TH1F("expf", "exponential dis", 100, 0, 1);
    // hExp2;
    hGen = new TH1F("gen", "general dist", 100, 0, 10000);
    hLan = new TH1F("lan", "landau dis", 20, 0, 20);
  }
  fSignalDataAnode.resize(fSamplingSize);

  return kTRUE;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndMdtParamDigi::~PndMdtParamDigi() {}
// setup parameters
PndMdtParamDigi &PndMdtParamDigi::SetParams(Int_t ptlType, TVector3 iniP, TVector3 iniPos, TVector3 finalPos, Double_t stripLen)
{
  fParticleType = ptlType;
  fParticleMomentum = iniP;
  fInitPostion = iniPos;
  fExitPosition = finalPos;
  fStripLength = stripLen;
  fParamsChanged = kTRUE;
  fSignalDataStripM.clear();

  if (fVerbose > 1) {
    cout << "===============================================" << endl;
    TString partileName[5] = {"Elec", "Muon", "Pion", "Kaon", "Proton"};
    cout << "PndMdtParamDigi::SetParams, " << partileName[ptlType] << endl
         << " initial momentum#" << iniP.X() << ", " << iniP.Y() << ", " << iniP.Z() << ", " << iniP.Mag() << "GeV" << endl
         << " initial position#" << iniPos.X() << ", " << iniPos.Y() << ", " << iniPos.Z() << ", " << iniPos.Mag() << "cm" << endl
         << " final position#" << finalPos.X() << ", " << finalPos.Y() << ", " << finalPos.Z() << ", " << finalPos.Mag() << "cm" << endl;
    cout << "===============================================" << endl;
  }
  return *this;
}

void PndMdtParamDigi::Draw()
{
  if (fUsePlot) {
    TCanvas *c = new TCanvas("cPndMdtParamDigi", "PndMdtParamDigi", 800, 600);
    c->Divide(2, 3);
    c->cd(1);
    hTrack->Draw("colz");
    c->cd(2);
    // hAva2D->Draw("colz");
    hIonNum->Draw();
    c->cd(3);
    hAvaPos->Draw("colz");
    c->cd(5);
    hIndex->DrawNormalized();
    c->cd(5);
    hAva1D->SetLineColor(kRed);
    hAva1D->SetLineWidth(2);
    hAva1D->Draw("same");
    c->cd(6);
    hAva2D->Draw("colz");
    c->cd(4);
    h2dIndex->Draw("colz");
    TCanvas *c2 = new TCanvas("dist", "PndMdtParamDigi", 800, 600);
    c2->Divide(2, 2);
    c2->cd(1);
    hExp1->Draw();
    c2->cd(2);
    c2->cd(3);
    hGen->Draw();
    c2->cd(4);
    hLan->Draw();
  }
  TCanvas *c1 = new TCanvas("cSignal", "Signals", 900, 900);
  c1->Divide(3, (fSignalDataStripM.size() + 1) / 3);
  c1->cd(1);
  TH1F *hAI = new TH1F("hAI", "induced current @anode", fSamplingSize, 0, fSamplingSize * fSamplingInterval / 1.e3);
  hAI->GetYaxis()->SetTitle("I(#muA)");
  for (Int_t i = 0; i < fSamplingSize; ++i) {
    hAI->SetBinContent(i + 1, fSignalDataAnode[i]);
  }
  hAI->Draw();
  std::map<Int_t, std::vector<Double_t>>::iterator it = fSignalDataStripM.begin();
  std::map<Int_t, std::vector<Double_t>>::iterator end = fSignalDataStripM.end();
  Int_t ipad = 1;
  for (; it != end; ++it, ++ipad) {
    c1->cd(1 + ipad);
    TString name = "hCI";
    name += it->first;
    TString title = "induced current @strip ";
    title += it->first;
    TH1F *hCI = new TH1F(name, title, fSamplingSize, 0, fSamplingSize * fSamplingInterval / 1.e3);
    hCI->GetYaxis()->SetTitle("I(#muA)");
    std::vector<Double_t> &fSignalDataStrip = it->second;
    for (Int_t i = 0; i < fSamplingSize; ++i) {
      hCI->SetBinContent(i + 1, fSignalDataStrip[i]);
    }
    hCI->Draw();
  }
}
void PndMdtParamDigi::Compute(Bool_t useConvolution)
{
  GetSignal(useConvolution);
}

void PndMdtParamDigi::GetRawSignalbySimAvalanche(Double_t fNoiseLevel)
{

  if (!fParamsChanged)
    return;

  fParamsChanged = kFALSE;
  memset(&fSignalDataAnode[0], 0, fSamplingSize * sizeof(Double_t));
  // memset(&fSignalDataStrip[0], 0, fSamplingSize*sizeof(Double_t));
  fSignalDataStripM.clear();

  TVector3 fUnitMotion = (fExitPosition - fInitPostion).Unit();
  Double_t fMaxPath = (fExitPosition - fInitPostion).Mag();
  Double_t mBeta = fParticleMomentum.Mag() / fRestMass[fParticleType];
  if (fParticleType == 4 && mBeta < 0.2)
    return;
  Double_t mPath = gFreePath[fParticleType]->Eval(mBeta);
  if (mPath < 0.)
    return;
  const Double_t fScaleFactor = 40.;

  Int_t jj;
  Int_t ii;

  ValueErrorType NumofPrimaryIonizaionMpv;
  GetMPVofPrimaryIonization(fParticleType, fParticleMomentum, NumofPrimaryIonizaionMpv);

  TVector3 fCurrentPosofCluster = fInitPostion;
  Int_t fNumofCluster(0);
  Int_t fNumofSingleIonization = 0;
  Int_t fNumofTotalIonizaion = 0;
  Int_t fNumofIonsofThisCluster = 0;
  Int_t fNumofTotalIons = 0;
  TVector2 fIonProductionPos;
  Double_t fStepLen = 0.;
  Double_t fTrackLen = 0.;
  Double_t fTime = 0.;

  TStopwatch sw;
  sw.Start();
  do {
    fStepLen = gRandom->Exp(mPath);
    fTime += fStepLen / (mBeta * SPEEDOFLIGHT * 1.e7); // nano second
    fTrackLen += fStepLen;
    fCurrentPosofCluster += fStepLen * fUnitMotion;

    if (fCurrentPosofCluster.XYvector().Mod() <= cWIRERADIUS)
      continue;

    fNumofSingleIonization = gRandom->Landau(NumofPrimaryIonizaionMpv.first, NumofPrimaryIonizaionMpv.second);
    fNumofTotalIonizaion += fNumofSingleIonization;

    if (fUsePlot) {
      hIonNum->Fill(fNumofSingleIonization);
      hTrack->Fill(fCurrentPosofCluster.X(), fCurrentPosofCluster.Y());
    }

    Int_t iSign = fCurrentPosofCluster.Z() >= 0 ? 1 : -1;
    Int_t zIndex = Int_t(fCurrentPosofCluster.Z() / cSTRIPWIDTH + iSign * 0.5);
    zIndex = zIndex >= 0 ? zIndex * 2 : -2 * zIndex - 1;

    // cout<<"fSignalDataStrip.size()#"<<fSignalDataStrip.size()<<endl;
    std::vector<Double_t> &fSignalDataStrip = fSignalDataStripM[zIndex];
    if (fSignalDataStrip.size() != fSamplingSize)
      fSignalDataStrip.resize(fSamplingSize);

    if (fNumofSingleIonization > 0) {
      fNumofIonsofThisCluster = hAvaSize->GetRandom();
      fNumofTotalIons += fNumofIonsofThisCluster;
      if (fUsePlot)
        hAvaNum->Fill(fNumofIonsofThisCluster);

      if (fNumofIonsofThisCluster > 10) {
        // Double_t lx = fCurrentPosofCluster.XYvector().Mod() - cWIRERADIUS;
        // Double_t lx1 = log(1.+lx);
        // clusInfo.fStartTime = fTime +  (lx*(p0 + p1*pow(lx, 0.25)) + lx1*(p2 + p3*lx1));

        Double_t fStartTime = fTime + GetElectronDriftTime(fCurrentPosofCluster.XYvector()); // micro second
        Int_t tShift = Int_t(fStartTime / fSamplingInterval);
        if (tShift >= fSamplingSize)
          tShift = fSamplingSize - 1;
        // Int_t refPhiIndex = Int_t(fCurrentPosofCluster.Phi()/cUNITPHI); //[R.K. 01/2017] unused variable?
        // if(refPhiIndex>=100) refPhiIndex = 99;

        TVector2 unitV = fCurrentPosofCluster.XYvector().Unit();
        std::map<Int_t, Int_t> fAnodeIndex; // fired region
        std::map<Int_t, Int_t> fStripIndex; // fired region

        for (ii = 0; ii < fNumofIonsofThisCluster; ++ii) { // avalanche profile
          // get position of this ion
          SamplingPosition(unitV, fIonProductionPos);
          // cout<<"sampling done, r = "<<fIonProductionPos.Mod()<<endl;
          Int_t rIndex = Int_t(TMath::Log(1. + cMAPFACTOR * (fIonProductionPos.Mod() - cWIRERADIUS)) / cUNITLOGDR);
          Int_t phiIndex = Int_t(fIonProductionPos.Phi() / cUNITPHI);
          if (fUsePlot) {
            hIndex->Fill(rIndex);
            // h2dIndex->Fill(rIndex, phiIndex-refPhiIndex < 0 ? 100 + phiIndex-refPhiIndex : phiIndex-refPhiIndex);
            h2dIndex->Fill(rIndex, phiIndex);
            hAvaPos->Fill(fIonProductionPos.X(), fIonProductionPos.Y());
          }
          // cout<<"plot done"<<endl;
          // if(rIndex>=NRAD) rIndex = NRAD-1;
          // if(phiIndex >= NPHI) phiIndex = NPHI -1;
          if (rIndex >= NRAD)
            continue;
          if (phiIndex >= NPHI)
            continue;
          // cout<<"rIndex = "<<rIndex<<", phiIndex = "<<phiIndex<<endl;
          ++fAnodeIndex[rIndex];
          ++fStripIndex[rIndex + phiIndex * NRAD];
        }
        std::map<Int_t, Int_t>::const_iterator mit = fAnodeIndex.begin();
        std::map<Int_t, Int_t>::const_iterator mend = fAnodeIndex.end();
        for (; mit != mend; ++mit) {
          if (mit->second > fNumofTruncation) {
            for (ii = tShift, jj = 1; ii < fSamplingSize; ++ii, ++jj) {
              fSignalDataAnode[ii] += mit->second * hAnodeI1d[mit->first]->GetBinContent(jj);
            }
          }
        }
        mit = fStripIndex.begin();
        mend = fStripIndex.end();
        for (; mit != mend; ++mit) {
          Double_t fN = mit->second;
          if (fN > fNumofTruncation) {
            Int_t rIdx = mit->first % NRAD;
            Int_t phiIdx = mit->first / NRAD;
            // cout<<"Index ("<<rIdx<<", "<<phiIdx<<") = "<<fN<<endl;
            for (ii = tShift, jj = 1; ii < fSamplingSize; ++ii, ++jj) {
              fSignalDataStrip[ii] += fScaleFactor * mit->second * hCathodeI2d[rIdx][phiIdx]->GetBinContent(jj);
            }
          }
        }
      }
    }
    ++fNumofCluster;
  } while (fTrackLen < fMaxPath);

  if (fUseNoise) {
    AddNoise(fNoiseLevel);
  }
  sw.Stop();
  cout << "PndMdtParamDigi::GetRawSignal, cost " << sw.CpuTime() << " s"
       << " clusters#" << fNumofCluster << " primary ionization#" << fNumofTotalIonizaion << " total ions#" << fNumofTotalIons << endl;
}
void PndMdtParamDigi::GetRawSignalbyWeightingAvalanche(Double_t fNoiseLevel)
{
  static Double_t fScaleFactor = 40.;

  if (!fParamsChanged)
    return;

  fParamsChanged = kFALSE;
  memset(&fSignalDataAnode[0], 0, fSamplingSize * sizeof(Double_t));
  // memset(&fSignalDataStrip[0], 0, fSamplingSize*sizeof(Double_t));
  fSignalDataStripM.clear();

  TVector3 fUnitMotion = (fExitPosition - fInitPostion).Unit();
  Double_t fMaxPath = (fExitPosition - fInitPostion).Mag();
  Double_t mBeta = fParticleMomentum.Mag() / fRestMass[fParticleType];
  Double_t mPath = gFreePath[fParticleType]->Eval(mBeta);
  if (mPath < 0.)
    return;

  Int_t jj;
  Int_t ii;

  ValueErrorType NumofPrimaryIonizaionMpv;
  GetMPVofPrimaryIonization(fParticleType, fParticleMomentum, NumofPrimaryIonizaionMpv);

  TVector3 fCurrentPosofCluster = fInitPostion;
  Int_t fNumofCluster(0);
  Int_t fNumofSingleIonization = 0;
  Int_t fNumofTotalIonizaion = 0;
  Int_t fNumofIonsofThisCluster = 0;
  Int_t fNumofTotalIons = 0;
  Double_t fStepLen = 0.;
  Double_t fTrackLen = 0.;
  Double_t fTime = 0.;

  // TStopwatch sw;
  // sw.Start();
  do {
    fStepLen = gRandom->Exp(mPath);
    fTime += fStepLen / (mBeta * SPEEDOFLIGHT * 1.e7); // nano second
    fTrackLen += fStepLen;
    fCurrentPosofCluster += fStepLen * fUnitMotion;

    if (fCurrentPosofCluster.XYvector().Mod() <= cWIRERADIUS)
      continue;

    fNumofSingleIonization = gRandom->Landau(NumofPrimaryIonizaionMpv.first, NumofPrimaryIonizaionMpv.second);
    fNumofTotalIonizaion += fNumofSingleIonization;

    if (fUsePlot) {
      hIonNum->Fill(fNumofSingleIonization);
      hTrack->Fill(fCurrentPosofCluster.X(), fCurrentPosofCluster.Y());
    }

    Int_t iSign = fCurrentPosofCluster.Z() >= 0 ? 1 : -1;
    Int_t zIndex = Int_t(fCurrentPosofCluster.Z() / cSTRIPWIDTH + iSign * 0.5);
    zIndex = zIndex >= 0 ? zIndex * 2 : -2 * zIndex - 1;

    // cout<<"fSignalDataStrip.size()#"<<fSignalDataStrip.size()<<endl;
    std::vector<Double_t> &fSignalDataStrip = fSignalDataStripM[zIndex];
    if (fSignalDataStrip.size() != fSamplingSize)
      fSignalDataStrip.resize(fSamplingSize);

    if (fNumofSingleIonization > 0) {
      fNumofIonsofThisCluster = hAvaSize->GetRandom();
      fNumofTotalIons += fNumofIonsofThisCluster;
      if (fUsePlot)
        hAvaNum->Fill(fNumofIonsofThisCluster);

      if (fNumofIonsofThisCluster > 1) {
        // Double_t lx = fCurrentPosofCluster.XYvector().Mod() - cWIRERADIUS;
        // Double_t lx1 = log(1.+lx);
        // clusInfo.fStartTime = fTime +  (lx*(p0 + p1*pow(lx, 0.25)) + lx1*(p2 + p3*lx1));

        Double_t fStartTime = fTime + GetElectronDriftTime(fCurrentPosofCluster.XYvector()); // micro second
        Int_t tShift = Int_t(fStartTime / fSamplingInterval);
        if (tShift >= fSamplingSize)
          tShift = fSamplingSize - 1;
        Int_t refPhiIndex = Int_t(fCurrentPosofCluster.Phi() / cUNITPHI);
        // wire signal
        // cout<<"1D, number of fired bins #"<<fProbFunc1D.size()<<endl;
        std::vector<AvaBinType>::const_iterator vit = fProbFunc1D.begin();
        std::vector<AvaBinType>::const_iterator end = fProbFunc1D.end();
        for (; vit != end; ++vit) {
          const AvaBinType &bin = *vit;
          Double_t fN = fNumofIonsofThisCluster * bin.Probabilty;
          if (fN > fNumofTruncation) {
            for (ii = tShift, jj = 1; ii < fSamplingSize; ++ii, ++jj) {
              fSignalDataAnode[ii] += fN * hAnodeI1d[bin.Index]->GetBinContent(jj);
            }
          }
          if (fUsePlot) {
            hIndex->SetBinContent(bin.Index + 1, hIndex->GetBinContent(bin.Index + 1) + fN);
          }
        }
        // cout<<"wire signal done"<<endl;
        // strip signal
        // cout<<"2D, number of fired bins #"<<fProbFunc2D.size()<<endl;
        vit = fProbFunc2D.begin();
        end = fProbFunc2D.end();
        std::map<Int_t, Double_t> fInfo;
        for (; vit != end; ++vit) {
          const AvaBinType &bin = *vit;
          Int_t fN = fNumofIonsofThisCluster * bin.Probabilty;
          if (fN > fNumofTruncation) {
            // retrieve the rIndex, phiIndex;
            Int_t rIdx = bin.Index % NRAD;
            Int_t phiIdx = bin.Index / NRAD;
            phiIdx += refPhiIndex;
            phiIdx %= NPHI;
            fInfo[rIdx + phiIdx * NRAD] = fN;
            for (ii = tShift, jj = 1; ii < fSamplingSize; ++ii, ++jj) {
              fSignalDataStrip[ii] += fScaleFactor * fN * hCathodeI2d[rIdx][phiIdx]->GetBinContent(jj);
            }
            if (fUsePlot) {
              Int_t irr = rIdx;
              Int_t ipp = phiIdx;
              h2dIndex->SetBinContent(irr + 1, ipp + 1, h2dIndex->GetBinContent(irr + 1, ipp + 1) + fN);
              // Int_t rIndex = Int_t(TMath::Log(1. + cMAPFACTOR*(fIonProductionPos.Mod() - cWIRERADIUS))/cUNITLOGDR);
              // Int_t phiIndex = Int_t(fIonProductionPos.Phi()/cUNITPHI);
              Double_t PHI = (ipp + 1) * cUNITPHI;
              Double_t RADIUS = (exp(irr + 1) - 1.) * cUNITLOGDR / cMAPFACTOR + cWIRERADIUS;
              Double_t x = RADIUS * TMath::Cos(PHI);
              Double_t y = RADIUS * TMath::Sin(PHI);
              // Int_t ix = (x + 0.01)/0.0002;
              // Int_t iy = (y + 0.01)/0.0002;
              for (Int_t iv = 0; iv < fN; ++iv)
                hAvaPos->Fill(x, y);
              // hAvaPos->SetBinContent(ix, iy, hAvaPos->GetBinContent(ix, iy) + fNrNphi[ir][ipp]);
            }
          }
        }
      }
    }
    ++fNumofCluster;
  } while (fTrackLen < fMaxPath);

  if (fUseNoise) {
    AddNoise(fNoiseLevel);
  }
  // sw.Stop();
  // cout<<"PndMdtParamDigi::GetRawSignal, cost "<<sw.CpuTime()<<" s"
  //  <<" clusters#"<<fNumofCluster
  //  <<" primary ionization#"<<fNumofTotalIonizaion
  //  <<" total ions#"<<fNumofTotalIons<<endl;
}
std::vector<std::pair<Int_t, Double_t>> PndMdtParamDigi::GetFiredInfo()
{
  std::vector<std::pair<Int_t, Double_t>> fFiredInfo;
  Double_t mTime;
  Double_t mAmplitude;
  GetSignal();

  if (Digitize(mTime, mAmplitude))
    fFiredInfo.push_back(std::pair<Int_t, Double_t>(-1, mTime));

  std::map<Int_t, std::vector<Double_t>>::iterator it = fSignalDataStripM.begin();
  std::map<Int_t, std::vector<Double_t>>::iterator end = fSignalDataStripM.end();
  for (; it != end; ++it) {
    if (Digitize(it->first, mTime, mAmplitude)) {
      fFiredInfo.push_back(std::pair<Int_t, Double_t>(it->first, mTime));
    }
  }
  return fFiredInfo;
}
struct print {
  void operator()(Double_t v) const { cout << v << ", "; }
};
// -------------------------------------------------------------------------
void PndMdtParamDigi::GetSignal(Bool_t useConvolution)
{

  GetRawSignalbyWeightingAvalanche(1.);
  // GetRawSignalbySimAvalanche(1.);
  // convolution with readout circuit
  if (useConvolution) {
    ApplyTransferFunction(&fSignalDataAnode[0]);
    // for_each(fSignalDataAnode.begin(), fSignalDataAnode.end(), print());
    // cout<<endl;

    std::map<Int_t, std::vector<Double_t>>::iterator it = fSignalDataStripM.begin();
    std::map<Int_t, std::vector<Double_t>>::iterator end = fSignalDataStripM.end();
    while (it != end) {
      std::vector<Double_t> &fSignalDataStrip = it->second;
      ApplyTransferFunction(&fSignalDataStrip[0]);
      ++it;
    }
  }
}
void PndMdtParamDigi::ApplyTransferFunction(Double_t *fSignalData, Int_t) // nSize //[R.K.03/2017] unused variable(s)
{
  Double_t dt = 0.01; // microsecond
  Double_t t(0.);
  Double_t fIntegral(0.);
  Double_t dtau(0.);
  Double_t fF(0.);
  Double_t fG(0.);
  Double_t tprime(0.);
  Int_t iBin(1);
  dtau = (fTrF->GetXmax() - fTrF->GetXmin()) / 1.e3;
  Double_t fSigCopy[fSamplingSize];
  if (fTrF) {
    memcpy(fSigCopy, fSignalData, fSamplingSize * sizeof(Double_t));
    // std::vector<Double_t> fSigCopy(fSignalData.begin(),fSignalData.end());
    for (Int_t i = 0; i < fSamplingSize; ++i) {
      t = i * dt; // microsecond
      fIntegral = 0.;
      tprime = fTrF->GetXmin();
      while (tprime < fTrF->GetXmax()) {
        iBin = (Int_t)(tprime / dt);
        fF = (iBin < 0 || iBin > fSamplingSize) ? 0 : fSigCopy[iBin];
        fG = (t - tprime < fTrF->GetXmin()) || (t - tprime > fTrF->GetXmax()) ? 0 : fTrF->Eval((t - tprime)); // convent to microsecond
        fIntegral += fF * fG * dtau;
        tprime += dtau;
      }
      fSignalData[i] = fIntegral;
    }
  }
}
// suppose noise level is 1 micro ampere;
void PndMdtParamDigi::AddNoise(Double_t fNoiseLevel, Int_t) // isAnode //[R.K.03/2017] unused variable(s)
{
  Double_t fSigmaAnode = fNoiseLevel * fNoiseSigmaAnode; // micro ampere
  Double_t fSigmaStrip = fNoiseLevel * fNoiseSigmaStrip; // micro ampere

  std::map<Int_t, std::vector<Double_t>>::iterator it = fSignalDataStripM.begin();
  std::map<Int_t, std::vector<Double_t>>::iterator end = fSignalDataStripM.end();
  while (it != end) {
    std::vector<Double_t> &fSignalDataStrip = it->second;
    for (Int_t i = 0; i < fSamplingSize; ++i) {
      fSignalDataStrip[i] += gRandom->Gaus(0, fSigmaStrip);
    }
    ++it;
  }
  for (Int_t i = 0; i < fSamplingSize; ++i) {
    fSignalDataAnode[i] += gRandom->Gaus(0, fSigmaAnode);
  }
}

Bool_t PndMdtParamDigi::Digitize(Double_t &time, Double_t &amp)
{
  time = -1;
  amp = -9999;
  Double_t fPeak = 0.;
  Bool_t NotFound = kTRUE;
  for (size_t is = 0; is < fSamplingSize; ++is) {
    if (TMath::Abs(fSignalDataAnode[is]) > TMath::Abs(fPeak))
      fPeak = fSignalDataAnode[is];
    if (TMath::Abs(fSignalDataAnode[is]) > 5 * fNoiseSigmaAnode && NotFound) {
      time = is * fSamplingInterval; // nano seconds
      NotFound = kFALSE;
    }
  }
  amp = fPeak;
  return time > 0;
}
Bool_t PndMdtParamDigi::Digitize(Int_t id, Double_t &time, Double_t &amp)
{
  time = -1;
  amp = -9999;
  Double_t fPeak = 0.;
  Bool_t NotFound = kTRUE;
  std::vector<Double_t> &fSignalDataStrip = fSignalDataStripM[id];
  for (size_t is = 0; is < fSamplingSize; ++is) {
    if (TMath::Abs(fSignalDataStrip[is]) > TMath::Abs(fPeak))
      fPeak = fSignalDataStrip[is];
    if (TMath::Abs(fSignalDataStrip[is]) > 5 * fNoiseSigmaStrip && NotFound) {
      time = is * fSamplingInterval; // nano seconds
      NotFound = kFALSE;
    }
  }
  amp = fPeak;
  return time > 0;
}

// index, momentum
// 0 - 50MeV
// 1 - 100MeV
// 2 - 200MeV
// 3 - 300MeV
// 4 - 500MeV
// 5 - 1GeV
// 6 - 2GeV
// 7 - 5GeV
// 8 - 10GeV
Int_t PndMdtParamDigi::GetAmplicationFactor(Int_t, Double_t momentum) const //  particleType //[R.K.03/2017] unused variable(s)
{
  TGraphErrors *gAmp(0);
  if (!gAmp) {
    gAmp = new TGraphErrors(9);
    gAmp->SetPoint(0, 0.05, 5440.5);
    gAmp->SetPointError(0, 0, 2402.1);
    gAmp->SetPoint(1, 0.10, 5683.1);
    gAmp->SetPointError(1, 0, 2661.3);
    gAmp->SetPoint(2, 0.20, 5774.8);
    gAmp->SetPointError(2, 0, 2803.5);
    gAmp->SetPoint(3, 0.30, 5629.1);
    gAmp->SetPointError(3, 0, 2590.4);
    gAmp->SetPoint(4, 0.50, 5708.1);
    gAmp->SetPointError(4, 0, 2734.2);
    gAmp->SetPoint(5, 1.00, 5667.6);
    gAmp->SetPointError(5, 0, 2755.5);
    gAmp->SetPoint(6, 2.00, 5958.2);
    gAmp->SetPointError(7, 0, 2712.1);
    gAmp->SetPoint(7, 5.00, 5643.8);
    gAmp->SetPointError(7, 0, 2389.5);
    gAmp->SetPoint(8, 10.00, 5749.3);
    gAmp->SetPointError(8, 0, 2552.6);
  }
  const Double_t fScaleFactor = 1.0; // simulation calibration to experiments
  Double_t fMeanAmp = gAmp->Eval(momentum);
  Double_t fAmpErr = gAmp->GetErrorY(momentum);
  return (Int_t)fScaleFactor * gRandom->Gaus(fMeanAmp, fAmpErr);
}

//
// ValueErrorType PndMdtParamDigi::NumofPrimaryIonizaion[9] = {
//  ValueErrorType(81.77, 39.38), ValueErrorType(49.40, 11.24), ValueErrorType(44.61, 10.45)
//    , ValueErrorType(42.95, 10.32), ValueErrorType(50.11, 36.60), ValueErrorType(48.45, 11.50)
//    , ValueErrorType(48.77, 10.94), ValueErrorType(56.05, 12.13), ValueErrorType(58.12, 12.93)
//};
void PndMdtParamDigi::GetMPVofPrimaryIonization(Int_t particleType, const TVector3 &fMomentum, ValueErrorType &val) const
{
  Double_t p0 = 9.83401e-01;
  Double_t p1 = 7.42822e-02;
  Double_t p2 = 5.62250e-01;
  Double_t p3 = 2.22743e-01;
  // index: electron 0, muon 1, pion 2, kaon, 3, proton 4;
  Double_t fMass = fRestMass[particleType];
  Double_t betagamma = fMomentum.Mag() / fMass;
  val.first = p0 * exp(p1 * log(1 + betagamma)) + p2 / (pow(betagamma, p3));
  val.second = betagamma < 0.3 ? 0.234 : 0.334;
}
void PndMdtParamDigi::SamplingPosition(TVector2 fDirection, TVector2 &fIonProductionPos)
{
  const Double_t cThetaSigma = 2.67859e-01;
  // sampling theta
  Double_t mTheta = gRandom->Gaus(0., cThetaSigma);
  // sampling radial
  const Double_t mTauR1 = 1.51900e-3;
  const Double_t mTauR2 = 5.53950e-4;
  const Double_t mTauR3 = 2.94876e-4;
  Double_t mRadius(0);
  Double_t prob = gRandom->Rndm();
  if (prob < 2.38210768816632112e-02) {
    mRadius = gRandom->Exp(mTauR1);
  } else if (prob < 4.34694439053706305e-01) {
    mRadius = gRandom->Exp(mTauR2);
  } else {
    mRadius = gRandom->Exp(mTauR3);
  }
  mRadius += cWIRERADIUS;
  Double_t mX = mRadius * cos(mTheta);
  Double_t mY = mRadius * sin(mTheta);
  // unitV.Print();
  TVector2 unitV = fDirection.Unit();

  fIonProductionPos.Set(unitV.X() * mX - unitV.Y() * mY, unitV.Y() * mX + unitV.X() * mY);
  // fIonProductionPos.Print();
}

ClassImp(PndMdtParamDigi)
