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
// PndTrkLegendreSecTask2
//
// Class for secondary track pattern recognition
//
// authors: Lia Lavezzi - INFN Pavia (2012)
//
////////////////////////////////////////////////////////////

#include "PndTrkLegendreSecTask2.h"

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
// tracking
#include "PndTrkClusterList.h"
#include "PndTrkGlpkFits.h"
#include "PndTrkClean.h"

#include <iostream>

using namespace std;

// -----   Default constructor   -------------------------------------------
PndTrkLegendreSecTask2::PndTrkLegendreSecTask2()
  : FairTask("secondary track finder", 0), fDisplayOn(kFALSE), fPersistence(kTRUE), fUseMVDPix(kTRUE), fUseMVDStr(kTRUE), fUseSTT(kTRUE), fSecondary(kFALSE),
    fMvdPix_RealDistLimit(1000), fMvdStr_RealDistLimit(1000), fStt_RealDistLimit(1000), fMvdPix_ConfDistLimit(1000), fMvdStr_ConfDistLimit(1000), fStt_ConfDistLimit(1000),
    fInitDone(kFALSE)
{
  sprintf(fSttBranch, "STTHit");
  sprintf(fMvdPixelBranch, "MVDHitsPixel");
  sprintf(fMvdStripBranch, "MVDHitsStrip");
  PndGeoHandling::Instance();
}

PndTrkLegendreSecTask2::PndTrkLegendreSecTask2(int verbose)
  : FairTask("secondary track finder", verbose), fDisplayOn(kFALSE), fPersistence(kTRUE), fUseMVDPix(kTRUE), fUseMVDStr(kTRUE), fUseSTT(kTRUE), fSecondary(kFALSE),
    fMvdPix_RealDistLimit(1000), fMvdStr_RealDistLimit(1000), fStt_RealDistLimit(1000), fMvdPix_ConfDistLimit(1000), fMvdStr_ConfDistLimit(1000), fStt_ConfDistLimit(1000),
    fInitDone(kFALSE)
{
  sprintf(fSttBranch, "STTHit");
  sprintf(fMvdPixelBranch, "MVDHitsPixel");
  sprintf(fMvdStripBranch, "MVDHitsStrip");
  PndGeoHandling::Instance();
}

// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndTrkLegendreSecTask2::~PndTrkLegendreSecTask2() {}
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
InitStatus PndTrkLegendreSecTask2::Init()
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
    cout << "-E- PndTrkLegendreSecTask2::Init: "
         << "RootManager not instantiated, return!" << endl;
    return kFATAL;
  }

  // -- HITS -------------------------------------------------
  //
  // STT
  fSttHitArray = (TClonesArray *)ioman->GetObject(fSttBranch);
  if (!fSttHitArray) {
    cout << "-W- PndTrkLegendreSecTask2::Init: "
         << "No STTHit array, return!" << endl;
    return kERROR;
  }
  //
  // MVD PIXEL
  fMvdPixelHitArray = (TClonesArray *)ioman->GetObject(fMvdPixelBranch);
  if (!fMvdPixelHitArray) {
    std::cout << "-W- PndTrkLegendreSecTask2::Init: "
              << "No MVD Pixel hitArray, return!" << std::endl;
    return kERROR;
  }
  //
  // MVD STRIP
  fMvdStripHitArray = (TClonesArray *)ioman->GetObject(fMvdStripBranch);
  if (!fMvdStripHitArray) {
    std::cout << "-W- PndTrkLegendreSecTask2::Init: "
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
  }

  legendre = new PndTrkLegendreTransform();
  if (fSecondary)
    legendre->SetUpLegendreHisto(90, 0, 180, 1000, -1.5, 1.5); // CHECK
  else
    legendre->SetUpLegendreHisto();
  legendre->SetUpZoomHisto();

  conform = new PndTrkConformalTransform();

  tools = new PndTrkTools();
  fFitter = new PndTrkFitter(fVerbose);

  return kSUCCESS;
}

// -------------------------------------------------------------------------

void PndTrkLegendreSecTask2::SetParContainers()
{
  FairRuntimeDb *rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fSttParameters = (PndGeoSttPar *)rtdb->getContainer("PndGeoSttPar");
}

// -------------------------------------------------------------------------

void PndTrkLegendreSecTask2::Initialize()
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

// void PndTrkLegendreSecTask2::Reset()
// {

//   if(fDisplayOn) {
//     char goOnChar;
//     display->Update();
//     display->Modified();
//     cout << "Finish? ";
//     cin >> goOnChar;
//     cout << "FINISH" << endl;
//   }

//   if(fInitDone) {
//     if(stthitlist) delete stthitlist;
//     if(mvdpixhitlist)  delete mvdpixhitlist;
//     if(mvdstrhitlist)  delete mvdstrhitlist;
//     if(fTimer) {
//       fTimer->Stop();
//       fTime += fTimer->RealTime();

//       if(fVerbose > 0) cerr << fEventCounter << " Real time " << fTime << " s" << endl;
//     }
//   }

//   fInitDone = kFALSE;
// }

