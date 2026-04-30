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
// -----                PndGemTrackFinderOnHitsTB source file             -----
// -----                Created 02.06.2009 by R. Karabowicz            -----
// -------------------------------------------------------------------------

#include "PndGemTrackFinderOnHitsTB.h"

// Pnd includes
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairBaseParSet.h"
#include "FairTrackParam.h"
#include "FairTrackParP.h"
#include "PndGemMCPoint.h"
#include "PndGemDigiPar.h"
#include "FairRootManager.h"
#include "PndDetectorList.h"
#include "PndTrack.h"
#include "PndTrackCand.h"
#include "PndTrackCandHit.h"
#include "FairLogger.h"
#include "FairMCPoint.h"
// ROOT includes
#include "TClonesArray.h"
#include "TGeoManager.h"
#include "TMatrixFSym.h"
#include "TStopwatch.h"

// C++ includes
#include <iostream>
#include <iomanip>
#include <map>
#include <cmath>
using std::cout;
using std::endl;
using std::map;

// -----   Default constructor   -------------------------------------------
PndGemTrackFinderOnHitsTB::PndGemTrackFinderOnHitsTB()
  : PndGemTrackFinder(), fDigiPar(nullptr), fMCTrackArray(nullptr), fMCPointArray(nullptr), fVerbose(0), fPrimary(0), fSigmaMult(60.), fParThetaA(0.), fParThetaB(0.),
    fParTheta0(0.), fParTheta1(0.), fParTheta2(0.), fParTheta3(0.), fParRadPhi0(0.), fParRadPhi2(0.), fPrepTime(0.), fSegmTime(0.), fMatchTime(0.), fRemoveTime(0.), fWriteTime(0.),
    fAllTime(0.), fNofEvents(0), fMCAvailable(-666), fNofClHits(0), fNofExpectedTrackSegments(0), fNofFoundTrackSegments(0)
{
  for (Int_t ipar = 0; ipar < 3; ipar++) {
    fParMat0[ipar] = 0.;
    fParMat1[ipar] = 0.;
  }
}

// -----   Destructor   ----------------------------------------------------
PndGemTrackFinderOnHitsTB::~PndGemTrackFinderOnHitsTB() {}

// -----   Init  -----------------------------------------------------------
void PndGemTrackFinderOnHitsTB::Init()
{

  fNofExpectedTrackSegments = 0;
  fNofFoundTrackSegments = 0;

  // Get and check FairRootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    cout << "-E- " << GetName() << "::Init: "
         << "RootManager not instantised!" << endl;
    return;
  }

  // Get the pointer to the singleton FairRunAna object
  FairRunAna *ana = FairRunAna::Instance();
  if (nullptr == ana) {
    cout << "-E- " << GetName() << "::Init :"
         << " no FairRunAna object!" << endl;
    return;
  }
  // Get the pointer to run-time data base
  FairRuntimeDb *rtdb = ana->GetRuntimeDb();
  if (nullptr == rtdb) {
    cout << "-E- " << GetName() << "::Init :"
         << " no runtime database!" << endl;
    return;
  }

  // Get MCTrack array
  fMCTrackArray = (TClonesArray *)ioman->GetObject("MCTrack");
  if (!fMCTrackArray) {
    LOG(info) << " " << GetName() << "::Init: No MCTrack array!";
    //    return;
  }

  // Get PndGemPoint (MCPoint) array
  fMCPointArray = (TClonesArray *)ioman->GetObject("GEMPoint");
  if (!fMCPointArray) {
    LOG(info) << " " << GetName() << "::Init: No MCPoint array!";
    //    return;
  }

  fDigiPar = (PndGemDigiPar *)(rtdb->getContainer("PndGemDetectors"));
  cout << "-I- "
       << "PndGemTrackFinderOnHitsTB"
       << "::Init(). There are " << fDigiPar->GetNStations() << " GEM stations." << endl;
  cout << "-I- "
       << "PndGemTrackFinderOnHitsTB"
       << "::Init(). Initialization succesfull." << endl;
  fParThetaA = fDigiPar->GetTrackFinderOnHits_ParThetaA();
  fParThetaB = fDigiPar->GetTrackFinderOnHits_ParThetaB();

  fParTheta0 = fDigiPar->GetTrackFinderOnHits_ParTheta0();
  fParTheta1 = fDigiPar->GetTrackFinderOnHits_ParTheta1();
  fParTheta2 = fDigiPar->GetTrackFinderOnHits_ParTheta2();
  fParTheta3 = fDigiPar->GetTrackFinderOnHits_ParTheta3();

  fParRadPhi0 = fDigiPar->GetTrackFinderOnHits_ParRadPhi0();
  fParRadPhi2 = fDigiPar->GetTrackFinderOnHits_ParRadPhi2();
  for (Int_t in = 0; in < 3; in++) {
    fParMat0[in] = fDigiPar->GetTrackFinderOnHits_ParMat0(in);
    fParMat1[in] = fDigiPar->GetTrackFinderOnHits_ParMat1(in);
  }

  LOG(info) << " " << GetName() << ": Intialization successfull";
}

// -----   Private method SetParContainers   -------------------------------
void PndGemTrackFinderOnHitsTB::SetParContainers()
{

  // Get run and runtime database
  FairRunAna *run = FairRunAna::Instance();
  if (!run)
    LOG(fatal) << "SetParContainers: No analysis run";

  FairRuntimeDb *db = run->GetRuntimeDb();
  if (!db)
    LOG(fatal) << "SetParContainers: No runtime database";

  // Get GEM digitisation parameter container
  fDigiPar = (PndGemDigiPar *)(db->getContainer("PndGemDetectors"));
}
// -------------------------------------------------------------------------

