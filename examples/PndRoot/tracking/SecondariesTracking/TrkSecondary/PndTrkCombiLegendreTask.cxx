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
// PndTrkCombiLegendreTask
//
// Class for secondary track pattern recognition
//
// authors: Lia Lavezzi - INFN Pavia (2012)
//
////////////////////////////////////////////////////////////

#include "PndTrkCombiLegendreTask.h"

// stt
#include "PndSttHit.h"
#include "PndSttPoint.h" // CHECK eliminate this afterwards (MC trurh not used in PR, used only in drawings)
#include "PndSttTube.h"
#include "PndSttMapCreator.h"
// sds
#include "PndSdsMCPoint.h" // CHECK eliminate this afterwards (MC trurh not used in PR, used only in drawings)
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
// tracking
#include "PndTrkClusterList.h"
#include "PndTrkTrackList.h"
#include "PndTrkGlpkFits.h"
#include "PndTrkClean.h"
#include "PndTrkNeighboringMap.h"
#include "PndTrkCombiLegendreTransform.h"

#include <iostream>

using namespace std;

// -----   Default constructor   -------------------------------------------
PndTrkCombiLegendreTask::PndTrkCombiLegendreTask()
  : FairTask("secondary track finder", 0), fDisplayOn(kFALSE), fPersistence(kTRUE), fUseMVDPix(kTRUE), fUseMVDStr(kTRUE), fUseSTT(kTRUE), fSecondary(kFALSE),
    fMvdPix_RealDistLimit(1000), fMvdStr_RealDistLimit(1000), fStt_RealDistLimit(1000), fMvdPix_ConfDistLimit(1000), fMvdStr_ConfDistLimit(1000), fStt_ConfDistLimit(1000),
    fInitDone(kFALSE), fUmin(-0.07), fUmax(0.07), fVmin(-0.07), fVmax(0.07), fRmin(-1.5), fRmax(1.5), fThetamin(0), fThetamax(180), fSeeMC(kFALSE), fRecoverIteration(-1)
{
  sprintf(fSttBranch, "STTHit");
  sprintf(fMvdPixelBranch, "MVDHitsPixel");
  sprintf(fMvdStripBranch, "MVDHitsStrip");
  PndGeoHandling::Instance();
}

PndTrkCombiLegendreTask::PndTrkCombiLegendreTask(int verbose)
  : FairTask("secondary track finder", verbose), fDisplayOn(kFALSE), fPersistence(kTRUE), fUseMVDPix(kTRUE), fUseMVDStr(kTRUE), fUseSTT(kTRUE), fSecondary(kFALSE),
    fMvdPix_RealDistLimit(1000), fMvdStr_RealDistLimit(1000), fStt_RealDistLimit(1000), fMvdPix_ConfDistLimit(1000), fMvdStr_ConfDistLimit(1000), fStt_ConfDistLimit(1000),
    fInitDone(kFALSE), fUmin(-0.07), fUmax(0.07), fVmin(-0.07), fVmax(0.07), fRmin(-1.5), fRmax(1.5), fThetamin(0), fThetamax(180), fSeeMC(kFALSE), fRecoverIteration(-1)
{
  sprintf(fSttBranch, "STTHit");
  sprintf(fMvdPixelBranch, "MVDHitsPixel");
  sprintf(fMvdStripBranch, "MVDHitsStrip");
  PndGeoHandling::Instance();
}

// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndTrkCombiLegendreTask::~PndTrkCombiLegendreTask() {}
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
InitStatus PndTrkCombiLegendreTask::Init()
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
    cout << "-E- PndTrkCombiLegendreTask::Init: "
         << "RootManager not instantiated, return!" << endl;
    return kFATAL;
  }

  // -- HITS -------------------------------------------------
  //
  // STT
  fSttHitArray = (TClonesArray *)ioman->GetObject(fSttBranch);
  if (!fSttHitArray) {
    cout << "-W- PndTrkCombiLegendreTask::Init: "
         << "No STTHit array, return!" << endl;
    return kERROR;
  }
  //
  // MVD PIXEL
  fMvdPixelHitArray = (TClonesArray *)ioman->GetObject(fMvdPixelBranch);
  if (!fMvdPixelHitArray) {
    std::cout << "-W- PndTrkCombiLegendreTask::Init: "
              << "No MVD Pixel hitArray, return!" << std::endl;
    return kERROR;
  }
  //
  // MVD STRIP
  fMvdStripHitArray = (TClonesArray *)ioman->GetObject(fMvdStripBranch);
  if (!fMvdStripHitArray) {
    std::cout << "-W- PndTrkCombiLegendreTask::Init: "
              << "No MVD Strip hitArray, return!" << std::endl;
    return kERROR;
  }

  fTrackArray = new TClonesArray("PndTrack");
  fTrackCandArray = new TClonesArray("PndTrackCand");
  ioman->Register("Track", "pr", fTrackArray, fPersistence);         // CHECK
  ioman->Register("TrackCand", "pr", fTrackCandArray, fPersistence); // CHECK

  // ----------------------------------------   maps of STT tubes
  fMapper = new PndSttMapCreator(fSttParameters);
  fTubeArray = fMapper->FillTubeArray();
  // ----------------------------------------------------  end map

  if (fDisplayOn) {
    display = new TCanvas("display", "display", 0, 0, 800, 800); // CHECK
    display->Divide(2, 2);
    if (fSeeMC) {
      fSttPointArray = (TClonesArray *)ioman->GetObject("STTPoint");
      fMvdPointArray = (TClonesArray *)ioman->GetObject("MVDPoint");
    }
  }

  legendre = new PndTrkLegendreTransform();
  legendrecombi = new PndTrkCombiLegendreTransform();

  if (fSecondary == kFALSE)
    legendrecombi->SetUpLegendreHisto();

  conform = new PndTrkConformalTransform();

  tools = new PndTrkTools();
  fFitter = new PndTrkFitter(fVerbose);
  fHitMap = new PndTrkNeighboringMap(fTubeArray);
  fTimer = new TStopwatch();

  return kSUCCESS;
}

// -------------------------------------------------------------------------

void PndTrkCombiLegendreTask::SetParContainers()
{
  FairRuntimeDb *rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fSttParameters = (PndGeoSttPar *)rtdb->getContainer("PndGeoSttPar");
}

// -------------------------------------------------------------------------

void PndTrkCombiLegendreTask::Initialize()
{

  stthitlist = new PndTrkSttHitList(fTubeArray);
  mvdpixhitlist = new PndTrkSdsHitList(MVDPIXEL);
  mvdstrhitlist = new PndTrkSdsHitList(MVDSTRIP);

  if (fUseSTT) {
    stthitlist->AddTCA(FairRootManager::Instance()->GetBranchId(fSttBranch), fSttHitArray);
    stthitlist->Instanciate();
  }

  if (fUseMVDPix) {
    mvdpixhitlist->AddTCA(FairRootManager::Instance()->GetBranchId(fMvdPixelBranch), fMvdPixelHitArray);
    mvdpixhitlist->InstanciatePixel();
  }

  if (fUseMVDStr) {
    mvdstrhitlist->AddTCA(FairRootManager::Instance()->GetBranchId(fMvdStripBranch), fMvdStripHitArray);
    mvdstrhitlist->InstanciateStrip();
  }

  //   conformalhitlist = new PndTrkConformalHitList();
  fFoundPeaks.clear();

  fInitDone = kTRUE;
  //  stthitlist->PrintSectors();
}

