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
// PndSttMvdGemTracking (a.k.a. "the GEM extension")
//
// Class for extending the SttMvdTrack to the GEM stations
//
// authors: Lia Lavezzi - INFN Pavia (2011)
//
////////////////////////////////////////////////////////////

#include "PndSttMvdGemTracking.h"

// pandaroot
#include "PndSttMapCreator.h"
#include "PndSttTube.h"
#include "PndTrack.h"
#include "PndTrackID.h"
#include "PndTrackCand.h"
#include "PndGemHit.h"
#include "PndGemMCPoint.h"
#include "PndMCTrack.h"
#include "PndSttHit.h"
#include "PndSdsHit.h"

// fairroot
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairGeanePro.h"
#include "FairGeaneUtil.h"
#include "FairTrackParP.h"
#include "FairLogger.h"
#include "FairRootFileSink.h"

// ROOT
#include "TDatabasePDG.h"
#include "TArc.h"
#include "TMarker.h"
#include "TF1.h"
#include "TClonesArray.h"

// general
#include <iostream>
#include <cmath>

// using GEM ideal digi?
#define IDEAL false // CHECKING

// if you want not to consider mcIndex == -1 or non primary tracks (from MC)
#define THROWAWAY false

// CHECKCOMBI:
//    0 no combinatorial taken into account
//    1 combi taken into account only after whole procedure
//    2 combi not allow during assignment
#define CHECKCOMBI 2

int countperformance[7];
using namespace std;

// -----   Default constructor   -------------------------------------------
PndSttMvdGemTracking::PndSttMvdGemTracking() : PndPersistencyTask("MVD-STT-GEM tracking")
{
  fVerbose = 1;
  fEvaluate = kTRUE;
  fDisplayOn = false;
  fTimes = 5;
  fTurn = 1;
  fMaxDistance = -1;
  fUseMC = kFALSE;
  fCombiDistance = 1.;
  fDefaultPdgCode = -13;
  fPdgFromMC = kFALSE;

  fMvdPixelBranchName = "MVDHitsPixel";
  fMvdStripBranchName = "MVDHitsStrip";
  fSttBranchName = "STTHit";
  fGemBranchName = "GEMHit";

  fStartTrackBranchName = "SttMvdTrack";
  fStartTrackCandBranchName = "SttMvdTrackCand";
  fStartTrackIDBranchName = "SttMvdTrackID";

  SetPersistency(kTRUE);
}
// -------------------------------------------------------------------------