// -----   Public method DoFind   ------------------------------------------
Int_t PndGemTrackFinderOnHitsTB::DoFind(TClonesArray *hitArray, TClonesArray *trackArray, TClonesArray *trackCandArray)
{
  //  cout << "======== PndGemTrackFinderOnHitsTB::Exec(Event = " << fNofEvents << " ) ====================" << endl;

  fTimer.Start();

  // Get GEM digitisation parameter container
  fTrackSegments.clear();

  // Count events
  fNofEvents++;

  if (fVerbose) {
    cout << endl << endl << endl << endl;
    cout << "=======================================================" << endl;
    LOG(info) << "        Event No: " << fNofEvents;
    cout << "=======================================================" << endl;

    LOG(info) << " " << GetName() << "::DoFind ";
    cout << "-------------------------------------------------------" << endl;
    cout << "     ### Start DoFind" << endl;
    cout << "-------------------------------------------------------" << endl;
  }

  // Check pointers
  fMCAvailable = kTRUE;
  if (!fMCTrackArray || !fMCPointArray) {
    fMCAvailable = kFALSE;
  }

  if (!hitArray) {
    cout << "-E- " << GetName() << "::DoFind: "
         << "Hit arrays missing! " << endl;
    return -1;
  }

  // Initialise control counters
  // Int_t nNoTrack     = 0; //[R.K. 01/2017] unused variable?
  // Int_t nNoGemPoint  = 0; //[R.K. 01/2017] unused variable?
  // Int_t nNoGemHit    = 0; //[R.K. 01/2017] unused variable?

  // Create pointers to GemHit and GemPoint
  // PndGemHit*   gemHit   = nullptr; //[R.K. 01/2017] unused variable?
  // PndGemHit*   gemHit2   = nullptr; //[R.K. 01/2017] unused variable?
  // PndTrackCand* gemTrackCand = nullptr; //[R.K. 01/2017] unused variable?

  // Declare variables outside the loop
  // Int_t trackIndex = 0;    // Gem track index //[R.K. 01/2017] unused variable?
  // Int_t relDetID = -1;//3000;   //  //[R.K. 01/2017] unused variable?

  if (fMCAvailable & fVerbose) {
    cout << "# MC Tracks: " << fMCTrackArray->GetEntriesFast() << endl;
    cout << "# MC Points: " << fMCPointArray->GetEntriesFast() << endl;
  }

  // Number of Gem hits
  Int_t nGemHits = hitArray->GetEntriesFast();
  if (fVerbose)
    cout << "# GemHits: " << nGemHits << endl;

  fPrepTime += fTimer.RealTime();
  fTimer.Continue();

  for (Int_t istat1 = 0; istat1 < fDigiPar->GetNStations(); istat1++) {
    for (Int_t istat2 = istat1 + 1; istat2 < fDigiPar->GetNStations(); istat2++) {
      FindTrackSegments(hitArray, istat1, istat2);
    }
  }

  fSegmTime += fTimer.RealTime();
  fTimer.Continue();

  if (fVerbose) {
    cout << "event " << fNofEvents << " >>> " << fTrackSegments.size() << " track segments. " << endl;
    if (fMCAvailable)
      PrintMCTrackSegments(hitArray);
    else
      PrintTrackSegments(hitArray);
  }

  Int_t nr = MatchTrackSegments();

  fMatchTime += fTimer.RealTime();
  fTimer.Continue();

  RemoveCloneTracks(nr);

  fRemoveTime += fTimer.RealTime();
  fTimer.Continue();

  if (fVerbose) {
    cout << "************************************************" << endl;
    if (fMCAvailable)
      PrintMCTracks(hitArray, nr);
    else
      PrintTracks(hitArray, nr);
    cout << "************************************************" << endl;
    cout << "finished printing tracks" << endl;
  }

  nr = CreateTracks(hitArray, trackArray, trackCandArray, nr);

  fWriteTime += fTimer.RealTime();
  fTimer.Continue();

  if (fVerbose) {
    cout << "------------------------------------------------" << endl;
    cout << "!!!!!!!!!!!!!!!!!! " << nr << " tracks have been found" << endl;
    cout << "------------------------------------------------" << endl;
  }

  //  cout << "         PndGemTrackFinderOnHitsTB::DoFind. Found " << nr << " tracks out of " << hitArray->GetEntries() << " hits." << endl;

  fAllTime += fTimer.RealTime();

  fTimer.Stop();

  return nr;
}
// ------------------------------------------------------------

// --- Private method to create tracks ------------------------
Int_t PndGemTrackFinderOnHitsTB::CreateTracks(TClonesArray *hitArray, TClonesArray *trackArray, TClonesArray *trackCandArray, Int_t nofRecoTracks)
{

  Int_t nofCreatedTracks = 0;

  const Int_t kNofRecoTracks = nofRecoTracks;
  const Int_t kNofStatDbl = 2 * fDigiPar->GetNStations();

  Int_t hitIndices[kNofRecoTracks][kNofStatDbl * 10]; // RKCHANGE
  Int_t nofHits[kNofRecoTracks];

  Double_t meanMom[kNofRecoTracks];
  Double_t meanPhi[kNofRecoTracks];
  Double_t meanThe[kNofRecoTracks];
  Int_t nofTS[kNofRecoTracks];

  PndTrackCandHit tcHit;
  PndTrackCand *gemTrackCand;
  PndGemHit *gemHit;
  // PndGemHit* gemHit2; //[R.K. 01/2017] unused variable?

  for (Int_t itr = 0; itr < nofRecoTracks; itr++) {
    nofHits[itr] = 0;
    meanMom[itr] = 0.;
    meanPhi[itr] = 0.;
    meanThe[itr] = 0.;
    nofTS[itr] = 0;

    for (size_t its = 0; its < fTrackSegments.size(); its++) {
      TrackSegmentTB iterTS = fTrackSegments[its];
      if (iterTS.recoTrackIndex != itr)
        continue;

      for (Int_t ihit = 0; ihit < 2; ihit++) {
        Bool_t hitAdded = kFALSE;
        for (Int_t itrh = 0; itrh < nofHits[itr]; itrh++) {
          if (hitIndices[itr][itrh] == iterTS.hitIndex[ihit])
            hitAdded = kTRUE;
        }
        if (hitAdded)
          continue;
        hitIndices[itr][nofHits[itr]] = iterTS.hitIndex[ihit];
        nofHits[itr] += 1;
      }

      if (meanPhi[itr] / (nofTS[itr] + 1) < 5. && iterTS.trackPhi > 355.) {
        iterTS.trackPhi -= 360.;
      }
      if (meanPhi[itr] / (nofTS[itr] + 1) > 355. && iterTS.trackPhi < 5.) {
        iterTS.trackPhi += 360.;
      }
      meanMom[itr] += iterTS.trackMom;
      meanPhi[itr] += iterTS.trackPhi;
      meanThe[itr] += iterTS.trackTheta;
      nofTS[itr]++;
    }

    if (nofTS[itr] == 0)
      continue;

    meanMom[itr] = meanMom[itr] / nofTS[itr];
    meanPhi[itr] = meanPhi[itr] / nofTS[itr];
    meanThe[itr] = meanThe[itr] / nofTS[itr];

    gemTrackCand = new ((*trackCandArray)[nofCreatedTracks]) PndTrackCand();

    //    cout << "TRACK----------------------------------" << endl;

    Double_t sumZ = 0.;
    Double_t sumZSq = 0.;
    Double_t sumZT = 0.;
    Double_t sumT = 0.;
    for (Int_t itrh = 0; itrh < nofHits[itr]; itrh++) {
      gemHit = (PndGemHit *)hitArray->At(hitIndices[itr][itrh]);

      //      cout << "gemHit " << ih << " AT z = " << gemHit->GetZ() << " cm and t = " << gemHit->GetTimeStamp() << " ns" << endl;
      gemTrackCand->AddHit(FairRootManager::Instance()->GetBranchId("GEMHit"), hitIndices[itr][itrh], gemHit->GetPosition().Mag());

      if (fVerbose)
        cout << "hit " << itrh << " @ " << gemHit->GetX() << " " << gemHit->GetY() << " " << gemHit->GetZ() << " was created at " << gemHit->GetTimeStamp() << endl;
      sumZ += gemHit->GetZ();
      sumZSq += gemHit->GetZ() * gemHit->GetZ();
      sumZT += gemHit->GetZ() * gemHit->GetTimeStamp();
      sumT += gemHit->GetTimeStamp();
    }
    Double_t time0 = (sumT * sumZSq - sumZ * sumZT) / (nofHits[itr] * sumZSq - sumZ * sumZ);
    if (fVerbose)
      cout << "----> time0 = " << time0 << endl;

    gemTrackCand->Sort();

    gemTrackCand->SetTimeStamp(time0);

    TVector3 pos(0., 0., 0.);
    TVector3 mom;
    mom.SetMagThetaPhi(TMath::Abs(meanMom[itr]), meanThe[itr] * TMath::DegToRad(), meanPhi[itr] * TMath::DegToRad());

    // cout << "TRACK "
    // 	 << "( " << meanMom[itr] << " , " << meanThe[itr] << " , " << meanPhi[itr] << " ) >>> "
    // 	 << "( " << mom.Mag() << " , " << mom.Theta()*TMath::RadToDeg() << " , " << mom.Phi()*TMath::RadToDeg() << " )" << endl;

    Int_t charge = -1;
    //    if ( mom.Mag() < 0. )
    if (meanThe[itr] < 0.) {
      //      cout << "THE CHARGE IS " << 1 << endl;
      charge = 1;
      mom.SetX(-mom.X());
      mom.SetY(-mom.Y());
    }

    FairTrackParP firstPar(pos, mom, TVector3(0.5, 0.5, 0.5), 0.1 * mom, charge, pos, TVector3(1., 0., 0.), TVector3(0., 1., 0.));
    FairTrackParP lastPar(pos, mom, TVector3(0.5, 0.5, 0.5), // tempMom[0],tempMom[1],tempMom[2]),
                          0.1 * mom, charge, pos, TVector3(1., 0., 0.), TVector3(0., 1., 0.));
    //    cout << "q = " << firstPar->GetQ() << endl;
    //      TClonesArray &pndtracks = *trackArray;
    //      Int_t size = pndtracks.GetEntriesFast();
    //      PndTrack* pndTrack = new(pndtracks[size]) PndTrack(*firstPar, *lastPar, *gemTrackCand);
    new ((*trackArray)[nofCreatedTracks]) PndTrack(firstPar, lastPar, *gemTrackCand);

    /*    PndTrack* checkTrack = (PndTrack*) trackArray->At(nofCreatedTracks);
    for ( Int_t ih = 0 ; ih < kNofStatDbl ; ih++ ) {
      if ( hitIndices[itr][ih] == -1 ) continue;
      checkTrack->AddLink(FairLink("GemHit", hitIndices[itr][ih]));
      }*/

    //     cout << "now q = " << checkTrack->GetParamFirst().GetQ() << endl;

    nofCreatedTracks++;
  }

  return nofCreatedTracks;
}
// ------------------------------------------------------------

