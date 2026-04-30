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
// -----                PndMvdGemTrackFinderOnHits source file             -----
// -----                Created 02.06.2009 by R. Karabowicz            -----
// -------------------------------------------------------------------------

#include "PndMvdGemTrackFinderOnHits.h"

// Pnd includes
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairBaseParSet.h"
#include "FairTrackParam.h"
#include "FairTrackParP.h"
#include "FairLogger.h"
#include "PndSdsMCPoint.h"
#include "PndGemMCPoint.h"
#include "PndGemDigiPar.h"
#include "FairRootManager.h"
#include "PndDetectorList.h"
#include "PndTrack.h"
#include "PndTrackCand.h"
#include "PndTrackCandHit.h"
// ROOT includes
#include "TClonesArray.h"
#include "TGeoManager.h"
#include "TMatrixFSym.h"

// C++ includes
#include <iostream>
#include <iomanip>
#include <map>
#include <cmath>
using std::cout;
using std::endl;
using std::map;

// -----   Default constructor   -------------------------------------------
PndMvdGemTrackFinderOnHits::PndMvdGemTrackFinderOnHits() : PndPersistencyTask("MvdGem TrackFinder On Hits", 0)
{
  fMvdPixelHitArray = nullptr;
  fMvdStripHitArray = nullptr;
  fGemHitArray = nullptr;

  fTrackArray = nullptr;
  fTrackCandArray = nullptr;

  fMCTrackArray = nullptr;
  fMCGemPointArray = nullptr;
  fMCMvdPointArray = nullptr;
  fNofEvents = 0;
}

// -----   Default constructor   -------------------------------------------
PndMvdGemTrackFinderOnHits::PndMvdGemTrackFinderOnHits(Int_t iVerbose) : PndPersistencyTask("MvdGem TrackFinder On Hits", iVerbose)
{
  fMvdPixelHitArray = nullptr;
  fMvdStripHitArray = nullptr;
  fGemHitArray = nullptr;

  fTrackArray = nullptr;
  fTrackCandArray = nullptr;

  fMCTrackArray = nullptr;
  fMCGemPointArray = nullptr;
  fMCMvdPointArray = nullptr;
  fNofEvents = 0;

  SetPersistency(kTRUE);
}

// -----   Destructor   ----------------------------------------------------
PndMvdGemTrackFinderOnHits::~PndMvdGemTrackFinderOnHits()
{
  fTrackArray->Delete();
  fTrackCandArray->Delete();
}

// -----   Init  -----------------------------------------------------------
InitStatus PndMvdGemTrackFinderOnHits::Init()
{

  fNofExpectedTrackSegments = 0;
  fNofFoundTrackSegments = 0;

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

  // Get MCTrack array
  fMCTrackArray = (TClonesArray *)ioman->GetObject("MCTrack");
  if (!fMCTrackArray)
    LOG(info) << " " << GetName() << "::Init: No MCTrack array!";

  // Get PndGemPoint (MCPoint) array
  fMCGemPointArray = (TClonesArray *)ioman->GetObject("GEMPoint");
  if (!fMCGemPointArray)
    LOG(info) << " " << GetName() << "::Init: No MCGemPoint array!";

  // Get PndMvdPoint (MCPoint) array
  fMCMvdPointArray = (TClonesArray *)ioman->GetObject("MVDPoint");
  if (!fMCMvdPointArray)
    LOG(info) << " " << GetName() << "::Init: No MCMvdPoint array!";

  // Get Mvd pixel hit Array
  fMvdPixelHitArray = (TClonesArray *)ioman->GetObject("MVDHitsPixel");
  if (!fMvdPixelHitArray) {
    LOG(warn) << " " << GetName() << "::Init: No PndMvdPixelHit array!";
    return kERROR;
  }
  // Get Mvd strip hit Array
  fMvdStripHitArray = (TClonesArray *)ioman->GetObject("MVDHitsStrip");
  if (!fMvdStripHitArray) {
    LOG(warn) << " " << GetName() << "::Init: No PndMvdStripHit array!";
    return kERROR;
  }
  // Get Gem hit Array
  fGemHitArray = (TClonesArray *)ioman->GetObject("GEMHit");
  if (!fGemHitArray) {
    LOG(warn) << " " << GetName() << "::Init: No PndGemHit array!";
    return kERROR;
  }

  // Create and register output PndTrack array
  fTrackArray = new TClonesArray("PndTrack", 100);
  ioman->Register("MVDGEMTrack", "Gem Tracks", fTrackArray, GetPersistency());

  // Create and register output PndTrackCand array
  fTrackCandArray = new TClonesArray("PndTrackCand", 100);
  ioman->Register("MVDGEMTrackCand", "Gem Track Cands", fTrackCandArray, GetPersistency());

  fDigiPar = (PndGemDigiPar *)(rtdb->getContainer("PndGemDetectors"));
  cout << "-I- "
       << "PndMvdGemTrackFinderOnHits"
       << "::Init(). There are " << fDigiPar->GetNStations() << " GEM stations." << endl;
  cout << "-I- "
       << "PndMvdGemTrackFinderOnHits"
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

  std::cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << std::endl;
  LOG(info) << " " << GetName() << ": Intialization successfull";
  std::cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << std::endl;

  return kSUCCESS;
}

// -----   Private method SetParContainers   -------------------------------
void PndMvdGemTrackFinderOnHits::SetParContainers()
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