void PndTrkLegendreSecTask2::Exec(Option_t *)
{
  fTrackArray->Delete();
  fTrackCandArray->Delete();
  if (fVerbose > 0)
    cout << "*********************** " << fEventCounter << " ***********************" << endl;

  fEventCounter++;
  // CHECK THIS!
  if (fSttHitArray->GetEntriesFast() < 3) {
    //     Reset();
    return;
  }

  Initialize();
  if (fVerbose > 1) {
    cout << "number of stt    hits " << fSttHitArray->GetEntriesFast() << endl;
    cout << "number of mvdpix hits " << fMvdPixelHitArray->GetEntriesFast() << endl;
    cout << "number of mvdstr hits " << fMvdStripHitArray->GetEntriesFast() << endl;
  }

  // **************** CHECK **************
  //  for(int isec = 0; isec < 6; isec++) {
  //     if(stthitlist->GetNofHitsInSector(isec) == 0) continue;
  //     conformalhitlist = new PndTrkConformalHitList();
  //     if(fDisplayOn)  {
  //       Refresh();
  //       char goOnChar;
  //       // cout << "Start?";
  //       //  cin >> goOnChar;
  //       display->Update();
  //       display->Modified();
  //       cout << " STARTING" << endl;
  //       cin >> goOnChar;
  //       stthitlist->DrawSector(isec, kGreen);
  //       stthitlist->PrintSector(isec);
  //       display->Update();
  //       display->Modified();
  //       cout << " STARTING" << endl;
  //       cin >> goOnChar;
  //     }

  //   std::vector< PndTrkHit* > sttinsec = stthitlist->GetHitListFromSector(isec);

  //   }

  //  for(int ilay = 0; ilay < 26; ilay++) {
  //     if(stthitlist->GetNofHitsInLayer(ilay) == 0) continue;
  //     conformalhitlist = new PndTrkConformalHitList();
  //     if(fDisplayOn)  {
  //       Refresh();
  //       char goOnChar;
  //       // cout << "Start?";
  //       //  cin >> goOnChar;
  //       display->Update();
  //       display->Modified();
  //       cout << " STARTING" << endl;
  //       cin >> goOnChar;
  //       stthitlist->DrawLayer(ilay, kGreen);
  //       stthitlist->PrintLayer(ilay);
  //       display->Update();
  //     display->Modified();
  //       cout << " STARTING" << endl;
  //       cin >> goOnChar;
  //     }

  //   std::vector< PndTrkHit* > sttinlay = stthitlist->GetHitListFromLayer(ilay);

  //   }

  //   for(int isec = 0; isec < 6; isec++) {

  //     for(int ilay = 0; ilay < 26; ilay++) {
  //       if(stthitlist->GetNofHitsInSectorAndLayer(isec, ilay) == 0) continue;
  //       conformalhitlist = new PndTrkConformalHitList();
  //       if(fDisplayOn)  {
  // 	Refresh();
  // 	char goOnChar;
  // 	cout << "Start?";
  // 	cin >> goOnChar;
  // 	display->Update();
  // 	display->Modified();
  // 	cout << " STARTING" << endl;
  // 	cin >> goOnChar;
  // 	stthitlist->DrawSectorAndLayer(isec, ilay, kGreen);
  // 	stthitlist->PrintSectorAndLayer(isec, ilay);
  // 	display->Update();
  // 	display->Modified();
  // 	cout << " STARTING" << endl;
  // 	cin >> goOnChar;
  //       }

  //       std::vector< PndTrkHit* > sttinlay = stthitlist->GetHitListFromSectorAndLayer(isec, ilay);

  //     }
  //   }
  // ********************************************

  PndTrkClusterList clusterlist;
  PndTrkCluster *cluster = nullptr;

  //   for(int isec = 0; isec < 6; isec++) {
  //     cout << "@@@@@@@@@@@@@@@@@ SECTOR  " << isec << endl;

  //     std::vector< PndTrkHit* > sttinsec = stthitlist->GetHitListFromSector(isec);

  {
    std::vector<PndTrkHit *> sttinsec = stthitlist->GetHitList();

    //   int toassigncounter = sttinsec.size();
    Bool_t stilltoassign = kTRUE;
    //   while(toassigncounter > 0) {
    while (stilltoassign == kTRUE) {

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

      cluster = new PndTrkCluster();
      PndTrkHit *tmphit = nullptr;

      for (int ilay = 0; ilay < 26; ilay++) {
        //       std::vector< PndTrkHit* > sttinlay = stthitlist->GetHitListFromSectorAndLayer(isec, ilay);
        std::vector<PndTrkHit *> sttinlay = stthitlist->GetHitListFromLayer(ilay);

        for (int ihit = 0; ihit < sttinlay.size(); ihit++) {
          PndTrkHit *thishit = sttinlay.at(ihit);
          if (thishit->IsUsed() == kTRUE)
            continue;
          // cout << "1TMPHIT " << tmphit << endl;
          //  if(tmphit) cout << " " << tmphit->GetHitID () << endl;
          if (tmphit == nullptr) {
            tmphit = thishit;
            //	    toassigncounter--;
            cluster->AddHit(thishit);
            //  cout << " start add " << thishit->GetHitID() << endl;
            thishit->SetUsedFlag(kTRUE);
            //	    cout << "2TMPHIT " << tmphit << endl;
            continue;
          }
          // .........

          PndSttTube *tmptube = (PndSttTube *)fTubeArray->At(tmphit->GetTubeID());

          if (tmptube->IsNeighboring(thishit->GetTubeID())) {
            cluster->AddHit(sttinlay.at(ihit));
            tmphit = thishit;
            thishit->SetUsedFlag(kTRUE);
            //  cout << " add " << thishit->GetHitID() << endl;
            //	    toassigncounter--;
          }
          //	  else cout << "not " << thishit->GetHitID () << endl;
        }
      }

      tmphit = nullptr;
      if (cluster->GetNofHits() > 3)
        clusterlist.AddCluster(cluster);
      else {
        // check if there is something more to assign
        stilltoassign = CheckAssignability(sttinsec);
        continue;
      }

      if (fDisplayOn) {
        Refresh();
        char goOnChar;
        //	cin >> goOnChar;
        cout << "ADD CLUSTER " << cluster->GetNofHits() << endl;

        cluster->LightUp();
        display->Update();
        display->Modified();
      }

      //      cout  << "clusterlist.size " << clusterlist.GetNofClusters() << endl;
      //    if(fDisplayOn)  {
      //    	Refresh();
      //    	char goOnChar;
      //    	cout << "Start?";
      //    	cin >> goOnChar;
      // 	cout << "ADD CLUSTER " << cluster->GetNofHits() << endl;

      //    	stthitlist->DrawSector(isec, kGreen);
      //    	stthitlist->PrintSector(isec);
      // 	cluster->LightUp();
      //    	display->Update();
      //    	display->Modified();
      //    	cout << " NEXT" << endl;
      //    	cin >> goOnChar;
      //       }

      // ================ --> TO CONFORMAL PLANE

      conformalhitlist = new PndTrkConformalHitList();

      // translation and rotation
      Int_t nchits = 0;
      Double_t delta = 0, trasl[2] = {0., 0.};
      if (fSecondary) {
        // translation and rotation - CHECK
        //	cout << " REFERENCE HIT " << cluster->GetNofHits() << endl;
        fRefHit = FindReferenceHit(cluster);
        if (fRefHit == nullptr) {
          //	  cout << "REFHIT " << fRefHit << endl;
          Reset();
          return;
        }
        ComputeTraAndRot(fRefHit, delta, trasl);
      }
      //      cout << "DELTA " << delta << " TRASL " << trasl[0] << " " << trasl[1] << endl;
      conform->SetOrigin(trasl[0], trasl[1], delta);
      nchits = FillConformalHitList();

      if (nchits == 0) {
        Reset();
        return;
      }

      // APPLY LEGENDRE TO STT ALONE
      // -------------------------------------------------------
      //   cout << "APPLY LEGENDRE =======================" << endl;
      double theta_max, r_max;
      Int_t maxpeak = ApplyLegendre(cluster, theta_max, r_max);
      if (maxpeak <= 3) {
        // check if there is something more to assign
        stilltoassign = CheckAssignability(sttinsec);

        continue; // break;
      }

      //      cout << "EXTRACT MAX =======================" << endl;

      double fitm, fitq;
      legendre->ExtractLegendreSingleLineParameters(fitm, fitq);
      if (fDisplayOn) {
        display->cd(2);
        TLine *line = new TLine(-10.07, fitq + fitm * (-10.07), 10.07, fitq + fitm * (10.07));
        line->Draw("SAME");

        display->Update();
        display->Modified();
      }

      // center and radius
      Double_t xc, yc, R;
      FromConformalToRealTrack(fitm, fitq, xc, yc, R);
      //      if(fVerbose > 1)  cout << "XR, YC, R: " << xc << " " << yc << " " << R << endl;
      PndTrkTrack *track = new PndTrkTrack(cluster, xc, yc, R);
      if (fDisplayOn) {
        display->cd(1);
        track->Draw(kRed);
        display->Update();
        display->Modified();
        char goOnChar;
        cout << "waiting 3 " << endl;
        cin >> goOnChar;
      }

      // ADD HITS TO CLUSTER: PIXEL, STRIP AND STT PARALLEL
      // -------------------------------------------------------
      // method of hit-to-track association:
      // 0 conformal: mvd and stt in conformal plane
      // 1 real: mvd and stt in real plane
      // 2 mixed: mvd in real/stt in conformal plane
      int method = 1;
      //   PndTrkCluster thiscluster = CreateClusterByDistance(method, fitm, fitq);
      //       cluster = &thiscluster;
      int counteradded = AddHitToClusterByDistance(cluster, method, fitm, fitq);

      // CLEANUP
      // -------------------------------------------------------
      PndTrkCluster thiscluster = Cleanup(*cluster);
      cluster = &thiscluster;

      double fitm2, fitq2;
      fFitter->Reset();

      for (int ihit = 0; ihit < cluster->GetNofHits(); ihit++) {
        PndTrkHit *hit = cluster->GetHit(ihit);
        //  if(!hit)  cout << "MISSING" << endl;
        //  else  cout << "IHIT " << ihit << " is here " << endl;
        double conformal[3];
        //	    cout << "hit " << hit->GetHitID() << " detid " << hit->GetDetectorID () << endl;
        PndTrkConformalHit *chit = nullptr;
        if (hit->IsSttSkew())
          continue;
        else if (hit->IsSttParallel() == kTRUE)
          chit = conform->GetConformalSttHit(hit);
        else
          chit = conform->GetConformalHit(hit);

        double xi1 = chit->GetU() + fitm * chit->GetIsochrone() / TMath::Sqrt(fitm * fitm + 1);
        double yi1 = chit->GetV() - chit->GetIsochrone() / TMath::Sqrt(fitm * fitm + 1);

        double xi2 = chit->GetU() - fitm * chit->GetIsochrone() / TMath::Sqrt(fitm * fitm + 1);
        double yi2 = chit->GetV() + chit->GetIsochrone() / TMath::Sqrt(fitm * fitm + 1);

        double xi = 0, yi = 0;

        fabs(yi1 - (fitm * xi1 + fitq)) < fabs(yi2 - (fitm * xi2 + fitq)) ? (yi = yi1, xi = xi1) : (yi = yi2, xi = xi2);

        double sigma = chit->GetIsochrone();
        fFitter->SetPointToFit(xi, yi, sigma);

        if (fDisplayOn) {
          display->cd(2);
          TMarker *mrk = new TMarker(xi, yi, 6);
          mrk->Draw("SAME");
        }
      }

      fFitter->StraightLineFit(fitm2, fitq2);

      FromConformalToRealTrack(fitm2, fitq2, xc, yc, R);
      //	if(fVerbose > 1)
      if (fVerbose > 1)
        cout << "FITTER XR, YC, R: " << xc << " " << yc << " " << R << endl;
      track->SetRadius(R);
      track->SetCenter(xc, yc);

      if (fDisplayOn) {
        char goOnChar;
        display->cd(2);
        TLine *line = new TLine(-0.07, fitq + fitm * (-0.07), 0.07, fitq + fitm * (0.07));
        line->Draw("SAME");
        TLine *line2 = new TLine(-0.07, fitq2 + fitm2 * (-0.07), 0.07, fitq2 + fitm2 * (0.07));
        line2->SetLineColor(2);
        line2->Draw("SAME");

        display->cd(1);
        track->Draw(kGreen);

        display->Update();
        display->Modified();
        cin >> goOnChar;
      }

      fitm = fitm2;
      fitq = fitq2;

      // check if there is something more to assign
      stilltoassign = CheckAssignability(sttinsec);

      //	cout << "still to assing " << stilltoassign << endl;

      // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      // SKEWED ASSOCIATION ********* CHECK *********
      // -------------------------------------------------------
      if (fVerbose > 1)
        cout << "%%%%%%%%%%%%%%%%%%%% ZFINDER %%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;

      if (fDisplayOn) {
        RefreshZ();
        DrawZGeometry(2);
      }

      // 	PndTrkCluster skewhitlist;
      // 	for(int ihit = 0; ihit < cluster->GetNofHits(); ihit++) {
      // 	  PndTrkHit *hit = (PndTrkHit*) cluster->GetHit(ihit);
      // 	  if(hit->IsSttSkew() == kTRUE) skewhitlist.AddHit(hit);
      // 	}

      PndTrkCluster skewhitlist = CreateSkewHitList(track);
      skewhitlist = CleanUpSkewHitList(&skewhitlist);

      // 	for(int ihit = 0; ihit < skewhitlist.GetNofHits(); ihit++) {
      // 	  PndTrkSkewHit *skewhit = (PndTrkSkewHit*) skewhitlist.GetHit(ihit);
      // 	  cout << "SKEWED " << ihit << endl;
      // 	  skewhit->GetPosition().Print();
      // 	}

      int iz = -1, jz = -1;
      //   cout  << "##################### CHOOSE Z" << endl;

      // 7. select which intersection is the most likely and add it to z fitting
      fFitter->Reset();
      for (int ihit = 0; ihit < skewhitlist.GetNofHits() - 1; ihit++) {
        PndTrkSkewHit *skewhit1 = (PndTrkSkewHit *)skewhitlist.GetHit(ihit);
        if (!skewhit1)
          continue;
        TVector3 fin_intersection1[2] = {skewhit1->GetIntersection1(), skewhit1->GetIntersection2()};
        double phi1[2] = {skewhit1->GetPhi1(), skewhit1->GetPhi2()};

        int jhit = ihit + 1;
        PndTrkSkewHit *skewhit2 = (PndTrkSkewHit *)skewhitlist.GetHit(jhit);
        if (!skewhit2)
          continue;
        TVector3 fin_intersection2[2] = {skewhit2->GetIntersection1(), skewhit2->GetIntersection2()};
        double phi2[2] = {skewhit2->GetPhi1(), skewhit2->GetPhi2()};

        //     cout << "start from " << ihit << " " << jhit << ": " << iz << " " << jz << endl;
        if (jz != -1) {
          iz = jz;
          double distance = fabs(fin_intersection1[iz].Z() - fin_intersection2[0].Z());
          jz = 0;
          // 11 22
          fabs(fin_intersection1[iz].Z() - fin_intersection2[1].Z()) < distance ? (distance = fabs(fin_intersection1[iz].Z() - fin_intersection2[1].Z()), jz = 1) : distance;
        } else {
          // 11 21
          double distance = fabs(fin_intersection1[0].Z() - fin_intersection2[0].Z());
          iz = 0;
          jz = 0;
          // 11 22
          fabs(fin_intersection1[0].Z() - fin_intersection2[1].Z()) < distance ? (distance = fabs(fin_intersection1[0].Z() - fin_intersection2[1].Z()), iz = 0, jz = 1) : distance;
          // 12 21
          fabs(fin_intersection1[1].Z() - fin_intersection2[0].Z()) < distance ? (distance = fabs(fin_intersection1[1].Z() - fin_intersection2[0].Z()), iz = 1, jz = 0) : distance;
          // 12 22
          fabs(fin_intersection1[1].Z() - fin_intersection2[1].Z()) < distance ? (distance = fabs(fin_intersection1[1].Z() - fin_intersection2[1].Z()), iz = 1, jz = 1) : distance;
        }
        //      cout << "end with " << ihit << " " << jhit << ": " << iz << " " << jz << endl;

        fFitter->SetPointToFit(phi1[iz], fin_intersection1[iz].Z(), 1.); // (fin_intersection1[0].Z() + fin_intersection1[1].Z())/2.); // CHECK sigma?

        // CHECK
        //	  cout << "SKEWED " << ihit << endl;
        //	  skewhit1->GetPosition().Print();
        // 	  skewhit1->GetIntersection1().Print();
        // 	  skewhit1->GetIntersection2().Print();

        if (iz == 0)
          skewhit1->SetPosition(skewhit1->GetIntersection1());
        else
          skewhit1->SetPosition(skewhit1->GetIntersection2());
        // 	  skewhit1->GetPosition().Print();

        // CHECK last point?
        if (ihit == skewhitlist.GetNofHits() - 2) {
          fFitter->SetPointToFit(phi2[jz], fin_intersection2[jz].Z(), 1.); // (fin_intersection2[0].Z() + fin_intersection2[1].Z())/2.); // CHECK sigma?

          // CHECK
          if (jz == 0)
            skewhit2->SetPosition(skewhit2->GetIntersection1());
          else
            skewhit2->SetPosition(skewhit2->GetIntersection2());
        }

        if (fDisplayOn) {
          char goOnChar;
          display->cd(4);
          TMarker *mrkfoundzphi1 = new TMarker(phi1[iz], fin_intersection1[iz].Z(), 20);
          mrkfoundzphi1->SetMarkerColor(kYellow);
          mrkfoundzphi1->Draw("SAME");

          TMarker *mrkfoundzphi2 = new TMarker(phi2[jz], fin_intersection2[jz].Z(), 20);
          mrkfoundzphi2->SetMarkerColor(kYellow);
          mrkfoundzphi2->Draw("SAME");

          display->Update();
          display->Modified();
          //
          // cin >> goOnChar;
        }
      }

      // ADD MVD POINTS ==================== RECHECK
      PndTrkCluster *mvdcluster = track->GetCluster();
      for (int ihit = 0; ihit < mvdcluster->GetNofHits(); ihit++) {
        //      cout << ihit << " TROVATO " << endl;
        PndTrkHit *hit = mvdcluster->GetHit(ihit);
        if (hit->GetDetectorID() != FairRootManager::Instance()->GetBranchId(fMvdPixelBranch) && hit->GetDetectorID() != FairRootManager::Instance()->GetBranchId(fMvdStripBranch))
          continue;
        TVector3 position = hit->GetPosition();
        double phi = track->ComputePhi(position);
        hit->SetPhi(phi);
        //      cout << ihit << " TROVATO " << phi << " " << position.Z() << endl;

        if (fDisplayOn) {
          char goOnChar;
          display->cd(4);
          TMarker *mrkfoundzphi = new TMarker(phi, position.Z(), 21);
          mrkfoundzphi->SetMarkerColor(kOrange);
          mrkfoundzphi->Draw("SAME");
          display->Update();
          display->Modified();
          //	cin >> goOnChar;
        }
        fFitter->SetPointToFit(phi, position.Z(), 0.1); // CHECK ERROR
      }
      // ===================================

      Double_t ml, pl;
      Bool_t straightlinefit = fFitter->StraightLineFit(ml, pl);

      // CHECK the following fit: if it fails, put the hits into play again?
      if (straightlinefit == kTRUE) {

        if (fDisplayOn) {
          char goOnChar;
          display->cd(4);
          TLine *l222 = new TLine(-1000, -1000 * ml + pl, 1000, 1000 * ml + pl);
          l222->Draw("SAME");
          display->cd(3);
          l222->Draw("SAME");
          display->Update();
          display->Modified();
          //
          //     cin >> goOnChar;
        }

        PndTrkCluster *trkcluster = track->GetCluster();
        for (int ihit = 0; ihit < trkcluster->GetNofHits(); ihit++) {
          PndTrkHit *hit = trkcluster->GetHit(ihit);
          //	    cout << "cluster ihit " << ihit << " " << hit->GetDetectorID() << " " << hit->GetHitID() << endl;
        }

        // 8. last association z --> HIT: fill position and add to cluster ???? CHECK
        for (int ihit = 0; ihit < skewhitlist.GetNofHits(); ihit++) {
          PndTrkSkewHit *skewhit = (PndTrkSkewHit *)skewhitlist.GetHit(ihit);

          //	    cout << "ihit " << ihit << " " << skewhit->GetHitID() << " is already there? " << trkcluster->DoesContain(skewhit) << endl;
          if (trkcluster->DoesContain(skewhit) == kTRUE) {
            trkcluster->Replace(skewhit);
          } else
            trkcluster->AddHit(skewhit);
          TVector3 intersection1 = skewhit->GetIntersection1();
          double phi1 = track->ComputePhi(intersection1);
          double calcz1 = ml * phi1 + pl;
          TVector3 intersection2 = skewhit->GetIntersection2();
          double phi2 = track->ComputePhi(intersection2);
          double calcz2 = ml * phi2 + pl;

          fabs(intersection1.Z() - calcz1) < fabs(intersection2.Z() - calcz2) ? skewhit->SetPosition(intersection1) : skewhit->SetPosition(intersection2);
        }

        // 9. add skewedhitlist to cluster of the track
        // fill the last two (missing) parameters

        for (int ihit = 0; ihit < trkcluster->GetNofHits(); ihit++) {
          PndTrkHit *hit = trkcluster->GetHit(ihit);
          double phi = track->ComputePhi(hit->GetPosition()); // CHECK is it neede to recalculate it?
          //   cout << "hit get pos " << endl;
          //
          //	    if(hit->IsSttSkew()) cout << "getp pos " << ihit << " " << hit->GetDetectorID() << " " << hit->GetHitID()  << " " <<  hit->GetPosition().X() << " " <<
          // hit->GetPosition().Y() << " " << hit->GetPosition().Z() << endl;

          hit->SetSortVariable(phi);
          hit->SetPhi(phi);
        }
        trkcluster->Sort();
        track->ComputeCharge();

        // 10. z-phi refit
        Bool_t zfit = ZPhiFit(0, trkcluster, ml, pl);
        //      cout << "zfit1 on " << trkcluster->GetNofHits() << " hits/ " << zfit << " " << ml << " " << pl << " tanl " << - track->GetCharge() * ml * (180./TMath::Pi())/R <<
        //      endl;

        PndTrkCluster *tmpcluster = CleanupZPhiFit(trkcluster, ml, pl);
        trkcluster = tmpcluster;

        if (fDisplayOn) {
          char goOnChar;
          display->cd(4);
          DrawZGeometry();
          hzphi->SetXTitle("#phi");
          hzphi->SetYTitle("#z");
          hzphi->Draw();
          display->Update();
          display->Modified();
        }

        zfit = ZPhiFit(1, trkcluster, ml, pl);

        // 11. fill the last two (missing) parameters

        //  tanl = -q ml (180/pi) /R: See PndTrkTrack.h for an explanation

        if (zfit) {
          track->SetTanL(-track->GetCharge() * ml * (180. / TMath::Pi()) / R);
          track->SetZ0(pl);
        } else { // CHECK put these in default
          track->SetTanL(-999);
          track->SetZ0(-999);
        }
        //   cout << "GET TANL " << ml << " " << R << " " << track->GetTanL() << endl;

        // test -------------------------------------------------------
        PndTrkCluster *trkcluster2 = track->GetCluster();
        // trkcluster2->Print();

        if (fDisplayOn) {
          char goOnChar;
          display->cd(1);
          Refresh();
          for (int ihit = 0; ihit < trkcluster2->GetNofHits(); ihit++) {
            PndTrkHit *hit = trkcluster2->GetHit(ihit);
            hit->Draw(kMagenta);

            display->Update();
            display->Modified();
            //	cin >> goOnChar;
          }
        }
      } else { // CHECK put these in default
        track->SetTanL(-999);
        track->SetZ0(-999);
      }
      // ------------------------------------------------------- end test

      // TRANSFORM TO PNDTRACK AND PNDTRACKCAND
      // -------------------------------------------------------
      RegisterTrack(track);
    }

    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  }

  cluster = nullptr;
  delete cluster;

  if (fDisplayOn) {
    char goOnChar;
    display->Update();
    display->Modified();
    cout << "Finish? ";
    cin >> goOnChar;
    cout << "FINISH" << endl;
  }
  //  if(fDisplayOn)  {

  //     for(int iclus = 0; iclus < clusterlist.GetNofClusters(); iclus++) {

  //       cluster = clusterlist.GetCluster(iclus);
  //       Refresh();
  //       char goOnChar;
  //       cout << "...Start?";
  //       cin >> goOnChar;
  //       cout << "ADD CLUSTER " << cluster->GetNofHits() << endl;

  //       cluster->LightUp();
  //       display->Update();
  //       display->Modified();
  //       cout << " NEXT" << endl;
  //       cin >> goOnChar;
  //     }
  //   }
  Reset();
}