// --- Private method to remove clone track -------------------
void PndGemTrackFinderOnHitsTB::RemoveCloneTracks(Int_t nofRecoTracks)
{
  Bool_t printInfo = kFALSE; // TRUE;

  if (fVerbose > 4 || printInfo)
    cout << "Trying to remove clone tracks" << endl;

  const Int_t kNofRecoTracks = nofRecoTracks;
  const Int_t kNofStatDbl = 2 * fDigiPar->GetNStations();

  Int_t hitIndices[kNofRecoTracks][kNofStatDbl];
  Int_t nofHits[kNofRecoTracks];
  Int_t nofMultiHits[kNofRecoTracks];

  Double_t meanMom[kNofRecoTracks];
  Double_t meanPhi[kNofRecoTracks];
  Double_t meanThe[kNofRecoTracks];

  Int_t nofComb = 0;
  for (Int_t i1 = 0; i1 < fDigiPar->GetNStations(); i1++)
    for (Int_t i2 = i1 + 1; i2 < fDigiPar->GetNStations(); i2++)
      nofComb++;
  const Int_t kNofComb = nofComb * 4;

  Int_t nofTS[kNofRecoTracks];
  Double_t valMom[kNofRecoTracks][kNofComb];
  Double_t valPhi[kNofRecoTracks][kNofComb];
  Double_t valThe[kNofRecoTracks][kNofComb];
  Double_t trackCons[kNofRecoTracks];

  for (Int_t itr = 0; itr < nofRecoTracks; itr++) {
    for (Int_t ih = 0; ih < kNofStatDbl; ih++)
      hitIndices[itr][ih] = -1;
    nofHits[itr] = 0;
    nofMultiHits[itr] = 0;
    meanMom[itr] = 0.;
    meanPhi[itr] = 0.;
    meanThe[itr] = 0.;
    nofTS[itr] = 0;
    for (Int_t ic = 0; ic < kNofComb; ic++) {
      valMom[itr][ic] = -666.;
      valPhi[itr][ic] = -666.;
      valThe[itr][ic] = -666.;
    }
    trackCons[itr] = 0.;

    for (size_t its = 0; its < fTrackSegments.size(); its++) {
      TrackSegmentTB iterTS = fTrackSegments[its];
      if (iterTS.recoTrackIndex != itr)
        continue;
      for (Int_t istat = 0; istat < 2; istat++) {
        hitIndices[itr][iterTS.stationIndex[istat]] = iterTS.hitIndex[istat];
        //        hitIndices[itr][2 * iterTS.stationIndex[istat] + 1] = iterTS.hitIndex[2 * istat + 1];
      }

      if (meanPhi[itr] / (nofTS[itr] + 1) < 5. && iterTS.trackPhi > 355.) {
        iterTS.trackPhi -= 360.;
      }
      if (meanPhi[itr] / (nofTS[itr] + 1) > 355. && iterTS.trackPhi < 5.) {
        iterTS.trackPhi += 360.;
      }

      valMom[itr][nofTS[itr]] = iterTS.trackMom;
      valPhi[itr][nofTS[itr]] = iterTS.trackPhi;
      valThe[itr][nofTS[itr]] = iterTS.trackTheta;
      meanMom[itr] += iterTS.trackMom;
      meanPhi[itr] += iterTS.trackPhi;
      meanThe[itr] += iterTS.trackTheta;
      nofTS[itr]++;
    }
    for (Int_t isens = 0; isens < kNofStatDbl; isens++)
      if (hitIndices[itr][isens] > -0.5)
        nofHits[itr] += 1;

    meanMom[itr] = meanMom[itr] / nofTS[itr];
    meanPhi[itr] = meanPhi[itr] / nofTS[itr];
    meanThe[itr] = meanThe[itr] / nofTS[itr];

    if (fVerbose > 4 || printInfo)
      cout << " MEAN = " << meanMom[itr] << " " << meanPhi[itr] << " " << meanThe[itr] << endl;
    for (Int_t its = 0; its < nofTS[itr]; its++) {
      if (fVerbose > 4 || printInfo)
        cout << " v" << its << " = " << valMom[itr][its] << " " << valPhi[itr][its] << " " << valThe[itr][its] << endl;
      trackCons[itr] += TMath::Abs((valMom[itr][its] - meanMom[itr]) / meanMom[itr]);
      trackCons[itr] += TMath::Abs(valPhi[itr][its] - meanPhi[itr]);
      trackCons[itr] += TMath::Abs(valThe[itr][its] - meanThe[itr]);
    }
    trackCons[itr] = trackCons[itr] / (3. * nofTS[itr]);
  }

  for (Int_t itr = 0; itr < nofRecoTracks; itr++) {
    for (Int_t ih = 0; ih < kNofStatDbl; ih++) {
      if (hitIndices[itr][ih] == -1)
        continue;
      Bool_t hitFound = kFALSE;
      for (Int_t itr2 = 0; itr2 < nofRecoTracks; itr2++) {
        if (itr == itr2)
          continue;
        for (Int_t ih2 = 0; ih2 < kNofStatDbl; ih2++) {
          if (hitIndices[itr2][ih2] == -1)
            continue;
          if (hitIndices[itr2][ih2] == hitIndices[itr][ih]) {
            hitFound = kTRUE;
            nofMultiHits[itr]++;
            break;
          }
        }
        if (hitFound)
          break;
      }
    }

    Int_t cloneIndicators = 0;
    if (trackCons[itr] > 1)
      cloneIndicators++; // TS's too different
    if (nofTS[itr] <= kNofComb * 2 / 3)
      cloneIndicators++; // not enough TS
    if (nofMultiHits[itr] >= nofHits[itr] / 3)
      cloneIndicators++; // too many hits shared with other tracks

    if (cloneIndicators >= 2) { // remove this track candidate
      for (size_t its = 0; its < fTrackSegments.size(); its++) {
        TrackSegmentTB iterTS = fTrackSegments[its];
        if (iterTS.recoTrackIndex != itr)
          continue;
        iterTS.recoTrackIndex = -1;
        fTrackSegments[its] = iterTS;
      }
    }
    if (fVerbose > 4 || printInfo) {
      cout << " consistency = " << trackCons[itr] << (trackCons[itr] > 1 ? " YES" : "") << " ( > 1 )" << endl;
      cout << " segments    = " << nofTS[itr] << (nofTS[itr] <= kNofComb * 2 / 3 ? " YES" : "") << " ( <= " << kNofComb * 2 / 3 << " )" << endl;
      cout << " multihits   = " << nofMultiHits[itr] << (nofMultiHits[itr] >= nofHits[itr] / 3 ? " YES" : "") << " ( >= " << nofHits[itr] / 3 << " )" << endl;
      cout << "TRACK " << itr << " HAS " << nofMultiHits[itr] << " MULTI HITS AND CONSISTENCY = " << trackCons[itr] << (cloneIndicators >= 2 ? " >>> REMOVED!!!! " : "") << endl;
    }
  }
}
// ------------------------------------------------------------

