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

//* $Id: */

// -------------------------------------------------------------------------
// -----                    PndBarrelTrackFinder source file           -----
// -----                  Created 05/12/2010 by R. Karabowicz          -----
// -------------------------------------------------------------------------

// Includes from GEM
#include "PndBarrelTrackFinder.h"

// Includes from base
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLink.h"
#include "FairField.h"

// Includes from ROOT
#include "TClonesArray.h"
#include "TObjArray.h"
#include "TMath.h"
#include "TGeoManager.h"
#include "TGeoNode.h"
#include "TRandom.h"

#include "FairTrackParam.h"
#include "FairHit.h"
#include "FairLogger.h"
#include "PndTrack.h"
#include "PndTrackCand.h"
#include "PndTrackCandHit.h"

#include "PndDetectorList.h"
#include "PndGemHit.h"
#include "PndSdsHit.h"
#include "PndSttHit.h"
#include "PndSttMapCreator.h"
#include "PndSttTube.h"
#include "PndSttTubeMap.h"

// STT2 stuff
#include "PndStt2GeoHandler.h"

#include <iostream>
#include <iomanip>
#include <map>

#include "TCanvas.h"  // DRAW
#include "TMarker.h"  // DRAW
#include "TEllipse.h" // DRAW
#include "TSystem.h"  // DRAW
#include "TLine.h"    // DRAW
#include "TLatex.h"   // DRAW
#include "TH1F.h"     // DRAW

using std::cerr;
using std::cout;
using std::endl;
using std::fixed;
using std::flush;
using std::left;
using std::map;
using std::pair;
using std::right;
using std::setprecision;
using std::setw;