void PndTrkCombiLegendreTask::Exec(Option_t *)
{

  // ############## I N I T I A L I Z A T I O N S ##############
  fTrackArray->Delete();
  fTrackCandArray->Delete();
  if (fVerbose > 0)
    cout << "*********************** " << fEventCounter << " ***********************" << endl;

  // initialize -----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~
  Initialize();
  if (fVerbose > 1) {
    cout << "number of stt    hits " << fSttHitArray->GetEntriesFast() << endl;
    cout << "number of mvdpix hits " << fMvdPixelHitArray->GetEntriesFast() << endl;
    cout << "number of mvdstr hits " << fMvdStripHitArray->GetEntriesFast() << endl;
  }

  // initialize display -----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~-
  if (fDisplayOn) {
    Refresh();
    char goOnChar;
    if (fSeeMC) {
      DrawGeometry(3);
      for (int ipnt = 0; ipnt < fSttPointArray->GetEntriesFast(); ipnt++) {
        PndSttPoint *pnt = (PndSttPoint *)fSttPointArray->At(ipnt);
        TMarker *mrk = new TMarker(pnt->GetX(), pnt->GetY(), 7);
        mrk->SetMarkerColor(pnt->GetTrackID() + 1);
        mrk->Draw("SAME");
      }
      for (int ipnt = 0; ipnt < fMvdPointArray->GetEntriesFast(); ipnt++) {
        PndSdsMCPoint *pnt = (PndSdsMCPoint *)fMvdPointArray->At(ipnt);
        TMarker *mrk = new TMarker(pnt->GetX(), pnt->GetY(), 7);
        mrk->SetMarkerColor(pnt->GetTrackID() + 1);
        mrk->Draw("SAME");
      }
    }
    display->Update();
    display->Modified();
    cout << " STARTING" << endl;
    // cin >> goOnChar;
    display->Update();
    display->Modified();
  }

  // initialize hit map -----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~-
  fHitMap->Clear();
  FillHitMap();
  // ##########################################################

  //
  fDisplayOn = kFALSE;

  // --------- NO CLUSTERING -----------
  PndTrkCluster *globalcluster = new PndTrkCluster();
  for (int ihit = 0; ihit < stthitlist->GetNofHits(); ihit++) {
    PndTrkHit *hit = stthitlist->GetHit(ihit);
    globalcluster->AddHit(hit);
  }
  // ------------------------------------

  //  ComputeSkewedXYZ(cluster);

  PndTrkTrackList tracklist;

  // print and display cluster -----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~--
  cout << "GLOBAL CLUSTER:";
  if (fDisplayOn) {
    char goOnChar;
    //    cin >> goOnChar;
    Refresh();
    globalcluster->LightUp();
    display->Update();
    display->Modified();
  }

  for (int ihit = 0; ihit < globalcluster->GetNofHits(); ihit++) {
    PndTrkHit *hit = globalcluster->GetHit(ihit);
    cout << " " << hit->GetHitID();
  }

  cout << endl;

  // count tracks in skew sector -----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~
  int noftracksinlay[30];
  Int_t maxnoftracks = CountTracksInSkewSector(globalcluster, noftracksinlay);

  // if(maxnoftracks > 1) continue; // CHECK

  cout << "\033[1;36m ITERATIONS -----------------------> " << maxnoftracks << "\033[0m" << endl;

  for (int iter = 0; iter < maxnoftracks; iter++) {
    cout << "\033[1;36m ############### ITER " << iter << "/" << maxnoftracks << "\033[0m" << endl;
    if (fDisplayOn) {
      char goOnChar;
      cin >> goOnChar;
      Refresh();
      globalcluster->LightUp();
      display->Update();
      display->Modified();
    }

    PndTrkCluster *thiscluster = nullptr, *cluster = nullptr;
    PndTrkTrack *track = nullptr;
    double fitm, fitp, xc, yc, R;

    for (int irun = 0; irun < 2; irun++) {
      thiscluster = nullptr;
      if (irun == 0)
        thiscluster = globalcluster;
      else
        thiscluster = cluster;
      cout << "\033[1;36m ############### ITER " << iter << " IRUN " << irun << "\033[0m" << endl;
      if (thiscluster == nullptr)
        continue;
      //	cout << thiscluster << endl;
      // fitting procedure -----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~
      Int_t nhits = 0;
      if (irun == 0)
        nhits = ClusterToConformal(thiscluster, 0);
      else
        nhits = ClusterToConformal(thiscluster, 1);
      if (nhits == 0)
        continue;
      if (fSecondary)
        ComputePlaneExtremities(thiscluster);

      if (fDisplayOn) {
        char goOnChar;
        fRefHit->DrawTube(kRed);
        cin >> goOnChar;
        display->Update();
        display->Modified();
      }

      // track = LegendreFit(thiscluster);
      track = LegendreFitWithRecovering(thiscluster);
      if (track == nullptr) {
        // if on the 2nd round there are too few points, then try with the old track
        if (irun == 1) {
          track = new PndTrkTrack(thiscluster, xc, yc, R);
          cluster = thiscluster;
        } else
          continue;
      }

      if (irun == 0)
        cluster = CreateClusterAroundTrack2(track);
      else
        cluster = CreateClusterAroundTrack(track);
      cout << "legendre fit " << irun << " *** nof hits " << cluster->GetNofHits() << endl;

      // cluster->Sort();                               // CHECK THIS ######
      track->SetCluster(cluster); // CHECK THIS ######

      // cleaning -----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~
      CleanTrack(track);

      // tracklist filling ~-----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~
      // if(cluster->GetNofHits() < 3) continue;        // CHECK THIS ######
      // tracklist.AddTrack(track);                     // CHECK THIS ######

      // print and display cluster -----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~--

      if (fDisplayOn) {
        char goOnChar;
        cin >> goOnChar;
        Refresh();
        cluster->LightUp();
        track->Draw();
      }
      xc = track->GetCenter().X();
      yc = track->GetCenter().Y();
      R = track->GetRadius();
      FromRealToConformalTrack(xc, yc, R, fitm, fitp);

      if (fDisplayOn) {
        RefreshConf();
        DrawGeometryConf(fUmin, fUmax, fVmin, fVmax);
        display->cd(2);
        TLine *line = new TLine(-10.07, fitp + fitm * (-10.07), 10.07, fitp + fitm * (10.07));
        line->Draw("SAME");

        display->Update();
        display->Modified();
      }
    }

    if (cluster == nullptr)
      continue;
    // fit with analytical chi2 -----~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~--
    // analytical LS fit
    double fitm2, fitp2;
    if (fSecondary)
      ComputePlaneExtremities(cluster);
    cout << "ANALYTICAL FIT " << endl;
    cout << "xc/yc/R " << xc << " " << yc << " " << R << endl;
    cout << "nof hits " << cluster->GetNofHits() << endl;
    AnalyticalFit2(cluster, fitm, fitp, fitm2, fitp2);

    // backup old values for comparison
    double xc0 = xc, yc0 = yc, R0 = R;
    // get the new ones ...
    FromConformalToRealTrack(fitm2, fitp2, xc, yc, R);
    // ... and compare them! if the difference is too big
    // consider this findings a failure
    if (fabs(xc - xc0) > 5 && fabs(yc - yc0) > 5 && fabs(R - R0) > 20) {
      cout << "\033[1;31m THIS FINDING IS A FAILURE!\033[0m" << endl;
      cout << "\033[1;31m LETS TRY ANOTHER ONE -------- BONUS ------------ \033[0m" << endl;
      maxnoftracks++;
    } else {

      // otherwise save them
      track->SetCenter(xc, yc);
      track->SetRadius(R);

      // create final cluster on the refitted track
      thiscluster = CreateClusterAroundTrack3(track);
      cout << "after analytical fit *** nof hits " << cluster->GetNofHits() << endl;

      track->SetCluster(thiscluster);
      cout << "ADD CLUSTER " << tracklist.GetNofTracks() << " TO TRACKLIST " << thiscluster->GetNofHits() << endl;

      tracklist.AddTrack(track);

      // count remaining hits to see whether the iterations can go on or not ~~~-----
      PndTrkCluster *remainingcluster = new PndTrkCluster();
      int skewed = 0;
      for (int ihit = 0; ihit < globalcluster->GetNofHits(); ihit++) {
        PndTrkHit *hit = globalcluster->GetHit(ihit);
        if (thiscluster->DoesContain(hit))
          continue;
        if (hit->IsSttSkew())
          skewed++;
        remainingcluster->AddHit(hit);
      }
      globalcluster = remainingcluster;
      cout << "tracklist " << tracklist.GetNofTracks() << ", remainning hits " << remainingcluster->GetNofHits() << " of which " << skewed << " skewed" << endl;

      // if there are still a lot of hits not assigned, maybe
      // there is another track...
      if (remainingcluster->GetNofHits() - skewed > 15 && iter == maxnoftracks - 1) {
        cout << "\033[1;31m LETS TRY ANOTHER ONE -------- BONUS ------------ \033[0m" << endl;
        maxnoftracks++;
      }
    }
  }

  fDisplayOn = kTRUE;
  cout << "\033[1;35m --------------- SUMMARY -------------- \033[0m" << endl;
  cout << "tracklist " << tracklist.GetNofTracks() << endl;
  if (fDisplayOn) {
    Refresh();
    for (int itrk = 0; itrk < tracklist.GetNofTracks(); itrk++) {
      PndTrkTrack *track = tracklist.GetTrack(itrk);
      track->GetCluster()->LightUp();
      display->cd(1);
      track->Draw(kMagenta);
      display->cd(3);
      track->Draw(kMagenta);
      display->Update();
      display->Modified();
      char goOnChar;
      cin >> goOnChar;
    }
  }

  if (fDisplayOn) {
    char goOnChar;
    display->Update();
    display->Modified();
    cout << "Finish? ";
    cin >> goOnChar;
  }

  Reset();
}

void PndTrkCombiLegendreTask::Reset()
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
    if (stthitlist)
      delete stthitlist;
    if (mvdpixhitlist)
      delete mvdpixhitlist;
    if (mvdstrhitlist)
      delete mvdstrhitlist;
    if (fTimer) {
      fTimer->Stop();
      fTime += fTimer->RealTime();

      if (fVerbose > 0)
        cerr << fEventCounter << " Real time " << fTime << " s" << endl;
    }
  }

  fInitDone = kFALSE;
}

// ============================================================================================
// DISPLAY  ***********************************************************************************
// ============================================================================================

void PndTrkCombiLegendreTask::Refresh()
{
  // CHECK
  char goOnChar;
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
    cout << "Refresh stop" << endl;
}

void PndTrkCombiLegendreTask::DrawLegendreHisto()
{
  display->cd(3);
  legendrecombi->Draw();
  display->Update();
  display->Modified();
}

void PndTrkCombiLegendreTask::RefreshConf()
{ // CHECK delete
  // CHECK
  char goOnChar;
  //  cout << "RefreshConf?" << endl;
  //  cin >> goOnChar;
  //  cout << "REFRESHING CONF" << endl;
}

void PndTrkCombiLegendreTask::DrawHits(PndTrkHitList *hitlist)
{
  display->cd(1);
  hitlist->Draw();
  display->Update();
  display->Modified();
}

void PndTrkCombiLegendreTask::DrawGeometry(int cpad)
{
  display->cd(cpad);
  DrawSttGeometry();
}