// --- Private method to match track segments -----------------
Int_t PndGemTrackFinderOnHitsTB::MatchTrackSegments()
{
  //  const Int_t kMaxNofSegments = fDigiPar->GetNStations()-1;

  Bool_t printInfo = kFALSE; // TRUE;

  Int_t nofRecoTracks = 0;

  for (size_t itrc = 0; itrc < fTrackSegments.size(); itrc++) {
    TrackSegmentTB origTS = fTrackSegments[itrc];
    if (origTS.recoTrackIndex != -1)
      continue; // this track segment has already been used
    if (fVerbose > 4 || printInfo)
      cout << "MATCH TRACK SEGMENTS TO SEGM " << itrc << " with params: " << origTS.trackMom << " " << origTS.trackPhi << " " << origTS.trackTheta << endl;
    //    if ( origTS.stationIndex[1] != origTS.stationIndex[0]+1 ) continue; // only consider segments of hits on neighbouring stations

    vector<Int_t> trackSegs;
    trackSegs.push_back(itrc);

    Bool_t overrepr = kFALSE;
    for (size_t itrc2 = 0; itrc2 < fTrackSegments.size(); itrc2++) {
      if (itrc2 == itrc)
        continue; // do not match segment with itself
      TrackSegmentTB matchTS = fTrackSegments[itrc2];
      if (origTS.stationIndex[0] == matchTS.stationIndex[0] && origTS.stationIndex[1] == matchTS.stationIndex[1] && origTS.sensorNumber[0] == matchTS.sensorNumber[0] &&
          origTS.sensorNumber[1] == matchTS.sensorNumber[1])
        continue;
      if (matchTS.recoTrackIndex != -1)
        continue; // this track segment has already been used
      //      if ( matchTS.stationIndex[1] != matchTS.stationIndex[0]+1 ) continue; // only consider segments of hits on neighbouring stations

      // check if track matches with any of the track segments in trackSegs
      Double_t meanMom = 0.;
      Double_t meanPhi = 0.;
      Double_t meanThe = 0.;
      Bool_t matching = kFALSE;
      for (size_t its = 0; its < trackSegs.size(); its++) {
        TrackSegmentTB iterTS = fTrackSegments[trackSegs[its]];
        if (!matching) {
          if (matchTS.hitIndex[0] == iterTS.hitIndex[0]) {
            matching = kTRUE;
          } // break; } // matchTS matches after iterTS
          if (matchTS.hitIndex[1] == iterTS.hitIndex[1]) {
            matching = kTRUE;
          } // break; } // matchTS matches before iterTS
          if (matchTS.hitIndex[0] == iterTS.hitIndex[1]) {
            matching = kTRUE;
          } // break; } // matchTS shares hit on first station with iterTS
          if (matchTS.hitIndex[1] == iterTS.hitIndex[0]) {
            matching = kTRUE;
          } // break; } // matchTS shares hit on second station with iterTS
        }
        meanMom += iterTS.trackMom / (trackSegs.size());
        meanPhi += iterTS.trackPhi / (trackSegs.size());
        meanThe += iterTS.trackTheta / (trackSegs.size());
      }
      if (!matching)
        continue; // matchTS does not match to any track segment in trackSegs

      if (meanPhi < 5. && matchTS.trackPhi > 355.)
        matchTS.trackPhi -= 360.;
      if (meanPhi > 355. && matchTS.trackPhi < 5.)
        matchTS.trackPhi += 360.;

      if (TMath::Abs(matchTS.trackPhi - meanPhi) > 5. || TMath::Abs(matchTS.trackTheta - meanThe) > 5. || TMath::Abs(matchTS.trackMom - meanMom) > 0.3 * TMath::Abs(meanMom)) {
        if (fVerbose > 4 || printInfo) {
          cout << " mean > " << meanMom << " " << meanPhi << " " << meanThe << endl;
          cout << " ++++ > " << matchTS.trackMom << " " << matchTS.trackPhi << " " << matchTS.trackTheta << endl;
        }
        continue;
      }

      // check if there exists in trackSegs a segment that uses different hits on some station
      Bool_t mismatch = kFALSE;
      Bool_t mismatchSolved = kFALSE;
      for (size_t its = 0; its < trackSegs.size(); its++) {
        TrackSegmentTB iterTS = fTrackSegments[trackSegs[its]];
        if (matchTS.stationIndex[0] == iterTS.stationIndex[0] && matchTS.stationIndex[1] == iterTS.stationIndex[1] && matchTS.sensorNumber[0] == iterTS.sensorNumber[0] &&
            matchTS.sensorNumber[1] == iterTS.sensorNumber[1]) {

          if (fVerbose > 4 || printInfo)
            cout << "there are already " << trackSegs.size() << " segments in this track: " << endl;
          meanMom = 0.;
          meanPhi = 0.;
          meanThe = 0.;
          for (size_t its2 = 0; its2 < trackSegs.size(); its2++) {
            if (its == its2)
              continue; // the track segment in question should not go to mean
            TrackSegmentTB aveTS = fTrackSegments[trackSegs[its2]];
            if (fVerbose > 4 || printInfo)
              cout << its2 << " (" << trackSegs[its2] << ") > " << aveTS.trackMom << " " << aveTS.trackPhi << " " << aveTS.trackTheta << endl;
            meanMom += aveTS.trackMom / (trackSegs.size() - 1);
            meanPhi += aveTS.trackPhi / (trackSegs.size() - 1);
            meanThe += aveTS.trackTheta / (trackSegs.size() - 1);
          }
          if (fVerbose > 4 || printInfo) {
            cout << its << " (" << trackSegs[its] << ") > " << iterTS.trackMom << " " << iterTS.trackPhi << " " << iterTS.trackTheta << endl;
            cout << " this track conflicts with track " << its << " and is: " << endl;
            cout << "C (" << itrc2 << ") > " << matchTS.trackMom << " " << matchTS.trackPhi << " " << matchTS.trackTheta << endl;
            cout << " should decide on mean of other track segments, which is: " << endl;
            cout << "MEAN   > " << meanMom << " " << meanPhi << " " << meanThe << endl;
          }
          if (meanPhi < 5.) {
            if (iterTS.trackPhi > 355.)
              iterTS.trackPhi -= 360.;
            if (matchTS.trackPhi > 355.)
              matchTS.trackPhi -= 360.;
          }
          if (meanPhi > 355) {
            if (iterTS.trackPhi < 5.)
              iterTS.trackPhi += 360.;
            if (matchTS.trackPhi < 5.)
              matchTS.trackPhi += 360.;
          }
          Bool_t changeSegm = kFALSE;
          // check if new segment is better than the old
          if (TMath::Abs(matchTS.trackPhi - meanPhi) < TMath::Abs(iterTS.trackPhi - meanPhi))       // && TMath::Abs(matchTS.trackPhi-meanPhi) < 5. )
            if (TMath::Abs(matchTS.trackTheta - meanThe) < TMath::Abs(iterTS.trackTheta - meanThe)) // && TMath::Abs(matchTS.trackTheta-meanThe) < 5. )
              if (TMath::Abs(matchTS.trackMom - meanMom) < TMath::Abs(iterTS.trackMom - meanMom))   // && 0.3*TMath::Abs(meanMom) )
                changeSegm = kTRUE;
          // new segment is better, replace it
          if (changeSegm) {
            if (fVerbose)
              cout << "Changing segment " << trackSegs[its] << " with segment " << itrc2 << endl;
            trackSegs[its] = itrc2;
            mismatchSolved = kTRUE;
            continue;
          }
          /*		if ( TMath::Abs(iterTS.trackPhi-meanPhi) < 5. )
            if ( TMath::Abs(iterTS.trackTheta-meanThe)  < 5. )
            if ( TMath::Abs(iterTS.trackMom-meanMom) < 0.3*TMath::Abs(meanMom) ) {
            }
            mismatch = kTRUE;
            break;
          */
          mismatchSolved = kTRUE;
          continue;
        }
      }
      if (mismatch) {
        if (fVerbose > 4 || printInfo)
          cout << " PROBLEM HERE " << endl;
        overrepr = kTRUE;
        break;
      }
      if (mismatchSolved) {
        continue;
      }
      // matchTS matches with some of trackSegs, and it does not conflict with any of trackSegs

      trackSegs.push_back(itrc2);
    }
    if (overrepr)
      continue;
    if (trackSegs.size() == 1)
      continue;

    if (fVerbose > 4 || printInfo)
      cout << "segments: " << endl;
    for (size_t its = 0; its < trackSegs.size(); its++) {
      TrackSegmentTB iterTS = fTrackSegments[trackSegs[its]];
      if (fVerbose > 4 || printInfo) {
        cout << iterTS.stationIndex[0] << "." << iterTS.sensorNumber[0] << " " << iterTS.stationIndex[1] << "." << iterTS.sensorNumber[1] << " > " << flush;
        for (Int_t ihit = 0; ihit < 2; ihit++)
          cout << setw(4) << iterTS.hitIndex[ihit] << " " << flush;
        cout << setw(11) << iterTS.trackMom << " " << setw(11) << iterTS.trackPhi << " " << setw(11) << iterTS.trackTheta << endl;
      }
      iterTS.recoTrackIndex = nofRecoTracks;
      fTrackSegments[trackSegs[its]] = iterTS;
    }
    if (fVerbose > 4 || printInfo)
      cout << " seems to belong to one track" << endl;
    nofRecoTracks++;
  }

  if (fVerbose || printInfo)
    cout << "********* " << nofRecoTracks << " track candidates have been found" << endl;

  return nofRecoTracks;
}
// ------------------------------------------------------------