PndSttMvdGemTracking::PndSttMvdGemTracking(Int_t verbose) : PndPersistencyTask("MVD-STT-GEM tracking")
{
  SetPersistency(kTRUE);
  fVerbose = verbose;
  if (verbose > 0)
    fEvaluate = kTRUE;
  else
    fEvaluate = kFALSE;
  fDisplayOn = false;
  fTimes = 5;
  fTurn = 1;
  fMaxDistance = -1;
  fUseMC = kFALSE;
  fCombiDistance = 1.;
  fDefaultPdgCode = -13;
  fPdgFromMC = kFALSE;

  fMvdPixelBranchName = "MVDHitsPixel";
  fMvdStripBranchName = "MVDHitsStrip";
  fSttBranchName = "STTHit";
  fGemBranchName = "GEMHit";

  fStartTrackBranchName = "SttMvdTrack";
  fStartTrackCandBranchName = "SttMvdTrackCand";
  fStartTrackIDBranchName = "SttMvdTrackID";
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndSttMvdGemTracking::~PndSttMvdGemTracking() {}
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
InitStatus PndSttMvdGemTracking::Init()
{

  evt = 0;
  countgood = 0;               // correct association
  countbad = 0;                // wrong association
  countdoubt = 0;              // doubt in ref index CHECK (temporary)
  countprinotassigned = 0;     // not assigned hits from primary tracks
  countsecnotassigned = 0;     // not assigned hits from secondary tracks
  countreconstructablehit = 0; // n of hits which belong to a mctrack which has mvd + stt candidate
  for (int iplane = 0; iplane < 8; iplane++) {
    countplane[iplane] = 0;
    countprop[0][iplane] = 0;
    countprop[1][iplane] = 0;
    countnohitonplane[iplane] = 0;
  }
  countsttmvd = 0;
  countsttmvdusable = 0;

  FairRootManager *ioman = FairRootManager::Instance();

  LOG(info) << " -------------------";
  LOG(info) << " PndSttMvdGemTracking: using branches " << fMvdPixelBranchName << " " << fMvdStripBranchName << " " << fSttBranchName << " " << fGemBranchName;
  cout << "-I- to change one or more of these use PndSttMvdGemTracking:SetBranchName( TStrings ); the order of TStrings is mvd pixel name, mvd strip name, stt name, gem name"
       << endl;
  cout << "starting track for extrapolation " << fStartTrackBranchName << " " << fStartTrackCandBranchName << endl;
  if (fPdgFromMC)
    cout << "starting trackid for extrapolation " << fStartTrackIDBranchName << endl;
  LOG(info) << " -------------------";

  if (!ioman) {
    cout << "-E- PndSttMvdGemTracking: "
         << "RootManager not instantised!" << endl;
    return kFATAL;
  }

  // open SttMvdTrackCand array
  fTrackCandArray = (TClonesArray *)ioman->GetObject(fStartTrackCandBranchName);
  if (fTrackCandArray == nullptr) {
    cout << "fStartTrackCandBranchName " << fStartTrackCandBranchName << " not found" << endl;
    Error("PndSttMvdGemTracking:Init", "stt + mvd trackcand - array not found!");
    return kERROR;
  }

  // open SttMvdTrack array
  fTrackArray = (TClonesArray *)ioman->GetObject(fStartTrackBranchName);
  if (!fTrackArray) {
    Error("PndSttMvdGemTracking:Init", "stt + mvd track - array not found!");
    return kERROR;
  }

  // open SttMvdTrack array
  if (fPdgFromMC) {
    fTrackIDArray = (TClonesArray *)ioman->GetObject(fStartTrackIDBranchName);
    if (!fTrackIDArray) {
      Error("PndSttMvdGemTracking:Init", "stt + mvd trackID - array not found!");
      return kERROR;
    }
  }

  // open GEM hit array
  fGemHitArray = (TClonesArray *)ioman->GetObject(fGemBranchName);
  if (!fGemHitArray) {
    Error("PndSttMvdGemTracking:Init", "gem hit - array not found!");
    return kERROR;
  }

  // open GEM point array
  fGemPointArray = (TClonesArray *)ioman->GetObject("GEMPoint");
  if (!fGemPointArray) {
    Error("PndSttMvdGemTracking:Init", "gem point - array not found!");
    return kERROR;
  }

  // open MC track array CHECK to be deleted for real data
  fMCTrackArray = (TClonesArray *)ioman->GetObject("MCTrack");
  if (!fMCTrackArray) {
    Error("PndSttMvdGemTracking:Init", "mctrack - array not found!");
    return kERROR;
  }

  // open MVD pixel hit array
  fMvdPixelHitArray = (TClonesArray *)ioman->GetObject(fMvdPixelBranchName);
  if (!fMvdPixelHitArray) {
    Error("PndSttMvdGemTracking:Init", "mvd pixel hit - array not found!");
    return kERROR;
  }

  // open MVD strip hit array
  fMvdStripHitArray = (TClonesArray *)ioman->GetObject(fMvdStripBranchName);
  if (!fMvdStripHitArray) {
    Error("PndSttMvdGemTracking:Init", "mvd strip hit - array not found!");
    return kERROR;
  }

  // open STT hit array
  fSttHitArray = (TClonesArray *)ioman->GetObject(fSttBranchName);
  if (!fSttHitArray) {
    Error("PndSttMvdGemTracking:Init", "stt hit - array not found!");
    return kERROR;
  }

  // open MVD point array
  fMvdPointArray = (TClonesArray *)ioman->GetObject("MVDPoint");
  if (!fMvdPointArray) {
    Error("PndSttMvdGemTracking:Init", "mvd point - array not found!");
    return kERROR;
  }

  // open STT point array
  fSttPointArray = (TClonesArray *)ioman->GetObject("STTPoint");
  if (!fSttPointArray) {
    Error("PndSttMvdGemTracking:Init", "stt point - array not found!");
    return kERROR;
  }

  // Create and register PndTrack array
  fCompleteTrackCandArray = new TClonesArray("PndTrackCand", 100);
  ioman->Register("SttMvdGemTrackCand", "SttMvdGem", fCompleteTrackCandArray, GetPersistency());

  fCompleteTrackArray = new TClonesArray("PndTrack", 100);
  ioman->Register("SttMvdGemTrack", "SttMvdGem", fCompleteTrackArray, GetPersistency());

  // GEANE propagation to volume
  fPro = new FairGeanePro();
  if (fVerbose == 0)
    fPro->SetPrintErrors(kFALSE);

  // STT mapper
  PndSttMapCreator *mapper = new PndSttMapCreator(fSttParameters);
  fTubeArray = mapper->FillTubeArray();

  // set up geometry of GEMs;
  SetupGEMPlanes();

  if (fPdgFromMC)
    LOG(info) << " PndSttMvdGemTracking: Taking PDG from MC (keeping backup default opt = " << fDefaultPdgCode << ")";
  else
    LOG(info) << " PndSttMvdGemTracking: using default PDG " << fDefaultPdgCode;

  LOG(info) << " PndSttMvdGemTracking: Intialisation successfull";
  return kSUCCESS;
}
// -------------------------------------------------------------------------

void PndSttMvdGemTracking::SetParContainers()
{

  FairRuntimeDb *rtdb = FairRun::Instance()->GetRuntimeDb();

  // get STT parameters
  fSttParameters = (PndGeoSttPar *)rtdb->getContainer("PndGeoSttPar");

  // get GEM parameters
  fGemParameters = (PndGemDigiPar *)rtdb->getContainer("PndGemDetectors");
}

void PndSttMvdGemTracking::SetupGEMPlanes()
{

  if (fVerbose > 0)
    cout << "********* SPndSttMvdGemTracking::SetupGEMPlanes ************" << endl;

  // geometry of GEM stations
  Int_t nstations = fGemParameters->GetNStations();
  Int_t nsensors = 0;
  Int_t posindex = 0;

  if (fVerbose > 0)
    cout << "NSTAT " << nstations << endl;

  if (nstations == 0)
    cerr << "PndSttMvdGemTracking::SetupGEMPlanes: NO GEOMETRY FOR GEMs!" << endl;

  // position of the planes
  fSensPositions = new TClonesArray("TVector3", 100);

  // order of the sensors:
  // stat = 1, sens = 1 ==> posindex = 11 ==> ordering = 0
  // stat = 1, sens = 2 ==> posindex = 12 ==> ordering = 1
  // stat = 2, sens = 1 ==> posindex = 21 ==> ordering = 2
  // stat = 2, sens = 2 ==> posindex = 22 ==> ordering = 3
  // ...

  // count sensors and stations and fill the sensor positions
  for (int istat = 0; istat < nstations; istat++) {
    PndGemStation *station = fGemParameters->GetStation(istat);
    for (int isens = 0; isens < station->GetNSensors(); isens++) {
      PndGemSensor *sensor = station->GetSensor(isens);
      nsensors++;
      posindex = (istat + 1) * 10 + (isens + 1);
      fOrdering.push_back(posindex);
      if (fVerbose > 0)
        cout << "istat/isens/posindex/ordering " << istat + 1 << " " << isens + 1 << " " << posindex << " " << fOrdering.size() << " " << fOrdering[fOrdering.size() - 1] << endl;

      // fill the TClonesArray with the origin
      // of the planes corresponding to each
      // sensor of GEM
      int size = fOrdering.size() - 1;
      TClonesArray &clref = *fSensPositions;
      new (clref[size]) TVector3(sensor->GetX0(), sensor->GetY0(), sensor->GetZ0());
    }
  }

  // the sensor planes are parallel to xy  // CHECK is this true?
  fDj.SetXYZ(1., 0., 0.);
  fDk.SetXYZ(0., 1., 0.);

  // CHECK delete this ---
  if (fVerbose > 0) {
    cout << "entries " << fSensPositions->GetEntriesFast() << endl;
    for (int i = 0; i < fSensPositions->GetEntriesFast(); i++) {
      TVector3 *a = (TVector3 *)fSensPositions->At(i);
      cout << i << endl;
      if (!a)
        continue;
      a->Print();
    }
  }
  // ---

  fNPositions = nsensors; //  * nstations;
  for (int ipos = 0; ipos < fNPositions; ipos++) {
    TString hname = "hdist";
    hname += ipos;
    hdist[ipos] = new TH1F(hname, hname, 200, 0, 100);
    hname += " turn 2";
    hdist2[ipos] = new TH1F(hname, hname, 200, 0, 100);
    hname = "hsigma";
    hname += ipos;
    hsigma[ipos] = new TH1F(hname, hname, 200, 0, 20);
    hname += " turn 2";
    hsigma2[ipos] = new TH1F(hname, hname, 200, 0, 20);
    hname = "hchosen";
    hname += ipos;
    hchosen[ipos] = new TH1F(hname, hname, 200, 0, 100);
    hname += " turn 2";
    hchosen2[ipos] = new TH1F(hname, hname, 200, 0, 100);
    hname = "hmcdist";
    hname += ipos;
    hmcdist[ipos] = new TH1F(hname, hname, 200, 0, 100);
    hname = "hmcx";
    hname += ipos;
    hmcx[ipos] = new TH1F(hname, hname, 200, 0, 100);
    hname = "hmcy";
    hname += ipos;
    hmcy[ipos] = new TH1F(hname, hname, 200, 0, 100);
  }

  // resize hitcounter
  hitcounter.ResizeTo(fNPositions, 1);
}

void PndSttMvdGemTracking::Reset(Int_t nhits, Int_t ntracks)
{

  if (fVerbose > 0)
    cout << "******* PndSttMvdGemTracking::Reset *******" << endl;
  if (fDisplayOn) {
    char goOnChar;
    cout << "press any key" << endl;
    cin >> goOnChar;
    cout << "GOING ON" << endl;

    display = new TCanvas("display", "display", 0, 0, 800, 500);
    display->Divide(3, 2); // fNPositions/2, 2);
    // GEM
    int ipos = 0;
    Int_t nstations = fGemParameters->GetNStations();
    for (int istat = 0; istat < nstations; istat++) {
      PndGemStation *station = fGemParameters->GetStation(istat);
      for (int isens = 0; isens < station->GetNSensors(); isens++) {
        PndGemSensor *sensor = station->GetSensor(isens);
        TString iposname = "hipos";
        TString ipostitle = "position ";
        iposname += ipos;
        ipostitle += ipos;

        h[ipos] = new TH2F(iposname, ipostitle, 100, -sensor->GetOuterRadius(), sensor->GetOuterRadius(), 100, -sensor->GetOuterRadius(), sensor->GetOuterRadius());
        display->cd(ipos + 1);
        h[ipos]->Draw();
        display->Update();
        display->Modified();
        ipos++;
      }
    }
  }

  hitmap.ResizeTo(fNPositions, nhits);
  distancemap.ResizeTo(ntracks, nhits);

  notassignedhits.clear();
  for (int ihit = 0; ihit < nhits; ihit++) {
    for (int itrk = 0; itrk < ntracks; itrk++)
      distancemap[itrk][ihit] = -1;
    notassignedhits.push_back(ihit);
  }
  if (fVerbose > 0)
    cout << "not assig " << nhits << " " << notassignedhits.size() << endl;

  for (int ipos = 0; ipos < fNPositions; ipos++) {
    towhichplane[ipos] = false;

    // initialize all to 0
    hitcounter(ipos, 0) = 0;

    for (int ihit = 0; ihit < nhits; ihit++)
      hitmap(ipos, ihit) = -1;
  }

  if (fVerbose > 0)
    cout << "npositions/nhits " << fNPositions << " " << nhits << endl;
  trackvector.clear();
  usabletracks.clear(); // CHECK 4 PERFORMANCE delete this!
  trackindexes.clear();

  // initialize the combimap to all combinatorials
  fCombiMap.clear();
  for (int ihit = 0; ihit < nhits; ihit++)
    fCombiMap[ihit] = 1;
}

void PndSttMvdGemTracking::OrderGemHits(Int_t nhits)
{
  if (nhits == 0)
    return;

  // loop on GEM hits to fill hitcounter
  // and hitmap and be able to order them
  for (int ihit = 0; ihit < nhits; ihit++) {
    PndGemHit *hit = (PndGemHit *)fGemHitArray->At(ihit);
    if (!hit)
      continue;
    if (fVerbose > 0)
      cout << "hit " << ihit << " @ " << hit->GetX() << " " << hit->GetY() << " " << hit->GetZ() << " " << hit->GetStationNr() << " " << hit->GetSensorNr() << " " << endl;

    // translate posindex to index in ordering
    // 11 ==> 0
    // 12 ==> 1
    // ...
    int posindex = GetPosIndex(hit);

    fOrderingIterator = find(fOrdering.begin(), fOrdering.end(), posindex);
    int index = fOrderingIterator - fOrdering.begin();
    int idx = (int)hitcounter(index, 0);
    if (fVerbose > 0)
      cout << posindex << " " << idx << " " << index << endl;

    if (fDisplayOn) {
      TMarker *gmrk = new TMarker(hit->GetX(), hit->GetY(), 7);
      display->cd(index + 1);
      gmrk->Draw("SAME");
      display->Update();
      display->Modified();
    }

    hitmap(index, idx) = ihit; // CHECK how to make a TMatrixT of int
    hitcounter(index, 0)++;
    // set the corresponding position in towhichplane to true
    towhichplane[index] = true;
  }
  //   hitmap.Print();
  //   hitcounter.Print();

  if (fVerbose > 0)
    for (int ipos = 0; ipos < fNPositions; ipos++)
      cout << "pos " << ipos << " " << towhichplane[ipos] << endl; // CHECK delete  it
}

// -----   Public method Exec   --------------------------------------------
void PndSttMvdGemTracking::Exec(Option_t *)
{
  if (fVerbose > 0) {
    cout << "==================== EVENT " << evt << endl;
    cout << "detId " << FairRootManager::Instance()->GetBranchId(fMvdPixelBranchName) << " " << FairRootManager::Instance()->GetBranchId(fMvdStripBranchName) << " "
         << FairRootManager::Instance()->GetBranchId(fGemBranchName) << " " << FairRootManager::Instance()->GetBranchId(fSttBranchName) << endl;
  }

  evt++;
  fTurn = 1;
  fCompleteTrackCandArray->Delete();
  fCompleteTrackArray->Delete();

  Int_t nhits = fGemHitArray->GetEntriesFast();
  Int_t ntracks = fTrackArray->GetEntriesFast();

  if (fVerbose > 0) {
    cout << "stt + mvd track array " << ntracks << endl;
    cout << "gem hits " << nhits << endl;
  }

  //  if(nhits == 0) return;   // if there is no GEM hit, there is no need to waste time! CHECK this has to be changed
  if (ntracks == 0)
    return; // if there is no track, there is no need to waste time!
  countsttmvd += ntracks;

  PndTrack *sttmvd = nullptr;
  PndTrackCand *sttmvdCand = nullptr;
  PndTrackCand *completeCand = nullptr;
  PndTrack *completeTrack = nullptr;

  // RESET all the maps and counters for this event
  Reset(nhits, ntracks);

  // Order GEM hits
  OrderGemHits(nhits);
  Int_t flag[ntracks];

  if (CHECKCOMBI > 0)
    ConsiderCombinatorialEffect(nhits);

  // list of tracks which can/cannot be propagated
  fProTracks.clear();

  // loop on the tracks found in mvd + stt ************
  for (Int_t itrk = 0; itrk < ntracks; itrk++) {
    fProTracks[itrk] = false;

    if (fVerbose > 0)
      cout << "----------- TRACK " << itrk << "------------" << endl;

    flag[itrk] = 0;
    sttmvd = (PndTrack *)fTrackArray->At(itrk);
    if (!sttmvd) {
      LOG(warn) << " PndSttMvdGemTracking::Exec: Empty PndTrack at " << itrk;
      continue;
    }
    sttmvdCand = sttmvd->GetTrackCandPtr();
    if (!sttmvdCand) {
      LOG(warn) << " PndSttMvdGemTracking::Exec: Empty PndTrackCand at " << itrk;
      continue;
    }

    if (fDisplayOn) {
      char goOnChar;
      cout << "press any key" << endl;
      cin >> goOnChar;
      cout << "GOING ON, nex track" << endl;
    }

    // -------------------------------------------------  CHECK this has to be changed
    Copy(completeCand, completeTrack, sttmvdCand, sttmvd);
    trackindexes.push_back(itrk);
    // -------------------------------------------------

    if (nhits == 0) {
      flag[itrk] = -1;
      continue;
    } // CHECK

    // cout << "copied completeCand from sttmvdCand @ " << itrk << " has hits " << completeCand->GetNHits() << endl;
    //
    FairTrackParP lastpar = sttmvd->GetParamLast();
    if (lastpar.GetMomentum().Z() > 999990.) {
      flag[itrk] = -2;
      continue;
    } // CHECK
    FairTrackParP *gempar = new FairTrackParP();

    if (fabs(lastpar.GetPosition().X()) > 42. || fabs(lastpar.GetPosition().Y()) > 42.) {
      flag[itrk] = -3;
      continue;
    } // CHECK 4 PERFORMANCE delete this!!!!
    if (lastpar.GetMomentum().Mag() < 0.15) {
      if (fVerbose > 0) {
        cout << "TOO LOW MOMENTUM " << itrk << endl;
        lastpar.GetPosition().Print();
        lastpar.GetMomentum().Print();
      }
      flag[itrk] = 1;

      //	continue; // CHECK 4 PERFORMANCE delete this!!!!
    }

    // last z position
    // get 1st sensor in 1st station
    PndGemStation *station = fGemParameters->GetStation(0);
    PndGemSensor *sensor = station->GetSensor(0);
    if (lastpar.GetPosition().Z() > sensor->GetZ0()) {
      if (fVerbose > 0) {
        cout << "Z OUT OF BOUNDS" << endl;
        lastpar.GetPosition().Print();
        lastpar.GetMomentum().Print();
      }
      flag[itrk] = 2;

      // continue; // CHECK 4 PERFORMANCE delete this!!!!
    }

    if (fVerbose > 0)
      cout << "SetParameters for track " << itrk << endl;
    FairTrackParP tmppar = SetStartParameters(sttmvd, sttmvdCand);

    if (fabs(tmppar.GetMomentum().Z()) < 1.e-5) {
      if (fVerbose > 0)
        cout << " CANNOT PROPAGATE because z mom == 0" << endl;
      flag[itrk] = -7;
      continue;
    }

    // =========== test of prop on 1st plane
    FairTrackParP *gempartest = new FairTrackParP();
    if (PropagateToGemPlaneAsHelix(sttmvd, gempartest, 0) == kFALSE) {
      if (fVerbose > 0)
        cout << " CANNOT PROPAGATE " << endl;
      delete gempartest;
      flag[itrk] = -6;
      continue;
    }

    // ===========

    int charge = tmppar.GetQ();
    //	fPdgCode = -13  * charge;
    if (fPdgFromMC)
      fPdgCode = GetChargeCorrectedPdgFromMC(itrk, charge);
    else
      fPdgCode = fDefaultPdgCode * charge;

    // 	cout << "START MOM " << lastpar.GetMomentum().Mag() << " " << tmppar.GetMomentum().Mag() << endl;

    // **************
    if (fEvaluate) {
      int mcIndex = sttmvdCand->getMcTrackId();
      if (mcIndex == -1) {
        flag[itrk] = -4;
        if (THROWAWAY)
          continue;
      } // CHECK 4 PERFORMANCE delete this!!!!
      else {
        //  cout << "FROM MC " << mcIndex << endl;
        PndMCTrack *mctrk = (PndMCTrack *)fMCTrackArray->At(mcIndex);
        if (mctrk) {
          // 	cout << "PDG " << mctrk->GetPdgCode() << " MOTHERID " << mctrk->GetMotherID() << endl;
          int mccharge = (int)TMath::Sign(1., TDatabasePDG::Instance()->GetParticle(mctrk->GetPdgCode())->Charge() / 3.);
          if (mccharge != charge && fVerbose > 0)
            cout << "WRONG CHARGE " << charge << " " << mccharge << " " << mctrk->GetPdgCode() << endl;

          if (mctrk->GetMotherID() != -1) {
            flag[itrk] = -5;
            if (THROWAWAY)
              continue;
          } // CHECK 4 PERFORMANCE delete this!!!!
        }
      }
      // **************
      countsttmvdusable++;
      usabletracks.push_back(mcIndex); // CHECK 4 PERFORMANCE delete this!!!!
    }

    fProTracks[itrk] = true;

    Int_t closestonfirst = -1;
    Double_t closestdistance = -1;
    // loop over the GEM hits from the closest to 0, 0, 0 to the most external
    // and
    // propagate to track from mvd + stt to each of them
    for (int ipos = 0; ipos < fNPositions; ipos++) {
      // if no hit then continue
      if (towhichplane[ipos] == false) {
        countnohitonplane[ipos]++;
        continue;
      }

      // ...else propagate here
      Bool_t prop = PropagateToGemPlane(&tmppar, gempar, ipos);
      if (prop == kFALSE)
        prop = PropagateToGemPlaneAsHelix(sttmvd, gempar, ipos);

      if (prop == kFALSE) {
        countprop[fTurn - 1][ipos]++;
        continue; // CHECK (continue or break?)
      }
      countplane[ipos]++;

      // if the propagation was successful ...
      // assign the hits to track itrk, extrapolated to gempar on ipos
      std::vector<int> assignedhits = AssignHits(itrk, gempar, ipos);
      for (size_t ihit = 0; ihit < assignedhits.size(); ihit++)
        AddHitToTrack(assignedhits[ihit], itrk);

      if (closestonfirst == -1)
        closestonfirst = GetClosestOnFirst(gempar, ipos, closestdistance);

      Double_t covMat[15];
      gempar->GetCov(covMat); // CHECK
      tmppar.SetTrackPar(gempar->GetX(), gempar->GetY(), gempar->GetZ(), gempar->GetPx(), gempar->GetPy(), gempar->GetPz(), gempar->GetQ(), covMat, gempar->GetOrigin(),
                         gempar->GetIVer(), gempar->GetJVer(), gempar->GetKVer());
    }

    // use closest on first CHECK with more tracks
    if (GetHitsAssociatedToTrack(itrk).size() == 0 && closestonfirst != -1) {
      //	cout << "CLOSEST HIT " << closestonfirst << " " << closestdistance << endl;
      distancemap[itrk][closestonfirst] = closestdistance;
      AddHitToTrack(closestonfirst, itrk);
    }
  }

  if (nhits == 0)
    return;

  if (fVerbose > 0 && CountTracks() != fCompleteTrackCandArray->GetEntriesFast())
    cout << "ERROR!!! " << CountTracks() << " " << fCompleteTrackCandArray->GetEntriesFast() << endl;

  // CLEANUP =================================
  if (CHECKCOMBI == 1)
    CheckCombinatorial(nhits, ntracks);
  ForbidMultiAssignedHits(nhits, ntracks);
  OnlyOneHitToEachTrack(nhits, ntracks);
  AddRemainingHits(ntracks);
  fTurn = 2;
  Retrack();

  // CHECK to test
  if (fVerbose > 0) {
    cout << "N OF TRACKS " << CountTracks() << endl;
    for (int i = 0; i < CountTracks(); i++) {
      int itrk = GetTrackIndex(i);
      Int_t nofhits = CountHitsInTrack(itrk);
      cout << "track " << itrk << " has " << nofhits << endl;
      std::vector<int> hitsoftrack;
      hitsoftrack = GetHitsAssociatedToTrack(itrk);
      for (int ihit = 0; ihit < nofhits; ihit++)
        cout << " " << hitsoftrack[ihit];
      cout << endl;
    }
  }
  // ----------------------------------

  // ACTUALLY ASSIGN HITS TO TRACK
  PndGemHit *gemhit = nullptr;
  for (Int_t i = 0; i < CountTracks(); i++) {
    int itrk = GetTrackIndex(i);
    if (fVerbose > 0)
      cout << i << " ACTUALLY ASSIGNING TRACK " << itrk << endl;
    completeCand = (PndTrackCand *)fCompleteTrackCandArray->At(itrk);
    std::vector<int> thistrackhits = GetHitsAssociatedToTrack(itrk);

    for (size_t j = 0; j < thistrackhits.size(); j++) {
      int ihit = thistrackhits[j];
      gemhit = (PndGemHit *)fGemHitArray->At(ihit);
      if (!gemhit)
        continue;
      if (fVerbose > 0)
        cout << "ACTUALLY add hit " << ihit << " to track " << itrk << endl;
      completeCand->AddHit(FairRootManager::Instance()->GetBranchId(fGemBranchName), ihit, gemhit->GetPosition().Mag()); // CHECK rho and kGemHit
    }

    completeTrack = (PndTrack *)fCompleteTrackArray->At(itrk);
    if (fVerbose > 0 && completeTrack->GetRefIndex() != itrk)
      cout << "************** ERROR ****************" << endl;
    completeTrack->SetTrackCand(*completeCand);
    completeTrack->SetFlag(flag[itrk]);
    //    cout << "track " << itrk << " has flag " << flag[itrk] << endl;
  }

  // performance
  if (fEvaluate)
    EvaluatePerformances(nhits, ntracks);
}

void PndSttMvdGemTracking::Copy(PndTrackCand *completeCand, PndTrack *completeTrack, PndTrackCand *sttmvdCand, PndTrack *sttmvd)
{

  // -------------------------------------------------  CHECK this has to be changed
  //      completeCand = sttmvdCand; // CHECK!
  // CHECK ref index, to assign completeCand to sttmvdCand (itrk) URGENT

  // copy to the complete track (CHECK will be probably changed)
  TClonesArray &clref = *fCompleteTrackCandArray;
  Int_t size = clref.GetEntriesFast();
  completeCand = new (clref[size]) PndTrackCand();

  completeCand->setMcTrackId(sttmvdCand->getMcTrackId());

  std::vector<PndTrackCandHit> sttmvdhits = sttmvdCand->GetSortedHits();
  for (size_t ihit = 0; ihit < sttmvdCand->GetNHits(); ihit++) {
    completeCand->AddHit(sttmvdCand->GetSortedHit(ihit).GetDetId(), sttmvdCand->GetSortedHit(ihit).GetHitId(), sttmvdCand->GetSortedHit(ihit).GetRho());

    if (fVerbose > 0)
      cout << "STT + MVD iHit " << sttmvdCand->GetSortedHit(ihit).GetHitId() << " detId " << sttmvdCand->GetSortedHit(ihit).GetDetId() << endl;
  }

  TClonesArray &clref2 = *fCompleteTrackArray;
  Int_t size2 = clref2.GetEntriesFast();
  completeTrack = new (clref2[size2]) PndTrack(sttmvd->GetParamFirst(), sttmvd->GetParamLast(), *completeCand);
  completeTrack->SetRefIndex("SttMvdGemTrackCand", size); // CHECK size or size2?
  completeTrack->SetFlag(sttmvd->GetFlag());
}

void PndSttMvdGemTracking::EvaluatePerformances(Int_t nhits, Int_t ntracks)
{
  // CHECK

  for (int itrk = 0; itrk < fCompleteTrackCandArray->GetEntriesFast(); itrk++) {
    PndTrackCand *completeCand = (PndTrackCand *)fCompleteTrackCandArray->At(itrk);
    if (!completeCand)
      continue;
    if (fVerbose > 0)
      cout << "complete cand " << itrk << " has nhits " << completeCand->GetNHits() << endl;

    //    UpdateMCTrackId(completeCand);

    for (size_t ihit = 0; ihit < completeCand->GetNHits(); ihit++) {
      PndTrackCandHit candhit = completeCand->GetSortedHit(ihit);
      Int_t iHit = candhit.GetHitId();
      Int_t detId = candhit.GetDetId();
      //      if(fVerbose != 0)       // CHECK
      if (fVerbose > 0)
        cout << "iHit " << iHit << " detId " << detId << "(" << FairRootManager::Instance()->GetBranchId(fGemBranchName) << ")" << endl;
      if (detId != FairRootManager::Instance()->GetBranchId(fGemBranchName))
        continue;

      PndGemHit *gemhit = (PndGemHit *)fGemHitArray->At(iHit);
      if (!gemhit)
        continue;

      // only for test.. will be deleted for real events ........CHECK
      Int_t refIndex = gemhit->GetRefIndex();
      if (refIndex == -1) {
        countbad++; // background hit
        if (fDisplayOn) {
          TMarker *g2mrkb = new TMarker(gemhit->GetX(), gemhit->GetY(), 30);
          int posindex = GetPosIndex(gemhit);
          fOrderingIterator = find(fOrdering.begin(), fOrdering.end(), posindex);
          int index = fOrderingIterator - fOrdering.begin();
          display->cd(index + 1);
          g2mrkb->SetMarkerSize(2);
          g2mrkb->SetMarkerColor(1);
          g2mrkb->Draw("SAME");
          display->Update();
          display->Modified();
        }
        continue;
      }

      PndGemMCPoint *gempnt = (PndGemMCPoint *)fGemPointArray->At(refIndex);

      Int_t mcIndex = gempnt->GetTrackID();

      if (mcIndex == -1 || completeCand->getMcTrackId() == -1)
        countdoubt++;
      else if (mcIndex == completeCand->getMcTrackId())
        countgood++;
      else if (mcIndex != completeCand->getMcTrackId())
        countbad++;

      if (fDisplayOn) {
        TMarker *g2mrk = new TMarker(gemhit->GetX(), gemhit->GetY(), 30);

        int posindex = GetPosIndex(gemhit);

        fOrderingIterator = find(fOrdering.begin(), fOrdering.end(), posindex);
        int index = fOrderingIterator - fOrdering.begin();
        display->cd(index + 1);
        g2mrk->SetMarkerSize(2);
        if (mcIndex == completeCand->getMcTrackId())
          g2mrk->SetMarkerColor(2); // red
        else if (mcIndex != completeCand->getMcTrackId())
          g2mrk->SetMarkerColor(1); // black
        g2mrk->Draw("SAME");
        display->Update();
        display->Modified();
      }
    }
  }

  std::vector<int> recoTrack;
  // the left hits have not been assigned
  for (int itrk = 0; itrk < ntracks; itrk++) {
    PndTrackCand *completeCand = (PndTrackCand *)fCompleteTrackCandArray->At(itrk);
    if (!completeCand)
      continue;
    recoTrack.push_back(completeCand->getMcTrackId());
  }

  for (int ihit = 0; ihit < nhits; ihit++) {
    PndGemHit *gemhit = (PndGemHit *)fGemHitArray->At(ihit);
    if (!gemhit)
      continue;
    Int_t refIndex = gemhit->GetRefIndex();
    if (refIndex == -1)
      continue;
    PndGemMCPoint *gempnt = (PndGemMCPoint *)fGemPointArray->At(refIndex);
    if (!gempnt)
      continue;

    Int_t mcIndex = gempnt->GetTrackID();
    if (mcIndex == -1)
      continue; // CHECK THIS
    std::vector<int>::iterator recoTrackIter;
    recoTrackIter = find(recoTrack.begin(), recoTrack.end(), mcIndex);
    Int_t where = recoTrackIter - recoTrack.begin();

    // take only real hits: no combinatories CHECK
    std::vector<int> truepoints;
    if (fabs(gempnt->GetX() - gemhit->GetX()) < 1 && // CHECK if 1 is ok as distance mcpoint to hit
        fabs(gempnt->GetY() - gemhit->GetY()) < 1) {
      std::vector<int>::iterator iter;
      iter = find(truepoints.begin(), truepoints.end(), refIndex);
      Int_t where2 = iter - truepoints.begin();

      // CHECK 4 PERFORMACE delete these .....
      std::vector<int>::iterator iter3;
      iter3 = find(usabletracks.begin(), usabletracks.end(), mcIndex);
      Int_t where3 = iter3 - usabletracks.begin();
      if (where != (int)recoTrack.size() && where2 == (int)truepoints.size() && where3 != (int)usabletracks.size()) {
        // .............. and uncomment this
        // if(where != recoTrack.size() && where2 == truepoints.size()) {

        countreconstructablepoint++;
        truepoints.push_back(refIndex);

        if (fDisplayOn) {
          int posindex = GetPosIndex(gemhit);
          fOrderingIterator = find(fOrdering.begin(), fOrdering.end(), posindex);
          int ipos = fOrderingIterator - fOrdering.begin();

          TMarker *gpntmrk = new TMarker(gempnt->GetX(), gempnt->GetY(), 7);
          gpntmrk->SetMarkerColor(5); //
          display->cd(1 + ipos);
          gpntmrk->Draw("SAME");
          display->Update();
          display->Modified();
        }
      }
    }

    if (where != (int)recoTrack.size())
      countreconstructablehit++; // if the hit is found

    if (GetTracksAssociatedToHit(ihit).size() != 0)
      continue;
    PndMCTrack *mctrk = (PndMCTrack *)fMCTrackArray->At(mcIndex);
    if (!mctrk)
      continue;
    if (mctrk->GetMotherID() == -1)
      countprinotassigned++;
    else
      countsecnotassigned++;
  }

  for (size_t i = 0; i < notassignedhits.size(); i++) {
    if (fVerbose != 0)
      cout << "hit " << notassignedhits[i] << "NOT ASSIGNED" << endl;
  }

  FillTrueDistances();

  // draw ALL the MC points
  for (int ipnt = 0; ipnt < fGemPointArray->GetEntriesFast(); ipnt++) {
    PndGemMCPoint *pnt = (PndGemMCPoint *)fGemPointArray->At(ipnt);
    if (!pnt)
      continue;
    double x0 = pnt->GetX();
    double y0 = pnt->GetY();
    double z0 = pnt->GetZ();
    int ipos = -1;
    if (fDisplayOn) { // CHECK
      if (z0 < 116.5)
        ipos = 0;
      else if (z0 < 118.)
        ipos = 1;
      else if (z0 < 152.4)
        ipos = 2;
      else if (z0 < 154)
        ipos = 3;
      else if (z0 < 188.4)
        ipos = 4;
      else if (z0 < 190)
        ipos = 5;

      cout << "MC POINT " << x0 << " " << y0 << " " << z0 << " " << pnt->GetTrackID() << endl;

      TMarker *gpntmrk = new TMarker(x0, y0, 3);
      gpntmrk->SetMarkerColor(7); //
      display->cd(1 + ipos);
      //  cout << "DRAWING" << endl;
      gpntmrk->Draw("SAME");
      display->Update();
      display->Modified();
    }
  }

  if (fVerbose > 0) {
    cout << "SUMMARY OF EVENT" << endl;
    cout << "mvd + stt tracks     " << fTrackArray->GetEntriesFast() << endl;
    cout << "gem hits             " << fGemHitArray->GetEntriesFast() << endl;
    cout << "complete track cands " << fCompleteTrackCandArray->GetEntriesFast() << endl;
  }
  cout << "COUNTERS " << countgood << " " << countbad << " " << countdoubt << " " << countreconstructablehit << " " << countreconstructablehit - countgood - countbad << " "
       << countprinotassigned << " " << countsecnotassigned << " " << countreconstructablepoint << endl;
  cout << "STTMVD " << countsttmvd << " of which usable " << countsttmvdusable << endl;

  cout << "planes: ";
  for (int iplane = 0; iplane < 8; iplane++)
    cout << countplane[iplane] << " ";
  cout << endl;
  cout << "nohit on plane: ";
  for (int iplane = 0; iplane < 8; iplane++)
    cout << countnohitonplane[iplane] << " ";
  cout << endl;
  cout << "prop 1 turn: ";
  for (int iplane = 0; iplane < 8; iplane++)
    cout << countprop[0][iplane] << " ";
  cout << endl;
  cout << "prop 2 turn: ";
  for (int iplane = 0; iplane < 8; iplane++)
    cout << countprop[1][iplane] << " ";
  cout << endl;
}

Bool_t PndSttMvdGemTracking::PropagateToGemPlane(FairTrackParP *tmppar, FairTrackParP *gempar, Int_t ipos)
{

  TVector3 *sensorpos = (TVector3 *)fSensPositions->At(ipos);
  TVector3 dj = tmppar->GetJVer();
  TVector3 dk = tmppar->GetKVer();
  fPro->PropagateFromPlane(dj, dk);
  //   fPro->PropagateFromPlane(fDj, fDk);
  fPro->PropagateToPlane(*sensorpos, fDj, fDk);

  if (fVerbose > 0) {
    cout << "propagation from " << endl;
    tmppar->GetPosition().Print();
    tmppar->GetMomentum().Print();
  }

  // last z position
  // get 1st sensor in 1st station
  // PndGemStation *station = fGemParameters->GetStation(0); //[R.K. 01/2017] unused variable?
  // PndGemSensor *sensor = station->GetSensor(0);  //[R.K. 01/2017] unused variable?
  if (tmppar->GetPosition().Z() > sensorpos->Z()) {
    if (fVerbose > 0)
      cout << "-> Z OUT OF BOUNDS: backpropagation" << endl;
    fPro->setBackProp();
  }

  Bool_t prop = fPro->Propagate(tmppar, gempar, fPdgCode);
  if (fVerbose > 0) {
    cout << "propagation to " << prop << endl;
    gempar->GetPosition().Print();
    gempar->GetMomentum().Print();
  }
  return prop;
}

// CHECK :-)GOOD!
Bool_t PndSttMvdGemTracking::PropagateToGemPlaneAsHelix(PndTrack *sttmvd, FairTrackParP *gempar, Int_t ipos)
{

  TVector3 *sensorpos = (TVector3 *)fSensPositions->At(ipos);
  Double_t z = sensorpos->Z();

  Double_t xc, yc, radius, fitm, fitp;
  GetInitialParams(sttmvd, xc, yc, radius, fitm, fitp);
  Int_t charge = sttmvd->GetParamFirst().GetQ();

  // z = fitp + scos * fitm
  if (fitm == 0)
    return kFALSE;
  Double_t scos = (z - fitp) / fitm;   // CHECK :-)GOOD!
  double Fi = -charge * scos / radius; // CHECK :-)GOOD!
  //  cout << "z1 " << z << " " << scos << " " << fitm << " " << fitp << endl;

  // x0 y0
  Double_t d = TMath::Sqrt(xc * xc + yc * yc) - radius;
  Double_t phi = TMath::ATan2(yc, xc);

  Double_t x0 = d * TMath::Cos(phi);
  Double_t y0 = d * TMath::Sin(phi);
  Double_t Phi0 = TMath::ATan2((y0 - yc), (x0 - xc));

  Double_t x = x0 + radius * (TMath::Cos(Phi0 + Fi) - TMath::Cos(Phi0));
  Double_t y = y0 + radius * (TMath::Sin(Phi0 + Fi) - TMath::Sin(Phi0));

  // ---
  // count sensors and stations and fill the sensor positions
  Int_t nstations = fGemParameters->GetNStations();
  Int_t nsensors = 0;
  double sens_rad = -1;
  Int_t posindex = 0;
  for (int istat = 0; istat < nstations; istat++) {
    PndGemStation *station = fGemParameters->GetStation(istat);
    for (int isens = 0; isens < station->GetNSensors(); isens++) {
      PndGemSensor *sensor = station->GetSensor(isens);
      nsensors++;
      posindex = (istat + 1) * 10 + (isens + 1);
      fOrderingIterator = find(fOrdering.begin(), fOrdering.end(), posindex);
      int jpos = fOrderingIterator - fOrdering.begin();
      if (ipos == jpos) {
        sens_rad = sensor->GetOuterRadius();
        break;
      }
    }
  }
  if (fabs(x) > sens_rad || fabs(y) > sens_rad) {
    if (fVerbose > 0)
      cout << "OUT OF SENSOR " << x << " " << y << " " << sens_rad << endl;
    return kFALSE;
  }

  // ---

  TVector2 v(x0 - xc, y0 - yc);
  Double_t alpha = TMath::ATan2(y - y0 + radius * TMath::Sin(Phi0), x - x0 + radius * TMath::Cos(Phi0));
  TVector2 p(x - xc, y - yc);
  Fi = CalculatePhi(v, p, alpha, Phi0, charge);
  scos = -charge * Fi * radius; // CHECK :-)GOOD!
  z = fitm * scos + fitp;       // CHECK

  //  cout << "z2 " <<  z << " " << scos << " " << fitm << " " << fitp <<  endl;

  double versor[2];
  versor[0] = xc - x;
  versor[1] = yc - y;

  Double_t Distance = TMath::Sqrt(versor[0] * versor[0] + versor[1] * versor[1]);
  versor[0] /= Distance;
  versor[1] /= Distance;

  double px = -charge * radius * 0.006 * versor[1];
  double py = charge * radius * 0.006 * versor[0];
  double pz = TMath::Sqrt(px * px + py * py) * fitm; // CHECK ?

  // error approx
  Double_t covMat[15];
  sttmvd->GetParamLast().GetCov(covMat); // CHECK it is not propagated

  gempar->SetTrackPar(x, y, z, px, py, pz, charge, covMat, *sensorpos, fDj.Cross(fDk), fDj, fDk);

  //   cout << "x0, y0 " << x0 << " " << y0 << endl;
  //   cout << "x y z px py pz " << endl;
  //   cout << x << " " << y << " " << z << endl;
  //   cout << px << " " <<  py << " " << pz << endl;

  return kTRUE;
}

// assign each it to at most one track!              CHECK CHECK CHECK
void PndSttMvdGemTracking::ForbidMultiAssignedHits(Int_t nhits, Int_t) // ntracks //[R.K.03/2017] unused variable(s)
{
  if (fVerbose > 0)
    cout << "FORBID MULTI ASSIGNED HITS : DELETING HITS" << endl;
  // ... use distancemap[itrk][ihit]
  for (int ihit = 0; ihit < nhits; ihit++) {
    if (fVerbose > 0)
      cout << "hit " << ihit << " associated to " << GetTracksAssociatedToHit(ihit).size() << " tracks" << endl;
    // if it is assigned to no track (0) or one track (1) we are done
    if (GetTracksAssociatedToHit(ihit).size() <= 1)
      continue;

    // else pick the right track
    Double_t tmpdistance = 1000; // CHECK set this to maxdistance
    Int_t trkcounter = 0;
    Int_t tmptrack = -1;

    int tracksassociated = GetTracksAssociatedToHit(ihit).size();

    // loop over the tracks
    for (int jtrk = 0; jtrk < tracksassociated; jtrk++) {
      Int_t itrk = GetTracksAssociatedToHit(ihit)[trkcounter];
      // cout << "track " << itrk << " distant " << distancemap[itrk][ihit] << " is " << trkcounter << endl;
      trkcounter++;
      if (distancemap[itrk][ihit] < tmpdistance) { // ... substitute this trk to tmp trk
        if (tmptrack != -1) {
          DeleteHitFromTrack(ihit, tmptrack);
          if (fVerbose > 0)
            cout << "hit " << ihit << " deleted from track " << tmptrack << endl;
          trkcounter--;
        }
        tmpdistance = distancemap[itrk][ihit];
        tmptrack = itrk;
      } else { // ... remove this hit definitively
        if (fVerbose > 0)
          cout << "hit " << ihit << " deleted from track " << itrk << endl;
        DeleteHitFromTrack(ihit, itrk);
        trkcounter--;
      }
    }
  }
}

// assign to each track ONLY one hit on each gem plane!              CHECK CHECK CHECK
void PndSttMvdGemTracking::OnlyOneHitToEachTrack(Int_t, Int_t) // nhits  ntracks//[R.K.03/2017] unused variable(s)
{
  if (fVerbose > 0)
    cout << "ONLY ONE HIT FOR EACH TRACK : DELETING HITS" << endl;

  for (Int_t i = 0; i < CountTracks(); i++) {
    int itrk = GetTrackIndex(i);
    if (fProTracks[itrk] == false)
      continue;
    std::vector<int> thistrackhits = GetHitsAssociatedToTrack(itrk);

    std::vector<double> tmpposdistance; // CHECK needs init?
    std::vector<int> tmpposhitindex;    // CHECK needs init?

    for (Int_t ipos = 0; ipos < fNPositions; ipos++) {
      tmpposdistance.push_back(-1);
      tmpposhitindex.push_back(-1);
    }

    for (size_t j = 0; j < thistrackhits.size(); j++) {
      int ihit = thistrackhits[j];

      PndGemHit *gemhit = (PndGemHit *)fGemHitArray->At(ihit);
      if (!gemhit)
        continue;

      int posindex = GetPosIndex(gemhit);

      fOrderingIterator = find(fOrdering.begin(), fOrdering.end(), posindex);
      int index = fOrderingIterator - fOrdering.begin();

      if (tmpposdistance[index] == -1) {
        tmpposdistance[index] = distancemap[itrk][ihit]; // CHECK
        tmpposhitindex[index] = ihit;
      } else {
        // if more than one hit on the same plane
        // is assigned to the same track, choose!
        if (fVerbose > 0) {
          cout << "hit " << ihit << " and " << tmpposhitindex[index] << " on the same plane" << endl;
          cout << "distance " << ihit << " " << distancemap[itrk][ihit] << endl;
          cout << "distance " << tmpposhitindex[index] << " " << tmpposdistance[index] << endl;
        }
        // if this distance is better...
        if (distancemap[itrk][ihit] < tmpposdistance[index]) {
          // ... delete the old hit and...
          DeleteHitFromTrack(tmpposhitindex[index], itrk); // CHECK
          // ... update
          tmpposdistance[index] = distancemap[itrk][ihit]; // CHECK
          tmpposhitindex[index] = ihit;
        } else {
          // otherwise, delete this hit!
          DeleteHitFromTrack(ihit, itrk);
        }
        if (fVerbose > 0)
          cout << tmpposhitindex[index] << " wins" << endl;
      }
    }
  }
}

Int_t PndSttMvdGemTracking::GetTrackIndex(int i)
{

  // probably this was wrong
  //   int counter = -1;
  //   int tmptrack = -1;
  //   std::vector< std::pair<int, int> >::iterator iter;
  //   for(iter = trackvector.begin(); iter != trackvector.end(); iter++) {
  //     std::pair<int, int> thispair = *iter;
  //     if(thispair.first != tmptrack) {
  //       tmptrack = thispair.first;
  //       counter++;
  //       cout << i << " " << counter << " " << tmptrack << " " << trackvector[i].first << endl;
  //       if(counter == i) { cout << "TRACK INDEX " << tmptrack << endl; return tmptrack; }
  //     }
  //   }

  if (i < (int)trackindexes.size())
    return trackindexes[i];
  cout << "PndSttMvdGemTracking::GetTrackIndex " << i << " Out Of Bounds" << endl;
  return -1;
}

Int_t PndSttMvdGemTracking::GetHitIndex(int i)
{ // CHECK THIS HAS TO BE TESTED!!
  int counter = -1;
  int tmphit = -1;
  std::vector<std::pair<int, int>>::iterator iter;
  for (iter = trackvector.begin(); iter != trackvector.end(); iter++) {
    std::pair<int, int> thispair = *iter;
    if (thispair.second != tmphit) {
      tmphit = thispair.second;
      counter++;
      //   cout << i << " " << counter << " " << tmphit << " " << trackvector[i].second << endl;
      if (counter == i)
        return tmphit;
    }
  }
  cout << "PndSttMvdGemTracking::GetTrackIndex " << i << " Out Of Bounds" << endl;
  return -1;
}

Int_t PndSttMvdGemTracking::CountTracks()
{
  // probably this was wrong
  //  int counter = 0;
  //  std::vector< std::pair<int, int> >::iterator iter;
  //  std::vector<int> counttracks;
  //  std::vector<int>::iterator iter2;
  //  for(iter = trackvector.begin(); iter != trackvector.end(); iter++) {
  //    std::pair<int, int> thispair = *iter;
  //    iter2 = find(counttracks.begin(), counttracks.end(), thispair.first);
  //    Int_t where = iter2 - counttracks.begin();
  //    if(where == counttracks.size()) {
  //      counter++;
  //      counttracks.push_back(thispair.first);
  //    }
  //  }
  // return counter;

  return trackindexes.size();
}

Int_t PndSttMvdGemTracking::CountHitsInTrack(Int_t itrk)
{

  int counter = 0;
  std::vector<std::pair<int, int>>::iterator iter;
  for (iter = trackvector.begin(); iter != trackvector.end(); iter++) {
    std::pair<int, int> thispair = *iter;
    if (thispair.first == itrk)
      counter++;
  }
  if (fVerbose > 0)
    std::cout << "track " << itrk << " has " << counter << " hits" << std::endl;
  return counter;
}

void PndSttMvdGemTracking::AddHitToTrack(Int_t ihit, Int_t itrk)
{
  std::pair<int, int> thispair(itrk, ihit);
  trackvector.push_back(thispair);
  std::vector<int>::iterator iter;
  iter = std::find(notassignedhits.begin(), notassignedhits.end(), ihit);
  int where = iter - notassignedhits.begin();
  if (where != (int)notassignedhits.size())
    notassignedhits.erase(iter); // remove from not assigned list
  if (fVerbose > 0)
    cout << "ADD HIT " << ihit << " TO TRK " << itrk << endl;
}

void PndSttMvdGemTracking::DeleteHitFromTrack(Int_t ihit, Int_t itrk)
{
  std::pair<int, int> thispair(itrk, ihit);
  std::vector<std::pair<int, int>>::iterator iter;
  iter = std::find(trackvector.begin(), trackvector.end(), thispair);
  int where = iter - trackvector.begin();
  if (fVerbose != 0)
    std::cout << "where " << where << std::endl;
  trackvector.erase(iter);
  std::vector<int>::iterator iter2;
  iter2 = std::find(notassignedhits.begin(), notassignedhits.end(), ihit);
  int where2 = iter2 - notassignedhits.begin();
  if (where2 != (int)notassignedhits.size())
    notassignedhits.push_back(ihit); // put it in not assigned list
  if (fVerbose > 0)
    cout << "DELETE HIT " << ihit << " FROM TRK " << itrk << endl;
}

std::vector<int> PndSttMvdGemTracking::GetTracksAssociatedToHit(Int_t ihit)
{
  // which track/s does hit # belong to
  std::vector<int> thishittracks;
  std::vector<std::pair<int, int>>::iterator iter;
  for (iter = trackvector.begin(); iter != trackvector.end(); iter++) {
    std::pair<int, int> thispair = *iter;
    if (thispair.second == ihit)
      thishittracks.push_back(thispair.first);
  }
  if (fVerbose != 0) {
    std::cout << "hit " << ihit << " belongs to " << thishittracks.size() << " tracks: ";
    for (size_t j = 0; j < thishittracks.size(); j++)
      std::cout << thishittracks[j] << " ";
    std::cout << std::endl;
  }
  return thishittracks;
}

std::vector<int> PndSttMvdGemTracking::GetHitsAssociatedToTrack(Int_t itrk)
{
  std::vector<int> thistrackhits;
  std::vector<std::pair<int, int>>::iterator iter;
  for (iter = trackvector.begin(); iter != trackvector.end(); iter++) {
    std::pair<int, int> thispair = *iter;
    if (thispair.first == itrk)
      thistrackhits.push_back(thispair.second);
  }
  if (fVerbose != 0) {
    std::cout << "track " << itrk << " has " << thistrackhits.size() << " hits: ";
    for (size_t j = 0; j < thistrackhits.size(); j++)
      std::cout << thistrackhits[j] << " ";
    std::cout << std::endl;
  }
  return thistrackhits;
}

std::vector<int> PndSttMvdGemTracking::GetHitsAssociatedToTrackOnPlane(Int_t itrk, Int_t ipos)
{
  //  cout  << "GetHitsAssociatedToTrackOnPlane " << itrk << " " << ipos << endl;
  int hitonsensor = (int)hitcounter(ipos, 0);
  std::vector<int> thistrackhitsonplane = GetHitsAssociatedToTrack(itrk);
  //  cout << "total associated hits " << thistrackhitsonplane.size() << endl;
  std::vector<int>::iterator iter;
  for (iter = thistrackhitsonplane.begin(); iter != thistrackhitsonplane.end(); iter++) {
    Int_t ihit = *iter;
    //    cout << "loop on " << hitonsensor << " hits" << endl;
    Bool_t hitfound = kFALSE;
    for (int jhit = 0; jhit < hitonsensor; jhit++) {
      Int_t hitindex = (int)hitmap(ipos, jhit);
      //  cout << "ihit: ipos, jhit, hitmap " << ihit << " " << ipos << " " << jhit << " " << hitindex << endl;
      if (hitindex == ihit)
        hitfound = kTRUE;
    }
    if (hitfound == kFALSE) {
      //  cout << "erase " << ihit << endl;
      thistrackhitsonplane.erase(iter);
      iter--;
    }
  }
  return thistrackhitsonplane;
}

Int_t PndSttMvdGemTracking::GetPosIndex(PndGemHit *hit)
{

  int istat = hit->GetStationNr();
  int isens = hit->GetSensorNr();

  // translate posindex to index in ordering
  // 11 ==> 0
  // 12 ==> 1
  // ...
  int posindex = istat * 10 + isens;

  // CHECK delete this, works only for ideal GEM hit prod *********
  if (IDEAL == true) {
    if (hit->GetZ() < 117)
      posindex = 11;
    else if (hit->GetZ() < 118)
      posindex = 12;
    else if (hit->GetZ() < 153)
      posindex = 21;
    else if (hit->GetZ() < 154)
      posindex = 22;
    else if (hit->GetZ() < 189)
      posindex = 31;
    else if (hit->GetZ() < 190)
      posindex = 32;
  }
  // *****************************************************************

  return posindex;
}

void PndSttMvdGemTracking::AddRemainingHits(Int_t)
{ // ntracks //[R.K.03/2017] unused variable(s)

  if (fVerbose > 0)
    cout << "ADD REMAINING HITS to " << CountTracks() << " tracks " << endl;

  for (Int_t k = 0; k < CountTracks(); k++) {
    int itrk = GetTrackIndex(k);
    if (fProTracks[itrk] == false)
      continue;
    if (fVerbose > 0)
      cout << k << " ITRK " << itrk << endl;

    std::vector<int> hitvector = GetHitsAssociatedToTrack(itrk);
    Int_t nhitinthistrack = hitvector.size();

    // if all the positions are filled then continue
    if (nhitinthistrack == fNPositions) {
      if (fVerbose > 0)
        cout << "all pos filled" << endl;
      continue;
    }

    // create a vector of all the positions and
    // initialize it as all full
    std::vector<int> missingpositions;
    for (int ipos = 0; ipos < fNPositions; ipos++)
      missingpositions.push_back(ipos);

    // run over the track hits and delete from
    // the list the positions already filled
    std::vector<int>::iterator iter;
    for (size_t i = 0; i < hitvector.size(); i++) {
      PndGemHit *gemhit = (PndGemHit *)fGemHitArray->At(hitvector[i]);
      if (!gemhit)
        continue;

      fOrderingIterator = find(fOrdering.begin(), fOrdering.end(), GetPosIndex(gemhit));
      int ipos = fOrderingIterator - fOrdering.begin();
      iter = std::find(missingpositions.begin(), missingpositions.end(), ipos);
      Int_t where = iter - missingpositions.begin();
      if (where != (int)missingpositions.size()) {
        if (fVerbose > 0)
          cout << "here" << endl; // CHECK  delete it
        missingpositions.erase(iter);
      }
    }

    // for each missing pos check
    // whether there are hits there
    // or not
    for (size_t i = 0; i < missingpositions.size(); i++) {
      Int_t ipos = missingpositions[i];
      if (fVerbose > 0)
        cout << "missing position " << ipos << endl;
      // if there are not hits continue
      if (towhichplane[ipos] == false)
        continue;

      // if there are, run over them and pick the closest
      // if it is below limit distance
      Double_t tmpdist = 1000;
      Int_t tmphitindex = -1;
      for (Int_t ihit = 0; ihit < hitcounter(ipos, 0); ihit++) {
        int hitindex = (int)hitmap(ipos, ihit);
        if (GetTracksAssociatedToHit(hitindex).size() != 0)
          continue;
        PndGemHit *gemhit = (PndGemHit *)fGemHitArray->At(hitindex);
        if (!gemhit)
          continue;
        if (CHECKCOMBI > 0)
          if (fCombiMap[hitindex] != 0)
            continue;
        if (fVerbose > 0)
          cout << "distance " << distancemap[itrk][hitindex] << endl;
        if (distancemap[itrk][hitindex] == -1)
          continue;
        if (distancemap[itrk][hitindex] < tmpdist) {
          tmpdist = distancemap[itrk][hitindex];
          tmphitindex = hitindex;
        }
      }
      if (tmphitindex != -1) {
        AddHitToTrack(tmphitindex, itrk);
        if (fVerbose > 0)
          cout << "ADDED " << tmphitindex << " TO TRK " << itrk << " at distance " << tmpdist << endl;
      }
    }
  }
}

void PndSttMvdGemTracking::Retrack()
{

  if (fVerbose > 0)
    cout << "RETRACKING" << endl;

  for (Int_t k = 0; k < CountTracks(); k++) {
    int itrk = GetTrackIndex(k);
    if (fProTracks[itrk] == false)
      continue;

    std::vector<int> alreadyassociatedhits = GetHitsAssociatedToTrack(itrk);
    if (fVerbose > 0) {
      cout << "TRK " << itrk << " has hits ";
      for (size_t ihit = 0; ihit < alreadyassociatedhits.size(); ihit++)
        cout << " " << alreadyassociatedhits[ihit];
      cout << endl;
    }

    std::vector<int> hitvector = GetHitsAssociatedToTrack(itrk);
    // Int_t nhitinthistrack = hitvector.size(); //[R.K. 01/2017] unused variable?

    PndTrack *completeTrack = (PndTrack *)fCompleteTrackArray->At(itrk);
    if (!completeTrack)
      continue;

    // parameters on last stt
    FairTrackParP par = completeTrack->GetParamLast();

    FairTrackParP *gempar = new FairTrackParP();

    FairTrackParP tmppar = SetStartParameters(completeTrack, completeTrack->GetTrackCandPtr());

    for (int ipos = 0; ipos < fNPositions; ipos++) {
      // if no hit then continue
      if (towhichplane[ipos] == false)
        continue;
      std::vector<int> alreadyassociatedhitsonplane = GetHitsAssociatedToTrackOnPlane(itrk, ipos);
      if (alreadyassociatedhitsonplane.size() > 1)
        cout << "ERROR!! more than 1 hit on a plane assigned! " << alreadyassociatedhitsonplane.size() << endl; // CHECK delete this

      // ...else propagate here
      Bool_t prop = PropagateToGemPlane(&tmppar, gempar, ipos);

      if (prop == kFALSE) {
        countprop[fTurn - 1][ipos]++;
        continue; // CHECK (continue or break?)
      }

      //         cout << "propagation successful" << endl;

      // if the propagation was successful ...
      // assign the hits to track itrk, extrapolated to gempar on ipos
      std::vector<int> assignedhits = AssignHits(itrk, gempar, ipos);

      // cout << "on plane " << ipos << " there are " << assignedhits.size() << "(there were " << alreadyassociatedhitsonplane.size() << ")" << endl;
      // if there is no associated hit, continue
      if (assignedhits.size() == 0) {
        // if there was no hit associated before,
        // continue, since no kalman is possible
        if (alreadyassociatedhitsonplane.size() == 0)
          continue;
        else { // if there was a hit, delete it!
          int oldhit = alreadyassociatedhitsonplane[0];
          if (fVerbose > 0)
            cout << "delete old hit " << oldhit << endl;
          DeleteHitFromTrack(oldhit, itrk);
          continue;
        }
      }

      // if there is more than one hit, take the
      // minimum distance
      if (assignedhits.size() > 1) {

        std::vector<int>::iterator iter;
        Int_t tmphit = -1;
        Double_t tmpdistance = 1000;

        // loop over the hits
        for (iter = assignedhits.begin(); iter != assignedhits.end(); iter++) {
          std::vector<int>::iterator iter2;
          Int_t ihit = *iter;
          if (CHECKCOMBI > 0 && fCombiMap[ihit] != 0) {
            assignedhits.erase(iter);
            iter--;
            continue;
          }
          if (distancemap[itrk][ihit] < tmpdistance) {
            iter2 = std::find(assignedhits.begin(), assignedhits.end(), tmphit);
            int where = iter2 - assignedhits.begin();
            if (where != (int)assignedhits.size()) {
              if (fVerbose > 0)
                cout << "deleting old " << *iter2 << endl;
              assignedhits.erase(iter2); // remove from assigned list
              iter--;
            }
            tmpdistance = distancemap[itrk][ihit];
            tmphit = ihit;
          } else {
            if (fVerbose > 0)
              cout << "deleting new " << *iter << endl;
            assignedhits.erase(iter); // remove from assigned list
            iter--;
          }
        }
      }
      if (assignedhits.size() > 1)
        cout << "ERROR!! still more than 1 hit assigned!" << endl; // CHECK delete this

      // now we have only one hit assigned:
      // is it the one already assigned?
      std::vector<int>::iterator iter;
      iter = std::find(alreadyassociatedhits.begin(), alreadyassociatedhits.end(), assignedhits[0]);
      int where = iter - alreadyassociatedhits.begin();

      // if not...
      if (where == (int)alreadyassociatedhits.size()) {
        // if there was a previosly assigned hit
        // here, then make the substitution
        if (alreadyassociatedhitsonplane.size() > 0) {
          // there is only one hit on this plane
          int oldhit = alreadyassociatedhitsonplane[0];
          if (fVerbose > 0)
            cout << "deleting old hit2 " << oldhit << endl;
          DeleteHitFromTrack(oldhit, itrk);
        }
        if (fVerbose > 0)
          cout << "adding " << assignedhits[0] << " to track " << itrk << endl;
        AddHitToTrack(assignedhits[0], itrk);
      }

      if (GetHitsAssociatedToTrackOnPlane(itrk, ipos).size() == 0)
        continue;
      // retrieve hit
      Int_t finalhit = GetHitsAssociatedToTrackOnPlane(itrk, ipos)[0];

      PndGemHit *gemhit = (PndGemHit *)fGemHitArray->At(finalhit);
      if (!gemhit)
        continue;

      // kalman filter ----------------------

      // extrapolated point
      TMatrixT<double> extrap(5, 1);
      extrap[0][0] = gempar->GetQp();
      extrap[1][0] = gempar->GetTV();
      extrap[2][0] = gempar->GetTW();
      extrap[3][0] = gempar->GetV();
      extrap[4][0] = gempar->GetW();

      TMatrixT<double> extrap_cov(5, 5);
      Double_t covMat[15], covMat55[5][5];
      gempar->GetCov(covMat); // CHECK
      FairGeaneUtil util;
      util.FromVec15ToMat25(covMat, covMat55);
      for (int icov = 0; icov < 5; icov++)
        for (int jcov = 0; jcov < 5; jcov++)
          extrap_cov(icov, jcov) = covMat55[icov][jcov];

      // measurement matrix
      TMatrixT<double> H(2, 5);
      H[0][0] = 0.;
      H[0][1] = 0.;
      H[0][2] = 0.;
      H[0][3] = 1.;
      H[0][4] = 0.;

      H[1][0] = 0.;
      H[1][1] = 0.;
      H[1][2] = 0.;
      H[1][3] = 0.;
      H[1][4] = 1.;

      TMatrixT<double> measurement(2, 1);
      measurement[0][0] = gemhit->GetX();
      measurement[1][0] = gemhit->GetY();

      Double_t phi = TMath::ATan2(gemhit->GetY(), gemhit->GetX());
      Double_t sinp = TMath::Sin(phi);
      Double_t cosp = TMath::Cos(phi);
      // Double_t raddist = gemhit->GetPosition().Perp(); //[R.K. 01/2017] unused variable?
      Double_t dR, dP;
      if (IDEAL == true) {
        dR = 0.01;
        dP = 0.08;
      } else {
        dR = gemhit->GetDr();
        dP = gemhit->GetDp();
      }
      Double_t dPhi = dP; // / raddist;
      Double_t errx = TMath::Sqrt(cosp * cosp * dR * dR + sinp * sinp * dPhi * dPhi);
      Double_t erry = TMath::Sqrt(sinp * sinp * dR * dR + cosp * cosp * dPhi * dPhi);

      // CHECK covariances??
      TMatrixT<double> measurement_cov(2, 2);
      measurement_cov[0][0] = errx * errx;
      measurement_cov[1][1] = erry * erry;

      TMatrixT<double> kalman(5, 1);
      TMatrixT<double> kalman_cov(5, 5);

      // kalman
      Kalman(extrap, measurement, H, extrap_cov, measurement_cov, kalman, kalman_cov);

      Double_t kalmanCov15[15], kalmanCov55[5][5];
      for (int icov = 0; icov < 5; icov++)
        for (int jcov = 0; jcov < 5; jcov++)
          kalmanCov55[icov][jcov] = kalman_cov(icov, jcov);
      util.FromMat25ToVec15(kalmanCov55, kalmanCov15);

      tmppar.SetTrackPar(kalman[3][0], kalman[4][0], kalman[1][0], kalman[2][0], kalman[0][0],
                         // covMat,
                         kalmanCov15, // CHECK NOW
                         gempar->GetOrigin(), gempar->GetIVer(), gempar->GetJVer(), gempar->GetKVer(),
                         gempar->GetSPU()); // CHECK recalculate spu
    }
  }
}

Double_t PndSttMvdGemTracking::IsAssignable(FairTrackParP *gempar, PndGemHit *gemhit)
{

  TVector3 gemErr(gempar->GetDV(), gempar->GetDW(),
                  0.0); // gempar->GetDZ());

  // error of gemhit
  // dr = radial
  // dp = perpendicular to radius
  // dp ~ dl = dphi * R --> dphi ~  dp / R
  Double_t phi = TMath::ATan2(gemhit->GetY(), gemhit->GetX());
  Double_t sinp = TMath::Sin(phi);
  Double_t cosp = TMath::Cos(phi);
  // Double_t raddist = gemhit->GetPosition().Perp(); //[R.K. 01/2017] unused variable?
  Double_t dR, dP;
  if (IDEAL == true) {
    dR = 0.01;
    dP = 0.08;
  } else {
    dR = gemhit->GetDr();
    dP = gemhit->GetDp();
  }
  Double_t dPhi = dP; // / raddist;
  Double_t errx = TMath::Sqrt(cosp * cosp * dR * dR + sinp * sinp * dPhi * dPhi);
  Double_t erry = TMath::Sqrt(sinp * sinp * dR * dR + cosp * cosp * dPhi * dPhi);

  TVector3 hitErr(errx, erry, 0.0); // CHECK
  if (fVerbose > 0) {
    cout << "errors " << endl;
    gemErr.Print();
    hitErr.Print();
  }
  TVector3 distVec = gempar->GetPosition() - gemhit->GetPosition();
  double distance = distVec.Perp(); // CHECK using xy distance ON plane
  // error on distance: sqrt((x - x0)**2 * (sigx**2 + sigx0**2) +
  //                          ... (y) ...) / distance
  // with x,  y,  z = extrap pos/err
  //      x0, y0, z0 = gem hit pos/err
  double distErr = (1. / distance) * TMath::Sqrt((distVec.X() * distVec.X()) * (gemErr.X() * gemErr.X() + hitErr.X() * hitErr.X()) +
                                                 (distVec.Y() * distVec.Y()) * (gemErr.Y() * gemErr.Y() + hitErr.Y() * hitErr.Y())); // CHECK if distance == 0

  if (fVerbose > 0)
    cout << "distance " << distance << " err " << distErr << endl;

  // maximum distance **************************
  double maxdistance = 0;
  if (fMaxDistance != -1)
    maxdistance = fMaxDistance;
  else {
    maxdistance = fTimes * distErr; // distErr; // CHECK if it has to be distErr ; // CHECK put this as a parameter and tune it
    if (maxdistance < 1)
      maxdistance = 1;
    if (maxdistance > 10)
      maxdistance = 10;
  }
  if (fTurn == 2 && gempar->GetMomentum().Mag() >= 0.5)
    maxdistance = 5; // CHECK NOW

  //  cout << "-> " << fTimes << " " << fTurn << " " << maxdistance << endl;

  int posindex = GetPosIndex(gemhit);
  fOrderingIterator = find(fOrdering.begin(), fOrdering.end(), posindex);
  int ipos = fOrderingIterator - fOrdering.begin();
  if (fTurn == 1) {
    hdist[ipos]->Fill(distance);
    hsigma[ipos]->Fill(maxdistance);
  } else if (fTurn == 2) {
    hdist2[ipos]->Fill(distance);
    hsigma2[ipos]->Fill(maxdistance);
  }

  if (fDisplayOn) {
    //    int posindex = GetPosIndex(gemhit);
    //     fOrderingIterator = find(fOrdering.begin(), fOrdering.end(), posindex);
    //     int ipos = fOrderingIterator - fOrdering.begin();

    TArc *earc = new TArc(gempar->GetPosition().X(), gempar->GetPosition().Y(), maxdistance);
    earc->SetFillStyle(0);
    if (fTurn == 1)
      earc->SetLineColor(3); // green
    else if (fTurn == 2)
      earc->SetLineColor(2); // red
    display->cd(ipos + 1);
    earc->Draw("SAME");
    display->Update();
    display->Modified();
  }

  if (distance <= maxdistance) {
    if (fTurn == 1)
      hchosen[ipos]->Fill(distance);
    else if (fTurn == 2)
      hchosen2[ipos]->Fill(distance);
    return distance;
  }
  return -1;
}

std::vector<int> PndSttMvdGemTracking::AssignHits(Int_t itrk, FairTrackParP *gempar, Int_t ipos)
{
  std::vector<int> assignedhits; //  CHECK need init?
  // compare the position with each *******************************
  // GEM hit on THIS sensor
  int hitonsensor = (int)hitcounter(ipos, 0);
  for (int ihit = 0; ihit < hitonsensor; ihit++) {
    int hitindex = (int)hitmap(ipos, ihit);
    PndGemHit *gemhit = (PndGemHit *)fGemHitArray->At(hitindex);
    if (!gemhit)
      continue;
    if (CHECKCOMBI == 2 && fCombiMap[hitindex] != 0)
      continue;
    Double_t distance = IsAssignable(gempar, gemhit);
    // if the track to hit distance is below threshold
    // "distance" is filled (-1 otherwise)
    if (distance >= 0.) {
      // the separate coordinates are not working at all ****
      //   if((distVec.X()/ TMath::Sqrt(hitErr.X() * hitErr.X() + gemErr.X() * gemErr.X())) <= 5  &&  (distVec.Y()/ TMath::Sqrt(hitErr.Y() * hitErr.Y() + gemErr.Y() * gemErr.Y()))
      //   <= 5) { // CHECK delete this and do not use it!
      // COUNTERS 167 0 0 210 43 43 0
      // COUNTERS 740 627 16 1101 -266 234 2
      // COUNTERS 241 385 8 359 -267 64 0

      // COUNTERS 189 0 0 215 26 26 4
      // COUNTERS 729 318 32 770 -277 87 3
      // COUNTERS 87 66 0 91 -62 1 0

      // COUNTERS 205 1 0 211 5 6 7
      // COUNTERS 806 300 13 838 -268 53 14
      // COUNTERS 1209 1087 0 1281 -1015 31 2
      // ****************************************************

      if (fVerbose > 0)
        cout << "this hit " << hitindex << " BELONGS to this track " << itrk << "!!!!!!!!!!!!!" << endl;
      // fill the distancemap: for each track compute its
      // distance from each hit and put it in the map
      distancemap[itrk][hitindex] = distance;

      // assign it to all the tracks it might belong to (for now)
      //	    completeCand->AddHit(FairRootManager::Instance()->GetBranchId(fGemBranchName), hitindex, gemhit->GetPosition().Mag());  // CHECK rho and kGemHit
      if (fVerbose != 0)
        cout << "assign " << hitindex << " to track " << itrk << endl;
      //      AddHitToTrack(hitindex, itrk);
      assignedhits.push_back(hitindex);

      if (fDisplayOn) {
        TMarker *g1mrk = new TMarker(gemhit->GetX(), gemhit->GetY(), 7);
        g1mrk->SetMarkerColor(4); // blue
        display->cd(1 + ipos);
        g1mrk->Draw("SAME");
        display->Update();
        display->Modified();
      }
    } else if (fVerbose > 0)
      cout << "this hit " << hitindex << " DOES NOT BELONG to this track " << itrk << "!!!!!!!!!!!!!" << endl;
  }

  return assignedhits;
}

void PndSttMvdGemTracking::Kalman(TMatrixT<double> extrap, TMatrixT<double> measurement, TMatrixT<double> H, TMatrixT<double> extrap_cov, TMatrixT<double> measurement_cov,
                                  TMatrixT<double> &kalman, TMatrixT<double> &kalman_cov)
{

  // gain
  // calculate sum of covariances = sum_cov = m_cov + H e_cov HT
  TMatrixT<double> extrapHT_cov(extrap_cov, TMatrixT<double>::kMultTranspose, H);
  TMatrixT<double> HextrapHT_cov(H, TMatrixT<double>::kMult, extrapHT_cov);
  TMatrixT<double> sum_cov = measurement_cov + HextrapHT_cov;
  // double det = 0; // CHECK fill it  //[R.K. 01/2017] unused variable?
  sum_cov.Invert();
  // calculate gain
  TMatrixT<double> Hsum_cov(H, TMatrixT<double>::kTransposeMult, sum_cov);
  TMatrixT<double> gain(extrap_cov, TMatrixT<double>::kMult, Hsum_cov);

  // kalman state
  //      extrap + gain * (measurement - H extrap)
  TMatrixT<double> Hextrap(H, TMatrixT<double>::kMult, extrap);
  TMatrixT<double> meas_Hextrap = measurement - Hextrap;
  TMatrixT<double> gainmeas_Hextrap(gain, TMatrixT<double>::kMult, meas_Hextrap);
  kalman = extrap + gainmeas_Hextrap;

  // kalman cov matrix
  // kalman_cov = (identity - gain H) extrap_cov
  TMatrixT<double> gainH(gain, TMatrixT<double>::kMult, H);
  TMatrixT<double> I(5, 5);
  for (int imat = 0; imat < 5; imat++) {
    for (int jmat = 0; jmat < 5; jmat++) {
      I[imat][jmat] = 0;
      if (imat == jmat)
        I[imat][jmat] = 1;
    }
  }
  TMatrixT<double> I_gainH = I - gainH;
  kalman_cov = TMatrixT<double>(I_gainH, TMatrixT<double>::kMult, extrap_cov);

  // kalman
  //       cout << "kalman" << endl;
  //       kalman.Print();
  //       kalman_cov.Print();
}

FairTrackParP PndSttMvdGemTracking::SetStartParameters(PndTrack *sttmvd, PndTrackCand *sttmvdCand)
{

  // in the following cuts, if it does not work, return this!
  FairTrackParP lastpar = sttmvd->GetParamLast();

  if (fVerbose > 0) {
    cout << "start from " << endl;
    lastpar.GetPosition().Print();
    cout << "lastpar error " << lastpar.GetDX() << " " << lastpar.GetDY() << " " << lastpar.GetDZ() << endl;
    // lastpar.GetMomentum().Print();
  }

  FairTrackParP startpar;
  if (fUseMC) {
    // --------------------------------------------------
    Int_t mcIndex = sttmvdCand->getMcTrackId();
    if (mcIndex == -1)
      return lastpar;
    PndMCTrack *mctrk = (PndMCTrack *)fMCTrackArray->At(mcIndex);
    //  FairTrackParP tmppar(mctrk->GetStartVertex(), mctrk->GetMomentum(),
    // 			   TVector3(0.1, 0.1, 0.1), TVector3(0.1, 0.1, 0.1),
    // 			   TDatabasePDG::Instance()->GetParticle(mctrk->GetPdgCode())->Charge()/3.,
    // 			   mctrk->GetStartVertex(), TVector3(1., 0., 0.), TVector3(0., 1., 0.));

    // start from MC last point on STT
    Int_t nsttmvdhits = sttmvdCand->GetNHits();
    PndTrackCandHit candhit = sttmvdCand->GetSortedHit(nsttmvdhits - 1);
    Int_t iHit = candhit.GetHitId();
    Int_t detId = candhit.GetDetId();
    FairHit *fhit = nullptr;
    FairMCPoint *fpnt = nullptr;

    //     cout << "LAST HIT " << detId
    // 	 << " " << FairRootManager::Instance()->GetBranchId(fMvdStripBranchName)
    // 	 << " " << FairRootManager::Instance()->GetBranchId(fMvdPixelBranchName)
    // 	 << " " << FairRootManager::Instance()->GetBranchId(fSttBranchName)
    // 	 << endl;

    if (detId != FairRootManager::Instance()->GetBranchId(fSttBranchName))
      return lastpar;

    if (detId == FairRootManager::Instance()->GetBranchId(fMvdStripBranchName)) {
      fhit = (FairHit *)fMvdStripHitArray->At(iHit);
      if (!fhit)
        return lastpar;
      fpnt = (FairMCPoint *)fMvdPointArray->At(fhit->GetRefIndex());
      if (!fpnt)
        return lastpar;
    } else if (detId == FairRootManager::Instance()->GetBranchId(fMvdPixelBranchName)) {
      fhit = (FairHit *)fMvdPixelHitArray->At(iHit);
      if (!fhit)
        return lastpar;
      fpnt = (FairMCPoint *)fMvdPointArray->At(fhit->GetRefIndex());
      if (!fpnt)
        return lastpar;
    } else if (detId == FairRootManager::Instance()->GetBranchId(fSttBranchName)) {
      fhit = (FairHit *)fSttHitArray->At(iHit);
      if (!fhit)
        return lastpar;
      fpnt = (FairMCPoint *)fSttPointArray->At(fhit->GetRefIndex());
      if (!fpnt)
        return lastpar;
    }

    startpar = FairTrackParP(TVector3(fpnt->GetX(), fpnt->GetY(), fpnt->GetZ()), TVector3(fpnt->GetPx(), fpnt->GetPy(), fpnt->GetPz()), TVector3(0.1, 0.1, 0.1),
                             TVector3(0.1, 0.1, 0.1), (int)TMath::Sign(1., TDatabasePDG::Instance()->GetParticle(mctrk->GetPdgCode())->Charge() / 3.),
                             TVector3(fpnt->GetX(), fpnt->GetY(), fpnt->GetZ()), TVector3(1., 0., 0.), TVector3(0., 1., 0.));
  } else {
    //    startpar = lastpar;
    //     cout << "from PR " << endl;
    //      lastpar.GetPosition().Print();
    //      lastpar.GetMomentum().Print();

    bool startpoint = false;
    // PndGemStation *station = fGemParameters->GetStation(0); //[R.K. 01/2017] unused variable?
    // PndGemSensor *sensor = station->GetSensor(0);  //[R.K. 01/2017] unused variable?
    TVector3 position;
    TVector3 momentum;
    //     if(fabs(lastpar.GetPosition().X()) > 42. || fabs(lastpar.GetPosition().Y()) > 42. ||
    //        lastpar.GetMomentum().Mag() < 0.15 ||
    //        lastpar.GetPosition().Z() > sensor->GetZ0())
    {
      startpoint = Prefit(sttmvd, sttmvdCand, position, momentum);
    }

    if (startpoint == true) {
      // find plane orthogonal to mom
      TVector3 iver = momentum;
      iver.SetMag(1.);
      TVector3 jver = momentum.Orthogonal();
      jver.SetMag(1.);
      TVector3 kver = iver.Cross(jver);
      kver.SetMag(1.);
      // by hand
      startpar = FairTrackParP(position, momentum, TVector3(lastpar.GetDX(), lastpar.GetDY(), 3.), TVector3(lastpar.GetDPx(), lastpar.GetDPy(), lastpar.GetDPz()), lastpar.GetQ(),
                               position, jver, kver);
    } else {
      // by hand
      startpar = FairTrackParP(lastpar.GetPosition(), lastpar.GetMomentum(), TVector3(lastpar.GetDX(), lastpar.GetDY(), 3.),
                               TVector3(lastpar.GetDPx(), lastpar.GetDPy(), lastpar.GetDPz()), lastpar.GetQ(), lastpar.GetOrigin(), lastpar.GetJVer(), lastpar.GetKVer());
    }

    // kalman filter between the lastpar and the last hit

    // Int_t nsttmvdhits = sttmvdCand->GetNHits();
    //     PndTrackCandHit candhit = sttmvdCand->GetSortedHit(nsttmvdhits - 1);
    //     Int_t iHit = candhit.GetHitId();
    //     Int_t detId = candhit.GetDetId();

    //     // CHECK
    //     if(detId != FairRootManager::Instance()->GetBranchId(fSttBranchName)) return lastpar;

    //     PndSttHit *stthit = (PndSttHit*) fSttHitArray->At(iHit);
    //     if(!stthit) return lastpar;

    //     // extrapolated point
    //     TMatrixT<double> extrap(5, 1);
    //     extrap[0][0] = lastpar.GetQp();
    //     extrap[1][0] = lastpar.GetTV();
    //     extrap[2][0] = lastpar.GetTW();
    //     extrap[3][0] = lastpar.GetV();
    //     extrap[4][0] = lastpar.GetW();

    //     TMatrixT<double> extrap_cov(5, 5);
    //     Double_t covMat[15], covMat55[5][5];
    //     lastpar.GetCov(covMat); // CHECK
    //     FairGeaneUtil util;
    //     util.FromVec15ToMat25(covMat, covMat55);
    //     for(int icov = 0; icov < 5; icov++) for(int jcov = 0; jcov < 5; jcov++) extrap_cov(icov, jcov) = covMat55[icov][jcov];

    //     // measurement matrix
    //     TMatrixT<double> H(1, 5);
    //     H[0][0] = 0.;
    //     H[0][1] = 0.;
    //     H[0][2] = 0.;
    //     H[0][3] = 1.;
    //     H[0][4] = 0.;

    //     TMatrixT<double> measurement(1, 1);
    //     measurement[0][0] = stthit->GetIsochrone();

    //     TMatrixT<double> measurement_cov(1, 1);
    //     measurement_cov[0][0] = stthit->GetIsochroneError() * stthit->GetIsochroneError();

    //     TMatrixT<double> kalman(5, 1);
    //     TMatrixT<double> kalman_cov(5, 5);

    //     // kalman
    //     Kalman(extrap, measurement, H, extrap_cov, measurement_cov, kalman, kalman_cov);

    //     Double_t kalmanCov15[15], kalmanCov55[5][5];
    //     for(int icov = 0; icov < 5; icov++) for(int jcov = 0; jcov < 5; jcov++) kalmanCov55[icov][jcov] = kalman_cov(icov, jcov);
    //     util.FromMat25ToVec15(kalmanCov55, kalmanCov15);

    //     startpar.SetTrackPar(kalman[3][0], kalman[4][0],
    // 			  kalman[1][0], kalman[2][0], kalman[0][0],
    // 			  kalmanCov15, // CHECK!!
    // 			  lastpar.GetOrigin(), lastpar.GetIVer(), lastpar.GetJVer(), lastpar.GetKVer(),
    // 			  lastpar.GetSPU()); // CHECK recalculate spu
    //      cout << "from prefit " << endl;
    //        startpar.GetPosition().Print();
    //        startpar.GetMomentum().Print();
  }

  if (fVerbose) {
    cout << "from prefit " << endl;
    startpar.GetPosition().Print();
    startpar.GetMomentum().Print();
  }

  return startpar;
}

Int_t PndSttMvdGemTracking::SelectPdgCode(PndTrack *sttmvd)
{

  FairTrackParP first = sttmvd->GetParamFirst();
  FairTrackParP last = sttmvd->GetParamLast();

  // first plane
  //  TVector3 o1  = firs.GetOrigin();
  TVector3 dj1 = first.GetJVer();
  TVector3 dk1 = first.GetKVer();

  // last plane
  TVector3 o2 = last.GetOrigin();
  TVector3 dj2 = last.GetJVer();
  TVector3 dk2 = last.GetKVer();

  fPro->PropagateFromPlane(dj1, dk1);
  fPro->PropagateToPlane(o2, dj2, dk2);

  FairTrackParP *propag = new FairTrackParP();
  Bool_t prop = kFALSE;
  int charge = first.GetQ();
  int pdg[5] = {-11 * charge, -13 * charge, 211 * charge, 321 * charge, 2212 * charge};
  int tmppdg = 0;
  double tmpdistance = 100000;
  for (int ipdg = 0; ipdg < 5; ipdg++) {
    prop = fPro->Propagate(&first, propag, pdg[ipdg]);
    if (prop) {
      TVector3 proppos = propag->GetPosition();
      TVector3 lastpos = last.GetPosition();
      double distance = (lastpos - proppos).Mag();
      cout << "this pdg " << pdg[ipdg] << " " << distance << endl;

      if (distance < tmpdistance) {
        tmpdistance = distance;
        tmppdg = pdg[ipdg];
      }
    }
  }
  cout << "CHOSEN PDG " << tmppdg << " " << tmpdistance << endl;
  return tmppdg;
}

void PndSttMvdGemTracking::WriteHistograms()
{

  //  TFile *file = FairRootManager::Instance()->GetOutFile();
  //  file->cd();
  //  file->mkdir("PndSttMvdGemTracking");
  //  file->cd("PndSttMvdGemTracking");

  FairSink *sink = FairRootManager::Instance()->GetSink();

  if (sink->GetSinkType() == kFILESINK) {
    TDirectory::TContext restorecwd{};
    TFile *outfile = dynamic_cast<FairRootFileSink *>(sink)->GetRootFile();
    outfile->mkdir("PndSttMvdGemTracking");
    outfile->cd("PndSttMvdGemTracking");

    for (int ipos = 0; ipos < fNPositions; ipos++) {
      outfile->WriteTObject(hdist[ipos]);
      delete hdist[ipos];
      outfile->WriteTObject(hdist2[ipos]);
      delete hdist2[ipos];
      outfile->WriteTObject(hsigma[ipos]);
      delete hsigma[ipos];
      outfile->WriteTObject(hsigma2[ipos]);
      delete hsigma2[ipos];
      hchosen[ipos]->SetFillColor(3);
      outfile->WriteTObject(hchosen[ipos]);
      delete hchosen[ipos];
      hchosen2[ipos]->SetFillColor(2);
      outfile->WriteTObject(hchosen2[ipos]);
      delete hchosen2[ipos];
      hmcdist[ipos]->SetFillColor(4);
      outfile->WriteTObject(hmcdist[ipos]);
      delete hmcdist[ipos];
      hmcx[ipos]->SetFillColor(4);
      outfile->WriteTObject(hmcx[ipos]);
      delete hmcx[ipos];
      hmcy[ipos]->SetFillColor(4);
      outfile->WriteTObject(hmcy[ipos]);
      delete hmcy[ipos];
    }
  }
}

void PndSttMvdGemTracking::FillTrueDistances()
{

  // loop on tracks
  //  cout << "FOUND TRACKS " << fTrackArray->GetEntriesFast() << endl;
  for (Int_t itrk = 0; itrk < fTrackArray->GetEntriesFast(); itrk++) {
    PndTrack *sttmvdTrack = (PndTrack *)fTrackArray->At(itrk);
    if (!sttmvdTrack)
      continue;
    PndTrackCand *sttmvdCand = sttmvdTrack->GetTrackCandPtr();
    if (!sttmvdCand)
      continue;
    if (fProTracks[itrk] == kFALSE)
      continue;
    Int_t mcIndex = sttmvdCand->getMcTrackId();
    //   cout << "TRACK " << itrk << " HAS MC " << mcIndex << endl;
    if (mcIndex == -1)
      continue;
    PndMCTrack *mctrk = (PndMCTrack *)fMCTrackArray->At(mcIndex);
    if (mctrk) {
      // int pdgcode = mctrk->GetPdgCode(); //[R.K. 01/2017] unused variable?
      // int motherid = mctrk->GetMotherID(); //[R.K. 01/2017] unused variable?
      TVector3 vertex = mctrk->GetStartVertex();
      TVector3 vtxmomentum = mctrk->GetMomentum();
      //    cout << "PDG " << pdgcode << " MOTHID " << motherid << " VTX POS " << vertex.Mag() << " MOM " << vtxmomentum.Mag() << endl;

      // start from MC last point on STT
      Int_t nsttmvdhits = sttmvdCand->GetNHits();
      PndTrackCandHit candhit = sttmvdCand->GetSortedHit(nsttmvdhits - 1);
      Int_t iHit = candhit.GetHitId();
      Int_t detId = candhit.GetDetId();
      FairHit *fhit = nullptr;
      FairMCPoint *fpnt = nullptr;
      //  cout << "HIT " << iHit << " FOR " << detId << endl;
      if (detId == FairRootManager::Instance()->GetBranchId(fMvdStripBranchName)) { //  CHECK
        //      if(detId == 26) {
        fhit = (FairHit *)fMvdStripHitArray->At(iHit);
        if (fhit->GetRefIndex() != -1 && fhit)
          fpnt = (FairMCPoint *)fMvdPointArray->At(fhit->GetRefIndex());
      } else if (detId == FairRootManager::Instance()->GetBranchId(fMvdPixelBranchName)) { //  CHECK
        // else if(detId == 24) {
        fhit = (FairHit *)fMvdPixelHitArray->At(iHit);
        if (fhit->GetRefIndex() != -1 && fhit)
          fpnt = (FairMCPoint *)fMvdPointArray->At(fhit->GetRefIndex());
      } else if (detId == FairRootManager::Instance()->GetBranchId(fSttBranchName)) {
        fhit = (FairHit *)fSttHitArray->At(iHit);
        if (fhit->GetRefIndex() != -1 && fhit)
          fpnt = (FairMCPoint *)fSttPointArray->At(fhit->GetRefIndex());
      }
      if (fpnt) {
        TVector3 position(fpnt->GetX(), fpnt->GetY(), fpnt->GetZ());
        TVector3 momentum(fpnt->GetPx(), fpnt->GetPy(), fpnt->GetPz());

        // 	cout << "MC/PR POS" << endl;
        // 	position.Print();
        // 	sttmvdTrack->GetParamLast().GetPosition().Print();
        // 	cout << "MC/PR MOM" << endl;
        // 	momentum.Print();
        // 	sttmvdTrack->GetParamLast().GetMomentum().Print();
      }
    }
    FairTrackParP *gempar = new FairTrackParP();
    FairTrackParP tmppar = SetStartParameters(sttmvdTrack, sttmvdCand);
    int charge = tmppar.GetQ();
    //    fPdgCode = -13  * charge;
    if (fPdgFromMC)
      fPdgCode = GetChargeCorrectedPdgFromMC(itrk, charge);
    else
      fPdgCode = fDefaultPdgCode * charge;

    // extrapolate each track on each plane
    for (int ipos = 0; ipos < fNPositions; ipos++) {

      Bool_t prop = PropagateToGemPlane(&tmppar, gempar, ipos);
      if (prop == kFALSE)
        prop = PropagateToGemPlaneAsHelix(sttmvdTrack, gempar, ipos);
      if (prop == kFALSE)
        continue; // CHECK (continue or break?)
      TVector3 extrappos = gempar->GetPosition();

      // loop on MC points
      for (int ipnt = 0; ipnt < fGemPointArray->GetEntriesFast(); ipnt++) {
        PndGemMCPoint *gempnt = (PndGemMCPoint *)fGemPointArray->At(ipnt);
        if (!gempnt)
          continue;
        Int_t pntMcIndex = gempnt->GetTrackID();
        // if the point belongs to the mctrack
        if (pntMcIndex != mcIndex)
          continue;
        TVector3 gempos(gempnt->GetX(), gempnt->GetY(), gempnt->GetZ());
        // if they are on the same plane
        if (fabs(gempos.Z() - extrappos.Z()) < 0.1) {
          double distance = (gempos - extrappos).Perp();
          hmcdist[ipos]->Fill(distance);
          hmcx[ipos]->Fill(gempos.X() - extrappos.X());
          hmcy[ipos]->Fill(gempos.Y() - extrappos.Y());
          //	  cout << "POINT " << ipnt << " TO TRACK " << itrk << "(MC " << mcIndex << ") distance " << distance << endl;
        }
      }
    }
  }
}

Int_t PndSttMvdGemTracking::GetClosestOnFirst(FairTrackParP *gempar, Int_t ipos, Double_t &closestdistance)
{
  int hitonsensor = (int)hitcounter(ipos, 0);
  Int_t tmphit = -1;
  Double_t tmpdistance = 10000;
  for (int ihit = 0; ihit < hitonsensor; ihit++) {
    int hitindex = (int)hitmap(ipos, ihit);
    PndGemHit *gemhit = (PndGemHit *)fGemHitArray->At(hitindex);
    if (!gemhit)
      continue;
    if (CHECKCOMBI == 2 && fCombiMap[hitindex] != 0)
      continue;
    TVector3 gemhitpos = gemhit->GetPosition();
    TVector3 extrapos = gempar->GetPosition();

    Double_t distance = (gemhitpos - extrapos).Perp();
    if (distance < tmpdistance) {
      tmpdistance = distance;
      tmphit = hitindex;
    }
  }
  closestdistance = tmpdistance;

  if (closestdistance < 0)
    return -1;
  return tmphit;
}

// CHECK :-)GOOD!
// -------------- Prefit  --------------------------------------
Bool_t PndSttMvdGemTracking::Prefit(PndTrack *sttmvdTrack, PndTrackCand *sttmvdCand, TVector3 &lastpos, TVector3 &lastmom)
{

  Int_t nhits = sttmvdCand->GetNHits();
  // 0     | 1     | 2 | 3 | 4 | 5  | 6  | 7  | 8   | 9      | 10
  // hitId | detid | x | y | z | dx | dy | dz | iso | isoerr | useforfit (0 = only xy, 1 = only z, 2 = both)
  TMatrixT<double> points(nhits, 11);
  Double_t xc, yc, radius, fitm, fitp;

  GetInitialParams(sttmvdTrack, xc, yc, radius, fitm, fitp);
  //  cout << " PR " << xc << " " << yc << " " << radius <<  " " << fitm <<  " " << fitp << endl;

  Int_t charge = sttmvdTrack->GetParamFirst().GetQ();
  Int_t firsthitid = -1, lasthitid = -1;
  for (int ihit = 0; ihit < nhits; ihit++) {
    // get hit
    PndTrackCandHit candhit = sttmvdCand->GetSortedHit(ihit);
    Int_t hitId = candhit.GetHitId();
    Int_t detId = candhit.GetDetId();

    points[ihit][0] = -1;
    points[ihit][1] = detId;
    points[ihit][2] = 0;
    points[ihit][3] = 0;
    points[ihit][4] = 0;
    points[ihit][5] = 0;
    points[ihit][6] = 0;
    points[ihit][7] = 0;
    points[ihit][8] = 0;
    points[ihit][9] = 0;
    points[ihit][10] = -1;

    if (detId == FairRootManager::Instance()->GetBranchId(fGemBranchName))
      continue;
    else if (detId == FairRootManager::Instance()->GetBranchId(fMvdPixelBranchName) || detId == FairRootManager::Instance()->GetBranchId(fMvdStripBranchName)) {
      PndSdsHit *mvdhit;
      if (detId == FairRootManager::Instance()->GetBranchId(fMvdPixelBranchName))
        mvdhit = (PndSdsHit *)fMvdPixelHitArray->At(hitId);
      else
        mvdhit = (PndSdsHit *)fMvdStripHitArray->At(hitId);
      if (!mvdhit) {
        cout << "mvd hit " << ihit << " " << hitId << " does not exist" << endl;
        continue;
      }
      points[ihit][0] = hitId;
      points[ihit][2] = mvdhit->GetX();
      points[ihit][3] = mvdhit->GetY();
      points[ihit][4] = mvdhit->GetZ();
      points[ihit][5] = TMath::Sqrt(mvdhit->GetCov()[0][0]);
      points[ihit][6] = TMath::Sqrt(mvdhit->GetCov()[1][1]);
      points[ihit][7] = TMath::Sqrt(mvdhit->GetCov()[2][2]);
      points[ihit][10] = 2;

      lasthitid = ihit;
      if (firsthitid == -1)
        firsthitid = ihit;
      //	cout << "MVD " << ihit << " " << hitId << " " << points[ihit][2] << " " << points[ihit][3] << " " << points[ihit][4] << endl;

    } else if (detId == FairRootManager::Instance()->GetBranchId(fSttBranchName)) {
      PndSttHit *stthit = (PndSttHit *)fSttHitArray->At(hitId);
      if (!stthit) {
        cout << "stt hit " << ihit << " " << hitId << " does not exist" << endl;
        continue;
      }
      TVector3 xyz(0, 0, 0);
      TVector3 dxyz(0, 0, 0);

      Int_t tubeID = stthit->GetTubeID();
      PndSttTube *tube = (PndSttTube *)fTubeArray->At(tubeID);

      TVector3 wireDirection = tube->GetWireDirection();
      if (wireDirection == TVector3(0., 0., 1.)) { // is parallel
        Bool_t intfin = IntersectionFinder(xc, yc, radius, stthit, xyz, dxyz);
        if (intfin == false) {
          //  cout << "intfin false" << endl;
          continue;
        }

        // CHECK get only parallel tubes?
        lasthitid = ihit;
        if (firsthitid == -1)
          firsthitid = ihit;
        //	cout << "STT " << ihit << " " << hitId << " " << points[ihit][2] << " " << points[ihit][3] << " " << points[ihit][4] << "iso "  << points[ihit][8] << endl;

        points[ihit][10] = 0;
      } else { // is skewed
        //	  continue;
        xyz = tube->GetPosition();
        points[ihit][10] = 1;
      }

      points[ihit][0] = hitId;
      points[ihit][2] = xyz.X();
      points[ihit][3] = xyz.Y();
      points[ihit][4] = xyz.Z();
      points[ihit][5] = dxyz.X();
      points[ihit][6] = dxyz.Y();
      points[ihit][7] = dxyz.Z();
      points[ihit][8] = stthit->GetIsochrone();
      points[ihit][9] = stthit->GetIsochroneError();
    }
  }

  for (int iteration = 0; iteration < 1; iteration++) {
    Bool_t fitting = Fit(points, xc, yc, radius);
    if (fitting == false) {
      // cout << "fit false" << endl;
      break;
    }
    //     cout << "FIT " << iteration << " " << xc << " " << yc << " " << radius << endl;

    for (int ihit = 0; ihit < nhits; ihit++) {

      Int_t hitId = (Int_t)points[ihit][0];
      Int_t detId = (Int_t)points[ihit][1];

      if (hitId == -1)
        continue;
      Int_t fitflag = (Int_t)points[ihit][10];
      if (fitflag != 0)
        continue;
      if (detId != FairRootManager::Instance()->GetBranchId(fSttBranchName))
        continue;
      PndSttHit *stthit = (PndSttHit *)fSttHitArray->At(hitId);
      if (!stthit)
        continue;

      TVector3 xyz(0, 0, 0);
      TVector3 dxyz(0, 0, 0);
      Bool_t intfin = IntersectionFinder(xc, yc, radius, stthit, xyz, dxyz);
      if (intfin == false) {
        // cout << "intfin false2" << endl;
        continue;
      }
      points[ihit][2] = xyz.X();
      points[ihit][3] = xyz.Y();
      points[ihit][4] = xyz.Z();
      points[ihit][5] = dxyz.X();
      points[ihit][6] = dxyz.Y();
      points[ihit][7] = dxyz.Z();
      points[ihit][8] = stthit->GetIsochrone();
      points[ihit][9] = stthit->GetIsochroneError();
      lasthitid = ihit;
      if (firsthitid == -1)
        firsthitid = ihit;
    }
  }

  // z ---------------
  //  ZFind(nhits, points, xc, yc, radius); // CHECK // to use zfinder

  Bool_t zfitting = ZFit(points, charge, xc, yc, radius, fitm, fitp);
  if (zfitting == false) {
    // cout << "zfit false " << endl;
    return false;
  }

  //  cout << "ZFIT " << fitm << " " << fitp << endl;
  if (firsthitid == -1 || lasthitid == -1)
    return kFALSE; // CHECK

  // x0 y0
  Double_t d = TMath::Sqrt(xc * xc + yc * yc) - radius;
  Double_t phi = TMath::ATan2(yc, xc);

  Double_t x0 = d * TMath::Cos(phi);
  Double_t y0 = d * TMath::Sin(phi);

  // z
  Double_t Phi0 = TMath::ATan2((y0 - yc), (x0 - xc));

  TVector2 v(x0 - xc, y0 - yc);
  Double_t alpha1 = TMath::ATan2(points[firsthitid][3] - y0 + radius * TMath::Sin(Phi0), points[firsthitid][2] - x0 + radius * TMath::Cos(Phi0));
  TVector2 p1(points[firsthitid][2] - xc, points[firsthitid][3] - yc);
  Double_t Fi1 = CalculatePhi(v, p1, alpha1, Phi0, charge);

  Double_t alpha2 = TMath::ATan2(points[lasthitid][3] - y0 + radius * TMath::Sin(Phi0), points[lasthitid][2] - x0 + radius * TMath::Cos(Phi0));
  TVector2 p2(points[lasthitid][2] - xc, points[lasthitid][3] - yc);
  Double_t Fi2 = CalculatePhi(v, p2, alpha2, Phi0, charge);
  Fi2 = CompareToPreviousPhi(Fi2, Fi1, charge);

  Double_t scos = -charge * radius * Fi2; // scos = -q * R * phi CHECK :-)GOOD!

  double z = (fitm * scos + fitp);

  double versor[2];
  versor[0] = xc - points[lasthitid][2];
  versor[1] = yc - points[lasthitid][3];

  Double_t Distance = TMath::Sqrt(versor[0] * versor[0] + versor[1] * versor[1]);
  versor[0] /= Distance;
  versor[1] /= Distance;

  double px = -charge * radius * 0.006 * versor[1];
  double py = charge * radius * 0.006 * versor[0];
  double pz = TMath::Sqrt(px * px + py * py) * fitm; // CHECK :-)GOOD!

  lastpos.SetXYZ(points[lasthitid][2], points[lasthitid][3], z);
  lastmom.SetXYZ(px, py, pz);

  //   cout <<  "PREFIT " << endl;
  //   lastpos.Print();
  //   lastmom.Print();

  return kTRUE;
}

Bool_t PndSttMvdGemTracking::IntersectionFinder(Double_t xc, Double_t yc, Double_t radius, PndSttHit *stthit, TVector3 &xyz, TVector3 &dxyz)
{

  // tubeID  CHECK added
  Int_t tubeID = stthit->GetTubeID();
  PndSttTube *tube = (PndSttTube *)fTubeArray->At(tubeID);
  TVector3 wiredirection = tube->GetWireDirection();

  if (wiredirection != TVector3(0., 0., 1.)) {
    // cout << "wire skewed" << endl;
    return false;
  }

  // [xp, yp] point = coordinates xy of the centre of the firing tube
  TVector2 point;
  point.Set(tube->GetPosition().X(), tube->GetPosition().Y());
  Double_t isochrone = stthit->GetIsochrone();

  // the coordinates of the point are taken from the intersection
  // between the circumference from the drift time and the R radius of
  // curvature. -------------------------------------------------------
  // 2. find the intersection between the little circle and the line // R
  // 2.a
  // find the line passing throught [xc, yc] (centre of curvature) and [xp, yp] (first wire)
  // y = mx + q
  Double_t m = (point.Y() - yc) / (point.X() - xc);
  Double_t q = point.Y() - m * point.X();

  // cut on radius CHECK
  // if the simulated radius is too small, the stthit
  // is not used because rouning errors may occur
  // if(isochrone < 0.7e-3) { cout << "isochrone < 0.7e-3" << endl; return false; } // CHECK

  Double_t x1 = 0, y1 = 0, x2 = 0, y2 = 0, xb1 = 0, yb1 = 0, xb2 = 0, yb2 = 0;

  // CHECK the vertical track
  if (fabs(point.X() - xc) < 1e-6) {

    // 2.b
    // intersection little circle and line --> [x1, y1]
    // + and - refer to the 2 possible intersections
    // +
    x1 = point.X();
    y1 = point.Y() + sqrt(isochrone * isochrone - (x1 - point.X()) * (x1 - point.X()));
    // -
    x2 = x1;
    y2 = point.Y() - sqrt(isochrone * isochrone - (x2 - point.X()) * (x2 - point.X()));

    // 2.c intersection between line and circle
    // +
    xb1 = xc;
    yb1 = yc + sqrt(radius * radius - (xb1 - xc) * (xb1 - xc));
    // -
    xb2 = xb1;
    yb2 = yc - sqrt(radius * radius - (xb2 - xc) * (xb2 - xc));

  } // END CHECK
  else {

    // 2.b
    // intersection little circle and line --> [x1, y1]
    // + and - refer to the 2 possible intersections
    // +

    if (((m * (q - point.Y()) - point.X()) * (m * (q - point.Y()) - point.X()) -
         (m * m + 1) * ((q - point.Y()) * (q - point.Y()) + point.X() * point.X() - isochrone * isochrone)) < 0.) {
      if (fVerbose > 0)
        cout << "IntersectionFinder round errors: " << isochrone << endl;
      return false;
    }

    x1 = (-(m * (q - point.Y()) - point.X()) + sqrt((m * (q - point.Y()) - point.X()) * (m * (q - point.Y()) - point.X()) -
                                                    (m * m + 1) * ((q - point.Y()) * (q - point.Y()) + point.X() * point.X() - isochrone * isochrone))) /
         (m * m + 1);
    y1 = m * x1 + q;
    // -
    x2 = (-(m * (q - point.Y()) - point.X()) - sqrt((m * (q - point.Y()) - point.X()) * (m * (q - point.Y()) - point.X()) -
                                                    (m * m + 1) * ((q - point.Y()) * (q - point.Y()) + point.X() * point.X() - isochrone * isochrone))) /
         (m * m + 1);
    y2 = m * x2 + q;

    // 2.c intersection between line and circle
    // +
    xb1 = (-(m * (q - yc) - xc) + sqrt((m * (q - yc) - xc) * (m * (q - yc) - xc) - (m * m + 1) * ((q - yc) * (q - yc) + xc * xc - (radius) * (radius)))) / (m * m + 1);
    yb1 = m * xb1 + q;
    // -
    xb2 = (-(m * (q - yc) - xc) - sqrt((m * (q - yc) - xc) * (m * (q - yc) - xc) - (m * m + 1) * ((q - yc) * (q - yc) + xc * xc - (radius) * (radius)))) / (m * m + 1);
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
  if (dist1 > dist2)
    xyz.SetXYZ(x2, y2, stthit->GetZ());
  else
    xyz.SetXYZ(x1, y1, stthit->GetZ()); // <========= THIS IS THE NEW POINT to be used for the fit

  Double_t sigr = stthit->GetIsochroneError();
  Double_t sigx = sigr; // fabs(sigr * TMath::Cos(m));
  Double_t sigy = sigr; // fabs(sigr * TMath::Sin(m));
  dxyz.SetXYZ(sigx, sigy, 0);

  return kTRUE;
}

Bool_t PndSttMvdGemTracking::Fit(TMatrixT<double> points, Double_t &outxc, Double_t &outyc, Double_t &outradius)
{

  Int_t nhits = points.GetRowUpb() + 1;
  int lasthitid = -1, firsthitid = -1;
  for (int ihit = 0; ihit < nhits; ihit++) {
    if (points[ihit][0] != -1 && points[ihit][10] != -1 && points[ihit][10] != 1) {
      if (firsthitid == -1)
        firsthitid = ihit;
      lasthitid = ihit;
    }
  }

  if (firsthitid == -1 || lasthitid == -1)
    return kFALSE; // CHECK

  Double_t trasl[2] = {points[firsthitid][2], points[firsthitid][3]};

  //  cout << "first/last " << firsthitid << " " << lasthitid << endl;
  if (firsthitid >= lasthitid)
    return false;
  Double_t alpha = TMath::ATan2(points[lasthitid][3] - points[firsthitid][3], points[lasthitid][2] - points[firsthitid][2]);
  Double_t Suu, Su, Sv, Suv, S1, Suuu, Suuv, Suuuu;

  Su = 0.;
  Sv = 0.;
  Suu = 0.;
  Suv = 0.;
  Suuu = 0.;
  S1 = 0.;
  Suuv = 0.;
  Suuuu = 0.;
  Double_t s = 0.001; // CHECK
  // ..............................................

  TVector3 fitpoint;
  for (int ihit = 0; ihit < nhits; ihit++) {
    Int_t hitId = (Int_t)points[ihit][0];
    Int_t detId = (Int_t)points[ihit][1];
    if (hitId == -1)
      continue;
    Int_t fitflag = (Int_t)points[ihit][10];
    if (fitflag == 1 || fitflag == -1)
      continue;
    if (detId == FairRootManager::Instance()->GetBranchId(fSttBranchName) && points[ihit][8] < 0.1)
      continue;
    fitpoint.SetXYZ(points[ihit][2], points[ihit][3], points[ihit][4]);
    Double_t sigx = points[ihit][5];
    Double_t sigy = points[ihit][6];
    //       cout << "fitpoint" << endl;
    //       fitpoint.Print();
    // to the fit ================================
    Double_t xtrasl, ytrasl;
    // traslation
    xtrasl = fitpoint.X() - trasl[0];
    ytrasl = fitpoint.Y() - trasl[1];

    Double_t xrot, yrot;
    // rotation
    xrot = TMath::Cos(alpha) * xtrasl + TMath::Sin(alpha) * ytrasl;
    yrot = -TMath::Sin(alpha) * xtrasl + TMath::Cos(alpha) * ytrasl;

    // re-traslation
    xtrasl = xrot + s;
    ytrasl = yrot;

    // change coordinate
    Double_t u, v, sigv2; //, sigu2; //[R.K.02/2017] Unused variable?
    u = xtrasl / (xtrasl * xtrasl + ytrasl * ytrasl);
    v = ytrasl / (xtrasl * xtrasl + ytrasl * ytrasl);

    Double_t dvdx = (-2 * xtrasl * ytrasl) / pow((xtrasl * xtrasl + ytrasl * ytrasl), 2);
    Double_t dvdy = (xtrasl * xtrasl - ytrasl * ytrasl) / pow((xtrasl * xtrasl + ytrasl * ytrasl), 2);
    // Double_t dudx = (ytrasl*ytrasl - xtrasl*xtrasl) / pow((xtrasl*xtrasl + ytrasl*ytrasl),2); //[R.K.02/2017] Unused variable?
    // Double_t dudy = (-2 * xtrasl * ytrasl)/pow((xtrasl*xtrasl + ytrasl*ytrasl),2); //[R.K.02/2017] Unused variable?

    // sigu2 = dudx * dudx * sigx * sigx + dudy * dudy * sigy * sigy + 2 * dudx * dudy * sigx * sigy;  //[R.K.02/2017] Unused variable?
    sigv2 = dvdx * dvdx * sigx * sigx + dvdy * dvdy * sigy * sigy + 2 * dvdx * dvdy * sigx * sigy;

    if (sigv2 == 0)
      sigv2 = 1e-5; // CHECK MVD covariance

    Su = Su + (u / sigv2);
    Sv = Sv + (v / sigv2);

    Suv = Suv + ((u * v) / sigv2);
    Suu = Suu + ((u * u) / sigv2);

    Suuu = Suuu + ((u * u * u) / sigv2);
    Suuv = Suuv + ((u * u * v) / sigv2);

    Suuuu = Suuuu + ((u * u * u * u) / sigv2);

    S1 = S1 + 1 / sigv2;
  }

  TMatrixT<double> matrix(3, 3);
  matrix[0][0] = S1;
  matrix[0][1] = Su;
  matrix[0][2] = Suu;

  matrix[1][0] = Su;
  matrix[1][1] = Suu;
  matrix[1][2] = Suuu;

  matrix[2][0] = Suu;
  matrix[2][1] = Suuu;
  matrix[2][2] = Suuuu;

  Double_t determ;

  determ = matrix.Determinant();

  if (determ != 0) {
    matrix.Invert();
  } else {
    //    cout << "DET 0" << endl; // CHECK what to do
    return false;
  }

  TMatrixT<double> column(3, 1);
  column[0][0] = Sv;
  column[1][0] = Suv;
  column[2][0] = Suuv;

  TMatrixT<double> column2(3, 1);
  column2.Mult(matrix, column);

  Double_t a, b, c;
  a = column2[0][0];
  b = column2[1][0];
  c = column2[2][0];

  if (fabs(a) < 0.000001) {
    // cout << "A < 1e-**" << endl;
    return kFALSE;
  }

  // center and radius
  Double_t xcrot, ycrot, xc, yc, epsilon, R;
  ycrot = 1 / (2 * a);
  xcrot = -b / (2 * a);
  epsilon = -c * pow((1 + (b * b)), -3 / 2);
  R = epsilon + sqrt((xcrot * xcrot) + (ycrot * ycrot));

  // re-rotation and re-traslation of xc and yc
  // translation
  xcrot = xcrot - s;
  // rotation
  xc = TMath::Cos(alpha) * xcrot - TMath::Sin(alpha) * ycrot;
  yc = TMath::Sin(alpha) * xcrot + TMath::Cos(alpha) * ycrot;
  // traslation
  xc = xc + trasl[0];
  yc = yc + trasl[1];
  // Double_t phi = TMath::ATan2(yc, xc);  //[R.K.02/2017] Unused variable?
  // Double_t d; //[R.K.02/2017] Unused variable?
  // d = ((xc + yc) - R*(TMath::Cos(phi) + TMath::Sin(phi)))/(TMath::Cos(phi) + TMath::Sin(phi));  //[R.K.02/2017] Unused variable?

  //  cout << "REFITTED FIT: " << xc << " " << yc << endl;
  //  cout << "RAGGIO: " << R << endl;

  outxc = xc;
  outyc = yc;
  outradius = R;
  return true;
}

// CHECK :-)GOOD!
Bool_t PndSttMvdGemTracking::ZFit(TMatrixT<double> points, Int_t charge, Double_t xc, Double_t yc, Double_t radius, Double_t &fitm, Double_t &fitp)
{

  // recalculate z - s fit only from MVD
  Double_t Sxx, Sx, Sz, Sxz, S1z;
  Double_t Detz = 0.;

  Sx = 0.;
  Sz = 0.;
  Sxx = 0.;
  Sxz = 0.;
  S1z = 0.;

  // x0 y0
  Double_t d = TMath::Sqrt(xc * xc + yc * yc) - radius;
  Double_t phi = TMath::ATan2(yc, xc);

  Double_t x0 = d * TMath::Cos(phi);
  Double_t y0 = d * TMath::Sin(phi);

  Double_t Phi0 = TMath::ATan2((y0 - yc), (x0 - xc));
  Double_t scos = 0;

  Int_t nhits = points.GetRowUpb() + 1;

  Double_t Fi_pre = 0.;
  int zcounter = 0;
  for (int ihit = 0; ihit < nhits; ihit++) {
    Int_t detId = (Int_t)points[ihit][1];
    Int_t hitId = (Int_t)points[ihit][0];
    //   cout << "hitId " << hitId << " detId " << detId << endl;
    if (hitId == -1)
      continue;
    Int_t fitflag = (Int_t)points[ihit][10];
    //  if(fitflag == 0 || fitflag == -1) continue;  // CHECK // to use zfinder
    if (fitflag != 2)
      continue; // CHECK // to use zfinder

    if (detId == FairRootManager::Instance()->GetBranchId(fSttBranchName) || detId == FairRootManager::Instance()->GetBranchId(fGemBranchName))
      continue;

    TVector2 v(x0 - xc, y0 - yc);
    Double_t alpha = TMath::ATan2(points[ihit][3] - y0 + radius * TMath::Sin(Phi0), points[ihit][2] - x0 + radius * TMath::Cos(Phi0));
    TVector2 p(points[ihit][2] - xc, points[ihit][3] - yc);
    Double_t Fi = CalculatePhi(v, p, alpha, Phi0, charge);
    if (ihit > 0)
      Fi = CompareToPreviousPhi(Fi, Fi_pre, charge);
    Fi_pre = Fi;
    scos = -charge * radius * Fi; // scos = -q * R * phi CHECK :-)GOOD!
    //    cout << charge << " zfit " << scos << endl;

    Double_t sigz2 = points[ihit][7] * points[ihit][7]; // CHECK

    if (sigz2 == 0)
      sigz2 = 1e-5; // CHECK MVD covariance
    //      cout << "scosl " << scos << " " << points[ihit][4] << " " << sigz2 <<  " " <<  points[ihit][7] << endl;
    zcounter++;
    Sx = Sx + (scos / (sigz2));
    Sz = Sz + (points[ihit][4] / (sigz2));
    Sxz = Sxz + ((scos * points[ihit][4]) / (sigz2));
    Sxx = Sxx + ((scos * scos) / (sigz2));
    S1z = S1z + 1 / (sigz2);
  }

  if (zcounter <= 1)
    return kFALSE; //  CHECK
  Detz = S1z * Sxx - Sx * Sx;
  if (Detz == 0) {
    cout << "DET Z = 0" << endl;
    return kFALSE;
  } // CHECK
  fitp = (1 / Detz) * (Sxx * Sz - Sx * Sxz);
  fitm = (1 / Detz) * (S1z * Sxz - Sx * Sz);
  //  cout << "z fit " << fitm << " " << fitp << endl;

  return true;
}

// CHECK :-)GOOD!
Bool_t PndSttMvdGemTracking::GetInitialParams(PndTrack *sttmvd, Double_t &xc, Double_t &yc, Double_t &radius, Double_t &fitm, Double_t &fitp)
{
  if (fVerbose > 0)
    cout << "GET INITIAL PARAMS" << endl;
  FairTrackParP recopar = sttmvd->GetParamFirst();
  TVector3 recomom = recopar.GetMomentum();
  TVector3 recopos = recopar.GetPosition();
  Int_t charge = recopar.GetQ();

  radius = recomom.Perp() / 0.006;
  Double_t beta;

  if (fabs(recomom.X()) > 1e-10) {
    // track from tangent ---------------------
    // double reco_m1 = recomom.Y() / recomom.X(); //[R.K. 01/2017] unused variable?
    // double reco_q1 = recopos.Y() - recopos.X() * reco_m1; //[R.K. 01/2017] unused variable?
    // double reco_m2 = -1./reco_m1; //[R.K. 01/2017] unused variable?
    // double reco_q2 = recopos.Y() - recopos.X() * reco_m2; //[R.K. 01/2017] unused variable?
    beta = TMath::ATan2(recomom.X(), recomom.Y());
  } else
    beta = TMath::Sign(1., recomom.Y()) * TMath::Pi();
  // double recoX0, recoY0; //[R.K. 01/2017] unused variable?
  if (charge > 0) {
    xc = recopos.X() + radius * TMath::Cos(beta);
    yc = recopos.Y() - radius * TMath::Sin(beta);
  } else {
    xc = recopos.X() - radius * TMath::Cos(beta);
    yc = recopos.Y() + radius * TMath::Sin(beta);
  }

  // vector calculation (alternative): tested, it works!
  //   TVector2 direction(recomom.X(), recomom.Y());
  //   direction = direction.Unit();
  //   TVector2 rad(charge * direction.Y() * R, - charge * direction.X() * R);

  //   TVector2 center = recopos.XYvector() + rad;
  //   xc = center.X();
  //   yc = center.Y();

  // ---------------------------------------------------
  FairTrackParP recoparlast = sttmvd->GetParamLast();
  TVector3 recoposlast = recoparlast.GetPosition();

  if (fVerbose > 0) {
    cout << "charge/xc/yc/radius " << charge << " " << xc << " " << yc << " " << radius << endl;
    recomom.Print();
    recopos.Print();
    recoparlast.GetMomentum().Print();
    recoposlast.Print();
  }

  fitm = recomom.Z() / recomom.Perp(); // CHECK fitm = pz / pt :-)GOOD!

  // x0 y0
  Double_t d = TMath::Sqrt(xc * xc + yc * yc) - radius;
  Double_t phi = TMath::ATan2(yc, xc);

  Double_t x0 = d * TMath::Cos(phi);
  Double_t y0 = d * TMath::Sin(phi);

  Double_t Phi0 = TMath::ATan2((y0 - yc), (x0 - xc));
  Double_t scosfirst = 0, scoslast = 0.;

  if (fVerbose > 0)
    cout << "Phi0 " << Phi0 * TMath::RadToDeg() << endl;
  // CHECK :-)GOOD! ...
  TVector2 v(x0 - xc, y0 - yc);
  double alpha1 = TMath::ATan2(recopos.Y() - y0 + radius * TMath::Sin(Phi0), recopos.X() - x0 + radius * TMath::Cos(Phi0));
  TVector2 p1(recopos.X() - xc, recopos.Y() - yc);
  Double_t Fi1 = CalculatePhi(v, p1, alpha1, Phi0, charge);

  if (fVerbose > 0) {
    cout << "alpha1, Fi1 " << alpha1 * TMath::RadToDeg() << " " << Fi1 * TMath::RadToDeg() << endl;
    p1.Print();
  }

  double alpha2 = TMath::ATan2(recoposlast.Y() - y0 + radius * TMath::Sin(Phi0), recoposlast.X() - x0 + radius * TMath::Cos(Phi0));
  TVector2 p2(recoposlast.X() - xc, recoposlast.Y() - yc);
  Double_t Fi2 = CalculatePhi(v, p2, alpha2, Phi0, charge);
  Fi2 = CompareToPreviousPhi(Fi2, Fi1, charge); // CHECK this!

  if (fVerbose > 0) {
    cout << "alpha2, Fi2 " << alpha2 * TMath::RadToDeg() << " " << Fi2 * TMath::RadToDeg() << endl;
    p2.Print();
  }

  scosfirst = -charge * radius * Fi1; // scos = -q * R * phi CHECK :-)GOOD!
  scoslast = -charge * radius * Fi2;  //                     CHECK :-)GOOD!
  // ............. :-)GOOD!

  // z = z0 + scos * fitm
  fitp = (recopos.Z() + recoposlast.Z() - fitm * (scosfirst + scoslast)) / 2.; // CHECK :-)GOOD!

  if (fVerbose > 0) {
    cout << "positions first/last" << endl;
    recopos.Print();
    recoposlast.Print();

    cout << "scosfirst/scoslast " << scosfirst << " " << scoslast << endl;
    cout << "fitm/fitp " << fitm << " " << fitp << endl;
    cout << "z1/z2 " << fitp + fitm * scosfirst << " " << fitp + fitm * scoslast << endl;
  }
  return true;
}

// CHECK :-)GOOD! this function has been tested and is ok!
Double_t PndSttMvdGemTracking::CalculatePhi(TVector2 v, TVector2 p, double alpha, double Phi0, int charge)
{
  Double_t Fi = -charge * TMath::ACos(v * p / (v.Mod() * p.Mod()));
  double pi = TMath::Pi();
  double pi2 = 2 * pi;

  // Fi = h * (pi2 - h * Fi) // should be correct
  if ((charge > 0 &&
       ((Phi0 > 0 && ((alpha > 0 && alpha > Phi0) || (alpha < 0 && alpha < Phi0 - pi))) || ((Phi0 < 0 && ((alpha > 0 && alpha < pi + Phi0) || (alpha < 0 && alpha > Phi0)))))))
    Fi = -(pi2 + Fi);
  else if ((charge < 0 &&
            ((Phi0 > 0 && ((alpha > 0 && alpha < Phi0) || (alpha < 0 && alpha > Phi0 - pi))) || ((Phi0 < 0 && ((alpha > 0 && alpha > pi + Phi0) || (alpha < 0 && alpha < Phi0)))))))
    Fi = pi2 - Fi;

  return Fi;
}

Double_t PndSttMvdGemTracking::CompareToPreviousPhi(Double_t Fi, Double_t Fi_pre, int charge)
{
  // if(fabs(Fi) < fabs(Fi_pre)) Fi += h * pi2 // CHECK should be ok
  double pi = TMath::Pi();
  double pi2 = 2 * pi;

  if (charge < 0 && Fi < Fi_pre)
    Fi += pi2;
  else if (charge > 0 && Fi > Fi_pre)
    Fi -= pi2;
  Fi_pre = Fi;
  return Fi;
}

void PndSttMvdGemTracking::ConsiderCombinatorialEffect(Int_t nhits)
{

  if (nhits == 0)
    return;

  // matrix hitid x y posindex = istat * 10 + isens iflag
  TMatrixT<double> sensor(nhits, 5);
  TMatrixT<double> nhitsonsensor(fNPositions, 1);
  TMatrixT<double> nhitsonsensor2(fNPositions, nhits);

  //   cout << "gem n hits " << nhits << endl;
  //  std::vector<int> mcpoints[fNPositions]; // CHECK
  std::vector<std::vector<int>> mcpoints; // CHECK

  for (Int_t ihit = 0; ihit < nhits; ihit++) {
    PndGemHit *hit = (PndGemHit *)fGemHitArray->At(ihit);
    if (!hit)
      continue;
    //  cout << "ihit " << ihit << endl;
    sensor[ihit][0] = ihit;
    sensor[ihit][1] = hit->GetX();
    sensor[ihit][2] = hit->GetY();
    // int istat = hit->GetStationNr(); //[R.K. 01/2017] unused variable?
    // int isens = hit->GetSensorNr(); //[R.K. 01/2017] unused variable?
    int posindex = GetPosIndex(hit);
    fOrderingIterator = find(fOrdering.begin(), fOrdering.end(), posindex);
    int ipos = fOrderingIterator - fOrdering.begin();
    sensor[ihit][3] = ipos;
    sensor[ihit][4] = true;
    /**
       sensor[ihit][3] = posindex;
       switch(posindex) {
       case 11:
       sensor[ihit][3] = 0; break;
       case 12:
       sensor[ihit][3] = 1; break;
       case 21:
       sensor[ihit][3] = 2; break;
       case 22:
       sensor[ihit][3] = 3; break;
       case 31:
       sensor[ihit][3] = 4; break;
       case 32:
       sensor[ihit][3] = 5; break;
       }
    **/

    nhitsonsensor2[ipos][(int)nhitsonsensor[ipos][0]] = ihit;
    nhitsonsensor[ipos][0]++;
  }

  //  std::vector<int> accepted[fNPositions];
  std::vector<std::vector<int>> accepted;
  for (int ipos = 0; ipos < fNPositions; ipos++) {
    std::vector<int> acc;
    accepted.push_back(acc);
  }

  //  nhitsonsensor2.Print();

  //   cout << "SENS I, STAT I   " << nhitsonsensor[0][0] << endl;
  //   cout << "SENS 2, STAT I   " << nhitsonsensor[1][0] << endl;
  //   cout << "SENS I, STAT II  " << nhitsonsensor[2][0] << endl;
  //   cout << "SENS 2, STAT II  " << nhitsonsensor[3][0] << endl;
  //   cout << "SENS I, STAT III " << nhitsonsensor[4][0] << endl;
  //   cout << "SENS 2, STAT III " << nhitsonsensor[5][0] << endl;

  int counter1 = 0;
  //   cout << "nhits " << nhits << endl;
  for (int ihit = 0; ihit < nhits; ihit++) {

    if (sensor[ihit][3] != 0 && sensor[ihit][3] != 2 && sensor[ihit][3] != 4)
      continue;
    int first = (int)sensor[ihit][3];
    if (sensor[ihit][4] == false)
      continue;
    // cout << "FIRST " << first << endl;
    double x1 = sensor[ihit][1];
    double y1 = sensor[ihit][2];
    counter1++;

    int counter2 = 0;
    for (int jhit = 0; jhit < nhits; jhit++) {

      if (sensor[jhit][3] != first + 1)
        continue;
      int second = (int)sensor[jhit][3];
      if (sensor[jhit][4] == false)
        continue;
      // cout << "SECOND " << second << endl;

      double x2 = sensor[jhit][1];
      double y2 = sensor[jhit][2];
      counter2++;
      double distance = TMath::Sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));

      if (distance < fCombiDistance) {
        std::vector<int> acc1 = accepted[first];
        std::vector<int> acc2 = accepted[second];
        bool alreadythere1 = false, alreadythere2 = false;
        for (size_t j = 0; j < acc1.size(); j++) {
          if (sensor[ihit][0] == acc1[j]) {
            alreadythere1 = true;
            break;
          }
        }
        for (size_t j = 0; j < acc2.size(); j++) {
          if (sensor[jhit][0] == acc2[j]) {
            alreadythere2 = true;
            break;
          }
        }

        if (alreadythere1 == false) {
          // accepted[first].push_back((int) sensor[ihit][0]);
          acc1.push_back((int)sensor[ihit][0]);
          accepted[first] = acc1;
          fCombiMap[ihit] = 0;

          // ^^^^^^^
          PndGemHit *hit = (PndGemHit *)fGemHitArray->At(ihit);
          if (!hit)
            continue;
          double digis[2];
          digis[0] = hit->GetDigiNr(0);
          digis[1] = hit->GetDigiNr(1);
          int stat = hit->GetStationNr();
          int sens = hit->GetSensorNr();
          int posindex = stat * 10 + sens;
          switch (posindex) {
          case 11: posindex = 0; break;
          case 12: posindex = 1; break;
          case 21: posindex = 2; break;
          case 22: posindex = 3; break;
          case 31: posindex = 4; break;
          case 32: posindex = 5; break;
          }

          // cout << "SETTING " << ihit << " to false with ";
          for (int khit = 0; khit < nhits; khit++) {

            if (ihit == khit)
              continue;
            if (sensor[khit][3] != posindex)
              continue;
            //		if(posindex == 1) cout << "? " << khit << endl;
            PndGemHit *ghit = (PndGemHit *)fGemHitArray->At(khit);
            if (!ghit)
              continue;

            if (ghit->GetDigiNr(0) == digis[0] || ghit->GetDigiNr(0) == digis[1]) {
              // cout << khit << " ";
              sensor[khit][4] = false;
            }
            if (ghit->GetDigiNr(1) == digis[0] || ghit->GetDigiNr(1) == digis[1]) {
              //   cout << khit << " ";
              sensor[khit][4] = false;
            }
          }
          sensor[ihit][4] = false;

          //   cout << endl;
          // ^^^^^^^

          if (fDisplayOn == kTRUE) {
            TMarker *amrk = new TMarker(x1, y1, 21);
            amrk->SetMarkerColor(5);
            display->cd(first + 1);
            amrk->Draw("SAME");
            display->Update();
            display->Modified();
          }
        }

        if (alreadythere2 == false) {
          // 	  accepted[second].push_back((int) sensor[jhit][0]);
          acc2.push_back((int)sensor[jhit][0]);
          accepted[second] = acc2;

          fCombiMap[jhit] = 0;

          // ^^^^^^^
          PndGemHit *hit = (PndGemHit *)fGemHitArray->At(jhit);
          if (!hit)
            continue;
          double digis[2];
          digis[0] = hit->GetDigiNr(0);
          digis[1] = hit->GetDigiNr(1);
          int stat = hit->GetStationNr();
          int sens = hit->GetSensorNr();
          int posindex = stat * 10 + sens;
          switch (posindex) {
          case 11: posindex = 0; break;
          case 12: posindex = 1; break;
          case 21: posindex = 2; break;
          case 22: posindex = 3; break;
          case 31: posindex = 4; break;
          case 32: posindex = 5; break;
          }

          //	      cout << "SETTING " << jhit << " to false with ";
          for (int khit = 0; khit < nhits; khit++) {
            if (jhit == khit)
              continue;
            if (sensor[khit][3] != posindex)
              continue;

            PndGemHit *ghit = (PndGemHit *)fGemHitArray->At(khit);
            if (!ghit)
              continue;
            if (ghit->GetDigiNr(0) == digis[0] || ghit->GetDigiNr(0) == digis[1]) {
              // cout << khit << " ";
              sensor[khit][4] = false;
            }
            if (ghit->GetDigiNr(1) == digis[0] || ghit->GetDigiNr(1) == digis[1]) {
              //	  cout << khit << " ";
              sensor[khit][4] = false;
            }
          }
          sensor[jhit][4] = false;
          //   cout << endl;
          // ^^^^^^^

          if (fDisplayOn == kTRUE) {
            TMarker *amrk2 = new TMarker(x2, y2, 21);
            amrk2->SetMarkerColor(5);
            display->cd(second + 1);
            amrk2->Draw("SAME");
            display->Update();
            display->Modified();
          }
        }
      }
      if (counter2 == nhitsonsensor[first + 1][0])
        break;
    }
    if (counter1 == (nhitsonsensor[0][0] + nhitsonsensor[2][0] + nhitsonsensor[4][0]))
      break;
  }

  //  cout << "MCPOINTS: " << endl;
  //   for(int istat = 0; istat < fNPositions; istat++) {
  //     cout << "istat " << istat << " " ;
  //     for(int j = 0; j < mcpoints[istat].size(); j++) cout << " " << mcpoints[istat][j];
  //     cout << endl;
  //   }
  //   cout << "ACCEPTED " << endl;
  //   for(int istat = 0; istat < fNPositions; istat++) {
  //     cout << "istat " << istat << " ";
  //     for(int i = 0; i < accepted[istat].size(); i++)  cout << " " << accepted[istat][i];
  //     cout << endl;
  //   }
}