// -----   Default constructor   ------------------------------------------
PndBarrelTrackFinder::PndBarrelTrackFinder() : PndPersistencyTask("Barrel Track Finder", 1)//, fGeoType(0)
{
  for (Int_t idet = 0; idet < 4; idet++) {
    fIncludeDet[idet] = kFALSE;
    fHitArray[idet] = nullptr;
    fDetName[idet] = "";
    fDetType[idet] = 0;
    fHitArrayName[idet] = "";
  }
  fBarrelTrackArray = nullptr;
  fBarrelTrackCandArray = nullptr;

  fTNofEvents = 0;
  fTNofTracks = 0;
  fTracksVector.clear();

  fHitDetId.clear();
  fHitDetNo.clear();
  fHitVector.clear();
  fHitVectDI.clear();
  fHitVectHN.clear();

  fRadToMomConversion = 1.0;

  fMaximalDist = 0.01;
  fReasonableDist = 0.2;

  fMaximalZ_PhiD = 2.;
  fReasonableZ_PhiD = 5.;

  fMaximalRadDiff = 0.1;
  fMaximalPhiDiff = 0.1;

  fTubeArray = nullptr;
  fSttParameters = nullptr;

  fDrawDetails = kFALSE;
  fDrawHistos = kFALSE;

  fStopTimeValue = 0.0;
  fFunctor = new StopTime();

  fRunTimeBased = kFALSE;

  Reset();
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndBarrelTrackFinder::PndBarrelTrackFinder(Int_t iVerbose) : PndPersistencyTask("Barrel Track Finder", iVerbose)
{
  for (Int_t idet = 0; idet < 4; idet++) {
    fIncludeDet[idet] = kFALSE;
    fHitArray[idet] = nullptr;
    fDetName[idet] = "";
    fDetType[idet] = 0;
    fHitArrayName[idet] = "";
  }
  fBarrelTrackArray = nullptr;
  fBarrelTrackCandArray = nullptr;

  fTNofEvents = 0;
  fTNofTracks = 0;
  fTracksVector.clear();

  fHitDetId.clear();
  fHitDetNo.clear();
  fHitVector.clear();
  fHitVectDI.clear();
  fHitVectHN.clear();

  fMaximalDist = 0.01;
  fReasonableDist = 0.2;

  fMaximalZ_PhiD = 2.;
  fReasonableZ_PhiD = 5.;

  fMaximalRadDiff = 0.1;
  fMaximalPhiDiff = 0.1;

  fTubeArray = nullptr;
  fSttParameters = nullptr;

  fStopTimeValue = 0.0;
  fFunctor = new StopTime();

  fRunTimeBased = kFALSE;

  Reset();
}
// -------------------------------------------------------------------------

// -----   Constructor with name   -----------------------------------------
PndBarrelTrackFinder::PndBarrelTrackFinder(const char *name, Int_t iVerbose) : PndPersistencyTask(name, iVerbose)
{
  for (Int_t idet = 0; idet < 4; idet++) {
    fIncludeDet[idet] = kFALSE;
    fHitArray[idet] = nullptr;
    fDetName[idet] = "";
    fDetType[idet] = 0;
    fHitArrayName[idet] = "";
  }
  fBarrelTrackArray = nullptr;
  fBarrelTrackCandArray = nullptr;

  fTNofEvents = 0;
  fTNofTracks = 0;
  fTracksVector.clear();

  fHitDetId.clear();
  fHitDetNo.clear();
  fHitVector.clear();
  fHitVectDI.clear();
  fHitVectHN.clear();

  fMaximalDist = 0.01;
  fReasonableDist = 0.2;
  fMaximalZ_PhiD = 2.;
  fReasonableZ_PhiD = 5.;

  fMaximalRadDiff = 0.1;
  fMaximalPhiDiff = 0.1;

  fTubeArray = nullptr;
  fSttParameters = nullptr;

  fStopTimeValue = 0.0;
  fFunctor = new StopTime();

  SetPersistency(kTRUE);

  fRunTimeBased = kFALSE;

  Reset();
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndBarrelTrackFinder::~PndBarrelTrackFinder()
{
  fBarrelTrackArray->Delete();
  fBarrelTrackCandArray->Delete();
  Reset();
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void PndBarrelTrackFinder::UseMvdSttGem(const Bool_t useMvd, const Bool_t useStt, const Bool_t useGem)
{
  fIncludeDet[0] = useMvd;
  fIncludeDet[1] = useMvd;
  fIncludeDet[2] = useStt;
  fIncludeDet[3] = useGem;
};


// -------------------------------------------------------------------------
// ------ Get Tube from tube ID -----------------------------------------
PndSttTube* PndBarrelTrackFinder::GetTube(int tubeid)
{
  if (fTubeArray!=nullptr)
    return (PndSttTube *)fTubeArray->At(tubeid);

  return PndSttTubeMap::Instance()->GetTube(tubeid);
}

// -------------------------------------------------------------------------
// ------ Get Tube ID from Stt Hit -----------------------------------------
int PndBarrelTrackFinder::GetTubeID(FairHit* hit)
{
  return ((PndSttHit*)hit)->GetTubeID();
}

// -------------------------------------------------------------------------
// ------ Get Hit Isochrone ------------------------------------------------
double PndBarrelTrackFinder::GetHitIsochrone(FairHit *hit)
{
  return ((PndSttHit*)hit)->GetIsochrone();
}

// -------------------------------------------------------------------------
// ------ Check whether tube with this id is skewed ------------------------
bool PndBarrelTrackFinder::IsSkewTube(int tubeid)
{
  PndSttTube *sttTube = GetTube(tubeid);
  return (sttTube!=nullptr) ? sttTube->IsSkew() : false;
}


// -------------------------------------------------------------------------

// ------ Fill tube parameter array ----------------------------------------
void PndBarrelTrackFinder::GetTubeParams(FairHit* hit, double *tubePar)
{
  int tubeid = GetTubeID(hit);
  
  // fetch STT tube
  PndSttTube *sttTube = GetTube(tubeid);
  // no STT tube -> return
  if (sttTube==nullptr) return;
  
  TVector3 pos = sttTube->GetPosition();
  TVector3 dir = sttTube->GetWireDirection();
  double hlen  = sttTube->GetHalfLength();
  
  Double_t iso = GetHitIsochrone(hit);
  Double_t ra  = TMath::ATan(dir.Pt()/dir.Z());  // dip angle of wire?
  Double_t a   = 0.5*iso*(1.+1./TMath::Cos(ra)); // ??

  tubePar[0] = pos.X();
  tubePar[1] = pos.Y();
  tubePar[2] = pos.Z();
  tubePar[3] = hlen;
  tubePar[4] = dir.X();
  tubePar[5] = dir.Y();
  tubePar[6] = dir.Z();
  tubePar[7] = a;
}


// -------------------------------------------------------------------------
// -----   Public method Exec   --------------------------------------------
void PndBarrelTrackFinder::Exec(Option_t *)
{
  Reset();
  fBarrelTrackArray->Delete();
  fBarrelTrackCandArray->Delete();

  if (fRunTimeBased) {
    for (Int_t idet = 0; idet < 5; idet++) {
      if (fIncludeDet[idet] == kTRUE) {

        fHitArray[idet]->Delete();
      }
    }
  }

  if (fVerbose > 0)
    cout << "=============== EVENT " << fTNofEvents << " =================" << endl;

  // Int_t nofCreatedTracks = 0; //[R.K. 01/2017] unused variable?

  Int_t nofHits = 0;
  Int_t firstDH[5];

  // putting hits into one common array
  for (Int_t idet = 0; idet < 4; idet++) {
    firstDH[idet] = nofHits;
    if (fIncludeDet[idet] == kTRUE) {

      // If run in time based mode
      if (fRunTimeBased) {
        fStopTimeValue += 2000.0; // In [ns]
        fHitArray[idet] = FairRootManager::Instance()->GetData(fHitArrayName[idet], fFunctor, fStopTimeValue);
      }

      nofHits += fHitArray[idet]->GetEntriesFast();
      //      cout << fDetName[idet].Data() << " hits from " << firstDH[idet] << " to " << nofHits << "   ( " << fHitArray[idet]->GetEntriesFast() << " hits )" << endl;
      for (Int_t ihit = firstDH[idet]; ihit < nofHits; ihit++) { // hit mixing could go here
        fHitDetId.push_back(idet);
        fHitDetNo.push_back(ihit - firstDH[idet]);
      }
    }
  }
  std::vector<Bool_t> hitUsed(fHitDetId.size(), kFALSE);

  FairHit *detHit;
  // DRAW

  if (fCanvas) {
    fCanvas->Clear();
    Double_t minX = -60.;
    Double_t maxX = +60.;
    Double_t minY = -60.;
    Double_t maxY = +60.;

    fCanvas->Range(minX, minY, maxX, maxY);
    fCanvas->SetFillColor(0);
    fCanvas->SetBorderSize(0);
    fCanvas->SetBorderMode(0);
    fCanvas->SetFrameFillColor(0);
    fCanvas->SetGrid(0, 0);
    fCanvas->SetLogy(0);

    // middle cross
    TLine *horLft = new TLine(-5., 0., -2., 0.);
    horLft->SetLineWidth(2);
    horLft->Draw();
    TLine *horRgt = new TLine(2., 0., 5., 0.);
    horRgt->SetLineWidth(2);
    horRgt->Draw();
    TLine *verTop = new TLine(0., 2., 0., 5.);
    verTop->SetLineWidth(2);
    verTop->Draw();
    TLine *verBot = new TLine(0., -5., 0., -2.);
    verBot->SetLineWidth(2);
    verBot->Draw();
    for (Int_t xline = -100; xline <= 100; xline += 10) {
      TLine *verGrid = new TLine(xline, -100, xline, 100);
      verGrid->SetLineStyle(2);
      verGrid->Draw();
    }
    for (Int_t yline = -100; yline <= 100; yline += 10) {
      TLine *horGrid = new TLine(-100, yline, 100, yline);
      horGrid->SetLineStyle(2);
      horGrid->Draw();
    }

    for (Int_t iell = 0; iell < 1000; iell++) {
      fEllipse[iell]->SetX1(100);
      fEllipse[iell]->SetY1(100);
      fEllipse[iell]->SetR1(0);
      fEllipse[iell]->SetR2(0);
      fEllipse[iell]->Draw();
      fStageDesc->Draw();
    }

    for (Int_t ihit = 0; ihit < nofHits; ihit++) {
      // MVD hit
      if (fHitDetId[ihit] == 0 || fHitDetId[ihit] == 1) { 
        detHit = (PndSdsHit *)fHitArray[fHitDetId[ihit]]->At(fHitDetNo[ihit]);
        TMarker *marker = new TMarker(detHit->GetX(), detHit->GetY(), 20);
        marker->SetMarkerColor(3);
        marker->Draw();
      } 
      // STT1/2 hit
      else if (fHitDetId[ihit] == 2) {
        detHit = (FairHit*) fHitArray[fHitDetId[ihit]]->At(fHitDetNo[ihit]);
        if (IsSkewTube(GetTubeID(detHit))) {
          continue; // don't draw skewed hits (how else to put them?)
        }
                          
        TEllipse *ellipse = new TEllipse(detHit->GetX(), detHit->GetY(), GetHitIsochrone(detHit));
        ellipse->SetFillStyle(0);
        ellipse->SetLineWidth(2);
        ellipse->SetLineColor(2);
        ellipse->Draw();
      } 
      // GEM hit
      else if (fHitDetId[ihit] == 3) { 
        detHit = (PndGemHit *)fHitArray[fHitDetId[ihit]]->At(fHitDetNo[ihit]);
        TMarker *marker = new TMarker(detHit->GetX(), detHit->GetY(), 20);
        marker->SetMarkerColor(6);
        marker->Draw();
      }
    }
    fStageDesc->SetText(-50., 50., "HITS");
    fStageDesc->Paint();
    fCanvas->Modified();
    fCanvas->Update();
    gSystem->Sleep(1000);
  }

  Int_t hitN = 0;
  // looping over hits as they are in the common array

  for (Int_t ihit = 0; ihit < nofHits; ihit++) {

    hitN = (hitN + gRandom->Integer(nofHits)) % nofHits;
    while (1 == 1) {
      if (hitUsed[hitN] == kFALSE)
        break;
      hitN = (hitN + 1) % nofHits;
    }
    hitUsed[hitN] = kTRUE;
    //    cout << " for hit " << ihit << " would like to take hit " << hitN << " >> " << fHitDetId[hitN] << "." << fHitDetNo[hitN] << endl;

    //    hitN = (firstDH[2]+ihit)%nofHits; // hit mixing could go here

    if (fHitDetId[hitN] == 0 || fHitDetId[hitN] == 1)
      detHit = (PndSdsHit *)fHitArray[fHitDetId[hitN]]->At(fHitDetNo[hitN]);
    else if (fHitDetId[hitN] == 2)
      detHit = (PndSttHit *)fHitArray[fHitDetId[hitN]]->At(fHitDetNo[hitN]);
    //else if (fHitDetId[hitN] == 2 && fSttVer==0)
      //detHit = (PndSttHit *)fHitArray[fHitDetId[hitN]]->At(fHitDetNo[hitN]);
    //else if (fHitDetId[hitN] == 2 && fSttVer>0)
      //detHit = (PndStt2Hit *)fHitArray[fHitDetId[hitN]]->At(fHitDetNo[hitN]);
    else if (fHitDetId[hitN] == 3)
      detHit = (PndGemHit *)fHitArray[fHitDetId[hitN]]->At(fHitDetNo[hitN]);

    if (fVerbose > 3) {
      cout << "--------------------------------------------------------" << endl;
      cout << ihit << " (" << hitN << ") > " << fHitDetId[hitN] << "." << fHitDetNo[hitN] << " in " << fDetName[fHitDetId[hitN]].Data() << " at " << detHit->GetX() << " "
           << detHit->GetY() << " " << detHit->GetZ() << "    error " << detHit->GetDx() << " " << detHit->GetDy() << " " << detHit->GetDz() << endl;
    }
    if (fVerbose > 3)
      PrintTracks();

    Bool_t sttHit = kFALSE;
    Bool_t skewedSttHit = kFALSE;
    if (fHitDetId[hitN] == 2) {
      sttHit = kTRUE;
      skewedSttHit = IsSkewTube(GetTubeID(detHit));
      if (skewedSttHit) {
        if (fVerbose > 3)
          cout << "SKEWED HIT" << endl;
      }
    }

    Bool_t hitBelongsToKnownTrack = kFALSE;
    for (size_t itr = 0; itr < fTracksVector.size(); itr++) {
      if (!sttHit) {
        if (MatchHitWithZInfoTT(detHit, fHitDetId[hitN], fHitDetNo[hitN], itr))
          hitBelongsToKnownTrack = kTRUE;
      } else if (skewedSttHit) {
        if (MatchSkewedSttHitTT(detHit, fHitDetId[hitN], fHitDetNo[hitN], itr))
          hitBelongsToKnownTrack = kTRUE;
      } else {
        if (MatchParallelSttHitTT(detHit, fHitDetId[hitN], fHitDetNo[hitN], itr))
          hitBelongsToKnownTrack = kTRUE;
      }
    }
    if (hitBelongsToKnownTrack)
      continue;

    if (skewedSttHit) {
      AddHitToPreviousHits(detHit, fHitDetId[hitN], fHitDetNo[hitN]);
      continue;
    }

    Bool_t hitMatchedToHit = kFALSE;
    if (fVerbose > 3)
      cout << "TRYING TO MATCH TO PREVIOUS " << fHitVector.size() << " HITS" << endl;
    for (Int_t ihitP = fHitVector.size() - 1; ihitP >= 0; ihitP--) {
      if (MatchHitToHit(detHit, fHitDetId[hitN], fHitDetNo[hitN], ihitP))
        hitMatchedToHit = kTRUE;
    }
    if (hitMatchedToHit)
      continue;

    AddHitToPreviousHits(detHit, fHitDetId[hitN], fHitDetNo[hitN]);
  }
  if (fCanvas) {
    DrawTracks();
    fStageDesc->SetText(-50., 50., "TRACKS FOUND");
    fStageDesc->Paint();
    fCanvas->Modified();
    fCanvas->Update();
    gSystem->Sleep(1000);
  }
  //   PrintTracks();
  //  cout << "REMOVING TRACKS!" << endl;
  RemoveShortTracks();
  //  PrintTracks();
  if (fVerbose > 0)
    PrintTracks();
  if (fCanvas) {
    DrawTracks();
    fStageDesc->SetText(-50., 50., "SHORT TRACKS REMOVED");
    fStageDesc->Paint();
    fCanvas->Modified();
    fCanvas->Update();
    gSystem->Sleep(1000);
  }

  // deal with left previous hits
  for (Int_t iuh = fHitVector.size() - 1; iuh >= 0; iuh--) {
    if (fVerbose > 3)
      cout << "PREVIOUS HIT " << iuh << " out of " << fHitVector.size() << endl;
    Bool_t sttHit = kFALSE;
    Bool_t skewedSttHit = kFALSE;
    if (fHitVectDI[iuh] == 2) {
      sttHit = kTRUE;
      skewedSttHit = IsSkewTube(GetTubeID(fHitVector[iuh]));
    }

    Bool_t hitBelongsToKnownTrack = kFALSE;
    for (size_t itr = 0; itr < fTracksVector.size(); itr++) {
      if (HitBelongsToTrack(fHitVectDI[iuh], fHitVectHN[iuh], itr))
        continue;
      if (!sttHit) {
        if (MatchHitWithZInfoTT(fHitVector[iuh], fHitVectDI[iuh], fHitVectHN[iuh], itr))
          hitBelongsToKnownTrack = kTRUE;
      } else if (skewedSttHit) {
        if (MatchSkewedSttHitTT(fHitVector[iuh], fHitVectDI[iuh], fHitVectHN[iuh], itr))
          hitBelongsToKnownTrack = kTRUE;
      } else {
        if (MatchParallelSttHitTT(fHitVector[iuh], fHitVectDI[iuh], fHitVectHN[iuh], itr))
          hitBelongsToKnownTrack = kTRUE;
      }
    }
    if (hitBelongsToKnownTrack) {
      RemoveHitFromPreviousHits(iuh);
      continue;
    }
  }

  // cout << "==============================================================" << endl;
  // cout << "==============================================================" << endl;
  // cout << "there are still " << fHitVector.size() << " unused hits: " << flush;
  // for ( Int_t iuh = 0 ; iuh < fHitVector.size() ; iuh++ )
  //   cout << fHitVectDI[iuh] << "." << fHitVectHN[iuh] << " " << flush;
  // cout << endl;
  // cout << "==============================================================" << endl;
  //  cout << "==============================================================" << endl;
  //  cout << "FOUND " << fTracksVector.size() << " tracks:" << endl;
  if (fVerbose > 0)
    PrintTracks();
  if (fCanvas) {
    DrawTracks();
    fStageDesc->SetText(-50., 50., "TRACKS REORGANIZED");
    fStageDesc->Paint();
    fCanvas->Modified();
    fCanvas->Update();
    gSystem->Sleep(1000);
  }
  //  cout << "==============================================================" << endl;
  //  cout << "REMOVE CLONE TRACKS" << endl;
  RemoveCloneTracks();
  if (fVerbose > 0)
    PrintTracks();
  if (fCanvas) {
    DrawTracks();
    fStageDesc->SetText(-50., 50., "CLONE TRACKS REMOVED");
    fStageDesc->Paint();
    fCanvas->Modified();
    fCanvas->Update();
    gSystem->Sleep(5000);
  }

  WriteTracks();

  fTNofEvents++;
  fTNofTracks += fTracksVector.size();

  fTracksVector.clear();
  fHitDetId.clear();
  fHitDetNo.clear();
  fHitVector.clear();
  fHitVectDI.clear();
  fHitVectHN.clear();

  return;
}
// -------------------------------------------------------------------------

// -----   Private method MatchHitToTrack   -------------------------------
Bool_t PndBarrelTrackFinder::MatchHitToTrack(FairHit *thisHit, Int_t detId, Int_t hitNo, Int_t trackNo)
{
  if (thisHit)
    cout << detId << "." << hitNo << " to " << trackNo << endl;
  return kFALSE;
}
// -------------------------------------------------------------------------

// -----   Private method HitBelongsToTrack   -------------------------------
Bool_t PndBarrelTrackFinder::HitBelongsToTrack(Int_t detId, Int_t hitNo, Int_t trackNo)
{
  for (size_t ihitP = 0; ihitP < fTracksVector[trackNo].trackHitD.size(); ihitP++) {
    if (fTracksVector[trackNo].trackHitD[ihitP] == detId && fTracksVector[trackNo].trackHitN[ihitP] == hitNo)
      return kTRUE;
  }
  return kFALSE;
}
// -------------------------------------------------------------------------

// -----   Private method MatchSkewedSttHitTT   -------------------------------
Bool_t PndBarrelTrackFinder::MatchSkewedSttHitTT(FairHit *thisHit, Int_t detId, Int_t hitNo, Int_t trackNo)
{
  Bool_t printInfo = kFALSE; // kTRUE;
  //   if ( fVerbose > 4 )
  //     cout << "skewed hit" << endl;
  if (TMath::Abs(fTracksVector[trackNo].meanR) < 1.)
    return kFALSE;

  // fetch tube position, direction and length
  // [0]-[2] = posX, posY, posZ, [3]=length/2, [4]-[6] = dirX, dirX, dirZ, [7] = 0.5*r_iso*(1+1/cos(theta_wire)) 
  Double_t tubePar[8];
  GetTubeParams(thisHit, tubePar);

  if (fVerbose > 4 || printInfo)
    cout << "matching to track " << trackNo << " with " << fTracksVector[trackNo].trackHits.size() << " hits" << endl;

  Bool_t trackFits = kFALSE;

  Double_t meanPhi = CalcPhi(fTracksVector[trackNo].meanX, fTracksVector[trackNo].meanY);
  Double_t meanCirc[4] = {fTracksVector[trackNo].meanX, fTracksVector[trackNo].meanY, fTracksVector[trackNo].meanR, meanPhi};
  Double_t intReg[4];

  Int_t nofReg = FindInterestingRegions(meanCirc, tubePar, intReg);
  if (nofReg == 0)
    return kFALSE;

  if (fVerbose > 4 || printInfo) {
    cout << " --> connects to " << trackNo << " with (" << meanPhi << " " << meanCirc[2] << " " << fTracksVector[trackNo].meanZ_Phi << ") at z_phi = " << flush;
    for (Int_t ireg = 0; ireg < nofReg; ireg++)
      cout << intReg[ireg] << "   " << flush;
    cout << endl;
  }

  // there is no mean z_phi, attach all we can
  if (fTracksVector[trackNo].meanZ_Phi > 1.e6) {
    for (Int_t ireg = 0; ireg < nofReg; ireg++) {
      TrackParameter tp0;
      tp0.x = meanCirc[0];
      tp0.y = meanCirc[1];
      tp0.r = meanCirc[2];
      tp0.z_phi = intReg[ireg];
      tp0.n = 400 / nofReg; // fTracksVector[trackNo].trackHits.size();
      fTracksVector[trackNo].trackPars.push_back(tp0);
      if (fVerbose > 4 || printInfo)
        cout << "creating another trackPar to track " << trackNo << " with z_phi -> " << intReg[ireg] << endl;
    }
    fTracksVector[trackNo].trackHits.push_back(thisHit);
    fTracksVector[trackNo].trackHitD.push_back(detId);
    fTracksVector[trackNo].trackHitN.push_back(hitNo);

    ExtractMeanZ_PhiFromTrack(trackNo);
  }
  // there is already mean z_phi !!!, check if any is good
  else {
    //    cout << "should compare if any of new found z_phi: " << flush;
    for (Int_t ireg = 0; ireg < nofReg; ireg++) {
      // cout << intReg[ireg] << " " << flush;
      if (TMath::Abs(intReg[ireg] - fTracksVector[trackNo].meanZ_Phi) < fReasonableZ_PhiD)
        trackFits = kTRUE;
    }
    //    cout << " is close enough to mean: " << fTracksVector[trackNo].meanZ_Phi << endl;
    if (!trackFits)
      return kFALSE;

    fTracksVector[trackNo].trackHits.push_back(thisHit);
    fTracksVector[trackNo].trackHitD.push_back(detId);
    fTracksVector[trackNo].trackHitN.push_back(hitNo);
  }

  return kTRUE;
}
// -------------------------------------------------------------------------

// -----   Private method MatchParallelSttHitTT   -------------------------------
Bool_t PndBarrelTrackFinder::MatchParallelSttHitTT(FairHit *thisHit, Int_t detId, Int_t hitNo, Int_t trackNo)
{
  Bool_t hitBelongsToTrack = kFALSE;

  Double_t sH1[3] = {thisHit->GetX(), thisHit->GetY(), 0.};
  //sH1[2] = ((PndSttHit *)thisHit)->GetIsochrone();
  sH1[2] = GetHitIsochrone(thisHit);
  Double_t sH2[3];                // in the loop over track previous hits
  Double_t sH3[3] = {0., 0., 0.}; // set to origin now, should be another hit (double for loop over hits)
  Double_t sH4[3];                // another loop over hits
  Double_t circPar[3];

  Double_t thisHitPhi = CalcPhi(sH1[0], sH1[1]);
  Double_t thisTrackPhi = CalcPhi(fTracksVector[trackNo].trackHits[0]->GetX(), fTracksVector[trackNo].trackHits[0]->GetY());
  Double_t phiHTDiffr = TMath::Abs(thisHitPhi - thisTrackPhi);
  if (phiHTDiffr > TMath::Pi() / 4. && phiHTDiffr < TMath::Pi() * 7. / 4.)
    return kFALSE; // checking if hit and track match approximately, should make it radius dependent

  if (TMath::Abs(fTracksVector[trackNo].meanR) > 1.) {
    circPar[0] = fTracksVector[trackNo].meanX;
    circPar[1] = fTracksVector[trackNo].meanY;
    circPar[2] = fTracksVector[trackNo].meanR;

    if (FindCircDist(circPar, sH1) < fReasonableDist)
      hitBelongsToTrack = kTRUE;
    if (!hitBelongsToTrack)
      return kFALSE;

    // putting thisHit into track
    fTracksVector[trackNo].trackHits.push_back(thisHit);
    fTracksVector[trackNo].trackHitD.push_back(detId);
    fTracksVector[trackNo].trackHitN.push_back(hitNo);

    if (fVerbose > 3)
      cout << "this stt hit belongs to track " << trackNo << " (cause dist = " << FindCircDist(circPar, sH1) << ")" << endl;
  } else {
    // track has no mean information yet
    for (size_t ipar = 0; ipar < fTracksVector[trackNo].trackPars.size(); ipar++) {
      if (fTracksVector[trackNo].trackPars[ipar].n < 0.9 * fTracksVector[trackNo].trackHits.size())
        continue;

      circPar[0] = fTracksVector[trackNo].trackPars[ipar].x;
      circPar[1] = fTracksVector[trackNo].trackPars[ipar].y;
      circPar[2] = fTracksVector[trackNo].trackPars[ipar].r;

      if (fVerbose > 4) {
        cout << " ..... to parameter (" << trackNo << "." << ipar << ") " << CalcPhi(circPar[0], circPar[1]) << " " << circPar[2] << ": distance is " << FindCircDist(sH1, circPar)
             << flush;
        cout << endl;
      }

      if (FindCircDist(sH1, circPar) < fMaximalDist) {
        hitBelongsToTrack = kTRUE;

        fTracksVector[trackNo].trackPars[ipar].n += 1; // because thisHit is close to parameter ipar

        if (fVerbose > 4)
          cout << " par: " << CalcPhi(circPar[0], circPar[1]) << " | " << circPar[2] << " of track " << trackNo << " matches to this hit with dist " << FindCircDist(sH1, circPar)
               << endl;
      }
    }
    if (!hitBelongsToTrack)
      return kFALSE;

    // calculating track parameters with previous hits belonging to the track
    for (size_t ihitP = 0; ihitP < fTracksVector[trackNo].trackHitD.size(); ihitP++) {
      sH2[0] = fTracksVector[trackNo].trackHits[ihitP]->GetX();
      sH2[1] = fTracksVector[trackNo].trackHits[ihitP]->GetY();
      sH2[2] = 0.;
      if (fTracksVector[trackNo].trackHitD[ihitP] == 2)
        sH2[2] = GetHitIsochrone(fTracksVector[trackNo].trackHits[ihitP]);
        //sH2[2] = ((PndSttHit *)fTracksVector[trackNo].trackHits[ihitP])->GetIsochrone();

      Int_t stepCno = 1;
      if (sH2[2] < 1.e-5)
        stepCno += 2;

      for (Int_t icirc = 0; icirc < 4; icirc += stepCno) {
        if (FindCircPar(sH1, sH2, sH3, icirc, circPar)) {
          if (circPar[2] < 10. || circPar[2] > 100000.)
            continue; // for now remove too egzotic tracks

          Double_t newParZ_Phi = 2.e6;
          if (sH2[2] < 1.e-5)
            newParZ_Phi = CalcZ_Phi(circPar, sH2[0], sH2[1], fTracksVector[trackNo].trackHits[ihitP]->GetZ());

          Int_t nofCloseHits = 2;
          for (size_t ihitF = 0; ihitF < fTracksVector[trackNo].trackHitD.size(); ihitF++) {
            if (ihitF == ihitP)
              continue;
            sH4[0] = fTracksVector[trackNo].trackHits[ihitF]->GetX();
            sH4[1] = fTracksVector[trackNo].trackHits[ihitF]->GetY();
            sH4[2] = 0.;
            if (fTracksVector[trackNo].trackHitD[ihitF] == 2)
              //sH4[2] = ((PndSttHit *)fTracksVector[trackNo].trackHits[ihitF])->GetIsochrone();
              sH4[2] = GetHitIsochrone(fTracksVector[trackNo].trackHits[ihitF]);

            if (FindCircDist(sH4, circPar) < fMaximalDist ||                         // if the hit is very very close to the new parameter in xy plane
                (newParZ_Phi < 1.e6 && FindCircDist(sH4, circPar) < fReasonableDist && // if the hit is close to the new parameter in xy plane and pz space
                 TMath::Abs(CalcZ_Phi(circPar, sH4[0], sH4[1], fTracksVector[trackNo].trackHits[ihitF]->GetZ()) - newParZ_Phi) < fMaximalZ_PhiD)) {
              nofCloseHits += 1;
            }
          }

          if (fVerbose > 5)
            cout << "there are " << nofCloseHits << " with limit set to " << 0.6 * fTracksVector[trackNo].trackHits.size() << " (0.6*" << fTracksVector[trackNo].trackHits.size()
                 << ")" << endl;
          if (nofCloseHits < 0.6 * fTracksVector[trackNo].trackHits.size())
            continue; // the track param has not enough close hits among previous track hits

          TrackParameter tp0;
          tp0.x = circPar[0];
          tp0.y = circPar[1];
          tp0.r = circPar[2];
          tp0.z_phi = newParZ_Phi;
          tp0.n = nofCloseHits;

          fTracksVector[trackNo].trackPars.push_back(tp0);
        }
      }
    }

    // putting thisHit into track
    fTracksVector[trackNo].trackHits.push_back(thisHit);
    fTracksVector[trackNo].trackHitD.push_back(detId);
    fTracksVector[trackNo].trackHitN.push_back(hitNo);

    if (fVerbose > 5) {
      cout << "HIT DOES " << (hitBelongsToTrack ? "" : "NOT ") << "BELONG TO TRACK " << trackNo << " WITH " << fTracksVector[trackNo].trackHits.size() << " HITS, "
           << fTracksVector[trackNo].trackPars.size() << " PARS: phi,rad,z_phi,n: " << endl;
      for (size_t ipar = 0; ipar < fTracksVector[trackNo].trackPars.size(); ipar++)
        cout << CalcPhi(fTracksVector[trackNo].trackPars[ipar].x, fTracksVector[trackNo].trackPars[ipar].y) << " " << fTracksVector[trackNo].trackPars[ipar].r << " "
             << fTracksVector[trackNo].trackPars[ipar].z_phi << " " << fTracksVector[trackNo].trackPars[ipar].n << "  |  " << flush;
      cout << endl;
    }

    ExtractMeanRPhiFromTrack(trackNo);
  }

  return kTRUE;
}
// -------------------------------------------------------------------------

// -----   Private method MatchHitWithZInfoTT   -------------------------------
Bool_t PndBarrelTrackFinder::MatchHitWithZInfoTT(FairHit *thisHit, Int_t detId, Int_t hitNo, Int_t trackNo)
{
  Bool_t hitBelongsToTrack = kFALSE;

  Double_t sH1[3] = {thisHit->GetX(), thisHit->GetY(), 0.};
  Double_t sH2[3];                // in the loop over track previous hits
  Double_t sH3[3] = {0., 0., 0.}; // set to origin now, should be another hit (double for loop over hits)
  Double_t sH4[3];                // another loop over hits
  Double_t circPar[3];

  Double_t thisHitPhi = CalcPhi(sH1[0], sH1[1]);
  Double_t thisTrackPhi = CalcPhi(fTracksVector[trackNo].trackHits[0]->GetX(), fTracksVector[trackNo].trackHits[0]->GetY());
  Double_t phiHTDiffr = TMath::Abs(thisHitPhi - thisTrackPhi);
  if (phiHTDiffr > TMath::Pi() / 4. && phiHTDiffr < TMath::Pi() * 7. / 4.)
    return kFALSE; // checking if hit and track match approximately, should make it radius dependent

  if (TMath::Abs(fTracksVector[trackNo].meanR) > 1.) {
    circPar[0] = fTracksVector[trackNo].meanX;
    circPar[1] = fTracksVector[trackNo].meanY;
    circPar[2] = fTracksVector[trackNo].meanR;

    if (fVerbose > 3)
      cout << "hit " << detId << "." << hitNo << " distance to track " << trackNo << " is " << FindCircDist(circPar, sH1) << " with errors ( " << ((PndGemHit *)thisHit)->GetDr()
           << " , " << ((PndGemHit *)thisHit)->GetDp() << " ) " << endl;

    if (FindCircDist(circPar, sH1) < fReasonableDist)
      hitBelongsToTrack = kTRUE;
    if (!hitBelongsToTrack)
      return kFALSE;

    if (fTracksVector[trackNo].meanZ_Phi > 1.e6) {
      TrackParameter tp0;
      tp0.x = circPar[0];
      tp0.y = circPar[1];
      tp0.r = circPar[2];
      tp0.z_phi = CalcZ_Phi(circPar, sH1[0], sH1[1], thisHit->GetZ());
      tp0.n = 400;
      fTracksVector[trackNo].trackPars.push_back(tp0);

      // putting thisHit into track
      fTracksVector[trackNo].trackHits.push_back(thisHit);
      fTracksVector[trackNo].trackHitD.push_back(detId);
      fTracksVector[trackNo].trackHitN.push_back(hitNo);
      ExtractMeanZ_PhiFromTrack(trackNo);
    } else {
      //      cout << "should compare if new found z_phi " << CalcZ_Phi(circPar,sH1[0],sH1[1],thisHit->GetZ()) << " is close enough to mean: " << fTracksVector[trackNo].meanZ_Phi <<
      //      endl;

      if (TMath::Abs(CalcZ_Phi(circPar, sH1[0], sH1[1], thisHit->GetZ()) - fTracksVector[trackNo].meanZ_Phi) < fReasonableZ_PhiD) {
        // putting thisHit into track
        fTracksVector[trackNo].trackHits.push_back(thisHit);
        fTracksVector[trackNo].trackHitD.push_back(detId);
        fTracksVector[trackNo].trackHitN.push_back(hitNo);
      } else
        return kFALSE;
    }
    if (fVerbose > 3)
      cout << " This hit belongs to track " << trackNo << " with " << CalcZ_Phi(circPar, sH1[0], sH1[1], thisHit->GetZ()) << endl;
  } else {
    // track has no mean information yet
    for (size_t ipar = 0; ipar < fTracksVector[trackNo].trackPars.size(); ipar++) {
      if (fTracksVector[trackNo].trackPars[ipar].n < 0.6 * fTracksVector[trackNo].trackHits.size())
        continue;

      circPar[0] = fTracksVector[trackNo].trackPars[ipar].x;
      circPar[1] = fTracksVector[trackNo].trackPars[ipar].y;
      circPar[2] = fTracksVector[trackNo].trackPars[ipar].r;

      if (fVerbose > 4) {
        cout << " ..... to parameter (" << trackNo << "." << ipar << ") " << CalcPhi(circPar[0], circPar[1]) << " " << circPar[2] << ": distance is " << FindCircDist(sH1, circPar)
             << flush;
        cout << "   new z_phi is " << CalcZ_Phi(circPar, sH1[0], sH1[1], thisHit->GetZ()) << " vs " << fTracksVector[trackNo].trackPars[ipar].z_phi << " old" << flush;
        cout << endl;
      }

      if ((FindCircDist(sH1, circPar) < fMaximalDist &&
           TMath::Abs(CalcZ_Phi(circPar, sH1[0], sH1[1], thisHit->GetZ()) - fTracksVector[trackNo].trackPars[ipar].z_phi) < 3. * fReasonableZ_PhiD) ||
          (FindCircDist(sH1, circPar) < 3. * fReasonableDist &&
           TMath::Abs(CalcZ_Phi(circPar, sH1[0], sH1[1], thisHit->GetZ()) - fTracksVector[trackNo].trackPars[ipar].z_phi) < fMaximalZ_PhiD)) {
        hitBelongsToTrack = kTRUE;

        fTracksVector[trackNo].trackPars[ipar].n += 1; // because thisHit is close to parameter ipar

        if (fVerbose > 4)
          cout << " par: " << CalcPhi(circPar[0], circPar[1]) << " | " << circPar[2] << " of track " << trackNo << " matches to this hit with dist " << FindCircDist(sH1, circPar)
               << endl;
      }
    }
    if (!hitBelongsToTrack)
      return kFALSE;

    // calculating track parameters with previous hits belonging to the track
    for (size_t ihitP = 0; ihitP < fTracksVector[trackNo].trackHitD.size(); ihitP++) {
      sH2[0] = fTracksVector[trackNo].trackHits[ihitP]->GetX();
      sH2[1] = fTracksVector[trackNo].trackHits[ihitP]->GetY();
      sH2[2] = 0.;
      if (fTracksVector[trackNo].trackHitD[ihitP] == 2)
        //sH2[2] = ((PndSttHit *)fTracksVector[trackNo].trackHits[ihitP])->GetIsochrone();
        sH2[2] = GetHitIsochrone(fTracksVector[trackNo].trackHits[ihitP]);

      Int_t stepCno = 2;
      if (sH2[2] < 1.e-5)
        stepCno += 2;

      for (Int_t icirc = 0; icirc < 4; icirc += stepCno) {
        if (FindCircPar(sH1, sH2, sH3, icirc, circPar)) {
          if (circPar[2] < 10. || circPar[2] > 100000.)
            continue; // for now remove too egzotic tracks

          Double_t newParZ_Phi = CalcZ_Phi(circPar, sH1[0], sH1[1], thisHit->GetZ());

          Int_t nofCloseHits = 2;
          for (size_t ihitF = 0; ihitF < fTracksVector[trackNo].trackHitD.size(); ihitF++) {
            if (ihitF == ihitP)
              continue;
            sH4[0] = fTracksVector[trackNo].trackHits[ihitF]->GetX();
            sH4[1] = fTracksVector[trackNo].trackHits[ihitF]->GetY();
            sH4[2] = 0.;
            if (fTracksVector[trackNo].trackHitD[ihitF] == 2)
              //sH4[2] = ((PndSttHit *)fTracksVector[trackNo].trackHits[ihitF])->GetIsochrone();
              sH4[2] = GetHitIsochrone(fTracksVector[trackNo].trackHits[ihitF]);

            // 	    if ( FindCircDist(sH4,circPar) < fMaximalDist ||     // if the hit is very very close to the new parameter in xy plane
            // 		 ( FindCircDist(sH4,circPar) < fReasonableDist &&   // if the hit is close to the new parameter in xy plane and pz space
            // 		   TMath::Abs(CalcZ_Phi(circPar,sH4[0],sH4[1],fTracksVector[trackNo].trackHits[ihitF]->GetZ())-newParZ_Phi) < fMaximalZ_PhiD ) ) {
            if ((FindCircDist(sH4, circPar) < fMaximalDist &&
                 TMath::Abs(CalcZ_Phi(circPar, sH4[0], sH4[1], fTracksVector[trackNo].trackHits[ihitF]->GetZ()) - newParZ_Phi) < 3. * fReasonableZ_PhiD) ||
                (FindCircDist(sH4, circPar) < 3. * fReasonableDist &&
                 TMath::Abs(CalcZ_Phi(circPar, sH4[0], sH4[1], fTracksVector[trackNo].trackHits[ihitF]->GetZ()) - newParZ_Phi) < fMaximalZ_PhiD)) {
              nofCloseHits += 1;
            }
          }

          if (fVerbose > 5)
            cout << "there are " << nofCloseHits << " with limit set to " << 0.6 * fTracksVector[trackNo].trackHits.size() << " (0.6*" << fTracksVector[trackNo].trackHits.size()
                 << ")" << endl;
          if (nofCloseHits < 0.6 * fTracksVector[trackNo].trackHits.size())
            continue; // the track param has not enough close hits among previous track hits

          TrackParameter tp0;
          tp0.x = circPar[0];
          tp0.y = circPar[1];
          tp0.r = circPar[2];
          tp0.z_phi = newParZ_Phi;
          tp0.n = nofCloseHits;

          fTracksVector[trackNo].trackPars.push_back(tp0);
        }
      }
    }

    // putting thisHit into track
    fTracksVector[trackNo].trackHits.push_back(thisHit);
    fTracksVector[trackNo].trackHitD.push_back(detId);
    fTracksVector[trackNo].trackHitN.push_back(hitNo);

    if (fVerbose > 5) {
      cout << "HIT DOES " << (hitBelongsToTrack ? "" : "NOT ") << "BELONG TO TRACK " << trackNo << " WITH " << fTracksVector[trackNo].trackHits.size() << " HITS, "
           << fTracksVector[trackNo].trackPars.size() << " PARS: phi,rad,z_phi,n: " << endl;
      for (size_t ipar = 0; ipar < fTracksVector[trackNo].trackPars.size(); ipar++)
        cout << CalcPhi(fTracksVector[trackNo].trackPars[ipar].x, fTracksVector[trackNo].trackPars[ipar].y) << " " << fTracksVector[trackNo].trackPars[ipar].r << " "
             << fTracksVector[trackNo].trackPars[ipar].z_phi << " " << fTracksVector[trackNo].trackPars[ipar].n << "  |  " << flush;
      cout << endl;
    }

    ExtractMeanRPhiFromTrack(trackNo);
  }

  return kTRUE;
}
// -------------------------------------------------------------------------

// -----   Private method MatchHitToHit   -------------------------------
Bool_t PndBarrelTrackFinder::MatchHitToHit(FairHit *thisHit, Int_t detId, Int_t hitNo, Int_t prevHNo)
{
  Double_t sH1[3] = {thisHit->GetX(), thisHit->GetY(), 0.};
  if (detId == 2)
    //sH1[2] = ((PndSttHit *)thisHit)->GetIsochrone();
    sH1[2] = GetHitIsochrone(thisHit);

  Double_t sH2[3] = {fHitVector[prevHNo]->GetX(), fHitVector[prevHNo]->GetY(), 0.};
  if (fHitVectDI[prevHNo] == 2)
    //sH2[2] = ((PndSttHit *)fHitVector[prevHNo])->GetIsochrone();
    sH2[2] = GetHitIsochrone(fHitVector[prevHNo]);

  Double_t sH3[3] = {0., 0., 0.}; // set to origin now, should be another hit (double for loop over hits)
  Double_t circPar[3];

  //      cout << fHitVector[prevHNo]->GetZ() << " " << flush;
  if (fHitVectDI[prevHNo] == 2) {
    if (IsSkewTube(GetTubeID(fHitVector[prevHNo])))
      return kFALSE;

    sH2[2] = GetHitIsochrone(fHitVector[prevHNo]);
  } else {
    if (detId == fHitVectDI[prevHNo])
      if (TMath::Abs(thisHit->GetZ() - fHitVector[prevHNo]->GetZ()) < 3.)
        return kFALSE;
    if (detId != 2) {
      Double_t thisRad = TMath::Sqrt(sH1[0] * sH1[0] + sH1[1] * sH1[1]);
      Double_t prevRad = TMath::Sqrt(sH2[0] * sH2[0] + sH2[1] * sH2[1]);
      if (fVerbose > 3) {
        cout << "matching r z " << thisRad << " " << thisHit->GetZ() << " (" << sH1[0] << " , " << sH1[1] << ") " << endl;
        cout << "    with r z " << prevRad << " " << fHitVector[prevHNo]->GetZ() << " (" << sH2[0] << " , " << sH2[1] << ") " << endl;
      }
    }
  }

  Bool_t hitsMatched = kFALSE;

  Double_t thisHitPhi = CalcPhi(sH1[0], sH1[1]);
  Double_t prevHitPhi = CalcPhi(sH2[0], sH2[1]);

  Double_t phiDiffr = TMath::Abs(thisHitPhi - prevHitPhi);
  if (phiDiffr > TMath::Pi() / 10. && phiDiffr < TMath::Pi() * 19. / 10.)
    return kFALSE;

  Int_t stepCno = 1;
  if (sH1[2] < 1.e-5)
    stepCno = 2;
  if (sH2[2] < 1.e-5)
    stepCno += 2;
  //   cout << "stepCno = " << stepCno << " cause " << sH1[2] << " and " << sH2[2] << endl;

  Int_t fTN = -1;
  for (Int_t icirc = 0; icirc < 4; icirc += stepCno) {
    //    cout << icirc << endl;
    if (FindCircPar(sH1, sH2, sH3, icirc, circPar)) {
      //      cout << " ---> " << circPar[2] << endl;
      if (circPar[2] < 10. || circPar[2] > 100000.)
        continue;

      TrackParameter tp0;
      tp0.x = circPar[0];
      tp0.y = circPar[1];
      tp0.r = circPar[2];
      tp0.z_phi = 2.e6;
      // temporary recognition of non-STT hit
      // should take only parameters created using hit with VALID z information
      if (sH1[2] < 1.e-5 && sH2[2] < 1.e-5) {
        if (TMath::Abs(CalcZ_Phi(circPar, sH1[0], sH1[1], thisHit->GetZ()) - CalcZ_Phi(circPar, sH2[0], sH2[1], fHitVector[prevHNo]->GetZ())) > 3. * fReasonableZ_PhiD) {
          continue;
        }
        tp0.z_phi = (CalcZ_Phi(circPar, sH1[0], sH1[1], thisHit->GetZ()) + CalcZ_Phi(circPar, sH2[0], sH2[1], fHitVector[prevHNo]->GetZ())) / 2.;
      } else {
        if (sH1[2] < 1.e-5)
          tp0.z_phi = CalcZ_Phi(circPar, sH1[0], sH1[1], thisHit->GetZ());
        else if (sH2[2] < 1.e-5)
          tp0.z_phi = CalcZ_Phi(circPar, sH2[0], sH2[1], fHitVector[prevHNo]->GetZ());
      }

      if (fVerbose > 3)
        if (fHitVectDI[prevHNo] != 2 && detId != 2) {
          cout << " combining hit " << fHitVectDI[prevHNo] << "." << fHitVectHN[prevHNo] << " with hit " << detId << "." << hitNo
               << " --> z_phi(1) = " << CalcZ_Phi(circPar, sH2[0], sH2[1], fHitVector[prevHNo]->GetZ()) << " --> z_phi(2) = " << CalcZ_Phi(circPar, sH1[0], sH1[1], thisHit->GetZ())
               << endl;
        }
      //	  cout << "and new z_phi = " << tp0.z_phi << endl;

      tp0.n = 2;

      if (fTN == -1) {
        TrackBasis tb0;
        tb0.trackPars.push_back(tp0);
        tb0.trackHits.push_back(fHitVector[prevHNo]);
        tb0.trackHitD.push_back(fHitVectDI[prevHNo]);
        tb0.trackHitN.push_back(fHitVectHN[prevHNo]);
        tb0.trackHits.push_back(thisHit);
        tb0.trackHitD.push_back(detId);
        tb0.trackHitN.push_back(hitNo);
        tb0.meanX = 0.;
        tb0.meanY = 0.;
        tb0.meanR = 0.;
        tb0.meanZ_Phi = 2.e6;

        fTN = fTracksVector.size();
        fTracksVector.push_back(tb0);

        hitsMatched = kTRUE;
        // 	    cout << "creating track " << fTracksVector.size() << " from hits "
        // 		 << fHitVectDI[prevHNo] << "." << fHitVectHN[prevHNo] << " and "
        // 		 << fHitDetId[hitN] << "." << fHitDetNo[hitN] << endl;
        if (fVerbose > 3) {
          cout << "!!! created track " << fTracksVector.size() - 1 << " with " << fHitVectDI[prevHNo] << "." << fHitVectHN[prevHNo] << endl;
          //	    if ( ihit == 54 ) {
          cout << "   >> with " << CalcPhi(tp0.x, tp0.y) << " (" << tp0.x << "," << tp0.y << ") " << tp0.r << " " << tp0.z_phi
               << endl; //"(" << CalcPhi(tp0.x-sH1[0],tp0.y-sH1[1]) << " on " << detHit->GetZ() << ")" << endl;
          // 	      Double_t tmpx = sH1[0]-tp0.x;
          // 	      Double_t tmpy = sH1[1]-tp0.y;
          // 	      cout << " should be: " << sH1[0] << " " << sH1[1] << " ---> " << tmpx << " " << tmpy << endl;
          // 	      Double_t rotA = TMath::Pi()-CalcPhi(tp0.x,tp0.y);
          // 	      cout << "rotate by " << rotA*TMath::RadToDeg() << " deg" << endl;
          // 	      Double_t tmpx2 = tmpx*TMath::Cos(rotA)-tmpy*TMath::Sin(rotA);
          // 	      Double_t tmpy2 = tmpx*TMath::Sin(rotA)+tmpy*TMath::Cos(rotA);
          // 	      cout << " and then : " << tmpx   << " " << tmpy   << " ---> " << tmpx2 << " " << tmpy2 << endl;
          // 	      cout << " and finally: " << CalcPhi(tmpx2,tmpy2) << endl;
        }
      } else {
        fTracksVector[fTN].trackPars.push_back(tp0);
        if (fVerbose > 3) {
          cout << "   >> with " << CalcPhi(tp0.x, tp0.y) << " (" << tp0.x << "," << tp0.y << ") " << tp0.r << " " << tp0.z_phi << "(" << CalcPhi(tp0.x - sH1[0], tp0.y - sH1[1])
               << " on " << thisHit->GetZ() << ")" << endl;
        }
      }
    }
  }

  if (hitsMatched)
    RemoveHitFromPreviousHits(prevHNo);
  return hitsMatched;
}
// -------------------------------------------------------------------------

// -----   Private method ExtractMeanRPhiFromTrack   -------------------------------
Bool_t PndBarrelTrackFinder::ExtractMeanRPhiFromTrack(Int_t trackNo)
{
  if (fVerbose > 3)
    cout << "extracting mean rphi from track " << trackNo << " with " << fTracksVector[trackNo].trackHits.size() << " hits." << endl;
  if (fTracksVector[trackNo].trackHits.size() < 4)
    return kFALSE;
  if (TMath::Abs(fTracksVector[trackNo].meanR) > 0.1)
    return kTRUE;

  Double_t meanPhi = 0.;
  Double_t meanRad = 0.;
  Double_t goodPNR = 0.;
  for (size_t ipar = 0; ipar < fTracksVector[trackNo].trackPars.size(); ipar++) {
    if (fTracksVector[trackNo].trackPars[ipar].n < 0.9 * fTracksVector[trackNo].trackHitD.size())
      continue;
    meanPhi += CalcPhi(fTracksVector[trackNo].trackPars[ipar].x, fTracksVector[trackNo].trackPars[ipar].y);
    meanRad += fTracksVector[trackNo].trackPars[ipar].r;
    goodPNR += 1.;
  }
  if (goodPNR < 0.5)
    return kFALSE;
  meanRad = meanRad / goodPNR;
  meanPhi = meanPhi / goodPNR;
  if (fVerbose > 3)
    cout << "mean rad phi is " << meanRad << " " << meanPhi << endl;

  Double_t sH1[3] = {meanRad * TMath::Cos(meanPhi), meanRad * TMath::Sin(meanPhi), meanRad};
  Double_t sH2[3] = {0., 0., 0.};

  Int_t nofCloseHits = 0;
  for (size_t ihit = 0; ihit < fTracksVector[trackNo].trackHits.size(); ihit++) {
    sH2[0] = fTracksVector[trackNo].trackHits[ihit]->GetX();
    sH2[1] = fTracksVector[trackNo].trackHits[ihit]->GetY();
    sH2[2] = 0.;
    if (fTracksVector[trackNo].trackHitD[ihit] == 2) {
      sH2[2] =  GetHitIsochrone(fTracksVector[trackNo].trackHits[ihit]);
    }

    if (FindCircDist(sH1, sH2) < fReasonableDist) {
      nofCloseHits++;
    }
  }
  if (fVerbose > 3)
    cout << "there are " << nofCloseHits << " close hits." << endl;
  if (nofCloseHits < 0.7 * fTracksVector[trackNo].trackHitD.size())
    return kFALSE;

  fTracksVector[trackNo].meanX = sH1[0];
  fTracksVector[trackNo].meanY = sH1[1];
  fTracksVector[trackNo].meanR = sH1[2];

  for (size_t ipar = 0; ipar < fTracksVector[trackNo].trackPars.size(); ipar++) {
    if (fTracksVector[trackNo].trackPars[ipar].n < 0.9 * fTracksVector[trackNo].trackHitD.size())
      continue;
    if (fTracksVector[trackNo].trackPars[ipar].z_phi > 1.e6)
      continue;
    fTracksVector[trackNo].trackPars[ipar].n = 400;
  }

  ExtractMeanZ_PhiFromTrack(trackNo);

  return kTRUE;
}
// -------------------------------------------------------------------------

// -----   Private method ExtractMeanZ_PhiFromTrack   -------------------------------
Bool_t PndBarrelTrackFinder::ExtractMeanZ_PhiFromTrack(Int_t trackNo)
{
    bool printInfo = false;
  if (fVerbose > 3 || printInfo)
    cout << "extracting mean Z_Phi from track " << trackNo << " with " << fTracksVector[trackNo].trackHits.size() << " hits." << endl;
  if (fTracksVector[trackNo].meanZ_Phi < 1.e6)
    return kTRUE;

  // count hits with z information
  Int_t nofZHits = 0;
  Double_t circPar[4] = {fTracksVector[trackNo].meanX, fTracksVector[trackNo].meanY, fTracksVector[trackNo].meanR,
                         CalcPhi(fTracksVector[trackNo].meanX, fTracksVector[trackNo].meanY)};

  std::vector<Double_t> trackHitsZ_Phi;

  for (size_t ihit = 0; ihit < fTracksVector[trackNo].trackHits.size(); ihit++) {
    if (fTracksVector[trackNo].trackHitD[ihit] != 2 && fTracksVector[trackNo].trackHitD[ihit] != 4 ) {
      nofZHits++;
      trackHitsZ_Phi.push_back(
        CalcZ_Phi(circPar, fTracksVector[trackNo].trackHits[ihit]->GetX(), fTracksVector[trackNo].trackHits[ihit]->GetY(), fTracksVector[trackNo].trackHits[ihit]->GetZ()));
    } else {
      
      if (IsSkewTube(GetTubeID(fTracksVector[trackNo].trackHits[ihit]))) {
        nofZHits++;

        // fetch tube position, direction and length
        // [0]-[2] = posX, posY, posZ, [3]=length/2, [4]-[6] = dirX, dirX, dirZ, [7] = 0.5*r_iso*(1+1/cos(theta_wire)) 
        Double_t tubePar[8];
        GetTubeParams(fTracksVector[trackNo].trackHits[ihit], tubePar);
        
        Double_t intReg[4];

        Int_t nofReg = FindInterestingRegions(circPar, tubePar, intReg);
        for (Int_t ireg = 0; ireg < nofReg; ireg++) {
          trackHitsZ_Phi.push_back(intReg[ireg]);
        }
      }
    }
  }
  if (nofZHits < 4)
    return kFALSE;

  if (fVerbose > 3 || printInfo) {
    cout << "trying to etract mean z_phi. There are " << nofZHits << " hits with Z information, parameters z_phi: " << flush;
  }

  Int_t nofClosePars = 0;
  Int_t highestNPars = 2;
  Int_t highestNPPar = -1;
  Double_t meanZ_Phi = 0.;
  Double_t sumOfDist = 0.;
  Double_t smallestSOD = 1000.;
  Double_t bestMeanZ_Phi = 0.;
  if (fVerbose > 3 || printInfo)
    cout << " got " << trackHitsZ_Phi.size() << " z_phi parameters: " << flush;
  for (size_t ipar = 0; ipar < trackHitsZ_Phi.size(); ipar++) {

    meanZ_Phi = 0.;
    sumOfDist = 0.;
    nofClosePars = 0;

    for (size_t ipar2 = 0; ipar2 < trackHitsZ_Phi.size(); ipar2++) {
      if (TMath::Abs(trackHitsZ_Phi[ipar] - trackHitsZ_Phi[ipar2]) > fReasonableZ_PhiD)
        continue;
      meanZ_Phi += trackHitsZ_Phi[ipar2];
      sumOfDist += TMath::Abs(trackHitsZ_Phi[ipar] - trackHitsZ_Phi[ipar2]);
      nofClosePars++;
    }

    if (nofClosePars >= highestNPars) {
      if (nofClosePars > highestNPars)
        smallestSOD = 1000.;
      if (sumOfDist < smallestSOD) {
        highestNPars = nofClosePars;
        highestNPPar = ipar;
        smallestSOD = sumOfDist;
        bestMeanZ_Phi = meanZ_Phi / ((Double_t)(nofClosePars));
      }
    }

    if (fVerbose > 3 || printInfo)
      cout << trackHitsZ_Phi[ipar] << "(" << ipar << "/" << nofClosePars << "/" << sumOfDist << ") " << endl;
  }
  if (fVerbose > 3 || printInfo) {
    cout << "best parameter number " << highestNPPar << " with " << highestNPars << " close parameters, sumOfDist = " << smallestSOD << " and meanZ_Phi = " << bestMeanZ_Phi << endl;
  }

  if (highestNPars < 3)
    return kFALSE;

  fTracksVector[trackNo].meanZ_Phi = bestMeanZ_Phi;

  return kTRUE;
}
// -------------------------------------------------------------------------

// -----   Private method PrintTracks   -------------------------------
void PndBarrelTrackFinder::PrintTracks()
{
  if (fTracksVector.size() > 0) {
    cout << "          --------- " << flush;
    for (Int_t idet = 0; idet < 4; idet++) {
      if (fIncludeDet[idet] == kTRUE) {
        cout << "\033[" << 91 + idet << "m" << fDetName[idet].Data() << "\033[0m " << flush;
        if (idet == 2)
          cout << "\033[33m(skewed)\033[0m " << flush;

        // 	cout << "\033[" << (skewedSttHit?33:91+fTracksVector[itr].trackHitD[ihit]) << "m"
        // 	     << fTracksVector[itr].trackHitN[ihit] << " "
        // 	     << "\033[0m" << flush;
      }
    }
    cout << "-------------" << endl;
  }

  for (size_t itr = 0; itr < fTracksVector.size(); itr++) {
    cout << "          |   track " << setw(2) << itr << ": " << flush;

    if (TMath::Abs(fTracksVector[itr].meanR) > 0.1) {
      for (size_t ihit = 0; ihit < fTracksVector[itr].trackHitD.size(); ihit++) {
        Bool_t skewedSttHit = kFALSE;
        
        if (fTracksVector[itr].trackHitD[ihit] == 2)
          skewedSttHit = IsSkewTube(GetTubeID(fTracksVector[itr].trackHits[ihit]));

        cout << "\033[" << (skewedSttHit ? 33 : 91 + fTracksVector[itr].trackHitD[ihit]) << "m" << fTracksVector[itr].trackHitN[ihit] << " "
             << "\033[0m" << flush;
      }
      cout << endl;

      Double_t thisPhi = CalcPhi(fTracksVector[itr].meanX, fTracksVector[itr].meanY);
      Double_t thisRad = fTracksVector[itr].meanR;
      Double_t thisZ_Phi = fTracksVector[itr].meanZ_Phi;
      if (TMath::IsNaN(thisZ_Phi))
        thisZ_Phi = 2.e6;

      Double_t calcPt = thisRad * fRadToMomConversion;
      Double_t calcPz = thisZ_Phi * fRadToMomConversion;
      Double_t calcP = TMath::Sqrt(calcPt * calcPt + calcPz * calcPz);
      Double_t calcThe = TMath::ACos(calcPz / calcP);

      Double_t calcPhi = thisPhi - TMath::Pi() / 2.;
      // Double_t calcChg = -1.; //[R.K. 01/2017] unused variable?
      if (calcPhi < 0.)
        calcPhi += TMath::Pi() * 2.;
      Double_t firpPhi = CalcPhi(fTracksVector[itr].trackHits[0]->GetX(), fTracksVector[itr].trackHits[0]->GetY());
      if (TMath::Abs(calcPhi - firpPhi) > TMath::Pi() / 2. && TMath::Abs(calcPhi - firpPhi) < 3. * TMath::Pi() / 2.) {
        calcPhi = thisPhi + TMath::Pi() / 2.;
        if (calcPhi > 2. * TMath::Pi())
          calcPhi -= TMath::Pi() * 2.;
        // calcChg = 1.; //[R.K. 01/2017] unused variable?
      }
      if (calcPhi > TMath::Pi())
        calcPhi -= 2. * TMath::Pi();

      TVector3 trackMomentum;
      trackMomentum.SetMagThetaPhi(calcP, calcThe, calcPhi);

      cout << "          | "
           << "\033[96m" << setw(7) << setprecision(6) << thisPhi << " " << setw(7) << setprecision(6) << thisRad << " " << setw(7) << setprecision(6) << thisZ_Phi << " " << setw(7)
           << setprecision(6) << calcPt << " " << setw(7) << setprecision(6) << calcPz << " " << setw(7) << setprecision(6) << calcP << " " << setw(7) << setprecision(6) << calcPhi
           << " " << setw(7) << setprecision(6) << calcThe << "\033[0m" << endl;
    }

    else {
      for (size_t ihit = 0; ihit < fTracksVector[itr].trackHitD.size(); ihit++)
        cout << setw(1) << fTracksVector[itr].trackHitD[ihit] << "." << fTracksVector[itr].trackHitN[ihit] << " " << flush;
      cout << endl;

      for (size_t ipar = 0; ipar < fTracksVector[itr].trackPars.size(); ipar++) {
        cout << "          |           /" << setw(2) << ipar << " " << setw(6) << setprecision(6)
             << CalcPhi(fTracksVector[itr].trackPars[ipar].x, fTracksVector[itr].trackPars[ipar].y) << " " << setw(6) << setprecision(6) << fTracksVector[itr].trackPars[ipar].r
             << " " << setw(6) << setprecision(6) << fTracksVector[itr].trackPars[ipar].z_phi << " " << setw(6) << setprecision(6) << fTracksVector[itr].trackPars[ipar].n << endl;
      }
    }

    cout << "          ------------------------------------------------------------" << endl;
  }
}
// -------------------------------------------------------------------------

// -----   Private method PrintTracks   -------------------------------
void PndBarrelTrackFinder::DrawTracks()
{
  for (Int_t iell = 0; iell < 1000; iell++) {
    fEllipse[iell]->SetX1(100);
    fEllipse[iell]->SetY1(100);
    fEllipse[iell]->SetR1(0);
    fEllipse[iell]->SetR2(0);
    fEllipse[iell]->Paint();
  }

  for (size_t itr = 0; itr < fTracksVector.size(); itr++) {
    if (TMath::Abs(fTracksVector[itr].meanR) > 0.1) {
      //for (size_t ihit = 0; ihit < fTracksVector[itr].trackHitD.size(); ihit++) {
        //// Bool_t skewedSttHit = kFALSE; //[R.K. 01/2017] unused variable?
        //if (fTracksVector[itr].trackHitD[ihit] == 2 || fTracksVector[itr].trackHitD[ihit] == 4) {
          //Int_t iTube = ((PndSttHit *)fTracksVector[itr].trackHits[ihit])->GetTubeID();
          //PndSttTube *sttTube = nullptr;
          //if (fGeoType == 1)
            //sttTube = (PndSttTube *)fTubeArray->At(iTube);
          //else if (fGeoType == 2) {
            //sttTube = PndSttTubeMap::Instance()->GetTube(iTube);
          //}
          //if (sttTube->GetWireDirection().Z() < 1.) {
            //// skewedSttHit = kTRUE; //[R.K. 01/2017] unused variable?
          //}
        //}
      //}

      // Double_t thisPhi = CalcPhi(fTracksVector[itr].meanX,
      // fTracksVector[itr].meanY); //[R.K. 01/2017] unused variable?
      // Double_t thisRad =         fTracksVector[itr].meanR; //[R.K. 01/2017] unused variable?

      fEllipse[itr]->SetX1(fTracksVector[itr].meanX);
      fEllipse[itr]->SetY1(fTracksVector[itr].meanY);
      fEllipse[itr]->SetR1(fTracksVector[itr].meanR);
      fEllipse[itr]->SetR2(fTracksVector[itr].meanR);
      fEllipse[itr]->SetFillStyle(0);
      fEllipse[itr]->SetLineWidth(2);
      fEllipse[itr]->SetLineColor(51 + 2 * itr);
      fEllipse[itr]->Paint();
    }
  }
}
// -------------------------------------------------------------------------

// -----   Private method CleanTracks   -------------------------------
Int_t PndBarrelTrackFinder::CleanTracks()
{
  return -1;
}
// -------------------------------------------------------------------------

// -----   Private method RemoveShortTracks   -------------------------------
void PndBarrelTrackFinder::RemoveShortTracks()
{
  for (Int_t itr = fTracksVector.size() - 1; itr >= 0; itr--) {
    if (TMath::Abs(fTracksVector[itr].meanZ_Phi) < 1.e6 && TMath::Abs(fTracksVector[itr].meanR) > 1.)
      continue;
    for (size_t ihit = 0; ihit < fTracksVector[itr].trackHits.size(); ihit++) {
      AddHitToPreviousHits(fTracksVector[itr].trackHits[ihit], fTracksVector[itr].trackHitD[ihit], fTracksVector[itr].trackHitN[ihit]);
    }
    for (size_t intr = itr; intr < fTracksVector.size() - 1; intr++) {
      fTracksVector[intr] = fTracksVector[intr + 1];
    }
    fTracksVector.pop_back();
  }
}
// -------------------------------------------------------------------------

// -----   Private method RemoveCloneTracks   -------------------------------
void PndBarrelTrackFinder::RemoveCloneTracks()
{
  Bool_t printInfo = kFALSE;
  for (Int_t itr1 = fTracksVector.size() - 1; itr1 >= 0; itr1--) {
    for (Int_t itr2 = fTracksVector.size() - 1; itr2 > itr1; itr2--) {
      fClonesXDiff->Fill(fTracksVector[itr1].meanX - fTracksVector[itr2].meanX);
      fClonesYDiff->Fill(fTracksVector[itr1].meanY - fTracksVector[itr2].meanY);
      fClonesRDiff->Fill(fTracksVector[itr1].meanR - fTracksVector[itr2].meanR);
      fClonesZ_PhiDiff->Fill(fTracksVector[itr1].meanZ_Phi - fTracksVector[itr2].meanZ_Phi);
      fClonesXDiffRel->Fill((fTracksVector[itr1].meanX - fTracksVector[itr2].meanX) / fTracksVector[itr2].meanR);
      fClonesYDiffRel->Fill((fTracksVector[itr1].meanY - fTracksVector[itr2].meanY) / fTracksVector[itr2].meanR);
      fClonesRDiffRel->Fill((fTracksVector[itr1].meanR - fTracksVector[itr2].meanR) / fTracksVector[itr2].meanR);
      fClonesZ_PhiDiffRel->Fill((fTracksVector[itr1].meanZ_Phi - fTracksVector[itr2].meanZ_Phi) / fTracksVector[itr2].meanZ_Phi);
    }
  }

  for (Int_t itr1 = fTracksVector.size() - 1; itr1 >= 0; itr1--) {
    Double_t tr1Phi = CalcPhi(fTracksVector[itr1].meanX, fTracksVector[itr1].meanY);
    Int_t nofTr1Hits = fTracksVector[itr1].trackHitD.size();
    for (Int_t itr2 = fTracksVector.size() - 1; itr2 > itr1; itr2--) {
      Double_t tr2Phi = CalcPhi(fTracksVector[itr2].meanX, fTracksVector[itr2].meanY);

      //       if (           (mcMag-reMag) <  0.3*mcMag &&
      // 		     (mcMag-reMag) > -0.3*mcMag &&
      // 	   TMath::Abs(mcPhi-rePhi) <  5. &&
      // 	   TMath::Abs(mcThe-reThe) <  5. ) {

      //       if ( TMath::Abs(fTracksVector[itr1].meanR  -fTracksVector[itr2].meanR  ) < 0.1*fTracksVector[itr1].meanR &&
      // 	   TMath::Abs(fTracksVector[itr1].meanZ_Phi-fTracksVector[itr2].meanZ_Phi) < fReasonableZ_PhiD+0.05*TMath::Abs(fTracksVector[itr1].meanZ_Phi) &&
      // 	   TMath::Abs(tr1Phi-tr2Phi) < 0.1*TMath::RadToDeg() ) {
      if (TMath::Abs(fTracksVector[itr1].meanX - fTracksVector[itr2].meanX) < 0.2 * TMath::Abs(fTracksVector[itr1].meanR) &&
          TMath::Abs(fTracksVector[itr1].meanY - fTracksVector[itr2].meanY) < 0.2 * TMath::Abs(fTracksVector[itr1].meanR) &&
          TMath::Abs(fTracksVector[itr1].meanR - fTracksVector[itr2].meanR) < 0.3 * fTracksVector[itr1].meanR &&
          TMath::Abs(fTracksVector[itr1].meanZ_Phi - fTracksVector[itr2].meanZ_Phi) < 0.4 * TMath::Abs(fTracksVector[itr1].meanZ_Phi) &&
          TMath::Abs(tr1Phi - tr2Phi) < 0.2 * TMath::RadToDeg()) {
        if (fVerbose > 0 || printInfo) {
          cout << "seems that tracks " << itr1 << " (" << fTracksVector[itr1].meanX << "," << fTracksVector[itr1].meanY << "," << fTracksVector[itr1].meanR << ","
               << fTracksVector[itr1].meanZ_Phi << " )" << endl;
          cout << "              and " << itr2 << " (" << fTracksVector[itr2].meanX << "," << fTracksVector[itr2].meanY << "," << fTracksVector[itr2].meanR << ","
               << fTracksVector[itr2].meanZ_Phi << " ) ARE THE SAME!" << endl;
        }

        Int_t nofTr2Hits = fTracksVector[itr2].trackHitD.size();
        for (Int_t ith2 = 0; ith2 < nofTr2Hits; ith2++) {
          Bool_t hitExists = kFALSE;
          for (Int_t ith1 = 0; ith1 < nofTr1Hits; ith1++) {
            if (fTracksVector[itr1].trackHitD[ith1] == fTracksVector[itr2].trackHitD[ith2] && fTracksVector[itr1].trackHitN[ith1] == fTracksVector[itr2].trackHitN[ith2]) {
              hitExists = kTRUE;
              break;
            }
          }
          if (hitExists)
            continue;
          fTracksVector[itr1].trackHits.push_back(fTracksVector[itr2].trackHits[ith2]);
          fTracksVector[itr1].trackHitD.push_back(fTracksVector[itr2].trackHitD[ith2]);
          fTracksVector[itr1].trackHitN.push_back(fTracksVector[itr2].trackHitN[ith2]);
          //	  cout << endl;
        }
        for (size_t intr = itr2; intr < fTracksVector.size() - 1; intr++) {
          fTracksVector[intr] = fTracksVector[intr + 1];
        }
        fTracksVector.pop_back();
      }
    }
  }
}
// -------------------------------------------------------------------------

// -----   Private method WriteTracks   -------------------------------
Int_t PndBarrelTrackFinder::WriteTracks()
{
  Int_t nofCreatedTracks = 0;

  for (size_t itr = 0; itr < fTracksVector.size(); itr++) {
    Double_t thisPhi = CalcPhi(fTracksVector[itr].meanX, fTracksVector[itr].meanY);
    Double_t thisRad = fTracksVector[itr].meanR;
    Double_t thisZ_Phi = fTracksVector[itr].meanZ_Phi;
    if (TMath::IsNaN(thisZ_Phi))
      thisZ_Phi = 2.e6;

    //       for ( Int_t ihit = 0 ; ihit < fTracksVector[itr].trackHitD.size() ; ihit++ )
    // 	cout << fTracksVector[itr].trackHitD[ihit] << "." << fTracksVector[itr].trackHitN[ihit] << " " << flush;
    //       cout << endl;
    Double_t calcPt = thisRad * fRadToMomConversion;
    Double_t calcPz = thisZ_Phi * fRadToMomConversion;
    Double_t calcP = TMath::Sqrt(calcPt * calcPt + calcPz * calcPz);
    Double_t calcThe = TMath::ACos(calcPz / calcP);

    Double_t calcPhi = thisPhi - TMath::Pi() / 2.;
    Int_t calcChg = -1;
    if (calcPhi < 0.)
      calcPhi += TMath::Pi() * 2.;
    Double_t firpPhi = CalcPhi(fTracksVector[itr].trackHits[0]->GetX(), fTracksVector[itr].trackHits[0]->GetY());
    if (TMath::Abs(calcPhi - firpPhi) > TMath::Pi() / 2. && TMath::Abs(calcPhi - firpPhi) < 3. * TMath::Pi() / 2.) {
      calcPhi = thisPhi + TMath::Pi() / 2.;
      if (calcPhi > 2. * TMath::Pi())
        calcPhi -= TMath::Pi() * 2.;
      calcChg = 1;
    }
    if (calcPhi > TMath::Pi())
      calcPhi -= 2. * TMath::Pi();

    //    cout << "F " << thisPhi << " " << thisRad << " " << thisZ_Phi << "    " << calcPt << "  " << calcPhi << " " << calcPz << " " << calcP << " " << calcPhi << " " << calcThe <<
    //    endl;//flush;

    //    cout << "                      MTP: " << calcP << " " << calcPhi << " " << calcThe << " and charge = " << calcChg << flush;

    TVector3 trackPosition(0., 0., 0.);
    TVector3 trackMomentum;
    trackMomentum.SetMagThetaPhi(calcP, calcThe, calcPhi);
    TVector3 lastTrackMom;

    PndTrackCand *trackCand = new ((*fBarrelTrackCandArray)[nofCreatedTracks]) PndTrackCand();

    std::vector<Int_t> trackHitsPerDet(4, 0);
    //    cout << "TRACK " << itr << " (" << fTracksVector[itr].meanX << "/" << fTracksVector[itr].meanY << "/" << fTracksVector[itr].meanR << "/" << fTracksVector[itr].meanZ_Phi << ") -> " << trackMomentum.X() << ", " << trackMomentum.Y() << ", " << trackMomentum.Z() << endl;

    double maxAngleDiff = 0.;
    int    maxAngleDHit = -1;

    for (size_t ihit = 0; ihit < fTracksVector[itr].trackHitD.size(); ihit++) {
      trackHitsPerDet[fTracksVector[itr].trackHitD[ihit]] += 1;
      Double_t tempPos = TMath::Sqrt(fTracksVector[itr].trackHits[ihit]->GetX() * fTracksVector[itr].trackHits[ihit]->GetX() +
                                     fTracksVector[itr].trackHits[ihit]->GetY() * fTracksVector[itr].trackHits[ihit]->GetY() +
                                     fTracksVector[itr].trackHits[ihit]->GetZ() * fTracksVector[itr].trackHits[ihit]->GetZ());
      double pointA =
          TMath::ATan2(fTracksVector[itr].trackHits[ihit]->GetY()-fTracksVector[itr].meanY,
                       fTracksVector[itr].trackHits[ihit]->GetX()-fTracksVector[itr].meanX) -
          TMath::ATan2(0.-fTracksVector[itr].meanY,
                       0.-fTracksVector[itr].meanX);
      if ( pointA >  TMath::Pi() ) pointA -= TMath::Pi()*2.;
      if ( pointA < -TMath::Pi() ) pointA += TMath::Pi()*2.;
      if ( TMath::Abs(pointA) > TMath::Abs(maxAngleDiff) ) {
          maxAngleDiff = pointA;
          maxAngleDHit = ihit;
      }
      /*     float s = TMath::Sin(pointA);
      float c = TMath::Cos(pointA);
    
      lastTrackMom.SetMagThetaPhi(calcP, calcThe,
                                  TMath::ATan2(fTracksVector[itr].trackHits[ihit]->GetY()-fTracksVector[itr].meanY,
                                               fTracksVector[itr].trackHits[ihit]->GetX()-fTracksVector[itr].meanX)
                                  + (pointA>0.?+1.:-1.) * TMath::Pi()/2.);
      double pointX = -fTracksVector[itr].meanX * c + fTracksVector[itr].meanY * s + fTracksVector[itr].meanX;
      double pointY = -fTracksVector[itr].meanX * s - fTracksVector[itr].meanY * c + fTracksVector[itr].meanY;
      double pointZ = TMath::Abs(pointA)*fTracksVector[itr].meanZ_Phi;
      cout << " -> hit " << ihit << " in " << fDetType[fTracksVector[itr].trackHitD[ihit]] << " at "
           << fTracksVector[itr].trackHits[ihit]->GetX() << ", "
           << fTracksVector[itr].trackHits[ihit]->GetY() << ", "
           << fTracksVector[itr].trackHits[ihit]->GetZ() << " -----> "
           << pointA*TMath::RadToDeg() << " -----> "
           << pointX << "," 
           << pointY << "," 
           << pointZ << " ---> p= "
           << lastTrackMom.X() << ","
           << lastTrackMom.Y() << ","
           << lastTrackMom.Z() << ","
           << endl;*/
      
      //       cout << ">>> " << fDetName[fTracksVector[itr].trackHitD[ihit]] << " hit will be written as "
      // 	   << fDetType[fTracksVector[itr].trackHitD[ihit]] << "." << fTracksVector[itr].trackHitN[ihit] << endl;
      trackCand->AddHit(FairRootManager::Instance()->GetBranchId(fHitArrayName[fTracksVector[itr].trackHitD[ihit]].Data()), fTracksVector[itr].trackHitN[ihit], tempPos);
    }
    
    float s = TMath::Sin(maxAngleDiff);
    float c = TMath::Cos(maxAngleDiff);
    double pointX = -fTracksVector[itr].meanX * c + fTracksVector[itr].meanY * s + fTracksVector[itr].meanX;
    double pointY = -fTracksVector[itr].meanX * s - fTracksVector[itr].meanY * c + fTracksVector[itr].meanY;
    double pointZ = TMath::Abs(maxAngleDiff)*fTracksVector[itr].meanZ_Phi;
    TVector3 lastTrackPos(pointX,pointY,pointZ);
    lastTrackMom.SetMagThetaPhi(calcP, calcThe,
                                TMath::ATan2(fTracksVector[itr].trackHits[maxAngleDHit]->GetY()-fTracksVector[itr].meanY,
                                             fTracksVector[itr].trackHits[maxAngleDHit]->GetX()-fTracksVector[itr].meanX)
                                + (maxAngleDiff>0.?+1.:-1.) * TMath::Pi()/2.);
    /*    cout << " LAST HIT [" << fTracksVector[itr].trackHitD[maxAngleDHit] << "] = "
         << fTracksVector[itr].trackHits[maxAngleDHit]->GetX() << ", "
         << fTracksVector[itr].trackHits[maxAngleDHit]->GetY() << ", "
         << fTracksVector[itr].trackHits[maxAngleDHit]->GetZ() << endl;         
    cout << " CALCULATED = "
         << pointX << ", "
         << pointY << ", "
         << pointZ << endl;*/

    if (fDrawHistos) {
      int histDet = fTracksVector[itr].trackHitD[maxAngleDHit];
      if ( histDet == 3 ) histDet = 4;
      if (fTracksVector[itr].trackHitD[maxAngleDHit] == 2) {
        if (IsSkewTube(GetTubeID(fTracksVector[itr].trackHits[maxAngleDHit])))
          histDet = 3;

      }
      fhHitDist[histDet][0]->Fill(pointX-fTracksVector[itr].trackHits[maxAngleDHit]->GetX());
      fhHitDist[histDet][1]->Fill(pointY-fTracksVector[itr].trackHits[maxAngleDHit]->GetY());
      fhHitDist[histDet][2]->Fill(pointZ-fTracksVector[itr].trackHits[maxAngleDHit]->GetZ());
    }
        
    // cout << " LAST HIT IS HIT " << maxAngleDHit << " in " << fDetType[fTracksVector[itr].trackHitD[maxAngleDHit]] << " at "
    //      << fTracksVector[itr].trackHits[maxAngleDHit]->GetX() << ", "
    //      << fTracksVector[itr].trackHits[maxAngleDHit]->GetY() << ", "
    //      << fTracksVector[itr].trackHits[maxAngleDHit]->GetZ() << " -----> "
    //      << maxAngleDiff << " -----> p= "
    //      << lastTrackMom.X() << ","
    //      << lastTrackMom.Y() << ","
    //      << lastTrackMom.Z() << ","
    //      << endl;
        
    trackCand->Sort();

    /*   cout << "track hits in :" << flush;
    for ( Int_t itch = 0 ; itch < trackCand->GetNHits() ; itch++ ) {
        cout << trackCand->GetSortedHit(itch).GetDetId() << " " << flush;
    }
    cout << endl;*/

    FairTrackParP firstPar(trackPosition, trackMomentum, TVector3(0.5, 0.5, 0.5), 0.1 * trackMomentum, calcChg, trackPosition, TVector3(1., 0., 0.), TVector3(0., 1., 0.));

    //    FairTrackParP*	    lastPar = new FairTrackParP(trackPosition,trackMomentum,
    FairTrackParP lastPar(lastTrackPos, lastTrackMom, TVector3(0.5, 0.5, 0.5), 0.1 * trackMomentum, calcChg, trackPosition, TVector3(1., 0., 0.), TVector3(0., 1., 0.));

    new ((*fBarrelTrackArray)[nofCreatedTracks]) PndTrack(firstPar, lastPar, *trackCand);

    nofCreatedTracks++;

    //    cout << endl;
  }

  return nofCreatedTracks;
}
// -------------------------------------------------------------------------

// -----   Private method RemoveHitFromPreviousHits   -------------------------------
void PndBarrelTrackFinder::RemoveHitFromPreviousHits(Int_t hitNo)
{
  //    cout << endl << "There are " << fTracksVector.size() << " segments in the event" << endl;
  for (size_t irh = hitNo; irh < fHitVector.size() - 1; irh++) {
    fHitVector[irh] = fHitVector[irh + 1];
    fHitVectDI[irh] = fHitVectDI[irh + 1];
    fHitVectHN[irh] = fHitVectHN[irh + 1];
  }
  fHitVector.pop_back();
  fHitVectDI.pop_back();
  fHitVectHN.pop_back();
}
// -------------------------------------------------------------------------

// -----   Private method AddHitToPreviousHits   -------------------------------
void PndBarrelTrackFinder::AddHitToPreviousHits(FairHit *thisHit, Int_t detId, Int_t hitNo)
{
  if (fVerbose > 3)
    cout << "!!! ADDING HIT " << detId << "." << hitNo << endl;
  fHitVector.push_back(thisHit);
  fHitVectDI.push_back(detId);
  fHitVectHN.push_back(hitNo);
}
// -------------------------------------------------------------------------

// -----   Private method FindCircPar   -------------------------------
// the method finds a circle cl tangent to the three given circles c1, c2, c3 (circle given by x,y,r)
// parameter cno (from 0 to 7) is a bit mask on whether a given circle is inside or outside circle cl
Bool_t PndBarrelTrackFinder::FindCircPar(Double_t *c1, Double_t *c2, Double_t *c3, Int_t cno, Double_t *cl)
{
  if (cno < 0 || cno > 7)
    return kFALSE;
  //   cout << "c1 : " << c1[0] << "," << c1[1] << " - " << c1[2] << endl;
  //   cout << "c2 : " << c2[0] << "," << c2[1] << " - " << c2[2] << endl;
  //   cout << "c3 : " << c3[0] << "," << c3[1] << " - " << c3[2] << endl;
  Double_t s1 = -1 + 2 * (cno % 2);
  cno /= 2;
  Double_t s2 = -1 + 2 * (cno % 2);
  cno /= 2;
  Double_t s3 = -1 + 2 * (cno % 2);

  Double_t a2 = 2. * (c1[0] - c2[0]);
  Double_t b2 = 2. * (c1[1] - c2[1]);
  Double_t e2 = 2. * (s1 * c1[2] - s2 * c2[2]);
  Double_t d2 = (c1[0] * c1[0] + c1[1] * c1[1] - c1[2] * c1[2]) - (c2[0] * c2[0] + c2[1] * c2[1] - c2[2] * c2[2]);

  Double_t a3 = 2. * (c1[0] - c3[0]);
  Double_t b3 = 2. * (c1[1] - c3[1]);
  Double_t e3 = 2. * (s1 * c1[2] - s3 * c3[2]);
  Double_t d3 = (c1[0] * c1[0] + c1[1] * c1[1] - c1[2] * c1[2]) - (c3[0] * c3[0] + c3[1] * c3[1] - c3[2] * c3[2]);
  //  cout << "a2 " << a2 << " a3 " << a3 << " b2 " << b2 << " b3 " << b3 << " /// a2*b3-a3*b2 = " << a2*b3-a3*b2 << endl;
  if (TMath::Abs(a2 * b3 - a3 * b2) < 0.001)
    return kFALSE;

  Double_t ax = (d2 * b3 - d3 * b2) / (a2 * b3 - a3 * b2);
  Double_t bx = (e2 * b3 - e3 * b2) / (a2 * b3 - a3 * b2);
  Double_t ay = (a2 * d3 - a3 * d2) / (a2 * b3 - a3 * b2);
  Double_t by = (a2 * e3 - a3 * e2) / (a2 * b3 - a3 * b2);

  Double_t sa = bx * bx + by * by - 1.;
  Double_t sb = 2. * bx * (ax - c1[0]) + 2. * by * (ay - c1[1]) + 2. * s1 * c1[2];
  Double_t sc = (ax - c1[0]) * (ax - c1[0]) + (ay - c1[1]) * (ay - c1[1]) - c1[2] * c1[2];
  if (sa == 0)
    return kFALSE;
  Double_t dt = sb * sb - 4. * sa * sc;
  if (dt < 0)
    return kFALSE;

  cl[2] = (-sb - TMath::Sqrt(dt)) / (2. * sa);
  cl[0] = ax + bx * cl[2];
  cl[1] = ay + by * cl[2];
  cl[2] = TMath::Abs(cl[2]);
  if (TMath::IsNaN(cl[2])) {
    return kFALSE;
  }
  return kTRUE;
}
// -------------------------------------------------------------------------

// -----   Private method FindCircDist   -------------------------------
Double_t PndBarrelTrackFinder::FindCircDist(Double_t *c1, Double_t *c2)
{
  Double_t circDist = TMath::Sqrt((c1[0] - c2[0]) * (c1[0] - c2[0]) + (c1[1] - c2[1]) * (c1[1] - c2[1]));
  if (circDist >= c1[2] + c2[2]) {
    return circDist - c1[2] - c2[2];
  }
  if (circDist <= TMath::Abs(c1[2] - c2[2])) {
    return TMath::Abs(c1[2] - c2[2]) - circDist;
  }
  if (c1[2] > c2[2]) {
    return c2[2] - TMath::Abs(circDist - c1[2]);
  }
  return c1[2] - TMath::Abs(circDist - c2[2]);
}
// -------------------------------------------------------------------------

// -----   Private method CalcPhi   ----------------------------------------
Int_t PndBarrelTrackFinder::FindInterestingRegions(Double_t *circ, Double_t *tube, Double_t *reg)
{
  Double_t ax = tube[4] / tube[6];
  Double_t ay = tube[5] / tube[6];
  Double_t a = ax * ax + ay * ay;
  Double_t bx = (tube[0] - ax * tube[2] - circ[0]);
  Double_t by = (tube[1] - ay * tube[2] - circ[1]);
  Double_t b = 2. * ax * bx + 2. * ay * by;
  Double_t c = bx * bx + by * by;

  Double_t delta[2];
  delta[0] = b * b - 4. * a * (c - (circ[2] + tube[7]) * (circ[2] + tube[7]));
  delta[1] = b * b - 4. * a * (c - (circ[2] - tube[7]) * (circ[2] - tube[7]));

  Int_t nofReg = 0;

  for (Int_t idel = 0; idel < 2; idel++) {
    if (delta[idel] < 0)
      continue;
    Double_t z[2];
    z[0] = (-b - TMath::Sqrt(delta[idel])) / 2. / a;
    z[1] = (-b + TMath::Sqrt(delta[idel])) / 2. / a;
    //    cout << " tube crosses circle in " << z[0] << " or " << z[1] << endl;

    for (Int_t iz = 0; iz < 2; iz++) {
      Double_t z_phi = 2.e6;
      if (TMath::Abs(z[iz] - tube[2]) > tube[3])
        continue;
      //      cout << "checking z = " << z[iz] << endl;
      Double_t x = tube[0] + ax * (z[iz] - tube[2]);
      Double_t y = tube[1] + ay * (z[iz] - tube[2]);
      Double_t xd = x - circ[0];
      Double_t yd = y - circ[1];
      Double_t rd = TMath::Sqrt(xd * xd + yd * yd);
      Double_t pointPhi = TMath::ACos(xd / rd);
      if (yd < 0)
        pointPhi = 2. * TMath::Pi() - pointPhi;
      pointPhi += TMath::Pi();
      if (pointPhi > 2. * TMath::Pi())
        pointPhi -= 2. * TMath::Pi();
      Double_t phiDiffr = (pointPhi - circ[3]);
      if (phiDiffr > TMath::Pi())
        phiDiffr -= 2. * TMath::Pi();
      if (phiDiffr < -TMath::Pi())
        phiDiffr += 2. * TMath::Pi();
      z_phi = z[iz] / TMath::Abs(phiDiffr);
      //      cout << "the z_phi = " << z_phi << " cause dz = " << z[iz] << " and dphi = " << phiDiffr << endl;
      //      cout << "got " << z_phi << " while from other calc. it is: " << CalcZ_Phi(circ,x,y,z[iz]) << endl;
      reg[nofReg] = z_phi;
      nofReg++;
    }
  }
  return nofReg;
}
// -------------------------------------------------------------------------

// -----   Private method CalcPhi   ----------------------------------------
Double_t PndBarrelTrackFinder::CalcPhi(Double_t x, Double_t y)
{
  Double_t r = TMath::Sqrt(x * x + y * y);
  Double_t p = TMath::ACos(x / r);
  if (y < 0)
    p = 2. * TMath::Pi() - p;
  return p;
}
// -------------------------------------------------------------------------

// -----   Private method CalcZ_Phi   ----------------------------------------
Double_t PndBarrelTrackFinder::CalcZ_Phi(Double_t *circ, Double_t hx, Double_t hy, Double_t hz)
{
  Double_t tmpx = hx - circ[0];
  Double_t tmpy = hy - circ[1];
  Double_t rotA = TMath::Pi() - CalcPhi(circ[0], circ[1]);
  Double_t tmpx2 = tmpx * TMath::Cos(rotA) - tmpy * TMath::Sin(rotA);
  Double_t tmpy2 = tmpx * TMath::Sin(rotA) + tmpy * TMath::Cos(rotA);
  Double_t phiT = CalcPhi(tmpx2, tmpy2);
  if (phiT > TMath::Pi())
    phiT = -(2. * TMath::Pi() - phiT);
  return hz / TMath::Abs(phiT);
  // Double_t phiT = CalcPhi(hx-circ[0],hy-circ[1]);
  //  return phiT/hz;
}
// -------------------------------------------------------------------------

// -----   Private method SetParContainers   -------------------------------
void PndBarrelTrackFinder::SetParContainers()
{

  // Get run and runtime database
  FairRunAna *run = FairRunAna::Instance();
  if (!run)
    LOG(fatal) << "SetParContainers: No analysis run";

  FairRuntimeDb *db = run->GetRuntimeDb();
  if (!db)
    LOG(fatal) << "SetParContainers: No runtime database";

  // just load the correct set of parameters
  //if (fSttVer==0)
    fSttParameters = (PndGeoSttPar *)db->getContainer("PndGeoSttPar");
  //else
    //fStt2Parameters = (PndStt2GeoPar *)db->getContainer("PndStt2GeoPar");

  LOG(info) << " PndBarrelTrackFinder::SetParContainers().";
}
// -------------------------------------------------------------------------

// -----   Private method Init   -------------------------------------------
InitStatus PndBarrelTrackFinder::Init()
{
  //  fTrackSegments.clear();

  // Get and check FairRootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    cout << "-E- " << GetName() << "::Init: "
         << "RootManager not instantised!" << endl;
    return kERROR;
  }

  // Get the pointer to the singleton FairRunAna object
  FairRunAna *ana = FairRunAna::Instance();
  if (nullptr == ana) {
    cout << "-E- " << GetName() << "::Init :"
         << " no FairRunAna object!" << endl;
    return kERROR;
  }
  // Get the pointer to run-time data base
  FairRuntimeDb *rtdb = ana->GetRuntimeDb();
  if (nullptr == rtdb) {
    cout << "-E- " << GetName() << "::Init :"
         << " no runtime database!" << endl;
    return kERROR;
  }

  FairField *Field = ana->GetField();

  double po[3], BB[3];
  po[0] = 0.;
  po[1] = 0.;
  po[2] = 0.;

  Field->GetFieldValue(po, BB); // return value in KG (G3)
  // fBFIELD = BB[2] / 10.;        // value in Tesla;
  fRadToMomConversion = 1.602 * BB[2]/10. / 100. / 5.36; // q = 1.602e-19C, [B] = T, [r] = cm, 1GeV/c = 5.36e-19kgm/s
  cout << " -I - PndBarrelTrackFinder: FIELD[2] at 0,0,0 is " << BB[2] / 10. << "T, so conversion factor is set to " << fRadToMomConversion << endl;

  fDetName[0] = "MVD Pixel";
  fDetName[1] = "MVD Strip";
  fDetName[2] = "STT";//(fSttVer==0 ? "STT" : "STT2");
  fDetName[3] = "GEM";

  if (!fRunTimeBased) {
    fHitArrayName[0] = "MVDHitsPixel";
    fHitArrayName[1] = "MVDHitsStrip";
    fHitArrayName[2] = "STTHit";//(fSttVer==0 ? "STTHit" : "STT2Hit");
    fHitArrayName[3] = "GEMHit";
  }
  if (fRunTimeBased) {
    fHitArrayName[0] = "MVDSortedHitsPixels";
    fHitArrayName[1] = "MVDSortedHitsStrips";
    fHitArrayName[2] = "STTSortedHits";//(fSttVer==0 ? "STTSortedHits" : "STT2SortedHits");
    fHitArrayName[3] = "GEMSortedHits";
  }

  fDetType[0] = static_cast<int>(DetectorType::kMVDHitsPixel); // FIXME: Use FairRootManager::Instance()->GetBranchId()
  fDetType[1] = static_cast<int>(DetectorType::kMVDHitsStrip); // FIXME: Use FairRootManager::Instance()->GetBranchId()
  fDetType[2] = static_cast<int>(DetectorType::kSttHit);
  fDetType[3] = static_cast<int>(DetectorType::kGemHit);

  std::string histDet[5] = {"MVDPixel","MVDStrip","STTParallel","STTSkewed","GEM"};
  char   histDim[3] = {'X','Y','Z'};
  for (Int_t idet = 0; idet < 5 ; idet++) {
      for ( int idim = 0 ; idim < 3 ; idim++ ) {
          fhHitDist[idet][idim] = new TH1F(Form("histDist_%s_%c",histDet[idet].c_str(),histDim[idim]),
                                           Form("histDist_%s_%c",histDet[idet].c_str(),histDim[idim]),
                                           1000,-10.,10.);//,1000,-10.,10.);
      }
  }
  
  for (Int_t idet = 0; idet < 4; idet++) {
    if (fIncludeDet[idet] == kTRUE) {
      fHitArray[idet] = (TClonesArray *)ioman->GetObject(fHitArrayName[idet].Data());

      if (!fHitArray[idet]) {
        LOG(error) << " " << GetName() << "::Init: No " << fHitArrayName[idet].Data() << " array!";
        return kERROR;
      }
      LOG(info) << " " << GetName() << ": " << fHitArrayName[idet].Data() << " array found";
    }
  }

  // Create and register PndTrack and PndTrackCand arrays
  fBarrelTrackArray = new TClonesArray("PndTrack", 100);
  ioman->Register("BarrelTrack", "Barrel Track", fBarrelTrackArray, GetPersistency());

  fBarrelTrackCandArray = new TClonesArray("PndTrackCand", 100);
  ioman->Register("BarrelTrackCand", "Barrel TrackCand", fBarrelTrackCandArray, GetPersistency());

  LOG(info) << " " << GetName() << ": Initialization successfull";
  std::cout << "-I- " << GetName() << ": Looking for tracks in " << flush;
  for (Int_t idet = 0; idet < 4; idet++) {
    if (fIncludeDet[idet] == kFALSE)
      continue;
    cout << fDetName[idet].Data() << ", ";
  }
  cout << "\b\b. " << endl;
  cout << "================================================================================" << endl;


  // Fill TubeArray 
  // geoType=1, 3 (.geo)
  fTubeArray = PndStt2GeoHandler::Instance(fSttParameters)->GetTubeArray();
  //PndStt2GeoHandler geoH(fSttParameters);
  //fTubeArray = geoH.GetTubeArray();

  // geoType=2: (ROOT geometry) no TubeArray is filled, but the singleton PndSttTubeMap::Instance() is used
  
  //   cout << "****************************************************" << endl;
  //   cout << "**  " << fTubeArray->GetEntriesFast() << "  TUBES  ***********************************" << endl;
  //   cout << "****************************************************" << endl;

  // DRAW
  fCanvas = nullptr;
  if (fDrawDetails)
    fCanvas = new TCanvas("AnimatedCanvas", "AnimatedCanvas", 10, 10, 800, 800);

  if (fCanvas) {
    for (Int_t iell = 0; iell < 1000; iell++) {
      fEllipse[iell] = new TEllipse(100, 100, 0);
    }
    fStageDesc = new TLatex(0, 0, "Init");
  }

  fClonesXDiff = new TH1F("fClonesXDiff", "fClonesXDiff", 20000, -100., 100.);
  fClonesYDiff = new TH1F("fClonesYDiff", "fClonesYDiff", 20000, -100., 100.);
  fClonesRDiff = new TH1F("fClonesRDiff", "fClonesRDiff", 20000, -100., 100.);
  fClonesZ_PhiDiff = new TH1F("fClonesZ_PhiDiff", "fClonesZ_PhiDiff", 20000, -100., 100.);
  fClonesXDiffRel = new TH1F("fClonesXDiffRel", "fClonesXDiffRel", 20000, -100., 100.);
  fClonesYDiffRel = new TH1F("fClonesYDiffRel", "fClonesYDiffRel", 20000, -100., 100.);
  fClonesRDiffRel = new TH1F("fClonesRDiffRel", "fClonesRDiffRel", 20000, -100., 100.);
  fClonesZ_PhiDiffRel = new TH1F("fClonesZ_PhiDiffRel", "fClonesZ_PhiDiffRel", 20000, -100., 100.);

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Private method ReInit   -----------------------------------------
InitStatus PndBarrelTrackFinder::ReInit()
{

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Private method Reset   ------------------------------------------
void PndBarrelTrackFinder::Reset() {}
// -------------------------------------------------------------------------

// -----   Public method Finish   ------------------------------------------
void PndBarrelTrackFinder::Finish()
{
  fBarrelTrackArray->Clear();
  fBarrelTrackCandArray->Clear();

  if (fDrawHistos) {
    new TCanvas("xdiff", "xdiff", 10, 10, 1000, 800);
    fClonesXDiff->Draw();
    new TCanvas("ydiff", "ydiff", 10, 10, 1000, 800);
    fClonesYDiff->Draw();
    new TCanvas("rdiff", "rdiff", 10, 10, 1000, 800);
    fClonesRDiff->Draw();
    new TCanvas("zdiff", "`diff", 10, 10, 1000, 800);
    fClonesZ_PhiDiff->Draw();
    new TCanvas("xdiffR", "xdiffR", 10, 10, 1000, 800);
    fClonesXDiffRel->Draw();
    new TCanvas("ydiffR", "ydiffR", 10, 10, 1000, 800);
    fClonesYDiffRel->Draw();
    new TCanvas("rdiffR", "rdiffR", 10, 10, 1000, 800);
    fClonesRDiffRel->Draw();
    new TCanvas("zdiffR", "zdiffR", 10, 10, 1000, 800);
    fClonesZ_PhiDiffRel->Draw();
    TCanvas* canv = new TCanvas("canv","canv",10,10,1000,1000);
    canv->Divide(5,3,0.,0.);
    for (Int_t idet = 0; idet < 5 ; idet++) {
        for ( int idim = 0 ; idim < 3 ; idim++ ) {
            canv->cd(idim*5+idet+1);
            fhHitDist[idet][idim]->Draw();
        }
    }
  }
  
  //delete fSttGeoH;

  cout << "-------------------- " << fName.Data() << " : Summary -----------------------" << endl;
  cout << " Events:        " << setw(10) << fTNofEvents << endl;
  cout << " Tracks:     " << setw(10) << fTNofTracks << "    ( " << (Double_t)fTNofTracks / ((Double_t)fTNofEvents) << " per event )" << endl;
  cout << "--------------------------------------------------------------------------------" << endl;
}
// -------------------------------------------------------------------------

ClassImp(PndBarrelTrackFinder)