// ---- Private function to find track segments on 2 stations --------------------
Int_t PndGemTrackFinderOnHitsTB::FindTrackSegments(TClonesArray *hitArray, Int_t stat1Id, Int_t stat2Id)
{
  Bool_t printInfo = kFALSE; // TRUE;

  PndGemHit *gemHit1;
  PndGemHit *gemHit2;
  Double_t gemHit1X = 0., gemHit1Y = 0., gemHit1Z = 0., gemHit1T = 0.;
  Double_t gemHit2X = 0., gemHit2Y = 0., gemHit2Z = 0., gemHit2T = 0.;

  Int_t nGemHits = hitArray->GetEntriesFast();

  for (Int_t iHit1 = 0; iHit1 < nGemHits; iHit1++) {
    // Get the pointer to Gem hit
    gemHit1 = (PndGemHit *)hitArray->At(iHit1);
    if (gemHit1->GetCharge() < 1.)
      continue; // RKChange
    if (gemHit1->GetStationNr() - 1 != stat1Id)
      continue;
    if (fVerbose > 3 || printInfo)
      cout << "LOOKING FOR TRACK SEGMENTS STARTING AT " << gemHit1->GetX() << " " << gemHit1->GetY() << " " << gemHit1->GetZ() << endl;

    gemHit1X = gemHit1->GetX();
    gemHit1Y = gemHit1->GetY();
    gemHit1Z = gemHit1->GetZ();
    gemHit1T = gemHit1->GetTimeStamp();

    if (fVerbose > 3 || printInfo)
      cout << "possible segment " << fTrackSegments.size() << " starts here " << gemHit1X << " " << gemHit1Y << " " << gemHit1Z << endl;
    Double_t radius = TMath::Sqrt(gemHit1X * gemHit1X + gemHit1Y * gemHit1Y);
    Double_t pangle = TMath::ACos(gemHit1X / radius);
    if (gemHit1Y < 0)
      pangle = 2. * TMath::Pi() - pangle;
    Double_t trackTheta = fParThetaA * radius / gemHit1Z + fParThetaB;
    if (fVerbose > 3 || printInfo)
      cout << "     -> with theta of " << trackTheta << " (radius = " << radius << " and phi angle = " << pangle * TMath::RadToDeg() << ")" << endl;

    for (Int_t iHit2 = 0; iHit2 < nGemHits; iHit2++) {
      gemHit2 = (PndGemHit *)hitArray->At(iHit2);
      if (gemHit2->GetCharge() < 1.)
        continue; // RKChange
      if (gemHit2->GetStationNr() - 1 != stat2Id)
        continue;

      gemHit2X = gemHit2->GetX();
      gemHit2Y = gemHit2->GetY();
      gemHit2Z = gemHit2->GetZ();
      gemHit2T = gemHit2->GetTimeStamp();

      if (gemHit2T < gemHit1T || gemHit2T > gemHit1T + 11.)
        continue; // the hits are too far away in time or hit2 is before hit1

      Double_t radius2 = TMath::Sqrt(gemHit2X * gemHit2X + gemHit2Y * gemHit2Y);
      Double_t pangle2 = TMath::ACos(gemHit2X / radius2);
      if (gemHit2Y < 0)
        pangle2 = 2. * TMath::Pi() - pangle2;

      if (pangle < TMath::Pi() / 2. && pangle2 > TMath::Pi() * 3. / 2.)
        pangle2 -= TMath::Pi() * 2.;
      if (pangle > TMath::Pi() * 3. / 2. && pangle2 < TMath::Pi() / 2.)
        pangle2 += TMath::Pi() * 2.;

      if (TMath::Abs(pangle - pangle2) * TMath::RadToDeg() > 40)
        continue;

      if (fVerbose > 3 || printInfo) {
        cout << "       trying to match it with " << gemHit2X << " , " << gemHit2Y << " , " << gemHit2Z << endl;
        cout << "       (radius = " << radius2 << " and phi angle = " << pangle2 * TMath::RadToDeg() << ")" << endl;
      }

      Double_t dphi = pangle2 - pangle;
      Double_t expectedRad2 = radius / (TMath::Cos(dphi) - TMath::Sin(dphi) / TMath::Tan(gemHit2Z / (gemHit2Z - gemHit1Z) * dphi));

      Double_t zDistRatio = gemHit2Z / gemHit1Z;
      Double_t zDiffRatio = gemHit1Z / (gemHit2Z - gemHit1Z);
      Double_t zCuDiff = gemHit2Z * gemHit2Z * gemHit2Z - gemHit1Z * gemHit1Z * gemHit1Z;
      Double_t zSqDiff = gemHit2Z * gemHit2Z - gemHit1Z * gemHit1Z;
      Double_t zDiff = gemHit2Z - gemHit1Z;
      Double_t par0_mom = fParMat0[0] * zCuDiff + fParMat0[1] * zSqDiff + fParMat0[2] * zDiff;
      Double_t par1_mom = fParMat1[0] * zCuDiff + fParMat1[1] * zSqDiff + fParMat1[2] * zDiff;

      Double_t expRadUncert = 0.08 * radius * zDistRatio; // changed 0.05 to  0.08

      if (fVerbose > 3 || printInfo)
        cout << " -> while expected radius was " << expectedRad2 << " with error of " << expRadUncert << endl;

      if (radius2 > expectedRad2 + expRadUncert || radius2 < expectedRad2 - expRadUncert)
        continue;

      if (fVerbose > 4 || printInfo)
        cout << "STRONG CORRELATION FOR THIS HIT!!!" << endl;
      // calculate phi and momentum basing on the pangle-pangle2;
      Double_t trackMomentum = 666.;
      if ((pangle - pangle2) != 0)
        trackMomentum = (par0_mom + par1_mom / radius) / ((pangle - pangle2) * TMath::RadToDeg());
      ;

      if (fVerbose > 0 && stat1Id == 0 && stat2Id == 1) {
        cout << "CALCULATED MOMENTUM " << trackMomentum << " FROM ( " << par0_mom << " + " << par1_mom << " / " << radius << " ) / ( " << pangle << " - " << pangle2 << " ) * "
             << TMath::RadToDeg() << " = ( " << par0_mom + par1_mom / radius << " ) / ( " << pangle - pangle2 << " ) * " << TMath::RadToDeg() << endl;
        cout << pangle - pangle2 << " from " << TMath::ACos(gemHit1X / radius) << " - " << TMath::ACos(gemHit2X / radius2) << " = ACos( " << gemHit1X << " / " << radius
             << ") - ACos( " << gemHit2X << " / " << radius2 << " ) " << endl;
        cout << "                     " << gemHit1X << "           " << gemHit2X << endl;
        cout << "                     " << gemHit1Y << "           " << gemHit2Y << endl;
        cout << "                     " << gemHit1Z << "           " << gemHit2Z << endl;
      }
      Double_t circRad = TMath::Sqrt((gemHit2X - gemHit1X) * (gemHit2X - gemHit1X) + (gemHit2Y - gemHit1Y) * (gemHit2Y - gemHit1Y)) / TMath::Sin(pangle - pangle2) / 2.;

      Double_t mom_trans = circRad / 196.;

      Double_t newMomPar = 4.585;             // value for stations 2-3
      if (TMath::Abs(gemHit1Z - 117.) < 5.) { // station 1
        if (TMath::Abs(gemHit2Z - 153.) < 5.) // station 2
          newMomPar = 5.602;                  // value for stations 1-2
        if (TMath::Abs(gemHit2Z - 188.) < 5.) // station 3
          newMomPar = 10.188;                 // value for stations 1-3
      }

      trackMomentum = newMomPar / (TMath::RadToDeg() * TMath::Abs(pangle2 - pangle));

      trackTheta = TMath::ATan(mom_trans / trackMomentum);

      // if ( fVerbose > 3 || printInfo )
      // 	cout << "calculated track momentum is " << trackMomentum << endl;
      Double_t trackPhiAngle = pangle + (pangle - pangle2) * zDiffRatio;
      if (trackPhiAngle < 0.)
        trackPhiAngle += TMath::Pi() * 2.;
      if (trackPhiAngle > TMath::Pi() * 2.)
        trackPhiAngle -= TMath::Pi() * 2.;

      // if ( fVerbose > 3 || printInfo )
      // 	cout << "calculated phi is " << trackPhiAngle*TMath::RadToDeg() << endl;

      // if ( fVerbose ) {
      // 	cout << "OK, THE MOMENTUM_" << gemHit1->GetStationNr() << gemHit2->GetStationNr()
      // 	     << "  mom = " << trackMomentum
      // 	     << "  the = " << trackTheta*TMath::RadToDeg()
      // 	     << "  phi = " << trackPhiAngle*TMath::RadToDeg()
      // 	     << " >>> mom_trans " << mom_trans << endl;
      // }

      TrackSegmentTB tempTS;
      tempTS.stationIndex[0] = stat1Id;
      tempTS.stationIndex[1] = stat2Id;
      tempTS.sensorNumber[0] = gemHit1->GetSensorNr();
      tempTS.sensorNumber[1] = gemHit2->GetSensorNr();
      tempTS.hitIndex[0] = iHit1;
      tempTS.hitIndex[1] = iHit2;
      tempTS.trackMom = trackMomentum;
      tempTS.trackPhi = trackPhiAngle * TMath::RadToDeg();
      tempTS.trackTheta = trackTheta * TMath::RadToDeg();
      tempTS.recoTrackIndex = -1;

      //      cout << "SEGMENT CREATED BY HITS @ " << gemHit1->GetTimeStamp() << " (" << gemHit1->GetZ() << ") AND " << gemHit2->GetTimeStamp() << " (" << gemHit2->GetZ() << ")" <<
      //      endl;

      if (fVerbose > 2 || printInfo) {
        cout << " FOUND SEGMENT (stat. " << stat1Id << " & " << stat2Id << "), hits " << iHit1 << ", " << iHit2 << " >>> " << trackMomentum << " GeV, "
             << trackPhiAngle * TMath::RadToDeg() << " deg, " << trackTheta << " deg." << endl;
      }

      fTrackSegments.push_back(tempTS);
    }
  }

  if (fVerbose > 2 || printInfo)
    cout << "===>>> " << fTrackSegments.size() << " track segments" << endl;
  return fTrackSegments.size();
}
// ------------------------------------------------------------