// -----   Private method ReInit   -----------------------------------------
InitStatus PndMvdGemTrackFinderOnHits::ReInit()
{

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Public method DoFind   ------------------------------------------
void PndMvdGemTrackFinderOnHits::Exec(Option_t *)
{
  if (fVerbose > 0) {
    cout << "===========================================" << endl;
    cout << "=============== EVENT " << fNofEvents << " =================" << endl;
  }
  fTrackArray->Delete();
  fTrackCandArray->Delete();

  fNofEvents++;

  fTrackSegments.clear();

  // Check pointers
  fMCAvailable = kTRUE;
  if (!fMCTrackArray || !fMCGemPointArray || !fMCMvdPointArray)
    fMCAvailable = kFALSE;

  for (Int_t itr = 0; itr < fMCTrackArray->GetEntriesFast(); itr++) {
    PndMCTrack *mcTrack = (PndMCTrack *)fMCTrackArray->At(itr);
    TVector3 mcmom = mcTrack->GetMomentum();
    //     cout << "MC " << itr << " p = : "
    //  	 << setw(11) << mcmom.Mag() << " GeV, "
    //  	 << setw(11) << TMath::RadToDeg()*mcmom.Phi()+(mcmom.Phi()>=0.?0:360.) << " deg, "
    //  	 << setw(11) << TMath::RadToDeg()*mcmom.Theta() << " deg." << endl;
  }
  /*
  for ( Int_t itp = 0 ; itp < fMCMvdPointArray->GetEntriesFast() ; itp++ ) {
    PndSdsMCPoint* mcPoint = (PndSdsMCPoint*) fMCMvdPointArray->At(itp);
    cout << " MVD #" << itp << " *" << mcPoint->GetTrackID() << "* "
   << mcPoint->GetX() << " "
   << mcPoint->GetY() << " "
   << mcPoint->GetZ() << " " << endl;
  }
  for ( Int_t itp = 0 ; itp < fMCGemPointArray->GetEntriesFast() ; itp++ ) {
    PndGemMCPoint* mcPoint = (PndGemMCPoint*) fMCGemPointArray->At(itp);
    cout << " GEM #" << itp << " *" << mcPoint->GetTrackID() << "* "
   << mcPoint->GetX() << " "
   << mcPoint->GetY() << " "
   << mcPoint->GetZ() << " " << endl;
  }
  */

  // Initialise control counters
  // Int_t nNoTrack     = 0; //[R.K. 01/2017] unused variable?
  // Int_t nNoGemPoint  = 0; //[R.K. 01/2017] unused variable?
  // Int_t nNoGemHit    = 0; //[R.K. 01/2017] unused variable?

  FindTrackSegments();
  for (Int_t istat1 = 0; istat1 < fDigiPar->GetNStations(); istat1++) {
    FindTrackSegments(istat1);
    for (Int_t istat2 = istat1 + 1; istat2 < fDigiPar->GetNStations(); istat2++) {
      FindTrackSegments(istat1, istat2);
    }
  }

  if (fVerbose) {
    cout << "event " << fNofEvents << " >>> " << fTrackSegments.size() << " track segments. " << endl;
    if (fMCAvailable)
      PrintMCTrackSegments();
    else
      PrintTrackSegments();
  }

  Int_t nr = MatchTrackSegments();

  RemoveCloneTracks(nr);

  if (fVerbose) {
    cout << "************************************************" << endl;
    if (fMCAvailable)
      PrintMCTracks(nr);
    else
      PrintTracks(nr);
    cout << "************************************************" << endl;
    cout << "finished printing tracks" << endl;
  }

  nr = CreateTracks(nr);

  if (fVerbose) {
    cout << "------------------------------------------------" << endl;
    cout << "!!!!!!!!!!!!!!!!!! " << nr << " tracks have been found" << endl;
    cout << "------------------------------------------------" << endl;
  }
}
// ------------------------------------------------------------

// --- Private method to create tracks ------------------------
Int_t PndMvdGemTrackFinderOnHits::CreateTracks(Int_t nofRecoTracks)
{
  Bool_t printInfo = kFALSE; // kTRUE;

  Int_t nofCreatedTracks = 0;

  const Int_t kNofRecoTracks = nofRecoTracks;

  const Int_t maxNofHits = 100;
  Int_t hitIndices[kNofRecoTracks][maxNofHits][2]; // detId, hitId for each track hit
  Int_t nofHits[kNofRecoTracks];
  Double_t meanMom[kNofRecoTracks];
  Double_t meanPhi[kNofRecoTracks];
  Double_t meanThe[kNofRecoTracks];
  Int_t nofTS[kNofRecoTracks];

  PndTrackCandHit tcHit;
  PndTrackCand *trackCand;
  PndGemHit *gemHit;
  PndSdsHit *mvdHit;

  Int_t itr = -1;
  for (itr = 0; itr < nofRecoTracks; itr++) {
    nofHits[itr] = 0;
    meanMom[itr] = 0.;
    meanPhi[itr] = 0.;
    meanThe[itr] = 0.;
    nofTS[itr] = 0;
  }

  for (size_t its = 0; its < fTrackSegments.size(); its++) {
    TrackSegment iterTS = fTrackSegments[its];
    if (iterTS.recoTrackIndex == -1)
      continue;
    itr = iterTS.recoTrackIndex;

    Bool_t hitInTrack[2] = {kFALSE, kFALSE};
    for (Int_t ih1 = 0; ih1 < 2; ih1++) {
      for (Int_t ihit = 0; ihit < nofHits[itr]; ihit++)
        if (hitIndices[itr][ihit][0] == iterTS.detId[ih1] && hitIndices[itr][ihit][1] == iterTS.hitIndex[ih1]) {
          hitInTrack[ih1] = kTRUE;
          break;
        }
      if (hitInTrack[ih1] == kFALSE) {
        hitIndices[itr][nofHits[itr]][0] = iterTS.detId[ih1];
        hitIndices[itr][nofHits[itr]][1] = iterTS.hitIndex[ih1];
        nofHits[itr]++;
      }
    }

    if (nofTS[itr] > 0) {
      if (meanPhi[itr] / nofTS[itr] < 5. && iterTS.trackPhi > 355.) {
        iterTS.trackPhi -= 360.;
      }
      if (meanPhi[itr] / nofTS[itr] > 355. && iterTS.trackPhi < 5.) {
        iterTS.trackPhi += 360.;
      }
    }
    meanMom[itr] += iterTS.trackMom;
    meanPhi[itr] += iterTS.trackPhi;
    meanThe[itr] += iterTS.trackTheta;
    nofTS[itr]++;
  }
  for (itr = 0; itr < nofRecoTracks; itr++) {
    if (nofTS[itr] == 0)
      continue;

    meanMom[itr] = meanMom[itr] / nofTS[itr];
    meanPhi[itr] = meanPhi[itr] / nofTS[itr];
    meanThe[itr] = meanThe[itr] / nofTS[itr];

    trackCand = new ((*fTrackCandArray)[nofCreatedTracks]) PndTrackCand();

    if (fVerbose > 3 || printInfo) {
      cout << "---------------------------------------------------------" << endl;
      cout << "  Track " << itr << " segments:" << endl;

      for (size_t its = 0; its < fTrackSegments.size(); its++) {
        TrackSegment iterTS = fTrackSegments[its];
        if (iterTS.recoTrackIndex != itr)
          continue;
        cout << iterTS.trackMom << " GeV/c, " << iterTS.trackTheta << " deg theta, " << iterTS.trackPhi << " deg phi)" << endl;
      }

      cout << " track has " << nofHits[itr] << " hits:" << endl;
    }
    Int_t nGemHits = 0;
    Int_t nMvdStrH = 0;
    Int_t nMvdPixH = 0;
    for (Int_t ihit = 0; ihit < nofHits[itr]; ihit++) {
      Int_t detId = static_cast<int>(DetectorType::kGemHit);
      if (hitIndices[itr][ihit][0] < 0)
        detId = (-hitIndices[itr][ihit][0]) >> 27;

      if (fVerbose > 3 || printInfo) {
        cout << "    #" << ihit << ". " << hitIndices[itr][ihit][0] << "." << hitIndices[itr][ihit][1] << " -> " << detId << flush;
      }

      if (detId == static_cast<int>(DetectorType::kGemHit)) {
        gemHit = (PndGemHit *)fGemHitArray->At(hitIndices[itr][ihit][1]);
        trackCand->AddHit(FairRootManager::Instance()->GetBranchId("GEMHit"), hitIndices[itr][ihit][1], gemHit->GetPosition().Mag());
        nGemHits += 1;
        if (fVerbose > 3 || printInfo) {
          cout << "  ( " << gemHit->GetX() << "    " << gemHit->GetY() << "    " << gemHit->GetZ() << " )  " << flush;
        }
      } else if (detId == static_cast<int>(DetectorType::kMVDHitsStrip)) { // FIXME: Use FairRootManager::Instance()->GetBranchId()
        mvdHit = (PndSdsHit *)fMvdStripHitArray->At(hitIndices[itr][ihit][1]);
        trackCand->AddHit(FairRootManager::Instance()->GetBranchId("MVDHitsStrip"), hitIndices[itr][ihit][1], mvdHit->GetPosition().Mag());
        nMvdStrH += 1;
        if (fVerbose > 3 || printInfo) {
          cout << "  ( " << mvdHit->GetX() << "    " << mvdHit->GetY() << "    " << mvdHit->GetZ() << " )  " << flush;
        }
      } else if (detId == static_cast<int>(DetectorType::kMVDHitsPixel)) { // FIXME: Use FairRootManager::Instance()->GetBranchId()
        mvdHit = (PndSdsHit *)fMvdPixelHitArray->At(hitIndices[itr][ihit][1]);
        trackCand->AddHit(FairRootManager::Instance()->GetBranchId("MVDHitsPixel"), hitIndices[itr][ihit][1], mvdHit->GetPosition().Mag());
        nMvdPixH += 1;
        if (fVerbose > 3 || printInfo) {
          cout << "  ( " << mvdHit->GetX() << "    " << mvdHit->GetY() << "    " << mvdHit->GetZ() << " )  " << flush;
        }
      }
      if (fVerbose > 3 || printInfo) {
        cout << endl;
      }
    }

    trackCand->Sort();

    TVector3 pos(0., 0., 0.);
    TVector3 mom;
    mom.SetMagThetaPhi(TMath::Abs(meanMom[itr]), meanThe[itr] * TMath::DegToRad(), meanPhi[itr] * TMath::DegToRad());

    Int_t charge = -1;
    if (mom.Mag() < 0.)
      charge = 1;

    FairTrackParP *firstPar = new FairTrackParP(pos, mom, TVector3(0.5, 0.5, 0.5), 0.1 * mom, charge, pos, TVector3(1., 0., 0.), TVector3(0., 1., 0.));
    FairTrackParP *lastPar = new FairTrackParP(TVector3(nGemHits, nMvdStrH, nMvdPixH), // should be something else;)
                                               mom, TVector3(0.5, 0.5, 0.5), 0.1 * mom, charge, pos, TVector3(1., 0., 0.), TVector3(0., 1., 0.));

    new ((*fTrackArray)[nofCreatedTracks]) PndTrack(*firstPar, *lastPar, *trackCand);

    if (fVerbose > 2 || printInfo)
      cout << "TRACK " << nofCreatedTracks << " HAS momentum: (" << meanMom[itr] << " GeV/c, " << meanThe[itr] << " deg theta, " << meanPhi[itr] << " deg phi)" << endl;

    if (fVerbose > 3 || printInfo) {
      mom.SetMagThetaPhi(TMath::Abs(meanMom[itr]), meanThe[itr] * TMath::DegToRad(), meanPhi[itr] * TMath::DegToRad());

      cout << endl;
      cout << " momentum: (" << meanMom[itr] << " GeV/c, " << meanThe[itr] << " deg theta, " << meanPhi[itr] << " deg phi)" << endl;
      cout << "---------------------------------------------------------" << endl;
    }

    nofCreatedTracks++;
  }
  return nofCreatedTracks;
}
// ------------------------------------------------------------

// --- Private method to remove clone track -------------------
void PndMvdGemTrackFinderOnHits::RemoveCloneTracks(Int_t nofRecoTracks)
{
  Bool_t printInfo = kFALSE; // TRUE;

  if (fVerbose > 4 || printInfo)
    cout << "Trying to remove clone tracks" << endl;

  const Int_t kNofRecoTracks = nofRecoTracks;

  const Int_t maxNofHits = 100;
  Double_t meanMom[kNofRecoTracks];                // mean momentum of segments in track
  Double_t meanPhi[kNofRecoTracks];                // mean phi angle of segments in track
  Double_t meanThe[kNofRecoTracks];                // mean theta angle of segments in track
  Int_t hitIndices[kNofRecoTracks][maxNofHits][2]; // detId, hitId for each track hit
  Int_t nofHits[kNofRecoTracks];                   // number of hits in track
  Int_t nofTS[kNofRecoTracks];                     // number of segments in track
  Double_t trackCons[kNofRecoTracks];              // kind of chi/ndf
  Int_t whatToDo[kNofRecoTracks];                  // =itr don't do anything, =-1 remove, =n attach to track n

  for (Int_t itr = 0; itr < kNofRecoTracks; itr++) {
    meanMom[itr] = 0.;
    meanPhi[itr] = 0.;
    meanThe[itr] = 0.;
    nofHits[itr] = 0;
    nofTS[itr] = 0;
    trackCons[itr] = 0.;
  }

  Int_t itr = -1;
  for (size_t its = 0; its < fTrackSegments.size(); its++) {
    TrackSegment iterTS = fTrackSegments[its];
    if (iterTS.recoTrackIndex == -1)
      continue;
    itr = iterTS.recoTrackIndex;

    if (nofTS[itr] != 0) {
      if (meanPhi[itr] / nofTS[itr] < 5. && iterTS.trackPhi > 355.) {
        iterTS.trackPhi -= 360.;
      }
      if (meanPhi[itr] / nofTS[itr] > 355. && iterTS.trackPhi < 5.) {
        iterTS.trackPhi += 360.;
      }
    }

    meanMom[itr] += iterTS.trackMom;
    meanPhi[itr] += iterTS.trackPhi;
    meanThe[itr] += iterTS.trackTheta;
    nofTS[itr] += 1;
  }
  for (itr = 0; itr < kNofRecoTracks; itr++) {
    meanMom[itr] = meanMom[itr] / nofTS[itr];
    meanPhi[itr] = meanPhi[itr] / nofTS[itr];
    meanThe[itr] = meanThe[itr] / nofTS[itr];
  }

  for (size_t its = 0; its < fTrackSegments.size(); its++) {
    TrackSegment iterTS = fTrackSegments[its];
    if (iterTS.recoTrackIndex == -1)
      continue;
    itr = iterTS.recoTrackIndex;

    trackCons[itr] += TMath::Abs((iterTS.trackMom - meanMom[itr]) / meanMom[itr]);
    trackCons[itr] += TMath::Abs(iterTS.trackPhi - meanPhi[itr]);
    trackCons[itr] += TMath::Abs(iterTS.trackTheta - meanThe[itr]);
  }

  for (itr = 0; itr < kNofRecoTracks; itr++) {
    trackCons[itr] = trackCons[itr] / (3. * nofTS[itr]);

    if (trackCons[itr] > 1. && nofTS[itr] < 6) {
      whatToDo[itr] = -1;
      if (fVerbose > 4)
        cout << "TRACK " << itr << " with " << nofTS[itr] << " segments will be deleted" << endl;
      continue;
    }
    whatToDo[itr] = itr;

    for (Int_t itr2 = 0; itr2 < itr; itr2++) {
      Double_t mmm = 0.5 * (TMath::Abs(meanMom[itr]) + TMath::Abs(meanMom[itr2]));
      Double_t similarity = (TMath::Abs(meanPhi[itr] - meanPhi[itr2]) + TMath::Abs(meanThe[itr] - meanThe[itr2]) + TMath::Abs(meanMom[itr] - meanMom[itr2]) / mmm);
      //      cout << " -->  " << meanPhi[itr ] << "   " << meanThe[itr ] << "   " << meanMom[itr ] << "    \\ " << endl;
      //      cout << " -->  " << meanPhi[itr2] << "   " << meanThe[itr2] << "   " << meanMom[itr2] << "    : " << similarity << endl;
      //       if ( TMath::Abs(meanPhi[itr]-meanPhi[itr2]) < 0. &&
      // 	   TMath::Abs(meanThe[itr]-meanThe[itr2]) < 0. &&
      // 	   (TMath::Abs(meanMom[itr]-meanMom[itr2]) < 0.3*TMath::Abs(meanMom[itr2])||
      // 	    TMath::Abs(meanMom[itr]-meanMom[itr2]) < 0.3*TMath::Abs(meanMom[itr ])) ) {
      if (similarity < 2.35) { // 2.35 chosen basen on several events
        if (whatToDo[itr2] == -1)
          whatToDo[itr2] = itr2;
        whatToDo[itr] = whatToDo[itr2];
        break;
      }
    }

    if (fVerbose > 4 || printInfo) {
      cout << " TRACK " << itr << " with " << nofTS[itr] << " segments:" << endl;
      cout << " MEAN = " << meanMom[itr] << " " << meanPhi[itr] << " " << meanThe[itr] << endl;
      cout << " CONS = " << trackCons[itr] << " ========> " << whatToDo[itr] << endl;
    }
  }

  for (size_t its = 0; its < fTrackSegments.size(); its++) {
    TrackSegment iterTS = fTrackSegments[its];
    if (iterTS.recoTrackIndex == -1)
      continue;
    iterTS.recoTrackIndex = whatToDo[iterTS.recoTrackIndex];
    fTrackSegments[its] = iterTS;
    if (iterTS.recoTrackIndex == -1)
      continue;
    itr = iterTS.recoTrackIndex;

    Bool_t hitInTrack[2] = {kFALSE, kFALSE};
    // Bool_t staInTrack[2] = {kFALSE,kFALSE}; //[R.K. 01/2017] unused variable?
    for (Int_t ih1 = 0; ih1 < 2; ih1++) {
      for (Int_t ihit = 0; ihit < nofHits[itr]; ihit++)
        if (hitIndices[itr][ihit][0] == iterTS.detId[ih1])
          if (hitIndices[itr][ihit][1] == iterTS.hitIndex[ih1]) {
            hitInTrack[ih1] = kTRUE;
          }
      if (hitInTrack[ih1] == kFALSE) {
        hitIndices[itr][nofHits[itr]][0] = iterTS.detId[ih1];
        hitIndices[itr][nofHits[itr]][1] = iterTS.hitIndex[ih1];
        nofHits[itr]++;
      }
    }
  }
  for (itr = 0; itr < kNofRecoTracks; itr++) {
    if (fVerbose > 3) {
      cout << "---Track " << itr << " has " << nofTS[itr] << " segments and " << nofHits[itr] << " hits:" << endl;
      for (Int_t ihit = 0; ihit < nofHits[itr]; ihit++)
        cout << "    #" << ihit << ". " << hitIndices[itr][ihit][0] << "." << hitIndices[itr][ihit][1] << " " << endl;
    }
  }
}
// ------------------------------------------------------------

// --- Private method to match track segments -----------------
Int_t PndMvdGemTrackFinderOnHits::MatchTrackSegments()
{
  //  const Int_t kMaxNofSegments = fDigiPar->GetNStations()-1;

  Bool_t printInfo = kFALSE; // TRUE;

  Int_t nofRecoTracks = 0;

  for (size_t itrc = 0; itrc < fTrackSegments.size(); itrc++) {
    TrackSegment origTS = fTrackSegments[itrc];
    if (origTS.recoTrackIndex != -1)
      continue; // this track segment has already been used
    if (fVerbose > 4 || printInfo)
      cout << "MATCH TRACK SEGMENTS TO SEGM " << itrc << " with params: " << origTS.trackMom << " " << origTS.trackPhi << " " << origTS.trackTheta << endl;
    //    if ( origTS.stationIndex[1] != origTS.stationIndex[0]+1 ) continue; // only consider segments of hits on neighbouring stations

    vector<Int_t> trackSegs;
    trackSegs.push_back(itrc);

    for (size_t itrc2 = 0; itrc2 < fTrackSegments.size(); itrc2++) {
      if (itrc2 == itrc)
        continue; // do not match segment with itself
      TrackSegment matchTS = fTrackSegments[itrc2];
      if (origTS.detId[0] == matchTS.detId[0] && origTS.detId[1] == matchTS.detId[1])
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
        TrackSegment iterTS = fTrackSegments[trackSegs[its]];

        if (TMath::Abs(matchTS.trackPhi - iterTS.trackPhi) < 1.0 && TMath::Abs(matchTS.trackTheta - iterTS.trackTheta) < 0.2 &&
            TMath::Abs(matchTS.trackMom - iterTS.trackMom) < 0.1 * TMath::Abs(iterTS.trackMom)) {
          matching = kTRUE;
          break;
        }
      }

      //       cout << "mean:(" << meanMom << "," << meanPhi << "," << meanThe << ") "
      // 	   << "does" << (matching?"":" not") << " match with "
      // 	   << "(" << matchTS.trackMom << "," << matchTS.trackPhi << "," << matchTS.trackTheta << ")" << endl;
      if (!matching)
        continue; // matchTS does not match to any track segment in trackSegs

      //      cout << "segment belongs to track" << endl;
      // check if there exists in trackSegs a segment that uses different hits on some station
      Bool_t sameStationSegment = kFALSE;
      for (size_t its = 0; its < trackSegs.size(); its++) {
        TrackSegment iterTS = fTrackSegments[trackSegs[its]];
        // different stations
        if (matchTS.detId[0] != iterTS.detId[0] || matchTS.detId[1] != iterTS.detId[1])
          continue;
        sameStationSegment = kTRUE;
        if (matchTS.hitIndex[0] == iterTS.hitIndex[0] && matchTS.hitIndex[1] != iterTS.hitIndex[1])
          continue; // the same segment
        if (fVerbose > 4 || printInfo)
          cout << "there are already " << trackSegs.size() << " segments in this track: " << endl;
        meanMom = 0.;
        meanPhi = 0.;
        meanThe = 0.;
        for (size_t its2 = 0; its2 < trackSegs.size(); its2++) {
          if (its == its2)
            continue; // the track segment in question should not go to mean
          TrackSegment aveTS = fTrackSegments[trackSegs[its2]];
          if (fVerbose > 4 || printInfo)
            cout << its2 << " (" << trackSegs[its2] << ") > " << aveTS.trackMom << " " << aveTS.trackPhi << " " << aveTS.trackTheta << " ---- size = " << trackSegs.size() - 1
                 << endl;
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
        // check if new segment is better than the old
        if (TMath::Abs(matchTS.trackPhi - meanPhi) < TMath::Abs(iterTS.trackPhi - meanPhi) && TMath::Abs(matchTS.trackTheta - meanThe) < TMath::Abs(iterTS.trackTheta - meanThe) &&
            TMath::Abs(matchTS.trackMom - meanMom) < TMath::Abs(iterTS.trackMom - meanMom)) {
          trackSegs[its] = itrc2;
          continue;
        }
      }

      if (sameStationSegment)
        continue;

      //      cout << "trackSegs ( now with " << trackSegs.size() << " ) increased by track: " << itrc2 << endl;
      trackSegs.push_back(itrc2);
    }
    if (trackSegs.size() < 5)
      continue;

    if (fVerbose > 4 || printInfo)
      cout << "track " << nofRecoTracks << " segments: " << endl;
    for (size_t its = 0; its < trackSegs.size(); its++) {
      TrackSegment iterTS = fTrackSegments[trackSegs[its]];
      if (fVerbose > 4 || printInfo) {
        cout << iterTS.detId[0] << " " << iterTS.detId[1] << " > " << flush;
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
Int_t PndMvdGemTrackFinderOnHits::FindTrackSegments(Int_t stat1Id, Int_t stat2Id)
{
  Bool_t printInfo = kFALSE; // TRUE;//FALSE;

  PndGemStation *stat1 = (PndGemStation *)fDigiPar->GetStation(stat1Id);
  PndGemStation *stat2 = (PndGemStation *)fDigiPar->GetStation(stat2Id);
  Double_t zStation1 = stat1->GetZ();
  Double_t zStation2 = stat2->GetZ();

  Double_t zDistRatio = zStation2 / zStation1;
  Double_t zDiffRatio = zStation1 / (zStation2 - zStation1);
  Double_t zCuDiff = zStation2 * zStation2 * zStation2 - zStation1 * zStation1 * zStation1;
  Double_t zSqDiff = zStation2 * zStation2 - zStation1 * zStation1;
  Double_t zDiff = zStation2 - zStation1;

  Double_t par0_mom = fParMat0[0] * zCuDiff + fParMat0[1] * zSqDiff + fParMat0[2] * zDiff;
  Double_t par1_mom = fParMat1[0] * zCuDiff + fParMat1[1] * zSqDiff + fParMat1[2] * zDiff;

  Int_t nGemHits = fGemHitArray->GetEntriesFast();

  PndGemHit *gemHit;
  PndGemHit *gemHit2;

  for (Int_t iHit = 0; iHit < nGemHits; iHit++) {
    // Get the pointer to Gem hit
    gemHit = (PndGemHit *)fGemHitArray->At(iHit);
    if (gemHit->GetStationNr() != stat1Id + 1)
      continue;
    zStation1 = gemHit->GetZ();

    if (fVerbose > 3 || printInfo)
      cout << "LOOKING FOR TRACK SEGMENTS STARTING AT " << gemHit->GetX() << " " << gemHit->GetY() << " " << gemHit->GetZ() << "( " << zStation1
           << ") ndigihits = " << gemHit->GetNDigiHits() << endl;
    //    if ( gemHit->GetZ() > zStation1 ) continue;
    if (fVerbose > 3 || printInfo)
      cout << "possible segment " << fTrackSegments.size() << " starts here " << gemHit->GetX() << " " << gemHit->GetY() << " " << gemHit->GetZ() << endl;
    Double_t radius = TMath::Sqrt(gemHit->GetX() * gemHit->GetX() + gemHit->GetY() * gemHit->GetY());
    Double_t pangle = TMath::ACos(gemHit->GetX() / radius);
    if (gemHit->GetY() < 0)
      pangle = 2. * TMath::Pi() - pangle;
    // Double_t theta = TMath::RadToDeg()*TMath::ATan(radius/zStation1);
    Double_t theta = fParThetaA * radius / zStation1 + fParThetaB;
    if (fVerbose > 3 || printInfo)
      cout << "     -> with theta of " << theta << " (radius = " << radius << " and phi angle = " << pangle * TMath::RadToDeg() << ")" << endl;
    for (Int_t iHit2 = 0; iHit2 < nGemHits; iHit2++) {
      gemHit2 = (PndGemHit *)fGemHitArray->At(iHit2);
      if (gemHit2->GetStationNr() != stat2Id + 1)
        continue;
      //      cout << "mixing station " << stat1Id+1 << " and " << stat2Id+1 << " hits, z = " << gemHit->GetZ() << " vs " << gemHit2->GetZ() << endl;
      //      if ( TMath::Abs(gemHit2->GetZ()-(zStation2-.6)) > 0.3 ) continue;
      //      if ( gemHit2->GetNDigiHits() < 0 ) continue;

      zStation2 = gemHit2->GetZ();

      zDistRatio = zStation2 / zStation1;
      zDiffRatio = zStation1 / (zStation2 - zStation1);
      zCuDiff = zStation2 * zStation2 * zStation2 - zStation1 * zStation1 * zStation1;
      zSqDiff = zStation2 * zStation2 - zStation1 * zStation1;
      zDiff = zStation2 - zStation1;

      par0_mom = fParMat0[0] * zCuDiff + fParMat0[1] * zSqDiff + fParMat0[2] * zDiff;
      par1_mom = fParMat1[0] * zCuDiff + fParMat1[1] * zSqDiff + fParMat1[2] * zDiff;

      if (fVerbose > 3 || printInfo)
        cout << "trying to match it with " << gemHit2->GetX() << " " << gemHit2->GetY() << " " << gemHit2->GetZ() << endl;
      Double_t radius2 = TMath::Sqrt(gemHit2->GetX() * gemHit2->GetX() + gemHit2->GetY() * gemHit2->GetY());
      Double_t pangle2 = TMath::ACos(gemHit2->GetX() / radius2);
      if (gemHit2->GetY() < 0)
        pangle2 = 2. * TMath::Pi() - pangle2;

      if (pangle < TMath::Pi() / 2. && pangle2 > TMath::Pi() * 3. / 2.)
        pangle2 -= TMath::Pi() * 2.;
      if (pangle > TMath::Pi() * 3. / 2. && pangle2 < TMath::Pi() / 2.)
        pangle2 += TMath::Pi() * 2.;

      if (TMath::Abs(pangle - pangle2) * TMath::RadToDeg() > 40)
        continue;

      if (fVerbose > 3 || printInfo)
        cout << "       (radius = " << radius2 << " and phi angle = " << pangle2 * TMath::RadToDeg() << ")" << endl;

      Double_t expectedRad2 = (fParRadPhi0 + fParRadPhi2 * TMath::RadToDeg() * TMath::RadToDeg() * (pangle - pangle2) * (pangle - pangle2)) * radius * zDistRatio;
      Double_t expRadUncert = 0.05 * radius * zDistRatio;

      if (fVerbose > 3 || printInfo) {
        cout << " expRad = " << (fParRadPhi0 + fParRadPhi2 * TMath::RadToDeg() * TMath::RadToDeg() * (pangle - pangle2) * (pangle - pangle2)) << " * " << radius << " * "
             << zDistRatio << endl;
        cout << " -> while expected radius was " << expectedRad2 << endl;
      }

      if (radius2 > expectedRad2 + expRadUncert || radius2 < expectedRad2 - expRadUncert)
        continue;

      if (fVerbose > 4 || printInfo)
        cout << "STRONG CORRELATION FOR THIS HIT!!!" << endl;
      // calculate phi and momentum basing on the pangle-pangle2;
      Double_t trackMomentum = 666.;
      if ((pangle - pangle2) != 0)
        trackMomentum = (par0_mom + par1_mom * radius) / ((pangle - pangle2) * TMath::RadToDeg());
      ;
      if (fVerbose > 3 || printInfo)
        cout << "calculated track momentum is " << trackMomentum << endl;
      Double_t trackPhiAngle = pangle + (pangle - pangle2) * zDiffRatio;
      if (trackPhiAngle < 0.)
        trackPhiAngle += TMath::Pi() * 2.;
      if (trackPhiAngle > TMath::Pi() * 2.)
        trackPhiAngle -= TMath::Pi() * 2.;
      if (fVerbose > 3 || printInfo)
        cout << "calculated phi is " << trackPhiAngle * TMath::RadToDeg() << endl;

      theta = (fParTheta0 + 1. / (TMath::Abs(trackMomentum) + fParTheta1 * zStation1 + fParTheta2)) / zStation1 * radius + fParTheta3;

      TrackSegment tempTS;
      tempTS.detId[0] = gemHit->GetDetectorID();
      tempTS.detId[1] = gemHit2->GetDetectorID();
      tempTS.hitIndex[0] = iHit;
      tempTS.hitIndex[1] = iHit2;
      tempTS.trackMom = trackMomentum;
      tempTS.trackPhi = trackPhiAngle * TMath::RadToDeg();
      tempTS.trackTheta = theta;
      tempTS.recoTrackIndex = -1;

      if (fVerbose > 2 || printInfo) {
        cout << " found segment (stat. " << stat1Id << " & " << stat2Id
             << "), hits "
             //	     << iHit << ", " << gemHit->GetNDigiHits() << ", " << iHit2 << ", " << gemHit2->GetNDigiHits()
             << iHit << "(" << tempTS.detId[0] << "), " << iHit2 << "(" << tempTS.detId[1] << "), "
             << " >>> " << trackMomentum << " GeV, " << trackPhiAngle * TMath::RadToDeg() << " deg, " << theta << " deg." << endl;
      }

      fTrackSegments.push_back(tempTS);
    }
  }

  if (fVerbose > 2 || printInfo)
    cout << "===>>> " << fTrackSegments.size() << " track segments" << endl;
  return fTrackSegments.size();
}
// ------------------------------------------------------------

// ---- Private function to find track segments between mvd and gem --------------------
Int_t PndMvdGemTrackFinderOnHits::FindTrackSegments(Int_t stat2Id)
{
  Bool_t printInfo = kFALSE;

  Double_t zStation1;
  Double_t zStation2;

  Double_t zDistRatio;
  Double_t zDiffRatio;
  Double_t zCuDiff;
  Double_t zSqDiff;
  Double_t zDiff;

  Double_t par0_mom;
  Double_t par1_mom;

  Int_t nMvdPixelHits = fMvdPixelHitArray->GetEntriesFast();
  Int_t nMvdStripHits = fMvdStripHitArray->GetEntriesFast();
  Int_t nGemHits = fGemHitArray->GetEntriesFast();

  PndSdsHit *mvdHit1;
  PndGemHit *gemHit2;

  for (Int_t iMvdHit1 = 0; iMvdHit1 < nMvdPixelHits + nMvdStripHits; iMvdHit1++) {
    Int_t mvdHit1Nr = iMvdHit1;
    if (iMvdHit1 < nMvdPixelHits)
      mvdHit1 = (PndSdsHit *)fMvdPixelHitArray->At(mvdHit1Nr);
    else {
      mvdHit1Nr = iMvdHit1 - nMvdPixelHits;
      mvdHit1 = (PndSdsHit *)fMvdStripHitArray->At(mvdHit1Nr);
    }
    zStation1 = mvdHit1->GetZ();
    if (zStation1 < 4.)
      continue; // take only hits in forward part

    if (fVerbose > 3 || printInfo)
      cout << "LOOKING FOR TRACK SEGMENTS STARTING AT " << mvdHit1->GetX() << " " << mvdHit1->GetY() << " " << mvdHit1->GetZ() << "( " << zStation1 << ")" << endl;
    Double_t radius = TMath::Sqrt(mvdHit1->GetX() * mvdHit1->GetX() + mvdHit1->GetY() * mvdHit1->GetY());
    Double_t pangle = TMath::ACos(mvdHit1->GetX() / radius);
    if (mvdHit1->GetY() < 0)
      pangle = 2. * TMath::Pi() - pangle;
    // Double_t theta = TMath::RadToDeg()*TMath::ATan(radius/zStation1);
    Double_t theta = fParThetaA * radius / zStation1 + fParThetaB;
    if (fVerbose > 3 || printInfo)
      cout << "     -> with theta of " << theta << " (radius = " << radius << " and phi angle = " << pangle * TMath::RadToDeg() << ")" << endl;

    for (Int_t iHit2 = 0; iHit2 < nGemHits; iHit2++) {
      gemHit2 = (PndGemHit *)fGemHitArray->At(iHit2);
      if (gemHit2->GetStationNr() != stat2Id + 1)
        continue;

      zStation2 = gemHit2->GetZ();

      zDistRatio = zStation2 / zStation1;
      zDiffRatio = zStation1 / (zStation2 - zStation1);
      zCuDiff = zStation2 * zStation2 * zStation2 - zStation1 * zStation1 * zStation1;
      zSqDiff = zStation2 * zStation2 - zStation1 * zStation1;
      zDiff = zStation2 - zStation1;

      par0_mom = fParMat0[0] * zCuDiff + fParMat0[1] * zSqDiff + fParMat0[2] * zDiff;
      par1_mom = fParMat1[0] * zCuDiff + fParMat1[1] * zSqDiff + fParMat1[2] * zDiff;

      if (fVerbose > 3 || printInfo)
        cout << "trying to match it with " << gemHit2->GetX() << " " << gemHit2->GetY() << " " << gemHit2->GetZ() << endl;
      Double_t radius2 = TMath::Sqrt(gemHit2->GetX() * gemHit2->GetX() + gemHit2->GetY() * gemHit2->GetY());
      Double_t pangle2 = TMath::ACos(gemHit2->GetX() / radius2);
      if (gemHit2->GetY() < 0)
        pangle2 = 2. * TMath::Pi() - pangle2;

      if (pangle < TMath::Pi() / 2. && pangle2 > TMath::Pi() * 3. / 2.)
        pangle2 -= TMath::Pi() * 2.;
      if (pangle > TMath::Pi() * 3. / 2. && pangle2 < TMath::Pi() / 2.)
        pangle2 += TMath::Pi() * 2.;

      if (TMath::Abs(pangle - pangle2) * TMath::RadToDeg() > 40)
        continue;

      if (fVerbose > 3 || printInfo)
        cout << "       (radius = " << radius2 << " and phi angle = " << pangle2 * TMath::RadToDeg() << ")" << endl;

      //      Double_t expectedRad2  = (fParRadPhi0 + fParRadPhi2*TMath::RadToDeg()*TMath::RadToDeg()*(pangle-pangle2)*(pangle-pangle2))*radius*zDistRatio;
      Double_t expectedRad2 = radius * zDistRatio;
      Double_t expRadUncert = 0.05 * radius * zDistRatio;

      if (fVerbose > 3 || printInfo) {
        cout << " expRad = " << (fParRadPhi0 + fParRadPhi2 * TMath::RadToDeg() * TMath::RadToDeg() * (pangle - pangle2) * (pangle - pangle2)) << " * " << radius << " * "
             << zDistRatio << endl;
        cout << " -> while expected radius was " << expectedRad2 << endl;
      }

      if (radius2 > expectedRad2 + expRadUncert || radius2 < expectedRad2 - expRadUncert)
        continue;

      if (fVerbose > 4 || printInfo)
        cout << "STRONG CORRELATION FOR THIS HIT!!!" << endl;
      // calculate phi and momentum basing on the pangle-pangle2;
      Double_t trackMomentum = 666.;
      if ((pangle - pangle2) != 0)
        trackMomentum = (par0_mom + par1_mom * radius) / ((pangle - pangle2) * TMath::RadToDeg());
      ;
      if (fVerbose > 3 || printInfo)
        cout << "calculated track momentum is " << trackMomentum << endl;
      Double_t trackPhiAngle = pangle + (pangle - pangle2) * zDiffRatio;
      if (trackPhiAngle < 0.)
        trackPhiAngle += TMath::Pi() * 2.;
      if (trackPhiAngle > TMath::Pi() * 2.)
        trackPhiAngle -= TMath::Pi() * 2.;
      if (fVerbose > 3 || printInfo)
        cout << "calculated phi is " << trackPhiAngle * TMath::RadToDeg() << endl;

      theta = (fParTheta0 + 1. / (TMath::Abs(trackMomentum) + fParTheta1 * zStation1 + fParTheta2)) / zStation1 * radius + fParTheta3;

      TrackSegment tempTS;
      tempTS.detId[0] = -(mvdHit1->GetSensorID() + (mvdHit1->GetDetectorID() << 27));
      tempTS.detId[1] = gemHit2->GetDetectorID();
      tempTS.hitIndex[0] = mvdHit1Nr;
      tempTS.hitIndex[1] = iHit2;
      tempTS.trackMom = trackMomentum;
      tempTS.trackPhi = trackPhiAngle * TMath::RadToDeg();
      tempTS.trackTheta = theta;
      tempTS.recoTrackIndex = -1;

      if (fVerbose > 2 || printInfo) {
        cout << " found segment "
             //	     << iHit << ", " << gemHit->GetNDigiHits() << ", " << iHit2 << ", " << gemHit2->GetNDigiHits()
             << mvdHit1Nr << "("
             << tempTS.detId[0]
             //<< " + " << mvdHit1->GetSensorID() << " = " << mvdHit1->GetSensorID()+(tempTS.detId[0]<<27)
             << "), " << iHit2 << "(" << tempTS.detId[1] << "), "
             << " >>> " << trackMomentum << " GeV, " << trackPhiAngle * TMath::RadToDeg() << " deg, " << theta << " deg." << endl;
      }

      fTrackSegments.push_back(tempTS);
    }
  }

  if (fVerbose > 2 || printInfo)
    cout << "===>>> " << fTrackSegments.size() << " track segments" << endl;
  return fTrackSegments.size();
}
// ------------------------------------------------------------

// ---- Private function to find track segments between mvd and gem --------------------
Int_t PndMvdGemTrackFinderOnHits::FindTrackSegments()
{
  Bool_t printInfo = kFALSE; // kTRUE;

  Double_t zStation1;
  Double_t zStation2;

  Double_t zDistRatio;
  Double_t zDiffRatio;
  Double_t zCuDiff;
  Double_t zSqDiff;
  Double_t zDiff;

  Double_t par0_mom;
  Double_t par1_mom;

  Int_t nMvdPixelHits = fMvdPixelHitArray->GetEntriesFast();
  Int_t nMvdStripHits = fMvdStripHitArray->GetEntriesFast();
  // Int_t nGemHits = fGemHitArray->GetEntriesFast(); //[R.K. 01/2017] unused variable?

  PndSdsHit *mvdHit1;
  PndSdsHit *mvdHit2;

  for (Int_t iMvdHit1 = 0; iMvdHit1 < nMvdPixelHits + nMvdStripHits; iMvdHit1++) {
    Int_t mvdHit1Nr = iMvdHit1;
    if (iMvdHit1 < nMvdPixelHits)
      mvdHit1 = (PndSdsHit *)fMvdPixelHitArray->At(mvdHit1Nr);
    else {
      mvdHit1Nr = iMvdHit1 - nMvdPixelHits;
      mvdHit1 = (PndSdsHit *)fMvdStripHitArray->At(mvdHit1Nr);
    }
    zStation1 = mvdHit1->GetZ();
    if (zStation1 < 4.)
      continue; // take only hits in forward part

    if (fVerbose > 3 || printInfo)
      cout << "LOOKING FOR TRACK SEGMENTS STARTING AT " << mvdHit1->GetX() << " " << mvdHit1->GetY() << " " << mvdHit1->GetZ() << "( " << zStation1 << ")" << endl;
    Double_t radius = TMath::Sqrt(mvdHit1->GetX() * mvdHit1->GetX() + mvdHit1->GetY() * mvdHit1->GetY());
    Double_t pangle = TMath::ACos(mvdHit1->GetX() / radius);
    if (mvdHit1->GetY() < 0)
      pangle = 2. * TMath::Pi() - pangle;
    // Double_t theta = TMath::RadToDeg()*TMath::ATan(radius/zStation1);
    Double_t theta = fParThetaA * radius / zStation1 + fParThetaB;
    if (fVerbose > 3 || printInfo)
      cout << "     -> with theta of " << theta << " (radius = " << radius << " and phi angle = " << pangle * TMath::RadToDeg() << ")" << endl;

    for (Int_t iMvdHit2 = iMvdHit1 + 1; iMvdHit2 < nMvdPixelHits + nMvdStripHits; iMvdHit2++) {
      Int_t mvdHit2Nr = iMvdHit2;
      if (iMvdHit2 < nMvdPixelHits)
        mvdHit2 = (PndSdsHit *)fMvdPixelHitArray->At(mvdHit2Nr);
      else {
        mvdHit2Nr = iMvdHit2 - nMvdPixelHits;
        mvdHit2 = (PndSdsHit *)fMvdStripHitArray->At(mvdHit2Nr);
      }

      zStation2 = mvdHit2->GetZ();
      if (zStation2 < 4.)
        continue; // take only hits in forward part

      if (TMath::Abs(zStation1 - zStation2) < 2.)
        continue;

      zStation2 = mvdHit2->GetZ();

      zDistRatio = zStation2 / zStation1;
      zDiffRatio = zStation1 / (zStation2 - zStation1);
      zCuDiff = zStation2 * zStation2 * zStation2 - zStation1 * zStation1 * zStation1;
      zSqDiff = zStation2 * zStation2 - zStation1 * zStation1;
      zDiff = zStation2 - zStation1;

      par0_mom = fParMat0[0] * zCuDiff + fParMat0[1] * zSqDiff + fParMat0[2] * zDiff;
      par1_mom = fParMat1[0] * zCuDiff + fParMat1[1] * zSqDiff + fParMat1[2] * zDiff;

      if (fVerbose > 3 || printInfo)
        cout << "trying to match it with " << mvdHit2->GetX() << " " << mvdHit2->GetY() << " " << mvdHit2->GetZ() << endl;
      Double_t radius2 = TMath::Sqrt(mvdHit2->GetX() * mvdHit2->GetX() + mvdHit2->GetY() * mvdHit2->GetY());
      Double_t pangle2 = TMath::ACos(mvdHit2->GetX() / radius2);
      if (mvdHit2->GetY() < 0)
        pangle2 = 2. * TMath::Pi() - pangle2;

      if (pangle < TMath::Pi() / 2. && pangle2 > TMath::Pi() * 3. / 2.)
        pangle2 -= TMath::Pi() * 2.;
      if (pangle > TMath::Pi() * 3. / 2. && pangle2 < TMath::Pi() / 2.)
        pangle2 += TMath::Pi() * 2.;

      if (TMath::Abs(pangle - pangle2) * TMath::RadToDeg() > 40)
        continue;

      if (fVerbose > 3 || printInfo)
        cout << "       (radius = " << radius2 << " and phi angle = " << pangle2 * TMath::RadToDeg() << ")" << endl;

      Double_t expectedRad2 = (fParRadPhi0 + fParRadPhi2 * TMath::RadToDeg() * TMath::RadToDeg() * (pangle - pangle2) * (pangle - pangle2)) * radius * zDistRatio;
      Double_t expRadUncert = 0.05 * radius * zDistRatio;

      if (fVerbose > 3 || printInfo) {
        cout << " expRad = " << (fParRadPhi0 + fParRadPhi2 * TMath::RadToDeg() * TMath::RadToDeg() * (pangle - pangle2) * (pangle - pangle2)) << " * " << radius << " * "
             << zDistRatio << endl;
        cout << " -> while expected radius was " << expectedRad2 << endl;
      }

      if (radius2 > expectedRad2 + expRadUncert || radius2 < expectedRad2 - expRadUncert)
        continue;

      if (fVerbose > 4 || printInfo)
        cout << "STRONG CORRELATION FOR THIS HIT!!!" << endl;
      // calculate phi and momentum basing on the pangle-pangle2;
      Double_t trackMomentum = 666.;
      if ((pangle - pangle2) != 0)
        trackMomentum = 1.6 * (par0_mom + par1_mom * radius) / ((pangle - pangle2) * TMath::RadToDeg());
      // this 1.6:
      // 12.11.2010: first version of the file, parameters are adjusted to GEM detector,
      // for pure MVD the found momentum is too far off
      if (fVerbose > 3 || printInfo)
        cout << "calculated track momentum is " << trackMomentum << endl;
      Double_t trackPhiAngle = pangle + (pangle - pangle2) * zDiffRatio;
      if (trackPhiAngle < 0.)
        trackPhiAngle += TMath::Pi() * 2.;
      if (trackPhiAngle > TMath::Pi() * 2.)
        trackPhiAngle -= TMath::Pi() * 2.;
      if (fVerbose > 3 || printInfo)
        cout << "calculated phi is " << trackPhiAngle * TMath::RadToDeg() << endl;

      theta = (fParTheta0 + 1. / (TMath::Abs(trackMomentum) + fParTheta1 * zStation1 + fParTheta2)) / zStation1 * radius + fParTheta3;

      TrackSegment tempTS;
      tempTS.detId[0] = -(mvdHit1->GetSensorID() + (mvdHit1->GetDetectorID() << 27));
      tempTS.detId[1] = -(mvdHit2->GetSensorID() + (mvdHit2->GetDetectorID() << 27));
      tempTS.hitIndex[0] = mvdHit1Nr;
      tempTS.hitIndex[1] = mvdHit2Nr;
      tempTS.trackMom = trackMomentum;
      tempTS.trackPhi = trackPhiAngle * TMath::RadToDeg();
      tempTS.trackTheta = theta;
      tempTS.recoTrackIndex = -1;

      if (fVerbose > 2 || printInfo) {
        cout << " found segment "
             //	     << iHit << ", " << gemHit->GetNDigiHits() << ", " << mvdHit2Nr << ", " << mvdHit2->GetNDigiHits()
             << mvdHit1Nr << "("
             << tempTS.detId[0]
             //<< " + " << mvdHit1->GetSensorID() << " = " << mvdHit1->GetSensorID()+(tempTS.detId[0]<<27)
             << "), " << mvdHit2Nr << "(" << tempTS.detId[1] << "), "
             << " >>> " << trackMomentum << " GeV, " << trackPhiAngle * TMath::RadToDeg() << " deg, " << theta << " deg." << endl;
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
void PndMvdGemTrackFinderOnHits::PrintTrackSegments()
{
  /*  PndGemHit* gemHit;

  const Int_t kNofGemStations = fDigiPar->GetNStations();

  for ( Int_t itrc = 0 ; itrc < fTrackSegments.size() ; itrc++ ) {
    TrackSegment tempTS = fTrackSegments[itrc];
    cout << tempTS.stationIndex[0] << " " << tempTS.stationIndex[1] << " >> segment " << itrc << ": " << flush;
    for ( Int_t ihit = 0 ; ihit < 4 ; ihit++ ) {
      if ( tempTS.hitIndex[ihit] == -1 ) continue;
      gemHit = (PndGemHit*) fGemHitArray->At(tempTS.hitIndex[ihit]);
      cout << " <" << gemHit->GetX() << "/" << gemHit->GetY() << "> " << flush;
      cout << setw(3) << tempTS.hitIndex[ihit] << "(" << flush;
      if ( gemHit->GetRefIndex() == -1 ) { cout << "--/-) " << flush; continue;}
      cout << setw(2) << gemHit->GetRefIndex() << ") " << flush;

    }
    cout << setw(11) << tempTS.trackMom << " " << setw(11) << tempTS.trackPhi << " " << setw(11) << tempTS.trackTheta << endl;
    }*/
}
// ------------------------------------------------------------

// --- Private method to print track segments -----------------
void PndMvdGemTrackFinderOnHits::PrintMCTrackSegments()
{
  /*  PndGemHit* gemHit;
  PndGemMCPoint* mcPoint;

  const Int_t kNofMCTracks    = fMCTrackArray->GetEntriesFast();

  vector<Int_t> nofFiredStations(kNofMCTracks,0);
  vector<TVector3> mcTrackMomentum(kNofMCTracks,0);

  const Int_t kNofGemPoints = fMCGemPointArray->GetEntriesFast();

  const Int_t kNofGemStations = fDigiPar->GetNStations();
  Int_t nofPointsPerStation[kNofMCTracks][kNofGemStations];
  for ( Int_t imct = 0 ; imct < kNofMCTracks ; imct++ )
    for ( Int_t is = 0 ; is < kNofGemStations ; is++ )
      nofPointsPerStation[imct][is] = 0;

  for ( Int_t imcp = 0 ; imcp < kNofGemPoints ; imcp++ ) {
    mcPoint = (PndGemMCPoint*)fMCGemPointArray->At(imcp);

    Int_t stationNr = fDigiPar->GetStationNr(mcPoint->GetSensorId());

    nofPointsPerStation[mcPoint->GetTrackID()][stationNr-1] += 1;
  }

  Int_t nofCGM = 0;
  for ( Int_t imct = 0 ; imct < kNofMCTracks ; imct++ ) {
    //    fMCTrackNofCrossedGemStations[imct] = nofCGM;
    nofCGM = 0;
    for ( Int_t is = 0 ; is < kNofGemStations ; is++ )
      if ( nofPointsPerStation[imct][is] > 0 )
  nofCGM++;
    nofFiredStations[imct] = nofCGM;
    PndMCTrack* mcTrack = (PndMCTrack*) fMCTrackArray->At(imct);
    mcTrackMomentum[imct] = mcTrack->GetMomentum();
  }

  vector<Int_t> nofTrSegments(kNofMCTracks,0);
  vector<Int_t> nofTrMCId(kNofMCTracks,0);
  vector<Int_t> segmentMCId(fTrackSegments.size(),-1);

  for ( Int_t itrc = 0 ; itrc < fTrackSegments.size() ; itrc++ ) {
    for ( Int_t itr = 0 ; itr < kNofMCTracks ; itr++ ) nofTrMCId[itr] = 0;
    TrackSegment tempTS = fTrackSegments[itrc];
    cout << tempTS.stationIndex[0] << " " << tempTS.stationIndex[1] << " >> segment " << itrc << ": " << flush;
    for ( Int_t ihit = 0 ; ihit < 4 ; ihit++ ) {
      if ( tempTS.hitIndex[ihit] == -1 ) continue;
      gemHit = (PndGemHit*) fGemHitArray->At(tempTS.hitIndex[ihit]);
      cout << " <" << gemHit->GetX() << "/" << gemHit->GetY() << "> " << flush;
      cout << setw(3) << tempTS.hitIndex[ihit] << "(" << flush;
      if ( gemHit->GetRefIndex() == -1 ) { cout << "--/-) " << flush; continue;}
      mcPoint = (PndGemMCPoint*) fMCGemPointArray->At(gemHit->GetRefIndex());
      cout << setw(2) << gemHit->GetRefIndex() << "/" << mcPoint->GetTrackID() << ") " << flush;

      if ( ihit < 2 && nofTrMCId[mcPoint->GetTrackID()] < 1 )
  nofTrMCId[mcPoint->GetTrackID()] += 1;
      if ( ihit > 1 && nofTrMCId[mcPoint->GetTrackID()] < 2 )
  nofTrMCId[mcPoint->GetTrackID()] += 2;
    }
    cout << setw(11) << tempTS.trackMom << " " << setw(11) << tempTS.trackPhi << " " << setw(11) << tempTS.trackTheta << endl;
    Int_t bestMCId = -1;
    for ( Int_t itr = 0 ; itr < kNofMCTracks ; itr++ ) {
      if ( nofTrMCId[itr] != 3 ) continue;
      if ( bestMCId > -1 ) { bestMCId = -1; break; }
      bestMCId = itr;
    }
    cout << "                                            " << flush;
    if ( bestMCId == -1 ) cout << "            -- NO MATCHING MC -----------------" << endl;
    else {
      cout << "MC " << setw(3) << bestMCId << " TRUTH: "
     << setw(11) << mcTrackMomentum[bestMCId].Mag() << " "
     << setw(11) << TMath::RadToDeg()*mcTrackMomentum[bestMCId].Phi()+(mcTrackMomentum[bestMCId].Phi()>=0.?0:360.) << " "
     << setw(11) << TMath::RadToDeg()*mcTrackMomentum[bestMCId].Theta() << endl;
      nofTrSegments[bestMCId] += 1;
    }
    segmentMCId[itrc] = bestMCId;
  }

  Int_t expNofS = 0;
  Int_t fndNofS = 0;
  for ( Int_t imct = 0 ; imct < kNofMCTracks ; imct++ ) {
    if ( nofTrSegments[imct] == 0 || nofFiredStations[imct] == 0 ) continue;
    cout << " track " << imct << " fired " << nofFiredStations[imct] << " stations, and " << nofTrSegments[imct] << " segments were created:" << endl;

    for ( Int_t itrc = 0 ; itrc < fTrackSegments.size() ; itrc++ ) {
      if ( segmentMCId[itrc] != imct ) continue;
      TrackSegment tempTS = fTrackSegments[itrc];
      cout << " " << itrc << "   " << setw(11) << tempTS.trackMom << " " << setw(11) << tempTS.trackPhi << " " << setw(11) << tempTS.trackTheta << endl;
    }

    if ( mcTrackMomentum[imct].Mag() < 0.5 ) continue;
    Int_t expSegms = 0;
    for ( Int_t is = 0 ; is < nofFiredStations[imct] ; is++ )     for ( Int_t is2 = is+1 ; is2 < nofFiredStations[imct] ; is2++ ) expSegms++;
    expNofS += expSegms;
    fndNofS += nofTrSegments[imct];
  }
  fNofExpectedTrackSegments += expNofS;
  fNofFoundTrackSegments    += fndNofS;

  cout << "********* FOUND " << fndNofS << " OUT OF " << expNofS << " SEGMENTS IN THIS EVENT" << endl;
  cout << "********* FOUND " << fNofFoundTrackSegments << " OUT OF " << fNofExpectedTrackSegments << " SEGMENTS IN ALL EVENTS" << endl;
  */
}
// ------------------------------------------------------------

// --- Private method to print track candidates ---------------
void PndMvdGemTrackFinderOnHits::PrintTracks(Int_t)
{ // nofRecoTracks // [R.K.03/2017] unused variable(s)
  /*
  PndGemHit* gemHit;

  const Int_t kNofStatDbl = 2*fDigiPar->GetNStations();

  for ( Int_t itr = 0 ; itr < nofRecoTracks ; itr++ ) {
    Double_t meanMom = 0.;
    Double_t meanPhi = 0.;
    Double_t meanThe = 0.;
//      meanMom = 0.;
//      meanPhi = 0.;
//      meanThe = 0.;

    vector<Int_t> hitIndices(kNofStatDbl,-1);
    cout << "===================== TRACK " << itr << " ======================" << endl;
    Int_t nofTS = 0;
    for ( Int_t its = 0 ; its < fTrackSegments.size() ; its++ ) {
      TrackSegment iterTS = fTrackSegments[its];
      if ( iterTS.recoTrackIndex != itr ) continue;
      //      for ( Int_t ih = 0 ; ih < 4 ; ih++ ) cout << its << " > " << ih << " > " <<  iterTS.hitIndex[ih] << endl;
      for ( Int_t istat = 0 ; istat < 2 ; istat++ ) {
  hitIndices[2*iterTS.stationIndex[istat]  ] = iterTS.hitIndex[2*istat  ];
  hitIndices[2*iterTS.stationIndex[istat]+1] = iterTS.hitIndex[2*istat+1];
      }
      cout << "segm " << its << " " << iterTS.trackMom << " " << iterTS.trackPhi << " " << iterTS.trackTheta << " "
     << iterTS.stationIndex[0] << ": " << iterTS.hitIndex[0] << " " << iterTS.hitIndex[1] << " / "
     << iterTS.stationIndex[1] << ": " << iterTS.hitIndex[2] << " " << iterTS.hitIndex[3] << endl;
      meanMom += iterTS.trackMom;
      meanPhi += iterTS.trackPhi;
      meanThe += iterTS.trackTheta;
      nofTS++;
    }
    meanMom = meanMom/nofTS;
    meanPhi = meanPhi/nofTS;
    meanThe = meanThe/nofTS;

    if ( nofTS == 0 ) { cout << "THIS TRACK WAS REMOVED " << endl; continue; }

    for ( Int_t ihit = 0 ; ihit < kNofStatDbl ; ihit++ ) {
      cout << ihit << "/" << hitIndices[ihit] << "/" << flush;
      if ( hitIndices[ihit] == -1 ) { cout << "- - " << flush; continue; }
      gemHit = (PndGemHit*) fGemHitArray->At(hitIndices[ihit]);
      if ( gemHit->GetRefIndex() == -1 ) { cout << "- " << flush; continue;}
      cout << setw(2) << gemHit->GetRefIndex() << "_ " << flush;
    }
    cout << endl;
  }
  */
}
// ------------------------------------------------------------

// --- Private method to print track candidates ---------------
void PndMvdGemTrackFinderOnHits::PrintMCTracks(Int_t)
{ // nofRecoTracks // [R.K.03/2017] unused variable(s)
  /*
  PndGemHit* gemHit;
  FairMCPoint* mcPoint;

  const Int_t kNofStatDbl = 2*fDigiPar->GetNStations();

  for ( Int_t itr = 0 ; itr < nofRecoTracks ; itr++ ) {
    vector<Int_t> nofTrMCId(500,0);

    Double_t meanMom = 0.;
    Double_t meanPhi = 0.;
    Double_t meanThe = 0.;
//      meanMom = 0.;
//      meanPhi = 0.;
//      meanThe = 0.;

    vector<Int_t> hitIndices(kNofStatDbl,-1);
    cout << "===================== TRACK " << itr << " ======================" << endl;
    Int_t nofTS = 0;
    for ( Int_t its = 0 ; its < fTrackSegments.size() ; its++ ) {
      TrackSegment iterTS = fTrackSegments[its];
      if ( iterTS.recoTrackIndex != itr ) continue;
      //      for ( Int_t ih = 0 ; ih < 4 ; ih++ ) cout << its << " > " << ih << " > " <<  iterTS.hitIndex[ih] << endl;
      for ( Int_t istat = 0 ; istat < 2 ; istat++ ) {
  hitIndices[2*iterTS.stationIndex[istat]  ] = iterTS.hitIndex[2*istat  ];
  hitIndices[2*iterTS.stationIndex[istat]+1] = iterTS.hitIndex[2*istat+1];
      }
      cout << "segm " << its << " " << iterTS.trackMom << " " << iterTS.trackPhi << " " << iterTS.trackTheta << " "
     << iterTS.stationIndex[0] << ": " << iterTS.hitIndex[0] << " " << iterTS.hitIndex[1] << " / "
     << iterTS.stationIndex[1] << ": " << iterTS.hitIndex[2] << " " << iterTS.hitIndex[3] << endl;
      meanMom += iterTS.trackMom;
      meanPhi += iterTS.trackPhi;
      meanThe += iterTS.trackTheta;
      nofTS++;
    }
    meanMom = meanMom/nofTS;
    meanPhi = meanPhi/nofTS;
    meanThe = meanThe/nofTS;

    if ( nofTS == 0 ) { cout << "THIS TRACK WAS REMOVED " << endl; continue; }

    for ( Int_t ihit = 0 ; ihit < kNofStatDbl ; ihit++ ) {
      cout << ihit << "/" << hitIndices[ihit] << "/" << flush;
      if ( hitIndices[ihit] == -1 ) { cout << "- - " << flush; continue; }
      gemHit = (PndGemHit*) fGemHitArray->At(hitIndices[ihit]);
      if ( gemHit->GetRefIndex() == -1 ) { cout << "- - " << flush; continue;}
      mcPoint = (FairMCPoint*) fMCGemPointArray->At(gemHit->GetRefIndex());
      cout << setw(2) << gemHit->GetRefIndex() << " _" << mcPoint->GetTrackID() << "_ " << flush;
      nofTrMCId[mcPoint->GetTrackID()] += 1;
    }
    cout << endl;
    Int_t bestMCId = -1;
    Int_t largestNofTracks = 0;
    for ( Int_t itm = 0 ; itm < 500 ; itm++ ) {
      if ( nofTrMCId[itm] == largestNofTracks ) { bestMCId = -1; }
      if ( nofTrMCId[itm]  > largestNofTracks ) { bestMCId = itm; largestNofTracks = nofTrMCId[itm]; }
    }
    cout << "                                                        " << flush;
    cout << setw(11) << meanMom << " " << setw(11) << meanPhi << " " << setw(11) << meanThe << endl;
    cout << "                                            " << flush;
    if ( bestMCId == -1 ) cout << "            -- NO MATCHING MC -----------------" << endl;
    else {
      PndMCTrack* mcTrack = (PndMCTrack*) fMCTrackArray->At(bestMCId);
      TVector3 mcmom = mcTrack->GetMomentum();
      cout << "MC " << bestMCId << " TRUTH: "
     << setw(11) << mcmom.Mag() << " "
     << setw(11) << TMath::RadToDeg()*mcmom.Phi()+(mcmom.Phi()>=0.?0:360.) << " "
     << setw(11) << TMath::RadToDeg()*mcmom.Theta() << endl;
    }
  }
  */
}
// ------------------------------------------------------------

// -----   Private method Finish   --------------------------------------------
void PndMvdGemTrackFinderOnHits::Finish()
{
  fTrackArray->Delete();
  fTrackCandArray->Delete();

  cout << "-------------------- " << fName.Data() << " : Summary -----------------------" << endl;
  cout << " Events:        " << setw(10) << fNofEvents << endl;
  cout << "--------------------------------------------------------------------------------" << endl;
}
// ------------------------------------------------------------

ClassImp(PndMvdGemTrackFinderOnHits)