Bool_t PndTrkLegendreSecTask2::CheckAssignability(std::vector<PndTrkHit *> hitlist)
{

  for (int ihit = 0; ihit < hitlist.size(); ihit++) {
    PndTrkHit *hit = hitlist.at(ihit);
    if (hit->IsUsed() == kFALSE)
      return kTRUE;
  }
  return kFALSE;
}

void PndTrkLegendreSecTask2::Reset()
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

Int_t PndTrkLegendreSecTask2::FillConformalHitList(int isec)
{

  conformalhitlist->SetConformalTransform(conform);

  // FILL ONCE FOR ALL THE CONFORMAL HIT LIST
  for (int jhit = 0; jhit < mvdpixhitlist->GetNofHits(); jhit++) {
    PndTrkHit *hit = mvdpixhitlist->GetHit(jhit);
    if (hit == fRefHit)
      continue;
    PndTrkConformalHit *chit = conform->GetConformalHit(hit);
    conformalhitlist->AddHit(chit);
  }
  for (int jhit = 0; jhit < mvdstrhitlist->GetNofHits(); jhit++) {
    PndTrkHit *hit = mvdstrhitlist->GetHit(jhit);
    if (hit == fRefHit)
      continue;
    PndTrkConformalHit *chit = conform->GetConformalHit(hit);
    conformalhitlist->AddHit(chit);
  }

  if (isec == -1) {
    for (int jhit = 0; jhit < stthitlist->GetNofHits(); jhit++) {
      PndTrkHit *hit = stthitlist->GetHit(jhit);
      if (hit == fRefHit)
        continue;

      if (hit->IsSttSkew()) {
        //	continue;
        PndTrkConformalHit *chit = conform->GetConformalHit(hit);
        conformalhitlist->AddHit(chit);
      } else {
        PndTrkConformalHit *chit = conform->GetConformalSttHit(hit);
        conformalhitlist->AddHit(chit);
      }
      //     PndTrkConformalHit * chit = conform->GetConformalSttHit(hit);
      //       conformalhitlist->AddHit(chit);
    }
  } else {
    for (int jhit = 0; jhit < stthitlist->GetNofHitsInSector(isec); jhit++) {
      PndTrkHit *hit = stthitlist->GetHitFromSector(jhit, isec);
      //      cout << "fil conformal " << hit->GetHitID() << " " << hit->GetDetectorID() << endl;
      if (hit->IsSttSkew()) {
        //	continue;
        PndTrkConformalHit *chit = conform->GetConformalHit(hit);
        conformalhitlist->AddHit(chit);
      } else {
        PndTrkConformalHit *chit = conform->GetConformalSttHit(hit);
        conformalhitlist->AddHit(chit);
      }
      //      PndTrkConformalHit * chit = conform->GetConformalSttHit(hit);
      //       conformalhitlist->AddHit(chit);
    }
  }

  return conformalhitlist->GetNofHits();
}