// --- Private method to print track segments -----------------
void PndGemTrackFinderOnHitsTB::PrintTrackSegments(TClonesArray *hitArray)
{
  PndGemHit *gemHit;

  // const Int_t kNofGemStations = fDigiPar->GetNStations(); //[R.K. 01/2017] unused variable?

  for (size_t itrc = 0; itrc < fTrackSegments.size(); itrc++) {
    TrackSegmentTB tempTS = fTrackSegments[itrc];
    cout << tempTS.stationIndex[0] << " " << tempTS.stationIndex[1] << " >> segment " << itrc << ": " << flush;
    for (Int_t ihit = 0; ihit < 2; ihit++) {
      gemHit = (PndGemHit *)hitArray->At(tempTS.hitIndex[ihit]);
      cout << " <" << gemHit->GetX() << "/" << gemHit->GetY() << "> " << flush;
      cout << setw(3) << tempTS.hitIndex[ihit] << flush;
    }
    cout << setw(11) << tempTS.trackMom << " " << setw(11) << tempTS.trackPhi << " " << setw(11) << tempTS.trackTheta << endl;
  }
}
// ------------------------------------------------------------

// --- Private method to print track segments -----------------
void PndGemTrackFinderOnHitsTB::PrintMCTrackSegments(TClonesArray *hitArray)
{
  PndGemHit *gemHit;
  PndGemMCPoint *mcPoint;

  const Int_t kNofMCTracks = fMCTrackArray->GetEntriesFast();

  vector<Int_t> nofFiredStations(kNofMCTracks, 0);
  vector<TVector3> mcTrackMomentum(kNofMCTracks);

  const Int_t kNofGemPoints = fMCPointArray->GetEntriesFast();

  const Int_t kNofGemStations = fDigiPar->GetNStations();
  Int_t nofPointsPerStation[kNofMCTracks][kNofGemStations];
  for (Int_t imct = 0; imct < kNofMCTracks; imct++)
    for (Int_t is = 0; is < kNofGemStations; is++)
      nofPointsPerStation[imct][is] = 0;

  for (Int_t imcp = 0; imcp < kNofGemPoints; imcp++) {
    mcPoint = (PndGemMCPoint *)fMCPointArray->At(imcp);

    Int_t stationNr = fDigiPar->GetStationNr(mcPoint->GetSensorId());

    nofPointsPerStation[mcPoint->GetTrackID()][stationNr - 1] += 1;
  }

  Int_t nofCGM = 0;
  for (Int_t imct = 0; imct < kNofMCTracks; imct++) {
    //    fMCTrackNofCrossedGemStations[imct] = nofCGM;
    nofCGM = 0;
    for (Int_t is = 0; is < kNofGemStations; is++)
      if (nofPointsPerStation[imct][is] > 0)
        nofCGM++;
    nofFiredStations[imct] = nofCGM;
    PndMCTrack *mcTrack = (PndMCTrack *)fMCTrackArray->At(imct);
    mcTrackMomentum[imct] = mcTrack->GetMomentum();
  }

  vector<Int_t> nofTrSegments(kNofMCTracks, 0);
  vector<Int_t> nofTrMCId(kNofMCTracks, 0);
  vector<Int_t> segmentMCId(fTrackSegments.size(), -1);

  for (size_t itrc = 0; itrc < fTrackSegments.size(); itrc++) {
    for (Int_t itr = 0; itr < kNofMCTracks; itr++)
      nofTrMCId[itr] = 0;
    TrackSegmentTB tempTS = fTrackSegments[itrc];
    cout << tempTS.stationIndex[0] << " " << tempTS.stationIndex[1] << " >> segment " << itrc << ": " << flush;
    for (Int_t ihit = 0; ihit < 2; ihit++) {
      gemHit = (PndGemHit *)hitArray->At(tempTS.hitIndex[ihit]);
      cout << " <" << gemHit->GetX() << "/" << gemHit->GetY() << "> " << flush;
      cout << setw(3) << tempTS.hitIndex[ihit] << flush;
      mcPoint = (PndGemMCPoint *)fMCPointArray->At(gemHit->GetRefIndex());
      cout << setw(2) << gemHit->GetRefIndex() << "/" << mcPoint->GetTrackID() << ") " << flush;

      if (ihit < 2 && nofTrMCId[mcPoint->GetTrackID()] < 1)
        nofTrMCId[mcPoint->GetTrackID()] += 1;
      if (ihit > 1 && nofTrMCId[mcPoint->GetTrackID()] < 2)
        nofTrMCId[mcPoint->GetTrackID()] += 2;
    }
    cout << setw(11) << tempTS.trackMom << " " << setw(11) << tempTS.trackPhi << " " << setw(11) << tempTS.trackTheta << endl;
    Int_t bestMCId = -1;
    for (Int_t itr = 0; itr < kNofMCTracks; itr++) {
      if (nofTrMCId[itr] != 3)
        continue;
      if (bestMCId > -1) {
        bestMCId = -1;
        break;
      }
      bestMCId = itr;
    }
    cout << "                                            " << flush;
    if (bestMCId == -1)
      cout << "            -- NO MATCHING MC -----------------" << endl;
    else {
      cout << "MC " << setw(3) << bestMCId << " TRUTH: " << setw(11) << mcTrackMomentum[bestMCId].Mag() << " " << setw(11)
           << TMath::RadToDeg() * mcTrackMomentum[bestMCId].Phi() + (mcTrackMomentum[bestMCId].Phi() >= 0. ? 0 : 360.) << " " << setw(11)
           << TMath::RadToDeg() * mcTrackMomentum[bestMCId].Theta() << endl;
      nofTrSegments[bestMCId] += 1;
    }
    segmentMCId[itrc] = bestMCId;
  }

  Int_t expNofS = 0;
  Int_t fndNofS = 0;
  for (Int_t imct = 0; imct < kNofMCTracks; imct++) {
    if (nofTrSegments[imct] == 0 || nofFiredStations[imct] == 0)
      continue;
    cout << " track " << imct << " fired " << nofFiredStations[imct] << " stations, and " << nofTrSegments[imct] << " segments were created:" << endl;

    for (size_t itrc = 0; itrc < fTrackSegments.size(); itrc++) {
      if (segmentMCId[itrc] != imct)
        continue;
      TrackSegmentTB tempTS = fTrackSegments[itrc];
      cout << " " << itrc << "   " << setw(11) << tempTS.trackMom << " " << setw(11) << tempTS.trackPhi << " " << setw(11) << tempTS.trackTheta << endl;
    }

    if (mcTrackMomentum[imct].Mag() < 0.5)
      continue;
    Int_t expSegms = 0;
    for (Int_t is = 0; is < nofFiredStations[imct]; is++)
      for (Int_t is2 = is + 1; is2 < nofFiredStations[imct]; is2++)
        expSegms++;
    expNofS += expSegms;
    fndNofS += nofTrSegments[imct];
  }
  fNofExpectedTrackSegments += expNofS;
  fNofFoundTrackSegments += fndNofS;

  cout << "********* FOUND " << fndNofS << " OUT OF " << expNofS << " SEGMENTS IN THIS EVENT" << endl;
  cout << "********* FOUND " << fNofFoundTrackSegments << " OUT OF " << fNofExpectedTrackSegments << " SEGMENTS IN ALL EVENTS" << endl;
}
// ------------------------------------------------------------