void PndTrkCombiLegendreTask::DrawSttGeometry()
{
  if (hxy == nullptr)
    hxy = new TH2F("hxy", "xy plane", 100, -43, 43, 100, -43, 43);
  else
    hxy->Reset();
  hxy->SetStats(kFALSE);
  hxy->Draw();

  // draw all the tubes
  for (int itube = 1; itube < fTubeArray->GetEntriesFast(); itube++) {
    PndSttTube *tube = (PndSttTube *)fTubeArray->At(itube);
    if (tube->IsParallel()) {
      TArc *arc = new TArc(tube->GetPosition().X(), tube->GetPosition().Y(), 0.5);
      arc->SetFillStyle(0);
      arc->SetLineColor(kCyan - 10);
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

void PndTrkCombiLegendreTask::DrawGeometryConf(double x1, double x2, double y1, double y2)
{
  // CHECK
  char goOnChar;
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
  huv->SetStats(kFALSE);
  huv->Draw();
  display->Update();
  display->Modified();
}

// draw lists ---------
void PndTrkCombiLegendreTask::DrawLists()
{
  char goOnChar;
  //  cout << "new hit?" << endl;
  //  cin >> goOnChar;
  //  Refresh();

  for (int i = 0; i < fHitMap->GetStandalone().GetEntriesFast(); i++) {
    PndTrkHit *hitA = (PndTrkHit *)fHitMap->GetStandalone().At(i);
    // hitA->DrawTube(kGreen);
  }
  for (int i = 0; i < fHitMap->GetSeeds().GetEntriesFast(); i++) {
    PndTrkHit *hitA = (PndTrkHit *)fHitMap->GetSeeds().At(i);
    //  hitA->DrawTube(kRed);
  }
  for (int i = 0; i < fHitMap->GetCandseeds().GetEntriesFast(); i++) {
    PndTrkHit *hitA = (PndTrkHit *)fHitMap->GetCandseeds().At(i);
    //  hitA->DrawTube(kBlue);
  }
  for (int i = 0; i < fHitMap->GetIndivisibles().GetEntriesFast(); i++) {
    PndTrkHit *hitA = (PndTrkHit *)fHitMap->GetIndivisibles().At(i);
    // hitA->DrawTube(kOrange);
    TObjArray *neighs = fHitMap->GetNeighboringsToHit(hitA);
    for (int j = 0; j < neighs->GetEntriesFast(); j++) {
      PndTrkHit *hitB = (PndTrkHit *)neighs->At(j);
      TObjArray *neighs2 = fHitMap->GetNeighboringsToHit(hitA);
      if (neighs2->GetEntriesFast() > 2) {
        int counter = 0;
        PndSttTube *tubeB = (PndSttTube *)fTubeArray->At(hitB->GetTubeID());
        for (int k = 0; k < neighs2->GetEntriesFast(); k++) {
          PndTrkHit *hitC = (PndTrkHit *)neighs2->At(k);
          PndSttTube *tubeC = (PndSttTube *)fTubeArray->At(hitC->GetTubeID());
          // cout << "tubes " << tubeB << " "<< tubeC << endl;
          if (tubeB->GetLayerID() == tubeC->GetLayerID())
            continue;
          counter++;
        }
        if (counter > 2)
          continue;
      }
      //   hitB->DrawTube(kYellow);
    }
    //    display->Update();
    //    display->Modified();
    //    cin >> goOnChar;
  }

  //     for(int i = 0; i < limits.GetEntriesFast(); i++) {
  //        PndTrkHit *hitA = (PndTrkHit*) limits.At(i);
  //        hitA->DrawTube(kYellow);
  //      }

  //  display->Update();
  //  display->Modified();
  //  cin >> goOnChar;
}

// draw neighborings ---------
void PndTrkCombiLegendreTask::DrawNeighborings()
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

void PndTrkCombiLegendreTask::DrawNeighboringsToHit(PndTrkHit *hit)
{

  Refresh();
  hit->DrawTube(kYellow);
  PndSttTube *tube = (PndSttTube *)fTubeArray->At(hit->GetTubeID());
  TObjArray *neighs = fHitMap->GetNeighboringsToHit(hit);

  cout << "HIT " << hit->GetHitID() << "(" << hit->GetTubeID() << "/" << tube->GetLayerID() << ")"
       << " has " << neighs->GetEntriesFast() << " neighborings: ";
  for (int i = 0; i < neighs->GetEntriesFast(); i++) {
    PndTrkHit *hitA = (PndTrkHit *)neighs->At(i);
    hitA->DrawTube(kCyan);
    PndSttTube *tubeA = (PndSttTube *)fTubeArray->At(hitA->GetTubeID());
    cout << " " << hitA->GetHitID() << "(" << hitA->GetTubeID() << "/" << tubeA->GetLayerID() << ")";
  }
  cout << endl;
  display->Update();
  display->Modified();
}

void PndTrkCombiLegendreTask::DrawConfHit(double u, double v, double r, int marker)
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

Int_t PndTrkCombiLegendreTask::FillConformalHitList(PndTrkCluster *cluster)
{

  // set the conformal transformation, where the
  // translation and rotation must be already set
  conformalhitlist->SetConformalTransform(conform);

  // loop over the cluster hits
  // and port them to conf plane
  for (int jhit = 0; jhit < cluster->GetNofHits(); jhit++) {
    PndTrkHit *hit = cluster->GetHit(jhit);
    if (hit == fRefHit)
      continue;
    PndTrkConformalHit *chit = nullptr;
    //    cout << "HIT " << hit->GetHitID() << " " << hit->IsSttParallel() << " " << hit->IsSttSkew() << endl;
    if (hit->IsSttParallel() == kTRUE)
      chit = conform->GetConformalSttHit(hit);
    else
      continue; // CHECK  chit = conform->GetConformalHit(hit); // CHECK
    conformalhitlist->AddHit(chit);
    //    cout << hit->GetPosition().X() << " " << hit->GetPosition().Y() << " " << hit->GetIsochrone() << " " << hit->IsSttParallel() << " " << " to CONFORMAL " << chit->GetU() <<
    //    " " << chit->GetV() << " " << chit->GetIsochrone() << endl;
    //  if(chit->GetPosition().Mod() > 1.16) cout << "\033[1;36m CONFORMAL DISTANCE IS " << chit->GetPosition().Mod() << "\033[0m" << endl;
  }
  return conformalhitlist->GetNofHits();
}

// ----------------------- FOR SECONDARIES
PndTrkHit *PndTrkCombiLegendreTask::FindSttReferenceHit(int isec)
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
      // cout << "STT hit " << jhit << "already used " << endl;
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
  //  if(fVerbose > 1) cout << "STT REFERENCE HIT " <<  tmphitid << " " << refhit->GetIsochrone() << endl;

  return refhit;
}

PndTrkHit *PndTrkCombiLegendreTask::FindMvdPixelReferenceHit()
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

PndTrkHit *PndTrkCombiLegendreTask::FindMvdStripReferenceHit()
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

PndTrkHit *PndTrkCombiLegendreTask::FindMvdReferenceHit()
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

PndTrkHit *PndTrkCombiLegendreTask::FindReferenceHit()
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

PndTrkHit *PndTrkCombiLegendreTask::FindReferenceHit(PndTrkCluster *cluster, bool keeprefhit)
{
  int ntot = cluster->GetNofHits();
  //  cout << "FIND REFERENCE HIT " << ntot << endl;
  if (ntot == 0)
    return nullptr;

  if (keeprefhit && cluster->DoesContain(fRefHit) == kTRUE) {
    cout << "THE REF HIT IS ALREADY INSIDE THE CLUSTER SO I KEEP IT" << endl;
    return fRefHit;
  }

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
          if (hit->WasRefHit() == kFALSE) {
            tmphitid = jhit;
            tmpiso = hit->GetIsochrone();
            refhit = hit;
          }
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
  refhit->SetRefHitFlag(1);
  return refhit;
}

void PndTrkCombiLegendreTask::ComputeTraAndRot(PndTrkHit *hit, Double_t &delta, Double_t trasl[2])
{

  trasl[0] = hit->GetPosition().X();
  trasl[1] = hit->GetPosition().Y();

  delta = 0.; // TMath::ATan2(hit->GetPosition().Y() - 0., hit->GetPosition().X() - 0.); // CHECK
}

void PndTrkCombiLegendreTask::ComputePlaneExtremities(PndTrkCluster *cluster)
{
  fUmin = 1000, fVmin = 1000, fRmin = 1000;
  fUmax = -1000, fVmax = -1000, fRmax = -1000;
  double rc_of_min, rc_of_max;

  for (int ihit = 0; ihit < conformalhitlist->GetNofHits(); ihit++) {
    PndTrkConformalHit *chit = conformalhitlist->GetHit(ihit);
    double u = chit->GetU();
    double v = chit->GetV();
    double rc = chit->GetIsochrone();
    u - rc < fUmin ? fUmin = u - rc : fUmin;
    v - rc < fVmin ? fVmin = v - rc : fVmin;
    u + rc > fUmax ? fUmax = u + rc : fUmax;
    v + rc > fVmax ? fVmax = v + rc : fVmax;
    //     cout << "compute plane extremities " << chit->GetHit()->GetHitID() << " " << u << " " << v << " " << u - rc << " " << u + rc << " " << v - rc << " " << v + rc << endl;

    for (int jhit = ihit + 1; jhit < conformalhitlist->GetNofHits(); jhit++) {
      PndTrkConformalHit *chit2 = conformalhitlist->GetHit(jhit);
      double u2 = chit2->GetU();
      double v2 = chit2->GetV();
      double rc2 = chit2->GetIsochrone();

      double theta, r;
      legendrecombi->ComputeThetaR(u, v, rc, u2, v2, rc2, theta, r);

      if (r < fRmin) {
        fRmin = r;
        rc < rc2 ? rc_of_min = rc : rc_of_min = rc2;
      }
      if (r > fRmax) {
        fRmax = r;
        rc < rc2 ? rc_of_max = rc2 : rc_of_max = rc;
      }
    }
  }

  //  cout << "u_min " << fUmin << " u_max " << fUmax << endl;
  //  cout << "v_min " << fVmin << " v_max " << fVmax << endl;
  //  cout << "r_min " << fRmin << " r_max " << fRmax << endl;
  //  cout << "theta_min 0 theta_max 180" << endl;

  // to square the conformal plane
  double du = fUmax - fUmin;
  double dv = fVmax - fVmin;
  double delta = fabs(dv - du) / 2.;
  du < dv ? (fUmin -= delta, fUmax += delta) : (fVmin -= delta, fVmax += delta);

  // if(fVerbose > 0)
  {
    cout << "u_min " << fUmin << " u_max " << fUmax << endl;
    cout << "v_min " << fVmin << " v_max " << fVmax << endl;
    cout << "r_min " << fRmin << " r_max " << fRmax << endl;
    cout << "theta_min 0 theta_max 180" << endl;
  }
}

void PndTrkCombiLegendreTask::FillPeakNeighCouplesHisto(PndTrkCluster *cluster)
{
  fTimer->Start();

  std::vector<std::pair<int, int>> usedcouples;
  for (int ihit = 0; ihit < conformalhitlist->GetNofHits(); ihit++) {
    PndTrkConformalHit *chit = conformalhitlist->GetHit(ihit);
    PndTrkHit *hit = chit->GetHit();
    if (fDisplayOn) {
      display->cd(2);
      chit->Draw(1);
    }
    //    cout << "---------- HIT ihit " << ihit <<  endl;
    //    if(fDisplayOn) {
    //       Refresh();
    //       display->cd(1);
    //       hit->DrawTube(kBlue);
    //       display->Update();
    //       display->Modified();
    //       char goOnchar;
    //       //  cin >> goOnchar;
    //     }

    TObjArray *neighborings = fHitMap->GetNeighboringsToHit(hit);
    if (neighborings->GetEntriesFast() == 0)
      continue;

    for (int jhit = 0; jhit < neighborings->GetEntriesFast(); jhit++) {
      PndTrkHit *hit2 = (PndTrkHit *)neighborings->At(jhit);

      //     cout << "jhit " << jhit << " ";
      //     if(fDisplayOn) {
      // 	display->cd(1);
      // 	hit2->DrawTube(kOrange);
      // 	display->Update();
      // 	display->Modified();
      // 	char goOnchar;
      // 	//	cin >> goOnchar;
      //       }

      for (int khit = 0; khit < conformalhitlist->GetNofHits(); khit++) {
        PndTrkConformalHit *chit2 = conformalhitlist->GetHit(khit);
        if (hit2 != chit2->GetHit())
          continue;

        std::pair<int, int> thiscouple(hit->GetHitID(), hit2->GetHitID());
        std::pair<int, int> thisrevcouple(hit2->GetHitID(), hit->GetHitID());

        if ((find(usedcouples.begin(), usedcouples.end(), thiscouple) != usedcouples.end()) || (find(usedcouples.begin(), usedcouples.end(), thisrevcouple) != usedcouples.end())) {
          // 	    cout << "ALREADY USED" << endl;
          //     if(fDisplayOn) {
          // 	      display->cd(1);
          // 	      hit2->DrawTube(kRed);
          // 	      display->Update();
          // 	      display->Modified();
          // 	      char goOnchar;
          // 	      // cin >> goOnchar;
          // 	    }

          break;
        }
        // 	cout << "ADDED" << endl;
        // 	if(fDisplayOn) {
        // 	  display->cd(1);
        // 	  hit2->DrawTube(kGreen);
        // 	  display->cd(4);
        // 	  legendrecombi->Draw();
        // 	  display->Update();
        // 	  display->Modified();
        // 	  char goOnchar;
        // 	  // cin >> goOnchar;
        // 	}

        //	cout << "couple " << usedcouples.size() << " " << hit->GetHitID() << " " << hit2->GetHitID() << endl;
        legendrecombi->FillLegendreHisto(chit->GetU(), chit->GetV(), chit->GetIsochrone(), chit2->GetU(), chit2->GetV(), chit2->GetIsochrone());
        usedcouples.push_back(thiscouple);
        break;
      }
    }
  }

  fTimer->Stop();
  cout << "fill peak legendre histo time " << fTimer->RealTime() << " sec" << endl;

  if (fDisplayOn) {
    display->cd(4);
    legendrecombi->Draw();
    display->Update();
    display->Modified();
    char goOnchar;
    cin >> goOnchar;
  }
}

void PndTrkCombiLegendreTask::FillPeakCouplesHisto(PndTrkCluster *cluster)
{
  fTimer->Start();
  for (int ihit = 0; ihit < conformalhitlist->GetNofHits(); ihit++) {
    PndTrkConformalHit *chit = conformalhitlist->GetHit(ihit);
    PndTrkHit *hit = chit->GetHit();
    if (cluster->DoesContain(hit) == kFALSE)
      continue;
    if (fDisplayOn) {
      display->cd(2);
      chit->Draw(1);
    }
    for (int jhit = ihit + 1; jhit < conformalhitlist->GetNofHits(); jhit++) {
      // if(ihit == jhit) continue;
      PndTrkConformalHit *chit2 = conformalhitlist->GetHit(jhit);
      PndTrkHit *hit2 = chit2->GetHit();
      if (cluster->DoesContain(hit2) == kFALSE)
        continue;

      legendrecombi->FillLegendreHisto(chit->GetU(), chit->GetV(), chit->GetIsochrone(), chit2->GetU(), chit2->GetV(), chit2->GetIsochrone());
    }
  }
  fTimer->Stop();
  cout << "fill peak legendre histo " << fTimer->RealTime() << endl;

  if (fDisplayOn) {
    display->cd(4);
    legendrecombi->Draw();
    display->Update();
    display->Modified();
    char goOnchar;
    cout << "get the max? " << endl;
    cin >> goOnchar;
  }
}

void PndTrkCombiLegendreTask::FromConformalToRealTrack(double fitm, double fitp, double &x0, double &y0, double &R)
{
  // CHECK if this needs to be kept --> change xc0 to xc etc
  // center and radius
  Double_t xcrot0, ycrot0;
  ycrot0 = 1 / (2 * fitp);
  xcrot0 = -fitm * ycrot0;
  R = sqrt(xcrot0 * xcrot0 + ycrot0 * ycrot0);
  // re-rotation and re-traslation of xc and yc
  // rotation
  x0 = TMath::Cos(conformalhitlist->GetConformalTransform()->GetRotation()) * xcrot0 - TMath::Sin(conformalhitlist->GetConformalTransform()->GetRotation()) * ycrot0;
  y0 = TMath::Sin(conformalhitlist->GetConformalTransform()->GetRotation()) * xcrot0 + TMath::Cos(conformalhitlist->GetConformalTransform()->GetRotation()) * ycrot0;
  // traslation
  x0 += conformalhitlist->GetConformalTransform()->GetTranslation().X();
  y0 += conformalhitlist->GetConformalTransform()->GetTranslation().Y();
}

void PndTrkCombiLegendreTask::FromRealToConformalTrack(double x0, double y0, double R, double &fitm, double &fitp)
{
  // CHECK if this needs to be kept --> change xc0 to xc etc
  Double_t xcrot0, ycrot0;

  // traslation
  x0 -= conformalhitlist->GetConformalTransform()->GetTranslation().X();
  y0 -= conformalhitlist->GetConformalTransform()->GetTranslation().Y();

  /**
     |x|  |c -s||xr|  ---> |xr|  | c s||x|
     |y|  |s  c||yr|       |yr|  |-s c||y|
  **/

  // rotation
  xcrot0 = TMath::Cos(conformalhitlist->GetConformalTransform()->GetRotation()) * x0 + TMath::Sin(conformalhitlist->GetConformalTransform()->GetRotation()) * y0;
  ycrot0 = -TMath::Sin(conformalhitlist->GetConformalTransform()->GetRotation()) * x0 + TMath::Cos(conformalhitlist->GetConformalTransform()->GetRotation()) * y0;

  // yr = 1/ (2 p) --> p = 1 / (2 yr)
  // xr = - m / (2 p) --> m = - 2 p xr
  fitp = 1 / (2 * ycrot0);
  fitm = -2 * fitp * xcrot0;
}

void PndTrkCombiLegendreTask::FillHitMap()
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

    //    cout << "HIT: " << hit->GetHitID() << " has " << neighborings->GetEntriesFast() << " hits" << endl;
    //     if(fDisplayOn) {
    //       Refresh();
    //     }

    fHitMap->AddNeighboringsToHit(hit, neighborings);

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
      TObjArray *indiv = fHitMap->GetIndivisiblesToHit(stthit);
      for (int jhit = 0; jhit < indiv->GetEntriesFast(); jhit++) {
        PndTrkHit *stthit2 = (PndTrkHit *)indiv->At(jhit);
        stthit2->Draw(kOrange);
        stthit->Draw(kOrange);
      }
    }

    display->Update();
    display->Modified();
    cout << " STARTING" << endl;
    cin >> goOnChar;
    display->Update();
    display->Modified();
    // ================================================
  }
  //  cout << "PRINT INDIVISIBILE MAP" << endl;
  //  fHitMap->PrintIndivisibleMap();
}