Int_t PndTrkLegendreSecTask2::FillConformalHitList(PndTrkCluster *cluster)
{

  conformalhitlist->SetConformalTransform(conform);

  for (int jhit = 0; jhit < cluster->GetNofHits(); jhit++) {
    PndTrkHit *hit = cluster->GetHit(jhit);
    if (hit == fRefHit)
      continue;
    PndTrkConformalHit *chit = nullptr;
    cout << "HIT " << hit->GetHitID() << " " << hit->IsSttParallel() << " " << hit->IsSttSkew() << endl;
    if (hit->IsSttParallel() == kTRUE)
      chit = conform->GetConformalSttHit(hit);
    else
      chit = conform->GetConformalHit(hit);
    conformalhitlist->AddHit(chit);
    cout << "CONFORMAL " << chit->GetU() << " " << chit->GetV() << " " << chit->GetIsochrone() << endl;
  }
  return conformalhitlist->GetNofHits();
}

void PndTrkLegendreSecTask2::FillLegendreHisto(Int_t mode)
{
  // mode 0 STT alone
  //      1 STT + MVD
  for (int ihit = 0; ihit < conformalhitlist->GetNofHits(); ihit++) {
    PndTrkConformalHit *chit = conformalhitlist->GetHit(ihit);
    //   if(mode == 0 && chit->GetDetectorID() != FairRootManager::Instance()->GetBranchId(fSttBranch)) continue;
    PndTrkHit *hit = chit->GetHit();
    //    if(hit->IsUsed()) continue;
    legendre->FillLegendreHisto(chit->GetU(), chit->GetV(), chit->GetIsochrone());
    if (fDisplayOn) {
      display->cd(2);
      //  cout << "conformal hit " << chit->GetU() << " " <<  chit->GetV() << endl;
      DrawConfHit(chit->GetU(), chit->GetV(), chit->GetIsochrone());
    }
  }
}

