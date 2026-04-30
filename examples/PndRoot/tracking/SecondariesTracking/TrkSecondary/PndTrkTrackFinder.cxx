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

////////////////////////////////////////////////////////////
//
// PndTrkTrackFinder
//
// Class for secondary track pattern recognition
//
// authors: Lia Lavezzi - University of Torino (2015)
//
////////////////////////////////////////////////////////////

#include "PndTrkTrackFinder.h"

// stt
#include "PndSttHit.h"
#include "PndSttTube.h"
#include "PndSttMapCreator.h"
// sds
#include "PndSdsHit.h"
// track(cand)
#include "PndTrackCand.h"
#include "PndTrackCandHit.h"
#include "PndTrack.h"

#include "PndTrkConformalHit.h"
#include "PndTrkConformalHitList.h"
#include "PndTrkConformalTransform.h"
#include "PndTrkTools.h"
#include "PndTrkSkewHit.h"
#include "PndTrkFitter.h"
// fairroot
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
// ROOT
#include "TClonesArray.h"
#include "TVector3.h"
#include "TArc.h"
#include "TLine.h"
#include "TMarker.h"
#include "TSpectrum2.h"
#include "TSpectrum.h"
#include "TStopwatch.h"
#include "TH3D.h"
#include "TMinuit.h"
// tracking
#include "PndTrkClusterList.h"
#include "PndTrkTrackList.h"
#include "PndTrkClean.h"
#include "PndTrkNeighboringMap.h"
#include "PndTrkIndivisibleHit.h"

#include <iostream>

using namespace std;

// this is the function used for the fit
//   par: vector with the fit parameters
Double_t fit_distance(float x, float y, Double_t *par)
{
  double value = (par[0] * x - y + par[1]) / TMath::Sqrt(par[0] * par[0] + 1);
  return value;
}

void Chi2Calculation(Int_t &, Double_t *, Double_t &f, Double_t *par, Int_t) // npar gin iflag//[R.K.03/2017] unused variable(s)
{

  TMatrixT<Double_t> *objtofit = (TMatrixT<Double_t> *)gMinuit->GetObjectFit();

  Double_t chi2 = 0;
  Int_t hitcounter = objtofit->GetNrows();

  for (Int_t ihit = 0; ihit < hitcounter; ihit++) {
    double r_reco = fit_distance(objtofit[0][ihit][0], objtofit[0][ihit][1], par);
    double delta = (objtofit[0][ihit][2] - r_reco) / objtofit[0][ihit][3];

    cout << "reco iso " << r_reco << endl;
    cout << "drift " << objtofit[0][ihit][2] << endl;
    cout << "delta " << delta << endl;
    chi2 += delta * delta;
  }
  f = chi2;
  return;
}

void Chi2Calculation2(Int_t &, Double_t *, Double_t &f, Double_t *par, Int_t) // npar gin iflag//[R.K.03/2017] unused variable(s)
{

  TMatrixT<Double_t> *objtofit = (TMatrixT<Double_t> *)gMinuit->GetObjectFit();

  Double_t chi2 = 0;
  Int_t hitcounter = objtofit->GetNrows();

  for (Int_t ihit = 0; ihit < hitcounter; ihit++) {

    double delta = TMath::Sqrt((objtofit[0][ihit][0] - par[0]) * (objtofit[0][ihit][0] - par[0]) + (objtofit[0][ihit][1] - par[1]) * (objtofit[0][ihit][1] - par[1])) - par[2] +
                   par[3] * objtofit[0][ihit][2];
    if (objtofit[0][ihit][2] == 0)
      chi2 += (delta * delta * 12.);
    else
      chi2 += (delta * delta) / (pow(objtofit[0][ihit][3], 2));
  }
  f = chi2;
  return;
}

Bool_t PndTrkTrackFinder::MinuitFit(PndTrkCluster *cluster, double mstart, double qstart, double &fitm, double &fitq)
{

  if (fDisplayOn) {
    display->cd(1);
    Refresh();
  }

  TMinuit minimizer;
  minimizer.SetPrintLevel(-1);

  // set the object to be fitted: ................
  // TMatrixT<Double_t> [x][y][r][err_r]
  TMatrixT<Double_t> fitvect;

  int nofhits = 0;
  for (int ihit = 0; ihit < cluster->GetNofHits(); ihit++) {
    PndTrkHit *hit = cluster->GetHit(ihit);
    if (hit == fRefHit)
      continue;
    if (hit->IsSttSkew())
      continue;
    nofhits++;
  }
  fitvect.ResizeTo(nofhits, 4); // x y r errr

  int counter = 0;
  for (int ihit = 0; ihit < cluster->GetNofHits(); ihit++) {
    PndTrkHit *hit = cluster->GetHit(ihit);
    if (hit == fRefHit)
      continue;
    if (hit->IsSttSkew())
      continue;

    PndTrkConformalHit chit = conform->GetConformalHit(hit);
    PndTrkConformalHit chitstt = conform->GetConformalSttHit(hit);
    double sigma = 1e-5;
    if (hit->IsSttParallel()) {
      sigma = chitstt.GetIsochrone(); // 0.1; // CHECK

      fitvect[counter][0] = chitstt.GetPosition().X();
      fitvect[counter][1] = chitstt.GetPosition().Y();
      fitvect[counter][2] = chitstt.GetIsochrone();
      fitvect[counter][3] = sigma;
    } else {
      fitvect[counter][0] = chit.GetPosition().X();
      fitvect[counter][1] = chit.GetPosition().Y();
      fitvect[counter][2] = 0.;
      fitvect[counter][3] = sigma;
    }
    counter++;

    if (fDisplayOn) {
      display->cd(1);
      TMarker *mrk = new TMarker(hit->GetPosition().X(), hit->GetPosition().Y(), 6);
      mrk->SetMarkerColor(kRed);
      mrk->Draw("SAME");

      display->cd(2);
      TMarker *mrk2 = new TMarker(chit.GetPosition().X(), chit.GetPosition().Y(), 6);
      mrk2->SetMarkerColor(kRed);
      mrk2->Draw("SAME");

      display->Update();
      display->Modified();
    }
  }
  if (nofhits != counter)
    fitvect.ResizeTo(counter, 4); // x y r errr
  //.....................

  minimizer.SetFCN(Chi2Calculation);
  //  minimizer.SetErrorDef(1);  // ???

  minimizer.DefineParameter(0, "m", mstart, 0.1, -3000., 3000.); // ???
  minimizer.DefineParameter(1, "q", qstart, 0.1, -3000., 3000.); // ??? LIMITS ???

  minimizer.SetObjectFit(&fitvect);
  minimizer.SetPrintLevel();

  minimizer.SetMaxIterations(500);
  minimizer.Migrad();

  Double_t results[3], errors[3]; // chisquare,  //[R.K. 01/2017] unused variable
  minimizer.GetParameter(0, results[0], errors[0]);
  minimizer.GetParameter(1, results[1], errors[1]);

  cout << "fitm: " << results[0] << endl;
  cout << "fitq: " << results[1] << endl;

  if (fitm == 0)
    return kFALSE;

  fitm = results[0];
  fitq = results[1];

  //     //  cout << "previous " << xc << " " << yc << " " << R << endl;
  //     FromConformalToRealTrack(fitm, fitq, xc, yc, R);
  //     // cout << "now " << xc << " " << yc << " " << R << endl;

  //     if(fDisplayOn) {
  //       display->cd(2);
  //       cout << "wanna see the line?" << endl;
  //       TLine *line = new TLine(-10.07, fitq + fitm * (-10.07), 10.07, fitq + fitm * (10.07));
  //       line->SetLineColor(2);
  //       line->Draw("SAME");
  //       char goOnChar;
  //       display->Update();
  //       display->Modified();
  //       cin >> goOnChar;
  //     }

  return kTRUE;
}

Bool_t PndTrkTrackFinder::MinuitFit2(PndTrkCluster *cluster, double xstart, double ystart, double rstart, double &xc, double &yc, double &R, double &sign)
{

  if (fDisplayOn) {
    display->cd(1);
    Refresh();
  }

  TMinuit minimizer;
  minimizer.SetPrintLevel(-1);

  // set the object to be fitted: ................
  // TMatrixT<Double_t> [x][y][r][err_r]
  TMatrixT<Double_t> fitvect;

  int nofhits = 0;
  for (int ihit = 0; ihit < cluster->GetNofHits(); ihit++) {
    PndTrkHit *hit = cluster->GetHit(ihit);
    if (hit == fRefHit)
      continue;
    if (hit->IsSttSkew())
      continue;
    nofhits++;
  }
  fitvect.ResizeTo(nofhits, 4); // x y r errr

  int counter = 0;
  for (int ihit = 0; ihit < cluster->GetNofHits(); ihit++) {
    PndTrkHit *hit = cluster->GetHit(ihit);
    if (hit == fRefHit)
      continue;
    if (hit->IsSttSkew())
      continue;

    double sigma = 1e-5;
    if (hit->IsSttParallel()) {
      sigma = hit->GetIsochrone(); // 0.1; // CHECK

      fitvect[counter][0] = hit->GetPosition().X();
      fitvect[counter][1] = hit->GetPosition().Y();
      fitvect[counter][2] = hit->GetIsochrone();
      fitvect[counter][3] = sigma;
    } else {
      fitvect[counter][0] = hit->GetPosition().X();
      fitvect[counter][1] = hit->GetPosition().Y();
      fitvect[counter][2] = 0.;
      fitvect[counter][3] = sigma;
    }
    counter++;

    if (fDisplayOn) {
      display->cd(1);
      TMarker *mrk = new TMarker(hit->GetPosition().X(), hit->GetPosition().Y(), 6);
      mrk->SetMarkerColor(kRed);
      mrk->Draw("SAME");

      display->Update();
      display->Modified();
    }
  }
  if (nofhits != counter)
    fitvect.ResizeTo(counter, 4); // x y r errr
  //.....................

  minimizer.SetFCN(Chi2Calculation2);
  //  minimizer.SetErrorDef(1);  // ???

  minimizer.DefineParameter(0, "x", xstart, 0.1, -3000., 3000.); // ???
  minimizer.DefineParameter(1, "y", ystart, 0.1, -3000., 3000.); // ??? LIMITS ???
  minimizer.DefineParameter(2, "R", rstart, 0.1, 0., 3000.);     // ??? LIMITS ???
  minimizer.DefineParameter(3, "sign", 0, 1, -1, 1);             // ??? LIMITS ???

  minimizer.SetObjectFit(&fitvect);
  minimizer.SetPrintLevel();

  minimizer.SetMaxIterations(500);
  minimizer.Migrad();

  Double_t results[4], errors[4]; // chisquare,  //[R.K. 01/2017] unused variable
  minimizer.GetParameter(0, results[0], errors[0]);
  minimizer.GetParameter(1, results[1], errors[1]);
  minimizer.GetParameter(2, results[2], errors[2]);
  minimizer.GetParameter(3, results[3], errors[3]);

  cout << "xc: " << results[0] << endl;
  cout << "yc: " << results[1] << endl;
  cout << "R: " << results[2] << endl;
  cout << "sign: " << results[3] << endl;

  //  if( == 0) return kFALSE;

  xc = results[0];
  yc = results[1];
  R = results[2];
  sign = results[3];

  //     //  cout << "previous " << xc << " " << yc << " " << R << endl;
  //     FromConformalToRealTrack(fitm, fitq, xc, yc, R);
  //     // cout << "now " << xc << " " << yc << " " << R << endl;

  //     if(fDisplayOn) {
  //       display->cd(2);
  //       cout << "wanna see the line?" << endl;
  //       TLine *line = new TLine(-10.07, fitq + fitm * (-10.07), 10.07, fitq + fitm * (10.07));
  //       line->SetLineColor(2);
  //       line->Draw("SAME");
  //       char goOnChar;
  //       display->Update();
  //       display->Modified();
  //       cin >> goOnChar;
  //     }

  return kTRUE;
}

// -----   Default constructor   -------------------------------------------
PndTrkTrackFinder::PndTrkTrackFinder()
  : FairTask("secondary track finder", 0), fDisplayOn(kFALSE), fPersistence(kTRUE), fUseMVDPix(kTRUE), fUseMVDStr(kTRUE), fUseSTT(kTRUE), fUseSCIT(kTRUE), fUseGEM(kTRUE),
    fSecondary(kFALSE), fInitDone(kFALSE), fMvdPix_RealDistLimit(1000), fMvdStr_RealDistLimit(1000), fStt_RealDistLimit(1000), fMvdPix_ConfDistLimit(1000),
    fMvdStr_ConfDistLimit(1000), fStt_ConfDistLimit(1000), fUmin(-0.07), fUmax(0.07), fVmin(-0.07), fVmax(0.07), fRmin(-1.5), fRmax(1.5), fThetamin(0), fThetamax(180),
    fRefHit(nullptr), fDelPrim(kFALSE), fNofPrimaries(0)
{
  sprintf(fSttBranch, "STTHit");
  sprintf(fMvdPixelBranch, "MVDHitsPixel");
  sprintf(fMvdStripBranch, "MVDHitsStrip");
  sprintf(fSciTBranch, "SciTHit");
  sprintf(fGemBranch, "GEMHit");
  PndGeoHandling::Instance();
}

PndTrkTrackFinder::PndTrkTrackFinder(int verbose)
  : FairTask("secondary track finder", verbose), fDisplayOn(kFALSE), fPersistence(kTRUE), fUseMVDPix(kTRUE), fUseMVDStr(kTRUE), fUseSTT(kTRUE), fUseSCIT(kTRUE), fUseGEM(kTRUE),
    fSecondary(kFALSE), fInitDone(kFALSE), fMvdPix_RealDistLimit(1000), fMvdStr_RealDistLimit(1000), fStt_RealDistLimit(1000), fMvdPix_ConfDistLimit(1000),
    fMvdStr_ConfDistLimit(1000), fStt_ConfDistLimit(1000), fUmin(-0.07), fUmax(0.07), fVmin(-0.07), fVmax(0.07), fRmin(-1.5), fRmax(1.5), fThetamin(0), fThetamax(180),
    fRefHit(nullptr), fDelPrim(kFALSE), fNofPrimaries(0)
{
  sprintf(fSttBranch, "STTHit");
  sprintf(fMvdPixelBranch, "MVDHitsPixel");
  sprintf(fMvdStripBranch, "MVDHitsStrip");
  sprintf(fSciTBranch, "SciTHit");
  sprintf(fGemBranch, "GEMHit");
  PndGeoHandling::Instance();
}

// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndTrkTrackFinder::~PndTrkTrackFinder()
{

  delete fSttPointArray;
  delete fSttHitArray;
  delete fMvdPixelHitArray;
  delete fMvdStripHitArray;
  delete fSciTHitArray;
  delete fGemHitArray;
  delete fTrackArray;
  delete fTrkTrackArray;
  delete fTrackCandArray;
  delete fTubeArray;

  delete fSttParameters;
  delete fMapper;

  delete stthitlist;
  delete mvdpixhitlist;
  delete mvdstrhitlist;
  delete scithitlist;
  delete gemhitlist;

  delete legendre;

  delete conform;
  delete fConformalHitList;
  delete tools;

  delete fRefHit;

  // delete fFoundPeaks;

  delete fTimer;

  delete fFitter;
  delete fHitMap;

  delete hxy;
  delete hxz;
  delete hzphi;
  delete display;
  delete huv;
}
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
InitStatus PndTrkTrackFinder::Init()
{

  fEventCounter = 0;

  fMvdPix_RealDistLimit = 0.5;    // CHECK limits
  fMvdStr_RealDistLimit = 0.5;    // CHECK limits
  fStt_RealDistLimit = 1.5 * 0.5; // CHECK limits
  fMvdPix_ConfDistLimit = 0.003;  // CHECK limits
  fMvdStr_ConfDistLimit = 0.003;  // CHECK limits
  fStt_ConfDistLimit = 0.001;     // CHECK limits
  if (fSecondary) {
    fMvdPix_RealDistLimit = 1.;    // CHECK limits
    fMvdStr_RealDistLimit = 1.;    // CHECK limits
    fMvdPix_ConfDistLimit = 0.007; // CHECK limits
    fMvdStr_ConfDistLimit = 0.007; // CHECK limits
  }

  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    cout << "-E- PndTrkTrackFinder::Init: "
         << "RootManager not instantiated, return!" << endl;
    return kFATAL;
  }

  // -- HITS -------------------------------------------------
  //
  // STT
  fSttHitArray = (TClonesArray *)ioman->GetObject(fSttBranch);
  if (!fSttHitArray) {
    cout << "-W- PndTrkTrackFinder::Init: "
         << "No STTHit array, return!" << endl;
    return kERROR;
  }
  //
  // MVD PIXEL
  fMvdPixelHitArray = (TClonesArray *)ioman->GetObject(fMvdPixelBranch);
  if (!fMvdPixelHitArray) {
    std::cout << "-W- PndTrkTrackFinder::Init: "
              << "No MVD Pixel hitArray, return!" << std::endl;
    return kERROR;
  }
  //
  // MVD STRIP
  fMvdStripHitArray = (TClonesArray *)ioman->GetObject(fMvdStripBranch);
  if (!fMvdStripHitArray) {
    std::cout << "-W- PndTrkTrackFinder::Init: "
              << "No MVD Strip hitArray, return!" << std::endl;
    return kERROR;
  }
  //
  // SciT
  fSciTHitArray = (TClonesArray *)ioman->GetObject(fSciTBranch);
  if (!fSciTHitArray) {
    std::cout << "-W- PndTrkTrackFinder::Init: "
              << "No SciT hitArray, return!" << std::endl;
    return kERROR;
  }
  //
  // GEM
  fGemHitArray = (TClonesArray *)ioman->GetObject(fGemBranch);
  if (!fGemHitArray) {
    std::cout << "-W- PndTrkTrackFinder::Init: "
              << "No GEM hitArray, return!" << std::endl;
    return kERROR;
  }

  if (fDelPrim == kTRUE)
    fPrimaryTrackArray = (TClonesArray *)ioman->GetObject("SttMvdGemTrack");

  fTrackArray = new TClonesArray("PndTrack");
  fTrkTrackArray = new TClonesArray("PndTrkTrack");
  fTrackCandArray = new TClonesArray("PndTrackCand");
  ioman->Register("Track", "pr", fTrackArray, fPersistence);         // CHECK
  ioman->Register("TrkTrack", "pr", fTrkTrackArray, fPersistence);   // CHECK
  ioman->Register("TrackCand", "pr", fTrackCandArray, fPersistence); // CHECK

  // ----------------------------------------   maps of STT tubes
  fMapper = new PndSttMapCreator(fSttParameters);
  fTubeArray = fMapper->FillTubeArray();
  // ----------------------------------------------------  end map

  if (fDisplayOn) {
    display = new TCanvas("display", "display", 0, 0, 800, 800); // CHECK
    display->Divide(2, 2);
  }

  legendre = new PndTrkLegendreTransform();

  conform = new PndTrkConformalTransform();

  tools = new PndTrkTools();
  fFitter = new PndTrkFitter(fVerbose);
  fHitMap = new PndTrkNeighboringMap(fTubeArray);
  fTimer = new TStopwatch();

  fCluster = new PndTrkCluster();
  fFinalCluster = new PndTrkCluster();
  fIndivCluster = new PndTrkCluster();
  fSkewCluster = new PndTrkCluster();
  fFinalSkewCluster = new PndTrkCluster();
  fTrackList = new PndTrkTrackList();
  fIndivisibleHitList = new PndTrkCluster();

  fConformalHitList = new PndTrkConformalHitList();

  // fLineHisto = new TH2F("fLineHisto", "hl", 720, -360, 360, 8000, -400, 400);
  fLineHisto = new TH2F("fLineHisto", "hl", 360, -360, 360, 500, -400, 400);

  stthitlist = new PndTrkSttHitList(fTubeArray);
  mvdpixhitlist = new PndTrkSdsHitList(MVDPIXEL);
  mvdstrhitlist = new PndTrkSdsHitList(MVDSTRIP);
  scithitlist = new PndTrkSciTHitList();
  gemhitlist = new PndTrkGemHitList();

  hxy = nullptr;
  huv = nullptr;
  hzphi = nullptr;

  fCombiFinder = new PndTrkGemCombinatorial(fGemHitArray, fVerbose);

  return kSUCCESS;
}

// -------------------------------------------------------------------------

void PndTrkTrackFinder::SetParContainers()
{
  FairRuntimeDb *rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fSttParameters = (PndGeoSttPar *)rtdb->getContainer("PndGeoSttPar");
}

// -------------------------------------------------------------------------

void PndTrkTrackFinder::Initialize()
{

  //   stthitlist = new PndTrkSttHitList(fTubeArray);
  //   mvdpixhitlist = new PndTrkSdsHitList(MVDPIXEL);
  //   mvdstrhitlist = new PndTrkSdsHitList(MVDSTRIP);
  //   scithitlist = new PndTrkSciTHitList();
  //   gemhitlist = new PndTrkGemHitList();

  //   stthitlist->Clear();
  //   mvdpixhitlist->Clear();
  //   mvdstrhitlist->Clear();
  //  gemhitlist->Clear();
  //   scithitlist->Clear();

  std::map<int, std::vector<int>> det_to_hitids;
  if (fDelPrim) {
    fNofPrimaries = RecreateHitArrays(det_to_hitids);
    cout << fNofPrimaries << " --->" << det_to_hitids.size() << endl;
  }

  if (fUseSTT) {
    stthitlist->AddTCA(FairRootManager::Instance()->GetBranchId(fSttBranch), fSttHitArray);
    std::map<int, bool> primaries = PrimaryCheck(FairRootManager::Instance()->GetBranchId(fSttBranch), det_to_hitids);
    for (size_t ihit = 0; ihit < primaries.size(); ihit++) {
      if (primaries[ihit] == true)
        continue;
      PndTrkHit *hit = stthitlist->GetHitByID(ihit);
      stthitlist->RemoveHit(hit);
    }
    stthitlist->Instanciate();
  }

  if (fUseMVDPix) {
    mvdpixhitlist->AddTCA(FairRootManager::Instance()->GetBranchId(fMvdPixelBranch), fMvdPixelHitArray);
    std::map<int, bool> primaries = PrimaryCheck(FairRootManager::Instance()->GetBranchId(fMvdPixelBranch), det_to_hitids);
    for (size_t ihit = 0; ihit < primaries.size(); ihit++) {
      if (primaries[ihit] == true)
        continue;
      PndTrkHit *hit = mvdpixhitlist->GetHitByID(ihit);
      mvdpixhitlist->RemoveHit(hit);
    }
    mvdpixhitlist->InstanciatePixel();
  }

  if (fUseMVDStr) {
    mvdstrhitlist->AddTCA(FairRootManager::Instance()->GetBranchId(fMvdStripBranch), fMvdStripHitArray);
    std::map<int, bool> primaries = PrimaryCheck(FairRootManager::Instance()->GetBranchId(fMvdStripBranch), det_to_hitids);
    for (size_t ihit = 0; ihit < primaries.size(); ihit++) {
      if (primaries[ihit] == true)
        continue;
      PndTrkHit *hit = mvdstrhitlist->GetHitByID(ihit);
      mvdstrhitlist->RemoveHit(hit);
    }
    mvdstrhitlist->InstanciateStrip();
  }

  if (fUseSCIT) {
    scithitlist->AddTCA(FairRootManager::Instance()->GetBranchId(fSciTBranch), fSciTHitArray);
    //     std::map< int, bool > primaries = PrimaryCheck(FairRootManager::Instance()->GetBranchId(fSciTBranch), det_to_hitids);
    //     for(int ihit = 0; ihit < primaries.size(); ihit++) {
    //       if(primaries[ihit] == true) continue;
    //       PndTrkHit *hit = scithitlist->GetHitByID(ihit);
    //       scithitlist->RemoveHit(hit);
    //     }
    scithitlist->Instanciate();
  }

  if (fUseGEM) {
    // gemhitlist->AddTCA(FairRootManager::Instance()->GetBranchId(fGemBranch), fGemHitArray);
    std::map<int, bool> hitidTousability = fCombiFinder->CombinatorialSuppression();
    gemhitlist->AddNonCombiHits(FairRootManager::Instance()->GetBranchId(fGemBranch), fGemHitArray, hitidTousability);

    std::map<int, bool> primaries = PrimaryCheck(FairRootManager::Instance()->GetBranchId(fGemBranch), det_to_hitids);
    for (size_t ihit = 0; ihit < primaries.size(); ihit++) {
      if (primaries[ihit] == true)
        continue;
      PndTrkHit *hit = gemhitlist->GetHitByID(ihit);
      if (hit)
        gemhitlist->RemoveHit(hit);
    }
    gemhitlist->Instanciate();
  }

  fConformalHitList->Clear();
  fFoundPeaks.clear();

  fInitDone = kTRUE;
  //  stthitlist->PrintSectors();

  //   cout << "number of stt    hits " << stthitlist->GetNofHits() << endl;
  //   cout << "number of mvdpix hits " << mvdpixhitlist->GetNofHits()  << endl;
  //   cout << "number of mvdstr hits " << mvdstrhitlist->GetNofHits()  << endl;
  //   cout << "number of scit   hits " << scithitlist->GetNofHits()  << endl;
  //   cout << "number of gem    hits " << gemhitlist->GetNofHits()  << endl;
}

void PndTrkTrackFinder::Exec(Option_t *)
{
  // ############## I N I T I A L I Z A T I O N S ##############
  fTrackArray->Delete();
  fTrkTrackArray->Delete();
  fTrackCandArray->Delete();
  if (fVerbose > 0)
    cout << "PndTrkTrackFinder:: *********************** " << fEventCounter << " ***********************" << endl;
  // fDisplayOn = kTRUE;
  fEventCounter++;
  // initialize -----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~
  Initialize();
  if (fVerbose > 1) {
    cout << "number of stt    hits " << fSttHitArray->GetEntriesFast() << endl;
    cout << "number of mvdpix hits " << fMvdPixelHitArray->GetEntriesFast() << endl;
    cout << "number of mvdstr hits " << fMvdStripHitArray->GetEntriesFast() << endl;
    cout << "number of scit   hits " << fSciTHitArray->GetEntriesFast() << endl;
    cout << "number of gem    hits " << fGemHitArray->GetEntriesFast() << endl;
  }

  if (fSttHitArray->GetEntriesFast() > 200) { // CHECK
    fEventCounter++;
    // cout << "STT hits " << fSttHitArray->GetEntriesFast() << endl;
    Reset();
    return;
  }

  // initialize display -----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~-
  if (fDisplayOn) {
    Refresh();
    char goOnChar;
    display->Update();
    display->Modified();
    cout << " STARTING" << endl;
    cin >> goOnChar;
    display->Update();
    display->Modified();
  }

  // initialize hit map -----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~-
  fHitMap->Clear();
  FillHitMap();

  // ##########################################################

  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // L      OOO  N   N  GGG
  // L     O   O NN  N G
  // L     O   O N N N G  GG
  // L     O   O N  NN G   G
  // LLLLL  OOO  N   N  GGG

  //  fDisplayOn = kFALSE;
  PndTrkHit *stthit = nullptr;
  TObjArray indiv;
  // calculate the indivisible parallel hits and
  // their center of mass
  //  std::vector< PndTrkIndivisibleHit > listofindhits;
  double numx, numy, den;
  for (int ihit = 0; ihit < stthitlist->GetNofHits(); ihit++) {
    stthit = stthitlist->GetHit(ihit);
    if (stthit->IsSttSkew())
      continue;
    numx = stthit->GetPosition().X() * stthit->GetIsochrone();
    numy = stthit->GetPosition().Y() * stthit->GetIsochrone();
    den = stthit->GetIsochrone();
    indiv = fHitMap->GetIndivisiblesToHit(stthit);
    //    cout << "--------------------------- indiv to stthit " << ihit << " " << indiv.GetEntriesFast() << " " << stthit->GetTubeID() << endl;
    //     cout <<  stthit->GetPosition().X() << " "  << stthit->GetPosition().Y() << endl;
    int nind = indiv.GetEntriesFast();
    if (nind == 0)
      continue;

    TArrayI indhitids(nind + 1);
    indhitids[0] = stthit->GetHitID();
    int counter = 1;
    for (int jhit = 0; jhit < nind; jhit++) {
      PndTrkHit *stthit2 = (PndTrkHit *)indiv.At(jhit);
      if (stthit2->IsSttSkew())
        continue;
      numx += stthit2->GetPosition().X() * stthit2->GetIsochrone();
      numy += stthit2->GetPosition().Y() * stthit2->GetIsochrone();
      den += stthit2->GetIsochrone();
      indhitids[counter] = stthit2->GetHitID();
      //      cout << counter << " " << stthit2->GetPosition().X() << " " << stthit2->GetPosition().Y() << " " << stthit2->GetIsochrone() << " tube " << stthit2->GetTubeID() <<
      //      endl;
      counter++;
    }
    numx /= den;
    numy /= den;

    // create combined hit
    TVector3 indpos(numx, numy, 0.);
    //    cout << fIndivisibleHitList->GetNofHits() << " num.den " << numx << " " << numy << " " << den << endl;
    PndTrkIndivisibleHit indhit(indhitids, indpos);

    fIndivisibleHitList->AddHit(&indhit);
  }

  //  cout << "nof indivisibles " << fIndivisibleHitList->GetNofHits() << endl;
  if (fDisplayOn) {
    for (int ihit = 0; ihit < fIndivisibleHitList->GetNofHits(); ihit++) {
      PndTrkIndivisibleHit *indhit = (PndTrkIndivisibleHit *)fIndivisibleHitList->GetHit(ihit);
      //  cout << "indhit " << indhit->GetPosition().X() << " " << indhit->GetPosition().Y() << " " << indhit->GetSector() << endl;
      indhit->Draw(kMagenta);
    }
    display->Update();
    display->Modified();
    char goOnChar;
    cin >> goOnChar;
  }

  // +++++++++++++++++++++++++++++

  std::map<int, std::vector<int>> maplay2hits;
  PndTrkHit *hit = nullptr;
  for (int ihit = 0; ihit < stthitlist->GetNofHits(); ihit++) {
    hit = stthitlist->GetHit(ihit);
    PndSttTube *tube = (PndSttTube *)fTubeArray->At(hit->GetTubeID());
    int layerID = tube->GetLayerID();
    std::map<int, std::vector<int>>::iterator it = maplay2hits.begin();
    it = maplay2hits.find(layerID);
    if (it == maplay2hits.end()) {
      std::vector<int> hits;
      hits.push_back(ihit);
      maplay2hits[layerID] = hits;
      //   maplay2hits.insert(std::pair<int, std::vector<int> >(layerID, hits));
      std::vector<int> hits2 = maplay2hits[layerID];
    } else {
      std::vector<int> hits = maplay2hits[layerID];
      hits.push_back(ihit);
      maplay2hits[layerID] = hits;
    }
  }

  for (size_t ilay = 0; ilay < maplay2hits.size(); ilay++) {
    std::vector<int> hits = maplay2hits[ilay];
    //    cout << "layer " << ilay << " has nof hits " << hits.size() << endl;
  }

  std::vector<std::vector<int>> trackcandidates;
  // first inner - last inner layers ------------------------------------------------------------
  // couples
  // CUTS:
  // xy distance < 10 cm
  // cosalpha < 0.94

  std::vector<int> hits0 = maplay2hits[0];
  for (size_t ihit = 0; ihit < hits0.size(); ihit++) {
    std::vector<int> hits7 = maplay2hits[7];
    for (size_t jhit = 0; jhit < hits7.size(); jhit++) {
      // cut on distance
      PndTrkHit *hiti = stthitlist->GetHit(hits0[ihit]);
      PndTrkHit *hitj = stthitlist->GetHit(hits7[jhit]);
      double distance = hiti->GetXYDistance(hitj);
      // cout << hits0[ihit] << " and " << hits7[jhit];
      if (distance > 10) {
        // cout << " too distant " << distance << endl;
        continue;
      }

      // cut on angle
      TVector2 hitipos = hiti->GetPosition().XYvector();
      TVector2 hitjpos = hitj->GetPosition().XYvector();
      double cosalpha = hitipos * hitjpos / (hitipos.Mod() * hitjpos.Mod());
      if (cosalpha < 0.94) {
        // cout << " have bad cosine " << cosalpha << endl;
        continue;
      }

      std::vector<int> couple;
      couple.push_back(hits0[ihit]);
      couple.push_back(hits7[jhit]);
      trackcandidates.push_back(couple);
    }
  }

  //  cout << "nof couples " << trackcandidates.size() << endl;
  //   for(int i = 0; i < trackcandidates.size(); i++) {
  //     std::vector< int > driplet = trackcandidates[i];
  //     PndTrkHit *hit0 = stthitlist->GetHit(driplet[0]);
  //     PndTrkHit *hit1 = stthitlist->GetHit(driplet[1]);
  //     TMarker *mrk0 = new TMarker(hit0->GetPosition().X(), hit0->GetPosition().Y(), 21);
  //     mrk0->Draw("SAME");
  //     TMarker *mrk1 = new TMarker(hit1->GetPosition().X(), hit1->GetPosition().Y(), 21);
  //     mrk1->Draw("SAME");
  //     char *goOnChar;

  //     display->Update();
  //     display->Modified();

  //       cin >> goOnChar;
  //   }

  //  cout << endl;
  // triplets
  std::vector<std::vector<int>> trackcandidates2;
  for (size_t ipair = 0; ipair < trackcandidates.size(); ipair++) {
    std::vector<int> couple = trackcandidates[ipair];

    std::vector<int> hits16 = maplay2hits[16];
    for (size_t jhit = 0; jhit < hits16.size(); jhit++) {
      PndTrkHit *hit0 = stthitlist->GetHit(couple[0]);
      PndTrkHit *hit1 = stthitlist->GetHit(couple[1]);

      PndTrkHit *hit16 = stthitlist->GetHit(hits16[jhit]);
      double distance = hit1->GetXYDistance(hit16);
      // cout << couple[0] << ", " << couple[1] <<  " and " << hits16[jhit];
      if (distance > 15) {
        // cout << " too distant " << distance << endl;
        continue;
      }

      // cut on angle
      TVector2 hit1_0pos = hit1->GetPosition().XYvector() - hit0->GetPosition().XYvector();
      TVector2 hit1_16pos = hit16->GetPosition().XYvector() - hit1->GetPosition().XYvector();
      double cosalpha = hit1_0pos * hit1_16pos / (hit1_0pos.Mod() * hit1_16pos.Mod());
      //      cout << "cosalpha " << cosalpha << endl;
      if (cosalpha < 0.900) {
        // cout << " have bad cosine " << cosalpha << endl;
        continue;
      }

      //      cout << " are fine! " << cosalpha << endl;

      std::vector<int> triplet;
      triplet.push_back(couple[0]);
      triplet.push_back(couple[1]);
      triplet.push_back(hits16[jhit]);

      trackcandidates2.push_back(triplet);
    }
  }
  //  cout << "nof triplets " << trackcandidates2.size() << endl;
  //   for(int i = 0; i < trackcandidates2.size(); i++) {
  //     std::vector< int > driplet = trackcandidates2[i];
  //     PndTrkHit *hit0 = stthitlist->GetHit(driplet[0]);
  //     PndTrkHit *hit1 = stthitlist->GetHit(driplet[1]);
  //     PndTrkHit *hit2 = stthitlist->GetHit(driplet[2]);

  //     TMarker *mrk0 = new TMarker(hit0->GetPosition().X(), hit0->GetPosition().Y(), 21);
  //     mrk0->Draw("SAME");
  //     TMarker *mrk1 = new TMarker(hit1->GetPosition().X(), hit1->GetPosition().Y(), 21);
  //     mrk1->Draw("SAME");
  //     TMarker *mrk2 = new TMarker(hit2->GetPosition().X(), hit2->GetPosition().Y(), 21);
  //     mrk2->Draw("SAME");

  //     char goOnChar;

  //     display->Update();
  //     display->Modified();

  //     cin >> goOnChar;
  //   }

  // fourth hit
  std::vector<std::vector<int>> trackcandidates3;
  for (size_t iqua = 0; iqua < trackcandidates2.size(); iqua++) {
    std::vector<int> triplet = trackcandidates2[iqua];

    // PndTrkHit *hit0 = stthitlist->GetHit(triplet[0]); //[R.K. 03/2017] unused variable
    PndTrkHit *hit1 = stthitlist->GetHit(triplet[1]);
    PndTrkHit *hit2 = stthitlist->GetHit(triplet[2]);
    //  cout << "TRIPLET: " << triplet[0] << ", " << triplet[1] <<  " and " << triplet[2] << endl;

    std::vector<int> hits20 = maplay2hits[20];
    for (size_t jhit = 0; jhit < hits20.size(); jhit++) {
      PndTrkHit *hit20 = stthitlist->GetHit(hits20[jhit]);
      double distance = hit2->GetXYDistance(hit20);
      // cout << triplet[0] << ", " << triplet[1] <<  ", " << triplet[2] << " and " << hits20[jhit];
      if (distance > 15) {
        // cout << " too distant " << distance << endl;
        continue;
      }

      // cut on angle
      TVector2 hit2_1pos = hit2->GetPosition().XYvector() - hit1->GetPosition().XYvector();
      TVector2 hit2_20pos = hit20->GetPosition().XYvector() - hit1->GetPosition().XYvector();
      double cosalpha = hit2_1pos * hit2_20pos / (hit2_1pos.Mod() * hit2_20pos.Mod());
      if (cosalpha < 0.94) {
        // cout << " have bad cosine " << cosalpha << endl;
        continue;
      }

      //      cout << " are fine! " << cosalpha << endl;

      std::vector<int> quadriplet;
      quadriplet.push_back(triplet[0]);
      quadriplet.push_back(triplet[1]);
      quadriplet.push_back(triplet[2]);
      quadriplet.push_back(hits20[jhit]);

      trackcandidates3.push_back(quadriplet);
    }
  }
  //  cout << "nof quadriplets " << trackcandidates3.size() << endl;
  //   for(int i = 0; i < trackcandidates3.size(); i++) {
  //     std::vector< int > driplet = trackcandidates3[i];
  //     PndTrkHit *hit0 = stthitlist->GetHit(driplet[0]);
  //     PndTrkHit *hit1 = stthitlist->GetHit(driplet[1]);
  //     PndTrkHit *hit2 = stthitlist->GetHit(driplet[2]);
  //     PndTrkHit *hit3 = stthitlist->GetHit(driplet[3]);

  //     TMarker *mrk0 = new TMarker(hit0->GetPosition().X(), hit0->GetPosition().Y(), 21);
  //     mrk0->Draw("SAME");
  //     TMarker *mrk1 = new TMarker(hit1->GetPosition().X(), hit1->GetPosition().Y(), 21);
  //     mrk1->Draw("SAME");
  //     TMarker *mrk2 = new TMarker(hit2->GetPosition().X(), hit2->GetPosition().Y(), 21);
  //     mrk2->Draw("SAME");
  //     TMarker *mrk3 = new TMarker(hit3->GetPosition().X(), hit3->GetPosition().Y(), 21);
  //     mrk3->Draw("SAME");

  //     char goOnChar;

  //     display->Update();
  //     display->Modified();

  //     cin >> goOnChar;
  //   }

  // circle through three points
  // find the tracks
  std::vector<std::vector<double>> tracks; // x0, y0, R
  std::vector<std::vector<int>> trackcandidates4;
  for (size_t iqua = 0; iqua < trackcandidates3.size(); iqua++) {
    std::vector<int> quadriplet = trackcandidates3[iqua];
    PndTrkHit *hit0 = stthitlist->GetHit(quadriplet[0]);
    PndTrkHit *hit1 = stthitlist->GetHit(quadriplet[1]);
    PndTrkHit *hit2 = stthitlist->GetHit(quadriplet[2]);
    PndTrkHit *hit3 = stthitlist->GetHit(quadriplet[3]);

    double x01, y01, rad1;
    CircleBy3Points(hit0, hit1, hit2, x01, y01, rad1);
    double x02, y02, rad2;
    CircleBy3Points(hit0, hit1, hit3, x02, y02, rad2);
    double x03, y03, rad3;
    CircleBy3Points(hit3, hit1, hit2, x03, y03, rad3);

    double xm = 0, ym = 0, rm = 0;
    int positive[2] = {0, 0}, negative[2] = {0, 0};
    if (x01 > 0)
      positive[0]++;
    else
      negative[0]++;
    if (x02 > 0)
      positive[0]++;
    else
      negative[0]++;
    if (x03 > 0)
      positive[0]++;
    else
      negative[0]++;

    if (y01 > 0)
      positive[1]++;
    else
      negative[1]++;
    if (y02 > 0)
      positive[1]++;
    else
      negative[1]++;
    if (y03 > 0)
      positive[1]++;
    else
      negative[1]++;

    bool posit[2] = {false, false};
    positive[0] > negative[0] ? posit[0] = true : posit[0] = false;
    positive[1] > negative[1] ? posit[1] = true : posit[1] = false;

    int count = 0;
    if (posit[0] == true && x01 > 0) {
      if (posit[1] == true && y01 > 0) {
        xm += x01;
        ym += y01;
        rm += rad1;
        count++;
      } else if (posit[1] == false && y01 < 0) {
        xm += x01;
        ym += y01;
        rm += rad1;
        count++;
      }
    } else if (posit[0] == false && x01 < 0) {
      if (posit[1] == true && y01 > 0) {
        xm += x01;
        ym += y01;
        rm += rad1;
        count++;
      } else if (posit[1] == false && y01 < 0) {
        xm += x01;
        ym += y01;
        rm += rad1;
        count++;
      }
    }

    if (posit[0] == true && x02 > 0) {
      if (posit[1] == true && y02 > 0) {
        xm += x02;
        ym += y02;
        rm += rad2;
        count++;
      } else if (posit[1] == false && y02 < 0) {
        xm += x02;
        ym += y02;
        rm += rad2;
        count++;
      }
    } else if (posit[0] == false && x02 < 0) {
      if (posit[1] == true && y02 > 0) {
        xm += x02;
        ym += y02;
        rm += rad2;
        count++;
      } else if (posit[1] == false && y02 < 0) {
        xm += x02;
        ym += y02;
        rm += rad2;
        count++;
      }
    }

    if (posit[0] == true && x03 > 0) {
      if (posit[1] == true && y03 > 0) {
        xm += x03;
        ym += y03;
        rm += rad3;
        count++;
      } else if (posit[1] == false && y03 < 0) {
        xm += x03;
        ym += y03;
        rm += rad3;
        count++;
      }
    } else if (posit[0] == false && x03 < 0) {
      if (posit[1] == true && y03 > 0) {
        xm += x03;
        ym += y03;
        rm += rad3;
        count++;
      } else if (posit[1] == false && y03 < 0) {
        xm += x03;
        ym += y03;
        rm += rad3;
        count++;
      }
    }

    xm /= count;
    ym /= count;
    rm /= count;

    //     cout << "pos.neg " << positive << " " << negative << endl;

    double distance0 = fabs(TMath::Sqrt((hit0->GetPosition().X() - xm) * (hit0->GetPosition().X() - xm) + (hit0->GetPosition().Y() - ym) * (hit0->GetPosition().Y() - ym)) - rm);
    double distance1 = fabs(TMath::Sqrt((hit1->GetPosition().X() - xm) * (hit1->GetPosition().X() - xm) + (hit1->GetPosition().Y() - ym) * (hit1->GetPosition().Y() - ym)) - rm);
    double distance2 = fabs(TMath::Sqrt((hit2->GetPosition().X() - xm) * (hit2->GetPosition().X() - xm) + (hit2->GetPosition().Y() - ym) * (hit2->GetPosition().Y() - ym)) - rm);
    double distance3 = fabs(TMath::Sqrt((hit3->GetPosition().X() - xm) * (hit3->GetPosition().X() - xm) + (hit3->GetPosition().Y() - ym) * (hit3->GetPosition().Y() - ym)) - rm);

    // cut on the distance to accept quadriplet hypo
    double maxlimit = 1.;
    if (fabs(distance0 - hit0->GetIsochrone()) > maxlimit || fabs(distance1 - hit1->GetIsochrone()) > maxlimit || fabs(distance2 - hit2->GetIsochrone()) > maxlimit ||
        fabs(distance3 - hit3->GetIsochrone()) > maxlimit)
      continue;

    if (fDisplayOn) {
      char goOnChar;
      Refresh();

      cout << "circ1 " << x01 << " " << y01 << " " << rad1 << endl;
      cout << "circ2 " << x02 << " " << y02 << " " << rad2 << endl;
      cout << "circ3 " << x03 << " " << y03 << " " << rad3 << endl;
      cout << "circm " << xm << " " << ym << " " << rm << endl;

      TArc *arc01 = new TArc(x01, y01, rad1);
      arc01->SetFillStyle(0);
      arc01->SetLineColor(1);
      arc01->Draw("SAME");
      TArc *arc02 = new TArc(x02, y02, rad2);
      arc02->SetFillStyle(0);
      arc02->SetLineColor(2);
      arc02->Draw("SAME");
      TArc *arc03 = new TArc(x03, y03, rad3);
      arc03->SetFillStyle(0);
      arc03->SetLineColor(3);
      arc03->Draw("SAME");

      TMarker *mrk1 = new TMarker(hit0->GetPosition().X(), hit0->GetPosition().Y(), 20);
      mrk1->SetMarkerColor(kBlack);
      mrk1->Draw("SAME");

      TMarker *mrk2 = new TMarker(hit1->GetPosition().X(), hit1->GetPosition().Y(), 20);
      mrk2->SetMarkerColor(kRed);
      mrk2->Draw("SAME");

      TMarker *mrk3 = new TMarker(hit2->GetPosition().X(), hit2->GetPosition().Y(), 20);
      mrk3->SetMarkerColor(kGreen);
      mrk3->Draw("SAME");

      TMarker *mrk4 = new TMarker(hit3->GetPosition().X(), hit3->GetPosition().Y(), 20);
      mrk4->SetMarkerColor(kBlue);
      mrk4->Draw("SAME");

      TArc *arcm = new TArc(xm, ym, rm);
      arcm->SetFillStyle(0);
      arcm->SetLineStyle(2);
      arcm->SetLineColor(kMagenta);
      arcm->Draw("SAME");
      display->Update();
      display->Modified();

      cout << "hit0: " << distance0 << " " << fabs(distance0 - hit0->GetIsochrone()) << " " << fabs(distance0 - hit0->GetIsochrone()) / hit0->GetIsochrone() << " "
           << distance0 / hit0->GetIsochrone() << endl;
      cout << "hit1: " << distance1 << " " << fabs(distance1 - hit1->GetIsochrone()) << " " << fabs(distance1 - hit1->GetIsochrone()) / hit1->GetIsochrone() << " "
           << distance1 / hit1->GetIsochrone() << endl;
      cout << "hit2: " << distance2 << " " << fabs(distance2 - hit2->GetIsochrone()) << " " << fabs(distance2 - hit2->GetIsochrone()) / hit2->GetIsochrone() << " "
           << distance2 / hit2->GetIsochrone() << endl;
      cout << "hit3: " << distance3 << " " << fabs(distance3 - hit3->GetIsochrone()) << " " << fabs(distance3 - hit3->GetIsochrone()) / hit3->GetIsochrone() << " "
           << distance3 / hit3->GetIsochrone() << endl;

      cout << "next quadriplet?" << endl;
      cin >> goOnChar;
    }

    std::vector<double> track;
    track.push_back(xm);
    track.push_back(ym);
    track.push_back(rm);

    // suppress identical clones
    std::vector<std::vector<double>>::iterator itr = tracks.begin();
    itr = std::find(tracks.begin(), tracks.end(), track);
    if (itr == tracks.end()) {
      //	cout << "miss" << endl;

      // request 10 < rm < 2500
      // . 10 because it touches layers 0 and 20 -->
      // minimum diameter = 20 * 1 cm = 20 cm --> rm > 10
      // . 2500 [cm] = 15 [GeV/c] / 0.006
      if (rm > 10 && rm < 2500) {
        tracks.push_back(track);
        trackcandidates4.push_back(quadriplet);
      }
    }
    //      else cout << "already" << endl;
  }

  PndTrkClusterList clusterlist;

  //   cout << "tracks " <<  tracks.size() << endl;
  for (size_t itrk = 0; itrk < tracks.size(); itrk++) {
    std::vector<double> track = tracks[itrk];
    double x = track[0];
    double y = track[1];
    double r = track[2];
    //    cout << x << " " << y << " " << r << endl;
    /**
       if(fDisplayOn)  {
       Refresh();
       char goOnChar;
       display->cd(1);
       TArc *arcm = new TArc(x, y, r);
       arcm->SetFillStyle(0);
       arcm->SetLineColor(5);
       arcm->Draw("SAME");
       display->Update();
       display->Modified();
       }
    **/

    // find sector
    std::vector<int> quadriplet = trackcandidates4[itrk];
    PndTrkHit *hit0 = stthitlist->GetHit(quadriplet[0]);
    PndSttTube *tube0 = (PndSttTube *)fTubeArray->At(hit0->GetTubeID());
    PndTrkHit *hit1 = stthitlist->GetHit(quadriplet[1]);
    PndSttTube *tube1 = (PndSttTube *)fTubeArray->At(hit1->GetTubeID());
    PndTrkHit *hit2 = stthitlist->GetHit(quadriplet[2]);
    PndSttTube *tube2 = (PndSttTube *)fTubeArray->At(hit2->GetTubeID());
    PndTrkHit *hit3 = stthitlist->GetHit(quadriplet[3]);
    PndSttTube *tube3 = (PndSttTube *)fTubeArray->At(hit3->GetTubeID());

    std::map<int, int> sectorids;
    int sec[4];
    sec[0] = tube0->GetSectorID();
    sec[1] = tube1->GetSectorID();
    sec[2] = tube2->GetSectorID();
    sec[3] = tube3->GetSectorID();
    for (int isec = 0; isec < 4; isec++) {
      if (sectorids.find(sec[isec]) == sectorids.end())
        sectorids[sec[isec]] = 1;
      else
        sectorids[sec[isec]]++;
      //      cout << "settori " << sec[0] << " " << sec[1] << " " << sec[2] << " " << sec[3] << endl;
    }

    int tmpsecentries = 0, tmpsec = -1;
    for (size_t isec = 0; isec < sectorids.size(); isec++) {
      if (tmpsecentries < sectorids[isec]) {
        tmpsecentries = sectorids[isec];
        tmpsec = isec;
      }
    }
    int sectorID = tmpsec;
    //    cout << "SECTOR " << sectorID << " " << sectorids.size() << endl;

    // border?
    bool border = false;
    int othersecID = -1;
    for (size_t isec = 0; isec < sectorids.size(); isec++) {
      if (sectorids[isec] > 0 && (int)isec != sectorID) {
        border = true;
        othersecID = isec;
      }
    }

    //      cout << "sector " << sectorID << " at border? " << border << " " << othersecID << endl;

    // === CREATE CLUSTER
    // in the same sector or
    // nearby if at the limit of two sectors <<<<<<<<<<<<------------------
    PndTrkCluster cluster;

    // first of all the 4 hits!
    cluster.AddHit(hit0);
    cluster.AddHit(hit1);
    cluster.AddHit(hit2);
    cluster.AddHit(hit3);

    // create cluster depending on fitting
    for (int ihit = 0; ihit < stthitlist->GetNofHits(); ihit++) {
      hit = stthitlist->GetHit(ihit);
      if (hit == hit0 || hit == hit1 || hit == hit2 || hit == hit3)
        continue;
      if (hit->IsSttParallel() == kFALSE)
        continue;
      PndSttTube *tube = (PndSttTube *)fTubeArray->At(hit->GetTubeID());
      if (border == false && tube->GetSectorID() != sectorID)
        continue;
      else if (border == true && (tube->GetSectorID() != sectorID && tube->GetSectorID() != othersecID))
        continue;

      double distance_hit_center = (hit->GetPosition().XYvector() - TVector2(x, y)).Mod();
      double recoiso = fabs(distance_hit_center - r);

      if (recoiso < 1.)
        cluster.AddHit(hit);
    }
    if (fDisplayOn) {
      display->cd(1);
      cluster.Draw(kBlue);
      display->Update();
      display->Modified();
      char goOnChar;
      cout << "want to go on?" << endl;
      cin >> goOnChar;
    }

    // get indivisible hits
    // only in appropriate sectors
    PndTrkCluster indcluster;
    //    cout << "indivisibles " << fIndivisibleHitList->GetNofHits() << endl;
    if (fDisplayOn) {
      Refresh();
    }
    for (int ihit = 0; ihit < fIndivisibleHitList->GetNofHits(); ihit++) {
      PndTrkIndivisibleHit *indhit = (PndTrkIndivisibleHit *)fIndivisibleHitList->GetHit(ihit);
      if (border == false && indhit->GetSector() != sectorID)
        continue;
      else if (border == true && (indhit->GetSector() != sectorID && indhit->GetSector() != othersecID))
        continue;

      double distance_hit_center = (indhit->GetPosition().XYvector() - TVector2(x, y)).Mod();
      double recoiso = fabs(distance_hit_center - r);
      if (recoiso < 1.) {
        indcluster.AddHit(indhit);

        /**
           if(fDisplayOn) {
           indhit->Draw(kMagenta);
           }
        **/
      }
    }
    if (fDisplayOn) {
      display->Update();
      display->Modified();
      // char goOnChar; //[R.K. 01/2017] unused variable
      //       cin >> goOnChar;
    }

    // conformal map on last indivisible hit
    double tmpdistance = -1;
    int tmphitID = -1;
    for (int ihit = 0; ihit < indcluster.GetNofHits(); ihit++) {
      PndTrkIndivisibleHit *indhit2 = (PndTrkIndivisibleHit *)indcluster.GetHit(ihit);
      TVector3 position = indhit2->GetPosition();
      if (position.Perp() > tmpdistance) {
        tmpdistance = position.Perp();
        tmphitID = ihit;
      }
    }

    if (tmphitID == -1)
      continue; // CHECK what if there is no indivisible hit??
    //     cout << "nof indivisible hits " << indcluster.GetNofHits() << " " << tmphitID << endl;
    // set up conformal map
    fConformalHitList->Clear("C");
    Double_t delta = 0, trasl[2] = {0., 0.};
    PndTrkHit *refhit = indcluster.GetHit(tmphitID);
    ComputeTraAndRot(refhit, delta, trasl);
    //     cout << refhit << " " << refhit->GetHitID() << " " << delta << " " << trasl[0] << " " << trasl[1] << endl;
    conform->SetOrigin(trasl[0], trasl[1], delta);
    fConformalHitList->SetConformalTransform(conform);

    // fill conformal hits
    Int_t nofconfhits = FillConformalHitList(&cluster);

    // compute conformal plane extremities ----------------------------
    fUmin = 1000, fVmin = 1000, fRmin = 1000;
    fUmax = -1000, fVmax = -1000, fRmax = -1000;
    double rc_of_min, rc_of_max;

    for (int jhit = 0; jhit < fConformalHitList->GetNofHits(); jhit++) {
      PndTrkConformalHit *chit = fConformalHitList->GetHit(jhit);
      double u = chit->GetU();
      double v = chit->GetV();
      double rc = chit->GetIsochrone();
      if (rc < 0)
        rc = 0;
      // cout << "conf hit " << jhit << " u, v " << u << " " << v << " " << rc << endl;
      u - rc < fUmin ? fUmin = u - rc : fUmin;
      v - rc < fVmin ? fVmin = v - rc : fVmin;
      u + rc > fUmax ? fUmax = u + rc : fUmax;
      v + rc > fVmax ? fVmax = v + rc : fVmax;

      double theta1 = TMath::ATan2(v, u);
      double theta2 = theta1 + TMath::Pi();

      double r1 = u * TMath::Cos(theta1) + v * TMath::Sin(theta1);
      double r2 = u * TMath::Cos(theta2) + v * TMath::Sin(theta2);

      double rimin, rimax;
      r1 < r2 ? (rimin = r1, rimax = r2) : (rimin = r2, rimax = r1);

      rimin < fRmin ? (rc_of_min = rc, fRmin = rimin) : fRmin;
      rimax > fRmax ? (rc_of_max = rc, fRmax = rimax) : fRmax;
    }

    fRmin -= rc_of_min;
    fRmax += rc_of_max;

    // to square the conformal plane
    double du = fUmax - fUmin;
    double dv = fVmax - fVmin;
    double delt = fabs(dv - du) / 2.;
    du < dv ? (fUmin -= delt, fUmax += delt) : (fVmin -= delt, fVmax += delt);

    if (fDisplayOn) {
      DrawGeometryConf(fUmin, fUmax, fVmin, fVmax);
      // ----------------------------------------------------------------------
      for (int ihit = 0; ihit < nofconfhits; ihit++) {
        PndTrkConformalHit *chit = fConformalHitList->GetHit(ihit);
        chit->Draw(kBlack);
      }
    }

    // ====== REFIT CLUSTER LEGENDRE
    double fitm, fitq;
    //    AnalyticalFit(&cluster, x, y, r, fitm, fitq);
    // LEGENDRE ----------------------------------------------------------
    legendre->SetUpLegendreHisto(180, 0, 180, 1000, -0.07, 0.07);
    // reset the legendre histo for a new legendre fit
    legendre->ResetLegendreHisto();

    if (fDisplayOn) {
      RefreshConf();
      DrawGeometryConf(fUmin, fUmax, fVmin, fVmax);
    }

    // fill legendre histo with the cluster hits
    FillLegendreHisto(&cluster);
    std::vector<double> theta_max, r_max;
    std::vector<int> content_max;
    // get the peak
    legendre->ExtractLegendreMaxima(1, theta_max, r_max, content_max);
    // from theta/r to line parameters in CONFORMAL plane
    double fitm2, fitq2;
    legendre->ExtractLineParameters(theta_max[0], r_max[0], fitm2, fitq2);
    if (fDisplayOn) {
      display->cd(2);
      TLine *line = new TLine(-10.07, fitq2 + fitm2 * (-10.07), 10.07, fitq2 + fitm2 * (10.07));
      line->Draw("SAME");

      display->cd(3);
      DrawLegendreHisto();
      display->Update();
      display->Modified();
    }

    // from line parameters to center/radius in REAL plane
    Double_t xc, yc, R;
    FromConformalToRealTrack(fitm2, fitq2, xc, yc, R);
    //     cout << "previous " << x << " " << y << " " << r << endl;
    //     cout << "\033[1;33m MAXPEAK " << content_max[0] << " XR, YC, R: " << xc << " " << yc << " " << R << "\033[0m" << endl;
    bool goodtrack = true;
    double distance = TMath::Sqrt((x - xc) * (x - xc) + (y - yc) * (y - yc));
    double r_minus_rc = fabs(r - R);

    //     cout << "start hit " << ihit << " " << hit->GetHitID() << " " << endsecid << " " << endlayid << endl;
    // --------------------------------------------------------------
    if (fabs(distance - r_minus_rc) > 2.) { // CHECK limit
      double recoisosum = 0;
      for (int ihit = 0; ihit < cluster.GetNofHits(); ihit++) {
        hit = cluster.GetHit(ihit);
        double distance_hit_center = (hit->GetPosition().XYvector() - TVector2(xc, yc)).Mod();
        double recoiso = fabs(distance_hit_center - R);
        recoisosum += recoiso;
      }

      if (recoisosum > 10)
        goodtrack = false; // CHECK limit
    }

    // dont use track which did not
    // pass the cuts
    if (goodtrack == false)
      continue;

    if (fDisplayOn) {
      // char goOnChar; //[R.K. 01/2017] unused variable
      display->cd(1);
      TArc *arcm = new TArc(xc, yc, R);
      arcm->SetFillStyle(0);
      arcm->SetLineColor(kBlue);
      arcm->Draw("SAME");
      display->Update();
      display->Modified();
      //    cin >> goOnChar;
    }

    // what about adding the mvd hits now? lets do it!
    // ... and put mvd pixel hits to conformal plane ---------------
    for (int ihit = 0; ihit < mvdpixhitlist->GetNofHits(); ihit++) {
      hit = mvdpixhitlist->GetHit(ihit);

      //      cout << "border " << border << " " << hit->GetSector() << " " << sectorID << " " << othersecID << endl;
      //      if(border == false && hit->GetSector() != sectorID) continue;
      //       else if(border == true && (hit->GetSector() != sectorID && hit->GetSector() != othersecID)) continue;

      if (fabs(hit->GetSector() - sectorID) > 1)
        continue;
      double distance_hit_center = (hit->GetPosition().XYvector() - TVector2(xc, yc)).Mod();
      double recoiso = fabs(distance_hit_center - R);
      //       cout << "recoiso pix " << recoiso << endl;
      if (recoiso < 1.) {
        cluster.AddHit(hit);
        if (fDisplayOn) {
          hit->Draw(kOrange);
          display->Update();
          display->Modified();
        }
        PndTrkConformalHit chit = conform->GetConformalHit(hit);
        fConformalHitList->AddHit(&chit);
      }
    }

    // ... and put mvd strip hits to conformal plane ---------------
    for (int ihit = 0; ihit < mvdstrhitlist->GetNofHits(); ihit++) {
      hit = mvdstrhitlist->GetHit(ihit);
      //    if(border == false && hit->GetSector() != sectorID) continue;
      //      else if(border == true && (hit->GetSector() != sectorID && hit->GetSector() != othersecID)) continue;
      if (fabs(hit->GetSector() - sectorID) > 1)
        continue;

      double distance_hit_center = (hit->GetPosition().XYvector() - TVector2(xc, yc)).Mod();
      double recoiso = fabs(distance_hit_center - R);
      //  cout << "mvdstr recoiso str " << recoiso << endl;
      if (recoiso < 1.) {
        cluster.AddHit(hit);
        if (fDisplayOn) {
          hit->Draw(kOrange);
          display->Update();
          display->Modified();
        }
        PndTrkConformalHit chit = conform->GetConformalHit(hit);
        fConformalHitList->AddHit(&chit);
      }
    }

    // and now: gem time!
    for (int ihit = 0; ihit < gemhitlist->GetNofHits(); ihit++) {
      hit = gemhitlist->GetHit(ihit);
      //       if(border == false && hit->GetSector() != sectorID) continue;
      //       else if(border == true && (hit->GetSector() != sectorID && hit->GetSector() != othersecID)) continue;
      if (fabs(hit->GetSector() - sectorID) > 1)
        continue;
      double distance_hit_center = (hit->GetPosition().XYvector() - TVector2(xc, yc)).Mod();
      double recoiso = fabs(distance_hit_center - R);
      // if(hit->GetPosition().Perp() > CTOUTRADIUS) cout << "recoiso gem " << recoiso << endl;
      if (recoiso < 1.5 && hit->GetPosition().Perp() > CTOUTRADIUS) {
        cluster.AddHit(hit);
        if (fDisplayOn) {
          hit->Draw(kOrange);
          display->Update();
          display->Modified();
        }
        //     PndTrkConformalHit *chit = conform->GetConformalHit(hit);
        //        conformalhitlist->AddHit(chit);
      }
    }

    // and the scitil? did we forget about that? here it comes...
    // ... and put scit hits to conformal plane ------------------------------
    // the scitil are far away so give a loose request - 10 cm
    // choose however the closest one!
    // remember to check the distance from hit20
    int tmphit = -1;
    tmpdistance = 1000;
    for (int ihit = 0; ihit < scithitlist->GetNofHits(); ihit++) {
      hit = scithitlist->GetHit(ihit);
      if (hit->GetXYDistance(hit3) > 30)
        continue;
      //  if(border == false && hit->GetSector() != sectorID) continue;
      // else if(border == true && (hit->GetSector() != sectorID && hit->GetSector() != othersecID)) continue;
      // if(hit->GetSector() != sectorID) continue;
      double distance_hit_center = (hit->GetPosition().XYvector() - TVector2(xc, yc)).Mod();
      double recoiso = fabs(distance_hit_center - R);
      // cout << "recoiso scitil " << recoiso << " " <<  hit->GetXYDistance(hit3) << endl;
      if (recoiso < 10.) {
        if (recoiso < tmpdistance) {
          tmphit = ihit;
          tmpdistance = recoiso;
        }
      }
    }
    // if there is at least one: yuppi!
    if (tmphit > -1) {
      hit = scithitlist->GetHit(tmphit);
      cluster.AddHit(hit);
      if (fDisplayOn) {
        hit->Draw(kOrange);
        cout << "herh" << endl;
        char goOnChar;
        cin >> goOnChar;
        display->Update();
        display->Modified();
      }
      //     PndTrkConformalHit *chit = conform->GetConformalHit(hit);
      //     fConformalHitList->AddHit(chit);
    }

    //    // --------------------------
    //     // if there is a scitil lets use it as seed hit
    //     // for the conformal map
    //     if(tmphit > -1) {
    //       //      refhit = scithitlist->GetHit(tmphit);
    //       fConformalHitList->Reset();
    //       ComputeTraAndRot(refhit, delta, trasl);
    //       conform->SetOrigin(trasl[0], trasl[1], delta);
    //       fConformalHitList->SetConformalTransform(conform);
    //       // cout << "conformal hits " << fConformalHitList->GetNofHits() << endl;

    //       // fill conformal hits
    //       nofconfhits = FillConformalHitList(&cluster);

    //       // compute conformal plane extremities ---------------------------- this must go to a fctn CHECK
    //       fUmin =  1000, fVmin =  1000, fRmin =  1000;
    //       fUmax = -1000, fVmax = -1000, fRmax = -1000;
    //       rc_of_min, rc_of_max;

    //       for(int jhit = 0; jhit < fConformalHitList->GetNofHits(); jhit++) {
    // 	PndTrkConformalHit *chit = fConformalHitList->GetHit(jhit);
    // 	double u = chit->GetU();
    // 	double v = chit->GetV();
    // 	double rc = chit->GetIsochrone();
    // 	if(rc < 0) rc = 0;
    // 	// cout << "conf hit " << jhit << " u, v " << u << " " << v << " " << rc << endl;
    // 	u - rc < fUmin ? fUmin = u - rc : fUmin;
    // 	v - rc < fVmin ? fVmin = v - rc : fVmin;
    // 	u + rc > fUmax ? fUmax = u + rc : fUmax;
    // 	v + rc > fVmax ? fVmax = v + rc : fVmax;

    // 	double theta1 = TMath::ATan2(v, u);
    // 	double theta2 = theta1 + TMath::Pi();

    // 	double r1 = u * TMath::Cos(theta1) + v * TMath::Sin(theta1);
    // 	double r2 = u * TMath::Cos(theta2) + v * TMath::Sin(theta2);

    // 	double rimin, rimax;
    // 	r1 < r2 ? (rimin = r1, rimax = r2) : (rimin = r2, rimax = r1);

    // 	rimin < fRmin ? (rc_of_min = rc, fRmin = rimin) : fRmin;
    // 	rimax > fRmax ? (rc_of_max = rc, fRmax = rimax) : fRmax;
    //       }

    //       fRmin -= rc_of_min;
    //       fRmax += rc_of_max;

    //       // to square the conformal plane
    //       du = fUmax - fUmin;
    //       dv = fVmax - fVmin;
    //       delt = fabs(dv - du)/2.;
    //       du < dv ? (fUmin -= delt, fUmax += delt) : (fVmin -= delt, fVmax += delt);

    //     }
    //     else {    // otherwise lets refit in the same conf map
    fConformalHitList->Reset(); // CHECK maybe you can just add hits
    // fill conformal hits
    nofconfhits = FillConformalHitList(&cluster);
    //    }

    if (fDisplayOn) {
      display->cd(2);
      DrawGeometryConf(fUmin, fUmax, fVmin, fVmax);
      // ----------------------------------------------------------------------
      for (int ihit = 0; ihit < nofconfhits; ihit++) {
        PndTrkConformalHit *chit = fConformalHitList->GetHit(ihit);
        chit->Draw(kRed);
        display->Update();
        display->Modified();
      }
    }

    // ====== REFIT CLUSTER ANALYTICALLY
    double xc2, yc2, R2;

    // line or parabola or minuit
    if (1 == 1) {
      bool fitting = AnalyticalFit(&cluster, xc, yc, R, fitm, fitq);
      if (fitting == kFALSE)
        continue;
      FromConformalToRealTrack(fitm, fitq, xc2, yc2, R2);
    } else if (1 == 0) {
      double fita, fitb, fitc, epsilon;
      bool fitting = AnalyticalParabolaFit(&cluster, xc, yc, R, fita, fitb, fitc, epsilon);
      if (fitting == kFALSE)
        continue;
      FromConformalToRealTrackParabola(fita, fitb, fitc, xc2, yc2, R2, epsilon);
    } else {
      bool fitting = MinuitFit(&cluster, fitm2, fitq2, fitm, fitq);
      if (fitting == kFALSE)
        continue;
      FromConformalToRealTrack(fitm, fitq, xc2, yc2, R2);
    }

    if (fDisplayOn) {
      // char goOnChar; //[R.K. 01/2017] unused variable
      display->cd(1);
      TArc *arcm = new TArc(xc2, yc2, R2);
      arcm->SetFillStyle(0);
      arcm->SetLineColor(5);
      arcm->Draw("SAME");
      display->Update();
      display->Modified();
    }
    //    cout << "xc2: " << xc2 << " " << yc2 << " " << R2 << endl;

    // check goodness
    distance = TMath::Sqrt((xc - xc2) * (xc - xc2) + (yc - yc2) * (yc - yc2));
    r_minus_rc = fabs(R - R2);
    //    cout << "distance " << distance << " r - rc " << r_minus_rc << " " << fabs(distance - r_minus_rc) << endl;

    // no backup, just throw it away!
    if (fabs(distance - r_minus_rc) > 2.)
      continue;

    // put in a list the hits you considered and
    // decided do not belong to the track
    std::vector<std::pair<int, int>> dontuse;

    // ==================
    // MAKE the FINAL CLUSTER (at least in xy)
    //    finalcluster = new PndTrkCluster();
    fFinalCluster->Clear("C");
    // STT
    for (int ihit = 0; ihit < stthitlist->GetNofHits(); ihit++) {
      hit = stthitlist->GetHit(ihit);
      if (hit->IsSttParallel() == kFALSE)
        continue;
      PndSttTube *tube = (PndSttTube *)fTubeArray->At(hit->GetTubeID());
      if (border == false && tube->GetSectorID() != sectorID)
        continue;
      else if (border == true && (tube->GetSectorID() != sectorID && tube->GetSectorID() != othersecID))
        continue;
      double distance_hit_center = (tube->GetPosition().XYvector() - TVector2(xc2, yc2)).Mod();
      double recoiso = fabs(distance_hit_center - R2);
      if (recoiso < 1.) {
        fFinalCluster->AddHit(hit);
      }
    }
    // MVD PIX
    for (int ihit = 0; ihit < mvdpixhitlist->GetNofHits(); ihit++) {
      hit = mvdpixhitlist->GetHit(ihit);
      //    if(border == false && hit->GetSector() != sectorID) continue;
      //       else if(border == true && (hit->GetSector() != sectorID && hit->GetSector() != othersecID)) continue;
      if (fabs(hit->GetSector() - sectorID) > 1)
        continue;

      double distance_hit_center = (hit->GetPosition().XYvector() - TVector2(xc2, yc2)).Mod();
      double recoiso = fabs(distance_hit_center - R2);
      if (recoiso < 0.5) {
        // dont want two hits on the same sensor
        int samesensor = false;
        for (int khit = 0; khit < fFinalCluster->GetNofHits(); khit++) {
          PndTrkHit *hitk = fFinalCluster->GetHit(khit);
          if (!hitk->IsMvdPixel())
            continue;
          if (hitk->GetSensorID() != hit->GetSensorID())
            continue;
          samesensor = true;
          double distance_hitk_center = (hitk->GetPosition().XYvector() - TVector2(xc2, yc2)).Mod();
          double recoisok = fabs(distance_hitk_center - R2);
          if (recoiso < recoisok) {
            fFinalCluster->DeleteHitAndCompress(hitk);
            std::pair<int, int> dontpair(hitk->GetHitID(), hitk->GetDetectorID());
            dontuse.push_back(dontpair);
            fFinalCluster->AddHit(hit);
          } else {
            std::pair<int, int> dontpair(hit->GetHitID(), hit->GetDetectorID());
            dontuse.push_back(dontpair);
          }
        }
        //	fFinalCluster->AddHit(hit);
        if (samesensor == false)
          fFinalCluster->AddHit(hit);
      }
    }
    // MVD STR
    for (int ihit = 0; ihit < mvdstrhitlist->GetNofHits(); ihit++) {
      hit = mvdstrhitlist->GetHit(ihit);
      //   if(border == false && hit->GetSector() != sectorID) continue;
      //       else if(border == true && (hit->GetSector() != sectorID && hit->GetSector() != othersecID)) continue;
      if (fabs(hit->GetSector() - sectorID) > 1)
        continue;

      double distance_hit_center = (hit->GetPosition().XYvector() - TVector2(xc2, yc2)).Mod();
      double recoiso = fabs(distance_hit_center - R2);
      if (recoiso < 1.) {
        // dont want two hits on the same sensor
        int samesensor = false;
        for (int khit = 0; khit < fFinalCluster->GetNofHits(); khit++) {
          PndTrkHit *hitk = fFinalCluster->GetHit(khit);
          if (!hitk->IsMvdStrip())
            continue;
          if (hitk->GetSensorID() != hit->GetSensorID())
            continue;
          samesensor = true;
          double distance_hitk_center = (hitk->GetPosition().XYvector() - TVector2(xc2, yc2)).Mod();
          double recoisok = fabs(distance_hitk_center - R2);
          if (recoiso < recoisok) {
            fFinalCluster->DeleteHitAndCompress(hitk);
            std::pair<int, int> dontpair(hitk->GetHitID(), hitk->GetDetectorID());
            dontuse.push_back(dontpair);
            fFinalCluster->AddHit(hit);
          } else {
            std::pair<int, int> dontpair(hit->GetHitID(), hit->GetDetectorID());
            dontuse.push_back(dontpair);
          }
        }
        //	    	fFinalCluster->AddHit(hit);
        if (samesensor == false)
          fFinalCluster->AddHit(hit);
      }
    }
    // GEM
    for (int ihit = 0; ihit < gemhitlist->GetNofHits(); ihit++) {
      hit = gemhitlist->GetHit(ihit);
      //     if(border == false && hit->GetSector() != sectorID) continue;
      //       else if(border == true && (hit->GetSector() != sectorID && hit->GetSector() != othersecID)) continue;
      if (fabs(hit->GetSector() - sectorID) > 1)
        continue;
      double distance_hit_center = (hit->GetPosition().XYvector() - TVector2(xc2, yc2)).Mod();
      double recoiso = fabs(distance_hit_center - R2);
      //     if(hit->GetPosition().Perp() > CTOUTRADIUS) cout << "gem recoiso " << recoiso << endl;
      if (recoiso < 1.5 && hit->GetPosition().Perp() > CTOUTRADIUS) {
        // cout << "add " << hit->GetDetectorID() << " " << hit->GetHitID()  << endl;
        fFinalCluster->AddHit(hit);
      }
    }
    // SCITIL
    tmphit = -1;
    tmpdistance = 1000;
    for (int ihit = 0; ihit < scithitlist->GetNofHits(); ihit++) {
      hit = scithitlist->GetHit(ihit);
      if (hit->GetXYDistance(hit3) > 30)
        continue;
      double distance_hit_center = (hit->GetPosition().XYvector() - TVector2(xc2, yc2)).Mod();
      double recoiso = fabs(distance_hit_center - R2);
      if (recoiso < 1.) {
        if (recoiso < tmpdistance) {
          tmphit = ihit;
          tmpdistance = recoiso;
        }
      }
    }
    if (tmphit > -1) {
      hit = scithitlist->GetHit(tmphit);
      fFinalCluster->AddHit(hit);
    }

    if (fFinalCluster->GetNofHits() == 0)
      continue;
    //    cout << "FINAL CLUSTER HAS " << fFinalCluster->GetNofHits() << endl;
    if (fDisplayOn) {
      Refresh();
      display->cd(1);
      fFinalCluster->LightUp();
      display->Update();
      display->Modified();
      char goOnChar;
      cin >> goOnChar;
    }

    // sorting
    for (int ihit = 0; ihit < fFinalCluster->GetNofHits(); ihit++) {
      hit = fFinalCluster->GetHit(ihit);
      hit->SetSortVariable(hit->GetPosition().Perp());
      /**
   if(hit->IsMvdPixel())
   else if(hit->IsMvdStrip())
   else if(hit->IsStt())
   else if(hit->IsGem())
   else if(hit->IsScitil())
      **/
    }
    fFinalCluster->Sort();

    PndTrkTrack finaltrack(fFinalCluster, xc2, yc2, R2);
    finaltrack.ComputeCharge();

    //
    //    fDisplayOn = kTRUE;
    if (fDisplayOn) {
      Refresh();
      display->cd(1);
      fFinalCluster->Draw(kGreen);
      finaltrack.Draw(kGreen);
      display->Update();
      display->Modified();
    }

    // =========================== Z PART ==================================
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // SKEWED ASSOCIATION ********* CHECK *********
    // -------------------------------------------------------
    //    cout << " %%%%%%%%%%%%%%%%%%%% ZFINDER %%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;

    if (fDisplayOn)
      DrawZGeometry(-360, 360, -40, 200);

    // create cluster for z finding

    // lets start from the skewed --------------------------------
    //    PndTrkCluster skewhitlist = CreateSkewHitList(finaltrack);
    PndTrkSkewHitList skewhitlist;
    // double phimin = 400, phimax = -1, zmin = 1000, zmax = -1; //[R.K. 01/2017] unused variable
    for (int ihit = 0; ihit < stthitlist->GetNofHits(); ihit++) {
      hit = stthitlist->GetHit(ihit);
      //      cout << hit->IsSttSkew() << " " << hit->GetSector() << " " << TMath::RadToDeg() * hit->GetPosition().Phi() << " " << sectorID << " " << othersecID << endl;
      if (hit->IsSttSkew() == kFALSE)
        continue;

      if (border == false && hit->GetSector() != sectorID)
        continue;
      else if (border == true && (hit->GetSector() != sectorID && hit->GetSector() != othersecID))
        continue;

      int tubeID = hit->GetTubeID();
      PndSttTube *tube = (PndSttTube *)fTubeArray->At(tubeID);

      TVector3 wireDirection = tube->GetWireDirection();
      Double_t halflength = tube->GetHalfLength();

      TVector3 first = tube->GetPosition() + wireDirection * halflength;  // CHECK
      TVector3 second = tube->GetPosition() - wireDirection * halflength; // CHECK

      //    double m1 = (first - second).Y()/(first - second).X();
      //    double q1 = first.Y() - m1 * first.X();

      // 1. compute intersection between the track circle and the wire
      TVector2 intersection1, intersection2;
      Int_t nofintersections =
        tools->ComputeSegmentCircleIntersection(TVector2(first.X(), first.Y()), TVector2(second.X(), second.Y()), xc2, yc2, R2, intersection1, intersection2);

      if (nofintersections == 0)
        continue;
      if (nofintersections >= 2) {
        if (fVerbose)
          cout << "ERROR: MORE THAN 1 INTERSECTION!!" << endl;
        continue; // CHECK
      }

      // 2. find the tangent to the track in the intersection point
      // tangent approximation
      TVector2 tangent = tools->ComputeTangentInPoint(xc2, yc2, intersection1);

      // 3. rotate clockwise the tangent/point/(wire, not explicitely)
      // in order to have the wire parallel to the x axis;
      // then translate everything to have the wire ON the x axis
      double beta = wireDirection.Phi();
      if (beta < 0)
        beta += TMath::Pi();
      // ... rotate the tangent
      double rtx = TMath::Cos(beta) * tangent.X() + TMath::Sin(beta) * tangent.Y();
      double rty = TMath::Cos(beta) * tangent.Y() - TMath::Sin(beta) * tangent.X();
      TVector2 rottangent(rtx, rty);
      rottangent = rottangent.Unit();
      // ... rotate the point
      double rx = TMath::Cos(beta) * intersection1.X() + TMath::Sin(beta) * intersection1.Y();
      double ry = TMath::Cos(beta) * intersection1.Y() - TMath::Sin(beta) * intersection1.X();

      // translation
      Double_t deltay = ry;
      rty -= deltay;
      ry -= deltay;

      // rotm, rotp
      Double_t rotm = rottangent.Y() / rottangent.X();
      Double_t rotp = ry - rotm * rx;

      // ellipsis
      double a = hit->GetIsochrone() * TMath::Cos(SKEW_ANGLE); // CHECK skew angle hard coded
      double b = hit->GetIsochrone();

      // center of ellipsis
      Double_t x0a, x0b, y0;
      y0 = 0.;
      x0a = (-rotp + TMath::Sqrt(b * b + a * a * rotm * rotm)) / rotm;
      x0b = (-rotp - TMath::Sqrt(b * b + a * a * rotm * rotm)) / rotm;

      // intersection point
      double intxa = (x0a * b * b - rotm * rotp * a * a) / (b * b + rotm * rotm * a * a);
      double intya = rotm * intxa + rotp;
      double intxb = (x0b * b * b - rotm * rotp * a * a) / (b * b + rotm * rotm * a * a);
      double intyb = rotm * intxb + rotp;

      // 4. retraslate/rerotate all back to the original plane
      // retranslate
      y0 += deltay;
      intya += deltay;
      intyb += deltay;

      // rerotate
      double x0anew = TMath::Cos(beta) * x0a - TMath::Sin(beta) * y0;
      double y0anew = TMath::Cos(beta) * y0 + TMath::Sin(beta) * x0a;
      double x0bnew = TMath::Cos(beta) * x0b - TMath::Sin(beta) * y0;
      double y0bnew = TMath::Cos(beta) * y0 + TMath::Sin(beta) * x0b;

      double intxanew = TMath::Cos(beta) * intxa - TMath::Sin(beta) * intya;
      double intyanew = TMath::Cos(beta) * intya + TMath::Sin(beta) * intxa;
      double intxbnew = TMath::Cos(beta) * intxb - TMath::Sin(beta) * intyb;
      double intybnew = TMath::Cos(beta) * intyb + TMath::Sin(beta) * intxb;

      intxa = intxanew;
      intya = intyanew;
      intxb = intxbnew;
      intyb = intybnew;

      // now we have x0a, y0a, center of the 1st ellipse
      // and x0b, y0b, center of the 2nd ellipse
      x0a = x0anew;
      double y0a = y0anew;
      x0b = x0bnew;
      double y0b = y0bnew;

      if (fDisplayOn) {
        // char goOnChar; //[R.K. 01/2017] unused variable
        display->cd(1);

        TEllipse *ell1 = new TEllipse(x0a, y0a, a, b, 0, 360, -beta);
        ell1->SetFillStyle(0);
        ell1->SetLineColor(4);
        ell1->Draw("SAME");
        TEllipse *ell2 = new TEllipse(x0b, y0b, a, b, 0, 360, -beta);
        ell2->SetFillStyle(0);
        ell2->SetLineColor(6);
        ell2->Draw("SAME");

        TMarker *mrkinta = new TMarker(intxa, intya, 20);
        mrkinta->SetMarkerColor(4);
        mrkinta->Draw("SAME");
        TMarker *mrkintb = new TMarker(intxb, intyb, 20);
        mrkintb->SetMarkerColor(6);
        mrkintb->Draw("SAME");
      }

      // 5. calculate z coordinate for each intersection

      // calculate z0a, z0b of the center of the ellipse
      Double_t t = ((x0a + y0a) - (first.X() + first.Y())) / ((second.X() - first.X()) + (second.Y() - first.Y()));
      Double_t z0a = first.Z() + (second.Z() - first.Z()) * t;
      //    cout << "0 : calculate t, z0a " << t << " " << z0a << endl;

      t = ((x0b + y0b) - (first.X() + first.Y())) / ((second.X() - first.X()) + (second.Y() - first.Y()));
      Double_t z0b = first.Z() + (second.Z() - first.Z()) * t;

      TVector3 center1(x0a, y0a, z0a);
      TVector3 center2(x0b, y0b, z0b);

      // calculate the z of the intersection ON the ellipse (CHECK this step calculations!)
      double dx = intxa - x0a;
      double dy = intya - y0a;
      TVector3 dxdy(dx, dy, 0.0);

      TVector3 tfirst = first + dxdy;
      TVector3 tsecond = second + dxdy;

      t = ((intxa + intya) - (tfirst.X() + tfirst.Y())) / ((tsecond.X() - tfirst.X()) + (tsecond.Y() - tfirst.Y()));
      double intza = tfirst.Z() + (tsecond.Z() - tfirst.Z()) * t;
      //      if(fDisplayOn) {
      // 	char goOnChar;
      // 	display->cd(3);
      // 	TLine *linezx1 = new TLine(tfirst.X(), tfirst.Z(), tsecond.X(), tsecond.Z());
      // 	linezx1->SetLineStyle(1);
      // 	linezx1->Draw("SAME");
      // 	TMarker *mrkza1 = new TMarker(intxa, intza, 20);
      // 	mrkza1->SetMarkerColor(kBlue - 9);
      // 	mrkza1->Draw("SAME");
      // 	cin >> goOnChar;
      // 	display->Update();
      // 	display->Modified();
      //      }

      tfirst = first - dxdy;
      tsecond = second - dxdy;

      t = ((intxb + intyb) - (tfirst.X() + tfirst.Y())) / ((tsecond.X() - tfirst.X()) + (tsecond.Y() - tfirst.Y()));
      double intzb = tfirst.Z() + (tsecond.Z() - tfirst.Z()) * t;

      TVector3 fin_intersection1(intxa, intya, intza);
      TVector3 fin_intersection2(intxb, intyb, intzb);

      // CHECK to be changed
      int trackID = 1;
      double phi1 = finaltrack.ComputePhi(fin_intersection1);
      double phi2 = finaltrack.ComputePhi(fin_intersection2);

      // skewhit = new PndTrkSkewHit(*hit, trackID, center1, fin_intersection1, phi1, center2, fin_intersection2, phi2, a, b, -1, beta);
      //    //      skewhit->Print();
      //       skewhitlist.AddHit(skewhit);

      skewhitlist.AddHit(PndTrkSkewHit(*hit, trackID, center1, fin_intersection1, phi1, center2, fin_intersection2, phi2, a, b, -1, beta));

      //      if(fDisplayOn) {
      // 	display->cd(4);
      // 	TLine *linezphi = new TLine(phi1, fin_intersection1.Z(), phi2, fin_intersection2.Z());
      // 	linezphi->SetLineStyle(1);
      // 	linezphi->Draw("SAME");
      // 	TMarker *mrkzphi1 = new TMarker(phi1, fin_intersection1.Z(), 20);
      // 	mrkzphi1->SetMarkerColor(kBlue - 9);
      // 	mrkzphi1->Draw("SAME");
      // 	TMarker *mrkzphi2 = new TMarker(phi2, fin_intersection2.Z(), 20);
      // 	mrkzphi2->SetMarkerColor(kMagenta - 7);
      // 	mrkzphi2->Draw("SAME");
      // 	display->Update();
      // 	display->Modified();
      //       }
      // --------------------------
    }

    // add hits which have a z info
    // and belong to the cluster
    for (int ihit = 0; ihit < fFinalCluster->GetNofHits(); ihit++) {
      hit = fFinalCluster->GetHit(ihit);
      double phi = finaltrack.ComputePhi(hit->GetPosition());
      hit->SetPhi(phi);
      if (hit->IsStt() == kFALSE)
        skewhitlist.AddHit(PndTrkSkewHit(*hit));
    }

    //    if(fDisplayOn) {

    //       for(int ihit = 0; ihit < fFinalCluster->GetNofHits(); ihit++) {
    // 	PndTrkHit *hitstt1 = fFinalCluster->GetHit(ihit);
    // 	if(hitstt1->IsStt() == kFALSE) continue;
    // 	PndSttHit *stthit1 = (PndSttHit*) fSttHitArray->At(hitstt1->GetHitID());

    // 	indiv = fHitMap->GetIndivisiblesToHit(hitstt1);
    // 	for(int jhit = 0; jhit < indiv.GetEntriesFast(); jhit++) {
    // 	  PndTrkHit *hitstt2 = (PndTrkHit*) indiv.At(jhit);
    // 	  PndSttHit *stthit2 =  (PndSttHit*) fSttHitArray->At(hitstt2->GetHitID());

    // 	  TVector3 poca(0, 0, 0);
    // 	  PndSttGeometryMap geomap;
    // 	  Double_t distancepoca = fMapper->GetGeometryMap()->CalculateStrawPoca(stthit1, stthit2, poca);

    // 	  TVector3 pos1, pos2;
    // 	  stthit1->Position(pos1);
    // 	  pos1.Print();
    // 	  stthit2->Position(pos2);
    // 	  pos2.Print();
    // 	  poca.Print();

    // 	  TMarker *mrk = new TMarker(poca.X(), poca.Y(), 20);
    // 	  mrk->SetMarkerColor(kOrange);
    // 	  mrk->Draw("SAME");
    // 	  char goOnChar;
    // 	  display->cd(1);
    // 	  cin >> goOnChar;
    // 	  display->Update();
    // 	  display->Modified();

    // 	}
    //       }

    //       for(int ihit = 0; ihit < skewhitlist.GetNofHits(); ihit++) {
    // 	PndTrkHit *hitstt1 = skewhitlist.GetHit(ihit);
    // 	if(hitstt1->IsStt() == kFALSE) continue;
    // 	PndSttHit *stthit1 = (PndSttHit*) fSttHitArray->At(hitstt1->GetHitID());

    // 	indiv = fHitMap->GetIndivisiblesToHit(hitstt1);
    // 	for(int jhit = 0; jhit < indiv.GetEntriesFast(); jhit++) {
    // 	  PndTrkHit *hitstt2 = (PndTrkHit*) indiv.At(jhit);
    // 	  PndSttHit *stthit2 =  (PndSttHit*) fSttHitArray->At(hitstt2->GetHitID());

    // 	  TVector3 poca(0, 0, 0);
    // 	  PndSttGeometryMap geomap;
    // 	  Double_t distancepoca = fMapper->GetGeometryMap()->CalculateStrawPoca(stthit1, stthit2, poca);

    // 	  TVector3 pos1, pos2;
    // 	  stthit1->Position(pos1);
    // 	  pos1.Print();
    // 	  stthit2->Position(pos2);
    // 	  pos2.Print();
    // 	  poca.Print();

    // 	  TMarker *mrk = new TMarker(poca.X(), poca.Y(), 20);
    // 	  mrk->SetMarkerColor(kOrange);
    // 	  mrk->Draw("SAME");
    // 	  char goOnChar;
    // 	  display->cd(1);
    // 	  cin >> goOnChar;
    // 	  display->Update();
    // 	  display->Modified();

    // 	}
    //       }

    //     }

    // ----------------------------------------------
    int phisec[2] = {0, 0};
    for (int ihit = 0; ihit < skewhitlist.GetNofHits(); ihit++) {
      PndTrkHit *hitj = skewhitlist.GetHit(ihit);
      if (!hitj)
        continue;

      TVector3 fin_intersection21(-999, -999, -999), fin_intersection22(-999, -999, -999);
      double phi21 = -999, phi22 = -999;

      if (hitj->IsStt() == kTRUE) {
        PndTrkSkewHit *skewhit2 = (PndTrkSkewHit *)hitj;

        fin_intersection21 = skewhit2->GetIntersection1();
        fin_intersection22 = skewhit2->GetIntersection2();
        phi21 = skewhit2->GetPhi1();
        phi22 = skewhit2->GetPhi2();

        double phimean = 0.5 * (phi21 + phi22);
        if (phimean >= 0 && phimean < 180)
          phisec[0]++;
        else
          phisec[1]++;

      } else {
        fin_intersection21 = hitj->GetPosition();
        phi21 = hitj->GetPhi();
        if (phi21 >= 0 && phi21 < 180)
          phisec[0]++;
        else
          phisec[1]++;
      }
    }

    //    cout << "PHI SECTOR " << phisec[0] << " " << phisec[1] << endl;

    // correction
    for (int ihit = 0; ihit < skewhitlist.GetNofHits(); ihit++) {
      PndTrkHit *hitj = skewhitlist.GetHit(ihit);
      if (!hitj)
        continue;

      TVector3 fin_intersection21(-999, -999, -999), fin_intersection22(-999, -999, -999);
      double phi21 = -999, phi22 = -999;

      if (hitj->IsStt() == kTRUE) {
        PndTrkSkewHit *skewhit2 = (PndTrkSkewHit *)hitj;

        fin_intersection21 = skewhit2->GetIntersection1();
        fin_intersection22 = skewhit2->GetIntersection2();
        phi21 = skewhit2->GetPhi1();
        phi22 = skewhit2->GetPhi2();

        double phimean = 0.5 * (phi21 + phi22);
        // if 1st sec
        if ((phisec[0] > phisec[1]) && (phimean >= 180)) {
          phi21 -= 180.;
          phi22 -= 180.;
          skewhit2->SetPhi1(phi21);
          skewhit2->SetPhi2(phi22);
        } else if ((phisec[0] < phisec[1]) && (phimean < 180)) { // 2nd sec
          phi21 += 180.;
          phi22 += 180.;
          skewhit2->SetPhi1(phi21);
          skewhit2->SetPhi2(phi22);
        }
      } else {
        fin_intersection21 = hitj->GetPosition();
        phi21 = hitj->GetPhi();

        // if 1st sec
        if ((phisec[0] > phisec[1]) && (phi21 >= 180)) {
          phi21 -= 180.;
          hitj->SetPhi(phi21);
        } else if ((phisec[0] < phisec[1]) && (phi21 < 180)) { // 2nd sec
          phi21 += 180.;
          hitj->SetPhi(phi21);
        }
      }
    }

    // ----------------------------------------------

    // ========================================================
    std::vector<int> first, second;
    double fitm3 = 0, fitq3 = 0;
    for (int ihit = 0; ihit < skewhitlist.GetNofHits(); ihit++) {
      hit = skewhitlist.GetHit(ihit);
      if (!hit)
        continue;
      if (hit->IsStt() == kFALSE)
        continue;

      PndTrkSkewHit *skewhit = (PndTrkSkewHit *)hit;

      TVector3 fin_intersection1 = skewhit->GetIntersection1();
      TVector3 fin_intersection2 = skewhit->GetIntersection2();
      double phi1 = skewhit->GetPhi1();
      double phi2 = skewhit->GetPhi2();

      if (fDisplayOn) {
        display->cd(4);
        TLine *linezphi = new TLine(phi1, fin_intersection1.Z(), phi2, fin_intersection2.Z());
        linezphi->SetLineStyle(1);
        linezphi->Draw("SAME");
        TMarker *mrkzphi1 = new TMarker(phi1, fin_intersection1.Z(), 20);
        mrkzphi1->SetMarkerColor(kBlue - 9);
        mrkzphi1->Draw("SAME");
        TMarker *mrkzphi2 = new TMarker(phi2, fin_intersection2.Z(), 20);
        mrkzphi2->SetMarkerColor(kMagenta - 7);
        mrkzphi2->Draw("SAME");
        display->Update();
        display->Modified();
      }

      // check neighborings @ layer 8 & 15
      // PndSttTube *tube = (PndSttTube*) fTubeArray->At(hit->GetTubeID()); //[R.K. 01/2017] unused variable
      //      if(tube->GetLayerID() == 8) {
      // 	PndTrkHit *thit = stthitlist->GetHit(hit->GetHitID());
      // 	TObjArray neighs = fHitMap->GetNeighboringsToHit(thit);
      // 	//	 cout << thit->GetHitID() << " on layer " << tube->GetLayerID() << " " << neighs.GetEntriesFast() << endl;
      // 	for(int j = 0; j < neighs.GetEntriesFast(); j++) {
      // 	  PndTrkHit *nhit = (PndTrkHit*) neighs.At(j);
      // 	  PndSttTube *ntube = (PndSttTube*) fTubeArray->At(nhit->GetTubeID());
      // 	  if(ntube->GetLayerID() == 7) {
      // 	    //	     cout << "hit close to 7" << endl;
      // 	    first.push_back(ihit);
      // 	  }
      // 	}
      //       }
      //       if(tube->GetLayerID() == 15)
      // 	{
      // 	  PndTrkHit *thit = stthitlist->GetHit(hit->GetHitID());
      // 	  TObjArray neighs = fHitMap->GetNeighboringsToHit(thit);
      // 	  // cout << hit->GetHitID() << " on layer " << tube->GetLayerID() << " " << neighs.GetEntriesFast() << endl;
      // 	  for(int j = 0; j < neighs.GetEntriesFast(); j++) {
      // 	    PndTrkHit *nhit = (PndTrkHit*) neighs.At(j);
      // 	    PndSttTube *ntube = (PndSttTube*) fTubeArray->At(nhit->GetTubeID());
      // 	    if(ntube->GetLayerID() == 16) {
      // 	      //	       cout << "hit close to 16" << endl;
      // 	      second.push_back(ihit);
      // 	    }
      // 	  }
      // 	}
    }

    //    if(first.size() != 0 && second.size() != 0) {

    //       double zdistance = 1000;
    //       int tmpi = -1, tmpj = -1;
    //       double tmpiz = -999, tmpjz = -999, tmpiphi = -999, tmpjphi = -999;

    //       for(int ihit = 0; ihit < first.size(); ihit++) {
    // 	int hitiskeid = first[ihit];
    // 	PndTrkSkewHit *skewhiti = (PndTrkSkewHit*) skewhitlist.GetHit(hitiskeid);
    // 	TVector3 fin_intersectioni = 0.5 * (skewhiti->GetIntersection1() + skewhiti->GetIntersection2());
    // 	double phii = 0.5 * (skewhiti->GetPhi1() + skewhiti->GetPhi2()) ;
    // 	for(int jhit = 0; jhit < second.size(); jhit++) {
    // 	  int hitjskeid = second[jhit];
    // 	  PndTrkSkewHit *skewhitj = (PndTrkSkewHit*) skewhitlist.GetHit(hitjskeid);
    // 	  TVector3 fin_intersectionj = 0.5 * (skewhitj->GetIntersection1() + skewhitj->GetIntersection2());
    // 	  double phij = 0.5 * (skewhitj->GetPhi1() + skewhitj->GetPhi2()) ;

    // 	  double tmpzdistance = fabs(fin_intersectioni.Z() - fin_intersectionj.Z());
    // 	  if(tmpzdistance < zdistance) {
    // 	    zdistance = tmpzdistance;
    // 	    tmpi = hitiskeid;
    // 	    tmpj = hitjskeid;
    // 	    tmpiz = fin_intersectioni.Z();
    // 	    tmpjz = fin_intersectionj.Z();
    // 	    tmpiphi = phii;
    // 	    tmpjphi = phij;
    // 	  }
    // 	}
    //       }

    //       fitm3 = (tmpiz - tmpjz) / (tmpiphi - tmpjphi);
    //       fitq3 = tmpiz - fitm3 * tmpiphi;
    //     }
    //     else {

    fLineHisto->Reset();
    for (int ihit = 0; ihit < skewhitlist.GetNofHits(); ihit++) {
      hit = skewhitlist.GetHit(ihit);
      if (!hit)
        continue;

      TVector3 fin_intersection11(-999, -999, -999), fin_intersection12(-999, -999, -999);
      double phi11 = -999, phi12 = -999;

      if (hit->IsStt() == kTRUE) {
        PndTrkSkewHit *skewhit1 = (PndTrkSkewHit *)hit;

        fin_intersection11 = skewhit1->GetIntersection1();
        fin_intersection12 = skewhit1->GetIntersection2();
        phi11 = skewhit1->GetPhi1();
        phi12 = skewhit1->GetPhi2();
      } else {
        fin_intersection11 = hit->GetPosition();
        phi11 = hit->GetPhi();
      }

      if (fDisplayOn) {
        display->cd(4);
        if (phi12 != -999) {
          TLine *linezphi = new TLine(phi11, fin_intersection11.Z(), phi12, fin_intersection12.Z());
          linezphi->SetLineStyle(1);
          linezphi->Draw("SAME");
          TMarker *mrkzphi1 = new TMarker(phi11, fin_intersection11.Z(), 20);
          mrkzphi1->SetMarkerColor(kBlue - 9);
          mrkzphi1->Draw("SAME");
          TMarker *mrkzphi2 = new TMarker(phi12, fin_intersection12.Z(), 20);
          mrkzphi2->SetMarkerColor(kMagenta - 7);
          mrkzphi2->Draw("SAME");
        } else {
          TMarker *mrkzphi1 = new TMarker(phi11, fin_intersection11.Z(), 20);
          mrkzphi1->SetMarkerColor(kGreen);
          mrkzphi1->Draw("SAME");
        }
        display->Update();
        display->Modified();
      }

      for (int jhit = ihit + 1; jhit < skewhitlist.GetNofHits(); jhit++) {
        PndTrkHit *hitj = skewhitlist.GetHit(jhit);
        if (!hitj)
          continue;

        TVector3 fin_intersection21(-999, -999, -999), fin_intersection22(-999, -999, -999);
        double phi21 = -999, phi22 = -999;

        if (hitj->IsStt() == kTRUE) {
          PndTrkSkewHit *skewhit2 = (PndTrkSkewHit *)hitj;

          fin_intersection21 = skewhit2->GetIntersection1();
          fin_intersection22 = skewhit2->GetIntersection2();
          phi21 = skewhit2->GetPhi1();
          phi22 = skewhit2->GetPhi2();
        } else {
          fin_intersection21 = hitj->GetPosition();
          phi21 = hitj->GetPhi();
        }

        // 1 1
        double cost = (fin_intersection21.Z() - fin_intersection11.Z()) /
                      TMath::Sqrt((phi11 - phi21) * (phi11 - phi21) + (fin_intersection21.Z() - fin_intersection11.Z()) * (fin_intersection21.Z() - fin_intersection11.Z()));
        double theta = TMath::ACos(cost);
        double r1 = phi11 * cost + fin_intersection11.Z() * TMath::Sin(theta);
        double r2 = phi21 * cost + fin_intersection21.Z() * TMath::Sin(theta);
        if (fabs(r1 - r2) > 1.e-9) {
          theta = -TMath::ACos(cost);
          r1 = phi11 * cost + fin_intersection11.Z() * TMath::Sin(theta);
        }
        fLineHisto->Fill(theta * TMath::RadToDeg(), r1);
        // double fitm11 =  -TMath::Cos(theta)/TMath::Sin(theta); //[R.K. 03/2017] unused variable
        // double fitq11 = r1/TMath::Sin(theta); //[R.K. 03/2017] unused variable
        // TLine *line11 = new TLine(0, fitq11, 360, 360 * fitm11 + fitq11); //[R.K. 03/2017] unused variable
        //    line11->Draw("SAME");
        // 	    cout << phi11 << " " << phi21 << endl;

        if (phi12 != -999) {
          // 2 1
          cost = (fin_intersection21.Z() - fin_intersection12.Z()) /
                 TMath::Sqrt((phi12 - phi21) * (phi12 - phi21) + (fin_intersection21.Z() - fin_intersection12.Z()) * (fin_intersection21.Z() - fin_intersection12.Z()));
          theta = TMath::ACos(cost);
          r1 = phi12 * cost + fin_intersection12.Z() * TMath::Sin(theta);
          r2 = phi21 * cost + fin_intersection21.Z() * TMath::Sin(theta);
          if (fabs(r1 - r2) > 1.e-9) {
            theta = -TMath::ACos(cost);
            r1 = phi12 * cost + fin_intersection12.Z() * TMath::Sin(theta);
          }
          fLineHisto->Fill(theta * TMath::RadToDeg(), r1);
          // double fitm21 =  -TMath::Cos(theta)/TMath::Sin(theta); //[R.K. 03/2017] unused variable
          // double fitq21 = r1/TMath::Sin(theta); //[R.K. 03/2017] unused variable
          // TLine *line21 = new TLine(0, fitq21, 360, 360 * fitm21 + fitq21); //[R.K. 03/2017] unused variable
          // 	      line21->Draw("SAME");
          // 	      cout << phi12 << " " << phi21 << endl;
        }

        if (phi22 != -999) {
          // 1 2
          cost = (fin_intersection11.Z() - fin_intersection22.Z()) /
                 TMath::Sqrt((phi22 - phi11) * (phi22 - phi11) + (fin_intersection11.Z() - fin_intersection22.Z()) * (fin_intersection11.Z() - fin_intersection22.Z()));
          theta = TMath::ACos(cost);
          r1 = phi22 * cost + fin_intersection22.Z() * TMath::Sin(theta);
          r2 = phi11 * cost + fin_intersection11.Z() * TMath::Sin(theta);
          if (fabs(r1 - r2) > 1.e-9) {
            theta = -TMath::ACos(cost);
            r1 = phi22 * cost + fin_intersection22.Z() * TMath::Sin(theta);
          }
          fLineHisto->Fill(theta * TMath::RadToDeg(), r1);
          // double fitm12 =  -TMath::Cos(theta)/TMath::Sin(theta); //[R.K. 03/2017] unused variable
          // double fitq12 = r1/TMath::Sin(theta); //[R.K. 03/2017] unused variable
          // TLine *line12 = new TLine(0, fitq12, 360, 360 * fitm12 + fitq12); //[R.K. 03/2017] unused variable
          // 	      line12->Draw("SAME");
          // 	      cout << phi11<< " " << phi22<< endl;
        }

        if (phi12 != -999 && phi22 != -999) {
          // 2 2
          cost = (fin_intersection12.Z() - fin_intersection22.Z()) /
                 TMath::Sqrt((phi22 - phi12) * (phi22 - phi12) + (fin_intersection12.Z() - fin_intersection22.Z()) * (fin_intersection12.Z() - fin_intersection22.Z()));
          theta = TMath::ACos(cost);
          r1 = phi22 * cost + fin_intersection22.Z() * TMath::Sin(theta);
          r2 = phi12 * cost + fin_intersection12.Z() * TMath::Sin(theta);
          if (fabs(r1 - r2) > 1.e-9) {
            theta = -TMath::ACos(cost);
            r1 = phi22 * cost + fin_intersection22.Z() * TMath::Sin(theta);
          }
          fLineHisto->Fill(theta * TMath::RadToDeg(), r1);
          // double fitm22 =  -TMath::Cos(theta)/TMath::Sin(theta); //[R.K. 03/2017] unused variable
          // double fitq22 = r1/TMath::Sin(theta); //[R.K. 03/2017] unused variable
          // TLine *line22 = new TLine(0, fitq22, 360, 360 * fitm22 + fitq22); //[R.K. 03/2017] unused variable
          // 	       line22->Draw("SAME");
          // 	       cout << phi12 << " " << phi22<< endl;
        }
      }
    }

    if (fDisplayOn) {
      display->cd(3);
      fLineHisto->Draw("colz");
      display->cd(4);
      display->Update();
      display->Modified();
      char goOnChar;
      cin >> goOnChar;
    }

    int bin = fLineHisto->GetMaximumBin();
    int binx, biny, binz;
    fLineHisto->GetBinXYZ(bin, binx, biny, binz);
    double tpeak = fLineHisto->GetXaxis()->GetBinCenter(binx);
    double rpeak = fLineHisto->GetYaxis()->GetBinCenter(biny);
    //  cout << "tpeak " << tpeak << " rpeak " << rpeak << endl;
    fitm3 = -TMath::Cos(tpeak * TMath::DegToRad()) / TMath::Sin(tpeak * TMath::DegToRad());
    fitq3 = rpeak / TMath::Sin(tpeak * TMath::DegToRad());
    // }

    // ==========================================
    fFitter->Reset();
    for (int ihit = 0; ihit < skewhitlist.GetNofHits(); ihit++) {
      hit = skewhitlist.GetHit(ihit);
      if (!hit)
        continue;

      TVector3 fin_intersection11(-999, -999, -999), fin_intersection12(-999, -999, -999);
      double phi11 = -999, phi12 = -999;

      double phi = -999, z = -999, sigma = -999;

      if (hit->IsStt() == kTRUE) {
        PndTrkSkewHit *skewhit1 = (PndTrkSkewHit *)hit;

        fin_intersection11 = skewhit1->GetIntersection1();
        fin_intersection12 = skewhit1->GetIntersection2();
        phi11 = skewhit1->GetPhi1();
        phi12 = skewhit1->GetPhi2();

        z = 0.5 * (fin_intersection11.Z() + fin_intersection12.Z());
        phi = 0.5 * (phi11 + phi12);
        sigma = (fin_intersection11.Z() - fin_intersection12.Z()) / TMath::Sqrt(12.);
      } else {
        fin_intersection11 = hit->GetPosition();
        phi11 = hit->GetPhi();

        z = fin_intersection11.Z();
        phi = phi11;
        sigma = 0.1;
      }

      fFitter->SetPointToFit(phi, z, sigma); // CHECK the error?
      if (fDisplayOn) {
        display->cd(4);
        if (phi12 != -999) {
          TLine *linezphi = new TLine(phi11, fin_intersection11.Z(), phi12, fin_intersection12.Z());
          linezphi->SetLineStyle(1);
          linezphi->Draw("SAME");
          TMarker *mrkzphi1 = new TMarker(phi11, fin_intersection11.Z(), 20);
          mrkzphi1->SetMarkerColor(kBlue - 9);
          mrkzphi1->Draw("SAME");
          TMarker *mrkzphi2 = new TMarker(phi12, fin_intersection12.Z(), 20);
          mrkzphi2->SetMarkerColor(kMagenta - 7);
          mrkzphi2->Draw("SAME");
        } else {
          TMarker *mrkzphi1 = new TMarker(phi11, fin_intersection11.Z(), 20);
          mrkzphi1->SetMarkerColor(kGreen);
          mrkzphi1->Draw("SAME");
        }

        TMarker *mrkzphi = new TMarker(phi, z, 20);
        mrkzphi->SetMarkerColor(kOrange);
        mrkzphi->Draw("SAME");

        display->Update();
        display->Modified();
      }
    }
    double fitm3b, fitq3b;
    fFitter->StraightLineFit(fitm3b, fitq3b);
    // ==========================================
    // at low fitm3 the fit prevails over legendre CHECK
    // this is temporary since it might be different
    // for a different channel
    if (fabs(fitm3b) > 1e-10) {
      if (fabs(fitm3b) > 0.1) {
        fitm3 = fitm3b;
        fitq3 = fitq3b;
      }
    }

    if (fDisplayOn) {
      display->cd(4);
      TLine *line = new TLine(0, fitq3, 360, 360 * fitm3 + fitq3);
      line->SetLineColor(kOrange);
      line->Draw("SAME");

      display->Update();
      display->Modified();
      char goOnChar;
      cin >> goOnChar;
    }

    // choose the fin_intersection of the skew
    PndTrkSkewHitList skewhitlist2;
    for (int ihit = 0; ihit < skewhitlist.GetNofHits(); ihit++) {
      hit = (PndTrkHit *)skewhitlist.GetHit(ihit);
      if (!hit)
        continue;
      if (hit->IsSttSkew() == kFALSE) {

        // check if already discarded in the phi.z procedure
        std::pair<int, int> thispair(hit->GetHitID(), hit->GetDetectorID());
        std::vector<std::pair<int, int>>::iterator itr = dontuse.begin();
        itr = std::find(dontuse.begin(), dontuse.end(), thispair);
        if (itr != dontuse.end())
          continue;

        double zdistance = fabs(hit->GetPosition().Z() - fitm3 * hit->GetPhi() - fitq3);
        if (zdistance < 5)
          skewhitlist2.AddHit(hit);
        else
          dontuse.push_back(thispair);
      } else {
        TVector3 fin_intersection1 = ((PndTrkSkewHit *)hit)->GetIntersection1();
        TVector3 fin_intersection2 = ((PndTrkSkewHit *)hit)->GetIntersection2();
        double phi1 = ((PndTrkSkewHit *)hit)->GetPhi1();
        double phi2 = ((PndTrkSkewHit *)hit)->GetPhi2();

        double dist1 = fabs(fitm3 * phi1 - fin_intersection1.Z() + fitq3) / TMath::Sqrt(fitm3 * fitm3 + 1); // CHECK ortho distance or not?
        double dist2 = fabs(fitm3 * phi2 - fin_intersection2.Z() + fitq3) / TMath::Sqrt(fitm3 * fitm3 + 1); // CHECK    "           "

        distance = 1000;
        dist1 < dist2 ? (distance = dist1, hit->SetPosition(fin_intersection1), hit->SetPhi(phi1)) : (distance = dist2, hit->SetPosition(fin_intersection2), hit->SetPhi(phi2));

        if (distance < 10)
          skewhitlist2.AddHit(hit);
      }
    }

    // further cleaning
    PndTrkSkewHitList skewhitlist3;
    double mvdmeandist = 0, sttmeandist = 0, gemmeandist = 0, scitmeandist = 0;
    int mvdcount = 0, sttcount = 0, gemcount = 0, scitcount = 0;
    for (int ihit = 0; ihit < skewhitlist2.GetNofHits(); ihit++) {
      hit = (PndTrkHit *)skewhitlist2.GetHit(ihit);
      if (!hit)
        continue;
      double zdistance = fabs(hit->GetPosition().Z() - fitm3 * hit->GetPhi() - fitq3);
      if (hit->IsMvd()) {
        mvdmeandist += zdistance;
        mvdcount++;
      } else if (hit->IsStt()) {
        sttmeandist += zdistance;
        sttcount++;
      } else if (hit->IsGem()) {
        gemmeandist += zdistance;
        gemcount++;
      } else if (hit->IsSciTil()) {
        scitmeandist += zdistance;
        scitcount++;
      }
    }
    if (mvdcount > 0)
      mvdmeandist /= mvdcount;
    if (sttcount > 0)
      sttmeandist /= sttcount;
    if (gemcount > 0)
      gemmeandist /= gemcount;
    if (scitcount > 0)
      scitmeandist /= scitcount;

    for (int ihit = 0; ihit < skewhitlist2.GetNofHits(); ihit++) {
      hit = (PndTrkHit *)skewhitlist2.GetHit(ihit);
      if (!hit)
        continue;
      double zdistance = fabs(hit->GetPosition().Z() - fitm3 * hit->GetPhi() - fitq3);
      if (hit->IsMvd()) {
        //	cout << "mvd " << mvdmeandist << " " << zdistance << " " << mvdmeandist - zdistance << endl;
        if (fabs(mvdmeandist - zdistance) < 2)
          skewhitlist3.AddHit(hit);
      } else if (hit->IsStt()) {
        // cout << "stt " << sttmeandist << " " << zdistance << " " << sttmeandist - zdistance << endl;
        if (fabs(sttmeandist - zdistance) < 10)
          skewhitlist3.AddHit(hit);
      } else if (hit->IsGem()) {
        // cout << "gem " << gemmeandist << " " << zdistance << " " << gemmeandist - zdistance << endl;
        if (fabs(gemmeandist - zdistance) < 20)
          skewhitlist3.AddHit(hit); // CHECK
      } else if (hit->IsSciTil()) {
        // cout << "scit " << scitmeandist << " " << zdistance << " " << scitmeandist - zdistance << endl;
        if (fabs(scitmeandist - zdistance) < 20)
          skewhitlist3.AddHit(hit); // CHECK
      }
    }

    // forget this track!
    if (skewhitlist3.GetNofHits() == 0)
      continue;

    if (fDisplayOn) {
      display->cd(4);

      cout << "final z " << skewhitlist3.GetNofHits() << endl;
      for (int ihit = 0; ihit < skewhitlist3.GetNofHits(); ihit++) {
        hit = (PndTrkHit *)skewhitlist3.GetHit(ihit);

        cout << "hitid " << hit->GetHitID() << " " << hit->GetDetectorID() << endl;
        display->cd(4);
        TMarker *mrkzphi = new TMarker(hit->GetPhi(), hit->GetPosition().Z(), 20);
        mrkzphi->SetMarkerColor(kMagenta);
        mrkzphi->Draw("SAME");
        display->Update();
        display->Modified();
      }

      char goOnChar;
      cin >> goOnChar;
    }

    // -------- refit with a line
    fFitter->Reset();

    // see the hits
    for (int ihit = 0; ihit < skewhitlist3.GetNofHits(); ihit++) {
      hit = skewhitlist3.GetHit(ihit);
      if (!hit)
        continue;
      TVector3 position = hit->GetPosition();
      double phi = finaltrack.ComputePhi(position);
      fFitter->SetPointToFit(phi, position.Z(), 0.1); // CHECK the error?
      //      cout << "final point " << phi << " " << position.Z() << endl;
      if (fDisplayOn) {
        display->cd(4);
        TMarker *mrkzphi = new TMarker(phi, position.Z(), 20);
        mrkzphi->SetMarkerColor(kBlue);
        if (hit->IsStt() == kTRUE)
          mrkzphi->SetMarkerColor(kYellow);
        mrkzphi->Draw("SAME");
        display->Update();
        display->Modified();
      }
    }

    double fitm4, fitq4;
    fFitter->StraightLineFit(fitm4, fitq4);

    if (fDisplayOn) {
      display->cd(4);
      TLine *line = new TLine(0, fitq4, 360, 360 * fitm4 + fitq4);
      line->SetLineColor(4);
      line->Draw("SAME");
      display->Update();
      display->Modified();
      char goOnChar;
      cin >> goOnChar;
    }

    // finalize the cluster
    for (int ihit = 0; ihit < skewhitlist2.GetNofHits(); ihit++) {
      hit = skewhitlist2.GetHit(ihit);
      if (fFinalCluster->DoesContain(hit) == kFALSE)
        fFinalCluster->AddHit((PndTrkHit *)hit);
    }

    // sorting
    for (int ihit = 0; ihit < fFinalCluster->GetNofHits(); ihit++) {
      hit = fFinalCluster->GetHit(ihit);
      hit->SetSortVariable(hit->GetPosition().Perp());
    }
    fFinalCluster->Sort();
    // sorting
    for (int ihit = 0; ihit < fFinalCluster->GetNofHits(); ihit++) {
      hit = fFinalCluster->GetHit(ihit);
    }

    //     fDisplayOn = kFALSE;

    // compute charge
    finaltrack.ComputeCharge();

    // last two parameters in real plane
    double tanl = -finaltrack.GetCharge() * fitm4 * (180. / TMath::Pi()) / R2;
    double z0 = fitq4;

    // PndTrkTrack *finaltrack2 = new PndTrkTrack(finalcluster, xc2, yc2, R2);
    finaltrack.SetCluster(fFinalCluster);
    finaltrack.SetZ0(z0);
    finaltrack.SetTanL(tanl);
    finaltrack.SetCluster(fFinalCluster);
    finaltrack.SetCenter(xc2, yc2);
    finaltrack.SetRadius(R2);

    //     cout << "TRACK to TRACKLIST " << fTrackList->GetNofTracks() << endl;
    //     cout << "X, Y, R " << finaltrack.GetCenter().X() << " " << finaltrack.GetCenter().Y() << " " << finaltrack.GetRadius() << endl;
    //     cout << "Z0, TANL " << finaltrack.GetZ0() << " " << finaltrack.GetTanL() << endl;
    //     cout << "CHARGE " <<  finaltrack.GetCharge() << endl;

    // clusterlist.AddCluster(finalcluster);
    fTrackList->AddTrack(&finaltrack);
  }

  //   fDisplayOn = kTRUE;

  //   cout << "TRACKS IN TRACKLIST " << fTrackList->GetNofTracks() << endl;
  if (fDisplayOn) {
    // char goOnChar; //[R.K. 01/2017] unused variable
    display->cd(1);
    for (int jtrk = 0; jtrk < fTrackList->GetNofTracks(); jtrk++) {
      PndTrkTrack *track = fTrackList->GetTrack(jtrk);

      track->Draw(kRed);
      display->Update();
      display->Modified();

      //      cout << "TRACK " << jtrk << endl;
      //       cout << "X, Y, R " << track->GetCenter().X() << " " << track->GetCenter().Y() << " " << track->GetRadius() << endl;
      //       cout << "Z0, TANL " << track->GetZ0() << " " << track->GetTanL() << endl;
      //       cout << "CHARGE " <<  track->GetCharge() << endl;
      //       cin >> goOnChar;
    }
  }

  // ====== MERGIN ==========================
  PndTrkTrackList *mergedtracklist = new PndTrkTrackList();
  // choose what to merge
  std::vector<int> merged;
  merged.resize(fTrackList->GetNofTracks());
  for (int itrk = 0; itrk < fTrackList->GetNofTracks(); itrk++)
    merged[itrk] = 0;
  std::map<int, std::vector<int>> mergingtracks;

  for (int itrk = 0; itrk < fTrackList->GetNofTracks(); itrk++) {
    if (merged[itrk] == 1)
      continue;
    PndTrkTrack *tracki = fTrackList->GetTrack(itrk);
    PndTrkCluster clusteri = tracki->GetCluster();
    std::vector<int> mtracks;
    for (int jtrk = itrk + 1; jtrk < fTrackList->GetNofTracks(); jtrk++) {
      if (merged[jtrk] == 1)
        continue;
      PndTrkTrack *trackj = fTrackList->GetTrack(jtrk);
      PndTrkCluster clusterj = trackj->GetCluster();
      if (clusterj.SharedAt(&clusteri, 0.70) == kTRUE) {
        mtracks.push_back(jtrk);
        //	cout << "track " << itrk << " needs merging with track " << jtrk << endl;
        merged[itrk] = 1; // CHECK save time!
        merged[jtrk] = 1;
      }
    }
    if (mtracks.size() == 0)
      mtracks.push_back(-1);
    mergingtracks.insert(std::pair<int, std::vector<int>>(itrk, mtracks));
    //   cout << "pair of " << itrk << " and " << mtracks.size() << endl;
  }

  //  cout << "merging " << mergingtracks.size() << endl;
  // do the actual merging
  std::map<int, std::vector<int>>::iterator it;
  for (int itrk = 0; itrk < fTrackList->GetNofTracks(); itrk++) {
    PndTrkTrack *tracki = fTrackList->GetTrack(itrk);
    PndTrkCluster clusteri = tracki->GetCluster();
    it = mergingtracks.find(itrk);
    if (it == mergingtracks.end())
      continue;

    std::vector<int> mtracks = it->second;
    // if it has merged tracks
    if (mtracks[0] != -1) {
      //  cout << it->first << "/" << mergingtracks.size() << endl; // " last fit " <<  clusteri.GetNofHits() << endl;
      // cout << "i " << clusteri.GetNofHits() << endl;
      for (size_t ktrk = 0; ktrk < mtracks.size(); ktrk++) {
        int jtrk = mtracks[ktrk];
        PndTrkTrack *trackj = fTrackList->GetTrack(jtrk);
        PndTrkCluster clusterj = trackj->GetCluster();
        clusteri.MergeTo(&clusterj);
      }
    }

    //--------------------------------------
    // to conformal
    fRefHit = clusteri.GetHit(clusteri.GetNofHits() - 1);
    fConformalHitList->Reset();
    double delta, trasl[2];
    ComputeTraAndRot(fRefHit, delta, trasl);
    conform->SetOrigin(trasl[0], trasl[1], delta);
    fConformalHitList->SetConformalTransform(conform);
    // cout << "conformal hits " << fConformalHitList->GetNofHits() << endl;

    // fill conformal hits
    int nofconfhits = FillConformalHitList(&clusteri);

    // compute conformal plane extremities ---------------------------- this must go to a fctn CHECK
    fUmin = 1000, fVmin = 1000, fRmin = 1000;
    fUmax = -1000, fVmax = -1000, fRmax = -1000;
    double rc_of_min, rc_of_max;

    for (int jhit = 0; jhit < fConformalHitList->GetNofHits(); jhit++) {
      PndTrkConformalHit *chit = fConformalHitList->GetHit(jhit);
      double u = chit->GetU();
      double v = chit->GetV();
      double rc = chit->GetIsochrone();
      if (rc < 0)
        rc = 0;
      // cout << "conf hit " << jhit << " u, v " << u << " " << v << " " << rc << endl;
      u - rc < fUmin ? fUmin = u - rc : fUmin;
      v - rc < fVmin ? fVmin = v - rc : fVmin;
      u + rc > fUmax ? fUmax = u + rc : fUmax;
      v + rc > fVmax ? fVmax = v + rc : fVmax;

      double theta1 = TMath::ATan2(v, u);
      double theta2 = theta1 + TMath::Pi();

      double r1 = u * TMath::Cos(theta1) + v * TMath::Sin(theta1);
      double r2 = u * TMath::Cos(theta2) + v * TMath::Sin(theta2);

      double rimin, rimax;
      r1 < r2 ? (rimin = r1, rimax = r2) : (rimin = r2, rimax = r1);

      rimin < fRmin ? (rc_of_min = rc, fRmin = rimin) : fRmin;
      rimax > fRmax ? (rc_of_max = rc, fRmax = rimax) : fRmax;
    }

    fRmin -= rc_of_min;
    fRmax += rc_of_max;

    // to square the conformal plane
    double du = fUmax - fUmin;
    double dv = fVmax - fVmin;
    double delt = 0.5 * fabs(dv - du);
    du < dv ? (fUmin -= delt, fUmax += delt) : (fVmin -= delt, fVmax += delt);

    if (fDisplayOn) {
      DrawGeometryConf(fUmin, fUmax, fVmin, fVmax);
      // ----------------------------------------------------------------------
      for (int ihit = 0; ihit < nofconfhits; ihit++) {
        PndTrkConformalHit *chit = fConformalHitList->GetHit(ihit);
        if (fDisplayOn) {
          chit->Draw(kRed);
        }
      }
    }

    // last fit
    double xc2 = tracki->GetCenter().X();
    double yc2 = tracki->GetCenter().Y();
    double R2 = tracki->GetRadius();
    double fitm, fitq;

    double xc3, yc3, R3;
    // line or parabola
    if (1 == 1) {
      bool fitting = AnalyticalFit(&clusteri, xc2, yc2, R2, fitm, fitq);
      if (fitting == kFALSE)
        continue;
      FromConformalToRealTrack(fitm, fitq, xc3, yc3, R3);
      if (fDisplayOn) {
        // char goOnChar; //[R.K. 01/2017] unused variable
        display->cd(2);
        TLine *line = new TLine(-10, -10 * fitm + fitq, 10, 10 * fitm + fitq);
        line->SetLineColor(kMagenta);
        line->Draw("SAME");
      }
    } else {
      double fita, fitb, fitc, epsilon;
      bool fitting = AnalyticalParabolaFit(&clusteri, xc2, yc2, R2, fita, fitb, fitc, epsilon);
      if (fitting == kFALSE)
        continue;
      FromConformalToRealTrackParabola(fita, fitb, fitc, xc3, yc3, R3, epsilon);
    }

    if (fDisplayOn) {
      char goOnChar;
      Refresh();

      display->cd(1);
      clusteri.LightUp();
      TArc *arcm = new TArc(xc3, yc3, R3);
      arcm->SetFillStyle(0);
      arcm->SetLineColor(kMagenta);
      arcm->Draw("SAME");

      display->Update();
      display->Modified();
      cin >> goOnChar;
    }

    tracki->SetCluster(&clusteri);
    tracki->SetCenter(xc3, yc3);
    tracki->SetRadius(R3);

    mergedtracklist->AddTrack(tracki);
  }

  //   cout << "TRACKS IN MERGED TRACK LIST " << mergedtracklist->GetNofTracks() << endl;
  if (fDisplayOn) {
    // char goOnChar; //[R.K. 01/2017] unused variable
    display->cd(1);
    for (int jtrk = 0; jtrk < mergedtracklist->GetNofTracks(); jtrk++) {
      PndTrkTrack *track = mergedtracklist->GetTrack(jtrk);

      track->Draw(kBlue);
      display->Update();
      display->Modified();

      //      cout << "MERGED TRACK " << jtrk << endl;
      //       cout << "X, Y, R " << track->GetCenter().X() << " " << track->GetCenter().Y() << " " << track->GetRadius() << endl;
      //       cout << "Z0, TANL " << track->GetZ0() << " " << track->GetTanL() << endl;
      //       cout << "CHARGE " <<  track->GetCharge() << endl;
      //       cin >> goOnChar;
    }
  }

  // =============================================
  // CLEANING
  // rms
  PndTrkTrackList *cleanedtracklist = new PndTrkTrackList();
  for (int itrk = 0; itrk < mergedtracklist->GetNofTracks(); itrk++) {
    PndTrkTrack *track = mergedtracklist->GetTrack(itrk);
    double xc3 = track->GetCenter().X();
    double yc3 = track->GetCenter().Y();
    double R3 = track->GetRadius();

    bool vote = true;
    // CHI2 ___________________
    double gap[5] = {0, 0, 0, 0, 0};
    int gapcounter[5] = {0, 0, 0, 0, 0};
    PndTrkCluster clusteri = track->GetCluster();

    // check if it is a long track
    hit = clusteri.GetHit(0);
    //    cout << "FIRST LAY " << hit->GetDetectorID() << endl;
    if (hit->GetDetectorID() == FairRootManager::Instance()->GetBranchId(fSttBranch)) {
      PndSttTube *tube0 = (PndSttTube *)fTubeArray->At(hit->GetTubeID());
      //       cout << tube0->GetLayerID() << endl;
      if (tube0->GetLayerID() > 4)
        continue; // CHECK
    }
    hit = clusteri.GetHit(clusteri.GetNofHits() - 1);
    //    cout << "LAST LAY " << hit->GetDetectorID() << endl;
    if (hit->GetDetectorID() == FairRootManager::Instance()->GetBranchId(fSttBranch)) {
      PndSttTube *tubeN = (PndSttTube *)fTubeArray->At(hit->GetTubeID());
      //       cout << tubeN->GetLayerID() << endl;
      if (tubeN->GetLayerID() < 16)
        continue; // CHECK
    }

    for (int ihit = 0; ihit < clusteri.GetNofHits(); ihit++) {
      hit = clusteri.GetHit(ihit);
      int detID = hit->GetDetectorID();
      TVector3 pos = hit->GetPosition();

      // mvd pix
      if (detID == FairRootManager::Instance()->GetBranchId(fMvdPixelBranch)) {
        gap[0] += fabs(TMath::Sqrt((pos.X() - xc3) * (pos.X() - xc3) + (pos.Y() - yc3) * (pos.Y() - yc3)) - R3);
        gapcounter[0]++;
      }
      // mvd str
      else if (detID == FairRootManager::Instance()->GetBranchId(fMvdStripBranch)) {
        gap[1] += fabs(TMath::Sqrt((pos.X() - xc3) * (pos.X() - xc3) + (pos.Y() - yc3) * (pos.Y() - yc3)) - R3);
        gapcounter[1]++;
      }
      // stt
      else if (detID == FairRootManager::Instance()->GetBranchId(fSttBranch)) {
        gap[2] += fabs(TMath::Sqrt((pos.X() - xc3) * (pos.X() - xc3) + (pos.Y() - yc3) * (pos.Y() - yc3)) - R3) - hit->GetIsochrone();
        gapcounter[2]++;
      }
      // scitil;
      else if (detID == FairRootManager::Instance()->GetBranchId(fSciTBranch)) {
        gap[3] += fabs(TMath::Sqrt((pos.X() - xc3) * (pos.X() - xc3) + (pos.Y() - yc3) * (pos.Y() - yc3)) - R3);
        gapcounter[3]++;
      }
      // gem
      else if (detID == FairRootManager::Instance()->GetBranchId(fGemBranch)) {
        gap[4] += fabs(TMath::Sqrt((pos.X() - xc3) * (pos.X() - xc3) + (pos.Y() - yc3) * (pos.Y() - yc3)) - R3);
        gapcounter[4]++;
      }
    }

    // give large cuts CHECK
    for (int igap = 0; igap < 5; igap++) {
      if (gapcounter[igap] > 0) {
        gap[igap] /= gapcounter[igap];
        gap[igap] = fabs(gap[igap]);
      }
    }
    if (gap[0] > 2)
      vote = false; // mvd pix
    if (gap[1] > 2)
      vote = false; // mvd str
    if (gap[2] > 10)
      vote = false; // stt
    if (gap[3] > 2)
      vote = false; // scit
    if (gap[4] > 2)
      vote = false; // gem
    //    cout << "VOTE " << vote << " " << gap[0] << " " << gap[1]  << " " << gap[2] << " " << gap[3] << " " << gap[4] << endl;

    if (vote == true)
      cleanedtracklist->AddTrack(track);
  }
  delete mergedtracklist;
  //   cout << "TRACKS IN CLEANED TRACK LIST " << cleanedtracklist->GetNofTracks() << endl;

  // at last, add the INDIVISIBLES
  PndTrkTrackList *indivtracklist = new PndTrkTrackList();
  for (int itrk = 0; itrk < cleanedtracklist->GetNofTracks(); itrk++) {
    PndTrkTrack *tracki = cleanedtracklist->GetTrack(itrk);
    PndTrkCluster clusteri = tracki->GetCluster();
    if (fDisplayOn) {
      char goOnChar;
      display->cd(1);
      Refresh();
      clusteri.Draw(kRed);
      display->Update();
      display->Modified();
      cin >> goOnChar;
    }
    for (int ihit = 0; ihit < clusteri.GetNofHits(); ihit++) {
      hit = clusteri.GetHit(ihit);
      if (hit->GetDetectorID() != FairRootManager::Instance()->GetBranchId(fSttBranch))
        continue;
      TObjArray indiv2 = fHitMap->GetIndivisiblesToHit(hit);

      if (fDisplayOn) {
        hit->Draw(kBlue);
        display->Update();
        display->Modified();
      }
      for (int jhit = 0; jhit < indiv2.GetEntriesFast(); jhit++) {
        PndTrkHit *hit2 = (PndTrkHit *)indiv2.At(jhit);
        if (fDisplayOn) {
          hit2->Draw(kGreen);
          display->Update();
          display->Modified();
        }
        hit2->SetSortVariable(hit2->GetPosition().Perp());
        if (!clusteri.DoesContain(hit2)) {

          // double distance_hit_center = (hit2->GetPosition().XYvector() - TVector2(tracki->GetCenter().X(), tracki->GetCenter().Y())).Mod(); //[R.K. 01/2017] unused variable?
          // double recoiso = fabs(distance_hit_center - tracki->GetRadius()); //[R.K. 01/2017] unused variable
          //	   if(recoiso < 0.5)
          clusteri.AddHit(hit2);
        }
      }
      if (fDisplayOn) {
        // char goOnChar; //[R.K. 01/2017] unused variable
        //	cin >> goOnChar ;
      }
    }
    clusteri.Sort();
    tracki->SetCluster(&clusteri);
    if (fDisplayOn) {
      char goOnChar;
      display->cd(1);
      Refresh();
      clusteri.Draw(kMagenta);
      tracki->Draw();
      display->Update();
      display->Modified();
      cin >> goOnChar;
    }
    indivtracklist->AddTrack(tracki);
  }
  delete cleanedtracklist;

  for (int itrk = 0; itrk < indivtracklist->GetNofTracks(); itrk++) {
    PndTrkTrack *tracki = indivtracklist->GetTrack(itrk);
    fTrackList->AddTrack(tracki);
  }
  delete indivtracklist;

  if (fDisplayOn) {
    // char goOnChar; //[R.K. 01/2017] unused variable
    display->cd(1);
    Refresh();
  }
  //
  //   fDisplayOn = kTRUE;

  //--------------------------------------
  // PndTrkTrack --> PndTrack
  for (int itrk = 0; itrk < fTrackList->GetNofTracks(); itrk++) {
    PndTrkTrack *track = fTrackList->GetTrack(itrk);
    //     cout << "- ----------------------------------- track red " << track->GetRadius() << endl;

    // CHECK CHECK CHECK CHECK CHECK CHECK CHECK CHECK CHECK CHECK CHECK CHECK CHECK CHECK
    //    temporary hacking not to save the SciTil to PndTrackCand CHECK CHECK CHECK CHECK
    // CHECK CHECK CHECK CHECK CHECK CHECK CHECK CHECK CHECK CHECK CHECK CHECK CHECK CHECK
    PndTrkCluster tempcluster = track->GetCluster();
    PndTrkCluster tempcluster2;
    for (int ihit = 0; ihit < tempcluster.GetNofHits(); ihit++) {
      hit = tempcluster.GetHit(ihit);
      if (hit->IsSciTil() == kTRUE)
        continue;
      tempcluster2.AddHit(hit);
    }
    track->SetCluster(&tempcluster2);
    //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

    PndTrack theTrack = track->ConvertToPndTrack();

    TClonesArray &clref1 = *fTrackArray;
    Int_t size = clref1.GetEntriesFast();
    PndTrack *outputtrack = new (clref1[size]) PndTrack(theTrack.GetParamFirst(), theTrack.GetParamLast(), theTrack.GetTrackCand());
    outputtrack->SetFlag(111);
    if (fabs(outputtrack->GetParamFirst().GetMomentum().Z()) < 1e-10)
      outputtrack->SetFlag(-111);

    TClonesArray &clref2 = *fTrackCandArray;
    size = clref2.GetEntriesFast();
    PndTrackCand *outputtrackcand = new (clref2[size]) PndTrackCand(theTrack.GetTrackCand());

    TClonesArray &clref3 = *fTrkTrackArray;
    size = clref3.GetEntriesFast();
    new (clref3[size]) PndTrkTrack(*track); // PndTrkTrack *outputtrktrack =  //[R.K.03/2017] unused variable

    //    cout << "\033[1;31m RECO R " << outputtrktrack->GetRadius() <<  ", " << outputtrack->GetParamFirst().GetMomentum().Perp() / 0.006 << "\033[0m" << endl;
    //         cout << "MOM FIRST: TOT, PT, PL " << outputtrack->GetParamFirst().GetMomentum().Mag() << " " << outputtrack->GetParamFirst().GetMomentum().Perp() << " " <<
    //         outputtrack->GetParamFirst().GetMomentum().Z() << endl;
    //     cout << "MOM LAST: TOT, PT, PL " << outputtrack->GetParamLast().GetMomentum().Mag() << " " << outputtrack->GetParamLast().GetMomentum().Perp() << " " <<
    //     outputtrack->GetParamLast().GetMomentum().Z() << endl;

    if (fDisplayOn) {
      char goOnChar;
      Refresh();
      display->cd(1);
      track->Draw(kRed);
      track->GetCluster().LightUp();
      display->Update();
      display->Modified();

      cout << "TRACK " << itrk << endl;
      cout << "MOM FIRST: TOT, PT, PL " << outputtrack->GetParamFirst().GetMomentum().Mag() << " " << outputtrack->GetParamFirst().GetMomentum().Perp() << " "
           << outputtrack->GetParamFirst().GetMomentum().Z() << " nofhits " << outputtrackcand->GetNHits() << endl;
      cout << "X, Y, R " << track->GetCenter().X() << " " << track->GetCenter().Y() << " " << track->GetRadius() << endl;
      cout << "Z0, TANL " << track->GetZ0() << " " << track->GetTanL() << endl;
      cout << "CHARGE " << track->GetCharge() << endl;
      cin >> goOnChar;
    }
  }

  int noflongtracks = fTrackArray->GetEntriesFast();

  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // FFFFF W   W DDD
  // F     W   W D  D
  // FFFF  W W W D   D
  // F     W W W D   D
  // F      W W  DDDD
  //  fDisplayOn = kFALSE;
  //   fDisplayOn = kTRUE;
  // set unusable the hits assigned to long tracks ....................................
  for (int itrk = fNofPrimaries; itrk < fTrackArray->GetEntriesFast(); itrk++) {
    PndTrack *trk = (PndTrack *)fTrackArray->At(itrk);
    PndTrackCand *cand = trk->GetTrackCandPtr();
    if (!cand) {
      cout << "ERROR track " << itrk << " has no candidate association" << endl;
      continue;
    }
    //    cout << fNofPrimaries << " track " << trk << " cand " << cand << endl;
    for (size_t ihit = 0; ihit < cand->GetNHits(); ihit++) {
      PndTrackCandHit candhit = cand->GetSortedHit(ihit);
      Int_t hitId = candhit.GetHitId();
      Int_t detId = candhit.GetDetId();

      if (detId == FairRootManager::Instance()->GetBranchId(fMvdPixelBranch)) {
        hit = mvdpixhitlist->GetHitByID(hitId);
      }
      // mvd str
      else if (detId == FairRootManager::Instance()->GetBranchId(fMvdStripBranch)) {
        hit = mvdstrhitlist->GetHitByID(hitId);
      }
      // stt
      else if (detId == FairRootManager::Instance()->GetBranchId(fSttBranch)) {
        hit = stthitlist->GetHitByID(hitId);
      }
      // scitil;
      else if (detId == FairRootManager::Instance()->GetBranchId(fSciTBranch)) {
        hit = scithitlist->GetHitByID(hitId);
      }
      // gem
      else if (detId == FairRootManager::Instance()->GetBranchId(fGemBranch)) {
        hit = gemhitlist->GetHitByID(hitId);
      }
      hit->SetUsedFlag(true);
    }
  }

  if (fDisplayOn) {
    Refresh();

    for (int itrk = 0; itrk < fTrackArray->GetEntriesFast(); itrk++) {
      PndTrack *trk = (PndTrack *)fTrackArray->At(itrk);
      PndTrkTrack track(trk);
      track.Draw();
    }

    char goOnChar;
    display->Update();
    display->Modified();
    cin >> goOnChar;
  }
  // ........................................................................................

  // Lets start from the GEM stations
  PndTrkCluster gemcluster;

  // sort by layer id 1, 2, 3, 4, 5, 6
  int nofgemhits_on_layer[NOFLAYERS] = {0, 0, 0, 0, 0, 0}; // CHECK
  for (int ihit = 0; ihit < gemhitlist->GetNofHits(); ihit++) {
    hit = gemhitlist->GetHit(ihit);
    if (hit->IsUsed() == kTRUE)
      continue;
    int layerid = hit->GetSensorID();
    hit->SetSortVariable(layerid);
    gemcluster.AddHit(hit);
    nofgemhits_on_layer[layerid]++;
  }
  gemcluster.Sort();

  for (int ihit = 0; ihit < gemcluster.GetNofHits(); ihit++) {
    PndTrkHit *hiti = gemcluster.GetHit(ihit);
    hiti->SetUsedFlag(kFALSE);
  }

  PndTrkClusterList newclusterlist;

  // make tracklets out of the gem hits ..............................
  //  cout << "GEM CLUSTER " << gemcluster.GetNofHits() << endl;
  for (int ihit = 0; ihit < gemcluster.GetNofHits(); ihit++) {
    PndTrkHit *hiti = gemcluster.GetHit(ihit);
    if (hiti->IsUsed() == kTRUE)
      continue;
    PndTrkCluster cluster;
    cluster.AddHit(hiti);

    if (fDisplayOn) {
      Refresh();
      hiti->Draw(kRed);
      // char goOnChar; //[R.K. 01/2017] unused variable
      display->Update();
      display->Modified();
      //  cin >> goOnChar;
    }

    hiti->SetUsedFlag(kTRUE);
    // int layerid = hiti->GetSensorID(); //[R.K. 01/2017] unused variable
    PndTrkHit *tmphiti = hiti;

    bool goOn = true;
    PndTrkHit *tmphitj = hiti;

    while (goOn == true) {

      double tmpdistance = 1000;
      for (int jhit = 0; jhit < gemcluster.GetNofHits(); jhit++) {
        PndTrkHit *hitj = gemcluster.GetHit(jhit);
        if (hitj->IsUsed() == kTRUE)
          continue;
        int layerjd = hitj->GetSensorID();

        if (layerjd != tmphitj->GetSensorID() + 1)
          continue;

        double distance = hitj->GetXYDistance(tmphiti);
        if (distance < tmpdistance) {
          tmphitj = hitj;
          tmpdistance = distance;
          //	  cout << "distance " << distance << " " << tmpdistance << endl;
        }
      }

      if (tmpdistance < 20) {
        cluster.AddHit(tmphitj);
        tmphitj->SetUsedFlag(kTRUE);
        tmphiti = tmphitj;

        if (fDisplayOn) {
          Refresh();
          tmphitj->Draw(kBlue);
          // char goOnChar; //[R.K. 01/2017] unused variable
          display->Update();
          display->Modified();
          //  cin >> goOnChar;
        }
      } else
        goOn = false;
    }

    //    cout << "CLUSTER " << cluster.GetNofHits() << endl;
    if (cluster.GetNofHits() >= 3) {

      // --------------------------------------------------------------
      // FIT THE CLUSTER
      //
      if (fDisplayOn) {
        Refresh();
        cluster.LightUp();
        char goOnChar;
        display->Update();
        display->Modified();
        cin >> goOnChar;
      }

      /**
   TVector3 stat1(0, 0, 0), stat2(0, 0, 0), stat3(0, 0, 0);
   int nstat1 = 0, nstat2 = 0, nstat3 = 0;
   for(int jhit = 0; jhit < cluster.GetNofHits(); jhit++) {
   hit = cluster.GetHit(jhit);
   int sensid = hit->GetSensorID();

   if(sensid <= 1) { stat1 += hit->GetPosition(); nstat1++; }
   else if(sensid <= 3) { stat2 += hit->GetPosition(); nstat2++; }
   else if(sensid <= 5) { stat3 += hit->GetPosition(); nstat3++; }
   }
   if(nstat1 == 0 || nstat2 == 0 || nstat3 == 0) continue;

   stat1 *= (1./nstat1);
   stat2 *= (1./nstat2);
   stat3 *= (1./nstat3);

   double alpha = 0.5 * (stat1.X() * stat1.X() - stat2.X() * stat2.X() + stat1.Y() * stat1.Y() - stat2.Y() * stat2.Y()) / (stat1.X() - stat2.X());
   double beta = (stat1.Y() - stat2.Y())/(stat1.X() - stat2.X());

   double yc0 = (alpha * (stat1.X() - stat3.X()) - 0.5 * (stat1.Y() * stat1.Y() - stat3.Y() * stat3.Y()) - 0.5 * (stat1.X() * stat1.X() - stat3.X() * stat3.X())) / ((stat1.X() -
   stat3.X()) * beta - (stat1.Y() - stat3.Y()));

   double xc0 = alpha - beta * yc0;

   double R0 = TMath::Sqrt((stat1.X() - xc0) * (stat1.X() - xc0) + (stat1.Y() - yc0) * (stat1.Y() - yc0));
      **/

      // ---------------------------------
      fConformalHitList->Clear("C");
      Double_t delta = 0, trasl[2] = {0., 0.};
      PndTrkHit *refhit = nullptr;

      int tmpsensid = 1000;
      for (int jhit = 0; jhit < cluster.GetNofHits(); jhit++) {
        PndTrkHit *hitj = cluster.GetHit(jhit);
        if (hitj->GetSensorID() < tmpsensid) {
          refhit = hitj;
          tmpsensid = hitj->GetSensorID();
        }
      }
      ComputeTraAndRot(refhit, delta, trasl);

      conform->SetOrigin(trasl[0], trasl[1], delta);
      fConformalHitList->SetConformalTransform(conform);

      // fill conformal hits
      Int_t nofconfhits = FillConformalHitList(&cluster);

      // compute conformal plane extremities ----------------------------
      fUmin = 1000, fVmin = 1000, fRmin = 1000;
      fUmax = -1000, fVmax = -1000, fRmax = -1000;
      double rc_of_min, rc_of_max;
      fFitter->Reset();
      for (int jhit = 0; jhit < fConformalHitList->GetNofHits(); jhit++) {
        PndTrkConformalHit *chit = fConformalHitList->GetHit(jhit);
        double u = chit->GetU();
        double v = chit->GetV();
        double rc = chit->GetIsochrone();
        if (rc < 0)
          rc = 0;

        double sigma = 1000.;
        if (hit->IsGem())
          sigma = 0.1 * hit->GetPosition().Z(); // CHECK
        if (TMath::IsNaN(chit->GetPosition().X()))
          continue; // prevents the nan of the ref hit
        fFitter->SetPointToFit(chit->GetPosition().X(), chit->GetPosition().Y(), sigma);

        // cout << "conf hit " << jhit << " u, v " << u << " " << v << " " << rc << endl;
        u - rc < fUmin ? fUmin = u - rc : fUmin;
        v - rc < fVmin ? fVmin = v - rc : fVmin;
        u + rc > fUmax ? fUmax = u + rc : fUmax;
        v + rc > fVmax ? fVmax = v + rc : fVmax;

        double theta1 = TMath::ATan2(v, u);
        double theta2 = theta1 + TMath::Pi();

        double r1 = u * TMath::Cos(theta1) + v * TMath::Sin(theta1);
        double r2 = u * TMath::Cos(theta2) + v * TMath::Sin(theta2);

        double rimin, rimax;
        r1 < r2 ? (rimin = r1, rimax = r2) : (rimin = r2, rimax = r1);

        rimin < fRmin ? (rc_of_min = rc, fRmin = rimin) : fRmin;
        rimax > fRmax ? (rc_of_max = rc, fRmax = rimax) : fRmax;
      }

      fRmin -= rc_of_min;
      fRmax += rc_of_max;

      // to square the conformal plane
      double du = fUmax - fUmin;
      double dv = fVmax - fVmin;
      double delt = fabs(dv - du) / 2.;
      du < dv ? (fUmin -= delt, fUmax += delt) : (fVmin -= delt, fVmax += delt);

      if (fDisplayOn) {
        DrawGeometryConf(fUmin, fUmax, fVmin, fVmax);
        // ----------------------------------------------------------------------
        for (int khit = 0; khit < nofconfhits; khit++) {
          PndTrkConformalHit *chit = fConformalHitList->GetHit(khit);
          chit->Draw(kBlack);
        }
      }

      // ====== REFIT CLUSTER LEGENDRE
      double fitm, fitq;
      fFitter->StraightLineFit(fitm, fitq);

      if (fDisplayOn) {
        RefreshConf();
        DrawGeometryConf(fUmin, fUmax, fVmin, fVmax);
      }

      // from line parameters to center/radius in REAL plane
      Double_t xc, yc, R;
      FromConformalToRealTrack(fitm, fitq, xc, yc, R);
      // ----------------------------------

      if (fDisplayOn) {
        char goOnChar;
        display->cd(1);
        TArc *arcm = new TArc(xc, yc, R);
        arcm->SetFillStyle(0);
        arcm->SetLineColor(3);
        arcm->Draw("SAME");
        display->Update();
        display->Modified();
        cin >> goOnChar;
      }

      newclusterlist.AddCluster(&cluster);

      // put in a list the hits you considered and
      // decided do not belong to the track
      std::vector<std::pair<int, int>> dontuse;

      // GET SECTOR ID ---------------------------------------------
      std::map<int, int> sectorids;
      for (int jhit = 0; jhit < cluster.GetNofHits(); jhit++) {
        hit = cluster.GetHit(jhit);
        if (hit == fRefHit)
          continue;
        int secid = hit->GetSector();

        if (sectorids.find(secid) == sectorids.end())
          sectorids[secid] = 1;
        else
          sectorids[secid]++;
      }

      int tmpsecentries = 0, tmpsec = -1;
      for (size_t isec = 0; isec < sectorids.size(); isec++) {
        if (tmpsecentries < sectorids[isec]) {
          tmpsecentries = sectorids[isec];
          tmpsec = isec;
        }
      }
      int sectorID = tmpsec;
      //    cout << "SECTOR " << sectorID << " " << sectorids.size() << endl;

      // border?
      bool border = false;
      int othersecID = -1;
      for (size_t isec = 0; isec < sectorids.size(); isec++) {
        if (sectorids[isec] > 0 && (int)isec != sectorID) {
          border = true;
          othersecID = isec;
        }
      }
      //      cout << "SECTOR " << sectorID << " " << sectorids.size() << " " << othersecID << " " << border << endl;
      // --------------------------------------------------------------

      // EVALUATE WHICH SECTORS are interesting
      // due to the curvature radius

      double xydistance = TMath::Sqrt(xc * xc + yc * yc);

      //  cout << CTINRADIUS - R << endl;
      bool searchnearby = false;
      if (xydistance < CTINRADIUS - R) {
        // 	cout << "************* INTERNAL TO INNER *****************" << endl;
        continue;
      } else if (xydistance < CTINRADIUS + R && xydistance < CTOUTRADIUS - R) {
        // 	cout << "************* INTERSECTING INNER *****************" << endl;
        searchnearby = true;
      } else if (xydistance < CTINRADIUS + R) {
        // 	cout << "************* INTERSECTING BOTH *****************" << endl;
      } else if (xydistance < CTOUTRADIUS - R) {
        // 	cout << "************* INTERNAL *****************" << endl;
        searchnearby = true;
      } else if (xydistance < CTOUTRADIUS + R) {
        // 	cout << "************* INTERSECTING EXTERNAL *****************" << endl;
        searchnearby = true;
      } else {
        // 	cout << "************* EXTERNAL TO OUTER *****************" << endl;
        continue;
      }

      // does it cross the target pipe?
      bool crossL = false;
      double y1left = 0, y2left = 0;
      double delta1 = R * R - (0.5 * PIPEDIAMETER - xc) * (0.5 * PIPEDIAMETER - xc);
      //      cout << "delta1 " << delta1 << endl;
      if (delta1 == 0) {
        y1left = yc;
        crossL = true;
      } else if (delta1 > 0) {
        y1left = yc + TMath::Sqrt(delta1);
        y2left = yc - TMath::Sqrt(delta1);
        crossL = true;
      }

      bool crossR = false;
      double y1right = 0, y2right = 0;
      double delta2 = R * R - (-0.5 * PIPEDIAMETER - xc) * (-0.5 * PIPEDIAMETER - xc);
      if (delta2 == 0) {
        y1right = yc;
        crossR = true;
      } else if (delta2 > 0) {
        y1right = yc + TMath::Sqrt(delta2);
        y2right = yc - TMath::Sqrt(delta2);
        crossR = true;
      }
      //       cout << "CROSS? " << crossL << " " << crossR << endl;
      bool crossingpipe = false;
      if (crossL && crossR) {
        double ycross = 0.5 * (y1left + y1right);
        if (fabs(ycross) < CTOUTRADIUS && fabs(ycross) > CTINRADIUS)
          crossingpipe = true;
        ycross = 0.5 * (y2left + y2right);
        if (fabs(ycross) < CTOUTRADIUS && fabs(ycross) > CTINRADIUS)
          crossingpipe = true;

        //	cout << ycross << " " << y1left << " " << y1right << " " << y2left << " " << y2right << endl;
      }

      if (crossingpipe) {
        //	cout << "************* CROSSING PIPE *****************" << endl;
        crossingpipe = true;
      }

      // -----------------------------------------
      // === CREATE CLUSTER - taken from PndTrkFinder ---> will be put in a fctn
      // in the same sector or
      // nearby if at the limit of two sectors <<<<<<<<<<<<------------------
      PndTrkCluster cluster2(cluster);

      // create cluster depending on fitting
      for (int jhit = 0; jhit < stthitlist->GetNofHits(); jhit++) {
        hit = stthitlist->GetHit(jhit);
        if (hit->IsUsed() == kTRUE)
          continue;
        if (hit->IsSttParallel() == kFALSE)
          continue;

        if (crossingpipe == true) {
          if (((hit->GetSector() == 0 && sectorID == 5) && !(hit->GetSector() == 2 && sectorID == 3) && !(hit->GetSector() == 5 && sectorID == 0) &&
               !(hit->GetSector() == 3 && sectorID == 2)) &&
              searchnearby == false)
            continue;
        } else if (searchnearby == true) { // search in this sector and +/- 1
          if (fabs(hit->GetSector() - sectorID) > 1 && !(hit->GetSector() == 5 && sectorID == 0) && !(hit->GetSector() == 0 && sectorID == 5))
            continue;
        } else { // search only in this sector and possible othersector
          if (border == false && hit->GetSector() != sectorID)
            continue;
          else if (border == true && (hit->GetSector() != sectorID && hit->GetSector() != othersecID))
            continue;
        }
        // double distance = hit->GetXYDistance(TVector3(xc, yc, 0.)); //[R.K. 01/2017] unused variable

        double distance_hit_center = (hit->GetPosition().XYvector() - TVector2(xc, yc)).Mod();
        double recoiso = fabs(distance_hit_center - R);

        if (recoiso < 3.)
          cluster2.AddHit(hit);
      }

      // GET SECTOR ID ---------------------------------------------
      sectorids.clear();
      for (int jhit = 0; jhit < cluster2.GetNofHits(); jhit++) {
        hit = cluster2.GetHit(jhit);
        if (hit == fRefHit)
          continue;
        int secid = hit->GetSector();

        if (sectorids.find(secid) == sectorids.end())
          sectorids[secid] = 1;
        else
          sectorids[secid]++;
      }

      tmpsecentries = 0, tmpsec = -1;
      for (size_t isec = 0; isec < sectorids.size(); isec++) {
        if (tmpsecentries < sectorids[isec]) {
          tmpsecentries = sectorids[isec];
          tmpsec = isec;
        }
      }
      sectorID = tmpsec;
      //    cout << "SECTOR " << sectorID << " " << sectorids.size() << endl;

      // border?
      border = false;
      othersecID = -1;
      for (size_t isec = 0; isec < sectorids.size(); isec++) {
        if (sectorids[isec] > 0 && (int)isec != sectorID) {
          border = true;
          othersecID = isec;
        }
      }
      //      cout << "SECTOR " << sectorID << " " << sectorids.size() << " " << othersecID << " " << border << endl;
      // --------------------------------------------------------------

      // what about adding the mvd hits now? lets do it!
      // ... and put mvd pixel hits to conformal plane ---------------
      for (int jhit = 0; jhit < mvdpixhitlist->GetNofHits(); jhit++) {
        hit = mvdpixhitlist->GetHit(jhit);
        if (hit->IsUsed() == kTRUE)
          continue;
        if (searchnearby == true) { // search in this sector and +/- 1
          if (fabs(hit->GetSector() - sectorID) > 1 && !(hit->GetSector() == 5 && sectorID == 0) && !(hit->GetSector() == 0 && sectorID == 5))
            continue;
        } else { // search only in this sector and possible othersector
          if (border == false && hit->GetSector() != sectorID)
            continue;
          else if (border == true && (hit->GetSector() != sectorID && hit->GetSector() != othersecID))
            continue;
        }
        //     if(hit->GetSector() != sectorID) continue;
        double distance_hit_center = (hit->GetPosition().XYvector() - TVector2(xc, yc)).Mod();

        double recoiso = fabs(distance_hit_center - R);
        //	cout << "recoiso pix " << recoiso << endl;
        if (recoiso < 3.) {

          // dont want two hits on the same sensor
          int samesensor = false;
          for (int khit = 0; khit < cluster2.GetNofHits(); khit++) {
            PndTrkHit *hitk = cluster2.GetHit(khit);
            if (!hitk->IsMvdPixel())
              continue;
            if (hitk->GetSensorID() != hit->GetSensorID())
              continue;
            samesensor = true;
            double distance_hitk_center = (hitk->GetPosition().XYvector() - TVector2(xc, yc)).Mod();
            double recoisok = fabs(distance_hitk_center - R);
            if (recoiso < recoisok) {
              cluster2.DeleteHitAndCompress(hitk);
              std::pair<int, int> dontpair(hitk->GetHitID(), hitk->GetDetectorID());
              dontuse.push_back(dontpair);
              cluster2.AddHit(hit);
            } else {
              std::pair<int, int> dontpair(hit->GetHitID(), hit->GetDetectorID());
              dontuse.push_back(dontpair);
            }
          }

          if (samesensor == false)
            cluster2.AddHit(hit);

          if (fDisplayOn) {
            hit->Draw(kOrange);
            display->Update();
            display->Modified();
          }
          //     PndTrkConformalHit *chit = conform->GetConformalHit(hit);
          //        conformalhitlist->AddHit(chit);
        }
      }

      // ... and put mvd strip hits to conformal plane ---------------
      for (int jhit = 0; jhit < mvdstrhitlist->GetNofHits(); jhit++) {
        hit = mvdstrhitlist->GetHit(jhit);
        if (hit->IsUsed() == kTRUE)
          continue;
        if (searchnearby == true) { // search in this sector and +/- 1
          if (fabs(hit->GetSector() - sectorID) > 1 && !(hit->GetSector() == 5 && sectorID == 0) && !(hit->GetSector() == 0 && sectorID == 5))
            continue;
        } else { // search only in this sector and possible othersector
          if (border == false && hit->GetSector() != sectorID)
            continue;
          else if (border == true && (hit->GetSector() != sectorID && hit->GetSector() != othersecID))
            continue;
        }
        // if(hit->GetSector() != sectorID) continue;
        double distance_hit_center = (hit->GetPosition().XYvector() - TVector2(xc, yc)).Mod();
        double recoiso = fabs(distance_hit_center - R);
        //	cout << "recoiso str " << recoiso << endl;
        if (recoiso < 3.) {

          // dont want two hits on the same sensor
          int samesensor = false;
          for (int khit = 0; khit < cluster2.GetNofHits(); khit++) {
            PndTrkHit *hitk = cluster2.GetHit(khit);
            if (!hitk->IsMvdStrip())
              continue;
            if (hitk->GetSensorID() != hit->GetSensorID())
              continue;
            samesensor = true;
            double distance_hitk_center = (hitk->GetPosition().XYvector() - TVector2(xc, yc)).Mod();
            double recoisok = fabs(distance_hitk_center - R);
            if (recoiso < recoisok) {
              cluster2.DeleteHitAndCompress(hitk);
              std::pair<int, int> dontpair(hitk->GetHitID(), hitk->GetDetectorID());
              dontuse.push_back(dontpair);
              cluster2.AddHit(hit);
            } else {
              std::pair<int, int> dontpair(hit->GetHitID(), hit->GetDetectorID());
              dontuse.push_back(dontpair);
            }
          }

          if (samesensor == false)
            cluster2.AddHit(hit);

          if (fDisplayOn) {
            hit->Draw(kOrange);
            display->Update();
            display->Modified();
          }
          PndTrkConformalHit chit = conform->GetConformalHit(hit);
          fConformalHitList->AddHit(&chit);
        }
      }

      if (fDisplayOn) {
        display->cd(1);
        cluster2.Draw(kRed);
        display->Update();
        display->Modified();
        // char goOnChar; //[R.K. 01/2017] unused variable
        cout << "want to go on?" << endl;
        // cin >> goOnChar;
      }

      // -------- sorting
      // more complex sorting
      // sort in z ....
      for (int jhit = 0; jhit < cluster2.GetNofHits(); jhit++) {
        hit = cluster2.GetHit(jhit);
        hit->SetSortVariable(hit->GetPosition().Z());
      }
      cluster2.Sort();
      // ... then sort in phi from the 1st hit
      PndTrkTrack track2(&cluster2, xc, yc, R);
      //      cout << "xc, yc, R " << xc << " " << yc << " " << R << endl;
      for (int jhit = 0; jhit < cluster2.GetNofHits(); jhit++) {
        hit = cluster2.GetHit(jhit);
        double phi = track2.ComputePhiFrom(hit->GetPosition(), cluster2.GetHit(0)->GetPosition());
        //	double phi = track2.ComputePhi(hit->GetPosition());
        hit->SetPhi(phi);
        hit->SetSortVariable(phi);
      }
      cluster2.Sort();

      //       // if the track is positive --> add 360 (otherwise is 0 followed by 350, 350...)
      //       track2.ComputeCharge();
      //       double charge = track2.GetCharge();
      //       //      cout << "charge " << charge << endl;
      //       if(charge > 0)      cluster2.GetHit(0)->SetPhi(360.) ; // CHECK

      //       // further check, but maybe nomore necessary CHECK
      //       if(cluster2.GetHit(0)->IsGem() && (cluster2.GetHit(cluster2.GetNofHits() - 1)->IsMvdPixel() || cluster2.GetHit(cluster2.GetNofHits() - 1)->IsMvdStrip())) {
      // 	PndTrkCluster cluster2b;
      // 	cluster2b.AddHit(cluster2.GetHit(0));
      // 	cluster2.ReverseSort();
      // 	for(int jhit = 1; jhit < cluster2.GetNofHits(); jhit++) {
      // 	  cluster2b.AddHit(cluster2.GetHit(jhit));
      // 	}
      // 	cluster2 = cluster2b;
      //       }
      // ..........................................

      for (int jhit = 0; jhit < cluster2.GetNofHits(); jhit++) {
        hit = cluster2.GetHit(jhit);
        //	cout << "hit " << hit->GetHitID() << " " << hit->GetDetectorID() << " " << hit->GetPhi() << " " << hit->GetSortVariable() << endl;
      }

      // first hypothesis of z slope
      if (fDisplayOn)
        DrawZGeometry(-360, 360, -40, 200);

      fLineHisto->Reset();

      for (int jhit = 0; jhit < cluster2.GetNofHits(); jhit++) {
        hit = cluster2.GetHit(jhit);
        if (hit->IsStt())
          continue;
        TVector3 position = hit->GetPosition();
        double phi = hit->GetPhi();
        if (fDisplayOn) {
          // char goOnChar; //[R.K. 01/2017] unused variable
          display->cd(4);
          TMarker *mrkz = nullptr;
          if (hit->IsMvdPixel())
            mrkz = new TMarker(phi, position.Z(), 21);
          else if (hit->IsMvdStrip())
            mrkz = new TMarker(phi, position.Z(), 25);
          else if (hit->IsStt())
            mrkz = new TMarker(phi, position.Z(), 6);
          else if (hit->IsGem())
            mrkz = new TMarker(phi, position.Z(), 24);
          else if (hit->IsSciTil())
            mrkz = new TMarker(phi, position.Z(), 26);

          mrkz->SetMarkerColor(kBlue);
          mrkz->Draw("SAME");
          display->Update();
          display->Modified();
          // 	  cin >> goOnChar;
        }
        for (int khit = jhit + 1; khit < cluster2.GetNofHits(); khit++) {
          PndTrkHit *hitk = cluster2.GetHit(khit);
          if (hitk->IsStt())
            continue;
          TVector3 positionk = hitk->GetPosition();
          double phik = hitk->GetPhi();

          double denom = (phi - phik) * (phi - phik) + (positionk.Z() - position.Z()) * (positionk.Z() - position.Z());
          if (denom == 0)
            continue;
          double cost = (positionk.Z() - position.Z()) / TMath::Sqrt(denom);
          double theta = TMath::ACos(cost);
          double r1 = phi * cost + position.Z() * TMath::Sin(theta);
          double r2 = phik * cost + positionk.Z() * TMath::Sin(theta);
          double r = r1;
          if (fabs(r1 - r2) > 1.e-9) {
            theta = -TMath::ACos(cost);
            r = phi * cost + position.Z() * TMath::Sin(theta);
          }
          fLineHisto->Fill(theta * TMath::RadToDeg(), r);
          //	  cout << theta * TMath::RadToDeg() << " " <<  r << endl;
        }
      }
      int bin = fLineHisto->GetMaximumBin();
      int binx, biny, binz;
      fLineHisto->GetBinXYZ(bin, binx, biny, binz);
      double tpeak = fLineHisto->GetXaxis()->GetBinCenter(binx);
      double rpeak = fLineHisto->GetYaxis()->GetBinCenter(biny);
      //  cout << "tpeak " << tpeak << " rpeak " << rpeak << endl;

      // .........................................
      fFitter->Reset();
      for (int jhit = 0; jhit < cluster2.GetNofHits(); jhit++) {
        hit = cluster2.GetHit(jhit);
        if (!hit->IsGem())
          continue;
        TVector3 position = hit->GetPosition();
        double phi = hit->GetPhi();

        if (fDisplayOn) {
          // char goOnChar; //[R.K. 01/2017] unused variable
          display->cd(4);
          TMarker *mrkz = nullptr;
          if (hit->IsMvdPixel())
            mrkz = new TMarker(phi, position.Z(), 21);
          else if (hit->IsMvdStrip())
            mrkz = new TMarker(phi, position.Z(), 25);
          else if (hit->IsStt())
            mrkz = new TMarker(phi, position.Z(), 6);
          else if (hit->IsGem())
            mrkz = new TMarker(phi, position.Z(), 24);
          else if (hit->IsSciTil())
            mrkz = new TMarker(phi, position.Z(), 26);

          mrkz->SetMarkerColor(kBlue);
          mrkz->Draw("SAME");
          display->Update();
          display->Modified();
          // 	  cin >> goOnChar;
        }

        double sigma = 1e-5 * position.Z();
        fFitter->SetPointToFit(phi, position.Z(), sigma);
      }

      double fitm4, fitq4;
      fFitter->StraightLineFit(fitm4, fitq4);

      // .........................................

      double fitm44 = -TMath::Cos(tpeak * TMath::DegToRad()) / TMath::Sin(tpeak * TMath::DegToRad());
      double fitq44 = rpeak / TMath::Sin(tpeak * TMath::DegToRad());

      if (fDisplayOn) {
        display->cd(3);
        fLineHisto->Draw("colz");
        display->cd(4);
        TLine *line = new TLine(-360, -360 * fitm4 + fitq4, 360, 360 * fitm4 + fitq4);
        line->SetLineColor(3);
        line->Draw("SAME");

        TLine *line4 = new TLine(-360, -360 * fitm44 + fitq44, 360, 360 * fitm44 + fitq44);
        line4->SetLineColor(4);
        line4->Draw("SAME");

        display->Update();
        display->Modified();
        // char goOnChar; //[R.K. 01/2017] unused variable
        // cin >> goOnChar;
      }

      //       continue;

      // retrieve the hits shifted by 360 deg
      for (int jhit = 0; jhit < cluster2.GetNofHits(); jhit++) {
        hit = cluster2.GetHit(jhit);
        if (hit->IsStt())
          continue;
        TVector3 position = hit->GetPosition();
        double phi = hit->GetPhi();
        double distance = fabs(position.Z() - fitm4 * phi - fitq4);

        // retrieve the hits shifted by 360
        if (distance > 100) {
          // try with 360 deg shift
          double comp_phi = (position.Z() - fitq4) / fitm4;
          if (fabs(comp_phi - phi) < 361 && fabs(comp_phi - phi) > 359) {
            if (comp_phi > phi)
              phi += 360.;
            else
              phi -= 360.;
            hit->SetPhi(phi);
            distance = fabs(position.Z() - fitm4 * phi - fitq4);
            //	    cout << "PHI possible! " << fabs(comp_phi - phi) << endl;
          }
        }
      }

      // associate only the right hits in z for gem & mvd
      PndTrkCluster cluster3;
      for (int jhit = 0; jhit < cluster2.GetNofHits(); jhit++) {
        hit = cluster2.GetHit(jhit);
        //	cout << " hit in cluster2: " << jhit << " " << hit->GetDetectorID() << endl;
        if (hit->IsStt()) {
          cluster3.AddHit(hit);
          //	  cout << " add hit " << jhit << endl;
        } else {
          TVector3 position = hit->GetPosition();
          double phi = hit->GetPhi();

          double distance = fabs(position.Z() - fitm4 * phi - fitq4);
          // cout << " z part " << phi << " " << position.Z() << " " << distance << endl;

          if (distance > 3 && hit->IsMvd() == kTRUE) {
            int hitId = hit->GetHitID();
            PndTrkHit *thishit = nullptr;
            if (hit->IsMvdPixel())
              thishit = mvdpixhitlist->GetHitByID(hitId);
            else if (hit->IsMvdStrip())
              thishit = mvdstrhitlist->GetHitByID(hitId);

            std::pair<int, int> dontpair(hitId, hit->GetDetectorID());
            dontuse.push_back(dontpair);

            thishit->SetUsedFlag(kFALSE);
            // 	    cout << "distance " << distance << " of hit " << hit->GetHitID() << " " << hit->GetDetectorID() << " " << hit->IsMvdPixel() << endl;
            continue;
          } else if (distance > 20 && hit->IsGem() == kTRUE) {
            int hitId = hit->GetHitID();
            PndTrkHit *thishit = gemhitlist->GetHitByID(hitId);
            thishit->SetUsedFlag(kFALSE);

            std::pair<int, int> dontpair(hitId, hit->GetDetectorID());
            dontuse.push_back(dontpair);

            // 	    cout << "distance " << distance << " of hit " << hit->GetDetectorID() << endl;
            continue;
          }

          if (fDisplayOn) {
            // char goOnChar; //[R.K. 01/2017] unused variable
            display->cd(4);
            TMarker *mrkz = new TMarker(phi, position.Z(), 20);
            mrkz->SetMarkerColor(kOrange);
            mrkz->Draw("SAME");
            display->Update();
            display->Modified();
          }
          // 	  cout << " add hit " << jhit << endl;
          cluster3.AddHit(hit);
        }
      }

      bool stat[3] = {false, false, false};
      for (int jhit = 0; jhit < cluster3.GetNofHits(); jhit++) {
        hit = cluster3.GetHit(jhit);
        if (hit->IsGem() == kFALSE)
          continue;
        switch (hit->GetSensorID()) {
        case 0:
        case 1: stat[0] = true; break;
        case 2:
        case 3: stat[1] = true; break;
        case 4:
        case 5: stat[2] = true; break;
        }
      }
      //       if(stat[0] == false || stat[1] == false || stat[2] == false) continue;
      if (stat[0] == false && stat[1] == false && stat[2] == false)
        continue;

      cluster3.Sort();
      if (cluster3.GetNofHits() == 0)
        continue;

      // -------- fit with a line
      fFitter->Reset();
      for (int jhit = 0; jhit < cluster3.GetNofHits(); jhit++) {
        hit = cluster3.GetHit(jhit);
        if (hit->IsStt())
          continue;
        TVector3 position = hit->GetPosition();
        double phi = hit->GetPhi();
        //	cout << " z part " << phi << " " << position.Z() << endl;
        if (fDisplayOn) {
          // char goOnChar; //[R.K. 01/2017] unused variable
          display->cd(4);
          TMarker *mrkz = nullptr;
          if (hit->IsMvdPixel())
            mrkz = new TMarker(phi, position.Z(), 21);
          else if (hit->IsMvdStrip())
            mrkz = new TMarker(phi, position.Z(), 25);
          else if (hit->IsStt())
            mrkz = new TMarker(phi, position.Z(), 6);
          else if (hit->IsGem())
            mrkz = new TMarker(phi, position.Z(), 24);

          mrkz->SetMarkerColor(kBlue);
          mrkz->Draw("SAME");
          display->Update();
          display->Modified();
          // 	  cin >> goOnChar;
        }

        fFitter->SetPointToFit(phi, position.Z(), 0.1); // CHECK the error?
      }

      double fitm5, fitq5;
      fFitter->StraightLineFit(fitm5, fitq5);

      if (fDisplayOn) {
        display->cd(4);
        TLine *line = new TLine(-360, -360 * fitm5 + fitq5, 360, 360 * fitm5 + fitq5);
        line->SetLineColor(4);
        line->Draw("SAME");
        display->Update();
        display->Modified();
        // char goOnChar; //[R.K. 01/2017] unused variable
        // cin >> goOnChar;
      }

      fitm4 = fitm5;
      fitq4 = fitq5;
      // REFIT ANALYTICALLY ....again taken from-..
      // --------------------------
      // if there is a scitil lets use it as seed hit
      // for the conformal map
      fConformalHitList->Clear("C");
      refhit = cluster3.GetHit(0); // cluster3.GetNofHits() - 1);
      trasl[0] = 0;
      trasl[1] = 0;
      delta = 0;

      //       cout <<  cluster3.GetNofHits() << " refhit ---> " << refhit << endl;

      ComputeTraAndRot(refhit, delta, trasl);
      conform->SetOrigin(trasl[0], trasl[1], delta);
      fConformalHitList->SetConformalTransform(conform);

      nofconfhits = FillConformalHitList(&cluster3);

      //      cout << "nofhits1: " << nofconfhits << endl;

      // compute conformal plane extremities ---------------------------- this must go to a fctn CHECK
      fUmin = 1000, fVmin = 1000, fRmin = 1000;
      fUmax = -1000, fVmax = -1000, fRmax = -1000;

      for (int jhit = 0; jhit < fConformalHitList->GetNofHits(); jhit++) {
        PndTrkConformalHit *chit = fConformalHitList->GetHit(jhit);
        double u = chit->GetU();
        double v = chit->GetV();
        double rc = chit->GetIsochrone();
        if (TMath::IsNaN(u))
          continue; // prevents the nan of the ref hit
        if (rc < 0)
          rc = 0;
        //	cout << "conf hit " << jhit << " u, v " << u << " " << v << " " << rc << endl;
        u - rc < fUmin ? fUmin = u - rc : fUmin;
        v - rc < fVmin ? fVmin = v - rc : fVmin;
        u + rc > fUmax ? fUmax = u + rc : fUmax;
        v + rc > fVmax ? fVmax = v + rc : fVmax;

        double theta1 = TMath::ATan2(v, u);
        double theta2 = theta1 + TMath::Pi();

        double r1 = u * TMath::Cos(theta1) + v * TMath::Sin(theta1);
        double r2 = u * TMath::Cos(theta2) + v * TMath::Sin(theta2);

        double rimin, rimax;
        r1 < r2 ? (rimin = r1, rimax = r2) : (rimin = r2, rimax = r1);

        rimin < fRmin ? (rc_of_min = rc, fRmin = rimin) : fRmin;
        rimax > fRmax ? (rc_of_max = rc, fRmax = rimax) : fRmax;
      }

      fRmin -= rc_of_min;
      fRmax += rc_of_max;

      // to square the conformal plane
      du = fUmax - fUmin;
      dv = fVmax - fVmin;
      delt = fabs(dv - du) / 2.;
      du < dv ? (fUmin -= delt, fUmax += delt) : (fVmin -= delt, fVmax += delt);
      // cout << "min/max " << fUmin << " " << fUmax << " " << fVmin << " " << fVmax << endl;
      if (fDisplayOn) {
        DrawGeometryConf(fUmin, fUmax, fVmin, fVmax);
        // ----------------------------------------------------------------------
        for (int jhit = 0; jhit < nofconfhits; jhit++) {
          PndTrkConformalHit *chit = fConformalHitList->GetHit(jhit);
          chit->Draw(kRed);
        }
      }

      // ====== REFIT CLUSTER ANALYTICALLY
      double fitm3, fitq3;
      bool fitting = AnalyticalFit(&cluster3, xc, yc, R, fitm3, fitq3);
      if (fitting == kFALSE)
        continue;
      double xc3, yc3, R3;
      FromConformalToRealTrack(fitm3, fitq3, xc3, yc3, R3);

      if (fDisplayOn) {
        // char goOnChar; //[R.K. 01/2017] unused variable
        display->cd(1);
        TArc *arcm = new TArc(xc3, yc3, R3);
        arcm->SetFillStyle(0);
        arcm->SetLineColor(4);
        arcm->Draw("SAME");
        display->Update();
        display->Modified();
        // cin >> goOnChar;
      }

      // ==================
      // MAKE the FINAL CLUSTER (at least in xy)
      fFinalCluster->Clear("C");
      //       cout << "cluster3 " << cluster3.GetNofHits() << " fFinalCluster " << fFinalCluster->GetNofHits() << endl;
      // GEM
      for (int jhit = 0; jhit < gemhitlist->GetNofHits(); jhit++) {
        hit = gemhitlist->GetHit(jhit);

        // check if already discarded in the phi.z procedure
        std::pair<int, int> thispair(hit->GetHitID(), hit->GetDetectorID());
        std::vector<std::pair<int, int>>::iterator itr = dontuse.begin();
        itr = std::find(dontuse.begin(), dontuse.end(), thispair);
        if (itr != dontuse.end())
          continue;

        if (border == false && hit->GetSector() != sectorID)
          continue;
        else if (border == true && (hit->GetSector() != sectorID && hit->GetSector() != othersecID))
          continue;
        double distance_hit_center = (hit->GetPosition().XYvector() - TVector2(xc3, yc3)).Mod();
        double recoiso = fabs(distance_hit_center - R3);
        if (recoiso < 0.5) {
          hit->SetSortVariable(hit->GetPosition().Z());

          // dont want two hits on the same sensor
          int samesensor = false;
          for (int khit = 0; khit < fFinalCluster->GetNofHits(); khit++) {
            PndTrkHit *hitk = fFinalCluster->GetHit(khit);
            if (!hitk->IsGem())
              continue;
            if (hitk->GetSensorID() != hit->GetSensorID())
              continue;
            samesensor = true;
            double distance_hitk_center = (hitk->GetPosition().XYvector() - TVector2(xc3, yc3)).Mod();
            double recoisok = fabs(distance_hitk_center - R3);
            if (recoiso < recoisok) {
              fFinalCluster->DeleteHitAndCompress(hitk);
              std::pair<int, int> dontpair(hitk->GetHitID(), hitk->GetDetectorID());
              dontuse.push_back(dontpair);
              fFinalCluster->AddHit(hit);
            } else {
              std::pair<int, int> dontpair(hit->GetHitID(), hit->GetDetectorID());
              dontuse.push_back(dontpair);
            }
          }

          if (samesensor == false)
            fFinalCluster->AddHit(hit);

          // 	  fFinalCluster->AddHit(hit);
        }
      }
      fFinalCluster->ReverseSort();

      for (int jhit = 0; jhit < fFinalCluster->GetNofHits(); jhit++) {
        hit = fFinalCluster->GetHit(jhit);

        // PndGemHit *gemhit = (PndGemHit*) fGemHitArray->At(hit->GetHitID()); //[R.K. 01/2017] unused variable?
        // int refindex = gemhit->GetRefIndex(); //[R.K. 01/2017] unused variable

        // 	 cout << "THIS it is " <<  hit->GetHitID() << " " << hit->GetDetectorID() << " " << hit->GetSensorID() << " " << refindex << endl;
      }

      // check if last station has only one hit ~~~~~~~~~~~~~~~~~~~~~
      // define last and almost last stations
      int laststat = -1, almostlaststat = -1;
      for (int jhit = 0; jhit < fFinalCluster->GetNofHits(); jhit++) {
        hit = fFinalCluster->GetHit(jhit);
        if (laststat == -1)
          laststat = hit->GetSensorID();
        if (laststat != -1 && almostlaststat == -1 && hit->GetSensorID() != laststat)
          almostlaststat = hit->GetSensorID();
      }
      // count
      int hitsonlaststat = 0;
      for (int jhit = 0; jhit < fFinalCluster->GetNofHits(); jhit++) {
        hit = fFinalCluster->GetHit(jhit);
        // 	cout << "hit " << hit->GetHitID() << " " << hit->GetDetectorID() << " " << hit->GetSensorID() << endl;
        if (hit->GetSensorID() != laststat)
          continue;
        hitsonlaststat++;
      }

      //      cout << "ON LAST STATION " << laststat << " - " << hitsonlaststat << endl;
      if (laststat == -1 || almostlaststat == -1)
        continue;
      TVector3 fromhere = fFinalCluster->GetHit(0)->GetPosition(); // CHECK HERE
      //      fromhere.Print();
      if (hitsonlaststat > 1) {
        double dist45 = 1000;
        for (int jhit = 0; jhit < fFinalCluster->GetNofHits(); jhit++) {
          hit = fFinalCluster->GetHit(jhit);
          if (hit->GetSensorID() != laststat)
            continue;
          else if (hit->GetSensorID() < laststat)
            break;
          for (int khit = jhit + 1; khit < fFinalCluster->GetNofHits(); khit++) {
            PndTrkHit *hitk = fFinalCluster->GetHit(khit);
            if (hitk->GetSensorID() != almostlaststat)
              continue;
            else if (hit->GetSensorID() < almostlaststat)
              break;
            double tmpdist = hit->GetXYDistance(hitk);
            if (tmpdist < dist45)
              fromhere = hit->GetPosition();
          }
        }
      }
      //      fromhere.Print();

      // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

      //       TVector3 fromhere = fFinalCluster->GetHit(0)->GetPosition(); // CHECK HERE
      //       cout << "fFinalCluster, gem " << fFinalCluster->GetNofHits() << endl;

      // STT
      for (int jhit = 0; jhit < stthitlist->GetNofHits(); jhit++) {
        hit = stthitlist->GetHit(jhit);
        if (hit->IsSttParallel() == kFALSE)
          continue;
        PndSttTube *tube = (PndSttTube *)fTubeArray->At(hit->GetTubeID());
        if (border == false && tube->GetSectorID() != sectorID)
          continue;
        else if (border == true && (tube->GetSectorID() != sectorID && tube->GetSectorID() != othersecID))
          continue;
        double distance_hit_center = (tube->GetPosition().XYvector() - TVector2(xc3, yc3)).Mod();
        double recoiso = fabs(distance_hit_center - R3);
        if (recoiso < 0.5) {
          fFinalCluster->AddHit(hit);
        }
      }
      //      cout << "fFinalCluster, gem + stt " << fFinalCluster->GetNofHits() << endl;

      // MVD PIX
      for (int jhit = 0; jhit < mvdpixhitlist->GetNofHits(); jhit++) {
        hit = mvdpixhitlist->GetHit(jhit);

        // check if already discarded in the phi.z procedure
        std::pair<int, int> thispair(hit->GetHitID(), hit->GetDetectorID());
        std::vector<std::pair<int, int>>::iterator itr = dontuse.begin();
        itr = std::find(dontuse.begin(), dontuse.end(), thispair);
        if (itr != dontuse.end())
          continue;

        if (border == false && hit->GetSector() != sectorID)
          continue;
        else if (border == true && (hit->GetSector() != sectorID && hit->GetSector() != othersecID))
          continue;
        double distance_hit_center = (hit->GetPosition().XYvector() - TVector2(xc3, yc3)).Mod();
        double recoiso = fabs(distance_hit_center - R3);
        if (recoiso < 0.5) {

          // dont want two hits on the same sensor
          int samesensor = false;
          for (int khit = 0; khit < fFinalCluster->GetNofHits(); khit++) {
            PndTrkHit *hitk = fFinalCluster->GetHit(khit);
            if (!hitk->IsMvdPixel())
              continue;
            if (hitk->GetSensorID() != hit->GetSensorID())
              continue;
            samesensor = true;
            double distance_hitk_center = (hitk->GetPosition().XYvector() - TVector2(xc3, yc3)).Mod();
            double recoisok = fabs(distance_hitk_center - R3);
            if (recoiso < recoisok) {
              fFinalCluster->DeleteHitAndCompress(hitk);
              std::pair<int, int> dontpair(hitk->GetHitID(), hitk->GetDetectorID());
              dontuse.push_back(dontpair);
              fFinalCluster->AddHit(hit);
            } else {
              std::pair<int, int> dontpair(hit->GetHitID(), hit->GetDetectorID());
              dontuse.push_back(dontpair);
            }
          }

          if (samesensor == false)
            fFinalCluster->AddHit(hit);
          // fFinalCluster->AddHit(hit);
        }
      }
      //           cout << "fFinalCluster, gem + stt + pix " << fFinalCluster->GetNofHits() << endl;

      // MVD STR
      for (int jhit = 0; jhit < mvdstrhitlist->GetNofHits(); jhit++) {
        hit = mvdstrhitlist->GetHit(jhit);

        // check if already discarded in the phi.z procedure
        std::pair<int, int> thispair(hit->GetHitID(), hit->GetDetectorID());
        std::vector<std::pair<int, int>>::iterator itr = dontuse.begin();
        itr = std::find(dontuse.begin(), dontuse.end(), thispair);
        if (itr != dontuse.end())
          continue;

        if (border == false && hit->GetSector() != sectorID)
          continue;
        else if (border == true && (hit->GetSector() != sectorID && hit->GetSector() != othersecID))
          continue;
        double distance_hit_center = (hit->GetPosition().XYvector() - TVector2(xc3, yc3)).Mod();
        double recoiso = fabs(distance_hit_center - R3);
        if (recoiso < 0.5) {

          fFinalCluster->AddHit(hit);
        }
      }
      //     cout << "fFinalCluster, gem + stt + str " << fFinalCluster->GetNofHits() << endl;

      // SCITIL
      int tmphit = -1;
      double tmpdistance = 1000;
      for (int jhit = 0; jhit < scithitlist->GetNofHits(); jhit++) {
        hit = scithitlist->GetHit(jhit);
        double distance_hit_center = (hit->GetPosition().XYvector() - TVector2(xc3, yc3)).Mod();
        double recoiso = fabs(distance_hit_center - R3);
        if (recoiso < 0.5) {
          if (recoiso < tmpdistance) {
            tmphit = jhit;
            tmpdistance = recoiso;
          }
        }
      }
      if (tmphit > -1) {
        hit = scithitlist->GetHit(tmphit);
        fFinalCluster->AddHit(hit);
      }
      //       cout << "fFinalCluster, gem + stt + pix + str + scit " << fFinalCluster->GetNofHits() << endl;

      if (fFinalCluster->GetNofHits() == 0) {
        continue;
      }

      // sorting from last gem

      PndTrkTrack track3(fFinalCluster, xc3, yc3, R3);
      for (int jhit = 0; jhit < fFinalCluster->GetNofHits(); jhit++) {
        hit = fFinalCluster->GetHit(jhit);

        double phi = track3.ComputePhiFrom(hit->GetPosition(), fromhere);
        if (phi == 360)
          phi = 0;
        hit->SetPhi(phi);
        hit->SetSortVariable(phi);
      }
      fFinalCluster->Sort();
      for (int jhit = 0; jhit < fFinalCluster->GetNofHits(); jhit++) {
        hit = fFinalCluster->GetHit(jhit);
        //      cout << "-> " << hit->GetHitID() << " " << hit->GetDetectorID() << " " << hit->GetPhi()  << " " << hit->GetSortVariable() << " " << hit->IsSortable() << endl;
      }

      // check the 0/360 discontinuity ..................
      // PndTrkHit *hit1 = fFinalCluster->GetHit(0); //[R.K. 03/2017] unused variable
      // PndTrkHit *hit2 = fFinalCluster->GetHit(1); //[R.K. 03/2017] unused variable
      if (fDisplayOn) {
        for (int jhit = 0; jhit < fFinalCluster->GetNofHits(); jhit++) {
          hit = fFinalCluster->GetHit(jhit);
          //	  cout << "-> " << hit->GetHitID() << " " << hit->GetDetectorID() << " " << hit->GetPhi()  << " " << hit->GetSortVariable() << endl;
          TMarker *mrk = new TMarker(hit->GetPosition().X(), hit->GetPosition().Y(), 20);
          mrk->SetMarkerColor(kMagenta);
          mrk->Draw("SAME");
          char goOnChar;
          cin >> goOnChar;
          display->Update();
          display->Modified();
        }
      }

      int counter = 0;
      double meanphi = 0;
      for (int jhit = 0; jhit < fFinalCluster->GetNofHits(); jhit++) {
        hit = fFinalCluster->GetHit(jhit);
        if (!hit->IsGem())
          continue;
        double phi = hit->GetPhi();
        if (phi != 0) {
          meanphi += phi;
          counter++;
        }
      }

      if (meanphi > 300) {
        for (int jhit = 0; jhit < fFinalCluster->GetNofHits(); jhit++) {
          hit = fFinalCluster->GetHit(jhit);
          if (!hit->IsGem())
            continue;
          double phi = hit->GetPhi();
          if (phi < 10)
            phi += 360;
          hit->SetPhi(phi);
          hit->SetSortVariable(phi);
        }
        fFinalCluster->Sort();
      } else
        fFinalCluster->ReverseSort();

      if (fDisplayOn) {
        for (int jhit = 0; jhit < fFinalCluster->GetNofHits(); jhit++) {
          hit = fFinalCluster->GetHit(jhit);
          cout << "*** " << hit->GetHitID() << " " << hit->GetDetectorID() << " " << hit->GetPhi() << " " << hit->GetSortVariable() << endl;
          TMarker *mrk = new TMarker(hit->GetPosition().X(), hit->GetPosition().Y(), 20);
          mrk->SetMarkerColor(kGreen);
          mrk->Draw("SAME");
          char goOnChar;
          cin >> goOnChar;
          display->Update();
          display->Modified();
        }
      }
      // ....................................................

      //     cout << "FINAL CLUSTER HAS " << fFinalCluster->GetNofHits() << endl;
      for (int jhit = 0; jhit < fFinalCluster->GetNofHits(); jhit++) {
        hit = fFinalCluster->GetHit(jhit);
        // cout << "final " << hit->GetHitID() << " " << hit->GetDetectorID() << " " << hit->GetPhi()  << " " << hit->GetSortVariable() << " " << hit->IsSortable() << endl;
      }
      if (fDisplayOn) {
        Refresh();
        fFinalCluster->LightUp();
        for (int jhit = 0; jhit < fFinalCluster->GetNofHits(); jhit++) {
          hit = fFinalCluster->GetHit(jhit);
          TMarker *mrk = new TMarker(hit->GetPosition().X(), hit->GetPosition().Y(), 20);
          mrk->SetMarkerColor(kOrange);
          mrk->Draw("SAME");
          char goOnChar;
          cin >> goOnChar;
          display->Update();
          display->Modified();
        }
        char goOnChar;
        cin >> goOnChar;
      }

      // ====== REFIT CLUSTER ANALYTICALLY
      // REFIT ANALYTICALLY ....again taken from-..
      // --------------------------
      // if there is a scitil lets use it as seed hit
      // for the conformal map
      fConformalHitList->Clear("C");
      refhit = fFinalCluster->GetHit(0); // cluster3.GetNofHits() - 1);
      //      cout << "refhit ---> " << refhit << endl;
      ComputeTraAndRot(refhit, delta, trasl);
      conform->SetOrigin(trasl[0], trasl[1], delta);
      fConformalHitList->SetConformalTransform(conform);

      nofconfhits = FillConformalHitList(fFinalCluster);

      // compute conformal plane extremities ---------------------------- this must go to a fctn CHECK
      fUmin = 1000, fVmin = 1000, fRmin = 1000;
      fUmax = -1000, fVmax = -1000, fRmax = -1000;

      for (int jhit = 0; jhit < fConformalHitList->GetNofHits(); jhit++) {
        PndTrkConformalHit *chit = fConformalHitList->GetHit(jhit);
        double u = chit->GetU();
        double v = chit->GetV();
        double rc = chit->GetIsochrone();
        if (TMath::IsNaN(u))
          continue; // prevents the nan of the ref hit
        if (rc < 0)
          rc = 0;
        //	cout << "conf hit " << jhit << " u, v " << u << " " << v << " " << rc << endl;
        u - rc < fUmin ? fUmin = u - rc : fUmin;
        v - rc < fVmin ? fVmin = v - rc : fVmin;
        u + rc > fUmax ? fUmax = u + rc : fUmax;
        v + rc > fVmax ? fVmax = v + rc : fVmax;

        double theta1 = TMath::ATan2(v, u);
        double theta2 = theta1 + TMath::Pi();

        double r1 = u * TMath::Cos(theta1) + v * TMath::Sin(theta1);
        double r2 = u * TMath::Cos(theta2) + v * TMath::Sin(theta2);

        double rimin, rimax;
        r1 < r2 ? (rimin = r1, rimax = r2) : (rimin = r2, rimax = r1);

        rimin < fRmin ? (rc_of_min = rc, fRmin = rimin) : fRmin;
        rimax > fRmax ? (rc_of_max = rc, fRmax = rimax) : fRmax;
      }

      fRmin -= rc_of_min;
      fRmax += rc_of_max;

      // to square the conformal plane
      du = fUmax - fUmin;
      dv = fVmax - fVmin;
      delt = fabs(dv - du) / 2.;
      du < dv ? (fUmin -= delt, fUmax += delt) : (fVmin -= delt, fVmax += delt);
      //      cout << "min/max " << fUmin << " " << fUmax << " " << fVmin << " " << fVmax << endl;
      if (fDisplayOn) {
        DrawGeometryConf(fUmin, fUmax, fVmin, fVmax);
        // ----------------------------------------------------------------------
        for (int jhit = 0; jhit < nofconfhits; jhit++) {
          PndTrkConformalHit *chit = fConformalHitList->GetHit(jhit);
          chit->Draw(kBlue);
        }
      }

      //       cout << "MINUIT FIT FWD" << endl;
      double fitm6, fitq6;
      fitting = AnalyticalFit(fFinalCluster, xc3, yc3, R3, fitm6, fitq6);
      if (fitting == kFALSE)
        continue;

      double xc6, yc6, R6;
      FromConformalToRealTrack(fitm6, fitq6, xc6, yc6, R6);

      // CHECK 15/0.006 cm
      if (R6 > 2500)
        continue;
      PndTrkTrack finaltrack(fFinalCluster, xc6, yc6, R6);
      finaltrack.ComputeCharge();

      if (fDisplayOn) {
        char goOnChar;

        display->cd(2);
        TLine *line = new TLine(-10, -10 * fitm6 + fitq6, 10, 10 * fitm6 + fitq6);
        line->SetLineColor(kMagenta);
        line->Draw("SAME");

        display->cd(1);
        TArc *arcm = new TArc(xc6, yc6, R6);
        arcm->SetFillStyle(0);
        arcm->SetLineColor(kMagenta);
        arcm->Draw("SAME");

        display->Update();
        display->Modified();
        cin >> goOnChar;
      }

      if (fDisplayOn) {
        char goOnChar;
        display->Update();
        display->Modified();
        cin >> goOnChar;
      }

      // ============================ Z PART FOR FWD
      // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      // SKEWED ASSOCIATION ********* CHECK *********
      // -------------------------------------------------------
      //    cout << " %%%%%%%%%%%%%%%%%%%% ZFINDER %%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;

      if (fDisplayOn)
        DrawZGeometry(-360, 360, -40, 200);

      // create cluster for z finding

      // lets start from the skewed --------------------------------
      //    PndTrkCluster skewhitlist = CreateSkewHitList(finaltrack);
      PndTrkSkewHitList skewhitlist;
      // double phimin = 400, phimax = -1, zmin = 1000, zmax = -1; //[R.K. 01/2017] unused variable
      for (int jhit = 0; jhit < stthitlist->GetNofHits(); jhit++) {
        hit = stthitlist->GetHit(jhit);
        //      cout << hit->IsSttSkew() << " " << hit->GetSector() << " " << TMath::RadToDeg() * hit->GetPosition().Phi() << " " << sectorID << " " << othersecID << endl;
        if (hit->IsSttSkew() == kFALSE)
          continue;

        if (border == false && hit->GetSector() != sectorID)
          continue;
        else if (border == true && (hit->GetSector() != sectorID && hit->GetSector() != othersecID))
          continue;

        int tubeID = hit->GetTubeID();
        PndSttTube *tube = (PndSttTube *)fTubeArray->At(tubeID);

        TVector3 wireDirection = tube->GetWireDirection();
        Double_t halflength = tube->GetHalfLength();

        TVector3 first = tube->GetPosition() + wireDirection * halflength;  // CHECK
        TVector3 second = tube->GetPosition() - wireDirection * halflength; // CHECK

        //    double m1 = (first - second).Y()/(first - second).X();
        //    double q1 = first.Y() - m1 * first.X();

        // 1. compute intersection between the track circle and the wire
        TVector2 intersection1, intersection2;
        Int_t nofintersections =
          tools->ComputeSegmentCircleIntersection(TVector2(first.X(), first.Y()), TVector2(second.X(), second.Y()), xc6, yc6, R6, intersection1, intersection2);

        if (nofintersections == 0)
          continue;
        if (nofintersections >= 2) {
          cout << "ERROR: MORE THAN 1 INTERSECTION!!" << endl;
          continue; // CHECK
        }

        // 2. find the tangent to the track in the intersection point
        // tangent approximation
        TVector2 tangent = tools->ComputeTangentInPoint(xc6, yc6, intersection1);

        // 3. rotate clockwise the tangent/point/(wire, not explicitely)
        // in order to have the wire parallel to the x axis;
        // then translate everything to have the wire ON the x axis
        double beta = wireDirection.Phi();
        if (beta < 0)
          beta += TMath::Pi();
        // ... rotate the tangent
        double rtx = TMath::Cos(beta) * tangent.X() + TMath::Sin(beta) * tangent.Y();
        double rty = TMath::Cos(beta) * tangent.Y() - TMath::Sin(beta) * tangent.X();
        TVector2 rottangent(rtx, rty);
        rottangent = rottangent.Unit();
        // ... rotate the point
        double rx = TMath::Cos(beta) * intersection1.X() + TMath::Sin(beta) * intersection1.Y();
        double ry = TMath::Cos(beta) * intersection1.Y() - TMath::Sin(beta) * intersection1.X();

        // translation
        Double_t deltay = ry;
        rty -= deltay;
        ry -= deltay;

        // rotm, rotp
        Double_t rotm = rottangent.Y() / rottangent.X();
        Double_t rotp = ry - rotm * rx;

        // ellipsis
        double a = hit->GetIsochrone() * TMath::Cos(SKEW_ANGLE); // CHECK skew angle hard coded
        double b = hit->GetIsochrone();

        // center of ellipsis
        Double_t x0a, x0b, y0;
        y0 = 0.;
        x0a = (-rotp + TMath::Sqrt(b * b + a * a * rotm * rotm)) / rotm;
        x0b = (-rotp - TMath::Sqrt(b * b + a * a * rotm * rotm)) / rotm;

        // intersection point
        double intxa = (x0a * b * b - rotm * rotp * a * a) / (b * b + rotm * rotm * a * a);
        double intya = rotm * intxa + rotp;
        double intxb = (x0b * b * b - rotm * rotp * a * a) / (b * b + rotm * rotm * a * a);
        double intyb = rotm * intxb + rotp;

        // 4. retraslate/rerotate all back to the original plane
        // retranslate
        y0 += deltay;
        intya += deltay;
        intyb += deltay;

        // rerotate
        double x0anew = TMath::Cos(beta) * x0a - TMath::Sin(beta) * y0;
        double y0anew = TMath::Cos(beta) * y0 + TMath::Sin(beta) * x0a;
        double x0bnew = TMath::Cos(beta) * x0b - TMath::Sin(beta) * y0;
        double y0bnew = TMath::Cos(beta) * y0 + TMath::Sin(beta) * x0b;

        double intxanew = TMath::Cos(beta) * intxa - TMath::Sin(beta) * intya;
        double intyanew = TMath::Cos(beta) * intya + TMath::Sin(beta) * intxa;
        double intxbnew = TMath::Cos(beta) * intxb - TMath::Sin(beta) * intyb;
        double intybnew = TMath::Cos(beta) * intyb + TMath::Sin(beta) * intxb;

        intxa = intxanew;
        intya = intyanew;
        intxb = intxbnew;
        intyb = intybnew;

        // now we have x0a, y0a, center of the 1st ellipse
        // and x0b, y0b, center of the 2nd ellipse
        x0a = x0anew;
        double y0a = y0anew;
        x0b = x0bnew;
        double y0b = y0bnew;

        if (fDisplayOn) {
          // char goOnChar; //[R.K. 01/2017] unused variable
          display->cd(1);

          TEllipse *ell1 = new TEllipse(x0a, y0a, a, b, 0, 360, -beta);
          ell1->SetFillStyle(0);
          ell1->SetLineColor(4);
          ell1->Draw("SAME");
          TEllipse *ell2 = new TEllipse(x0b, y0b, a, b, 0, 360, -beta);
          ell2->SetFillStyle(0);
          ell2->SetLineColor(6);
          ell2->Draw("SAME");

          TMarker *mrkinta = new TMarker(intxa, intya, 20);
          mrkinta->SetMarkerColor(4);
          mrkinta->Draw("SAME");
          TMarker *mrkintb = new TMarker(intxb, intyb, 20);
          mrkintb->SetMarkerColor(6);
          mrkintb->Draw("SAME");
          display->Update();
          display->Modified();
          // 	cin >> goOnChar;
        }

        // 5. calculate z coordinate for each intersection

        // calculate z0a, z0b of the center of the ellipse
        Double_t t = ((x0a + y0a) - (first.X() + first.Y())) / ((second.X() - first.X()) + (second.Y() - first.Y()));
        Double_t z0a = first.Z() + (second.Z() - first.Z()) * t;
        //    cout << "0 : calculate t, z0a " << t << " " << z0a << endl;

        t = ((x0b + y0b) - (first.X() + first.Y())) / ((second.X() - first.X()) + (second.Y() - first.Y()));
        Double_t z0b = first.Z() + (second.Z() - first.Z()) * t;

        TVector3 center1(x0a, y0a, z0a);
        TVector3 center2(x0b, y0b, z0b);

        // calculate the z of the intersection ON the ellipse (CHECK this step calculations!)
        double dx = intxa - x0a;
        double dy = intya - y0a;
        TVector3 dxdy(dx, dy, 0.0);

        TVector3 tfirst = first + dxdy;
        TVector3 tsecond = second + dxdy;

        t = ((intxa + intya) - (tfirst.X() + tfirst.Y())) / ((tsecond.X() - tfirst.X()) + (tsecond.Y() - tfirst.Y()));
        double intza = tfirst.Z() + (tsecond.Z() - tfirst.Z()) * t;
        if (fDisplayOn) {
          // char goOnChar; //[R.K. 01/2017] unused variable
          display->cd(3);
          TLine *linezx1 = new TLine(tfirst.X(), tfirst.Z(), tsecond.X(), tsecond.Z());
          linezx1->SetLineStyle(1);
          linezx1->Draw("SAME");
          TMarker *mrkza1 = new TMarker(intxa, intza, 20);
          mrkza1->SetMarkerColor(kBlue - 9);
          mrkza1->Draw("SAME");
          // cin >> goOnChar;
        }

        tfirst = first - dxdy;
        tsecond = second - dxdy;

        t = ((intxb + intyb) - (tfirst.X() + tfirst.Y())) / ((tsecond.X() - tfirst.X()) + (tsecond.Y() - tfirst.Y()));
        double intzb = tfirst.Z() + (tsecond.Z() - tfirst.Z()) * t;

        TVector3 fin_intersection1(intxa, intya, intza);
        TVector3 fin_intersection2(intxb, intyb, intzb);

        // CHECK to be changed
        int trackID = 1;
        double phi1 = finaltrack.ComputePhiFrom(fin_intersection1, fromhere);
        double phi2 = finaltrack.ComputePhiFrom(fin_intersection2, fromhere);
        hit->SetPhi((phi1 + phi2) * 0.5);
        hit->SetSortVariable((phi1 + phi2) * 0.5);
        skewhitlist.AddHit(PndTrkSkewHit(*hit, trackID, center1, fin_intersection1, phi1, center2, fin_intersection2, phi2, a, b, -1, beta));
      }

      // add hits which have a z info
      // and belong to the cluster
      for (int jhit = 0; jhit < fFinalCluster->GetNofHits(); jhit++) {
        hit = fFinalCluster->GetHit(jhit);
        // double phi = hit->GetPhi(); // finaltrack.ComputePhiFrom(hit->GetPosition(), fromhere); //[R.K. 01/2017] unused variable
        if (hit->IsStt() == kFALSE) {
          //	hit->SetPhi(phi);
          //	hit->SetSortVariable(phi);
          skewhitlist.AddHit(PndTrkSkewHit(*hit));
        }
      }

      //    cout << "dopo sorting" << endl;
      for (int jhit = 0; jhit < fFinalCluster->GetNofHits(); jhit++) {
        hit = fFinalCluster->GetHit(jhit);
        if (fDisplayOn) {
          display->cd(1);
          TMarker *mrk = new TMarker(hit->GetPosition().X(), hit->GetPosition().Y(), 20);
          mrk->SetMarkerColor(kBlue);
          mrk->Draw("SAME");
          display->Update();
          display->Modified();
          char goOnChar;
          cin >> goOnChar;
        }
      }

      //      cout << "fitm5 " << fitm5 << " fitq5 " << fitq5 << endl;;
      double fitm7 = fitm5;
      double fitq7 = 0.;
      for (int jhit = 0; jhit < skewhitlist.GetNofHits(); jhit++) {
        hit = skewhitlist.GetHit(jhit);
        if (!hit)
          continue;
        if (hit->IsStt() == kFALSE) {
          fitq7 = hit->GetPosition().Z() - fitm7 * hit->GetPhi();
          //	  cout << "non stt " << hit->GetPhi() << " " << hit->GetPosition().Z() << endl;
        }
      }

      if (fDisplayOn) {
        char goOnChar;
        display->cd(4);
        for (int jhit = 0; jhit < skewhitlist.GetNofHits(); jhit++) {
          hit = skewhitlist.GetHit(jhit);
          if (!hit)
            continue;
          //	  cout << "hit.det.phi " << hit->GetHitID() << " " << hit->GetDetectorID() << " " << hit->GetPhi() << endl;
          if (hit->IsStt() == kTRUE) {
            PndTrkSkewHit *skewhit = (PndTrkSkewHit *)hit;

            TVector3 fin_intersection1 = skewhit->GetIntersection1();
            TVector3 fin_intersection2 = skewhit->GetIntersection2();
            double phi1 = skewhit->GetPhi1();
            double phi2 = skewhit->GetPhi2();

            display->cd(4);
            TLine *linezphi = new TLine(phi1, fin_intersection1.Z(), phi2, fin_intersection2.Z());
            linezphi->SetLineStyle(1);
            linezphi->Draw("SAME");
            TMarker *mrkzphi1 = new TMarker(phi1, fin_intersection1.Z(), 20);
            mrkzphi1->SetMarkerColor(kBlue - 9);
            mrkzphi1->Draw("SAME");
            TMarker *mrkzphi2 = new TMarker(phi2, fin_intersection2.Z(), 20);
            mrkzphi2->SetMarkerColor(kMagenta - 7);
            mrkzphi2->Draw("SAME");
            display->Update();
            display->Modified();
          } else {
            cout << "2non stt " << hit->GetPhi() << " " << hit->GetPosition().Z() << endl;
            TMarker *mrkzphi = new TMarker(hit->GetPhi(), hit->GetPosition().Z(), 20);
            mrkzphi->SetMarkerColor(kOrange);
            mrkzphi->Draw("SAME");
            display->Update();
            display->Modified();
          }
        }
        cin >> goOnChar;
      }

      if (fDisplayOn) {
        display->cd(4);
        TLine *line = new TLine(-360, -360 * fitm7 + fitq7, 360, 360 * fitm7 + fitq7);
        line->SetLineColor(2);
        line->Draw("SAME");

        display->Update();
        display->Modified();
        char goOnChar;
        cin >> goOnChar;
      }

      // choose the fin_intersection of the skew
      PndTrkSkewHitList skewhitlist2;
      for (int jhit = 0; jhit < skewhitlist.GetNofHits(); jhit++) {
        hit = (PndTrkHit *)skewhitlist.GetHit(jhit);
        if (!hit)
          continue;
        if (hit->IsSttSkew() == kFALSE) {
          skewhitlist2.AddHit(hit);
          continue;
        } else {
          TVector3 fin_intersection1 = ((PndTrkSkewHit *)hit)->GetIntersection1();
          TVector3 fin_intersection2 = ((PndTrkSkewHit *)hit)->GetIntersection2();
          double phi1 = ((PndTrkSkewHit *)hit)->GetPhi1();
          double phi2 = ((PndTrkSkewHit *)hit)->GetPhi2();

          double dist1 = fabs(fitm7 * phi1 - fin_intersection1.Z() + fitq7) / TMath::Sqrt(fitm7 * fitm7 + 1); // CHECK ortho distance or not?
          double dist2 = fabs(fitm7 * phi2 - fin_intersection2.Z() + fitq7) / TMath::Sqrt(fitm7 * fitm7 + 1); // CHECK    "           "

          double distance = 1000;
          dist1 < dist2 ? (distance = dist1, hit->SetPosition(fin_intersection1), hit->SetPhi(phi1)) : (distance = dist2, hit->SetPosition(fin_intersection2), hit->SetPhi(phi2));

          if (distance < 10)
            skewhitlist2.AddHit(hit);
        }
      }
      fLineHisto->Reset();
      // see the hits
      TVector3 position, positionk;
      double phi, phik;
      for (int jhit = 0; jhit < skewhitlist2.GetNofHits(); jhit++) {
        hit = skewhitlist2.GetHit(jhit);
        if (!hit)
          continue;

        int solutions = 0;
        if (hit->IsStt())
          solutions = 2;
        else
          solutions = 1;

        for (int khit = jhit + 1; khit < skewhitlist2.GetNofHits(); khit++) {
          PndTrkHit *hitk = skewhitlist2.GetHit(khit);
          if (!hitk)
            continue;

          int ksolutions = 0;
          if (hitk->IsStt())
            ksolutions = 2;
          else
            ksolutions = 1;

          for (int jsol = 0; jsol < solutions; jsol++) {
            if (solutions == 1) {
              position = hit->GetPosition();
              phi = hit->GetPhi();
            } else if (jsol == 0) {
              position = ((PndTrkSkewHit *)hit)->GetIntersection1();
              phi = ((PndTrkSkewHit *)hit)->GetPhi1();
            } else {
              position = ((PndTrkSkewHit *)hit)->GetIntersection2();
              phi = ((PndTrkSkewHit *)hit)->GetPhi2();
            }

            for (int ksol = 0; ksol < ksolutions; ksol++) {
              if (ksolutions == 1) {
                positionk = hitk->GetPosition();
                phik = hitk->GetPhi();
              } else if (ksol == 0) {
                positionk = ((PndTrkSkewHit *)hitk)->GetIntersection1();
                phik = ((PndTrkSkewHit *)hitk)->GetPhi1();
              } else {
                positionk = ((PndTrkSkewHit *)hitk)->GetIntersection2();
                phik = ((PndTrkSkewHit *)hitk)->GetPhi2();
              }

              double cost = (positionk.Z() - position.Z()) / TMath::Sqrt((phi - phik) * (phi - phik) + (positionk.Z() - position.Z()) * (positionk.Z() - position.Z()));
              double theta = TMath::ACos(cost);
              double r1 = phi * cost + position.Z() * TMath::Sin(theta);
              double r2 = phik * cost + positionk.Z() * TMath::Sin(theta);
              double r = r1;
              if (fabs(r1 - r2) > 1.e-9) {
                theta = -TMath::ACos(cost);
                r = phi * cost + position.Z() * TMath::Sin(theta);
              }
              fLineHisto->Fill(theta * TMath::RadToDeg(), r);
            }
          }
        }
      }

      bin = fLineHisto->GetMaximumBin();
      fLineHisto->GetBinXYZ(bin, binx, biny, binz);
      tpeak = fLineHisto->GetXaxis()->GetBinCenter(binx);
      rpeak = fLineHisto->GetYaxis()->GetBinCenter(biny);
      //  cout << "tpeak " << tpeak << " rpeak " << rpeak << endl;
      double fitm8 = -TMath::Cos(tpeak * TMath::DegToRad()) / TMath::Sin(tpeak * TMath::DegToRad());
      double fitq8 = rpeak / TMath::Sin(tpeak * TMath::DegToRad());

      if (fDisplayOn) {
        display->cd(3);
        fLineHisto->Draw("colz");
        display->cd(4);
        TLine *line = new TLine(-360, -360 * fitm8 + fitq8, 360, 360 * fitm8 + fitq8);
        line->SetLineColor(3);
        line->Draw("SAME");
        display->Update();
        display->Modified();
        char goOnChar;
        cin >> goOnChar;
      }

      // finalize the cluster
      for (int jhit = 0; jhit < skewhitlist2.GetNofHits(); jhit++) {
        hit = skewhitlist2.GetHit(jhit);
        if (fFinalCluster->DoesContain(hit) == kFALSE)
          fFinalCluster->AddHit((PndTrkHit *)hit);
      }
      fFinalCluster->Sort();
      if (fFinalCluster->GetHit(0)->IsGem()) {
        if (fFinalCluster->GetHit(1)->GetPosition().Z() < fFinalCluster->GetHit(0)->GetPosition().Z())
          fFinalCluster->ReverseSort();
      }

      //       cout << endl;
      for (int jhit = 0; jhit < fFinalCluster->GetNofHits(); jhit++) {
        hit = fFinalCluster->GetHit(jhit);
        //	cout << "final hit " << hit->GetHitID() << " " << hit->GetDetectorID() << " " << hit->GetSortVariable() << endl;
      }

      // ................ final track ......................
      PndTrkTrack finalfwdtrack(fFinalCluster, xc6, yc6, R6);
      // compute charge
      finalfwdtrack.ComputeCharge();

      // last two parameters in real plane
      double tanl = -finalfwdtrack.GetCharge() * fitm8 * (180. / TMath::Pi()) / R6;
      double z0 = fitq8;
      finalfwdtrack.SetZ0(z0);
      finalfwdtrack.SetTanL(tanl);
      fTrackList->AddTrack(&finalfwdtrack);
      // for(int jtrk = 0; jtrk < fTrack //[R.K. 01/2017] unused variableList->GetNofTracks(); jtrk++) {
      // PndTrkTrack *tmptrack = fTrackList->GetTrack(jtrk); //[R.K. 01/2017] unused variable
      //} //[R.K. 01/2017] unused variable
    }
  }

  //   fDisplayOn = kTRUE;
  if (fDisplayOn) {
    // char goOnChar; //[R.K. 01/2017] unused variable
    display->cd(1);
    Refresh();
  }
  // PndTrkTrack --> PndTrack
  for (int itrk = noflongtracks; itrk < fTrackList->GetNofTracks(); itrk++) {
    PndTrkTrack *track = fTrackList->GetTrack(itrk);
    //       cout << "- ----------------------------------- track green " << track->GetRadius() << endl;

    PndTrack theTrack = track->ConvertToPndTrack();

    TClonesArray &clref1 = *fTrackArray;
    Int_t size = clref1.GetEntriesFast();
    PndTrack *outputtrack = new (clref1[size]) PndTrack(theTrack.GetParamFirst(), theTrack.GetParamLast(), theTrack.GetTrackCand());
    outputtrack->SetFlag(222);
    TClonesArray &clref2 = *fTrackCandArray;
    size = clref2.GetEntriesFast();
    PndTrackCand *outputtrackcand = new (clref2[size]) PndTrackCand(theTrack.GetTrackCand());

    TClonesArray &clref3 = *fTrkTrackArray;
    size = clref3.GetEntriesFast();
    new (clref3[size]) PndTrkTrack(*track); // PndTrkTrack*outputtrktrack =  //[R.K.03/2017] unused variable

    //     cout << "\033[1;31m RECO R " << outputtrktrack->GetRadius() <<  ", " << outputtrack->GetParamFirst().GetMomentum().Perp() / 0.006 << "\033[0m" << endl;

    //     cout << "MOM FIRST: TOT, PT, PL " << outputtrack->GetParamFirst().GetMomentum().Mag() << " " << outputtrack->GetParamFirst().GetMomentum().Perp() << " " <<
    //     outputtrack->GetParamFirst().GetMomentum().Z() << endl; cout << "MOM LAST: TOT, PT, PL " << outputtrack->GetParamLast().GetMomentum().Mag() << " " <<
    //     outputtrack->GetParamLast().GetMomentum().Perp() << " " << outputtrack->GetParamLast().GetMomentum().Z() << endl;

    if (fDisplayOn) {
      char goOnChar;
      display->cd(1);
      track->Draw(kGreen);
      track->GetCluster().LightUp();

      cout << "TRACK " << itrk << endl;
      cout << "MOM FIRST: TOT, PT, PL " << outputtrack->GetParamFirst().GetMomentum().Mag() << " " << outputtrack->GetParamFirst().GetMomentum().Perp() << " "
           << outputtrack->GetParamFirst().GetMomentum().Z() << " nofhits " << outputtrackcand->GetNHits() << endl;
      cout << "X, Y, R " << track->GetCenter().X() << " " << track->GetCenter().Y() << " " << track->GetRadius() << endl;
      //       cout << "Z0, TANL " << track->GetZ0() << " " << track->GetTanL() << endl;
      track->ComputeCharge();

      cout << "CHARGE " << track->GetCharge() << endl;
      display->Update();
      display->Modified();
      cin >> goOnChar;
    }
  }

  if (fDisplayOn) {
    char goOnChar;
    cin >> goOnChar;
    display->Update();
    display->Modified();
  }

  //   cout << "HOW MANY TRACKS? " <<  fTrackArray->GetEntriesFast() << endl;
  //   for(int itrk = 0; itrk < fTrackArray->GetEntriesFast(); itrk++) {
  //     PndTrack *trk = (PndTrack*) fTrackArray->At(itrk);
  //     cout << "TRACK " << itrk << " HAS FLAG " << trk->GetFlag() << endl;
  //   }

  Reset();
  fFinalCluster->Clear("C");
  fTrackList->Clear("C");
  fConformalHitList->Clear("C");
  //   delete indivtracklist;
}

void PndTrkTrackFinder::Reset()
{

  if (fDisplayOn) {
    char goOnChar;
    display->Update();
    display->Modified();
    cout << "Finish? ";
    cin >> goOnChar;
    cout << "FINISH" << endl;
  }

  if (fInitDone) {
    stthitlist->Clear();
    mvdpixhitlist->Clear();
    mvdstrhitlist->Clear();
    gemhitlist->Clear();
    scithitlist->Clear();

    //   if(stthitlist)    delete stthitlist;
    //     if(mvdpixhitlist) delete mvdpixhitlist;
    //     if(mvdstrhitlist) delete mvdstrhitlist;
    //     if(scithitlist)   delete scithitlist;
    //     if(gemhitlist)    delete gemhitlist;

    if (fTimer) {
      fTimer->Stop();
      fTime += fTimer->RealTime();

      if (fVerbose > 0)
        cerr << fEventCounter << " Real time " << fTime << " s" << endl;
    }
  }

  fIndivisibleHitList->Clear();
  fInitDone = kFALSE;
}

// void PndTrkTrackFinder::Apollonius(PndTrkCluster *cluster, double *X, double *Y, double *R)
void PndTrkTrackFinder::Apollonius(PndTrkCluster *cluster, std::vector<double> &X, std::vector<double> &Y, std::vector<double> &R)
{

  if (fDisplayOn) {
    TH2F *h2 = new TH2F("h2", "", 100, -70, 70, 100, -70, 70);
    display->cd(2);
    h2->Draw();

    display->Update();
    display->Modified();
  }

  int ncircles = cluster->GetNofHits();
  double circle[ncircles][3];

  for (int i = 0; i < ncircles; i++) {
    PndTrkHit *hit = cluster->GetHit(i);
    double xc = hit->GetPosition().X();
    double yc = hit->GetPosition().Y();
    double rc = hit->GetIsochrone();

    circle[i][0] = xc;
    circle[i][1] = yc;
    circle[i][2] = rc;
    //      cout << xc << " " << yc << "  " << rc << endl;

    if (fDisplayOn) {
      // char goOnChar; //[R.K. 01/2017] unused variable
      display->cd(2);
      TArc *arc = new TArc(xc, yc, rc);
      arc->SetFillStyle(0);
      arc->Draw("SAME");
      display->Update();
      display->Modified();
    }
  }

  int counter = 0;
  for (int a = 0; a < ncircles; a++) {
    for (int b = a + 1; b < ncircles; b++) {
      for (int c = b + 1; c < ncircles; c++) {
        for (int csign1 = -1; csign1 <= 1; csign1 += 2) {
          for (int csign2 = -1; csign2 <= 1; csign2 += 2) {
            double avar = 2 * (circle[a][0] - circle[b][0]);
            double bvar = 2 * (circle[a][1] - circle[b][1]);
            double cvar = 2 * (csign1 * circle[a][2] + csign2 * circle[b][2]);
            double dvar = (circle[a][0] * circle[a][0] + circle[a][1] * circle[a][1] - circle[a][2] * circle[a][2]) -
                          (circle[b][0] * circle[b][0] + circle[b][1] * circle[b][1] - circle[b][2] * circle[b][2]);

            for (int csignp = -1; csignp <= 1; csignp += 2) {
              double avarp = 2 * (circle[a][0] - circle[c][0]);
              double bvarp = 2 * (circle[a][1] - circle[c][1]);
              double cvarp = 2 * (csign1 * circle[a][2] + csignp * circle[c][2]);
              double dvarp = (circle[a][0] * circle[a][0] + circle[a][1] * circle[a][1] - circle[a][2] * circle[a][2]) -
                             (circle[c][0] * circle[c][0] + circle[c][1] * circle[c][1] - circle[c][2] * circle[c][2]);

              // ======================================
              double thisq = (dvar * bvarp - dvarp * bvar) / (avar * bvarp - avarp * bvar);
              double thism = (bvar * cvarp - bvarp * cvar) / (avar * bvarp - avarp * bvar);

              double thisp = (avar * dvarp - avarp * dvar) / (avar * bvarp - avarp * bvar);
              double thiss = (cvar * avarp - cvarp * avar) / (avar * bvarp - avarp * bvar);
              //	    cout << thism  << " " << thisq << " " << thiss << " " << thisp << endl;
              // ======================================
              // thisa x**2 + 2 * thisb x + thisc = 0

              double thisa = thism * thism + thiss * thiss - 1;
              double thisb = thism * (thisq - circle[a][0]) + thiss * (thisp - circle[a][1]) - csign1 * circle[a][2];
              double thisc = (thisq - circle[a][0]) * (thisq - circle[a][0]) + (thisp - circle[a][1]) * (thisp - circle[a][1]) - circle[a][2] * circle[a][2];

              if ((thisb * thisb - thisa * thisc) < 0)
                continue;
              double r = (-thisb + TMath::Sqrt(thisb * thisb - thisa * thisc)) / thisa;
              // 	      cout << "r1 " << r << endl;
              if (r < 0) {
                r = (-thisb - TMath::Sqrt(thisb * thisb - thisa * thisc)) / thisa;
                // 	cout << "r2 " << r << endl;
              }

              if (r < 0)
                continue; // CHECK

              //  R[counter] = r;
              // 	      X[counter] = thism * R[counter] + thisq;
              // 	      Y[counter]  = thiss * R[counter] + thisp;

              R.push_back(r);
              X.push_back(thism * R[counter] + thisq);
              Y.push_back(thiss * R[counter] + thisp);
              cout << R[counter] << " " << X[counter] << " " << Y[counter] << endl;
              //  if(fDisplayOn) {
              // 		char goOnChar;
              // 		display->cd(2);
              // 		TArc *arc0 = new TArc(X[counter], Y[counter], R[counter]);
              // 		arc0->SetFillStyle(0);
              // 		arc0->SetLineColor(2);
              // 		arc0->Draw("SAME");
              // 		display->Update();
              // 		display->Modified();
              // 		//	cin >> goOnChar;
              // 	      }

              //     cout << "ctr/dist/R " << counter << " " << TMath::Sqrt((circle[a][0] - X[counter]) * (circle[a][0] - X[counter]) + (circle[a][1] - Y[counter]) * (circle[a][1]
              //     - Y[counter])) - R[counter] << " " << R[counter] << endl;
              counter++;
              //  cout << endl;
            }
          }
        }
      }
    }
  }
}

void PndTrkTrackFinder::CircleBy3Points(PndTrkHit *hit1, PndTrkHit *hit2, PndTrkHit *hit3, double &X, double &Y, double &R)
{

  int ncircles = 3;
  double circle[ncircles][3];

  //   cout << "circle 1 by 3 points " << hit1->GetPosition().X() << " " << hit1->GetPosition().Y() << endl;
  //   cout << "circle 2 by 3 points " << hit2->GetPosition().X() << " " << hit2->GetPosition().Y() << endl;
  //   cout << "circle 3 by 3 points " << hit3->GetPosition().X() << " " << hit3->GetPosition().Y() << endl;

  circle[0][0] = hit1->GetPosition().X();
  circle[0][1] = hit1->GetPosition().Y();
  circle[0][2] = hit1->GetIsochrone();

  circle[1][0] = hit2->GetPosition().X();
  circle[1][1] = hit2->GetPosition().Y();
  circle[1][2] = hit2->GetIsochrone();

  circle[2][0] = hit3->GetPosition().X();
  circle[2][1] = hit3->GetPosition().Y();
  circle[2][2] = hit3->GetIsochrone();

  // in case of alignement ad an error CHECK
  if ((circle[1][1] - circle[0][1]) / (circle[1][0] - circle[0][0]) - (circle[2][1] - circle[0][1]) / (circle[2][0] - circle[0][0]) < 0.0001) {
    circle[0][0] += 0.01;
    circle[0][1] += 0.01;
  }

  for (int a = 0; a < ncircles; a++) {
    for (int b = a + 1; b < ncircles; b++) {
      for (int c = b + 1; c < ncircles; c++) {
        Y = 0.5 *
            ((circle[a][0] - circle[b][0]) * (circle[a][0] * circle[a][0] - circle[c][0] * circle[c][0] + circle[a][1] * circle[a][1] - circle[c][1] * circle[c][1]) -
             (circle[a][0] - circle[c][0]) * (circle[a][0] * circle[a][0] - circle[b][0] * circle[b][0] + circle[a][1] * circle[a][1] - circle[b][1] * circle[b][1])) /
            ((circle[a][1] - circle[c][1]) * (circle[a][0] - circle[b][0]) - (circle[a][1] - circle[b][1]) * (circle[a][0] - circle[c][0]));

        if (circle[a][0] != circle[b][0]) {
          X = 0.5 * (circle[a][0] * circle[a][0] - circle[b][0] * circle[b][0] + circle[a][1] * circle[a][1] - circle[b][1] * circle[b][1]) / (circle[a][0] - circle[b][0]) -
              Y * (circle[a][1] - circle[b][1]) / (circle[a][0] - circle[b][0]);
        } else if (circle[a][0] != circle[c][0]) {
          X = 0.5 * (circle[a][0] * circle[a][0] - circle[c][0] * circle[c][0] + circle[a][1] * circle[a][1] - circle[c][1] * circle[c][1]) / (circle[a][0] - circle[c][0]) -
              Y * (circle[a][1] - circle[c][1]) / (circle[a][0] - circle[c][0]);
        }

        R = TMath::Sqrt((circle[a][0] - X) * (circle[a][0] - X) + (circle[a][1] - Y) * (circle[a][1] - Y));
      }
    }
  }
}

void PndTrkTrackFinder::Apollonius(PndTrkHit *hit1, PndTrkHit *hit2, PndTrkHit *hit3, std::vector<double> &X, std::vector<double> &Y, std::vector<double> &R)
{

  int ncircles = 3;
  double circle[ncircles][3];

  circle[0][0] = hit1->GetPosition().X();
  circle[0][1] = hit1->GetPosition().Y();
  circle[0][2] = hit1->GetIsochrone();

  circle[1][0] = hit2->GetPosition().X();
  circle[1][1] = hit2->GetPosition().Y();
  circle[1][2] = hit2->GetIsochrone();

  circle[2][0] = hit3->GetPosition().X();
  circle[2][1] = hit3->GetPosition().Y();
  circle[2][2] = hit3->GetIsochrone();

  int counter = 0;
  for (int a = 0; a < ncircles; a++) {
    for (int b = a + 1; b < ncircles; b++) {
      for (int c = b + 1; c < ncircles; c++) {
        for (int csign1 = -1; csign1 <= 1; csign1 += 2) {
          for (int csign2 = -1; csign2 <= 1; csign2 += 2) {
            double avar = 2 * (circle[a][0] - circle[b][0]);
            double bvar = 2 * (circle[a][1] - circle[b][1]);
            double cvar = 2 * (csign1 * circle[a][2] + csign2 * circle[b][2]);
            double dvar = (circle[a][0] * circle[a][0] + circle[a][1] * circle[a][1] - circle[a][2] * circle[a][2]) -
                          (circle[b][0] * circle[b][0] + circle[b][1] * circle[b][1] - circle[b][2] * circle[b][2]);

            for (int csignp = -1; csignp <= 1; csignp += 2) {
              double avarp = 2 * (circle[a][0] - circle[c][0]);
              double bvarp = 2 * (circle[a][1] - circle[c][1]);
              double cvarp = 2 * (csign1 * circle[a][2] + csignp * circle[c][2]);
              double dvarp = (circle[a][0] * circle[a][0] + circle[a][1] * circle[a][1] - circle[a][2] * circle[a][2]) -
                             (circle[c][0] * circle[c][0] + circle[c][1] * circle[c][1] - circle[c][2] * circle[c][2]);

              // ======================================
              double thisq = (dvar * bvarp - dvarp * bvar) / (avar * bvarp - avarp * bvar);
              double thism = (bvar * cvarp - bvarp * cvar) / (avar * bvarp - avarp * bvar);

              double thisp = (avar * dvarp - avarp * dvar) / (avar * bvarp - avarp * bvar);
              double thiss = (cvar * avarp - cvarp * avar) / (avar * bvarp - avarp * bvar);
              //	    cout << thism  << " " << thisq << " " << thiss << " " << thisp << endl;
              // ======================================
              // thisa x**2 + 2 * thisb x + thisc = 0

              double thisa = thism * thism + thiss * thiss - 1;
              double thisb = thism * (thisq - circle[a][0]) + thiss * (thisp - circle[a][1]) - csign1 * circle[a][2];
              double thisc = (thisq - circle[a][0]) * (thisq - circle[a][0]) + (thisp - circle[a][1]) * (thisp - circle[a][1]) - circle[a][2] * circle[a][2];

              if ((thisb * thisb - thisa * thisc) < 0)
                continue;
              double r = (-thisb + TMath::Sqrt(thisb * thisb - thisa * thisc)) / thisa;
              // 	      cout << "r1 " << r << endl;
              if (r < 0) {
                r = (-thisb - TMath::Sqrt(thisb * thisb - thisa * thisc)) / thisa;
                // 	cout << "r2 " << r << endl;
              }

              if (r < 0)
                continue; // CHECK

              //  R[counter] = r;
              // 	      X[counter] = thism * R[counter] + thisq;
              // 	      Y[counter]  = thiss * R[counter] + thisp;

              R.push_back(r);
              X.push_back(thism * R[counter] + thisq);
              Y.push_back(thiss * R[counter] + thisp);
              cout << R[counter] << " " << X[counter] << " " << Y[counter] << endl;
              //  if(fDisplayOn) {
              // 		char goOnChar;
              // 		display->cd(2);
              // 		TArc *arc0 = new TArc(X[counter], Y[counter], R[counter]);
              // 		arc0->SetFillStyle(0);
              // 		arc0->SetLineColor(2);
              // 		arc0->Draw("SAME");
              // 		display->Update();
              // 		display->Modified();
              // 		//	cin >> goOnChar;
              // 	      }

              //     cout << "ctr/dist/R " << counter << " " << TMath::Sqrt((circle[a][0] - X[counter]) * (circle[a][0] - X[counter]) + (circle[a][1] - Y[counter]) * (circle[a][1]
              //     - Y[counter])) - R[counter] << " " << R[counter] << endl;
              counter++;
              //  cout << endl;
            }
          }
        }
      }
    }
  }
}

// ============================================================================================
// DISPLAY  ***********************************************************************************
// ============================================================================================

void PndTrkTrackFinder::Refresh()
{
  // CHECK
  // char goOnChar; //[R.K. 01/2017] unused variable
  //  cout << "Refresh?" << endl;
  //  cin >> goOnChar;
  //  cout << "REFRESHING" << endl;
  DrawGeometry();
  if (fVerbose)
    cout << "Refresh stt" << endl;
  DrawHits(stthitlist);
  if (fVerbose)
    cout << "Refresh pixel" << endl;
  DrawHits(mvdpixhitlist);
  if (fVerbose)
    cout << "Refresh strip" << endl;
  DrawHits(mvdstrhitlist);
  if (fVerbose)
    cout << "Refresh scit" << endl;
  DrawHits(scithitlist);
  if (fVerbose)
    cout << "Refresh gem" << endl;
  DrawHits(gemhitlist);
  if (fVerbose)
    cout << "Refresh stop" << endl;
}

void PndTrkTrackFinder::DrawLegendreHisto()
{
  display->cd(3);
  legendre->Draw();
  display->Update();
  display->Modified();
}

void PndTrkTrackFinder::RefreshConf()
{ // CHECK delete
  // CHECK
  // char goOnChar;
  //  cout << "RefreshConf?" << endl;
  //  cin >> goOnChar;
  //  cout << "REFRESHING CONF" << endl;
}

void PndTrkTrackFinder::DrawHits(PndTrkHitList *hitlist)
{
  display->cd(1);
  hitlist->Draw();
  display->Update();
  display->Modified();
}

void PndTrkTrackFinder::DrawGeometry()
{

  if (hxy == nullptr)
    hxy = new TH2F("hxy", "xy plane", 110, -55, 55, 110, -55, 55);
  else
    hxy->Reset();
  display->cd(1);
  hxy->SetStats(kFALSE);
  hxy->Draw();

  // draw all the tubes
  for (int itube = 1; itube < fTubeArray->GetEntriesFast(); itube++) {
    PndSttTube *tube = (PndSttTube *)fTubeArray->At(itube);
    if (tube->IsParallel()) {
      TArc *arc = new TArc(tube->GetPosition().X(), tube->GetPosition().Y(), 0.5);
      arc->SetFillStyle(0);
      arc->SetLineColor(kCyan - 10);
      //       if(tube->GetLayerID() == 0 || tube->GetLayerID() == 7 || tube->GetLayerID() == 16 || tube->GetLayerID() == 20) arc->SetFillColor(kRed);
      //       else         arc->SetFillStyle(0);

      arc->Draw("SAME");
    } else {
      TMarker *mrk = new TMarker(tube->GetPosition().X(), tube->GetPosition().Y(), 6);

      mrk->SetMarkerColor(kCyan - 10);
      mrk->Draw("SAME");
    }
  }
  // ............................

  display->Update();
  display->Modified();
}

void PndTrkTrackFinder::DrawGeometryConf(double x1, double x2, double y1, double y2)
{
  // CHECK
  // char goOnChar; //[R.K. 01/2017] unused variable
  //  cout << "DRAWING GEOMETRY CONF" << endl;
  // cin >> goOnChar;

  // CHECK previous calculations, now not used;
  if (huv == nullptr)
    huv = new TH2F("huv", "uv plane", 100, x1, x2, 100, y1, y2);
  else {
    huv->Reset();
    huv->GetXaxis()->SetLimits(x1, x2);
    huv->GetYaxis()->SetLimits(y1, y2);
  }
  display->cd(2);
  huv->Draw();
  display->Update();
  display->Modified();
}

// draw lists ---------
void PndTrkTrackFinder::DrawLists()
{
  char goOnChar;
  //   cout << "DrawLists" << endl;
  //   cin >> goOnChar;
  //   Refresh();

  // for(int i = 0; i < fHitMap->GetStandalone().GetEntriesFast(); i++) {  //[R.K. 03/2017] unused variable
  // PndTrkHit *hitA = (PndTrkHit*) fHitMap->GetStandalone().At(i); //[R.K. 03/2017] unused variable
  ////    hitA->DrawTube(kGreen); //[R.K. 03/2017] unused variable
  //} //[R.K. 03/2017] unused variable
  // for(int i = 0; i < fHitMap->GetSeeds().GetEntriesFast(); i++) { //[R.K. 03/2017] unused variable
  // PndTrkHit *hitA = (PndTrkHit*) fHitMap->GetSeeds().At(i); //[R.K. 03/2017] unused variable
  ////    hitA->DrawTube(kRed); //[R.K. 03/2017] unused variable
  //} //[R.K. 03/2017] unused variable
  // for(int i = 0; i < fHitMap->GetCandseeds().GetEntriesFast(); i++) { //[R.K. 03/2017] unused variable
  // PndTrkHit *hitA = (PndTrkHit*) fHitMap->GetCandseeds().At(i); //[R.K. 03/2017] unused variable
  ////    hitA->DrawTube(kBlue); //[R.K. 03/2017] unused variable
  //} //[R.K. 03/2017] unused variable
  for (int i = 0; i < fHitMap->GetIndivisibles().GetEntriesFast(); i++) {
    PndTrkHit *hitA = (PndTrkHit *)fHitMap->GetIndivisibles().At(i);
    //    hitA->DrawTube(kOrange);
    TObjArray neighs = fHitMap->GetNeighboringsToHit(hitA);
    for (int j = 0; j < neighs.GetEntriesFast(); j++) {
      PndTrkHit *hitB = (PndTrkHit *)neighs.At(j);
      TObjArray neighs2 = fHitMap->GetNeighboringsToHit(hitA);
      if (neighs2.GetEntriesFast() > 2) {
        int counter = 0;
        PndSttTube *tubeB = (PndSttTube *)fTubeArray->At(hitB->GetTubeID());
        for (int k = 0; k < neighs2.GetEntriesFast(); k++) {
          PndTrkHit *hitC = (PndTrkHit *)neighs2.At(k);
          PndSttTube *tubeC = (PndSttTube *)fTubeArray->At(hitC->GetTubeID());
          cout << "tubes " << tubeB << " " << tubeC << endl;
          if (tubeB->GetLayerID() == tubeC->GetLayerID())
            continue;
          counter++;
        }
        if (counter > 2)
          continue;
      }
      //  hitB->DrawTube(kYellow);
    }
    // display->Update();
    // display->Modified();
    // cin >> goOnChar;
  }

  //     for(int i = 0; i < limits.GetEntriesFast(); i++) {
  //        PndTrkHit *hitA = (PndTrkHit*) limits.At(i);
  //        hitA->DrawTube(kYellow);
  //      }

  display->Update();
  display->Modified();
  cin >> goOnChar;
}

// draw neighborings ---------
void PndTrkTrackFinder::DrawNeighborings()
{
  char goOnChar;

  for (int ihit = 0; ihit < stthitlist->GetNofHits(); ihit++) {
    cout << "new neigh hit?" << endl;
    cin >> goOnChar;
    PndTrkHit *hit = stthitlist->GetHit(ihit);
    DrawNeighboringsToHit(hit);
    cin >> goOnChar;
  }
}

void PndTrkTrackFinder::DrawNeighboringsToHit(PndTrkHit *hit)
{

  Refresh();
  hit->DrawTube(kYellow);
  // PndSttTube *tube = (PndSttTube*) fTubeArray->At(hit->GetTubeID()); //[R.K. 01/2017] unused variable
  TObjArray neighs = fHitMap->GetNeighboringsToHit(hit);

  //  cout << "HIT " << hit->GetHitID() << "(" << hit->GetTubeID() << "/" << tube->GetLayerID() << ")" << " has " << neighs.GetEntriesFast() << " neighborings: ";
  for (int i = 0; i < neighs.GetEntriesFast(); i++) {
    PndTrkHit *hitA = (PndTrkHit *)neighs.At(i);
    hitA->DrawTube(kCyan);
    PndSttTube *tubeA = (PndSttTube *)fTubeArray->At(hitA->GetTubeID());
    cout << " " << hitA->GetHitID() << "(" << hitA->GetTubeID() << "/" << tubeA->GetLayerID() << ")";
  }
  cout << endl;
  display->Update();
  display->Modified();
}

void PndTrkTrackFinder::DrawConfHit(double u, double v, double r, int marker)
{
  display->cd(2);
  if (r >= 0) {
    TArc *arc = new TArc(u, v, r);
    arc->SetFillStyle(0);
    arc->Draw("SAME");
  } else {
    TMarker *mrk = new TMarker(u, v, marker);
    mrk->Draw("SAME");
  }
  //  display->Update();
  //   display->Modified();
}

Int_t PndTrkTrackFinder::FillConformalHitList(PndTrkCluster *cluster)
{

  // set the conformal transformation, where the
  // translation and rotation must be already set
  fConformalHitList->SetConformalTransform(conform);

  // loop over the cluster hits
  // and port them to conf plane
  for (int jhit = 0; jhit < cluster->GetNofHits(); jhit++) {
    PndTrkHit *hit = cluster->GetHit(jhit);
    if (hit == fRefHit)
      continue;
    PndTrkConformalHit chit;
    // cout << "HIT " << hit->GetHitID() << " " << hit->IsSttParallel() << " " << hit->IsSttSkew() << endl;
    if (hit->IsSttParallel() == kTRUE)
      chit = conform->GetConformalSttHit(hit);
    else
      chit = conform->GetConformalHit(hit); // CHECK
    fConformalHitList->AddHit(&chit);
    // cout << hit->GetPosition().X() << " " << hit->GetPosition().Y() << " " << hit->GetIsochrone() << " " << hit->IsSttParallel() << " " << " to CONFORMAL " << chit->GetU() << "
    // " << chit->GetV() << " " << chit->GetIsochrone() << endl;
  }
  return fConformalHitList->GetNofHits();
}

// ----------------------- FOR SECONDARIES
PndTrkHit *PndTrkTrackFinder::FindSttReferenceHit(int isec)
{
  int ntot = 0;
  if (isec == -1)
    ntot = stthitlist->GetNofHits();
  else
    ntot = stthitlist->GetNofHitsInSector(isec);

  if (ntot == 0)
    return nullptr;

  int tmphitid = -1;
  Double_t tmpiso = 1.;
  PndTrkHit *refhit = nullptr;
  for (int jhit = 0; jhit < ntot; jhit++) {
    PndTrkHit *hit = nullptr;
    if (isec == -1)
      hit = stthitlist->GetHit(jhit);
    else
      hit = stthitlist->GetHitFromSector(jhit, isec);

    if (hit->IsUsed()) {
      if (fVerbose > 1)
        cout << "STT hit " << jhit << "already used " << endl;
      continue;
    }
    if (hit->IsSttSkew())
      continue;
    if (hit->GetIsochrone() < tmpiso) {
      tmphitid = jhit;
      tmpiso = hit->GetIsochrone();
      refhit = hit;
    }
  }
  if (tmphitid == -1)
    return nullptr;

  // PndTrkHit *refhit =  &thishitlist[tmphitid];
  if (fVerbose > 1)
    cout << "STT REFERENCE HIT " << tmphitid << " " << refhit->GetIsochrone() << endl;
  return refhit;
}

PndTrkHit *PndTrkTrackFinder::FindMvdPixelReferenceHit()
{
  if (mvdpixhitlist->GetNofHits() == 0)
    return nullptr;
  // loop on mvd pix hits
  int tmphitid = -1;
  PndTrkHit *refhit = nullptr;
  for (int jhit = 0; jhit < mvdpixhitlist->GetNofHits(); jhit++) {
    PndTrkHit *hit = mvdpixhitlist->GetHit(jhit);
    if (hit->IsUsed()) {
      if (fVerbose > 1)
        cout << "already used V" << endl;
      continue;
    }
    tmphitid = jhit;
    break;
  }
  if (tmphitid == -1)
    return nullptr;
  refhit = mvdpixhitlist->GetHit(tmphitid);
  if (fVerbose > 1)
    cout << "MVD PIXEL REFERENCE HIT " << refhit->GetHitID() << endl;
  return refhit;
}

PndTrkHit *PndTrkTrackFinder::FindMvdStripReferenceHit()
{
  if (mvdstrhitlist->GetNofHits() == 0)
    return nullptr;
  // loop on mvd str hits
  int tmphitid = -1;
  PndTrkHit *refhit = nullptr;
  for (int jhit = 0; jhit < mvdstrhitlist->GetNofHits(); jhit++) {
    PndTrkHit *hit = mvdstrhitlist->GetHit(jhit);
    if (hit->IsUsed()) {
      if (fVerbose > 1)
        cout << "already used V" << endl;
      continue;
    }
    tmphitid = jhit;
    break;
  }
  if (tmphitid == -1)
    return nullptr;
  refhit = mvdstrhitlist->GetHit(tmphitid);
  if (fVerbose > 1)
    cout << "MVD STRIP REFERENCE HIT " << refhit->GetHitID() << endl;
  return refhit;
}

PndTrkHit *PndTrkTrackFinder::FindMvdReferenceHit()
{
  PndTrkHit *refhit = nullptr;
  refhit = FindMvdStripReferenceHit();
  // refhit = FindMvdPixelReferenceHit();
  if (refhit != nullptr)
    return refhit;
  //  refhit = FindMvdStripReferenceHit();
  FindMvdPixelReferenceHit();
  return refhit;
}

PndTrkHit *PndTrkTrackFinder::FindReferenceHit()
{
  PndTrkHit *refhit = nullptr;
  // refhit = FindMvdReferenceHit();
  refhit = FindSttReferenceHit();
  if (refhit != nullptr)
    return refhit;
  // refhit = FindSttReferenceHit();
  FindMvdReferenceHit();

  return refhit;
}

PndTrkHit *PndTrkTrackFinder::FindReferenceHit(PndTrkCluster *cluster)
{
  int ntot = cluster->GetNofHits();
  //  cout << "FIND REFERENCE HIT " << ntot << endl;

  if (ntot == 0)
    return nullptr;

  int tmphitid = -1;
  Double_t tmpiso = 1.;
  PndTrkHit *refhit = nullptr;
  for (int jhit = 0; jhit < ntot; jhit++) {
    PndTrkHit *hit = cluster->GetHit(jhit);

    //   if(hit->IsUsed()) {
    //       if(fVerbose > 1) cout << "STT hit " << jhit << "already used " << endl;
    //       continue; }

    if (hit->IsStt()) {
      if (hit->IsSttParallel()) {
        if (hit->GetIsochrone() < tmpiso) {
          tmphitid = jhit;
          tmpiso = hit->GetIsochrone();
          refhit = hit;
        }
      }
    } else {
      tmphitid = jhit;
      break;
    }
  }

  if (tmphitid == -1)
    return nullptr;
  refhit = cluster->GetHit(tmphitid);
  if (fVerbose > 1)
    cout << "REFERENCE HIT " << refhit->GetHitID() << " " << refhit->GetDetectorID() << endl;
  return refhit;
}

void PndTrkTrackFinder::ComputeTraAndRot(PndTrkHit *hit, Double_t &delta, Double_t trasl[2])
{

  trasl[0] = hit->GetPosition().X();
  trasl[1] = hit->GetPosition().Y();

  delta = 0.; // TMath::ATan2(hit->GetPosition().Y() - 0., hit->GetPosition().X() - 0.); // CHECK
}

void PndTrkTrackFinder::ComputePlaneExtremities(PndTrkCluster *)
{ // cluster //[R.K.03/2017] unused variable(s)
  fUmin = 1000, fVmin = 1000, fRmin = 1000;
  fUmax = -1000, fVmax = -1000, fRmax = -1000;
  double rc_of_min, rc_of_max;

  for (int ihit = 0; ihit < fConformalHitList->GetNofHits(); ihit++) {
    PndTrkConformalHit *chit = fConformalHitList->GetHit(ihit);
    double u = chit->GetU();
    double v = chit->GetV();
    double rc = chit->GetIsochrone();
    u - rc < fUmin ? fUmin = u - rc : fUmin;
    v - rc < fVmin ? fVmin = v - rc : fVmin;
    u + rc > fUmax ? fUmax = u + rc : fUmax;
    v + rc > fVmax ? fVmax = v + rc : fVmax;

    double theta1 = TMath::ATan2(v, u);
    double theta2 = theta1 + TMath::Pi();

    double r1 = u * TMath::Cos(theta1) + v * TMath::Sin(theta1);
    double r2 = u * TMath::Cos(theta2) + v * TMath::Sin(theta2);

    double rimin, rimax;
    r1 < r2 ? (rimin = r1, rimax = r2) : (rimin = r2, rimax = r1);

    rimin < fRmin ? (rc_of_min = rc, fRmin = rimin) : fRmin;
    rimax > fRmax ? (rc_of_max = rc, fRmax = rimax) : fRmax;
  }

  fRmin -= rc_of_min;
  fRmax += rc_of_max;

  // to square the conformal plane
  double du = fUmax - fUmin;
  double dv = fVmax - fVmin;
  double delta = fabs(dv - du) / 2.;
  du < dv ? (fUmin -= delta, fUmax += delta) : (fVmin -= delta, fVmax += delta);

  cout << "u_min " << fUmin << " u_max " << fUmax << endl;
  cout << "v_min " << fVmin << " v_max " << fVmax << endl;
  cout << "r_min " << fRmin << " r_max " << fRmax << endl;
  cout << "theta_min 0 theta_max 180" << endl;
}

void PndTrkTrackFinder::FillLegendreHisto(PndTrkCluster *cluster)
{
  // ---------------------------------------------------------------
  //  cout << "FILL LEGENDRE HISTO " << cluster->GetNofHits() << endl;

  for (int ihit = 0; ihit < fConformalHitList->GetNofHits(); ihit++) {
    PndTrkConformalHit *chit = fConformalHitList->GetHit(ihit);
    PndTrkHit *hit = chit->GetHit();
    if (cluster->DoesContain(hit) == kFALSE)
      continue;
    legendre->FillLegendreHisto(chit->GetU(), chit->GetV(), chit->GetIsochrone());
    if (fDisplayOn) {
      DrawConfHit(chit->GetU(), chit->GetV(), chit->GetIsochrone());
      //      cout << "conformal2: " << chit->GetU() << " " << chit->GetV() << " " << chit->GetIsochrone() << endl;
    }
  }
}

void PndTrkTrackFinder::RePrepareLegendre(PndTrkCluster *cluster)
{

  //    cout << "RESETTING LEGENDRE HISTO" << endl;
  legendre->ResetLegendreHisto();

  if (fDisplayOn) {
    RefreshConf();
    DrawGeometryConf(fUmin, fUmax, fVmin, fVmax);
  }
  // cout << "%%%%%%%%%%%%%%%%%%%% XY FINDE %%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
  FillLegendreHisto(cluster);
}

Int_t PndTrkTrackFinder::ApplyLegendre(PndTrkCluster *cluster, double &theta_max, double &r_max)
{
  RePrepareLegendre(cluster);
  return ExtractLegendre(1, theta_max, r_max);
}

Int_t PndTrkTrackFinder::ExtractLegendre(Int_t mode, double &theta_max, double &r_max)
{
  if (fDisplayOn) {
    // char goOnChar; //[R.K. 01/2017] unused variable
    //      cin >> goOnChar;
    DrawLegendreHisto();
    //    cout << "LEGENDRE (nof conf hits = " <<  conformalhitlist->GetNofHits() << ")" << endl;
    display->cd();
    //      cin >> goOnChar;
    display->Update();
    display->Modified();
  }

  // FIND MAXIMUM IN LEGENDRE HISTO

  //  legendre->ApplyThresholdLegendreHisto(0.3);
  int maxpeak = legendre->ExtractLegendreMaximum(theta_max, r_max);

  bool alreadythere = false;
  //  if(mode == 0)
  {

    if (maxpeak <= 3) {
      if (fVerbose > 1)
        cout << "\033[1;31m MAXPEAK " << maxpeak << ", BREAK NOW! \033[0m" << endl;
      return maxpeak;
    }

    for (size_t ialready = 0; ialready < fFoundPeaks.size(); ialready++) {
      std::pair<double, double> foundthetar = fFoundPeaks.at(ialready);
      double foundtheta = foundthetar.first;
      double foundr = foundthetar.second;
      // IF THIS PEAK WAS ALREADY FOUND, DELETE THE PEAK AND GO ON (TO AVOID INFINITE LOOPS)
      if (theta_max == foundtheta && r_max == foundr) {
        legendre->DeleteZoneAroundXYLegendre(theta_max, r_max);
        maxpeak = legendre->ExtractLegendreMaximum(theta_max, r_max);
        alreadythere = true;
        if (fVerbose > 0)
          cout << "OH NO! THIS PEAK IS ALREADY THERE" << endl;
        return -1;
      }
    }

    if (alreadythere == false) {
      std::pair<double, double> tr(theta_max, r_max);
      fFoundPeaks.push_back(tr);
    }
  }
  // ZOOM LEGENDRE HISTO
  legendre->SetUpZoomHisto(theta_max, r_max, 3, 0.005);
  //   cout << "THETA/R " << theta_max << " " << r_max << " maxpeak " << maxpeak << endl;

  for (int ihit = 0; ihit < fConformalHitList->GetNofHits(); ihit++) {
    PndTrkConformalHit *chit = fConformalHitList->GetHit(ihit);
    legendre->FillZoomHisto(chit->GetU(), chit->GetV(), chit->GetIsochrone());
  }

  if (mode == 0 && alreadythere == true) {
    cout << "THIS PEAK IS ALREADY THERE" << endl;
    legendre->DeleteZoneAroundXYZoom(theta_max, r_max);
  }

  legendre->ExtractZoomMaximum(theta_max, r_max); // int maxpeakzoom = //FIXME [R.K. 01/2017] unused variable
  //  cout << "THETA/R ZOOM " << theta_max << " " << r_max <<  " maxpeakzoom " << maxpeakzoom << endl;

  if (fDisplayOn) {
    // char goOnChar; //[R.K. 01/2017] unused variable
    display->cd(3);
    TMarker *mrk = new TMarker(theta_max, r_max, 29);
    mrk->Draw("SAME");
    display->cd(4);
    legendre->DrawZoom();
    mrk->Draw("SAME");
    display->Update();
    display->Modified();
    //      cin >> goOnChar;
  }

  return maxpeak;
}

void PndTrkTrackFinder::FromConformalToRealTrack(double fitm, double fitp, double &x0, double &y0, double &R)
{
  // CHECK if this needs to be kept --> change xc0 to xc etc
  // center and radius

  //  cout << "conformal fit " << fitm << " " << fitp << endl;

  Double_t xcrot0, ycrot0;
  ycrot0 = 1 / (2 * fitp);
  xcrot0 = -fitm * ycrot0;
  R = sqrt(xcrot0 * xcrot0 + ycrot0 * ycrot0);
  // re-rotation and re-traslation of xc and yc
  // rotation
  x0 = TMath::Cos(fConformalHitList->GetConformalTransform()->GetRotation()) * xcrot0 - TMath::Sin(fConformalHitList->GetConformalTransform()->GetRotation()) * ycrot0;
  y0 = TMath::Sin(fConformalHitList->GetConformalTransform()->GetRotation()) * xcrot0 + TMath::Cos(fConformalHitList->GetConformalTransform()->GetRotation()) * ycrot0;
  // traslation
  x0 += fConformalHitList->GetConformalTransform()->GetTranslation().X();
  y0 += fConformalHitList->GetConformalTransform()->GetTranslation().Y();
}

void PndTrkTrackFinder::FromConformalToRealTrackParabola(double fita, double fitb, double fitc, double &x0, double &y0, double &R, double &epsilon)
{
  // CHECK if this needs to be kept --> change xc0 to xc etc
  // center and radius

  //  cout << "conformal fit " << fita << " " << fitb << " " << fitc << endl;

  Double_t xcrot0, ycrot0;

  // center and radius
  ycrot0 = 1 / (2 * fita);
  xcrot0 = -fitb / (2 * fita);
  epsilon = -fitc * pow((1 + (fitb * fitb)), -3 / 2);
  R = epsilon + sqrt((xcrot0 * xcrot0) + (ycrot0 * ycrot0));

  // re-rotation and re-traslation of xc and yc
  // rotation
  x0 = TMath::Cos(fConformalHitList->GetConformalTransform()->GetRotation()) * xcrot0 - TMath::Sin(fConformalHitList->GetConformalTransform()->GetRotation()) * ycrot0;
  y0 = TMath::Sin(fConformalHitList->GetConformalTransform()->GetRotation()) * xcrot0 + TMath::Cos(fConformalHitList->GetConformalTransform()->GetRotation()) * ycrot0;
  // traslation
  x0 += fConformalHitList->GetConformalTransform()->GetTranslation().X();
  y0 += fConformalHitList->GetConformalTransform()->GetTranslation().Y();
}

void PndTrkTrackFinder::FromRealToConformalTrack(double x0, double y0, double, double &fitm, double &fitp)
{ // R //[R.K.03/2017] unused variable(s)
  // CHECK if this needs to be kept --> change xc0 to xc etc
  Double_t xcrot0, ycrot0;

  // traslation
  x0 -= fConformalHitList->GetConformalTransform()->GetTranslation().X();
  y0 -= fConformalHitList->GetConformalTransform()->GetTranslation().Y();

  /**
     |x|  |c -s||xr|  ---> |xr|  | c s||x|
     |y|  |s  c||yr|       |yr|  |-s c||y|
  **/

  // rotation
  xcrot0 = TMath::Cos(fConformalHitList->GetConformalTransform()->GetRotation()) * x0 + TMath::Sin(fConformalHitList->GetConformalTransform()->GetRotation()) * y0;
  ycrot0 = -TMath::Sin(fConformalHitList->GetConformalTransform()->GetRotation()) * x0 + TMath::Cos(fConformalHitList->GetConformalTransform()->GetRotation()) * y0;

  // yr = 1/ (2 p) --> p = 1 / (2 yr)
  // xr = - m / (2 p) --> m = - 2 p xr
  fitp = 1 / (2 * ycrot0);
  fitm = -2 * fitp * xcrot0;
}

void PndTrkTrackFinder::FillHitMap()
{
  TObjArray limits;
  TObjArray sector[6];
  TObjArray *neighborings = nullptr;

  //  PndTrkHit *hit, hit2;
  // Loop over all hits and look for:
  // 1 - tubes limiting sectors                               std::vector< int > limithit
  // 2 - tubes with no neighborings (will not use them)       std::vector< int > standalone
  // 3 - tubes with only 1 neighboring (will serve as seed)   std::vector< int > seeds
  // 4 - tubes with only 2 neighborings, one of which is on   std::vector< int > candseeds
  //     the same layer and has neighboring (will be candidate
  //     to serve as seed, if needed)
  // 5 - Fill the sector std::vector according to sectors     std::vector< int > sector*

  fHitMap->SetOwnerValue(kTRUE); // CHECK
  for (int ihit = 0; ihit < stthitlist->GetNofHits(); ihit++) {

    PndTrkHit *hit = stthitlist->GetHit(ihit);
    int tubeID = hit->GetTubeID();
    PndSttTube *tube = (PndSttTube *)fTubeArray->At(tubeID);
    if (tube->IsSectorLimit() == kTRUE)
      limits.Add(hit);

    neighborings = new TObjArray();

    for (int jhit = 0; jhit < stthitlist->GetNofHits(); jhit++) {
      if (ihit == jhit)
        continue;

      PndTrkHit *hit2 = stthitlist->GetHit(jhit);
      int tubeID2 = hit2->GetTubeID();

      if (tube->IsNeighboring(tubeID2) == kTRUE)
        neighborings->Add(hit2);
    }

    //    cout << "HIT: " << hit->GetHitID() << "(tube: " << hit->GetTubeID() << ") has " << neighborings->GetEntriesFast() << " hits" << endl;
    //     if(fDisplayOn) {
    //       Refresh();
    //     }

    fHitMap->AddNeighboringsToHit(hit, neighborings);
    TObjArray indiv = fHitMap->GetIndivisiblesToHit(hit);
    //    cout << "hit " << hit->GetHitID() << "(tube: " << hit->GetTubeID() << ") has " << indiv.GetEntriesFast() << endl;

    //     if(fDisplayOn) {
    //       char goOnChar;
    //       display->Update();
    //       display->Modified();
    //       cout << " go on?" << endl;
    //       cin >> goOnChar;
    //     }
  }
  //  neighborings = nullptr;
  //   delete neighborings;

  if (fDisplayOn) {
    if (1 == 1)
      DrawLists();
    if (1 == 2)
      DrawNeighborings();
    // ================================================
    Refresh();
    char goOnChar;
    for (int ihit = 0; ihit < stthitlist->GetNofHits(); ihit++) {
      PndTrkHit *stthit = stthitlist->GetHit(ihit);
      TObjArray indiv = fHitMap->GetIndivisiblesToHit(stthit);
      for (int jhit = 0; jhit < indiv.GetEntriesFast(); jhit++) {
        // PndTrkHit *stthit2 = (PndTrkHit*) indiv.At(jhit); //[R.K. 01/2017] unused variable
        // 	stthit2->Draw(kOrange);
        //  	stthit->Draw(kOrange);
      }
    }

    display->Update();
    display->Modified();
    cout << " FILLHITMAP STARTING" << endl;
    cin >> goOnChar;
    display->Update();
    display->Modified();
    // ================================================
  }
  //     cout << "PRINT INDIVISIBILE MAP" << endl;
  //     fHitMap->PrintIndivisibleMap();
}

PndTrkClusterList PndTrkTrackFinder::CreateFullClusterization()
{
  PndTrkClusterList clusterlist;

  cout << "CLUSTERIZATION <---------------" << endl;

  // get seeds *********************************************8
  TObjArray seeds = fHitMap->GetSeeds();
  TObjArray neighborings;
  int clusterizedhits = 0;
  // ----------------- loop over seeds
  for (int iseed = 0; iseed < seeds.GetEntriesFast(); iseed++) {
    PndTrkCluster *cluster = new PndTrkCluster();
    PndTrkHit *seedhit = (PndTrkHit *)seeds.At(iseed);

    // is it already used
    if (seedhit->IsUsed() == kTRUE)
      continue;

    int seedtubeID = seedhit->GetTubeID();
    // PndSttTube *seedtube = (PndSttTube*) fTubeArray->At(seedtubeID); //[R.K. 01/2017] unused variable?
    // int seedlayerID = seedtube->GetLayerID(); //[R.K. 01/2017] unused variable

    // add hit to cluster
    cluster->AddHit(seedhit);

    if (fDisplayOn) {
      display->cd(1);
      char goOnChar;
      cin >> goOnChar;
      cout << "SEED " << seedtubeID << endl;
      seedhit->Draw(kRed);
      display->Update();
      display->Modified();
    }

    //    // add cluster to clusterlist
    //    clusterlist.AddCluster(cluster);

    int nlastadded = 1, addedcounter = 0;
    // cout << "nlastadded to " << seedhit->GetHitID() << "(" << seedtubeID << ")" << " " << nlastadded << endl;
    //    if(nlastadded == 0) continue;
    while (nlastadded > 0) {
      // loop on the last nlastadded hits to this cluster
      // example: add to a 5 hits cluster: 0 1 2 3 4
      // the hits no. 5, 6, 7
      // --> nlastadded = 3 & nof hits in cluster = 5 + 3 = 8
      // 7 6 5 = 8 - 3
      // here loop from hit 8 - 1 = 7 to hit 8 - 3 = 5

      addedcounter = 0;

      // cout << "@@@@@@@@@@@@@@@@@ loop on the last " << nlastadded << " hits of cluster" << endl;
      //   for(int ihit = 0; ihit < cluster->GetNofHits(); ihit++) {
      // 	PndTrkHit *hit = cluster->GetHit(ihit);
      // 	cout << " " << hit->GetHitID() ;
      //       }
      //       cout << endl;

      int nclusterhits = cluster->GetNofHits();
      for (int iadded = nclusterhits - 1; iadded >= (nclusterhits - nlastadded); iadded--) {
        PndTrkHit *addedhit = cluster->GetHit(iadded);
        neighborings = fHitMap->GetNeighboringsToHit(addedhit);
        if (neighborings.GetEntriesFast() == 0)
          continue;
        //	cout << "hit " << addedhit->GetHitID() << "(" << addedhit->GetTubeID() << ")" << " has " << neighborings->GetEntriesFast()  << " neighborigns: " << endl;

        // loop over the neighborings and add them all
        for (int ineigh = 0; ineigh < neighborings.GetEntriesFast(); ineigh++) {
          PndTrkHit *neighhit = (PndTrkHit *)neighborings.At(ineigh);
          //  cout << " " << neighhit->GetHitID() << "(" << neighhit->GetTubeID() << ")";
          if (cluster->DoesContain(neighhit) == kTRUE) {
            //  cout << "UN-ADDED, in cluster already" << endl;
            continue;
          }
          cluster->AddHit(neighhit);
          addedcounter++;
          //  cout << " - ADDED; ";

          if (fDisplayOn) {
            display->cd(1);
            char goOnChar;
            cin >> goOnChar;
            neighhit->Draw(kGreen);
            cout << "nof hits " << cluster->GetNofHits() << endl;
            // cluster->LightUp();
            display->Update();
            display->Modified();
            cin >> goOnChar;
          }
        }
      }
      //       cout << endl;
      nlastadded = addedcounter;
    }
    //     cout << "NEXT seed " << endl;
    // add cluster to clusterlist
    if (cluster->GetNofHits() > 3) {
      clusterlist.AddCluster(cluster); // CHECK
      clusterizedhits += cluster->GetNofHits();
    }
  }

  // -----------------------------------------
  cout << "NOF TOTAL HITS " << stthitlist->GetNofHits() << " NOF CLUSTERIZED HITS " << clusterizedhits << endl;
  if (stthitlist->GetNofHits() - clusterizedhits > 6) {
    // get candseeds *********************************************8
    TObjArray candseeds = fHitMap->GetCandseeds();
    // ----------------- loop over cand seeds

    cout << "we have " << candseeds.GetEntriesFast() << " canduidate seeds" << endl;
    for (int jseed = 0; jseed < candseeds.GetEntriesFast(); jseed++) {
      PndTrkCluster *cluster = new PndTrkCluster();
      PndTrkHit *cseedhit = (PndTrkHit *)candseeds.At(jseed);

      // is it already used

      if (fDisplayOn) {
        display->cd(1);
        char goOnChar;
        cin >> goOnChar;
        cout << "CSEED " << endl;
        cseedhit->Draw(kBlue);
        display->Update();
        display->Modified();
      }

      if (cseedhit->IsUsed() == kTRUE) {
        cout << "already" << endl;
        continue;
      }

      int cseedtubeID = cseedhit->GetTubeID();
      // PndSttTube *cseedtube = (PndSttTube*) fTubeArray->At(cseedtubeID); //[R.K. 01/2017] unused variable?
      // int cseedlayerID = cseedtube->GetLayerID(); //[R.K. 01/2017] unused variable

      // add hit to cluster
      cluster->AddHit(cseedhit);

      if (fDisplayOn) {
        display->cd(1);
        char goOnChar;
        cin >> goOnChar;
        cout << "SEED " << cseedtubeID << endl;
        cseedhit->Draw(kRed);
        display->Update();
        display->Modified();
      }

      //    // add cluster to clusterlist
      //    clusterlist.AddCluster(cluster);

      int nlastadded = 1, addedcounter = 0;
      // cout << "nlastadded to " << seedhit->GetHitID() << "(" << seedtubeID << ")" << " " << nlastadded << endl;
      //    if(nlastadded == 0) continue;
      while (nlastadded > 0) {
        // loop on the last nlastadded hits to this cluster
        // example: add to a 5 hits cluster: 0 1 2 3 4
        // the hits no. 5, 6, 7
        // --> nlastadded = 3 & nof hits in cluster = 5 + 3 = 8
        // 7 6 5 = 8 - 3
        // here loop from hit 8 - 1 = 7 to hit 8 - 3 = 5

        addedcounter = 0;

        // cout << "@@@@@@@@@@@@@@@@@ loop on the last " << nlastadded << " hits of cluster" << endl;
        //   for(int ihit = 0; ihit < cluster->GetNofHits(); ihit++) {
        // 	PndTrkHit *hit = cluster->GetHit(ihit);
        // 	cout << " " << hit->GetHitID() ;
        //       }
        //       cout << endl;

        int nclusterhits = cluster->GetNofHits();
        for (int iadded = nclusterhits - 1; iadded >= (nclusterhits - nlastadded); iadded--) {
          PndTrkHit *addedhit = cluster->GetHit(iadded);
          neighborings = fHitMap->GetNeighboringsToHit(addedhit);
          if (neighborings.GetEntriesFast() == 0)
            continue;
          //	cout << "hit " << addedhit->GetHitID() << "(" << addedhit->GetTubeID() << ")" << " has " << neighborings->GetEntriesFast()  << " neighborigns: " << endl;

          // loop over the neighborings and add them all
          for (int ineigh = 0; ineigh < neighborings.GetEntriesFast(); ineigh++) {
            PndTrkHit *neighhit = (PndTrkHit *)neighborings.At(ineigh);
            //  cout << " " << neighhit->GetHitID() << "(" << neighhit->GetTubeID() << ")";
            if (cluster->DoesContain(neighhit) == kTRUE) {
              //  cout << "UN-ADDED, in cluster already" << endl;
              continue;
            }
            cluster->AddHit(neighhit);
            addedcounter++;
            //  cout << " - ADDED; ";

            if (fDisplayOn) {
              display->cd(1);
              char goOnChar;
              cin >> goOnChar;
              neighhit->Draw(kGreen);
              cout << "nof hits " << cluster->GetNofHits() << endl;
              // cluster->LightUp();
              display->Update();
              display->Modified();
              cin >> goOnChar;
            }
          }
        }
        //       cout << endl;
        nlastadded = addedcounter;
      }

      if (cluster->GetNofHits() > 3) {
        clusterlist.AddCluster(cluster); // CHECK
      }
    }
  }
  // -----------------------------------------
  return clusterlist;
}

// PndTrkClusterList PndTrkTrackFinder::CreateFullClusterization2() {
//   /**
//      PndTrkClusterList clusterlist;

//      // get seeds *********************************************8
//      TObjArray seeds = fHitMap->GetSeeds();
//      neighborings = nullptr;
//   **/
// }

Int_t PndTrkTrackFinder::CountPossibleTracks()
{
  // check how many neighboring tubes each skew
  // tube on a layer has on that same layer
  // the total number of tubes on a layer minus the number
  // of neighboring couples gives the number of tracks:
  // example 1 with 3 tracks:
  // OOO OO OOO are tube no.: 0 1 2   3 4  5 6 7
  // nof tubes on the layer = 8
  // calculation of neighborings:
  // 0 <--> 1
  // 1 <--> 2
  // 3 <--> 4
  // 5 <--> 6
  // 6 <--> 7
  // so, nof neigboging couples = 5
  // Then: noftubes (8) - nofcouples (5) = 3 tracks !!OK!!

  int nofhitsinlay[30]; // CHECK initialize this
  for (int ilay = 0; ilay < 30; ilay++)
    nofhitsinlay[ilay] = 0;
  PndTrkCluster cluster;
  for (int ihit = 0; ihit < stthitlist->GetNofHits(); ihit++) {
    PndTrkHit *hit = stthitlist->GetHit(ihit);
    PndSttTube *tube = (PndSttTube *)fTubeArray->At(hit->GetTubeID());
    hit->SetSortVariable(tube->GetLayerID());
    nofhitsinlay[tube->GetLayerID()]++;
    cluster.AddHit(hit);
    //    cout << "hit " << ihit << " " << tube->GetLayerID() << " " << nofhitsinlay[tube->GetLayerID()] << endl;
  }
  cluster.Sort();

  // for(int ihit = 0; ihit < cluster.GetNofHits(); ihit++) { //[R.K. 01/2017] unused variable?
  // PndTrkHit *hit = cluster.GetHit(ihit); //[R.K. 01/2017] unused variable?
  // PndSttTube *tube = (PndSttTube*) fTubeArray->At(hit->GetTubeID()); //[R.K. 01/2017] unused variable
  //    cout << "SORTED " << ihit << " " << hit->GetHitID() << " " << tube->GetLayerID() << endl;
  //} //[R.K. 01/2017] unused variable?

  int maxnoftracks = 1;
  int tmplayid = -1;
  int counter = 0, counter1 = 0;
  ;
  int isneigh = 0;
  // loop over cluster hits
  for (int ihit = 0; ihit < cluster.GetNofHits(); ihit++) {
    PndTrkHit *hit = cluster.GetHit(ihit);
    counter++;

    PndSttTube *tube = (PndSttTube *)fTubeArray->At(hit->GetTubeID());

    int layid = tube->GetLayerID();
    if (nofhitsinlay[layid] <= 1)
      continue;

    // new layer?
    if (layid != tmplayid) {
      /**
   int noftracks = nofhitsinlay[tmplayid] - isneigh;
   if(tmplayid != -1) cout << "CLUSTER CONTAINS @ LAYER " << tmplayid << " ACTUALLY " << nofhitsinlay[tmplayid] << " - " << isneigh << " = " << noftracks << " TRACKS" << endl;
   if(noftracks > maxnoftracks) maxnoftracks = noftracks;
      **/
      isneigh = 0;
      tmplayid = layid;
      counter1 = 0;
      //	continue; //	break;
    }
    //    cout << "hit " << ihit << " on layid " << layid << "/ " <<  nofhitsinlay[layid] << endl;

    // count processed hits
    // in this same layer
    counter1++;

    // if it is the last hit ==> all its
    // neighborings have already been taken
    // into account
    if (counter1 == nofhitsinlay[layid])
      continue;

    for (int jhit = counter; jhit < counter + nofhitsinlay[layid] - counter1; jhit++) {
      PndTrkHit *hit2 = cluster.GetHit(jhit);
      int tubeid2 = hit2->GetTubeID();
      PndSttTube *tube2 = (PndSttTube *)fTubeArray->At(tubeid2);

      if (tube->GetLayerID() != tube2->GetLayerID())
        cout << "ERROR" << tube->GetLayerID() << " " << tube2->GetLayerID() << endl;
      //      cout << "compare " << ihit << "(" << hit->GetHitID() << "- " << hit->GetTubeID() << ") with " << jhit << " (" << hit2->GetHitID() << "- " << tubeid2 << ") from " <<
      //      counter << " to " << counter + nofhitsinlay[layid] - 1 << endl;
      if (tube->IsNeighboring(tubeid2) == kTRUE) {
        isneigh++;
        //	cout << "isneigh " << isneigh << endl;
        //	// break;
      }
    }

    // if all the hits in the layer have been processed
    if (counter1 == nofhitsinlay[layid] - 1) {
      int noftracks = nofhitsinlay[layid] - isneigh;
      cout << "CLUSTER CONTAINS @ LAYER " << layid << " ACTUALLY " << nofhitsinlay[layid] << " - " << isneigh << " = " << noftracks << " TRACKS" << endl;
      if (noftracks > maxnoftracks)
        maxnoftracks = noftracks;
    }
  }

  cout << "THIS CLUSTER HAS A TOTAL OF " << maxnoftracks << " TRACKS" << endl;
  return maxnoftracks;
}

Int_t PndTrkTrackFinder::CountTracksInCluster(PndTrkCluster *cluster)
{
  return CountTracksInCluster(cluster, 0);
}

Int_t PndTrkTrackFinder::CountTracksInSkewSector(PndTrkCluster *cluster)
{
  return CountTracksInCluster(cluster, 1);
}

Int_t PndTrkTrackFinder::CountTracksInCluster(PndTrkCluster *cluster, Int_t where)
{
  // where means:
  // 0 all: parallel & skewed sectors
  // 1: only skewed

  // check how many neighboring tubes each skew
  // tube on a layer has on that same layer
  // the total number of tubes on a layer minus the number
  // of neighboring couples gives the number of tracks:
  // example 1 with 3 tracks:
  // OOO OO OOO are tube no.: 0 1 2   3 4  5 6 7
  // nof tubes on the layer = 8
  // calculation of neighborings:
  // 0 <--> 1
  // 1 <--> 2
  // 3 <--> 4
  // 5 <--> 6
  // 6 <--> 7
  // so, nof neigboging couples = 5
  // Then: noftubes (8) - nofcouples (5) = 3 tracks !!OK!!
  cout << "COUNT TRACKS IN SKEW SECTOR" << endl;
  int nofhitsinlay[30]; // CHECK initialize this
  for (int ilay = 0; ilay < 30; ilay++)
    nofhitsinlay[ilay] = 0;

  for (int ihit = 0; ihit < cluster->GetNofHits(); ihit++) {
    PndTrkHit *hit = cluster->GetHit(ihit);
    PndSttTube *tube = (PndSttTube *)fTubeArray->At(hit->GetTubeID());
    hit->SetSortVariable(tube->GetLayerID());
    nofhitsinlay[tube->GetLayerID()]++;
    //    cout << "hit " << ihit << " " << tube->GetLayerID() << " " << nofhitsinlay[tube->GetLayerID()] << endl;
  }
  cluster->Sort();

  // for(int ihit = 0; ihit < cluster->GetNofHits(); ihit++) { //[R.K. 01/2017] unused variable?
  // PndTrkHit *hit = cluster->GetHit(ihit); //[R.K. 01/2017] unused variable?
  // PndSttTube *tube = (PndSttTube*) fTubeArray->At(hit->GetTubeID()); //[R.K. 01/2017] unused variable
  //    cout << "SORTED " << ihit << " " << hit->GetHitID() << " " << tube->GetLayerID() << endl;
  //} //[R.K. 01/2017] unused variable?

  int maxnoftracks = 1;
  int tmplayid = -1;
  int counter = 0, counter1 = 0;
  ;
  int isneigh = 0;
  // loop over cluster hits
  for (int ihit = 0; ihit < cluster->GetNofHits(); ihit++) {
    PndTrkHit *hit = cluster->GetHit(ihit);
    counter++;

    // which sector?
    if (where == 1 && hit->IsSttParallel() == kTRUE)
      continue;
    PndSttTube *tube = (PndSttTube *)fTubeArray->At(hit->GetTubeID());

    int layid = tube->GetLayerID();
    if (nofhitsinlay[layid] <= 1)
      continue;

    // new layer?
    if (layid != tmplayid) {
      /**
   int noftracks = nofhitsinlay[tmplayid] - isneigh;
   if(tmplayid != -1) cout << "CLUSTER CONTAINS @ LAYER " << tmplayid << " ACTUALLY " << nofhitsinlay[tmplayid] << " - " << isneigh << " = " << noftracks << " TRACKS" << endl;
   if(noftracks > maxnoftracks) maxnoftracks = noftracks;
      **/
      isneigh = 0;
      tmplayid = layid;
      counter1 = 0;
      //	continue; //	break;
    }
    //    cout << "hit " << ihit << " on layid " << layid << "/ " <<  nofhitsinlay[layid] << endl;

    // count processed hits
    // in this same layer
    counter1++;

    // if it is the last hit ==> all its
    // neighborings have already been taken
    // into account
    if (counter1 == nofhitsinlay[layid])
      continue;

    for (int jhit = counter; jhit < counter + nofhitsinlay[layid] - counter1; jhit++) {
      PndTrkHit *hit2 = cluster->GetHit(jhit);
      int tubeid2 = hit2->GetTubeID();
      PndSttTube *tube2 = (PndSttTube *)fTubeArray->At(tubeid2);

      if (tube->GetLayerID() != tube2->GetLayerID())
        cout << "ERROR" << tube->GetLayerID() << " " << tube2->GetLayerID() << endl;
      //      cout << "compare " << ihit << "(" << hit->GetHitID() << "- " << hit->GetTubeID() << ") with " << jhit << " (" << hit2->GetHitID() << "- " << tubeid2 << ") from " <<
      //      counter << " to " << counter + nofhitsinlay[layid] - 1 << endl;
      if (tube->IsNeighboring(tubeid2) == kTRUE) {
        isneigh++;
        //	cout << "isneigh " << isneigh << endl;
        //	// break;
      }
    }

    // if all the hits in the layer have been processed
    if (counter1 == nofhitsinlay[layid] - 1) {
      int noftracks = nofhitsinlay[layid] - isneigh;
      cout << "CLUSTER CONTAINS @ LAYER " << layid << " ACTUALLY " << nofhitsinlay[layid] << " - " << isneigh << " = " << noftracks << " TRACKS" << endl;
      if (noftracks > maxnoftracks)
        maxnoftracks = noftracks;
    }
  }

  cout << "THIS CLUSTER HAS A TOTAL OF " << maxnoftracks << " TRACKS" << endl;
  return maxnoftracks;
}

Int_t PndTrkTrackFinder::ClusterToConformal(PndTrkCluster *cluster)
{
  // ================ --> TO CONFORMAL PLANE
  fConformalHitList = new PndTrkConformalHitList(); // CHECK
  // translation and rotation
  Int_t nchits = 0;
  Double_t delta = 0, trasl[2] = {0., 0.};
  if (fSecondary) {
    // translation and rotation - CHECK
    //	cout << " REFERENCE HIT " << cluster->GetNofHits() << endl;
    fRefHit = FindReferenceHit(cluster);
    if (fRefHit == nullptr) {
      //	  cout << "REFHIT " << fRefHit << endl;
      //	Reset();
      return 0;
    }
    ComputeTraAndRot(fRefHit, delta, trasl);
  }
  //
  cout << "DELTA " << delta << " TRASL " << trasl[0] << " " << trasl[1] << endl;
  conform->SetOrigin(trasl[0], trasl[1], delta);
  nchits = FillConformalHitList(cluster);

  return nchits;
}

PndTrkTrack *PndTrkTrackFinder::LegendreFit(PndTrkCluster *cluster)
{

  //  cout << "APPLY LEGENDRE =======================" << endl;
  //  cout << "nof hits " << cluster->GetNofHits() << endl;

  // reset the legendre histo for a new legendre fit
  legendre->ResetLegendreHisto();

  if (fDisplayOn) {
    RefreshConf();
    DrawGeometryConf(fUmin, fUmax, fVmin, fVmax);
  }

  // fill legendre histo with the cluster hits
  FillLegendreHisto(cluster);
  double theta_max, r_max;
  // get the peak
  int maxpeak = ExtractLegendre(1, theta_max, r_max); // CHECK mode??

  if (maxpeak < 4)
    return nullptr; // CHECK
  // from theta/r to line parameters in CONFORMAL plane
  double fitm, fitq;
  legendre->ExtractLegendreSingleLineParameters(fitm, fitq);
  if (fDisplayOn) {
    display->cd(2);
    TLine *line = new TLine(-10.07, fitq + fitm * (-10.07), 10.07, fitq + fitm * (10.07));
    line->Draw("SAME");
  }

  // from line parameters to center/radius in REAL plane
  Double_t xc, yc, R;
  FromConformalToRealTrack(fitm, fitq, xc, yc, R);
  //  cout << "\033[1;33m MAXPEAK " << maxpeak << " XR, YC, R: " << xc << " " << yc << " " << R << "\033[0m" << endl;
  //	cout << "start hit " << ihit << " " << hit->GetHitID() << " " << endsecid << " " << endlayid << endl;

  // create a track from the cluster
  PndTrkTrack *track = new PndTrkTrack(cluster, xc, yc, R);

  return track;
}

PndTrkCluster *PndTrkTrackFinder::CreateClusterAroundTrack(PndTrkTrack *track)
{

  double R = track->GetRadius();
  double xc = track->GetCenter().X();
  double yc = track->GetCenter().Y();
  double fitm, fitp;
  FromRealToConformalTrack(xc, yc, R, fitm, fitp);

  PndTrkCluster cluster = track->GetCluster();

  // create cluster depending on fitting
  double rmin = R - R * 0.05; // CHECK 5%?
  double rmax = R + R * 0.05; // "      "

  if (fDisplayOn) {
    display->cd(1);
    track->Draw(kBlue);

    TArc *arcmin = new TArc(xc, yc, rmin);
    TArc *arcmax = new TArc(xc, yc, rmax);

    arcmin->SetFillStyle(0);
    arcmax->SetFillStyle(0);
    arcmin->SetLineColor(kGreen);
    arcmax->SetLineColor(kBlue);

    //    arcmin->Draw("SAME");
    //     arcmax->Draw("SAME");

    display->Update();
    display->Modified();
    char goOnChar;
    cout << "want to go to new cluster?" << endl;
    cin >> goOnChar;
  }

  // create cluster depending on fitting
  PndTrkCluster *thiscluster = new PndTrkCluster();
  int startsecid = 1000, endsecid = -1, startlayid = 1000, endlayid = -1;
  double totaldistanceconf = 0; //,  chi2 = 0; //[R.K. 01/2017] unused variable
  // clean existing cluster
  for (int ihit = 0; ihit < cluster.GetNofHits(); ihit++) {
    PndTrkHit *hit = cluster.GetHit(ihit);
    PndTrkConformalHit chit;
    if (hit->IsSttParallel())
      chit = conform->GetConformalSttHit(hit);
    else
      chit = conform->GetConformalHit(hit); // CHECK if skew?
    double distanceconf = fabs((chit.GetV() - fitm * chit.GetU() - fitp) / TMath::Sqrt(fitm * fitm + 1));

    double distance = hit->GetXYDistance(TVector3(xc, yc, 0.));
    //    cout << "distance " << distance << " " << rmin << " " << rmax << " " << distanceconf << endl;
    if (distance <= rmax && distance >= rmin) {
      thiscluster->AddHit(hit);
      if (fDisplayOn) {
        display->cd(1);
        //	hit->DrawTube(kGreen);
        display->Update();
        display->Modified();
        //	  char goOnChar;
        //	  cout << "want to go to next hitcluster2?" << endl;
        //	  cin >> goOnChar;
      }

      PndSttTube *tube = (PndSttTube *)fTubeArray->At(hit->GetTubeID());

      if (tube->GetLayerID() < startlayid)
        startlayid = tube->GetLayerID();
      if (tube->GetSectorID() < startsecid)
        startsecid = tube->GetSectorID();
      if (tube->GetLayerID() > endlayid)
        endlayid = tube->GetLayerID();
      if (tube->GetSectorID() > endsecid)
        endsecid = tube->GetSectorID();

      //      cout << "hit " << ihit << " " << hit->GetHitID() << " " << endsecid << " " << endlayid << endl;
      totaldistanceconf += distanceconf;
    }
  }

  // double meandistanceconf = totaldistanceconf/thiscluster->GetNofHits(); //[R.K. 01/2017] unused variable

  //   cout << "START SECTOR " << startsecid << " END SECTOR " << endsecid << endl;
  //   cout << "START LAYER  " << startlayid << " END LAYER  " << endlayid << endl;

  if (fDisplayOn) {
    display->cd(1);
    thiscluster->Draw(kRed);
    display->Update();
    display->Modified();
    char goOnChar;
    cout << "want to go to next cluster1?" << endl;
    cin >> goOnChar;
  }

  if (startlayid != 0 || endlayid != 23) {
    for (int ihit = 0; ihit < stthitlist->GetNofHits(); ihit++) {
      PndTrkHit *hit = stthitlist->GetHit(ihit);
      if (cluster.DoesContain(hit))
        continue;
      PndSttTube *tube = (PndSttTube *)fTubeArray->At(hit->GetTubeID());

      double distance = hit->GetXYDistance(TVector3(xc, yc, 0.));
      if (distance <= rmax && distance >= rmin) {
        //	cout << endl;
        //	cout << "other sector " << tube->GetSectorID() << " " << tube->GetLayerID();

        if (tube->GetSectorID() == 0 || tube->GetSectorID() == 5) {
          if (startsecid != 5 && endsecid != 5 && startsecid != 0 && endsecid != 0)
            continue;
        } else if (fabs(tube->GetSectorID() - startsecid) > 1 && fabs(tube->GetSectorID() - endsecid) > 1)
          continue;

        if (tube->GetLayerID() > startlayid && tube->GetLayerID() < endlayid)
          continue;
        PndTrkConformalHit chit;
        if (hit->IsSttParallel())
          chit = conform->GetConformalSttHit(hit);
        else
          chit = conform->GetConformalHit(hit); // CHECK if skew?
        // double distanceconf = fabs((chit.GetV() - fitm * chit.GetU() - fitp)/ TMath::Sqrt(fitm * fitm + 1)); //[R.K. 01/2017] unused variable

        //	cout << "->distance " << distance << " (" << meandistanceconf << ") " << rmin << " " << rmax << " " << distanceconf << endl;

        if (fDisplayOn) {
          display->cd(1);
          //	  hit->DrawTube(kBlue);
          display->Update();
          display->Modified();
          // char goOnChar; //[R.K. 01/2017] unused variable
          cout << "want to go to next?" << endl;
          // cin >> goOnChar;
        }

        //	cout << "tubeid " << hit->GetTubeID() << " " << tube->GetLayerID() << endl;
        thiscluster->AddHit(hit);
        //	cout << " ***";
      }
    }
  }
  cout << endl;

  if (fDisplayOn) {
    display->cd(1);
    thiscluster->Draw(kRed);
    display->Update();
    display->Modified();
    char goOnChar;
    cout << "want to go to next cluster2?" << endl;
    cin >> goOnChar;
  }
  // ---------------------------

  return thiscluster;
}

Bool_t PndTrkTrackFinder::AnalyticalFit(PndTrkCluster *cluster, double xc, double yc, double R, double &fitm, double &fitq)
{

  // fit with analytical chi2 -----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~--
  fFitter->Reset();
  if (fDisplayOn) {
    display->cd(1);
    Refresh();
  }
  for (int ihit = 0; ihit < cluster->GetNofHits(); ihit++) {
    PndTrkHit *hit = cluster->GetHit(ihit);
    if (hit == fRefHit)
      continue;
    if (hit->IsSttSkew())
      continue;
    if (hit->IsSttParallel())
      IntersectionFinder(hit, xc, yc, R);

    PndTrkConformalHit chit = conform->GetConformalHit(hit);
    PndTrkConformalHit chitstt = conform->GetConformalSttHit(hit);
    double sigma = 1e-5 * hit->GetPosition().Perp();
    if (hit->IsSttParallel())
      sigma = chitstt.GetIsochrone() * hit->GetPosition().Perp(); // 0.1; // CHECK
    if (hit->IsGem())
      sigma = 0.1 * hit->GetPosition().Perp(); // CHECK
    if (hit->IsSciTil())
      sigma = 0.5 * hit->GetPosition().Perp(); // CHECK
    //       // if(chit.GetPosition().Mod() > 2) continue; // CHECK THIS OUT!

    if (TMath::IsNaN(chit.GetPosition().X()))
      continue; // prevents the nan of the ref hit
    fFitter->SetPointToFit(chit.GetPosition().X(), chit.GetPosition().Y(), sigma);
    //      cout << "set point to fit " << chit.GetHit()->GetHitID() << " " << chit.GetHit()->GetDetectorID() << " " << chit.GetPosition().X() << " " <<  chit.GetPosition().Y() <<
    //      " " << sigma << endl;
    if (fDisplayOn) {
      display->cd(1);
      TMarker *mrk = new TMarker(hit->GetPosition().X(), hit->GetPosition().Y(), 6);
      mrk->SetMarkerColor(kRed);
      mrk->Draw("SAME");

      display->cd(2);
      TMarker *mrk2 = new TMarker(chit.GetPosition().X(), chit.GetPosition().Y(), 6);
      mrk2->SetMarkerColor(kRed);
      mrk2->Draw("SAME");

      display->Update();
      display->Modified();
    }
  }

  fFitter->StraightLineFit(fitm, fitq);

  // fitq == 0 means infinite radius
  // fitm == 0

  if (fitm == 0)
    return kFALSE;

  //  cout << "previous " << xc << " " << yc << " " << R << endl;
  FromConformalToRealTrack(fitm, fitq, xc, yc, R);
  // cout << "now " << xc << " " << yc << " " << R << endl;

  if (fDisplayOn) {
    display->cd(2);
    cout << "wanna see the line?" << endl;
    TLine *line = new TLine(-10.07, fitq + fitm * (-10.07), 10.07, fitq + fitm * (10.07));
    line->SetLineColor(2);
    line->Draw("SAME");
    char goOnChar;
    display->Update();
    display->Modified();
    cin >> goOnChar;
  }

  return kTRUE;
}

void PndTrkTrackFinder::AnalyticalFit2(PndTrkCluster *cluster, double fitm, double fitp, double &fitm2, double &fitp2)
{

  // fit with analytical chi2 -----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~--
  fFitter->Reset();
  if (fDisplayOn) {
    display->cd(1);
    Refresh();
  }
  for (int ihit = 0; ihit < cluster->GetNofHits(); ihit++) {
    PndTrkHit *hit = cluster->GetHit(ihit);
    if (hit->IsSttSkew())
      continue;

    PndTrkConformalHit chit = conform->GetConformalSttHit(hit);
    IntersectionFinder(&chit, fitm, fitp);

    double sigma = chit.GetIsochrone();
    fFitter->SetPointToFit(chit.GetPosition().X(), chit.GetPosition().Y(), sigma);
    //      cout << "set point to fit " << chit.GetPosition().X() << " " <<  chit.GetPosition().Y() << endl;
    if (fDisplayOn) {
      display->cd(1);
      TMarker *mrk = new TMarker(hit->GetPosition().X(), hit->GetPosition().Y(), 6);
      mrk->SetMarkerColor(kRed);
      mrk->Draw("SAME");

      display->cd(2);
      chit.Draw(1);

      display->cd(2);
      TMarker *mrk2 = new TMarker(chit.GetPosition().X(), chit.GetPosition().Y(), 6);
      mrk2->SetMarkerColor(kRed);
      mrk2->Draw("SAME");

      display->Update();
      display->Modified();
    }
  }

  fFitter->StraightLineFit(fitm2, fitp2);

  double xc, yc, R;
  FromConformalToRealTrack(fitm, fitp, xc, yc, R);
  cout << "previous " << xc << " " << yc << " " << R << endl;
  FromConformalToRealTrack(fitm2, fitp2, xc, yc, R);
  cout << "now " << xc << " " << yc << " " << R << endl;

  if (fDisplayOn) {
    display->cd(2);
    cout << "wanna see the line?" << endl;
    TLine *line = new TLine(-10.07, fitp2 + fitm2 * (-10.07), 10.07, fitp2 + fitm2 * (10.07));
    line->SetLineColor(2);
    line->Draw("SAME");

    display->cd(1);
    TArc *aline = new TArc(xc, yc, R);
    aline->SetFillStyle(0);
    aline->SetLineColor(2);
    aline->Draw("SAME");

    char goOnChar;
    display->Update();
    display->Modified();
    cin >> goOnChar;
  }
}

Bool_t PndTrkTrackFinder::AnalyticalParabolaFit(PndTrkCluster *cluster, double xc, double yc, double R, double &fita, double &fitb, Double_t &fitc, Double_t &epsilon)
{

  // fit with analytical chi2 -----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~--
  fFitter->Reset();
  if (fDisplayOn) {
    display->cd(1);
    Refresh();
  }
  for (int ihit = 0; ihit < cluster->GetNofHits(); ihit++) {
    PndTrkHit *hit = cluster->GetHit(ihit);
    if (hit == fRefHit)
      continue;
    if (hit->IsSttSkew())
      continue;
    if (hit->IsSttParallel())
      IntersectionFinder(hit, xc, yc, R);

    PndTrkConformalHit chit = conform->GetConformalHit(hit);
    PndTrkConformalHit chitstt = conform->GetConformalSttHit(hit);
    double sigma = 1e-5;
    if (hit->IsSttParallel())
      sigma = chitstt.GetIsochrone(); // 0.1; // CHECK
    if (hit->IsGem())
      sigma = 0.1; // CHECK
    if (hit->IsSciTil())
      sigma = 0.5; // CHECK
    //       // if(chit.GetPosition().Mod() > 2) continue; // CHECK THIS OUT!

    if (TMath::IsNaN(chit.GetPosition().X()))
      continue; // prevents the nan of the ref hit
    fFitter->SetPointToFit(chit.GetPosition().X(), chit.GetPosition().Y(), sigma);
    //      cout << "set point to fit " << chit.GetHit()->GetHitID() << " " << chit.GetHit()->GetDetectorID() << " " << chit.GetPosition().X() << " " <<  chit.GetPosition().Y() <<
    //      " " << sigma << endl;
    if (fDisplayOn) {
      display->cd(1);
      TMarker *mrk = new TMarker(hit->GetPosition().X(), hit->GetPosition().Y(), 6);
      mrk->SetMarkerColor(kRed);
      mrk->Draw("SAME");

      display->cd(2);
      TMarker *mrk2 = new TMarker(chit.GetPosition().X(), chit.GetPosition().Y(), 6);
      mrk2->SetMarkerColor(kRed);
      mrk2->Draw("SAME");

      display->Update();
      display->Modified();
    }
  }

  fFitter->ParabolaFit(fita, fitb, fitc);

  // CHECK this
  if (fita == 0)
    return kFALSE;

  FromConformalToRealTrackParabola(fita, fitb, fitc, xc, yc, R, epsilon);
  // cout << "now " << xc << " " << yc << " " << R << endl;

  //   if(fDisplayOn) {
  //     display->cd(2);
  //     cout << "wanna see the line?" << endl;
  //     TLine *line = new TLine(-10.07, fitq + fitm * (-10.07), 10.07, fitq + fitm * (10.07));
  //     line->SetLineColor(2);
  //     line->Draw("SAME");
  //     char goOnChar;
  //     display->Update();
  //     display->Modified();
  //     cin >> goOnChar;
  //   }

  return kTRUE;
}

void PndTrkTrackFinder::IntersectionFinder(PndTrkConformalHit *chit, double fitm, double fitp)
{

  double xi1 = chit->GetU() + fitm * chit->GetIsochrone() / TMath::Sqrt(fitm * fitm + 1);
  double yi1 = chit->GetV() - chit->GetIsochrone() / TMath::Sqrt(fitm * fitm + 1);

  double xi2 = chit->GetU() - fitm * chit->GetIsochrone() / TMath::Sqrt(fitm * fitm + 1);
  double yi2 = chit->GetV() + chit->GetIsochrone() / TMath::Sqrt(fitm * fitm + 1);

  double xi = 0, yi = 0;

  fabs(yi1 - (fitm * xi1 + fitp)) < fabs(yi2 - (fitm * xi2 + fitp)) ? (yi = yi1, xi = xi1) : (yi = yi2, xi = xi2);

  chit->SetPosition(xi, yi);
}

void PndTrkTrackFinder::IntersectionFinder(PndTrkHit *hit, double xc, double yc, double R)
{

  TVector2 vec(xc, yc);

  // tubeID  CHECK added
  Int_t tubeID = hit->GetTubeID();
  PndSttTube *tube = (PndSttTube *)fTubeArray->At(tubeID);

  // [xp, yp] point = coordinates xy of the centre of the firing tube
  TVector2 point(tube->GetPosition().X(), tube->GetPosition().Y());
  double radius = hit->GetIsochrone();

  // the coordinates of the point are taken from the intersection
  // between the circumference from the drift time and the R radius of
  // curvature. -------------------------------------------------------
  // 2. find the intersection between the little circle and the line // R
  // 2.a
  // find the line passing throught [xc, yc] (centre of curvature) and [xp, yp] (first wire)
  // y = mx + q
  Double_t m = (point.Y() - vec.Y()) / (point.X() - vec.X());
  Double_t q = point.Y() - m * point.X();

  Double_t x1 = 0, y1 = 0, x2 = 0, y2 = 0, xb1 = 0, yb1 = 0, xb2 = 0, yb2 = 0;

  // CHECK the vertical track
  if (fabs(point.X() - vec.X()) < 1e-6) {

    // 2.b
    // intersection little circle and line --> [x1, y1]
    // + and - refer to the 2 possible intersections
    // +
    x1 = point.X();
    y1 = point.Y() + sqrt(radius * radius - (x1 - point.X()) * (x1 - point.X()));
    // -
    x2 = x1;
    y2 = point.Y() - sqrt(radius * radius - (x2 - point.X()) * (x2 - point.X()));

    // 2.c intersection between line and circle
    // +
    xb1 = vec.X();
    yb1 = vec.Y() + sqrt(R * R - (xb1 - vec.X()) * (xb1 - vec.X()));
    // -
    xb2 = xb1;
    yb2 = vec.Y() - sqrt(R * R - (xb2 - vec.X()) * (xb2 - vec.X()));

  } // END CHECK
  else {

    // 2.b
    // intersection little circle and line --> [x1, y1]
    // + and - refer to the 2 possible intersections
    // +
    x1 = (-(m * (q - point.Y()) - point.X()) + sqrt((m * (q - point.Y()) - point.X()) * (m * (q - point.Y()) - point.X()) -
                                                    (m * m + 1) * ((q - point.Y()) * (q - point.Y()) + point.X() * point.X() - radius * radius))) /
         (m * m + 1);
    y1 = m * x1 + q;
    // -
    x2 = (-(m * (q - point.Y()) - point.X()) - sqrt((m * (q - point.Y()) - point.X()) * (m * (q - point.Y()) - point.X()) -
                                                    (m * m + 1) * ((q - point.Y()) * (q - point.Y()) + point.X() * point.X() - radius * radius))) /
         (m * m + 1);
    y2 = m * x2 + q;

    // 2.c intersection between line and circle
    // +
    xb1 = (-(m * (q - vec.Y()) - vec.X()) +
           sqrt((m * (q - vec.Y()) - vec.X()) * (m * (q - vec.Y()) - vec.X()) - (m * m + 1) * ((q - vec.Y()) * (q - vec.Y()) + vec.X() * vec.X() - R * R))) /
          (m * m + 1);
    yb1 = m * xb1 + q;
    // -
    xb2 = (-(m * (q - vec.Y()) - vec.X()) -
           sqrt((m * (q - vec.Y()) - vec.X()) * (m * (q - vec.Y()) - vec.X()) - (m * m + 1) * ((q - vec.Y()) * (q - vec.Y()) + vec.X() * vec.X() - R * R))) /
          (m * m + 1);
    yb2 = m * xb2 + q;
  }

  // calculation of the distance between [xb, yb] and [xp, yp]
  Double_t distb1 = sqrt((yb1 - point.Y()) * (yb1 - point.Y()) + (xb1 - point.X()) * (xb1 - point.X()));
  Double_t distb2 = sqrt((yb2 - point.Y()) * (yb2 - point.Y()) + (xb2 - point.X()) * (xb2 - point.X()));

  // choice of [xb, yb]
  TVector2 xyb;
  if (distb1 > distb2)
    xyb.Set(xb2, yb2);
  else
    xyb.Set(xb1, yb1);

  // calculation of the distance between [x, y] and [xb. yb]
  Double_t dist1 = sqrt((xyb.Y() - y1) * (xyb.Y() - y1) + (xyb.X() - x1) * (xyb.X() - x1));
  Double_t dist2 = sqrt((xyb.Y() - y2) * (xyb.Y() - y2) + (xyb.X() - x2) * (xyb.X() - x2));

  // choice of [x, y]
  TVector2 *xy;
  if (dist1 > dist2)
    xy = new TVector2(x2, y2);
  else
    xy = new TVector2(x1, y1); // <========= THIS IS THE NEW POINT to be used for the fit

  hit->SetPosition(TVector3(xy->X(), xy->Y(), 0.0));

  delete xy;
}

// ================================== ZFINDER
PndTrkCluster PndTrkTrackFinder::CreateSkewHitList(PndTrkTrack *track)
{

  double xc = track->GetCenter().X();
  double yc = track->GetCenter().Y();
  double R = track->GetRadius();
  PndTrkCluster skewhitlist;

  // double phimin = 400, phimax = -1, zmin = 1000, zmax = -1; //[R.K. 01/2017] unused variable
  for (int ihit = 0; ihit < stthitlist->GetNofHits(); ihit++) {
    PndTrkHit *hit = stthitlist->GetHit(ihit);
    if (hit->IsSttSkew() == kFALSE)
      continue;

    int tubeID = hit->GetTubeID();
    PndSttTube *tube = (PndSttTube *)fTubeArray->At(tubeID);

    TVector3 wireDirection = tube->GetWireDirection();
    Double_t halflength = tube->GetHalfLength();

    TVector3 first = tube->GetPosition() + wireDirection * halflength;  // CHECK
    TVector3 second = tube->GetPosition() - wireDirection * halflength; // CHECK
    //    if(fDisplayOn) {
    // 	char goOnChar;
    //      	display->cd(1);
    // 	TLine *l = new TLine(first.X(), first.Y(), second.X(), second.Y());
    // 	l->SetLineColor(kBlue);
    // 	l->Draw("SAME");
    // 	display->Update();
    // 	display->Modified();
    // 	cin >> goOnChar;
    //       }
    //    double m1 = (first - second).Y()/(first - second).X();
    //    double q1 = first.Y() - m1 * first.X();

    // 1. compute intersection between the track circle and the wire
    TVector2 intersection1, intersection2;
    Int_t nofintersections = tools->ComputeSegmentCircleIntersection(TVector2(first.X(), first.Y()), TVector2(second.X(), second.Y()), xc, yc, R, intersection1, intersection2);

    if (nofintersections == 0)
      continue;
    if (nofintersections >= 2) {
      cout << "ERROR: MORE THAN 1 INTERSECTION!!" << endl;
      continue; // CHECK
    }

    if (fDisplayOn) {
      // char goOnChar; //[R.K. 01/2017] unused variable
      display->cd(1);
      TLine *l = new TLine(first.X(), first.Y(), second.X(), second.Y());
      l->SetLineColor(kBlue);
      l->Draw("SAME");

      TMarker *mrk = new TMarker(intersection1.X(), intersection1.Y(), 20);
      mrk->SetMarkerColor(kBlue);
      mrk->Draw("SAME");

      display->Update();
      display->Modified();

      //	cin >> goOnChar;
    }

    // 2. find the tangent to the track in the intersection point
    // tangent approximation
    TVector2 tangent = tools->ComputeTangentInPoint(xc, yc, intersection1);

    // 3. rotate clockwise the tangent/point/(wire, not explicitely)
    // in order to have the wire parallel to the x axis;
    // then translate everything to have the wire ON the x axis
    double beta = wireDirection.Phi();
    if (beta < 0)
      beta += TMath::Pi();
    // ... rotate the tangent
    double rtx = TMath::Cos(beta) * tangent.X() + TMath::Sin(beta) * tangent.Y();
    double rty = TMath::Cos(beta) * tangent.Y() - TMath::Sin(beta) * tangent.X();
    TVector2 rottangent(rtx, rty);
    rottangent = rottangent.Unit();
    // ... rotate the point
    double rx = TMath::Cos(beta) * intersection1.X() + TMath::Sin(beta) * intersection1.Y();
    double ry = TMath::Cos(beta) * intersection1.Y() - TMath::Sin(beta) * intersection1.X();

    // translation
    Double_t deltay = ry;
    rty -= deltay;
    ry -= deltay;

    // rotm, rotp
    Double_t rotm = rottangent.Y() / rottangent.X();
    Double_t rotp = ry - rotm * rx;

    // ellipsis
    double a = hit->GetIsochrone() * TMath::Cos(SKEW_ANGLE); // CHECK skew angle hard coded
    double b = hit->GetIsochrone();

    // center of ellipsis
    Double_t x0a, x0b, y0;
    y0 = 0.;
    x0a = (-rotp + TMath::Sqrt(b * b + a * a * rotm * rotm)) / rotm;
    x0b = (-rotp - TMath::Sqrt(b * b + a * a * rotm * rotm)) / rotm;

    // intersection point
    double intxa = (x0a * b * b - rotm * rotp * a * a) / (b * b + rotm * rotm * a * a);
    double intya = rotm * intxa + rotp;
    double intxb = (x0b * b * b - rotm * rotp * a * a) / (b * b + rotm * rotm * a * a);
    double intyb = rotm * intxb + rotp;

    // 4. retraslate/rerotate all back to the original plane
    // retranslate
    y0 += deltay;
    intya += deltay;
    intyb += deltay;

    // rerotate
    double x0anew = TMath::Cos(beta) * x0a - TMath::Sin(beta) * y0;
    double y0anew = TMath::Cos(beta) * y0 + TMath::Sin(beta) * x0a;
    double x0bnew = TMath::Cos(beta) * x0b - TMath::Sin(beta) * y0;
    double y0bnew = TMath::Cos(beta) * y0 + TMath::Sin(beta) * x0b;

    double intxanew = TMath::Cos(beta) * intxa - TMath::Sin(beta) * intya;
    double intyanew = TMath::Cos(beta) * intya + TMath::Sin(beta) * intxa;
    double intxbnew = TMath::Cos(beta) * intxb - TMath::Sin(beta) * intyb;
    double intybnew = TMath::Cos(beta) * intyb + TMath::Sin(beta) * intxb;

    intxa = intxanew;
    intya = intyanew;
    intxb = intxbnew;
    intyb = intybnew;

    // now we have x0a, y0a, center of the 1st ellipse
    // and x0b, y0b, center of the 2nd ellipse
    x0a = x0anew;
    double y0a = y0anew;
    x0b = x0bnew;
    double y0b = y0bnew;

    if (fDisplayOn) {
      // char goOnChar; //[R.K. 01/2017] unused variable
      display->cd(1);

      TEllipse *ell1 = new TEllipse(x0a, y0a, a, b, 0, 360, -beta);
      ell1->SetFillStyle(0);
      ell1->SetLineColor(4);
      ell1->Draw("SAME");
      TEllipse *ell2 = new TEllipse(x0b, y0b, a, b, 0, 360, -beta);
      ell2->SetFillStyle(0);
      ell2->SetLineColor(6);
      ell2->Draw("SAME");

      TMarker *mrkinta = new TMarker(intxa, intya, 20);
      mrkinta->SetMarkerColor(4);
      mrkinta->Draw("SAME");
      TMarker *mrkintb = new TMarker(intxb, intyb, 20);
      mrkintb->SetMarkerColor(6);
      mrkintb->Draw("SAME");
      //	 cin >> goOnChar;
    }

    // 5. calculate z coordinate for each intersection

    // calculate z0a, z0b of the center of the ellipse
    Double_t t = ((x0a + y0a) - (first.X() + first.Y())) / ((second.X() - first.X()) + (second.Y() - first.Y()));
    Double_t z0a = first.Z() + (second.Z() - first.Z()) * t;
    //    cout << "0 : calculate t, z0a " << t << " " << z0a << endl;

    t = ((x0b + y0b) - (first.X() + first.Y())) / ((second.X() - first.X()) + (second.Y() - first.Y()));
    Double_t z0b = first.Z() + (second.Z() - first.Z()) * t;

    TVector3 center1(x0a, y0a, z0a);
    TVector3 center2(x0b, y0b, z0b);
    //   if(fDisplayOn) {
    // 	char goOnChar;
    // 	display->cd(3);
    // 	//	cout << "COMPUTE Z COORDINATE" << endl;
    // 	DrawZGeometry();
    // 	TLine *linezx = new TLine(first.X(), first.Z(), second.X(), second.Z());
    // 	linezx->Draw("SAME");
    // 	TMarker *mrkza = new TMarker(x0a, z0a, 20);
    // 	mrkza->SetMarkerColor(4);
    // 	mrkza->Draw("SAME");
    // 	TMarker *mrkzb = new TMarker(x0b, z0b, 20);
    // 	mrkzb->SetMarkerColor(6);
    // 	mrkzb->Draw("SAME");
    // 	//	 cin >> goOnChar;
    //       }

    // calculate the z of the intersection ON the ellipse (CHECK this step calculations!)
    double dx = intxa - x0a;
    double dy = intya - y0a;
    TVector3 dxdy(dx, dy, 0.0);

    TVector3 tfirst = first + dxdy;
    TVector3 tsecond = second + dxdy;

    t = ((intxa + intya) - (tfirst.X() + tfirst.Y())) / ((tsecond.X() - tfirst.X()) + (tsecond.Y() - tfirst.Y()));
    double intza = tfirst.Z() + (tsecond.Z() - tfirst.Z()) * t;
    if (fDisplayOn) {
      // char goOnChar; //[R.K. 01/2017] unused variable
      display->cd(3);
      TLine *linezx1 = new TLine(tfirst.X(), tfirst.Z(), tsecond.X(), tsecond.Z());
      linezx1->SetLineStyle(1);
      linezx1->Draw("SAME");
      TMarker *mrkza1 = new TMarker(intxa, intza, 20);
      mrkza1->SetMarkerColor(kBlue - 9);
      mrkza1->Draw("SAME");
      // cin >> goOnChar;
    }

    tfirst = first - dxdy;
    tsecond = second - dxdy;

    t = ((intxb + intyb) - (tfirst.X() + tfirst.Y())) / ((tsecond.X() - tfirst.X()) + (tsecond.Y() - tfirst.Y()));
    double intzb = tfirst.Z() + (tsecond.Z() - tfirst.Z()) * t;

    TVector3 fin_intersection1(intxa, intya, intza);
    TVector3 fin_intersection2(intxb, intyb, intzb);

    //   if(fDisplayOn) {
    // 	char goOnChar;
    // 	display->cd(3);
    // 	TLine *linezx2 = new TLine(tfirst.X(), tfirst.Z(), tsecond.X(), tsecond.Z());
    // 	linezx2->SetLineStyle(1);
    // 	linezx2->Draw("SAME");
    // 	TMarker *mrkzb1 = new TMarker(intxb, intzb, 20);
    // 	mrkzb1->SetMarkerColor(kMagenta - 7);
    // 	mrkzb1->Draw("SAME");
    // 	//	 cin >> goOnChar;
    //       }
    //        int1.SetXYZ(intxa, intya, intza);
    //        int2.SetXYZ(intxb, intyb, intzb);
    //        errz = fabs(intza - intzb)/2.   ;

    // CHECK to be changed
    int trackID = 1;
    double phi1 = track->ComputePhi(fin_intersection1);
    double phi2 = track->ComputePhi(fin_intersection2);

    PndTrkSkewHit *skewhit = new PndTrkSkewHit(*hit, trackID, center1, fin_intersection1, phi1, center2, fin_intersection2, phi2, a, b, -1, beta);
    //      skewhit->Print();
    skewhitlist.AddHit(skewhit);
  }
  //     cout << "PHI: " << phimin << " " << phimax << endl;
  //     cout << "Z  : " << zmin << " " << zmax << endl;

  return skewhitlist;
}

PndTrkCluster PndTrkTrackFinder::CleanUpSkewHitList(PndTrkCluster *skewhitlist)
{

  // 6. FIRST CLEANING of the track skewed associations
  // find most probable z - CHECK what happens if a track is very fwd peaked?
  //                        TRY WITH DELTA z = COST
  //    cout  << "##################### FIND MOST PROBABLE Z" << endl;
  double phimin = 400, phimax = -1, zmin = 1000, zmax = -1;

  for (int ihit = 0; ihit < skewhitlist->GetNofHits(); ihit++) {
    PndTrkSkewHit *skewhit = (PndTrkSkewHit *)skewhitlist->GetHit(ihit);
    if (!skewhit)
      continue;
    TVector3 fin_intersection1 = skewhit->GetIntersection1();
    TVector3 fin_intersection2 = skewhit->GetIntersection2();

    double phi1 = skewhit->GetPhi1();
    double phi2 = skewhit->GetPhi2();

    if (phi1 < phimin)
      phimin = phi1;
    if (phi2 < phimin)
      phimin = phi2;
    if (fin_intersection1.Z() < zmin)
      zmin = fin_intersection1.Z();
    if (fin_intersection2.Z() < zmin)
      zmin = fin_intersection2.Z();

    if (phi1 > phimax)
      phimax = phi1;
    if (phi2 > phimax)
      phimax = phi2;
    if (fin_intersection1.Z() > zmax)
      zmax = fin_intersection1.Z();
    if (fin_intersection2.Z() > zmax)
      zmax = fin_intersection2.Z();
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~ CHECK
  }
  //     cout << "PHI: " << phimin << " " << phimax << endl;
  //     cout << "Z  : " << zmin << " " << zmax << endl;

  // DISPLAY ----------------------------
  if (fDisplayOn) {
    // char goOnChar; //[R.K. 01/2017] unused variable
    display->cd(4);
    phimin -= 30;
    phimax += 30;
    zmin -= 13;
    zmax += 13;
    DrawZGeometry(phimin, phimax, zmin, zmax);
    hzphi->SetXTitle("#phi");
    hzphi->SetYTitle("#z");
    hzphi->Draw();
    display->Update();
    display->Modified();

    for (int ihit = 0; ihit < skewhitlist->GetNofHits(); ihit++) {
      PndTrkSkewHit *skewhit = (PndTrkSkewHit *)skewhitlist->GetHit(ihit);
      if (!skewhit)
        continue;

      TVector3 fin_intersection1 = skewhit->GetIntersection1();
      TVector3 fin_intersection2 = skewhit->GetIntersection2();

      double phi1 = skewhit->GetPhi1();
      double phi2 = skewhit->GetPhi2();

      TLine *linezphi = new TLine(phi1, fin_intersection1.Z(), phi2, fin_intersection2.Z());
      // TLine *linezphi = new TLine(fin_intersection1.Z(), phi1, fin_intersection2.Z(), phi2);
      linezphi->SetLineStyle(1);
      linezphi->Draw("SAME");

      TMarker *mrkzphi1 = new TMarker(phi1, fin_intersection1.Z(), 20);
      // TMarker *mrkzphi1 = new TMarker(fin_intersection1.Z(), phi1, 20);

      mrkzphi1->SetMarkerColor(kBlue - 9);
      mrkzphi1->Draw("SAME");

      TMarker *mrkzphi2 = new TMarker(phi2, fin_intersection2.Z(), 20);
      //	 TMarker *mrkzphi2 = new TMarker(fin_intersection2.Z(), phi2, 20);
      mrkzphi2->SetMarkerColor(kMagenta - 7);
      mrkzphi2->Draw("SAME");
    }
    display->Update();
    display->Modified();
    //      cin >> goOnChar;
  }
  // DISPLAY ----------------------------

  TH1F hz("hz", "z", (zmax - zmin) / 10., zmin, zmax); // CHECK
  for (int ihit = 0; ihit < skewhitlist->GetNofHits(); ihit++) {
    PndTrkSkewHit *skewhit = (PndTrkSkewHit *)skewhitlist->GetHit(ihit);
    if (!skewhit)
      continue;
    TVector3 fin_intersection1 = skewhit->GetIntersection1();
    TVector3 fin_intersection2 = skewhit->GetIntersection2();
    hz.Fill(fin_intersection1.Z());
    hz.Fill(fin_intersection2.Z());
  }
  int maxbinz = hz.GetMaximumBin();
  double mostprobZ = hz.GetBinCenter(maxbinz);
  //    cout << mostprobZ << endl;

  // delete hits too far away ..........
  //    cout  << "##################### DELETE HITS" << endl;
  PndTrkCluster tmpskewhitlist;
  for (int ihit = 0; ihit < skewhitlist->GetNofHits(); ihit++) {
    PndTrkSkewHit *skewhit = (PndTrkSkewHit *)skewhitlist->GetHit(ihit);
    if (!skewhit)
      continue;
    TVector3 fin_intersection1 = skewhit->GetIntersection1();
    TVector3 fin_intersection2 = skewhit->GetIntersection2();
    double phi1 = skewhit->GetPhi1();
    double phi2 = skewhit->GetPhi2();

    if (fabs(fin_intersection1.Z() - mostprobZ) > 30. && fabs(fin_intersection2.Z() - mostprobZ) > 30.) {
      // 	cout << "THROW AWAY " << ihit << " " << fabs(fin_intersection1.Z() - mostprobZ) << "  " << fabs(fin_intersection2.Z() - mostprobZ) << endl;
      continue;
    }
    skewhit->SetSortVariable((phi1 + phi2) / 2.);
    tmpskewhitlist.AddHit(skewhit);
  }
  tmpskewhitlist.Sort();
  return tmpskewhitlist;
}

void PndTrkTrackFinder::DrawZGeometry(double phimin, double phimax, double zmin, double zmax)
{
  if (hzphi == nullptr)
    hzphi = new TH2F("hzphi", "z - phi plane", phimax - phimin, phimin, phimax, zmax - zmin, zmin, zmax);
  else {
    hzphi->Reset();
    hzphi->GetXaxis()->SetLimits(phimin, phimax);
    hzphi->GetYaxis()->SetLimits(zmin, zmax);
  }
  display->cd(4);
  hzphi->Draw();

  display->Update();
  display->Modified();
}

Int_t PndTrkTrackFinder::RecreateHitArrays(std::map<int, std::vector<int>> &det_to_hitids)
{

  int counter = 0;
  //  std::map< int, std::vector< int > > det_to_hitids;
  for (int itrk = 0; itrk < fPrimaryTrackArray->GetEntriesFast(); itrk++) {
    PndTrack *pritrack = (PndTrack *)fPrimaryTrackArray->At(itrk);

    //     if(pritrack->GetParamFirst().GetPosition().Perp() > 5) continue;
    //     if(pritrack->GetParamFirst().GetPosition().Z() > 5) continue;
    // //     if(pritrack->GetParamFirst().GetMomentum().Theta() * TMath::RadToDeg() < 10) continue;

    PndTrackCand *pricand = pritrack->GetTrackCandPtr();
    for (size_t ihit = 0; ihit < pricand->GetNHits(); ihit++) {
      PndTrackCandHit candhit = pricand->GetSortedHit(ihit);
      Int_t hitId = candhit.GetHitId();
      Int_t detId = candhit.GetDetId();

      std::vector<int> hitids = det_to_hitids[detId];
      if (std::find(hitids.begin(), hitids.end(), hitId) == hitids.end())
        hitids.push_back(hitId);
      det_to_hitids[detId] = hitids;
    }

    // copy the track
    TClonesArray &clref1 = *fTrackArray;
    Int_t size = clref1.GetEntriesFast();
    PndTrack *outputtrack = new (clref1[size]) PndTrack(pritrack->GetParamFirst(), pritrack->GetParamLast(), pritrack->GetTrackCand());
    outputtrack->SetFlag(333);

    TClonesArray &clref2 = *fTrackCandArray;
    size = clref2.GetEntriesFast();
    new (clref2[size]) PndTrackCand(pritrack->GetTrackCand()); // PndTrackCand *outputtrackcand =  //[R.K.03/2017] unused variable
    counter++;
  }

  /**
     cout << "WRONG CANDIDATES MAP " << det_to_hitids.size() << endl;
     std::map< int, std::vector< int > >::iterator it = det_to_hitids.begin();
     while(it != det_to_hitids.end()) {
     int detid = (*it).first;
     std::vector< int > hits = (*it).second;
     cout << "DET " << detid << " has " <<  hits.size() << " hits: ";
     for(int ihit = 0; ihit < hits.size(); ihit++) cout << " " << hits[ihit];
     cout << endl;
     it++;
     }
  **/

  return counter;
}

std::map<int, bool> PndTrkTrackFinder::PrimaryCheck(Int_t detid, std::map<int, std::vector<int>> &det_to_hitids)
{
  std::map<int, bool> hitidTousability;
  std::vector<int> hits = det_to_hitids[detid];

  TClonesArray *array = nullptr;
  if (detid == FairRootManager::Instance()->GetBranchId(fSttBranch))
    array = fSttHitArray;
  else if (detid == FairRootManager::Instance()->GetBranchId(fMvdPixelBranch))
    array = fMvdPixelHitArray;
  else if (detid == FairRootManager::Instance()->GetBranchId(fMvdStripBranch))
    array = fMvdStripHitArray;
  else if (detid == FairRootManager::Instance()->GetBranchId(fGemBranch))
    array = fGemHitArray;

  for (int ihit = 0; ihit < array->GetEntriesFast(); ihit++) {
    hitidTousability[ihit] = true;
    if (find(hits.begin(), hits.end(), ihit) != hits.end())
      hitidTousability[ihit] = false;
  }
  return hitidTousability;
}

ClassImp(PndTrkTrackFinder)