void PndSttMvdGemTracking::CheckCombinatorial(Int_t nhits, Int_t ntracks)
{

  if (nhits == 0)
    return;

  if (fVerbose > 0)
    cout << "CHECK COMBINATORIAL: DELETE FAKE HITS" << endl;

  // loop over the tracks
  for (Int_t it = 0; it < ntracks; it++) {
    int itrk = GetTrackIndex(it);
    if (fProTracks[itrk] == false)
      continue;
    std::vector<int> thistrackhits = GetHitsAssociatedToTrack(itrk);
    if (thistrackhits.size() == 0)
      continue;
    // fill table:
    // each row is a sensor plane and contains
    // the hits associtaed to this track on it
    TMatrixT<double> combi(fNPositions, thistrackhits.size());
    TMatrixT<double> addhit(fNPositions, 1);

    // init
    for (Int_t ipos = 0; ipos < fNPositions; ipos++) {
      addhit[ipos][0] = 0;
      for (size_t j = 0; j < thistrackhits.size(); j++)
        combi[ipos][j] = -1;
    }

    for (size_t j = 0; j < thistrackhits.size(); j++) {
      int ihit = thistrackhits[j];

      PndGemHit *gemhit = (PndGemHit *)fGemHitArray->At(ihit);
      if (!gemhit)
        continue;

      int posindex = GetPosIndex(gemhit);
      fOrderingIterator = find(fOrdering.begin(), fOrdering.end(), posindex);
      int index = fOrderingIterator - fOrdering.begin();

      int hitpos = (int)addhit[index][0];
      combi[index][hitpos] = ihit;
      addhit[index][0]++;
    }

    if (fVerbose > 0) {
      cout << "itrk " << itrk << " " << nhits << " " << thistrackhits.size() << endl;
      combi.Print();
    }

    // loop on sensor planes for this track
    for (Int_t ipos = 0; ipos < fNPositions; ipos++) {

      int count = 0;
      // count hits associated on this sensor plane
      for (size_t i = 0; i < thistrackhits.size(); i++) {
        if (combi[ipos][i] != -1)
          count++;
      }

      if (fVerbose > 0)
        cout << count << " hits on pos " << ipos << endl;

      // if there is no hit or only one, continue ...
      if (count <= 1)
        continue;
      // ... else
      int count2 = 0;
      // count how many true (non combi) hits are there
      for (size_t i = 0; i < thistrackhits.size(); i++) {
        int ihit = (int)combi[ipos][i];
        if (ihit == -1)
          continue;
        if (fCombiMap[ihit] == 0)
          count2++;
      }

      if (fVerbose > 0)
        cout << count2 << " true hits on pos " << ipos << endl;

      // if there is no true hit, continue ...
      if (count2 == 0)
        continue;
      // ... else, clean up from combinatorial hits
      for (size_t i = 0; i < thistrackhits.size(); i++) {
        int ihit = (int)combi[ipos][i];
        if (fCombiMap[ihit] != 0) {
          if (fVerbose > 0)
            cout << "delete " << ihit << " from track " << itrk << endl;
          DeleteHitFromTrack(ihit, itrk);
        }
      }
    }
  }
}