void PndTrkLegendreSecTask2::FillLegendreHisto(PndTrkCluster *cluster)
{
  // ---------------------------------------------------------------
  //  cout << "FILL LEGENDRE HISTO " << cluster->GetNofHits() << endl;

  for (int ihit = 0; ihit < conformalhitlist->GetNofHits(); ihit++) {
    PndTrkConformalHit *chit = conformalhitlist->GetHit(ihit);
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

PndTrkCluster PndTrkLegendreSecTask2::CreateClusterByConfDistance(double fitm, double fitq)
{

  PndTrkCluster cluster;
  // ADD HIT IN CONFORMAL PLANE WITH DISTANCE CRITERION
  // cost x + sint y - r = 0 -> |cost x0 + sint y0 - r|
  //    cout << "CONFORMALHITLIST " << conformalhitlist->GetNofHits() << endl;
  for (int ihit = 0; ihit < conformalhitlist->GetNofHits(); ihit++) {
    PndTrkConformalHit *chit = conformalhitlist->GetHit(ihit);
    double dist = chit->GetDistanceFromTrack(fitm, fitq);
    int hitID = chit->GetHitID();
    int detID = chit->GetDetectorID();

    // CHECK limits
    double distlimit = 0;
    if (detID == FairRootManager::Instance()->GetBranchId(fMvdPixelBranch))
      distlimit = 0.003;
    else if (detID == FairRootManager::Instance()->GetBranchId(fMvdStripBranch))
      distlimit = 0.003;
    else if (detID == FairRootManager::Instance()->GetBranchId(fSttBranch)) {
      if (chit->GetIsochrone() > 1.e-4)
        distlimit = 6 * chit->GetIsochrone();
      else
        distlimit = 0.001;
    }
    // ---------------------------
    Bool_t accept = kFALSE;

    if (dist < distlimit) {
      if (fDisplayOn) {
        if (chit->GetIsochrone() > 0) {
          TArc *arc = new TArc(chit->GetU(), chit->GetV(), chit->GetIsochrone());
          arc->SetFillStyle(0);
          arc->SetLineColor(5);
          display->cd(2);
          arc->Draw("SAME");
        } else {
          TMarker *mrk = nullptr;
          if (detID == FairRootManager::Instance()->GetBranchId(fMvdPixelBranch))
            mrk = new TMarker(chit->GetU(), chit->GetV(), 21);
          else if (detID == FairRootManager::Instance()->GetBranchId(fMvdStripBranch))
            mrk = new TMarker(chit->GetU(), chit->GetV(), 25);
          if (mrk != nullptr) {
            mrk->SetMarkerColor(5);
            display->cd(2);
            mrk->Draw("SAME");
          }
        }
      }

      PndTrkHit *hit;
      if (detID == FairRootManager::Instance()->GetBranchId(fMvdPixelBranch)) {
        hit = mvdpixhitlist->GetHit(hitID);
      } else if (detID == FairRootManager::Instance()->GetBranchId(fMvdStripBranch)) {
        hit = mvdstrhitlist->GetHit(hitID);
      } else if (detID == FairRootManager::Instance()->GetBranchId(fSttBranch)) {
        hit = stthitlist->GetHit(hitID);
      }

      // ********** CHECK ***********
      // if you put "is used" the efficiency beecomes much lower!
      // DO NOT: 	if(!hit->IsUsed())
      cluster.AddHit(hit);
      accept = kTRUE;
    }

    //      else cout << "DISCARDED " << hitID << " " << detID << " " << dist << " " << distlimit << endl;
  }

  return cluster;
}

PndTrkCluster PndTrkLegendreSecTask2::CreateClusterByRealDistance(double xc0, double yc0, double R0)
{
  // ADD HIT IN REAL PLANE WITH DISTANCE CRITERION
  // sqrt((xc - x)**2 + (yc - y)**2) < distlim
  PndTrkCluster cluster;

  // .........................................................
  if (fDisplayOn) {
    TArc *arc = new TArc(xc0, yc0, R0);
    arc->SetFillStyle(0);
    arc->SetLineColor(5);
    display->cd(1);
    arc->Draw("SAME");
  }
  for (int ihit = 0; ihit < stthitlist->GetNofHits(); ihit++) {
    PndTrkHit *hit = stthitlist->GetHit(ihit);
    if (hit->IsSttSkew())
      continue;
    double dist = hit->GetXYDistanceFromTrack(xc0, yc0, R0);
    //      cout << ihit << " dist " << dist << endl;
    // CHECK limits
    double distlimit = 1.5 * 0.5;
    if (dist < distlimit) {
      //  cout << "TRACK  "  << xc0  << " " << yc0 << " " << R0 << endl;
      //  cout << "ADD HIT " << dist << " " << distlimit << " " << hit->GetHitID() << endl;
      cluster.AddHit(hit);
      if (fDisplayOn) {
        TArc *arc = new TArc(hit->GetPosition().X(), hit->GetPosition().Y(), hit->GetIsochrone());
        arc->SetFillStyle(0);
        arc->SetLineColor(5);
        display->cd(1);
        arc->Draw("SAME");
      }
    }
  }
  for (int ihit = 0; ihit < mvdpixhitlist->GetNofHits(); ihit++) {
    PndTrkHit *hit = mvdpixhitlist->GetHit(ihit);
    double dist = hit->GetXYDistanceFromTrack(xc0, yc0, R0);
    // CHECK limits
    double distlimit = 0.5;
    if (dist < distlimit) {
      cluster.AddHit(hit);
      if (fDisplayOn) {
        TMarker *mrk = new TMarker(hit->GetPosition().X(), hit->GetPosition().Y(), 21);
        mrk->SetMarkerColor(5);
        display->cd(1);
        mrk->Draw("SAME");
      }
    }
  }
  for (int ihit = 0; ihit < mvdstrhitlist->GetNofHits(); ihit++) {
    PndTrkHit *hit = mvdstrhitlist->GetHit(ihit);
    double dist = hit->GetXYDistanceFromTrack(xc0, yc0, R0);
    // CHECK limits
    double distlimit = 0.5;
    if (dist < distlimit) {
      cluster.AddHit(hit);
      if (fDisplayOn) {
        TMarker *mrk = new TMarker(hit->GetPosition().X(), hit->GetPosition().Y(), 25);
        mrk->SetMarkerColor(5);
        display->cd(1);
        mrk->Draw("SAME");
      }
    }
  }
  return cluster;
}

PndTrkCluster PndTrkLegendreSecTask2::CreateClusterByMixedDistance(double fitm, double fitq)
{

  PndTrkCluster cluster;
  double delta = conformalhitlist->GetConformalTransform()->GetRotation();
  // CHECK
  // ADD HIT IN MIXED MODE REAL P. FOR MVD & CONFORMAL P. FOR STT,
  // WITH DISTANCE CRITERION
  // real: fabs(R - sqrt((xc - x)**2 + (yc - y)**2)) < distlim
  // cofn: cost x + sint y - r = 0 -> |cost x0 + sint y0 - r|

  // CHECK if this needs to be kept --> change xc0 to xc etc
  // center and radius
  Double_t xc0, yc0, R0;
  FromConformalToRealTrack(fitm, fitq, xc0, yc0, R0);
  // .........................................................
  if (fDisplayOn) {
    TArc *arc = new TArc(xc0, yc0, R0);
    arc->SetFillStyle(0);
    arc->SetLineColor(5);
    display->cd(1);
    arc->Draw("SAME");
  }
  for (int ihit = 0; ihit < mvdpixhitlist->GetNofHits(); ihit++) {
    PndTrkHit *hit = mvdpixhitlist->GetHit(ihit);
    double dist = hit->GetXYDistanceFromTrack(xc0, yc0, R0);
    // CHECK limits
    double distlimit = 0.5;
    if (dist < distlimit) {
      cluster.AddHit(hit);
      if (fDisplayOn) {
        TMarker *mrk = new TMarker(hit->GetPosition().X(), hit->GetPosition().Y(), 21);
        mrk->SetMarkerColor(5);
        display->cd(1);
        mrk->Draw("SAME");
      }
    }
  }
  for (int ihit = 0; ihit < mvdstrhitlist->GetNofHits(); ihit++) {
    PndTrkHit *hit = mvdstrhitlist->GetHit(ihit);
    double dist = hit->GetXYDistanceFromTrack(xc0, yc0, R0);
    // CHECK limits
    double distlimit = 0.5;
    if (dist < distlimit) {
      cluster.AddHit(hit);
      if (fDisplayOn) {
        TMarker *mrk = new TMarker(hit->GetPosition().X(), hit->GetPosition().Y(), 25);
        mrk->SetMarkerColor(5);
        display->cd(1);
        mrk->Draw("SAME");
      }
    }
  }

  for (int ihit = 0; ihit < conformalhitlist->GetNofHits(); ihit++) {
    PndTrkConformalHit *chit = conformalhitlist->GetHit(ihit);
    double dist = chit->GetDistanceFromTrack(fitm, fitq);

    int hitID = chit->GetHitID();
    int detID = chit->GetDetectorID();

    // CHECK limits
    if (detID != FairRootManager::Instance()->GetBranchId(fSttBranch))
      continue;
    double distlimit = 0;
    if (chit->GetIsochrone() > 1.e-4)
      distlimit = 6 * chit->GetIsochrone();
    else
      distlimit = 0.001;
    // ---------------------------

    if (dist < distlimit) {

      //	cout << "ADDED " << hitID << " " << detID << " " << dist << " " << distlimit << endl;

      if (fDisplayOn) {
        TArc *arc = new TArc(chit->GetU(), chit->GetV(), chit->GetIsochrone());
        arc->SetFillStyle(0);
        arc->SetLineColor(5);
        display->cd(2);
        arc->Draw("SAME");
      }

      PndTrkHit *hit = stthitlist->GetHit(hitID);

      // ********** CHECK ***********
      // if you put "is used" the efficiency beecomes much lower!
      // DO NOT: 	if(!hit->IsUsed())
      cluster.AddHit(hit);
    }
  }
  return cluster;
}

Bool_t PndTrkLegendreSecTask2::DoesRealHitBelong(PndTrkHit *hit, double x0, double y0, double R)
{
  double dist = hit->GetXYDistanceFromTrack(x0, y0, R);
  int detID = hit->GetDetectorID();
  if (detID == FairRootManager::Instance()->GetBranchId(fMvdPixelBranch)) {
    if (dist < fMvdPix_RealDistLimit)
      return kTRUE;
  } else if (detID == FairRootManager::Instance()->GetBranchId(fMvdStripBranch)) {
    if (dist < fMvdStr_RealDistLimit)
      return kTRUE;
  } else if (detID == FairRootManager::Instance()->GetBranchId(fSttBranch)) {
    if (dist < fStt_RealDistLimit)
      return kTRUE;
  }
  return kFALSE;
}

Bool_t PndTrkLegendreSecTask2::DoesConfHitBelong(PndTrkConformalHit *chit, double fitm, double fitp)
{
  double dist = chit->GetDistanceFromTrack(fitm, fitp);
  int detID = chit->GetDetectorID();
  if (detID == FairRootManager::Instance()->GetBranchId(fMvdPixelBranch)) {
    if (dist < fMvdPix_ConfDistLimit)
      return kTRUE;
  } else if (detID == FairRootManager::Instance()->GetBranchId(fMvdStripBranch)) {
    if (dist < fMvdStr_ConfDistLimit)
      return kTRUE;
  } else if (detID == FairRootManager::Instance()->GetBranchId(fSttBranch)) {
    if (chit->GetIsochrone() > 1.e-4) {
      if (dist < (6 * chit->GetIsochrone()))
        return kTRUE;
    } else if (dist < fStt_ConfDistLimit)
      return kTRUE;
  }

  return kFALSE;
}

PndTrkCluster PndTrkLegendreSecTask2::CreateClusterByDistance(Int_t mode, double fitm, double fitp)
{
  PndTrkCluster cluster;

  // mode: -------------------------------------------
  // 0 all conformal: pix conf - str conf - stt conf
  // 1 all real:      pix real - str real - stt real
  // 2 mix:           pix real - str real - stt conf
  // --------------------------------------------------
  Double_t x0, y0, R;
  FromConformalToRealTrack(fitm, fitp, x0, y0, R);

  for (int ihit = 0; ihit < conformalhitlist->GetNofHits(); ihit++) {
    PndTrkConformalHit *chit = conformalhitlist->GetHit(ihit);
    PndTrkHit *hit = chit->GetHit();
    Bool_t accept = kFALSE;

    if (hit == fRefHit)
      accept = DoesRealHitBelong(hit, x0, y0, R);
    else {
      if (mode == 0)
        accept = DoesConfHitBelong(chit, fitm, fitp);
      else if (mode == 1)
        accept = DoesRealHitBelong(hit, x0, y0, R);
      else if (mode == 2) {
        if (hit->GetDetectorID() == FairRootManager::Instance()->GetBranchId(fSttBranch))
          accept = DoesConfHitBelong(chit, fitm, fitp);
        else
          accept = DoesRealHitBelong(hit, x0, y0, R);
      }
    }

    if (accept == kTRUE && fDisplayOn) {
      display->cd(1);
      if (hit->GetDetectorID() == FairRootManager::Instance()->GetBranchId(fSttBranch)) {
        TArc *arc = new TArc(hit->GetPosition().X(), hit->GetPosition().Y(), hit->GetIsochrone());
        arc->SetFillStyle(0);
        arc->SetLineColor(5);
        arc->Draw("SAME");
      } else {
        TMarker *mrk = nullptr;
        if (hit->GetDetectorID() == FairRootManager::Instance()->GetBranchId(fMvdPixelBranch))
          mrk = new TMarker(hit->GetPosition().X(), hit->GetPosition().Y(), 21);
        else if (hit->GetDetectorID() == FairRootManager::Instance()->GetBranchId(fMvdStripBranch))
          mrk = new TMarker(hit->GetPosition().X(), hit->GetPosition().Y(), 25);
        if (mrk != nullptr) {
          mrk->SetMarkerColor(5);
          mrk->Draw("SAME");
        }
      }
    }

    // ********** CHECK ***********
    // if you put "is used" the efficiency beecomes much lower!
    // DO NOT: 	if(!hit->IsUsed())
    if (accept == kTRUE) {
      //      cout << "ADDING TO CLUSTER" << endl;
      cluster.AddHit(hit);
    }
  }
  return cluster;
}

int PndTrkLegendreSecTask2::AddHitToClusterByDistance(PndTrkCluster *cluster, Int_t mode, double fitm, double fitp)
{

  // mode: -------------------------------------------
  // 0 all conformal: pix conf - str conf - stt conf
  // 1 all real:      pix real - str real - stt real
  // 2 mix:           pix real - str real - stt conf
  // --------------------------------------------------
  Double_t x0, y0, R;
  FromConformalToRealTrack(fitm, fitp, x0, y0, R);
  int addedcounter = 0;
  for (int ihit = 0; ihit < conformalhitlist->GetNofHits(); ihit++) {
    PndTrkConformalHit *chit = conformalhitlist->GetHit(ihit);
    PndTrkHit *hit = chit->GetHit();
    if (cluster->DoesContain(hit) == kTRUE)
      continue;
    Bool_t accept = kFALSE;

    if (hit == fRefHit)
      accept = DoesRealHitBelong(hit, x0, y0, R);
    else {
      if (mode == 0)
        accept = DoesConfHitBelong(chit, fitm, fitp);
      else if (mode == 1)
        accept = DoesRealHitBelong(hit, x0, y0, R);
      else if (mode == 2) {
        if (hit->GetDetectorID() == FairRootManager::Instance()->GetBranchId(fSttBranch))
          accept = DoesConfHitBelong(chit, fitm, fitp);
        else
          accept = DoesRealHitBelong(hit, x0, y0, R);
      }
    }

    //    cout << "@@@@@@@@@@@@@@ ACCEPT " << accept << endl;
    if (accept == kTRUE)
      accept = CheckVicinity(hit, cluster);

    if (accept == kTRUE && fDisplayOn) {
      display->cd(1);
      if (hit->GetDetectorID() == FairRootManager::Instance()->GetBranchId(fSttBranch)) {
        TArc *arc = new TArc(hit->GetPosition().X(), hit->GetPosition().Y(), hit->GetIsochrone());
        arc->SetFillStyle(0);
        arc->SetLineColor(5);
        arc->Draw("SAME");
      } else {
        TMarker *mrk = nullptr;
        if (hit->GetDetectorID() == FairRootManager::Instance()->GetBranchId(fMvdPixelBranch))
          mrk = new TMarker(hit->GetPosition().X(), hit->GetPosition().Y(), 21);
        else if (hit->GetDetectorID() == FairRootManager::Instance()->GetBranchId(fMvdStripBranch))
          mrk = new TMarker(hit->GetPosition().X(), hit->GetPosition().Y(), 25);
        if (mrk != nullptr) {
          mrk->SetMarkerColor(5);
          mrk->Draw("SAME");
        }
      }
    }

    // ********** CHECK ***********
    // if you put "is used" the efficiency beecomes much lower!
    // DO NOT: 	if(!hit->IsUsed())
    if (accept == kTRUE) {
      //      cout << "ADDING TO CLUSTER" << endl;
      addedcounter++;
      cluster->AddHit(hit);
    }
  }
  return addedcounter;
}

// CHECK change the function to compute distance: now it just computes
// the minimum hit-to-cluster distance but it could check:
// 1- for mvd-stt the distance of the mvd hit from the 1st layer stt hit
// 2- think of other solutions...
Bool_t PndTrkLegendreSecTask2::CheckVicinity(PndTrkHit *hit, PndTrkCluster *cluster)
{
  Double_t fMinimumDistanceLimit = 3;

  double distance = cluster->GetMinimumXYDistanceFromHit(hit);
  if (hit->IsMvdStrip())
    fMinimumDistanceLimit = 10;
  else if (hit->IsMvdPixel())
    fMinimumDistanceLimit = 3; // CHECK
  else if (hit->IsStt())
    fMinimumDistanceLimit = 2;

  //  cout << "GET MINIMUM DISTANCE " << distance << endl;

  if (distance < fMinimumDistanceLimit)
    return kTRUE;
  return kFALSE;
}

void PndTrkLegendreSecTask2::FromConformalToRealTrack(double fitm, double fitp, double &x0, double &y0, double &R)
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

void PndTrkLegendreSecTask2::DrawGeometry()
{
  if (hxy == nullptr)
    hxy = new TH2F("hxy", "xy plane", 100, -43, 43, 100, -43, 43);
  else
    hxy->Reset();
  display->cd(1);
  hxy->Draw();
  display->Update();
  display->Modified();
}

void PndTrkLegendreSecTask2::DrawZGeometry(int whichone, double phimin, double phimax, double zmin, double zmax)
{
  if (hxz == nullptr)
    hxz = new TH2F("hxz", "xz plane", 100, -43, 43, 100, -43, 113);
  else
    hxz->Reset();
  display->cd(3);
  hxz->Draw();
  if (whichone == 2) {
    if (hzphi == nullptr)
      hzphi = new TH2F("hzphi", "z - phi plane", phimax - phimin, phimin, phimax, zmax - zmin, zmin, zmax);
    else {
      hzphi->Reset();
      hzphi->GetXaxis()->SetLimits(phimin, phimax);
      hzphi->GetYaxis()->SetLimits(zmin, zmax);
    }
    display->cd(4);
    hzphi->Draw();
  }
  display->Update();
  display->Modified();
}

// ============================================================================================
// DISPLAY  ***********************************************************************************
// ============================================================================================

void PndTrkLegendreSecTask2::Refresh()
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

void PndTrkLegendreSecTask2::DrawHits(PndTrkHitList *hitlist)
{
  display->cd(1);
  hitlist->Draw();
  display->Update();
  display->Modified();
}
void PndTrkLegendreSecTask2::DrawGeometryConf(double x1, double y1, double x2, double y2)
{
  // CHECK
  char goOnChar;
  //  cout << "DRAWING GEOMETRY CONF" << endl;
  // cin >> goOnChar;

  // CHECK previous calculations, now not used;
  if (huv == nullptr)
    huv = new TH2F("huv", "uv plane", 100, x1, y1, 100, x2, y2);
  else {
    huv->Reset();
    huv->GetXaxis()->SetLimits(x1, y1);
    huv->GetYaxis()->SetLimits(x2, y2);
  }
  display->cd(2);
  huv->Draw();
  display->Update();
  display->Modified();
}

void PndTrkLegendreSecTask2::DrawLegendreHisto()
{
  display->cd(3);
  legendre->Draw();
  display->Update();
  display->Modified();
}

void PndTrkLegendreSecTask2::RefreshConf()
{ // CHECK delete
  // CHECK
  char goOnChar;
  //  cout << "RefreshConf?" << endl;
  //  cin >> goOnChar;
  //  cout << "REFRESHING CONF" << endl;
}

void PndTrkLegendreSecTask2::RefreshZ()
{ // CHECK delete
  // CHECK
  char goOnChar;
  // cout << "RefreshZ?" << endl;
  //  cin >> goOnChar;
  // cout << "REFRESHING Z" << endl;
}

void PndTrkLegendreSecTask2::DrawConfHit(double u, double v, double r, int marker)
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

PndTrkCluster PndTrkLegendreSecTask2::CreateSttCluster(PndTrkHit *firsthit)
{

  /**  THE PROCEDURE
       =============

       2) LOOP (int hit2) on all the hits
       a) consider only the ones in region iregion

       2) LOOP (int hit3) on all the hits IN THE CLUSTER
       a) IF the hit2 you are considering in the region IS the one in the cluster (hit3), continue.
       ELSE try to associate it to one of the hits in the cluster (IsSttAssociate)
       b) if IsSttAssociate is true, add the hit2 to the cluster too.

  **/

  PndTrkCluster cluster;
  cluster.AddHit(firsthit);
  cluster.SetIRegion(firsthit->GetIRegion());

  // 2) LOOP  on all the hits
  for (int ihit = 0; ihit < stthitlist->GetNofHits(); ihit++) {
    PndTrkHit *hit = stthitlist->GetHit(ihit);

    if (hit->IsUsed()) {
      //  cout << "already used IV" << endl;
      continue;
    }
    if (!hit->IsRegion(firsthit->GetIRegion()))
      continue;
    //    cout << ihit << " " << cluster.GetNofHits() << endl;
    // 3) LOOP (int hit3) on all the hits IN THE CLUSTER
    for (int jhit = 0; jhit < cluster.GetNofHits(); jhit++) {
      PndTrkHit *clushit = cluster.GetHit(jhit);

      if (*hit == *clushit) {
        continue;
      }

      Bool_t success = IsSttAssociate(hit, clushit);
      //       cout << "success " << success << endl;
      if (!success)
        continue;

      cluster.AddHit(hit);
      break;
    }
  }

  if (cluster.GetNofHits() < 2)
    for (int ihit = 0; ihit < cluster.GetNofHits(); ihit++)
      cluster.DeleteAllHits();
  return cluster;
}

Bool_t PndTrkLegendreSecTask2::IsSttAssociate(PndTrkHit *hit1, PndTrkHit *hit2)
{

  double distance = hit1->GetXYDistance(hit2); // ComputeDistance(hit1, hit2);

  if (distance < STTPARALDISTANCE) {
    //  cout << "distance " << distance <<  endl;
    return kTRUE;
  }
  return kFALSE;
}

void PndTrkLegendreSecTask2::LightCluster(PndTrkCluster *cluster)
{

  //  cout << "LIGHT UP" << endl;
  Refresh();
  cluster->LightUp();
  display->Update();
  display->Modified();
}

// ================================== ZFINDER
PndTrkCluster PndTrkLegendreSecTask2::CreateSkewHitList(PndTrkTrack *track)
{

  double xc = track->GetCenter().X();
  double yc = track->GetCenter().Y();
  double R = track->GetRadius();
  PndTrkCluster skewhitlist;

  double phimin = 400, phimax = -1, zmin = 1000, zmax = -1;
  for (int ihit = 0; ihit < stthitlist->GetNofHits(); ihit++) {
    PndTrkHit *hit = stthitlist->GetHit(ihit);
    //  if(hit->IsUsed()) {
    //  cout << "already used IV" << endl;
    //  continue;
    //  }
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
    double m1 = (first - second).Y() / (first - second).X();
    double q1 = first.Y() - m1 * first.X();

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
      char goOnChar;
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
      char goOnChar;
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
    if (fDisplayOn) {
      char goOnChar;
      display->cd(3);
      //	cout << "COMPUTE Z COORDINATE" << endl;
      RefreshZ();
      DrawZGeometry();
      TLine *linezx = new TLine(first.X(), first.Z(), second.X(), second.Z());
      linezx->Draw("SAME");
      TMarker *mrkza = new TMarker(x0a, z0a, 20);
      mrkza->SetMarkerColor(4);
      mrkza->Draw("SAME");
      TMarker *mrkzb = new TMarker(x0b, z0b, 20);
      mrkzb->SetMarkerColor(6);
      mrkzb->Draw("SAME");
      //	 cin >> goOnChar;
    }

    // calculate the z of the intersection ON the ellipse (CHECK this step calculations!)
    double dx = intxa - x0a;
    double dy = intya - y0a;
    TVector3 dxdy(dx, dy, 0.0);

    TVector3 tfirst = first + dxdy;
    TVector3 tsecond = second + dxdy;

    t = ((intxa + intya) - (tfirst.X() + tfirst.Y())) / ((tsecond.X() - tfirst.X()) + (tsecond.Y() - tfirst.Y()));
    double intza = tfirst.Z() + (tsecond.Z() - tfirst.Z()) * t;
    if (fDisplayOn) {
      char goOnChar;
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

    if (fDisplayOn) {
      char goOnChar;
      display->cd(3);
      TLine *linezx2 = new TLine(tfirst.X(), tfirst.Z(), tsecond.X(), tsecond.Z());
      linezx2->SetLineStyle(1);
      linezx2->Draw("SAME");
      TMarker *mrkzb1 = new TMarker(intxb, intzb, 20);
      mrkzb1->SetMarkerColor(kMagenta - 7);
      mrkzb1->Draw("SAME");
      //	 cin >> goOnChar;
    }
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

PndTrkCluster PndTrkLegendreSecTask2::CleanUpSkewHitList(PndTrkCluster *skewhitlist)
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
    char goOnChar;
    display->cd(4);
    phimin -= 30;
    phimax += 30;
    zmin -= 13;
    zmax += 13;
    DrawZGeometry(2, phimin, phimax, zmin, zmax);
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

void PndTrkLegendreSecTask2::RegisterTrack(PndTrkTrack *track)
{
  PndTrack *finaltrack = track->ConvertToPndTrack();

  TClonesArray &clref1 = *fTrackArray;
  Int_t size = clref1.GetEntriesFast();
  PndTrack *outputtrack = new (clref1[size]) PndTrack(finaltrack->GetParamFirst(), finaltrack->GetParamLast(), finaltrack->GetTrackCand());

  TClonesArray &clref2 = *fTrackCandArray;
  size = clref2.GetEntriesFast();
  PndTrackCand *outputtrackcand = new (clref2[size]) PndTrackCand(finaltrack->GetTrackCand());

  if (fVerbose > 1) {
    cout << "MOM FIRST: TOT, PT, PL " << outputtrack->GetParamFirst().GetMomentum().Mag() << " " << outputtrack->GetParamFirst().GetMomentum().Perp() << " "
         << outputtrack->GetParamFirst().GetMomentum().Z() << endl;
    cout << "MOM LAST: TOT, PT, PL " << outputtrack->GetParamLast().GetMomentum().Mag() << " " << outputtrack->GetParamLast().GetMomentum().Perp() << " "
         << outputtrack->GetParamLast().GetMomentum().Z() << endl;
  }
  if (fDisplayOn) {
    char goOnChar;
    display->cd(1);
    Refresh();

    track->Draw(kRed);
    display->Update();
    display->Modified();

    cout << "X, Y, R " << track->GetCenter().X() << " " << track->GetCenter().Y() << " " << track->GetRadius() << endl;
    cout << "Z0, TANL " << track->GetZ0() << " " << track->GetTanL() << endl;
    cout << "CHARGE " << track->GetCharge() << endl;
    //      cin >> goOnChar;
  }
}

// ----------------------- FOR SECONDARIES
PndTrkHit *PndTrkLegendreSecTask2::FindSttReferenceHit(int isec)
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

PndTrkHit *PndTrkLegendreSecTask2::FindMvdPixelReferenceHit()
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

PndTrkHit *PndTrkLegendreSecTask2::FindMvdStripReferenceHit()
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

PndTrkHit *PndTrkLegendreSecTask2::FindMvdReferenceHit()
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

PndTrkHit *PndTrkLegendreSecTask2::FindReferenceHit()
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

PndTrkHit *PndTrkLegendreSecTask2::FindReferenceHit(PndTrkCluster *cluster)
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

void PndTrkLegendreSecTask2::ComputeTraAndRot(PndTrkHit *hit, Double_t &delta, Double_t trasl[2])
{

  trasl[0] = hit->GetPosition().X();
  trasl[1] = hit->GetPosition().Y();

  delta = 0.; // TMath::ATan2(hit->GetPosition().Y() - 0., hit->GetPosition().X() - 0.); // CHECK
}

void PndTrkLegendreSecTask2::RePrepareLegendre(PndTrkCluster *cluster)
{

  //    cout << "RESETTING LEGENDRE HISTO" << endl;
  legendre->ResetLegendreHisto();

  if (fDisplayOn) {
    RefreshConf();
    if (fSecondary)
      DrawGeometryConf(-1., 1., -1., 1.);
    else
      DrawGeometryConf(-0.07, 0.07, -0.07, 0.07);
  }
  // cout << "%%%%%%%%%%%%%%%%%%%% XY FINDE %%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
  FillLegendreHisto(cluster);
}

void PndTrkLegendreSecTask2::PrepareLegendre()
{

  //    cout << "RESETTING LEGENDRE HISTO" << endl;
  legendre->ResetLegendreHisto();

  if (fDisplayOn) {
    RefreshConf();
    if (fSecondary)
      DrawGeometryConf(-1., 1., -1., 1.);
    else
      DrawGeometryConf(-0.07, 0.07, -0.07, 0.07);
  }
  if (fVerbose > 1)
    cout << "%%%%%%%%%%%%%%%%%%%% XY FINDER %%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
  FillLegendreHisto(0);
}

Int_t PndTrkLegendreSecTask2::ApplyLegendre(double &theta_max, double &r_max)
{
  PrepareLegendre();
  return ExtractLegendre(0, theta_max, r_max);
}

Int_t PndTrkLegendreSecTask2::ApplyLegendre(PndTrkCluster *cluster, double &theta_max, double &r_max)
{
  RePrepareLegendre(cluster);
  return ExtractLegendre(1, theta_max, r_max);
}

Int_t PndTrkLegendreSecTask2::ExtractLegendre(Int_t mode, double &theta_max, double &r_max)
{
  if (fDisplayOn) {
    char goOnChar;
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
      //      if(fVerbose > 1)
      cout << "MAXPEAK " << maxpeak << ", BREAK NOW! " << endl;
      return maxpeak;
    }

    for (int ialready = 0; ialready < fFoundPeaks.size(); ialready++) {
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

  for (int ihit = 0; ihit < conformalhitlist->GetNofHits(); ihit++) {
    PndTrkConformalHit *chit = conformalhitlist->GetHit(ihit);
    legendre->FillZoomHisto(chit->GetU(), chit->GetV(), chit->GetIsochrone());
  }

  if (mode == 0 && alreadythere == true) {
    cout << "THIS PEAK IS ALREADY THERE" << endl;
    legendre->DeleteZoneAroundXYZoom(theta_max, r_max);
  }

  int maxpeakzoom = legendre->ExtractZoomMaximum(theta_max, r_max);
  //  cout << "THETA/R ZOOM " << theta_max << " " << r_max <<  " maxpeakzoom " << maxpeakzoom << endl;

  if (fDisplayOn) {
    char goOnChar;
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

PndTrkCluster PndTrkLegendreSecTask2::Cleanup(PndTrkCluster cluster)
{

  // CLEANUP
  // -------------------------------------------------------
  //    CleanCluster(cluster);
  // cluster->SortMvdPixelByLayerID();
  //    cluster->SortByLayerID();
  PndTrkClean clean;
  int leftvotes = 0;
  int rightvotes = 0;

  TVector2 trasl = conform->GetTranslation();
  Double_t angle = conform->GetRotation();

  // SET SORT VARIABLE
  // MVD PIX PART OF CLUSTER
  PndTrkCluster mvdpixcluster = cluster.GetMvdPixelHitList();
  for (int ihit = 0; ihit < mvdpixcluster.GetNofHits(); ihit++) {
    PndTrkHit *hit = mvdpixcluster.GetHit(ihit);
    if (fSecondary)
      hit->SetSortVariable(hit->GetDistance(TVector3(trasl.X(), trasl.Y(), angle)));
    else
      hit->SetSortVariable(hit->GetDistance(TVector3(0., 0., 0)));
    int sensorID = hit->GetSensorID();
    int layerID = clean.FindMvdLayer(sensorID);
    if (layerID % 2 == 0)
      rightvotes++;
    else
      leftvotes++;
  }
  // MVD STR PART OF CLUSTER
  PndTrkCluster mvdstrcluster = cluster.GetMvdStripHitList();
  for (int ihit = 0; ihit < mvdstrcluster.GetNofHits(); ihit++) {
    PndTrkHit *hit = mvdstrcluster.GetHit(ihit);
    if (fSecondary)
      hit->SetSortVariable(hit->GetDistance(TVector3(trasl.X(), trasl.Y(), angle)));
    else
      hit->SetSortVariable(hit->GetDistance(TVector3(0., 0., 0)));
    int sensorID = hit->GetSensorID();
    int layerID = clean.FindMvdLayer(sensorID);
    if (layerID % 2 == 0)
      rightvotes++;
    else
      leftvotes++;
  }
  // STT PARALLEL PART OF CLUSTER
  PndTrkCluster sttcluster = cluster.GetSttParallelHitList();
  for (int ihit = 0; ihit < sttcluster.GetNofHits(); ihit++) {
    PndTrkHit *hit = sttcluster.GetHit(ihit);
    int layerID = -999, tubeID = hit->GetTubeID(), sectorID = -999;
    PndSttTube *tube = (PndSttTube *)fTubeArray->At(tubeID);
    layerID = tube->GetLayerID();
    sectorID = tube->GetSectorID();
    if (fSecondary)
      hit->SetSortVariable(hit->GetDistance(TVector3(trasl.X(), trasl.Y(), angle)));
    else
      hit->SetSortVariable(1000 + layerID);
    if (sectorID < 3)
      rightvotes++;
    else
      leftvotes++;
  }

  // cout << "VOTES: LEFT " << leftvotes << " RIGHT " << rightvotes << endl;
  for (int ihit = 0; ihit < cluster.GetNofHits(); ihit++) {
    PndTrkHit *hit = cluster.GetHit(ihit);
    //  cout << "for sorting " << hit->GetDetectorID () << " " << hit->GetHitID() << " " << hit->GetSortVariable() << endl;
  }

  cluster.Sort();
  for (int ihit = 0; ihit < cluster.GetNofHits(); ihit++) {
    PndTrkHit *hit = cluster.GetHit(ihit);
    // cout << "sorted " << hit->GetDetectorID () << " " << hit->GetHitID() << " " << hit->GetSortVariable() << endl;
  }

  //    cluster.Print();

  // DELETE HITS
  PndTrkCluster deletioncluster;

  // LEFT/RIGHT CRITERION ------------------------------------------ NOW OFF
  // PIXEL
  mvdpixcluster = cluster.GetMvdPixelHitList();
  for (int ihit = 0; ihit < mvdpixcluster.GetNofHits(); ihit++) {
    PndTrkHit *hit = mvdpixcluster.GetHit(ihit);
    int sensorID = hit->GetSensorID();
    int layerID = clean.FindMvdLayer(sensorID);
    //  cout << "pixel sorted " <<  hit->GetHitID() << " " << hit->GetSortVariable() << " " << sensorID << " " << layerID << endl;

    if ((leftvotes > rightvotes) && (layerID % 2 == 0)) {
      // deletioncluster.AddHit(hit);
      // cout << "pixel DELETED " <<  hit->GetHitID() << " " << ihit << " " << mvdpixcluster.GetNofHits() << endl;
    } else if ((leftvotes < rightvotes) && (layerID % 2 != 0)) {
      // deletioncluster.AddHit(hit);
      // cout << "pixel DELETED " << hit->GetHitID() << " " << ihit << " " << mvdpixcluster.GetNofHits() << endl;
    }

    if (fDisplayOn) {
      char goOnChar;
      display->cd(1);
      hit->Draw(kGreen);
      display->Update();
      display->Modified();
      cout << "WAITING" << endl;
      cin >> goOnChar;
    }
  }
  // STRIP
  mvdstrcluster = cluster.GetMvdStripHitList();
  for (int ihit = 0; ihit < mvdstrcluster.GetNofHits(); ihit++) {
    PndTrkHit *hit = mvdstrcluster.GetHit(ihit);
    int sensorID = hit->GetSensorID();
    //  cout << "strip sorted " <<  hit->GetHitID() << " " << hit->GetSortVariable() << " " << sensorID << " " << clean.FindMvdLayer(sensorID) << endl;
    int layerID = clean.FindMvdLayer(sensorID);
    if ((leftvotes > rightvotes) && (layerID % 2 == 0)) {
      //	deletioncluster.AddHit(hit);
      //	cout << "strip DELETED " <<  hit->GetHitID() << endl;
    } else if ((leftvotes < rightvotes) && (layerID % 2 != 0)) {
      //	deletioncluster.AddHit(hit);
      //	cout << "strip DELETED " <<  hit->GetHitID() << endl;
    }
    if (fDisplayOn) {
      char goOnChar;
      display->cd(1);
      hit->Draw(kBlue);
      display->Update();
      display->Modified();
      //	cin >> goOnChar;
    }
  }

  // STT PARALLEL
  sttcluster = cluster.GetSttParallelHitList();
  for (int ihit = 0; ihit < sttcluster.GetNofHits(); ihit++) {
    PndTrkHit *hit = sttcluster.GetHit(ihit);
    int layerID = -999, tubeID = hit->GetTubeID();
    PndSttTube *tube = (PndSttTube *)fTubeArray->At(tubeID);
    layerID = tube->GetLayerID();
    //      cout << "stt sorted " <<  hit->GetHitID() << " " << hit->GetSortVariable() << " " << tubeID << " " << layerID << endl;

    int sectorID = tube->GetSectorID();

    if ((leftvotes > rightvotes) && (sectorID < 3)) {
      //		deletioncluster.AddHit(hit);
      //	cout << "stt DELETED " <<  hit->GetHitID() << " " << sectorID << endl;
    } else if ((leftvotes < rightvotes) && (sectorID >= 3)) {
      // 		deletioncluster.AddHit(hit);
      //	cout << "stt DELETED " <<  hit->GetHitID() << " " << sectorID << endl;
    }
    // ~~~~~~~~~~~~~~~~~~ check *******************
    else { // NEIGHBORING CRITERION ------------------------------------------------
      TArrayI neighboring = tube->GetNeighborings();
      //	cout << "NEIGHBORING TO TUBE " << tubeID << endl;
      // 	for(int nhit = 0; nhit < neighboring.GetSize(); nhit++) {
      // 	  cout << " " << neighboring.At(nhit);
      // 	}
      // 	cout << endl;

      int neighboringcounter = 0;
      for (int ohit = 0; ohit < sttcluster.GetNofHits(); ohit++) {
        if (ihit == ohit)
          continue;
        PndTrkHit *otherhit = sttcluster.GetHit(ohit);
        int otherhitID = otherhit->GetTubeID();

        for (int nhit = 0; nhit < neighboring.GetSize(); nhit++) {
          if (otherhitID == neighboring.At(nhit))
            neighboringcounter++;
        }

        // 	if(isthere == false)
        // 	  {
        // 	    deletioncluster.AddHit(hit);
        // 	    cout << "stt DELETEDbis " <<  hit->GetHitID() << " " << sectorID << endl;
        // 	  }
      }
      //	cout << "hit " << ihit << " has " << neighboringcounter << " NEIGHBORINS" << endl;

      if (layerID != 0 && layerID != 7 && layerID != 8 && ihit != sttcluster.GetNofHits() - 1 && neighboringcounter == 1) {
        //	  cout << "I SHOULD DELETE THIS" << endl;
        //  deletioncluster.AddHit(hit);
        // OFF FOR NOW: delete if there is just 1 neighboring
        //	  cout << "stt DELETED " <<  hit->GetHitID() << " " << sectorID << endl;
      }

      // ON, delete if there is no neighboring (isolated hit)
      if (neighboringcounter == 0) {
        //	  cout << "I WILL DELETE THIS" << endl;
        deletioncluster.AddHit(hit);
        //	  cout << "stt DELETED " <<  hit->GetHitID() << " " << sectorID << endl;
      }

      if (fDisplayOn) {
        char goOnChar;
        display->cd(1);
        hit->Draw(kCyan);
        display->Update();
        display->Modified();
        //	  cin >> goOnChar;
      }
    }
  }

  if (fDisplayOn) {
    char goOnChar;
    //      cin >> goOnChar;
  }
  // ~~~~~~~~~~~~~~~~~~

  // ACTUAL DELETION OF HITS

  PndTrkCluster finalcluster;
  for (int ihit = 0; ihit < cluster.GetNofHits(); ihit++) {
    PndTrkHit *hit = cluster.GetHit(ihit);
    bool stop = false;
    for (int jhit = 0; jhit < deletioncluster.GetNofHits(); jhit++) {
      PndTrkHit *dhit = deletioncluster.GetHit(jhit);
      if (hit == dhit) {
        // ********** CHECK ***********
        //	  cout << "trhrow in again" << endl;
        //	  dhit->SetUsedFlag(0);
        stop = true;
        break;
      }
    }
    if (!stop)
      finalcluster.AddHit(hit);
  }

  return finalcluster;
}

PndTrkCluster *PndTrkLegendreSecTask2::CleanupZPhiFit(PndTrkCluster *cluster, double fitm, double fitp)
{

  PndTrkCluster *cleancluster = new PndTrkCluster();
  for (int ihit = 0; ihit < cluster->GetNofHits(); ihit++) {
    PndTrkHit *hit = cluster->GetHit(ihit);
    if (hit->IsSttParallel()) {
      cleancluster->AddHit(hit);
      continue; // CHECK IsSttParrallel will be changed
    }

    TVector3 position = hit->GetPosition();
    double phi = hit->GetPhi();

    // d = | m x - y + p | / sqrt(m**2 + 1)
    double distance = fabs(fitm * phi - position.Z() + fitp) / sqrt(fitm * fitm + 1);
    //    cout << "distance " << hit->GetHitID() << " " << distance << endl;

    if (hit->GetDetectorID() == FairRootManager::Instance()->GetBranchId(fMvdPixelBranch) || hit->GetDetectorID() == FairRootManager::Instance()->GetBranchId(fMvdStripBranch)) {
      if (distance < 1)
        cleancluster->AddHit(hit);
    } else if (hit->GetDetectorID() == FairRootManager::Instance()->GetBranchId(fSttBranch)) {
      if (distance < 5)
        cleancluster->AddHit(hit);
    }
  }
  //  cout << "cleancluster " << cleancluster->GetNofHits () << endl;
  return cleancluster;
}

Bool_t PndTrkLegendreSecTask2::ZPhiFit(int iter, PndTrkCluster *cluster, double &fitm, double &fitp)
{

  //  cout << "ZPHIFIT " << iter << endl;

  if (iter != 0) {
    if (fDisplayOn) {
      char goOnChar;
      display->cd(3);
      DrawZGeometry();
      hzphi->SetXTitle("#phi");
      hzphi->SetYTitle("#z");
      hzphi->Draw();
      display->Update();
      display->Modified();
    }
  }

  fFitter->Reset();
  PndTrkHit *hit = nullptr;
  PndTrkHit *refhit = nullptr;
  double refiso = 1000;
  for (int ihit = 0; ihit < cluster->GetNofHits(); ihit++) {
    hit = cluster->GetHit(ihit);
    if (hit->IsSttParallel() == kTRUE)
      continue; // CHECK IsSttParrallel will be changed

    TVector3 position = hit->GetPosition();
    double phi = hit->GetPhi();
    //    fFitter->SetPointToFit(phi, position.Z(), 0.1);

    // CHECK refhit stuff
    if (hit->GetDetectorID() == FairRootManager::Instance()->GetBranchId(fMvdPixelBranch) || hit->GetDetectorID() == FairRootManager::Instance()->GetBranchId(fMvdStripBranch)) {
      if (iter != 0)
        refhit = hit;
      fFitter->SetPointToFit(phi, position.Z(), 0.001);
      //  cout << ihit << " " << hit->GetDetectorID() << " " << hit->GetHitID() << " " << hit->IsSttParallel() << " SetPointToFit " << phi << " " << position.Z() << " " << endl;
    } else if (hit->GetDetectorID() == FairRootManager::Instance()->GetBranchId(fSttBranch)) {
      if (iter != 0 && refhit == nullptr && hit->GetIsochrone() < refiso)
        refhit = hit;
      fFitter->SetPointToFit(phi, position.Z(), 0.1);
      //  cout << ihit << " " << hit->GetDetectorID() << " " << hit->GetHitID() << " " << hit->IsSttParallel() << " SetPointToFit " << phi << " " << position.Z() << " " << endl;
    }

    if (fDisplayOn) {
      char goOnChar;
      if (iter == 0)
        display->cd(4);
      TMarker *mrkfoundzphi = nullptr;
      if (hit->GetDetectorID() == FairRootManager::Instance()->GetBranchId(fMvdPixelBranch) || hit->GetDetectorID() == FairRootManager::Instance()->GetBranchId(fMvdStripBranch)) {
        mrkfoundzphi = new TMarker(phi, position.Z(), 21);
        if (iter == 0)
          mrkfoundzphi->SetMarkerColor(kOrange);
        else
          mrkfoundzphi->SetMarkerColor(4);
      } else if (hit->GetDetectorID() == FairRootManager::Instance()->GetBranchId(fSttBranch)) {
        mrkfoundzphi = new TMarker(phi, position.Z(), 20);
        if (iter == 0)
          mrkfoundzphi->SetMarkerColor(kGreen);
        else
          mrkfoundzphi->SetMarkerColor(4);
      }
      mrkfoundzphi->Draw("SAME");
      //      display->cd(3);
      //      mrkfoundzphi->Draw("SAME");
    }
  }
  // ===================================
  bool fit = kFALSE;
  if (iter == 0)
    fit = fFitter->StraightLineFit(fitm, fitp);
  else {
    if (refhit != nullptr)
      fit = fFitter->ConstrainedStraightLineFit(refhit->GetPhi(), refhit->GetPosition().Z(), fitm, fitp);
    else
      fit = fFitter->StraightLineFit(fitm, fitp);
  }

  if (fit) {
    if (fDisplayOn) {
      char goOnChar;
      if (iter != 0)
        display->cd(4);
      TLine *l22 = new TLine(-1000, -1000 * fitm + fitp, 1000, 1000 * fitm + fitp);
      if (iter == 0)
        l22->SetLineColor(3);
      else if (iter == 1)
        l22->SetLineColor(4);
      l22->Draw("SAME");
      display->cd(3);
      l22->Draw("SAME");
      display->Update();
      display->Modified();
      cin >> goOnChar;
    }
  }
  // ++++++++++++++++++++++++++++++++++

  return fit;
}

double PndTrkLegendreSecTask2::ComputeZRediduals(PndTrkCluster *cluster, double fitm, double fitp)
{

  TH1F hresiduals("hresiduals", "residuals in z", 20, -5, 5);

  for (int ihit = 0; ihit < cluster->GetNofHits(); ihit++) {
    PndTrkHit *hit = cluster->GetHit(ihit);

    if (hit->GetDetectorID() != FairRootManager::Instance()->GetBranchId(fSttBranch))
      continue;
    if (hit->IsSttParallel())
      continue;

    TVector3 position = hit->GetPosition();
    double phi = hit->GetPhi();

    double distance = fitm * phi + fitp - position.Z();

    hresiduals.Fill(distance);
  }

  if (fDisplayOn) {
    display->cd(4);
    char goOnChar;
    cout << "residuals";
    cin >> goOnChar;
    hresiduals.Draw();
    display->Update();
    display->Modified();
    cin >> goOnChar;
  }
  return hresiduals.GetMean();
}

double PndTrkLegendreSecTask2::CorrectZ(PndTrkCluster *cluster, double deltaz, double fitm, double fitp)
{

  for (int ihit = 0; ihit < cluster->GetNofHits(); ihit++) {
    PndTrkHit *hit = cluster->GetHit(ihit);

    if (hit->GetDetectorID() != FairRootManager::Instance()->GetBranchId(fSttBranch))
      continue;
    if (hit->IsSttParallel())
      continue;

    TVector3 position = hit->GetPosition();
    double newz = position.Z() - deltaz;

    int tubeID = hit->GetTubeID();
    PndSttTube *tube = (PndSttTube *)fTubeArray->At(tubeID);

    TVector3 wireDirection = tube->GetWireDirection();
    Double_t halflength = tube->GetHalfLength();

    TVector3 first = tube->GetPosition() + wireDirection * halflength;  // CHECK
    TVector3 second = tube->GetPosition() - wireDirection * halflength; // CHECK

    double m, p;
    double newx = position.X();
    double newy = position.Y();
    if ((second.X() - first.X()) != 0) {
      m = (second.Z() - first.Z()) / (second.X() - first.X());
      p = position.Z() - m * position.X();
      newx = (newz - p) / m;
    }

    if ((second.Y() - first.Y()) != 0) {

      m = (second.Z() - first.Z()) / (second.Y() - first.Y());
      p = position.Z() - m * position.Y();
      newy = (newz - p) / m;
    }

    hit->SetPosition(TVector3(newx, newy, newz));

    if (fDisplayOn) {
      display->cd(1);
      char goOnChar;
      cout << endl;
      cout << "SEE IT";
      TMarker *mrk = new TMarker(newx, newy, 6);
      mrk->SetMarkerColor(kOrange);
      mrk->Draw("SAME");
      display->Update();
      display->Modified();

      cout << "old " << position.X() << " " << position.Y() << " " << position.Z() << endl;
      cout << "new " << newx << " " << newy << " " << newz << endl;

      cin >> goOnChar;
    }
  }
}
ClassImp(PndTrkLegendreSecTask2)
