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
// -----                PndGemTrackFinderOnHits source file             -----
// -----                Created 02.06.2009 by R. Karabowicz            -----
// -------------------------------------------------------------------------

#include "PndGemTrackFinderOnHits.h"

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

// C++ includes
#include <iostream>
#include <iomanip>
#include <map>
#include <cmath>
using std::cout;
using std::endl;
using std::map;

// -----   Default constructor   -------------------------------------------
PndGemTrackFinderOnHits::PndGemTrackFinderOnHits()
{
  fDigiPar = nullptr;

  fSigmaMult = 60.;

  fMCTrackArray = nullptr;
  fMCPointArray = nullptr;
  fNofEvents = 0;
  fNofClHits = 0;

  fVerbose = 0;
  fPrimary = 0;

  // Parameters, taken from the digiPar file
  fParThetaA = 0.;
  fParThetaB = 0.;

  fParTheta0 = 0.;
  fParTheta1 = 0.;
  fParTheta2 = 0.;
  fParTheta3 = 0.;

  fParRadPhi0 = 0.;
  fParRadPhi2 = 0.;

  for (Int_t ipar = 0; ipar < 3; ipar++) {
    fParMat0[ipar] = 0.;
    fParMat1[ipar] = 0.;
  }

  fMCAvailable = -666;

  fNofExpectedTrackSegments = 0;
  fNofFoundTrackSegments = 0;
}

// -----   Destructor   ----------------------------------------------------
PndGemTrackFinderOnHits::~PndGemTrackFinderOnHits() {}