Bool_t PndSttMvdGemTracking::ZFind(Int_t nhits, TMatrixT<double> points, Double_t xc, Double_t yc, Double_t radius)
{
  //  cout << "Z FINDER" << endl;
  if (nhits == 0)
    return kFALSE;

  for (int ihit = 0; ihit < nhits; ihit++) {
    Int_t detId = (Int_t)points[ihit][1];
    Int_t hitId = (Int_t)points[ihit][0];
    //      cout << "hitId " << hitId << " detId " << detId << endl;
    if (hitId == -1)
      continue;
    Int_t fitflag = (Int_t)points[ihit][10];
    if (fitflag != 1)
      continue;
    if (detId != FairRootManager::Instance()->GetBranchId(fSttBranchName))
      continue;

    // intersection: tube line with circle trajectory in xy

    // find the tube line
    // y = mx + q
    PndSttHit *hit = (PndSttHit *)fSttHitArray->At(hitId);
    if (!hit)
      continue;

    Int_t tubeID = hit->GetTubeID();

    PndSttTube *tube = (PndSttTube *)fTubeArray->At(tubeID);
    TVector3 pos = tube->GetPosition();
    TVector3 wireDirection = tube->GetWireDirection();
    Double_t halflength = tube->GetHalfLength();
    if (wireDirection == TVector3(0., 0., 1.))
      continue;

    //       pos.Print();
    //       wireDirection.Print();
    //      cout << "hl " << halflength << endl;
    TVector3 first = pos + wireDirection * halflength;  // CHECK
    TVector3 second = pos - wireDirection * halflength; // CHECK
                                                        //     first.Print();
                                                        //       second.Print();

    double xint, yint, x1, y1, x2, y2, delta;

    // when tube is vertical
    if (fabs(second.X() - first.X()) < 1.e-5) {
      x1 = first.X();
      x2 = x1;

      delta = radius * radius - (x1 - xc) * (x1 - xc);
      if (delta < 0)
        continue;
      y1 = yc + TMath::Sqrt(delta);
      y2 = yc - TMath::Sqrt(delta);

    } else {

      Double_t m = (second.Y() - first.Y()) / (second.X() - first.X());
      Double_t q = first.Y() - m * first.X();

      // center of trajectory xc, yc, radius
      delta = (m * (q - yc) - xc) - (m * m + 1) * ((q - yc) * (q - yc) + xc * xc - radius * radius);
      if (delta < 0)
        continue;

      x1 = (-(m * (q - yc) - xc) + delta) / (m * m + 1);
      y1 = m * x1 + q;
      x2 = (-(m * (q - yc) - xc) - delta) / (m * m + 1);
      y2 = m * x2 + q;
    }

    double d1 = 0, d2 = 0;
    d1 = TMath::Sqrt((y1 - first.Y()) * (y1 - first.Y()) + (x1 - first.X()) * (x1 - first.X()));
    d2 = TMath::Sqrt((y2 - first.Y()) * (y2 - first.Y()) + (x2 - first.X()) * (x2 - first.X()));

    if (d1 < d2) {
      xint = x1;
      yint = y1;
    } else {
      xint = x2;
      yint = y2;
    }

    // APPROXIMATION: take the z of this intersection point // CHECK
    Double_t ll = ((xint - first.X()) + (yint - first.Y())) / (wireDirection.X() + wireDirection.Y());
    double zint = first.Z() + wireDirection.Z() * ll;

    points[ihit][2] = xint;
    points[ihit][3] = yint;
    points[ihit][4] = zint;
    points[ihit][5] = 1.; // CHECK
    points[ihit][6] = 1.; // CHECK
    points[ihit][7] = 1.; // CHECK

    //      cout << "inters " << xint << " " << yint << " " << zint << endl;
  }

  return kTRUE;
}

