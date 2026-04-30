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


#include "PndSttTrackFinderIdeal.h"

#include "PndSttHit.h"
#include "PndSttPoint.h"
#include "PndSttHelixHit.h"
#include "PndSttHoughDefines.h"
#include "PndDetectorList.h"
#include "PndTrackCand.h"
#include "PndTrack.h"
#include "PndSttTube.h"

#include "FairMCPoint.h"
#include "FairTrackParP.h"
#include "FairRootManager.h"
#include "FairLogger.h"

// ROOT includes
#include "TClonesArray.h"
#include "TCanvas.h"
#include "TH2F.h"
#include "TArc.h"
#include "TDatabasePDG.h"
#include "TRandom.h"

// C++ includes
#include <iostream>
#include <map>
#include <cmath>

using std::cin;
using std::cout;
using std::endl;
using std::map;

// -----   Default constructor   -------------------------------------------
PndSttTrackFinderIdeal::PndSttTrackFinderIdeal()
{
  rootoutput = kFALSE;

  fMCTrackArray = nullptr;
  fVerbose = 1;
  fHelixHitProduction = kFALSE;
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndSttTrackFinderIdeal::PndSttTrackFinderIdeal(Int_t verbose)
{
  fMCTrackArray = nullptr;
  fVerbose = verbose;
  fHelixHitProduction = kFALSE;

  if (verbose >= 4)
    rootoutput = kTRUE;
  else
    rootoutput = kFALSE; // stt1
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndSttTrackFinderIdeal::~PndSttTrackFinderIdeal() {}
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
void PndSttTrackFinderIdeal::Init()
{
  // Get and check FairRootManager
  FairRootManager *ioman = FairRootManager::Instance();

  if (!ioman) {
    cout << "-E- PndSttTrackFinderIdeal::Init: "
         << "RootManager not instantised!" << endl;
    return;
  }

  // Get MCTrack array
  fMCTrackArray = (TClonesArray *)ioman->GetObject("MCTrack");
  if (!fMCTrackArray) {
    LOG(error) << " PndSttTrackFinderIdeal::Init: No MCTrack array!";
    return;
  }
}

Int_t PndSttTrackFinderIdeal::DoFind(TClonesArray *trackCandArray, TClonesArray *trackArray, TClonesArray *helixHitArray)
{
  // Check pointers
  if (!fMCTrackArray) {
    cout << "-E- PndSttTrackFinderIdeal::DoFind: "
         << "MCTrack array missing! " << endl;
    return -1;
  }

  if (fHitCollectionList.GetEntries() == 0) {
    cout << "-E- PndSttTrackFinderIdeal::DoFind: "
         << "No hit arrays present, call AddHitCollection() first (at least once)! " << endl;
    return -1;
  }

  if (fPointCollectionList.GetEntries() == 0) {
    cout << "-E- PndSttTrackFinderIdeal::DoFind: "
         << "No point arrays present, call AddHitCollection() first (at least once)! " << endl;
    return -1;
  }

  if (!trackCandArray) {
    cout << "-E- PndSttTrackFinderIdeal::DoFind: "
         << "Track cand array missing! " << endl;
    return -1;
  }

  if (!trackArray) {
    cout << "-E- PndSttTrackFinderIdeal::DoFind: "
         << "Track array missing! " << endl;
    return -1;
  }

  TCanvas *finderCanvas;

  if (rootoutput) {
    TH2F myRange("range", "range", 100, -50., 50., 100, -50., 50);

    finderCanvas = new TCanvas("findercanvas", "findercanvas", 800, 600);
    myRange.DrawCopy();
    plotAllStraws();
  }

  // Initialise control counters
  Int_t nNoMCTrack = 0;
  Int_t nNoTrack = 0;
  Int_t nNoSttPoint = 0;
  Int_t nNoSttHit = 0;

  // Create pointers to hit and SttPoint
  PndSttHit *pMhit = nullptr;
  FairMCPoint *pMCpt = nullptr;
  PndMCTrack *pMCtr = nullptr;
  PndTrackCand *pTrckCand = nullptr;
  PndTrack *pTrck = nullptr;

  // Number of STT hits
  Int_t nHits = 0;

  for (Int_t hitListCounter = 0; hitListCounter < fHitCollectionList.GetEntries(); hitListCounter++) {
    nHits += ((TClonesArray *)fHitCollectionList.At(hitListCounter))->GetEntriesFast();
  }

  // Declare some variables outside the loops
  Int_t ptIndex = 0;      // MCPoint index
  Int_t mcTrackIndex = 0; // MCTrack index
  Int_t trackIndex = 0;   // STTTrack index

  // Create STL map from MCtrack index to number of valid SttHits
  map<Int_t, map<Double_t, Int_t>> hitMap;

  // Loop over hits
  for (Int_t iHit = 0; iHit < nHits; iHit++) {
    pMhit = GetHitFromCollections(iHit);

    if (!pMhit)
      continue;

    // tubeID  CHECK added
    Int_t tubeID = pMhit->GetTubeID();
    PndSttTube *tube = (PndSttTube *)fTubeArray->At(tubeID);

    ptIndex = pMhit->GetRefIndex();

    if (rootoutput) {
      TArc myArc;

      myArc.SetFillStyle(0);
      myArc.SetLineColor(1);

      if (pMhit->GetIsochrone() == 0)
        myArc.DrawArc(tube->GetPosition().X(), tube->GetPosition().Y(), 1.0);
      else
        myArc.DrawArc(tube->GetPosition().X(), tube->GetPosition().Y(), pMhit->GetIsochrone());
    }

    if (ptIndex < 0)
      continue; // fake or background hit

    pMCpt = GetPointFromCollections(iHit);

    if (!pMCpt)
      continue;

    mcTrackIndex = pMCpt->GetTrackID();

    Double_t wireX = tube->GetPosition().X(), wireY = tube->GetPosition().Y();
    (hitMap[mcTrackIndex])[wireX * wireX + wireY * wireY]++;
  }

  // Create STL map from MCTrack index to SttTrack index
  map<Int_t, Int_t> correlationMap, trackMap;

  // Create STTTracks for reconstructable MCTracks
  Int_t nMCacc = 0;  // accepted MCTracks (more than 3 points)
  Int_t nTracks = 0; // reconstructable MCTracks
  Int_t nMCTracks = fMCTrackArray->GetEntriesFast();

  for (Int_t iMCTrack = 0; iMCTrack < nMCTracks; iMCTrack++) {
    pMCtr = (PndMCTrack *)fMCTrackArray->At(iMCTrack);
    if (!pMCtr)
      continue;

    if (hitMap[iMCTrack].size() < 3)
      continue;

    // temporary hack, fix this in monte carlo ....
    if (TDatabasePDG::Instance()->GetParticle(pMCtr->GetPdgCode()) == nullptr)
      continue;

    if (!(fabs(TDatabasePDG::Instance()->GetParticle(pMCtr->GetPdgCode())->Charge() / 3.0) > 0.))
      continue;
    nMCacc++;

    new ((*trackCandArray)[nTracks]) PndTrackCand();

    if (fVerbose >= 2)
      LOG(info) << " PndSttTrackFinderIdeal: STTTrack " << nTracks << " created from MCTrack " << iMCTrack << " (" << pMCtr->GetNPoints(PndDetectorId::kSTT) << " STTPoints)";

    correlationMap[nTracks] = iMCTrack;
    trackMap[iMCTrack] = nTracks++;
  }

  // Loop over hits. Get corresponding MCPoint and MCTrack index
  for (Int_t iHit = 0; iHit < nHits; iHit++) {
    pMhit = GetHitFromCollections(iHit);
    if (!pMhit) {
      cout << "-E- PndSttTrackFinderIdeal::DoFind: Empty slot "
           << "in HitArray at position " << iHit << endl;
      nNoSttHit++;
      continue;
    }

    // tubeID  CHECK added
    Int_t tubeID = pMhit->GetTubeID();
    PndSttTube *tube = (PndSttTube *)fTubeArray->At(tubeID);

    if (pMhit->GetIsochrone() == 0.) {
      if (fVerbose == 3)
        cout << "take the center: " << tube->GetPosition().X() << "  " << tube->GetPosition().Y() << "   " << tube->GetPosition().Z() << endl;
    }

    ptIndex = pMhit->GetRefIndex();
    if (ptIndex < 0)
      continue; // fake or background hit

    pMCpt = GetPointFromCollections(iHit);

    if (!pMCpt) {
      nNoSttPoint++;
      continue;
    }

    mcTrackIndex = pMCpt->GetTrackID();

    if (mcTrackIndex < 0 || mcTrackIndex > nMCTracks) {
      cout << "-E- PndSttTrackFinderIdeal::DoFind: "
           << "MCTrack index out of range. " << mcTrackIndex << " " << nMCTracks << endl;
      nNoMCTrack++;
      continue;
    }

    if (trackMap.find(mcTrackIndex) == trackMap.end())
      continue;

    trackIndex = trackMap[mcTrackIndex];
    pTrckCand = (PndTrackCand *)trackCandArray->At(trackIndex);
    if (!pTrckCand) {
      cout << "-E- PndSttTrackFinderIdeal::DoFind: "
           << "No SttTrack pointer. " << iHit << " " << ptIndex << " " << mcTrackIndex << " " << trackIndex << endl;
      nNoTrack++;
      continue;
    }

    // ---------------------------------------------------------------------

    TVector3 MCmom;
    pMCpt->Momentum(MCmom);

    // tubeID  CHECK added
    tubeID = pMhit->GetTubeID();
    tube = (PndSttTube *)fTubeArray->At(tubeID);

    Double_t wireRad = tube->GetPosition().Perp();

    if (MCmom.Mag() < 0.3) {
      // for low momentum particles, hits added to pTrck by R could not be
      // in the correct order due to spiralizing tracks
      // 	cout << "LOW MOMENTUM --> AddHit by HitID " << endl;
      if (pTrckCand->GetNHits() > 25)
        continue;
      // CHECK: test iHit and how to organize sorting (here...)
      //	pTrckCand->AddHit(FairRootManager::Instance()->GetBranchId("STTHit"), iHit, iHit);
      pTrckCand->AddHit(FairRootManager::Instance()->GetBranchId("STTHit"), iHit, iHit);

    } else {
      // 	cout << "HIGH MOMENTUM --> AddHit by R " << endl;
      // CHECK: test iHit and how to organize sorting (... and here)
      //	pTrckCand->AddHit(FairRootManager::Instance()->GetBranchId("STTHit"), iHit, wireRad);
      pTrckCand->AddHit(FairRootManager::Instance()->GetBranchId("STTHit"), iHit, wireRad);
    }

    // ---------------------------------------------------------------------
    if (fHelixHitProduction) {

      // HelixHit Production after PR
      TClonesArray &clref = *helixHitArray;
      Int_t size = clref.GetEntriesFast();

      // CHECK remember to SET the errors on position!!!!!!!
      TVector3 pos;
      pMCpt->Position(pos);
      TVector3 posErr(0, 0, 0);
      PndSttHelixHit *helixhit = new (clref[size])
        PndSttHelixHit(pMhit->GetDetectorID(), pMhit->GetTubeID(), iHit, pMhit->GetRefIndex(), pos, posErr, pMhit->GetIsochrone(), pMhit->GetIsochroneError(), 0.0);
      // dedx
      // calculate it from MC
      double InOut[6];
      memset(InOut, 0, sizeof(InOut));
      InOut[0] = ((PndSttPoint *)pMCpt)->GetXInLocal();
      InOut[1] = ((PndSttPoint *)pMCpt)->GetYInLocal();
      InOut[2] = ((PndSttPoint *)pMCpt)->GetZInLocal();
      InOut[3] = ((PndSttPoint *)pMCpt)->GetXOutLocal();
      InOut[4] = ((PndSttPoint *)pMCpt)->GetYOutLocal();
      InOut[5] = ((PndSttPoint *)pMCpt)->GetZOutLocal();
      TVector3 diff3(InOut[0] - InOut[3], InOut[1] - InOut[4], InOut[2] - InOut[5]);
      double distance = diff3.Mag(); //
      Double_t dedx = 999;
      Double_t eloss = ((PndSttPoint *)pMCpt)->GetEnergyLoss();
      if (distance != 0)
        dedx = eloss / (distance); // in GeV/cm (I guess) CHECK
      helixhit->SetdEdx(dedx);
    }
    // ---------------------------------------------------------------------

    if (rootoutput) {
      TArc myArc;

      myArc.SetFillStyle(0);
      myArc.SetLineColor(trackIndex + 2);

      if (pMhit->GetIsochrone() == 0)
        myArc.DrawArc(pMhit->GetX(), pMhit->GetY(), 1.0);
      else
        myArc.DrawArc(pMhit->GetX(), pMhit->GetY(), pMhit->GetIsochrone());
    }

    if (fVerbose > 2)
      cout << "Hit " << iHit << " from STTPoint " << ptIndex << " (MCTrack " << mcTrackIndex << ") added to STTTrack " << trackIndex << endl;
  }

  for (Int_t trackTeller = 0; trackTeller < nTracks; trackTeller++) {
    // loop over
    pTrckCand = (PndTrackCand *)trackCandArray->At(trackTeller);

    if (pTrckCand != nullptr) {
      pTrckCand->setMcTrackId(correlationMap[trackTeller]);

      Double_t dSeed, rSeed, phiSeed, zSeed, tanLamSeed;

      GetTrack(dSeed, phiSeed, rSeed, zSeed, tanLamSeed, correlationMap[trackTeller]);

      Double_t xSeed = (dSeed + rSeed) * cos(phiSeed), ySeed = (dSeed + rSeed) * sin(phiSeed);

      // Double_t //[R.K. 01/2017] unused variable
      // xSeed_old = xSeed, //[R.K. 01/2017] unused variable?
      // ySeed_old = ySeed, //[R.K. 01/2017] unused variable?
      // rSeed_old = rSeed; //[R.K. 01/2017] unused variable?

      //	  ZoomTrack(dSeed, phiSeed, rSeed, pTrck); // CHECK!!!

      xSeed = (dSeed + rSeed) * cos(phiSeed);
      ySeed = (dSeed + rSeed) * sin(phiSeed);

      if (rootoutput) {
        TArc myArc;

        myArc.SetFillStyle(0);
        // myArc.SetLineColor(trackTeller + 2);
        // myArc.DrawArc(xSeed_old, ySeed_old, rSeed_old);

        myArc.SetLineColor(trackTeller + 2);
        myArc.DrawArc(xSeed, ySeed, rSeed);
      }

      //  ----------------------------------
      // check: seeds directly from MC
      Double_t vxSeed = dSeed * TMath::Cos(phiSeed);
      Double_t vySeed = dSeed * TMath::Sin(phiSeed);
      Double_t vzSeed = zSeed;
      TVector3 posSeed(vxSeed, vySeed, vzSeed); // starting point

      PndMCTrack *mcTrack2 = (PndMCTrack *)fMCTrackArray->At(correlationMap[trackTeller]);

      Int_t pdg = mcTrack2->GetPdgCode();
      TDatabasePDG *fdbPDG = TDatabasePDG::Instance();
      TParticlePDG *fParticle = fdbPDG->GetParticle(pdg);
      Double_t chargeSeed = fParticle->Charge() / 3.;

      TVector3 dirSeed(mcTrack2->GetMomentum().X(), mcTrack2->GetMomentum().Y(),
                       mcTrack2->GetMomentum().Z()); // momentum direction in starting point

      Double_t qop = chargeSeed / dirSeed.Mag(); // q over p
      dirSeed.SetMag(1.);

      // set up PndTrack ***********************************************************************
      TVector3 momSeed(mcTrack2->GetMomentum().X(), mcTrack2->GetMomentum().Y(),
                       mcTrack2->GetMomentum().Z()); // momentum direction in starting point

      // get 1st and last hits
      Int_t hitcounter = pTrckCand->GetNHits();
      PndTrackCandHit candhit = pTrckCand->GetSortedHit(0);
      Int_t iHit = candhit.GetHitId();
      PndSttPoint *firstpnt = (PndSttPoint *)GetPointFromCollections(iHit);
      if (!firstpnt) {
        cout << "PndSttTrackFinderIdeal::DoFind ERROR: 1st pnt " << iHit << endl;
        continue;
      }
      TVector3 MCfirstPos(firstpnt->GetX(), firstpnt->GetY(), firstpnt->GetZ());
      TVector3 MCfirstMom(firstpnt->GetPx(), firstpnt->GetPy(), firstpnt->GetPz());
      PndSttTube *firsttube = (PndSttTube *)fTubeArray->At(firstpnt->GetTubeID());
      TVector3 difirst = MCfirstMom;
      difirst.SetMag(1.);
      TVector3 djfirst = firsttube->GetWireDirection();
      djfirst.SetMag(1.);
      TVector3 dkfirst = difirst.Cross(djfirst);
      dkfirst.SetMag(1.);

      candhit = pTrckCand->GetSortedHit(hitcounter - 1);
      iHit = candhit.GetHitId();
      PndSttPoint *lastpnt = (PndSttPoint *)GetPointFromCollections(iHit);
      if (!lastpnt) {
        cout << "PndSttTrackFinderIdeal::DoFind ERROR last pnt " << iHit << endl;
        continue;
      }
      TVector3 MClastPos(lastpnt->GetX(), lastpnt->GetY(), lastpnt->GetZ());
      TVector3 MClastMom(lastpnt->GetPx(), lastpnt->GetPy(), lastpnt->GetPz());
      PndSttTube *lasttube = (PndSttTube *)fTubeArray->At(lastpnt->GetTubeID());
      TVector3 dilast = MClastMom;
      dilast.SetMag(1.);
      TVector3 djlast = lasttube->GetWireDirection();
      djlast.SetMag(1.);
      TVector3 dklast = dilast.Cross(djlast);
      dklast.SetMag(1.);

      FairTrackParP first(MCfirstPos, MCfirstMom, TVector3(0., 0., 0.), TVector3(0., 0., 0.), ((int)chargeSeed), MCfirstPos, djfirst, dkfirst);

      FairTrackParP last(MClastPos, MClastMom, TVector3(0., 0., 0.), TVector3(0., 0., 0.), ((int)chargeSeed), MClastPos, djlast, dklast);

      // pTrck = new((*trackArray)[trackTeller]) PndTrack(first, last, *pTrckCand, 0, -1., 0, 0, trackTeller, -1);
      pTrck = new ((*trackArray)[trackTeller]) PndTrack(first, last, *pTrckCand);
      pTrck->SetRefIndex("STTTrackCand", trackTeller);
      //	  pTrck->SetLink("STTTrackCand", trackTeller);

      // *****************************************************************************************

      // CHECK to be deleted: tests!
      // position: MY posSeed, PREV dSeed
      if (1 == 0) { // to pilot the print outs
        if (fabs(dSeed - posSeed.Mag()) > 1e-3) {
          cout << "pos" << endl;
          cout << dSeed << " " << posSeed.Mag() << endl;
          posSeed.Print();
        }

        // direction: MY dirSeed, PREV perp to radius <-> (0, 0, 0)
        TVector3 dtest(-TMath::Sin(phiSeed) * 0.006 * rSeed, TMath::Cos(phiSeed) * 0.006 * rSeed, mcTrack2->GetMomentum().Z());
        dtest.SetMag(1.);
        if (fabs(fabs(dtest.X()) - fabs(dirSeed.X())) > 1e-3) {
          cout << "dir x " << endl;
          dirSeed.Print();
          dtest.Print();
        }

        if (fabs(fabs(dtest.Y()) - fabs(dirSeed.Y())) > 1e-3) {
          cout << "dir y " << endl;
          dirSeed.Print();
          dtest.Print();
        }

        if (fabs(fabs(dtest.Z()) - fabs(dirSeed.Z())) > 1e-3) {
          cout << "dir z " << endl;
          dirSeed.Print();
          dtest.Print();
        }

        if (fabs(qop - (chargeSeed / ((0.006 * rSeed) * (0.006 * rSeed) + mcTrack2->GetMomentum().Z() * mcTrack2->GetMomentum().Z()))) > 1e-3)
          cout << "qop " << qop << " " << (chargeSeed / ((0.006 * rSeed) * (0.006 * rSeed) + mcTrack2->GetMomentum().Z() * mcTrack2->GetMomentum().Z())) << " "
               << fabs(qop - (chargeSeed / ((0.006 * rSeed) * (0.006 * rSeed) + mcTrack2->GetMomentum().Z() * mcTrack2->GetMomentum().Z()))) << endl;
      }
    }
  }

  if (rootoutput) {
    finderCanvas->Update();
    finderCanvas->Show();

    char waitchar;

    cout << "press any key to continue." << endl;
    cin >> waitchar;

    delete finderCanvas;
  }

  if (fVerbose == 3) {
    cout << endl;
    cout << "-------------------------------------------------------" << endl;
    LOG(info) << "           Ideal STT track finding               -I-";
    cout << "Hits: " << nHits << endl;
    cout << "MCTracks: total " << nMCTracks << ", accepted " << nMCacc << ", reconstructable: " << nTracks << endl;

    cout << "SttHits not found   : " << nNoSttHit << endl;
    cout << "SttPoints not found : " << nNoSttPoint << endl;
    cout << "MCTracks not found  : " << nNoMCTrack << endl;
    cout << "SttTracks not found : " << nNoTrack << endl;
    cout << "-------------------------------------------------------" << endl;
  } else if (fVerbose)
    LOG(info) << " PndSttTrackFinderIdeal: all " << nMCTracks << ", acc. " << nMCacc << ", rec. " << nTracks;

  return nTracks;
}

void PndSttTrackFinderIdeal::GetTrackletCircular(Double_t firstX, Double_t firstY, Double_t firstR, Double_t secondX, Double_t secondY, Double_t secondR, Double_t thirdX,
                                                 Double_t thirdY, Double_t thirdR, Double_t *circleRadii, Double_t *circleCentersX, Double_t *circleCentersY) const
{
  Int_t trackletCounter = 0;

  Double_t small_limit = 0.0001;

  for (Int_t sign1 = -1; sign1 < 3; sign1 += 2) {
    for (Int_t sign2 = -1; sign2 < 3; sign2 += 2) {
      for (Int_t sign3 = -1; sign3 < 3; sign3 += 2) {
        // if three points are collinear, shift middle point by 50 microns
        if ((firstX - secondX == 0) && (secondX - thirdX == 0)) {
          secondX += 0.005;
          secondY += 0.005;
        } else if (!((fabs(firstX - secondX) < small_limit) || (fabs(secondX - thirdX) < small_limit))) {
          if (fabs((firstY - secondY) / (firstX - secondX) - (secondY - thirdY) / (secondX - thirdX)) < small_limit) {
            secondX += 0.005;
            secondY += 0.005;
          }
        }

        Double_t a = -2. * (firstX - secondX), b = -2. * (firstY - secondY), c = 2. * (sign1 * firstR - sign2 * secondR),
                 d = ((firstX * firstX + firstY * firstY - firstR * firstR) - (secondX * secondX + secondY * secondY - secondR * secondR)), e = -2. * (firstX - thirdX),
                 f = -2. * (firstY - thirdY), g = 2. * (sign1 * firstR - sign3 * thirdR),
                 h = ((firstX * firstX + firstY * firstY - firstR * firstR) - (thirdX * thirdX + thirdY * thirdY - thirdR * thirdR));

        Double_t A = -1. * (f * d - b * h) / (a * f - b * e), B = -1. * (f * c - b * g) / (a * f - b * e), C = (e * d - a * h) / (a * f - e * b),
                 D = (e * c - a * g) / (a * f - e * b),

                 I = B * B + D * D - 1., II = 2. * A * B - 2. * B * firstX + 2. * C * D - 2. * D * firstY + sign1 * 2. * firstR,
                 III = A * A - 2. * A * firstX + firstX * firstX + C * C - 2. * C * firstY + firstY * firstY - firstR * firstR;

        if ((fabs(I) > small_limit) && ((II * II - 4. * I * III) > 0.)) {
          Double_t r = (-1. * II - sqrt(II * II - 4. * I * III)) / (2. * I),

                   x = A + B * r, y = C + D * r;

          circleRadii[trackletCounter] = sqrt(r * r);
          circleCentersX[trackletCounter] = x;
          circleCentersY[trackletCounter] = y;
          trackletCounter++;
        } else {
          circleRadii[trackletCounter] = -1.;
          circleCentersX[trackletCounter] = 0.;
          circleCentersY[trackletCounter] = 0.;
          trackletCounter++;
        }
      }
    }
  }
}

// void PndSttTrackFinderIdeal::ZoomTrack(Double_t &dSeed, Double_t &phiSeed,
// 				       Double_t &rSeed, PndSttTrack *track)
// {
//     track->SortHits();

//     if (track->GetNofHits() < 3)
//     {
// 	cout << "-W- PndSttTrackFinderHough::GetSeed2Circular: less than three hits found, not possible to fit!"
// 	     << endl;
// 	return;
//     }
//     else
//     {
// 	PndSttHoughAccumulatorNew
// 	    *accumulator = new PndSttHoughAccumulatorNew(2, kTRUE,
// 							 150, dSeed - 5., dSeed + 5.,
// 							 150, phiSeed - 0.5, phiSeed + 0.5,
// 							 100, rSeed - 200., rSeed + 200.);

// 	Int_t
// 	  segmentLength1 = track->GetNofHits() / 3,
// 	  segmentLength2 = track->GetNofHits() / 3,
// 	  segmentLength3 = track->GetNofHits() - segmentLength2 - segmentLength1;

// 	Int_t
// 	  segmentStart1 = 0,
// 	  segmentStart2 = segmentLength1,
// 	  segmentStart3 = segmentLength1 + segmentLength2;

// 	// select n random combinations
// 	for (Int_t teller = 0; teller < 100; teller++)
// 	  {
// 	    Int_t
// 	      firstSample = gRandom->Integer(segmentLength1),
// 	      secondSample = gRandom->Integer(segmentLength2),
// 	      thirdSample = gRandom->Integer(segmentLength3);

// 	    Int_t
// 	      i = segmentStart1 + firstSample,
// 	      ii = segmentStart2 + secondSample,
// 	      iii = segmentStart3 + thirdSample;

// 	    PndSttHit
// 	      *iFirstHit = GetHitFromCollections(track->GetHitIndex(i)),
// 	      *iSecondHit = GetHitFromCollections(track->GetHitIndex(ii)),
// 	      *iThirdHit = GetHitFromCollections(track->GetHitIndex(iii));

// 	    if ((!iFirstHit) || (!iSecondHit) ||  (!iThirdHit))
// 	      continue;

// 	    Double_t
// 	      firstX = iFirstHit->GetX(),
// 	      firstY = iFirstHit->GetY(),
// 	      firstR = iFirstHit->GetIsochrone(),
// 	      secondX = iSecondHit->GetX(),
// 	      secondY = iSecondHit->GetY(),
// 	      secondR = iSecondHit->GetIsochrone(),
// 	      thirdX = iThirdHit->GetX(),
// 	      thirdY = iThirdHit->GetY(),
// 	      thirdR = iThirdHit->GetIsochrone();

// 	    Double_t
// 	      circleRadii[8],
// 	      circleCentersX[8],
// 	      circleCentersY[8];

// 	    GetTrackletCircular(firstX, firstY, firstR,
// 				secondX, secondY, secondR,
// 				thirdX, thirdY, thirdR,
// 				circleRadii, circleCentersX, circleCentersY);

// 	    // loop over all tracklets between this and next
// 	    for (Int_t trackletteller = 0; trackletteller < 8; trackletteller++)
// 	      {
// 		if (circleRadii[trackletteller] > 0.)
// 		  {
// 		    Double_t
// 		      phi = atan(circleCentersY[trackletteller] / circleCentersX[trackletteller]),
// 		      dist = sqrt(circleCentersX[trackletteller] * circleCentersX[trackletteller] +
// 				  circleCentersY[trackletteller] * circleCentersY[trackletteller])
// 		      - circleRadii[trackletteller];

// 		    if (circleCentersX[trackletteller] < 0.)
// 		      {
// 			if (circleCentersY[trackletteller] < 0.)
// 			  phi -= dPi;
// 			else
// 			  phi += dPi;
// 		      }

// 		    accumulator->AddHit(dist, i,
// 					phi, ii,
// 					circleRadii[trackletteller], iii);
// 		  }
// 	      }
// 	  }

// 	accumulator->Cluster();

// 	dSeed = accumulator->GetHighestPeakX();
// 	phiSeed = accumulator->GetHighestPeakY();
// 	rSeed = accumulator->GetHighestPeakZ();

// 	delete accumulator;
//     }
// }

void PndSttTrackFinderIdeal::GetTrack(Double_t &dSeed, Double_t &phiSeed, Double_t &rSeed, Double_t &zSeed, Double_t &tanLamSeed, Int_t mcTrackNo)
{
  PndMCTrack *mcTrack = (PndMCTrack *)fMCTrackArray->At(mcTrackNo);

  // TODO: read field from container
  rSeed = sqrt(mcTrack->GetMomentum().X() * mcTrack->GetMomentum().X() + mcTrack->GetMomentum().Y() * mcTrack->GetMomentum().Y()) / 0.006;

  Double_t phiStart = atan(mcTrack->GetMomentum().Y() / mcTrack->GetMomentum().X()), xStart = mcTrack->GetStartVertex().X(), yStart = mcTrack->GetStartVertex().Y();

  if (mcTrack->GetMomentum().X() < 0.) {
    if (mcTrack->GetMomentum().Y() < 0.)
      phiStart -= dPi;
    else
      phiStart += dPi;
  }

  Double_t sign = 1.;

  if (TDatabasePDG::Instance()->GetParticle(mcTrack->GetPdgCode()) != nullptr) {
    if (TDatabasePDG::Instance()->GetParticle(mcTrack->GetPdgCode())->Charge() < 0.) {
      sign = -1.;
    }
  }

  Double_t xCircleCenter = xStart + sign * (rSeed * sin(phiStart)), yCircleCenter = yStart - sign * (rSeed * cos(phiStart));

  dSeed = sqrt(xCircleCenter * xCircleCenter + yCircleCenter * yCircleCenter) - rSeed;
  phiSeed = atan(yCircleCenter / xCircleCenter);

  if (xCircleCenter < 0.) {
    if (yCircleCenter < 0.)
      phiSeed -= dPi;
    else
      phiSeed += dPi;
  }

  // z - tan(lam)
  tanLamSeed = mcTrack->GetMomentum().Z() / sqrt(mcTrack->GetMomentum().X() * mcTrack->GetMomentum().X() + mcTrack->GetMomentum().Y() * mcTrack->GetMomentum().Y());

  zSeed = mcTrack->GetStartVertex().Z();
}

// -------------------------------------------------------------------------
Bool_t PndSttTrackFinderIdeal::putStraw(Double_t xpos, Double_t ypos, Double_t radius)
{
  Double_t pipeDiam = 0.42, tubeOuterDiam = 1.032, CoverThickness = 0.1, outerRadius = 42., innerRadius = 15.;

  if ((sqrt(xpos * xpos + ypos * ypos) < outerRadius - CoverThickness - (tubeOuterDiam / 2.)) &&
      (sqrt(xpos * xpos + ypos * ypos) > innerRadius + CoverThickness + (tubeOuterDiam / 2.)) && (sqrt(ypos * ypos) > (pipeDiam + tubeOuterDiam / 2.))) {
    TArc myArc;

    myArc.SetFillStyle(0);
    myArc.SetLineColor(17);
    myArc.DrawArc(xpos, ypos, radius);
  } else {
    return false;
  }

  return true;
}

void PndSttTrackFinderIdeal::plotAllStraws()
{
  Double_t tubeOuterDiam = 1.032, sttCenterX = 0., sttCenterY = 0.;

  Int_t ringmax = -1;

  Bool_t started = kFALSE, goOn = kTRUE;

  Int_t ringteller = 18;

  while (goOn) {
    goOn = kFALSE;

    Double_t sqrt3 = sqrt(3.), radius = tubeOuterDiam / 2.;

    Double_t zpos = radius;

    // place first
    Double_t xpos = sttCenterX + ((ringteller)*2 * radius), ypos = sttCenterY;

    for (Int_t i = 0; i < ringteller; i++) {
      xpos -= radius;
      ypos += sqrt3 * radius;
      if (putStraw(xpos, ypos, zpos))
        goOn = kTRUE;
    }
    for (Int_t i = 0; i < ringteller; i++) {
      xpos -= 2 * radius;
      if (putStraw(xpos, ypos, zpos))
        goOn = kTRUE;
    }
    for (Int_t i = 0; i < ringteller; i++) {
      xpos -= radius;
      ypos -= sqrt3 * radius;
      if (putStraw(xpos, ypos, zpos))
        goOn = kTRUE;
    }
    for (Int_t i = 0; i < ringteller; i++) {
      xpos += radius;
      ypos -= sqrt3 * radius;
      if (putStraw(xpos, ypos, zpos))
        goOn = kTRUE;
    }
    for (Int_t i = 0; i < ringteller; i++) {
      xpos += 2 * radius;
      if (putStraw(xpos, ypos, zpos))
        goOn = kTRUE;
    }
    for (Int_t i = 0; i < ringteller; i++) {
      xpos += radius;
      ypos += sqrt3 * radius;
      if (putStraw(xpos, ypos, zpos))
        goOn = kTRUE;
    }

    if (goOn)
      started = kTRUE;

    if (!started)
      goOn = kTRUE;

    ringteller++;
    if ((ringmax > 0) && (ringteller == ringmax))
      goOn = kFALSE;
  }
}

PndSttHit *PndSttTrackFinderIdeal::GetHitFromCollections(Int_t hitCounter)
{
  PndSttHit *retval = nullptr;

  Int_t relativeCounter = hitCounter;

  for (Int_t collectionCounter = 0; collectionCounter < fHitCollectionList.GetEntries(); collectionCounter++) {
    Int_t size = ((TClonesArray *)fHitCollectionList.At(collectionCounter))->GetEntriesFast();

    if (relativeCounter < size) {
      retval = (PndSttHit *)((TClonesArray *)fHitCollectionList.At(collectionCounter))->At(relativeCounter);
      break;
    } else {
      relativeCounter -= size;
    }
  }
  return retval;
}

FairMCPoint *PndSttTrackFinderIdeal::GetPointFromCollections(Int_t hitCounter)
{
  FairMCPoint *retval = nullptr;

  Int_t relativeCounter = hitCounter;

  for (Int_t collectionCounter = 0; collectionCounter < fHitCollectionList.GetEntries(); collectionCounter++) {
    Int_t size = ((TClonesArray *)fHitCollectionList.At(collectionCounter))->GetEntriesFast();

    if (relativeCounter < size) {
      Int_t tmpHit = ((PndSttHit *)((TClonesArray *)fHitCollectionList.At(collectionCounter))->At(relativeCounter))->GetRefIndex();

      retval = (FairMCPoint *)((TClonesArray *)fPointCollectionList.At(collectionCounter))->At(tmpHit);

      break;
    } else {
      relativeCounter -= size;
    }
  }
  return retval;
}

ClassImp(PndSttTrackFinderIdeal)