// -----   Init  -----------------------------------------------------------
void PndGemTrackFinderOnHits::Init()
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
       << "PndGemTrackFinderOnHits"
       << "::Init(). There are " << fDigiPar->GetNStations() << " GEM stations." << endl;
  cout << "-I- "
       << "PndGemTrackFinderOnHits"
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
void PndGemTrackFinderOnHits::SetParContainers()
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
Int_t PndGemTrackFinderOnHits::DoFind(TClonesArray *hitArray, TClonesArray *trackArray, TClonesArray *trackCandArray)
{

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
  PndGemHit *gemHit = nullptr;
  PndGemHit *gemHit2 = nullptr;
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

  for (Int_t iHit = 0; iHit < nGemHits; iHit++) {
    // Get the pointer to Gem hit
    gemHit = (PndGemHit *)hitArray->At(iHit);
    if (fVerbose > 1)
      cout << "Hit " << iHit << " -> " << gemHit->GetX() << " " << gemHit->GetY() << " " << gemHit->GetZ() << " -> " << gemHit->GetRefIndex() << endl;
    gemHit->SetNDigiHits(-1);
  }

  map<Int_t, Double_t> hitMatchDist;
  for (Int_t iHit = 0; iHit < nGemHits; iHit++) {
    // Get the pointer to Gem hit
    gemHit = (PndGemHit *)hitArray->At(iHit);
    Double_t x1 = gemHit->GetX();
    Double_t y1 = gemHit->GetY();

    Int_t closestHit = -1;
    Double_t closestDist = 100000.;

    for (Int_t iHit2 = 0; iHit2 < nGemHits; iHit2++) {
      gemHit2 = (PndGemHit *)hitArray->At(iHit2);

      //       if ( TMath::Abs(gemHit->GetZ()-89.4) < 0.2 && TMath::Abs(gemHit2->GetZ()-90.6) < 0.2 )
      // 	cout << "TRYING TO MATCH HITS FROM FIRST STATION: " << iHit << " WITH " << iHit2 << endl;

      if (gemHit2->GetZ() < gemHit->GetZ() + 0.5 || gemHit2->GetZ() > gemHit->GetZ() + 10.) {
        // 	if ( TMath::Abs(gemHit->GetZ()-89.4) < 0.2 && TMath::Abs(gemHit2->GetZ()-90.6) < 0.2 )
        // 	  cout << "FAILED Z " << endl;
        continue;
      }

      Double_t x1p = x1 * gemHit2->GetZ() / gemHit->GetZ();
      Double_t y1p = y1 * gemHit2->GetZ() / gemHit->GetZ();

      Double_t x2 = gemHit2->GetX();
      Double_t y2 = gemHit2->GetY();

      Double_t xer = TMath::Sqrt(gemHit->GetDx() * gemHit->GetDx() + gemHit2->GetDx() * gemHit2->GetDx());
      Double_t yer = TMath::Sqrt(gemHit->GetDy() * gemHit->GetDy() + gemHit2->GetDy() * gemHit2->GetDy());

      if (fVerbose) {
        cout << "X comparing " << x1 << " -> " << x1p << " with " << x2 << " //// error " << xer << " = " << TMath::Abs(x1p - x2) / xer << endl;
        cout << "Y comparing " << y1 << " -> " << y1p << " with " << y2 << " //// error " << yer << " = " << TMath::Abs(y1p - y2) / yer << endl;
      }

      //        1 full phi / 4 full phi  / 1 sel phi  / 2 sel phi   / 4 sel phi   / p=.4 / p=.6 / p=1. / p=2. / p=5.
      // 60. -> 931 tracks / 3099 tracks / 980 tracks / 1455 tracks / 2228 tracks /  /  /  /  /
      // 36. -> 931 tracks / 3099 tracks / 980 tracks / 1455 tracks / 2228 tracks /    2 /  116 /  993 / 1031 / 1042
      // 33. -> 931 tracks / 3099 tracks / 980 tracks / 1455 tracks / 2228 tracks /    2 /  116 /  993 / 1031 / 1042
      // 30. -> 930 tracks / 3092 tracks / 978 tracks / 1458 tracks / 2213 tracks /    2 /  111 /  993 / 1031 / 1042
      // 27. -> 926 tracks / 3072 tracks / 973 tracks / 1449 tracks / 2179 tracks /    2 /  105 /  990 / 1030 / 1041
      // 24. -> 920 tracks / 3038 tracks / 967 tracks / 1438 tracks / 2112 tracks /    2 /   46 /  952 / 1030 / 1040
      // 21. -> 906 tracks / 2992 tracks / 957 tracks / 1413 tracks / 2053 tracks /    2 /   26 /  561 / 1028 / 1038
      // 18. -> 894 tracks / 2927 tracks / 938 tracks / 1381 tracks / 1969 tracks /    2 /   21 /  167 / 1025 / 1033
      // 15. -> 867 tracks / 2820 tracks / 916 tracks / 1331 tracks / 1850 tracks /    2 /   10 /  143 / 1020 / 1022
      // 12. -> 828 tracks / 2699 tracks / 879 tracks / 1260 tracks / 1735 tracks /    2 /    6 /   32 / 1016 / 1014
      //  9. -> 773 tracks / 2482 tracks / 804 tracks / 1148 tracks / 1538 tracks /    2 /    2 /   13 /  331 /  997
      //  6. -> 668 tracks / 2110 tracks / 688 tracks /  947 tracks / 1202 tracks /    0 /    0 /    0 /    1 /  985
      //  3. -> 397 tracks / 1275 tracks / 339 tracks /  474 tracks /  510 tracks /    0 /    0 /    0 /    0 /   34

      //     .4   .6   1.    2.    5.   1ful  4ful 1sel  2sel  4sel
      /*
      100    666  990  1003  1040  1048  944  3276  984  1584  2853
       90    666  990  1003  1040  1048  944  3255  984  1574  2788
       80    666  990  1002  1040  1048  944  3235  983  1557  2708
       70    666  990  1002  1040  1048  944  3231  983  1541  2661
       60    666  989  1002  1040  1047  945  3203  983  1531  2569
       50    666  988  1002  1040  1048  941  3171  983  1510  2475
       45    657  987  1001  1040  1047  938  3158  982  1501  2422
       40    323  985  1001  1040  1047  931  3151  983  1489  2358
       36    130  667  1000  1040  1045  931  3141  985  1482  2333
       33     54  200  1000  1040  1044  934  3135  983  1477  2307
       30     49  136  1000  1040  1043  935  3121  983  1471  2269
       27     48  127   998  1039  1042  930  3101  979  1458  2218
       24     46   61   985  1039  1041  922  3067  973  1443  2165
       21     40   38   660  1037  1037  901  3017  965  1428  2094
       18     36   32   247  1033  1033  887  2955  947  1397  2015
       15     29   22   216  1028  1021  864  2840  919  1339  1883
       12     23   18    51  1023  1013  829  2708  881  1263  1750
       10     11    7    28   763  1004  797  2577  839  1193  1618
        9     11    6    22   336   998  776  2492  805  1152  1534
        6      1    0     2     1   985  669  2114  688   950  1209
        4      0    0     0     0   537  523  1639  509   684   815
        3      0    0     0     0    34  397  1276  339   474   510
      */

      if (TMath::Abs(x1p - x2) > fSigmaMult * xer) { // changed 3.* to 6.*, but would be happier with r/phi cut
                                                     // 	if ( TMath::Abs(gemHit->GetZ()-89.4) < 0.2 && TMath::Abs(gemHit2->GetZ()-90.6) < 0.2 )
                                                     // 	  cout << "FAILED X with xer " << xer << " yer " << yer << " /// x1-x2 = " << TMath::Abs(x1p-x2) << endl;
        if (fVerbose)
          cout << "failed x" << endl;
        continue;
      }
      if (TMath::Abs(y1p - y2) > fSigmaMult * yer) { // changed 3.* to 6.*, but would be happier with r/phi cut
        // 	if ( TMath::Abs(gemHit->GetZ()-89.4) < 0.2 && TMath::Abs(gemHit2->GetZ()-90.6) < 0.2 )
        // 	  cout << "FAILED Y with xer " << xer << " yer " << yer << " /// y1-y2 = " << TMath::Abs(y1p-y2) << endl;
        if (fVerbose)
          cout << "failed y" << endl;
        continue;
      }

      Double_t distSq = (x1p - x2) * (x1p - x2) / xer / xer + (y1p - y2) * (y1p - y2) / yer / yer;
      if (fVerbose)
        cout << "distsq = " << distSq << endl;
      if (closestDist > distSq) {
        closestDist = distSq;
        closestHit = iHit2;
      }
      if (closestHit != -1) {
        gemHit2 = (PndGemHit *)hitArray->At(closestHit);
        if (gemHit2->GetNDigiHits() != -1) {          // it means we already used this hit... comparing distances
          if (closestDist > hitMatchDist[closestHit]) // the new hit is more away than the previous, skipping
            continue;
          // now we have the hit that is closer, so remove the information about matching
          if (fVerbose > 5)
            cout << "  Remove matching of hits " << gemHit2->GetNDigiHits() << " and " << closestHit << endl;
          PndGemHit *gemHitR = (PndGemHit *)hitArray->At(gemHit2->GetNDigiHits());
          gemHitR->SetNDigiHits(-1);
        }
        gemHit->SetNDigiHits(closestHit);
        gemHit2->SetNDigiHits(iHit);

        hitMatchDist[closestHit] = closestDist;
        if (fVerbose > 4)
          cout << "matching hits " << iHit << " and " << iHit2 << endl;
      }
    }
    if (closestHit != -1)
      fNofClHits++;
  }

  for (Int_t istat1 = 0; istat1 < fDigiPar->GetNStations(); istat1++) {
    for (Int_t istat2 = istat1 + 1; istat2 < fDigiPar->GetNStations(); istat2++) {
      FindTrackSegments(hitArray, istat1, istat2);
    }
  }

  if (fVerbose) {
    cout << "event " << fNofEvents << " >>> " << fTrackSegments.size() << " track segments. " << endl;
    if (fMCAvailable)
      PrintMCTrackSegments(hitArray);
    else
      PrintTrackSegments(hitArray);
  }

  Int_t nr = MatchTrackSegments();

  RemoveCloneTracks(nr);

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

  if (fVerbose) {
    cout << "------------------------------------------------" << endl;
    cout << "!!!!!!!!!!!!!!!!!! " << nr << " tracks have been found" << endl;
    cout << "------------------------------------------------" << endl;
  }

  //  cout << "co za pojebanstwo, na koniec mam " << fNofClHits << " closeHitow" << endl;

  return nr;
}
// ------------------------------------------------------------