void PndSttMvdGemTracking::UpdateMCTrackId(PndTrackCand *completeCand)
{

  cout << "UPDATE MC TRACK ID" << endl;
  cout << "ORIGINAL MC " << completeCand->getMcTrackId() << endl;

  // track ID <---> counter
  std::map<int, int> mctrackids;
  //  std::vector<int> newtracks;

  for (size_t ihit = 0; ihit < completeCand->GetNHits(); ihit++) {
    PndTrackCandHit candhit = completeCand->GetSortedHit(ihit);
    Int_t iHit = candhit.GetHitId();
    Int_t detId = candhit.GetDetId();

    FairHit *fhit;
    FairMCPoint *fpnt;
    if (detId == FairRootManager::Instance()->GetBranchId(fMvdStripBranchName)) {
      fhit = (FairHit *)fMvdStripHitArray->At(iHit);
      if (!fhit)
        continue;
      Int_t refIndex = fhit->GetRefIndex();
      if (refIndex == -1)
        continue;
      fpnt = (FairMCPoint *)fMvdPointArray->At(refIndex);
      if (!fpnt)
        continue;
    } else if (detId == FairRootManager::Instance()->GetBranchId(fMvdPixelBranchName)) {
      fhit = (FairHit *)fMvdPixelHitArray->At(iHit);
      if (!fhit)
        continue;
      Int_t refIndex = fhit->GetRefIndex();
      if (refIndex == -1)
        continue;
      fpnt = (FairMCPoint *)fMvdPointArray->At(refIndex);
      if (!fpnt)
        continue;
    } else if (detId == FairRootManager::Instance()->GetBranchId(fSttBranchName)) {
      fhit = (FairHit *)fSttHitArray->At(iHit);
      if (!fhit)
        continue;
      Int_t refIndex = fhit->GetRefIndex();
      if (refIndex == -1)
        continue;
      fpnt = (FairMCPoint *)fSttPointArray->At(refIndex);
      if (!fpnt)
        continue;
    } else if (detId == FairRootManager::Instance()->GetBranchId(fGemBranchName)) {
      fhit = (FairHit *)fGemHitArray->At(iHit);
      if (!fhit)
        continue;
      Int_t refIndex = fhit->GetRefIndex();
      if (refIndex == -1)
        continue;
      fpnt = (FairMCPoint *)fGemPointArray->At(refIndex);
    } else
      continue; // CHECK

    int trackID = fpnt->GetTrackID();
    cout << "HIT ID " << iHit << " DET ID " << detId << " TRACK ID " << trackID << endl;
    //       // is the track ID new?
    //       std::vector<int>::iterator iter;
    //       iter = find(newtracks.begin(), newtracks.end(), trackID);
    //       int index = iter - newtracks.begin();
    //       // if no
    //       if(index == newtracks.size()) newtracks.push_back(trackID);

    mctrackids[trackID]++;
  }

  int counter = 0;
  int tmptrackID = -1;
  for (size_t itrk = 0; itrk < mctrackids.size(); itrk++) {
    if (counter < mctrackids[itrk]) {
      counter = mctrackids[itrk];
      tmptrackID = itrk;
    }
  }
  completeCand->setMcTrackId(tmptrackID);
  cout << "NEW MC " << completeCand->getMcTrackId() << endl;
}