PndTrkClusterList PndTrkCombiLegendreTask::CreateFullClusterization()
{
  PndTrkClusterList clusterlist;

  // get seeds *********************************************8
  TObjArray seeds = fHitMap->GetSeeds();
  TObjArray *neighborings = nullptr;

  // ----------------- loop over seeds
  for (int iseed = 0; iseed < seeds.GetEntriesFast(); iseed++) {
    PndTrkCluster *cluster = new PndTrkCluster();
    PndTrkHit *seedhit = (PndTrkHit *)seeds.At(iseed);

    // is it already used
    if (seedhit->IsUsed() == kTRUE)
      continue;

    int seedtubeID = seedhit->GetTubeID();
    PndSttTube *seedtube = (PndSttTube *)fTubeArray->At(seedtubeID);
    int seedlayerID = seedtube->GetLayerID();

    // add hit to cluster
    cluster->AddHit(seedhit);

    //    if(fDisplayOn) {
    //       char goOnChar;
    //       cin >> goOnChar;
    //       cout << "SEED " << seedtubeID << endl;
    //       cluster->LightUp();
    //       display->Update();
    //       display->Modified();
    //     }

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
        if (neighborings->GetEntriesFast() == 0)
          continue;
        //	cout << "hit " << addedhit->GetHitID() << "(" << addedhit->GetTubeID() << ")" << " has " << neighborings->GetEntriesFast()  << " neighborigns: " << endl;

        // loop over the neighborings and add them all
        for (int ineigh = 0; ineigh < neighborings->GetEntriesFast(); ineigh++) {
          PndTrkHit *neighhit = (PndTrkHit *)neighborings->At(ineigh);
          //  cout << " " << neighhit->GetHitID() << "(" << neighhit->GetTubeID() << ")";
          if (cluster->DoesContain(neighhit) == kTRUE) {
            //  cout << "UN-ADDED, in cluster already" << endl;
            continue;
          }
          cluster->AddHit(neighhit);
          addedcounter++;
          //  cout << " - ADDED; ";

          //     if(fDisplayOn) {
          // 	      char goOnChar;
          // 	      cin >> goOnChar;
          // 	      cluster->LightUp();
          // 	      display->Update();
          // 	      display->Modified();
          // 	      //  cin >> goOnChar;
          // 	    }
        }
      }
      //       cout << endl;
      nlastadded = addedcounter;
    }
    //     cout << "NEXT seed " << endl;
    // add cluster to clusterlist
    if (cluster->GetNofHits() > 3)
      clusterlist.AddCluster(cluster); // CHECK
  }
  return clusterlist;
}