// --- Private method to print track candidates ---------------
void PndGemTrackFinderOnHitsTB::PrintTracks(TClonesArray *hitArray, Int_t nofRecoTracks)
{

  PndGemHit *gemHit;

  const Int_t kNofStatDbl = 2 * fDigiPar->GetNStations();

  for (Int_t itr = 0; itr < nofRecoTracks; itr++) {
    Double_t meanMom = 0.;
    Double_t meanPhi = 0.;
    Double_t meanThe = 0.;
    //      meanMom = 0.;
    //      meanPhi = 0.;
    //      meanThe = 0.;

    vector<Int_t> hitIndices(kNofStatDbl, -1);
    cout << "===================== TRACK " << itr << " ======================" << endl;
    Int_t nofTS = 0;
    for (size_t its = 0; its < fTrackSegments.size(); its++) {
      TrackSegmentTB iterTS = fTrackSegments[its];
      if (iterTS.recoTrackIndex != itr)
        continue;
      //      for ( Int_t ih = 0 ; ih < 4 ; ih++ ) cout << its << " > " << ih << " > " <<  iterTS.hitIndex[ih] << endl;
      hitIndices[2 * iterTS.stationIndex[0] - 1 + iterTS.sensorNumber[0]] = iterTS.hitIndex[0];
      hitIndices[2 * iterTS.stationIndex[1] - 1 + iterTS.sensorNumber[1]] = iterTS.hitIndex[1];
      cout << "segm " << its << " " << iterTS.trackMom << " " << iterTS.trackPhi << " " << iterTS.trackTheta << " " << iterTS.stationIndex[0] << "." << iterTS.sensorNumber[0]
           << ": " << iterTS.hitIndex[0] << " / " << iterTS.stationIndex[1] << "." << iterTS.sensorNumber[1] << ": " << iterTS.hitIndex[1] << endl;
      meanMom += iterTS.trackMom;
      meanPhi += iterTS.trackPhi;
      meanThe += iterTS.trackTheta;
      nofTS++;
    }
    meanMom = meanMom / nofTS;
    meanPhi = meanPhi / nofTS;
    meanThe = meanThe / nofTS;

    if (nofTS == 0) {
      cout << "THIS TRACK WAS REMOVED " << endl;
      continue;
    }

    for (Int_t ihit = 0; ihit < kNofStatDbl; ihit++) {
      cout << ihit << "/" << hitIndices[ihit] << "/" << flush;
      if (hitIndices[ihit] == -1) {
        cout << "- - " << flush;
        continue;
      }
      gemHit = (PndGemHit *)hitArray->At(hitIndices[ihit]);
      if (gemHit->GetRefIndex() == -1) {
        cout << "- " << flush;
        continue;
      }
      cout << setw(2) << gemHit->GetRefIndex() << "_ " << flush;
    }
    cout << endl;
  }
}
// ------------------------------------------------------------