Int_t PndSttMvdGemTracking::GetPdgFromMC(int trackid)
{

  PndTrackID *trackID = (PndTrackID *)fTrackIDArray->At(trackid);
  if (trackID->GetNCorrTrackId() > 0) {
    Int_t mctrackid = trackID->GetCorrTrackID();
    if (mctrackid == -1) {
      LOG(warn) << " PndSttMvdGemTracking: no mctrackid - use default hypo " << fDefaultPdgCode;
      return fDefaultPdgCode;
    }

    PndMCTrack *mctrack = (PndMCTrack *)fMCTrackArray->At(mctrackid);
    if (!mctrack) {
      LOG(warn) << " PndSttMvdGemTracking: no mctrack " << mctrackid << " - use default hypo " << fDefaultPdgCode;
      return fDefaultPdgCode;
    }

    Int_t pdg = mctrack->GetPdgCode();

    if (pdg >= 100000000) // ion
    {
      LOG(warn) << " PndSttMvdGemTracking: we have an ion here " << pdg << " - use default hypo " << fDefaultPdgCode;
      return fDefaultPdgCode;
    } else if (TDatabasePDG::Instance()->GetParticle(mctrack->GetPdgCode())->Charge() == 0) {
      LOG(warn) << " PndSttMvdGemTracking: we have an neutral here " << pdg << " - use default hypo " << fDefaultPdgCode;
      return fDefaultPdgCode;
    }
    return pdg;
  } // end of "at least one correlated mc index"
  else {
    LOG(warn) << " PndSttMvdGemTracking: no correlated mctrackid at all - use default hypo " << fDefaultPdgCode;
    return fDefaultPdgCode;
  }
}

Int_t PndSttMvdGemTracking::GetChargeCorrectedPdgFromMC(int trackid, int charge)
{
  int pdg = GetPdgFromMC(trackid);
  // is the reco track in accordance with the mc one?
  double mccharge = TDatabasePDG::Instance()->GetParticle(pdg)->Charge() / 3.;
  if ((charge * mccharge) > 0.)
    return pdg;
  else
    return -pdg;
}

void PndSttMvdGemTracking::SetBranchNames(TString mvdpixel, TString mvdstrip, TString stt, TString gem)
{
  fMvdPixelBranchName = mvdpixel;
  fMvdStripBranchName = mvdstrip;
  fSttBranchName = stt;
  fGemBranchName = gem;
}

void PndSttMvdGemTracking::SetTrackBranchNames(TString starttrack, TString starttrackcand)
{
  fStartTrackBranchName = starttrack;
  fStartTrackCandBranchName = starttrackcand;
}

ClassImp(PndSttMvdGemTracking)