PndTrkClusterList PndTrkCombiLegendreTask::CreateFullClusterization2()
{
  /**
     PndTrkClusterList clusterlist;

    // get seeds *********************************************8
    TObjArray seeds = fHitMap->GetSeeds();
    neighborings = nullptr;
  **/
}

Int_t PndTrkCombiLegendreTask::CountTracksInCluster(PndTrkCluster *cluster, int *noftracksinlayer)
{
  return CountTracksInCluster(cluster, 0, noftracksinlayer);
}

Int_t PndTrkCombiLegendreTask::CountTracksInSkewSector(PndTrkCluster *cluster, int *noftracksinlayer)
{
  return CountTracksInCluster(cluster, 1, noftracksinlayer);
}

Int_t PndTrkCombiLegendreTask::CountTracksInCluster(PndTrkCluster *cluster, Int_t where, int *noftracksinlayer)
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

  for (int ihit = 0; ihit < cluster->GetNofHits(); ihit++) {
    PndTrkHit *hit = cluster->GetHit(ihit);
    PndSttTube *tube = (PndSttTube *)fTubeArray->At(hit->GetTubeID());
    //    cout << "SORTED " << ihit << " " << hit->GetHitID() << " " << tube->GetLayerID() << " " << nofhitsinlay[tube->GetLayerID()] << endl;
  }

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
    if (nofhitsinlay[layid] <= 1) {
      noftracksinlayer[layid] = nofhitsinlay[layid];
      continue;
    }

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

      noftracksinlayer[layid] = noftracks;
    }
  }

  cout << "THIS CLUSTER HAS A TOTAL OF " << maxnoftracks << " TRACKS" << endl;
  return maxnoftracks;
}