// --- Private method to print track candidates ---------------
void PndGemTrackFinderOnHitsTB::PrintMCTracks(TClonesArray *hitArray, Int_t nofRecoTracks)
{

  PndGemHit *gemHit;
  FairMCPoint *mcPoint;

  const Int_t kNofStatDbl = 2 * fDigiPar->GetNStations();

  for (Int_t itr = 0; itr < nofRecoTracks; itr++) {
    vector<Int_t> nofTrMCId(500, 0);

    Double_t meanMom = 0.;
    Double_t meanPhi = 0.;
    Double_t meanThe = 0.;
    //      meanMom = 0.;
    //      meanPhi = 0.;
    //      meanThe = 0.;

    vector<Int_t> hitIndices(kNofStatDbl, -1);
    cout << "===================== TRACK " << itr << " ======================" << endl;
    Int_t nofTS = 0;
    for (size_t its = 0; its < fTrackSegments.size(); its++) {
      TrackSegmentTB iterTS = fTrackSegments[its];
      if (iterTS.recoTrackIndex != itr)
        continue;
      //      for ( Int_t ih = 0 ; ih < 4 ; ih++ ) cout << its << " > " << ih << " > " <<  iterTS.hitIndex[ih] << endl;
      hitIndices[2 * iterTS.stationIndex[0] - 1 + iterTS.sensorNumber[0]] = iterTS.hitIndex[0];
      hitIndices[2 * iterTS.stationIndex[1] - 1 + iterTS.sensorNumber[1]] = iterTS.hitIndex[1];
      cout << "segm " << its << " " << iterTS.trackMom << " " << iterTS.trackPhi << " " << iterTS.trackTheta << " " << iterTS.stationIndex[0] << "." << iterTS.sensorNumber[0]
           << ": " << iterTS.hitIndex[0] << " / " << iterTS.stationIndex[1] << "." << iterTS.sensorNumber[1] << ": " << iterTS.hitIndex[1] << endl;
      meanMom += iterTS.trackMom;
      meanPhi += iterTS.trackPhi;
      meanThe += iterTS.trackTheta;
      nofTS++;
    }
    meanMom = meanMom / nofTS;
    meanPhi = meanPhi / nofTS;
    meanThe = meanThe / nofTS;

    if (nofTS == 0) {
      cout << "THIS TRACK WAS REMOVED " << endl;
      continue;
    }

    for (Int_t ihit = 0; ihit < kNofStatDbl; ihit++) {
      cout << ihit << "/" << hitIndices[ihit] << "/" << flush;
      if (hitIndices[ihit] == -1) {
        cout << "- - " << flush;
        continue;
      }
      gemHit = (PndGemHit *)hitArray->At(hitIndices[ihit]);
      if (gemHit->GetRefIndex() == -1) {
        cout << "- - " << flush;
        continue;
      }
      mcPoint = (FairMCPoint *)fMCPointArray->At(gemHit->GetRefIndex());
      cout << setw(2) << gemHit->GetRefIndex() << " _" << mcPoint->GetTrackID() << "_ " << flush;
      nofTrMCId[mcPoint->GetTrackID()] += 1;
    }
    cout << endl;
    Int_t bestMCId = -1;
    Int_t largestNofTracks = 0;
    for (Int_t itm = 0; itm < 500; itm++) {
      if (nofTrMCId[itm] == largestNofTracks) {
        bestMCId = -1;
      }
      if (nofTrMCId[itm] > largestNofTracks) {
        bestMCId = itm;
        largestNofTracks = nofTrMCId[itm];
      }
    }
    cout << "                                                        " << flush;
    cout << setw(11) << meanMom << " " << setw(11) << meanPhi << " " << setw(11) << meanThe << endl;
    cout << "                                            " << flush;
    if (bestMCId == -1)
      cout << "            -- NO MATCHING MC -----------------" << endl;
    else {
      PndMCTrack *mcTrack = (PndMCTrack *)fMCTrackArray->At(bestMCId);
      TVector3 mcmom = mcTrack->GetMomentum();
      cout << "MC " << bestMCId << " TRUTH: " << setw(11) << mcmom.Mag() << " " << setw(11) << TMath::RadToDeg() * mcmom.Phi() + (mcmom.Phi() >= 0. ? 0 : 360.) << " " << setw(11)
           << TMath::RadToDeg() * mcmom.Theta() << endl;
    }
  }
}
// ------------------------------------------------------------

// --- Private method to print info at the end ---------------
void PndGemTrackFinderOnHitsTB::Finish()
{
  cout << "---------------------------------------------------------------------" << endl;
  cout << " >>> TF >>> prep    time = " << fPrepTime << "s      (get data from input)" << endl;
  cout << " >>> TF >>> segm    time = " << fSegmTime - fPrepTime << "s    (create track segments, " << fSegmTime << ")" << endl;
  cout << " >>> TF >>> match   time = " << fMatchTime - fSegmTime << "s    (match track segments, " << fMatchTime << ")" << endl;
  cout << " >>> TF >>> remove  time = " << fRemoveTime - fMatchTime << "s      (remove clone tracks, " << fRemoveTime << ")" << endl;
  cout << " >>> TF >>> write   time = " << fWriteTime - fRemoveTime << "s    (wrie tracks, " << fWriteTime << ")" << endl;
  cout << " >>> TF >>> all     time = " << fAllTime - fWriteTime << "s       (all time spent in DoFind, " << fAllTime << ")" << endl;
  cout << "---------------------------------------------------------------------" << endl;
}
// ------------------------------------------------------------

ClassImp(PndGemTrackFinderOnHitsTB)