// --- Private method to create tracks ------------------------
Int_t PndGemTrackFinderOnHits::CreateTracks(TClonesArray *hitArray, TClonesArray *trackArray, TClonesArray *trackCandArray, Int_t nofRecoTracks)
{
  Int_t nofCreatedTracks = 0;

  const Int_t kNofRecoTracks = nofRecoTracks;
  const Int_t kNofStatDbl = 2 * fDigiPar->GetNStations();

  Int_t hitIndices[kNofRecoTracks][kNofStatDbl];
  Int_t nofHits[kNofRecoTracks];

  Double_t meanMom[kNofRecoTracks];
  Double_t meanPhi[kNofRecoTracks];
  Double_t meanThe[kNofRecoTracks];
  Int_t nofTS[kNofRecoTracks];

  PndTrackCandHit tcHit;
  PndGemHit *gemHit;

  for (Int_t itr = 0; itr < nofRecoTracks; itr++) {
    for (Int_t ih = 0; ih < kNofStatDbl; ih++)
      hitIndices[itr][ih] = -1;
    nofHits[itr] = 0;
    meanMom[itr] = 0.;
    meanPhi[itr] = 0.;
    meanThe[itr] = 0.;
    nofTS[itr] = 0;

    for (size_t its = 0; its < fTrackSegments.size(); its++) {
      TrackSegment iterTS = fTrackSegments[its];
      if (iterTS.recoTrackIndex != itr)
        continue;
      for (Int_t istat = 0; istat < 2; istat++) {
        hitIndices[itr][2 * iterTS.stationIndex[istat]] = iterTS.hitIndex[2 * istat];
        hitIndices[itr][2 * iterTS.stationIndex[istat] + 1] = iterTS.hitIndex[2 * istat + 1];
        nofHits[itr] += 2;
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

    PndTrackCand *gemTrackCand = new ((*trackCandArray)[nofCreatedTracks]) PndTrackCand();

    for (Int_t ih = 0; ih < kNofStatDbl; ih++) {
      if (hitIndices[itr][ih] == -1)
        continue;
      gemHit = (PndGemHit *)hitArray->At(hitIndices[itr][ih]);
      gemTrackCand->AddHit(FairRootManager::Instance()->GetBranchId("GEMHit"), hitIndices[itr][ih], gemHit->GetPosition().Mag());
    }

    gemTrackCand->Sort();

    TVector3 pos(0., 0., 0.);
    TVector3 mom;
    mom.SetMagThetaPhi(TMath::Abs(meanMom[itr]), meanThe[itr] * TMath::DegToRad(), meanPhi[itr] * TMath::DegToRad());

    Int_t charge = -1;
    if (mom.Mag() < 0.)
      charge = 1;

    FairTrackParP firstPar(pos, mom, TVector3(0.5, 0.5, 0.5), 0.1 * mom, charge, pos, TVector3(1., 0., 0.), TVector3(0., 1., 0.));
    FairTrackParP lastPar(pos, mom, TVector3(0.5, 0.5, 0.5), 0.1 * mom, charge, pos, TVector3(1., 0., 0.), TVector3(0., 1., 0.));
    new ((*trackArray)[nofCreatedTracks]) PndTrack(firstPar, lastPar, *gemTrackCand);

    nofCreatedTracks++;
  }

  return nofCreatedTracks;
}
// ------------------------------------------------------------

// --- Private method to remove clone track -------------------
void PndGemTrackFinderOnHits::RemoveCloneTracks(Int_t nofRecoTracks)
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
  const Int_t kNofComb = nofComb;

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
      TrackSegment iterTS = fTrackSegments[its];
      if (iterTS.recoTrackIndex != itr)
        continue;
      for (Int_t istat = 0; istat < 2; istat++) {
        hitIndices[itr][2 * iterTS.stationIndex[istat]] = iterTS.hitIndex[2 * istat];
        hitIndices[itr][2 * iterTS.stationIndex[istat] + 1] = iterTS.hitIndex[2 * istat + 1];
        nofHits[itr] += 2;
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
    if (nofMultiHits[itr] >= nofHits[itr] / 2)
      cloneIndicators++; // too many hits shared with other tracks

    if (cloneIndicators >= 2) { // remove this track candidate
      for (size_t its = 0; its < fTrackSegments.size(); its++) {
        TrackSegment iterTS = fTrackSegments[its];
        if (iterTS.recoTrackIndex != itr)
          continue;
        iterTS.recoTrackIndex = -1;
        fTrackSegments[its] = iterTS;
      }
    }
    if (fVerbose > 4 || printInfo) {
      cout << " consistency = " << trackCons[itr] << (trackCons[itr] > 1 ? " YES" : "") << endl;
      cout << " segments    = " << nofTS[itr] << (nofTS[itr] <= kNofComb * 2 / 3 ? " YES" : "") << endl;
      cout << " multihits   = " << nofMultiHits[itr] << (nofMultiHits[itr] >= nofHits[itr] / 2 ? " YES" : "") << endl;
      cout << "TRACK " << itr << " HAS " << nofMultiHits[itr] << " MULTI HITS AND CONSISTENCY = " << trackCons[itr] << (cloneIndicators >= 2 ? " >>> REMOVED!!!! " : "") << endl;
    }
  }
}
// ------------------------------------------------------------

// --- Private method to match track segments -----------------
Int_t PndGemTrackFinderOnHits::MatchTrackSegments()
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

    Bool_t overrepr = kFALSE;
    for (size_t itrc2 = 0; itrc2 < fTrackSegments.size(); itrc2++) {
      if (itrc2 == itrc)
        continue; // do not match segment with itself
      TrackSegment matchTS = fTrackSegments[itrc2];
      if (origTS.stationIndex[0] == matchTS.stationIndex[0] && origTS.stationIndex[1] == matchTS.stationIndex[1])
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
        if (!matching) {
          if (matchTS.hitIndex[0] == iterTS.hitIndex[2] && matchTS.hitIndex[1] == iterTS.hitIndex[3]) {
            matching = kTRUE;
          } // break; } // matchTS matches after iterTS
          if (matchTS.hitIndex[2] == iterTS.hitIndex[0] && matchTS.hitIndex[3] == iterTS.hitIndex[1]) {
            matching = kTRUE;
          } // break; } // matchTS matches before iterTS
          if (matchTS.hitIndex[0] == iterTS.hitIndex[0] && matchTS.hitIndex[1] == iterTS.hitIndex[1]) {
            matching = kTRUE;
          } // break; } // matchTS shares hit on first station with iterTS
          if (matchTS.hitIndex[2] == iterTS.hitIndex[2] && matchTS.hitIndex[3] == iterTS.hitIndex[3]) {
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
        TrackSegment iterTS = fTrackSegments[trackSegs[its]];
        for (Int_t istat1 = 0; istat1 < 2; istat1++) {
          for (Int_t istat2 = 0; istat2 < 2; istat2++) {
            if (matchTS.stationIndex[istat1] == iterTS.stationIndex[istat2])
              if (matchTS.hitIndex[istat1 * 2] != iterTS.hitIndex[istat2 * 2] || matchTS.hitIndex[istat1 * 2 + 1] != iterTS.hitIndex[istat2 * 2 + 1]) {
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
      TrackSegment iterTS = fTrackSegments[trackSegs[its]];
      if (fVerbose > 4 || printInfo) {
        cout << iterTS.stationIndex[0] << " " << iterTS.stationIndex[1] << " > " << flush;
        for (Int_t ihit = 0; ihit < 4; ihit++)
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
Int_t PndGemTrackFinderOnHits::FindTrackSegments(TClonesArray *hitArray, Int_t stat1Id, Int_t stat2Id)
{
  Bool_t printInfo = kFALSE; // TRUE;

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

  Int_t nGemHits = hitArray->GetEntriesFast();

  PndGemHit *gemHit;
  PndGemHit *gemHit2;

  for (Int_t iHit = 0; iHit < nGemHits; iHit++) {
    // Get the pointer to Gem hit
    gemHit = (PndGemHit *)hitArray->At(iHit);
    if (fVerbose > 3 || printInfo)
      cout << "LOOKING FOR TRACK SEGMENTS STARTING AT " << gemHit->GetX() << " " << gemHit->GetY() << " " << gemHit->GetZ() << "( " << zStation1
           << ") ndigihits = " << gemHit->GetNDigiHits() << endl;
    if (gemHit->GetZ() > zStation1)
      continue;
    if (gemHit->GetNDigiHits() < 0)
      continue;
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
      gemHit2 = (PndGemHit *)hitArray->At(iHit2);
      if (fVerbose > 3 || printInfo)
        cout << "gemhit2.z = " << gemHit2->GetZ() << " /// gemhit1.z = " << gemHit->GetZ() << " /// station2z = " << zStation2 << endl;
      if (TMath::Abs(gemHit2->GetZ() - (zStation2 - 1.)) > 0.4) { // where the second hit is..., but i think does not have to check this
        if (fVerbose > 3 || printInfo)
          cout << "not good Z" << endl;
        continue;
      }
      if (gemHit2->GetNDigiHits() < 0)
        continue;
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
      tempTS.stationIndex[0] = stat1Id;
      tempTS.stationIndex[1] = stat2Id;
      tempTS.hitIndex[0] = iHit;
      tempTS.hitIndex[1] = gemHit->GetNDigiHits();
      tempTS.hitIndex[2] = iHit2;
      tempTS.hitIndex[3] = gemHit2->GetNDigiHits();
      tempTS.trackMom = trackMomentum;
      tempTS.trackPhi = trackPhiAngle * TMath::RadToDeg();
      tempTS.trackTheta = theta;
      tempTS.recoTrackIndex = -1;

      if (fVerbose > 2 || printInfo) {
        cout << " found segment (stat. " << stat1Id << " & " << stat2Id << "), hits " << iHit << ", " << gemHit->GetNDigiHits() << ", " << iHit2 << ", " << gemHit2->GetNDigiHits()
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
void PndGemTrackFinderOnHits::PrintTrackSegments(TClonesArray *hitArray)
{
  PndGemHit *gemHit;

  // const Int_t kNofGemStations = fDigiPar->GetNStations(); //[R.K. 01/2017] unused variable?

  for (size_t itrc = 0; itrc < fTrackSegments.size(); itrc++) {
    TrackSegment tempTS = fTrackSegments[itrc];
    cout << tempTS.stationIndex[0] << " " << tempTS.stationIndex[1] << " >> segment " << itrc << ": " << flush;
    for (Int_t ihit = 0; ihit < 4; ihit++) {
      if (tempTS.hitIndex[ihit] == -1)
        continue;
      gemHit = (PndGemHit *)hitArray->At(tempTS.hitIndex[ihit]);
      cout << " <" << gemHit->GetX() << "/" << gemHit->GetY() << "> " << flush;
      cout << setw(3) << tempTS.hitIndex[ihit] << "(" << flush;
      if (gemHit->GetRefIndex() == -1) {
        cout << "--/-) " << flush;
        continue;
      }
      cout << setw(2) << gemHit->GetRefIndex() << ") " << flush;
    }
    cout << setw(11) << tempTS.trackMom << " " << setw(11) << tempTS.trackPhi << " " << setw(11) << tempTS.trackTheta << endl;
  }
}
// ------------------------------------------------------------

// --- Private method to print track segments -----------------
void PndGemTrackFinderOnHits::PrintMCTrackSegments(TClonesArray *hitArray)
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
    TrackSegment tempTS = fTrackSegments[itrc];
    cout << tempTS.stationIndex[0] << " " << tempTS.stationIndex[1] << " >> segment " << itrc << ": " << flush;
    for (Int_t ihit = 0; ihit < 4; ihit++) {
      if (tempTS.hitIndex[ihit] == -1)
        continue;
      gemHit = (PndGemHit *)hitArray->At(tempTS.hitIndex[ihit]);
      cout << " <" << gemHit->GetX() << "/" << gemHit->GetY() << "> " << flush;
      cout << setw(3) << tempTS.hitIndex[ihit] << "(" << flush;
      if (gemHit->GetRefIndex() == -1) {
        cout << "--/-) " << flush;
        continue;
      }
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
      TrackSegment tempTS = fTrackSegments[itrc];
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
void PndGemTrackFinderOnHits::PrintTracks(TClonesArray *hitArray, Int_t nofRecoTracks)
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
      TrackSegment iterTS = fTrackSegments[its];
      if (iterTS.recoTrackIndex != itr)
        continue;
      //      for ( Int_t ih = 0 ; ih < 4 ; ih++ ) cout << its << " > " << ih << " > " <<  iterTS.hitIndex[ih] << endl;
      for (Int_t istat = 0; istat < 2; istat++) {
        hitIndices[2 * iterTS.stationIndex[istat]] = iterTS.hitIndex[2 * istat];
        hitIndices[2 * iterTS.stationIndex[istat] + 1] = iterTS.hitIndex[2 * istat + 1];
      }
      cout << "segm " << its << " " << iterTS.trackMom << " " << iterTS.trackPhi << " " << iterTS.trackTheta << " " << iterTS.stationIndex[0] << ": " << iterTS.hitIndex[0] << " "
           << iterTS.hitIndex[1] << " / " << iterTS.stationIndex[1] << ": " << iterTS.hitIndex[2] << " " << iterTS.hitIndex[3] << endl;
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
void PndGemTrackFinderOnHits::PrintMCTracks(TClonesArray *hitArray, Int_t nofRecoTracks)
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
      TrackSegment iterTS = fTrackSegments[its];
      if (iterTS.recoTrackIndex != itr)
        continue;
      //      for ( Int_t ih = 0 ; ih < 4 ; ih++ ) cout << its << " > " << ih << " > " <<  iterTS.hitIndex[ih] << endl;
      for (Int_t istat = 0; istat < 2; istat++) {
        hitIndices[2 * iterTS.stationIndex[istat]] = iterTS.hitIndex[2 * istat];
        hitIndices[2 * iterTS.stationIndex[istat] + 1] = iterTS.hitIndex[2 * istat + 1];
      }
      cout << "segm " << its << " " << iterTS.trackMom << " " << iterTS.trackPhi << " " << iterTS.trackTheta << " " << iterTS.stationIndex[0] << ": " << iterTS.hitIndex[0] << " "
           << iterTS.hitIndex[1] << " / " << iterTS.stationIndex[1] << ": " << iterTS.hitIndex[2] << " " << iterTS.hitIndex[3] << endl;
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

ClassImp(PndGemTrackFinderOnHits)