Int_t PndTrkCombiLegendreTask::ClusterToConformal(PndTrkCluster *cluster, bool samerefhit)
{
  // ================ --> TO CONFORMAL PLANE
  conformalhitlist = new PndTrkConformalHitList(); // CHECK
  // translation and rotation
  Int_t nchits = 0;
  Double_t delta = 0, trasl[2] = {0., 0.};
  if (fSecondary) {
    // translation and rotation - CHECK
    fRefHit = FindReferenceHit(cluster, samerefhit);
    cout << " REFERENCE HIT " << fRefHit << " found among " << cluster->GetNofHits() << " hits of cluster " << cluster << endl;
    if (fRefHit == nullptr) {
      //      cout << "REFHIT " << fRefHit << endl;
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

PndTrkTrack *PndTrkCombiLegendreTask::LegendreFitWithRecovering(PndTrkCluster *cluster)
{
  PndTrkTrack *track = nullptr;
  fRecoverIteration = 0;
  int stopit = false;
  while (stopit != true) {
    track = LegendreFit(cluster);
    if (track != nullptr)
      stopit = true;
    else if (legendrecombi->GetLegendreHisto()->GetNbinsX() < 50)
      stopit = true;
  }
  return track;
}

PndTrkTrack *PndTrkCombiLegendreTask::LegendreFit(PndTrkCluster *cluster)
{

  cout << "APPLY LEGENDRE ======================= (nofhits " << cluster->GetNofHits() << ") " << endl;
  // reset the legendre histo for a new legendre fit
  legendrecombi->SetUpLegendreHisto(1000, fThetamin, fThetamax, 1000, fRmin, fRmax);
  if (fRecoverIteration > 0) {
    legendrecombi->GetLegendreHisto()->Rebin2D(TMath::Power(2, fRecoverIteration), TMath::Power(2, fRecoverIteration));
    cout << "RECOVER ITERATION " << fRecoverIteration << " " << TMath::Power(2, fRecoverIteration) << " " << legendrecombi->GetLegendreHisto()->GetNbinsX() << " "
         << legendrecombi->GetLegendreHisto()->GetNbinsY() << endl;
  }

  if (legendrecombi->GetLegendreHisto()->GetNbinsX() < 50)
    return nullptr; // CHECK

  legendrecombi->ResetLegendreHisto();

  if (fDisplayOn) {
    RefreshConf();
    DrawGeometryConf(fUmin, fUmax, fVmin, fVmax);
  }

  // ---------------------------------------
  // NEW LEGENDRE WITH ONLY PEAKS
  FillPeakCouplesHisto(cluster);
  // FillPeakNeighCouplesHisto(cluster);
  double theta_max;
  double r_max;
  int maxpeak = legendrecombi->ExtractLegendreMaximum(theta_max, r_max);
  //  cout << " MAX PEAK " << maxpeak << endl;
  if (maxpeak <= 3) {
    if (fRecoverIteration != -1)
      fRecoverIteration++;
    return nullptr;
  }

  double fitm, fitq;
  legendrecombi->ExtractLegendreSingleLineParameters(fitm, fitq);
  Double_t xc, yc, R;
  FromConformalToRealTrack(fitm, fitq, xc, yc, R);
  cout << "\033[1;33m MAXPEAK " << maxpeak << " XR, YC, R: " << xc << " " << yc << " " << R << "\033[0m" << endl;

  // create a track from the cluster
  PndTrkTrack *track = new PndTrkTrack(cluster, xc, yc, R);

  if (fDisplayOn) {
    TMarker *maxmrk = new TMarker(theta_max, r_max, 20);
    display->cd(4);
    maxmrk->Draw("SAME");
    //    display->cd(3); maxmrk->Draw("SAME");

    display->cd(2);
    TLine *line = new TLine(-10.07, fitq + fitm * (-10.07), 10.07, fitq + fitm * (10.07));
    line->SetLineColor(kGreen);
    line->Draw("SAME");

    display->cd(1);
    track->Draw(kGreen);

    display->Update();
    display->Modified();
    char goOnChar;
    cout << "want to go on?" << endl;
    cin >> goOnChar;
  }

  return track;
}

Bool_t PndTrkCombiLegendreTask::DoesBelong(PndTrkHit *hit, double xc, double yc, double R)
{

  // create cluster depending on fitting
  double rmin = R - 1; // R * 0.05; // CHECK value%? // CHECK CAN BE DONE ONCE FOR EACH TRACK
  double rmax = R + 1; // R * 0.05; // "      "
  // cout << "rmin " << rmin << " rmax " << rmax << endl;

  double distance = hit->GetXYDistance(TVector3(xc, yc, 0.));
  double dist = fabs(distance - R);

  // ------- to be deleted --- only to check MC truth
  if (hit->IsStt()) {
    PndSttHit *stthit = (PndSttHit *)fSttHitArray->At(hit->GetHitID());
    PndSttPoint *sttpnt = (PndSttPoint *)fSttPointArray->At(stthit->GetRefIndex());
    /**
       if(dist < 5 * hit->GetIsochrone()) {
       if(distance <= rmax && distance >= rmin) cout << " distance " << distance << " dist " << dist << " " << hit->GetIsochrone() << " "<< sttpnt->GetTrackID() << endl;
       else cout << "\033[1;31m distance " << distance << " dist " << dist << "\033[0m" << " " << hit->GetIsochrone() << " "<< sttpnt->GetTrackID() << endl;
       }
       else if(distance <= rmax && distance >= rmin) cout << "\033[1;33m distance " << distance << " dist " << dist << "\033[0m" << " " << hit->GetIsochrone() << " "<<
    sttpnt->GetTrackID() << endl;
    **/
  }
  // ------------------------------------------------

  //    if(distance <= rmax && distance >= rmin) {
  //    if(dist < 5 * hit->GetIsochrone()) {
  if ((distance <= rmax && distance >= rmin) || (dist < 5 * hit->GetIsochrone()))
    return kTRUE;
  return kFALSE;
}

PndTrkCluster *PndTrkCombiLegendreTask::CreateClusterAroundTrack2(PndTrkTrack *track)
{

  double R = track->GetRadius();
  double xc = track->GetCenter().X();
  double yc = track->GetCenter().Y();

  // create cluster depending on fitting
  PndTrkCluster *thiscluster = new PndTrkCluster();
  int startsecid = 1000, endsecid = -1, startlayid = 1000, endlayid = -1;

  // initialization
  int nofhitsinsec[6], lowlayinsec[6], uplayinsec[6];
  TVector2 lowinsec[6], upinsec[6];
  bool sectorsallow[6];
  for (int isec = 0; isec < 6; isec++) {
    nofhitsinsec[isec] = 0;
    lowlayinsec[isec] = 1000;
    uplayinsec[isec] = -1;
    sectorsallow[isec] = false;
  }

  int maxnofhits = -1, maxhitsec = -1;
  for (int ihit = 0; ihit < stthitlist->GetNofHits(); ihit++) {
    PndTrkHit *hit = stthitlist->GetHit(ihit);
    bool doesitbelong = DoesBelong(hit, xc, yc, R);
    if (doesitbelong == kTRUE) {
      thiscluster->AddHit(hit);
      if (fDisplayOn) {
        display->cd(1);
        hit->DrawTube(kGreen);
      }

      PndSttTube *tube = (PndSttTube *)fTubeArray->At(hit->GetTubeID());
      int isec = tube->GetSectorID();
      int ilay = tube->GetLayerID();

      if (ilay < startlayid)
        startlayid = ilay;
      if (isec < startsecid)
        startsecid = isec;
      if (ilay > endlayid)
        endlayid = ilay;
      if (isec > endsecid)
        endsecid = isec;
      nofhitsinsec[isec]++;
      if (maxnofhits < nofhitsinsec[isec]) {
        maxnofhits = nofhitsinsec[isec];
        maxhitsec = isec;
      }
      if (ilay < lowlayinsec[isec]) {
        lowlayinsec[isec] = ilay;
        lowinsec[isec].Set(hit->GetPosition().X(), hit->GetPosition().Y());
      }
      if (ilay > uplayinsec[isec]) {
        uplayinsec[isec] = ilay;
        upinsec[isec].Set(hit->GetPosition().X(), hit->GetPosition().Y());
      }
    }
  }

  // ============== CHECK ALLOWED SECTORS ================
  sectorsallow[maxhitsec] = true;
  for (int isec = maxhitsec + 1; isec <= endsecid; isec++) {
    if (nofhitsinsec[isec] != 0)
      sectorsallow[isec] = true;
    else
      break;
  }
  for (int isec = maxhitsec - 1; isec >= startsecid; isec--) {
    if (nofhitsinsec[isec] != 0)
      sectorsallow[isec] = true;
    else
      break;
  }
  if (sectorsallow[0] == true && nofhitsinsec[5] > 0)
    sectorsallow[5] = true;
  if (sectorsallow[5] == true && nofhitsinsec[0] > 0)
    sectorsallow[0] = true;
  // ======================================================

  // I have the center of the track circle (xc, yc) and I want
  // the point ON the circle (0, 0, CTOUTRADIUS) closest to it
  TVector2 qpoint = tools->ComputePocaToPointOnCircle2(xc, yc, 0, 0, CTOUTRADIUS);
  double OQ = qpoint.Mod();
  TVector2 center(xc, yc);
  double OC = center.Mod();

  // check if there is the intersection
  // center of curvature inside OR outside CT
  if ((OC < CTOUTRADIUS && (OC + R) > CTOUTRADIUS) || (OC > CTOUTRADIUS && (OC - R) < CTOUTRADIUS)) {
    double arrow = OC + R - OQ;
    cout << "========================> ARROW " << arrow << endl;

    bool sectorsallow2[6];
    for (int isec = 0; isec < 6; isec++) { // CHECK hardcoded nof sectors
      sectorsallow2[isec] = false;
    }

    int nofsteps = endsecid - startsecid;
    if (nofsteps == 5)
      nofsteps++;
    int nofstepsup = endsecid - maxhitsec;
    int nofstepsdown = maxhitsec - startsecid;
    sectorsallow2[maxhitsec] = true;
    cout << "nofsteps " << nofsteps << " whose up " << nofstepsup << " and down " << nofstepsdown << endl;

    for (int isec = 0; isec < 6; isec++) {
      cout << "preliminary sec " << isec << " " << sectorsallow[isec] << endl;
    }
    for (int isec = 0; isec < 6; isec++) {
      cout << "preliminary sec2 " << isec << " " << sectorsallow2[isec] << endl;
    }
    for (int isec = 0; isec < nofsteps; isec++) {
      int ksec = maxhitsec, jsec;
      if (isec < nofstepsup) {
        ksec += isec;
        if (ksec > 5)
          ksec -= 6;
        jsec = ksec;
        jsec++;
        if (ksec == 5)
          jsec = 0;
      } else {
        ksec -= (isec - nofstepsup);
        if (ksec < 0)
          ksec += 6;
        jsec = ksec;
        jsec--;
        if (ksec == 0)
          jsec = 5;
      }
      cout << "===== ksec " << ksec << " jsec " << jsec << endl;
      cout << "sectorsallow2[0]:  " << sectorsallow2[0] << endl;

      //      cout << "ksec layers " << lowlayinsec[ksec] << " " << uplayinsec[ksec] << endl;
      // cout << lowinsec[ksec].Mod() << " " << upinsec[ksec].Mod() << endl;
      //      cout << lowinsec[ksec].X() << " " << lowinsec[ksec].Y() << endl;
      //      cout << upinsec[ksec].X() << " " << upinsec[ksec].Y() << endl;
      //      cout << "jsec layers " << lowlayinsec[jsec] << " " << uplayinsec[jsec] << endl;
      //      cout << lowinsec[jsec].Mod() << " " << upinsec[jsec].Mod() << endl;
      //      cout << lowinsec[jsec].X() << " " << lowinsec[jsec].Y() << endl;
      //      cout << upinsec[jsec].X() << " " << upinsec[jsec].Y() << endl;

      if (sectorsallow[ksec] == true && sectorsallow[jsec] == true) {
        cout << "go on " << endl;

        if (upinsec[ksec].Mod() > 39 && upinsec[jsec].Mod() > 39) { // both pieces @ CTOUTRADIUS?
          if (arrow < 7.5) {
            cout << "BOTH AT OUTER RADIUS" << endl;
            sectorsallow2[ksec] = true;
            sectorsallow2[jsec] = true;
          } else
            cout << "WRONG BOTH AT OUTER RADIUS ************* " << upinsec[ksec].Mod() << " " << upinsec[jsec].Mod() << endl;
        } else if (lowinsec[ksec].Mod() < 17 && lowinsec[jsec].Mod() < 17) { // both @ 0
          if (arrow < 7.5) {
            cout << "BOTH AT INNER RADIUS" << endl;
            sectorsallow2[ksec] = true;
            sectorsallow2[jsec] = true;
          } else
            cout << "WRONG BOTH AT INNER RADIUS ************* " << lowinsec[ksec].Mod() << " " << lowinsec[jsec].Mod() << endl;
        } else { // one here/one there
          TVector2 lowvec, upvec;
          int lowlay, uplay;
          (upinsec[ksec] - lowinsec[jsec]).Mod() < (lowinsec[ksec] - upinsec[jsec]).Mod()
            ? (lowvec = lowinsec[jsec], upvec = upinsec[ksec], lowlay = lowlayinsec[jsec], uplay = uplayinsec[ksec])
            : (upvec = upinsec[jsec], lowvec = lowinsec[ksec], lowlay = lowlayinsec[ksec], uplay = uplayinsec[jsec]);
          if ((upvec - lowvec).Mod() < 10) {
            cout << "ONE HERE / ONE THERE" << endl;
            sectorsallow2[ksec] = true;
            sectorsallow2[jsec] = true;
          } else {
            cout << "WRONG ONE HERE / ONE THERE *************** " << (upvec - lowvec).Mod() << " " << lowlay << " " << uplay << endl;
          }
        }
      }

      cout << "@step " << isec << ":";
      for (int lsec = 0; lsec < 6; lsec++)
        cout << " " << sectorsallow2[lsec];
      cout << endl;
    }
    for (int isec = 0; isec < 6; isec++) {
      sectorsallow[isec] = sectorsallow2[isec];
      cout << "final " << sectorsallow[isec] << endl;
    }

  } else
    cout << "========================> NO INTERSECTION " << OQ << " " << CTOUTRADIUS << " " << R << endl;

  if (fDisplayOn) {
    display->cd(1);
    thiscluster->Draw(kRed);
    display->Update();
    display->Modified();
    char goOnChar;
    //    cout << "want to go to next cluster2?" << endl;
    //    cin >> goOnChar;
  }

  cout << "START SECTOR " << startsecid << " END SECTOR " << endsecid << endl;
  cout << "START LAYER  " << startlayid << " END LAYER  " << endlayid << endl;
  cout << "MAX NOF HITS IS " << maxnofhits << "/" << thiscluster->GetNofHits() << " IN SEC " << maxhitsec << endl;
  for (int isec = 0; isec < 6; isec++) {
    if (nofhitsinsec[isec] > 0)
      cout << "isec " << isec << " " << nofhitsinsec[isec] << " from lay " << lowlayinsec[isec] << " to lay " << uplayinsec[isec] << " allowed? " << sectorsallow[isec] << endl;
  }

  PndTrkCluster *thiscluster2 = new PndTrkCluster();
  for (int ihit = 0; ihit < thiscluster->GetNofHits(); ihit++) {
    PndTrkHit *hit = thiscluster->GetHit(ihit);
    PndSttTube *tube = (PndSttTube *)fTubeArray->At(hit->GetTubeID());
    int isec = tube->GetSectorID();
    if (sectorsallow[isec] == true) {
      thiscluster2->AddHit(hit);
      if (fDisplayOn) {
        display->cd(1);
        hit->DrawTube(kRed);
      }
    }
  }
  return thiscluster2;
}

PndTrkCluster *PndTrkCombiLegendreTask::CreateClusterAroundTrack(PndTrkTrack *track)
{

  PndTrkCluster *cluster = track->GetCluster();

  cout << "CREATE CLUSTER AROUND TRACK " << track->GetCluster() << " with hits " << track->GetCluster()->GetNofHits() << endl;

  double R = track->GetRadius();
  double xc = track->GetCenter().X();
  double yc = track->GetCenter().Y();

  // create cluster depending on fitting
  PndTrkCluster *thiscluster = new PndTrkCluster();
  int startsecid = 1000, endsecid = -1, startlayid = 1000, endlayid = -1;

  // ...................................................
  // I. clean existing cluster
  // add hits within certain rmin & rmax;
  // find min/max sec/layer
  for (int ihit = 0; ihit < cluster->GetNofHits(); ihit++) {
    PndTrkHit *hit = cluster->GetHit(ihit);
    bool doesitbelong = DoesBelong(hit, xc, yc, R);
    if (doesitbelong == kTRUE) {
      thiscluster->AddHit(hit);
      if (fDisplayOn) {
        display->cd(1);
        hit->DrawTube(kGreen);
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
    }
  }

  cout << "START SECTOR " << startsecid << " END SECTOR " << endsecid << endl;
  cout << "START LAYER  " << startlayid << " END LAYER  " << endlayid << endl;

  if (fDisplayOn) {
    display->cd(1);
    thiscluster->Draw(kRed);
    display->Update();
    display->Modified();
    char goOnChar;
    //    cout << "want to go to next cluster1?" << endl;
    //    cin >> goOnChar;
  }
  // ...................................................
  // II. for its not in the cluster
  bool select_sec_lay = true;
  if ((select_sec_lay == true && (startlayid != 0 || endlayid != 23)) || select_sec_lay == false) {
    for (int ihit = 0; ihit < stthitlist->GetNofHits(); ihit++) {
      PndTrkHit *hit = stthitlist->GetHit(ihit);
      if (cluster->DoesContain(hit))
        continue;
      PndSttTube *tube = (PndSttTube *)fTubeArray->At(hit->GetTubeID());
      bool doesitbelong = DoesBelong(hit, xc, yc, R);
      if (doesitbelong == kTRUE) {

        if (select_sec_lay == true) {
          if (tube->GetSectorID() == 0 || tube->GetSectorID() == 5) {
            if (startsecid != 5 && endsecid != 5 && startsecid != 0 && endsecid != 0)
              continue;
          } else if (fabs(tube->GetSectorID() - startsecid) > 1 && fabs(tube->GetSectorID() - endsecid) > 1)
            continue;
          if (tube->GetLayerID() > startlayid && tube->GetLayerID() < endlayid)
            continue;
        }

        if (fDisplayOn) {
          display->cd(1);
          hit->DrawTube(kBlue);
          display->Update();
          display->Modified();
          char goOnChar;
          cout << "want to go to next?" << endl;
          // cin >> goOnChar;
        }

        thiscluster->AddHit(hit);
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
    //    cout << "want to go to next cluster2?" << endl;
    //    cin >> goOnChar;
  }
  // ---------------------------

  return thiscluster;
}

PndTrkCluster *PndTrkCombiLegendreTask::CreateClusterAroundTrack3(PndTrkTrack *track)
{

  PndTrkCluster *cluster = track->GetCluster();

  cout << "CREATE CLUSTER AROUND TRACK " << track->GetCluster() << " with hits " << track->GetCluster()->GetNofHits() << endl;

  double R = track->GetRadius();
  double xc = track->GetCenter().X();
  double yc = track->GetCenter().Y();

  // create cluster depending on fitting
  PndTrkCluster *thiscluster = new PndTrkCluster();
  for (int ihit = 0; ihit < cluster->GetNofHits(); ihit++) {
    PndTrkHit *hit = cluster->GetHit(ihit);
    double distance = hit->GetXYDistance(TVector3(xc, yc, 0.));

    double distancefromtrack = fabs(distance - R);
    if (distancefromtrack < 1) {
      thiscluster->AddHit(hit);
      if (fDisplayOn) {
        display->cd(1);
        hit->DrawTube(kGreen);
      }
    }
  }

  if (fDisplayOn) {
    display->cd(1);
    thiscluster->Draw(kRed);
    display->Update();
    display->Modified();
    char goOnChar;
    //    cout << "want to go to next cluster2?" << endl;
    //    cin >> goOnChar;
  }
  // ---------------------------

  return thiscluster;
}

void PndTrkCombiLegendreTask::AnalyticalFit(PndTrkCluster *cluster, double xc, double yc, double R, double &fitm, double &fitq)
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
    if (hit->IsSttParallel())
      IntersectionFinder(hit, xc, yc, R);
    PndTrkConformalHit *chit = conform->GetConformalHit(hit);
    double sigma = 0.1; // CHECK
    fFitter->SetPointToFit(chit->GetPosition().X(), chit->GetPosition().Y(), sigma);
    //      cout << "set point to fit " << chit->GetPosition().X() << " " <<  chit->GetPosition().Y() << endl;
    //     if(fDisplayOn) {
    // 	display->cd(1);
    // 	TMarker *mrk = new TMarker(hit->GetPosition().X(), hit->GetPosition().Y(), 6);
    // 	mrk->SetMarkerColor(kRed);
    // 	mrk->Draw("SAME");

    // 	display->cd(2);
    // 	TMarker *mrk2 = new TMarker(chit->GetPosition().X(), chit->GetPosition().Y(), 6);
    // 	mrk2->SetMarkerColor(kRed);
    // 	mrk2->Draw("SAME");

    // 	display->Update();
    // 	display->Modified();
    //       }
  }

  fFitter->StraightLineFit(fitm, fitq);
  cout << "previous " << xc << " " << yc << " " << R << endl;
  FromConformalToRealTrack(fitm, fitq, xc, yc, R);
  cout << "now " << xc << " " << yc << " " << R << endl;

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
}

void PndTrkCombiLegendreTask::AnalyticalFit2(PndTrkCluster *cluster, double fitm, double fitp, double &fitm2, double &fitp2)
{

  // fit with analytical chi2 -----~~~~~-----~~~~~-----~~~~~-----~~~~~-----~~~~~--
  fFitter->Reset();
  if (fDisplayOn) {
    display->cd(1);
    Refresh();
  }

  int counter = 0;
  for (int ihit = 0; ihit < cluster->GetNofHits(); ihit++) {
    PndTrkHit *hit = cluster->GetHit(ihit);
    if (hit == fRefHit) {
      //	cout << hit->GetHitID() << " ref hit " << endl;
      continue;
    }
    if (hit->IsSttSkew()) {
      //	cout << hit->GetHitID() << " skew " << endl;
      continue;
    }

    PndTrkConformalHit *chit = conform->GetConformalSttHit(hit);
    if (fDisplayOn) {
      display->cd(2);
      chit->Draw(1);
    }

    IntersectionFinder(chit, fitm, fitp);

    //      PndSttHit *stthit = (PndSttHit*) fSttHitArray->At(hit->GetHitID());
    //      double isoerr = stthit->GetIsochroneError();

    double sigma = chit->GetIsochrone();
    //      double sigma = hit->GetIsochrone() * chit->GetPosition().Y(); // * isoerr;

    fFitter->SetPointToFit(chit->GetPosition().X(), chit->GetPosition().Y(), sigma);
    counter++;
    //      cout << ihit << " set point to fit {" << chit->GetPosition().X() << ", " <<  chit->GetPosition().Y() << "}" << endl;
    //      cout << sigma << endl;
    //      cout << "C iso " << chit->GetIsochrone() << endl;
    //      cout << hit->GetIsochrone() << endl;

    if (fDisplayOn) {
      display->cd(1);
      double xreal, yreal, rreal;
      conform->PerformRealTransformation(chit->GetPosition().X(), chit->GetPosition().Y(), 0, xreal, yreal, rreal);
      TMarker *mrk = new TMarker(xreal, yreal, 6);
      mrk->SetMarkerColor(kRed);
      mrk->Draw("SAME");

      display->cd(2);
      TMarker *mrk2 = new TMarker(chit->GetPosition().X(), chit->GetPosition().Y(), 6);
      mrk2->SetMarkerColor(kRed);
      mrk2->Draw("SAME");

      // 	display->Update();
      // 	display->Modified();
    }
  }

  fFitter->StraightLineFit(fitm2, fitp2);

  double xc, yc, R;
  FromConformalToRealTrack(fitm, fitp, xc, yc, R);
  cout << "previous " << xc << " " << yc << " " << R << "/" << fitm << " " << fitp << endl;
  FromConformalToRealTrack(fitm2, fitp2, xc, yc, R);
  cout << "now " << xc << " " << yc << " " << R << "/" << fitm2 << " " << fitp2 << endl;

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

// ...........................................
// CHECK not satisfying at all...
Double_t PndTrkCombiLegendreTask::ComputePerpendicularChi2(PndTrkCluster *cluster, double fitm, double fitp)
{
  double chi2 = 0;
  for (int ihit = 0; ihit < cluster->GetNofHits(); ihit++) {
    PndTrkHit *hit = cluster->GetHit(ihit);
    if (hit == fRefHit) {
      //	cout << hit->GetHitID() << " ref hit " << endl;
      continue;
    }
    if (hit->IsSttSkew()) {
      //	cout << hit->GetHitID() << " skew " << endl;
      continue;
    }

    PndTrkConformalHit *chit = conform->GetConformalSttHit(hit);
    if (fDisplayOn) {
      display->cd(2);
      chit->Draw(1);
    }

    IntersectionFinder(chit, fitm, fitp);

    PndSttHit *stthit = (PndSttHit *)fSttHitArray->At(hit->GetHitID());
    PndSttPoint *sttpnt = (PndSttPoint *)fSttPointArray->At(stthit->GetRefIndex());
    double erriso = stthit->GetIsochroneError();
    double der = chit->GetPosition().Y() * chit->GetPosition().Y() + chit->GetPosition().X() * chit->GetPosition().X() - hit->GetIsochrone() * hit->GetIsochrone();
    double sigma = der * erriso;

    double distanceperp = fabs(chit->GetPosition().Y() - (chit->GetPosition().X() * fitm + fitp)) / TMath::Sqrt(1 + fitm * fitm);
    double chi = (distanceperp - chit->GetIsochrone()) / sigma;
    chi2 += chi * chi;
    cout << "=> " << distanceperp << " " << chit->GetIsochrone() << " " << sigma << " " << chi << " " << chi * chi << " " << chi2 << " " << sttpnt->GetTrackID() << endl;
  }

  cout << "final chi2 " << chi2 << endl;
  // ,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,

  return chi2;
}

void PndTrkCombiLegendreTask::IntersectionFinder(PndTrkConformalHit *chit, double fitm, double fitp)
{

  double xi1 = chit->GetU() + fitm * chit->GetIsochrone() / TMath::Sqrt(fitm * fitm + 1);
  double yi1 = chit->GetV() - chit->GetIsochrone() / TMath::Sqrt(fitm * fitm + 1);

  double xi2 = chit->GetU() - fitm * chit->GetIsochrone() / TMath::Sqrt(fitm * fitm + 1);
  double yi2 = chit->GetV() + chit->GetIsochrone() / TMath::Sqrt(fitm * fitm + 1);

  double xi = 0, yi = 0;

  fabs(yi1 - (fitm * xi1 + fitp)) < fabs(yi2 - (fitm * xi2 + fitp)) ? (yi = yi1, xi = xi1) : (yi = yi2, xi = xi2);

  chit->SetPosition(xi, yi);
}

void PndTrkCombiLegendreTask::IntersectionFinder(PndTrkHit *hit, double xc, double yc, double R)
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

// wiredirection | layerID | correlate with layerID
// PARALLEL      |   16    |
// S   [-3 deg]  |   15    | 16 ( --> post)
// K   [-3 deg]  |   14    | 13 ( --> pre)
// *   [+3 deg]  |   13    |
// E   [+3 deg]  |   12    | 11 ( --> pre)
// W   [-3 deg]  |   11    |
// *   [-3 deg]  |   10    | 9  ( --> pre)
// E   [+3 deg]  |    9    |
// D   [+3 deg]  |    8    | 7  ( --> pre)
// PARALLEL      |    7    |
PndTrkCluster *PndTrkCombiLegendreTask::ComputeSkewedXYZ(PndTrkCluster *cluster)
{
  /**
     PndTrkCluster *skewcluster = new PndTrkCluster();
     // loop on all the hist
     for(int ihit = 0; ihit < cluster->GetNofHits(); ihit++) {
     PndTrkHit *hit = cluster->GetHit(ihit);
     if(hit->IsStt() == kFALSE) continue;
     if(hit->IsSttParallel() == kTRUE) continue;

     PndSttTube *tube = (PndSttTube*) fTubeArray->At(hit->GetTubeID());
     int layID = tube->GetLayerID();
     if(layID != 8 && layID != 10 && layID != 12 && layID != 14 && layID != 15) continue;

     TObjArray *neighs = fHitMap->GetNeighboringsToHit(hit);

     TVector3 poca1(-999, -999, -999), poca2;
     double iso1, iso2;

     bool isfound = false;
     for(int jhit = 0; jhit < neighs->GetEntriesFast(); jhit++) {
     PndTrkHit *hit2 = (PndTrkHit*) neighs->At(jhit);
     PndSttTube *tube2 = (PndSttTube*) fTubeArray->At(hit2->GetTubeID());
     int layID2 = tube2->GetLayerID();

     // 8/10/12/14 seek the pre layer; 15 seeks the post layer
     if((layID != 15 && layID2 != layID - 1) || (layID == 15 && layID2 != layID + 1)) continue;

     // if all the "continue" were passed it means we are
     // dealing with two stereo tubes with opposite tilting
     // angle or a parallel + a stereo tube
     cout << "****************************" << endl;
     double distance = -999;
     if(poca1.X() == -999) {
     isfound = true;
     tube->GetPoca(tube2, poca1, poca2);
     iso1 = hit->GetIsochrone();
     iso2 = hit2->GetIsochrone();
     }
     else isfound = false;
     cout << endl;

     //     PndSttHit *hitI = (PndSttHit*) fSttHitArray->At(hit->GetHitID());
     //       PndSttHit *hitII = (PndSttHit*) fSttHitArray->At(hit2->GetHitID());

     //       TVector3 pocamiddle;
     //       Double_t poca = (fMapper->GetGeometryMap())->CalculateStrawPoca(hitI, hitII, pocamiddle);
     //       cout << "distance " << poca << endl;

     if(isfound == true) {
     if(layID != 7 && layID != 15) {
     PndTrkHit *newhit = new PndTrkHit(*hit);
     newhit->SetPosition(poca1);
     newhit->SetRegion(INNER_LEFT); // CHECK to consider this as parallel for the fit in xy
     skewcluster->AddHit(newhit);
     }
     if(layID2 != 7 && layID2 != 15) {
     PndTrkHit *newhit2 = new PndTrkHit(*hit2);
     newhit2->SetPosition(poca2);
     newhit2->SetRegion(INNER_LEFT); // CHECK to consider this as parallel for the fit in xy
     skewcluster->AddHit(newhit2);
     }

     if(fDisplayOn)  {
     cout << " STARTING" << endl;

     // 	   TMarker *mrkpoca1 = new TMarker(poca1.X(), poca1.Y(), 20);
     // 	   mrkpoca1->SetMarkerSize(0.5);
     // 	   mrkpoca1->Draw("SAME");
     // 	   TMarker *mrkpoca2 = new TMarker(poca2.X(), poca2.Y(), 20);
     // 	   mrkpoca2->SetMarkerSize(0.5);
     // 	   mrkpoca2->Draw("SAME");
     // 	   TMarker *mrkpoca = new TMarker(pocamiddle.X(), pocamiddle.Y(), 20);
     // 	   mrkpoca->SetMarkerColor(2);
     // 	   mrkpoca->SetMarkerSize(0.5);
     // 	   mrkpoca->Draw("SAME");

     TArc *mrkpoca1 = new TArc(poca1.X(), poca1.Y(), iso1);
     mrkpoca1->SetFillStyle(0);
     mrkpoca1->Draw("SAME");

     TArc *mrkpoca2 = new TArc(poca2.X(), poca2.Y(), iso2);
     mrkpoca2->SetFillStyle(0);
     mrkpoca2->Draw("SAME");

     display->Update();
     display->Modified();
     }
     }
     }
     }
     return skewcluster;
  **/
}

void PndTrkCombiLegendreTask::CleanTrack(PndTrkTrack *track)
{

  PndTrkCluster *cluster = track->GetCluster();

  if (fDisplayOn) {
    char goOnChar;
    cin >> goOnChar;
    Refresh();
    track->Draw();
    cluster->LightUp();
    display->Update();
    display->Modified();
  }

  // 1. count tracks in cluster_from_track
  int noftracksinlay[30];
  for (int ilay = 0; ilay < 30; ilay++)
    noftracksinlay[ilay] = 0;
  Int_t noftracks = CountTracksInCluster(cluster, noftracksinlay);
  cout << " NOF EFFECTIVE TRACKS IN THIS TRACK: " << noftracks << endl;

  int noflayerswith0tracks = 0, noflayerswith1track = 0, noflayerswithmoretracks = 0;
  int firstlayid = -1, lastlayid = -1;
  for (int ilay = 0; ilay < 30; ilay++) {
    // cout << "layer " << ilay << " " << noftracksinlay[ilay] << endl;

    // are there tracks in this layer?
    if (noftracksinlay[ilay] != 0) {
      if (firstlayid == -1)
        firstlayid = ilay;
      lastlayid = ilay;
      if (noftracksinlay[ilay] == 1)
        noflayerswith1track++;
      else
        noflayerswithmoretracks++;
    }
  }
  noflayerswith0tracks = (lastlayid - firstlayid + 1) - noflayerswith1track - noflayerswithmoretracks;

  cout << "last layer " << lastlayid << " first layer " << firstlayid << endl;
  cout << "#layers in range with 0 tracks " << noflayerswith0tracks << "; with 1 track " << noflayerswith1track << "; with more tracks " << noflayerswithmoretracks << endl;

  int classification = -1;
  // 0 = fake
  // 1 = one-way
  // 2 = full circle
  noflayerswith0tracks < noflayerswith1track ? classification = 1 : classification = 0;
  if (classification == 0)
    noflayerswith0tracks <= noflayerswithmoretracks ? classification = 2 : classification = 0;
  else if (classification == 1)
    noflayerswith1track <= noflayerswithmoretracks ? classification = 2 : classification = 1;

  switch (classification) {
  case 0: cout << "\033[1;30m THIS TRACK IS FAKE " << noflayerswith0tracks << " " << 100. * noflayerswith0tracks / (lastlayid - firstlayid + 1) << "%\033[0m" << endl; break;
  case 1: cout << "\033[1;30m THIS TRACK IS ONE-WAY " << noflayerswith1track << " " << 100. * noflayerswith1track / (lastlayid - firstlayid + 1) << "%\033[0m" << endl; break;
  case 2:
    cout << "\033[1;30m THIS IS A FULL CIRCLE " << noflayerswithmoretracks << " " << 100. * noflayerswithmoretracks / (lastlayid - firstlayid + 1) << "%\033[0m" << endl;
    break;
  default: cout << "\033[1;30m THIS IS NOT CLASSIFIED\033[0m" << endl;
  }

  for (int ihit = 0; ihit < cluster->GetNofHits(); ihit++) {
    PndTrkHit *hit = cluster->GetHit(ihit);
    int hitid = hit->GetHitID();
    int tubeid = hit->GetTubeID();
    PndSttTube *tube = (PndSttTube *)fTubeArray->At(tubeid);
    int layid = tube->GetLayerID();
    if (noftracksinlay[layid] > 2) {
      //    cout << "hit " << ihit << " hitid " << hitid << " tubeid " << tubeid << " layid " << layid << endl;
      if (fDisplayOn) {
        display->cd(1);
        //	hit->DrawTube(kBlue);
        //	display->Update();
        //	display->Modified();
      }
    }
  }
}

ClassImp(PndTrkCombiLegendreTask)
