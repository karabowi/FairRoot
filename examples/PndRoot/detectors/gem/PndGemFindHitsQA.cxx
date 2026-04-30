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
// -----                PndGemFindHitsQA source file             -----
// -----                Created 02.06.2009 by R. Karabowicz            -----
// -------------------------------------------------------------------------

#include "PndGemFindHitsQA.h"

// FairRoot includes
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairBaseParSet.h"
#include "FairTrackParam.h"
#include "FairRootManager.h"
#include "FairLogger.h"
// Pnd includes
#include "PndGemMCPoint.h"
#include "PndGemDigi.h"
#include "PndGemDigiPar.h"
#include "PndTrack.h"
#include "PndTrackCand.h"
#include "PndTrackCandHit.h"
#include "PndDetectorList.h"
// ROOT includes
#include "TClonesArray.h"
#include "TGeoManager.h"
#include "TMatrixFSym.h"
#include "TObjString.h"

// C++ includes
#include <iostream>
#include <iomanip>
#include <map>
#include <cmath>
using std::cout;
using std::endl;
using std::map;
using std::pair;

// -----   Default constructor   ------------------------------------------
PndGemFindHitsQA::PndGemFindHitsQA()
  : FairTask("GEM Find Hits QA", 1), fDigiPar(nullptr), fMCPointArray(nullptr), fGemHitArray(nullptr), fNofEvents(0), fHistoList(nullptr), fHistPlaneDivs(0.0), fPointEffDist(0.1),
    fhTrueMatchDiXY(nullptr), fhTrueMatchDist(nullptr), fhTrueMatchValue(nullptr), fhTrueMatchDistValue(nullptr)

    ,
    fhTrueMatchNofPerHit(nullptr), fhTrueMatchNofPerPoint(nullptr)
{
  for (Int_t istat = 0; istat < 4; istat++) {
    for (Int_t isens = 0; isens < 2; isens++) {
      fHistWidth[istat][isens] = 0.;
      for (Int_t iregx = 0; iregx < 4; iregx++) {
        for (Int_t iregy = 0; iregy < 4; iregy++) {
          fhPointToHit[istat][isens][iregx][iregy] = nullptr;
        }
      }
      //      fhPointClosest     [istat][isens] = nullptr;
      fhPointNof[istat][isens] = nullptr;
      fhPointReco[istat][isens] = nullptr;
      fhPointRecoEff[istat][isens] = nullptr;
      fhPointRadNof[istat][isens] = nullptr;
      fhPointRadReco[istat][isens] = nullptr;
      fhPointRadRecoEff[istat][isens] = nullptr;

      fhPointMatch[istat][isens] = nullptr;
      fhPointMatchEff[istat][isens] = nullptr;
      fhPointRadMatch[istat][isens] = nullptr;
      fhPointRadMatchEff[istat][isens] = nullptr;

      fhHitNof[istat][isens] = nullptr;
      fhHitFake[istat][isens] = nullptr;
      fhHitFakeProb[istat][isens] = nullptr;
      fhHitRadNof[istat][isens] = nullptr;
      fhHitRadFake[istat][isens] = nullptr;
      fhHitRadFakeProb[istat][isens] = nullptr;
      fhHitMultipleRate[istat][isens] = nullptr;
      fhCloseHits[istat][isens] = nullptr;
      fhTrueMatchDiXYPerSt[istat][isens] = nullptr;
      fhTrueMatchDistPerSt[istat][isens] = nullptr;
    }
  }
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndGemFindHitsQA::PndGemFindHitsQA(Int_t iVerbose)
  : FairTask("GEM Find Hits QA", iVerbose), fDigiPar(nullptr), fMCPointArray(nullptr), fGemHitArray(nullptr), fNofEvents(0), fHistoList(nullptr), fHistPlaneDivs(0.0),
    fPointEffDist(0.1), fhTrueMatchDiXY(nullptr), fhTrueMatchDist(nullptr), fhTrueMatchValue(nullptr), fhTrueMatchDistValue(nullptr)

    ,
    fhTrueMatchNofPerHit(nullptr), fhTrueMatchNofPerPoint(nullptr)
{
  for (Int_t istat = 0; istat < 4; istat++) {
    for (Int_t isens = 0; isens < 2; isens++) {
      fHistWidth[istat][isens] = 0.;
      for (Int_t iregx = 0; iregx < 4; iregx++) {
        for (Int_t iregy = 0; iregy < 4; iregy++) {
          fhPointToHit[istat][isens][iregx][iregy] = nullptr;
        }
      }
      //      fhPointClosest     [istat][isens] = nullptr;
      fhPointNof[istat][isens] = nullptr;
      fhPointReco[istat][isens] = nullptr;
      fhPointRecoEff[istat][isens] = nullptr;
      fhPointRadNof[istat][isens] = nullptr;
      fhPointRadReco[istat][isens] = nullptr;
      fhPointRadRecoEff[istat][isens] = nullptr;

      fhPointMatch[istat][isens] = nullptr;
      fhPointMatchEff[istat][isens] = nullptr;
      fhPointRadMatch[istat][isens] = nullptr;
      fhPointRadMatchEff[istat][isens] = nullptr;

      fhHitNof[istat][isens] = nullptr;
      fhHitFake[istat][isens] = nullptr;
      fhHitFakeProb[istat][isens] = nullptr;
      fhHitRadNof[istat][isens] = nullptr;
      fhHitRadFake[istat][isens] = nullptr;
      fhHitRadFakeProb[istat][isens] = nullptr;
      fhHitMultipleRate[istat][isens] = nullptr;
      fhCloseHits[istat][isens] = nullptr;
      fhTrueMatchDiXYPerSt[istat][isens] = nullptr;
      fhTrueMatchDistPerSt[istat][isens] = nullptr;
    }
  }
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndGemFindHitsQA::~PndGemFindHitsQA() {}

// -----   Init  -----------------------------------------------------------
InitStatus PndGemFindHitsQA::Init()
{

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

  // Get PndGemPoint (MCPoint) array
  fMCPointArray = (TClonesArray *)ioman->GetObject("GEMPoint");
  if (!fMCPointArray) {
    LOG(error) << " " << GetName() << "::Init: No MCPoint array!";
    return kERROR;
  }

  // Get Gem hit Array
  fGemHitArray = (TClonesArray *)ioman->GetObject("GEMHit");
  if (!fGemHitArray) {
    LOG(error) << " " << GetName() << "::Init: No PndGemHit array!";
    return kERROR;
  }

  // Get GEM digitisation parameter container
  fDigiPar = (PndGemDigiPar *)(rtdb->getContainer("PndGemDetectors"));

  LOG(info) << " " << fName.Data() << "::Init(). There are " << fDigiPar->GetNStations() << " GEM stations.";
  LOG(info) << " " << fName.Data() << "::Init(). Initialization succesfull.";

  TList *branchNames = FairRootManager::Instance()->GetBranchNameList();

  Int_t nofGemBranches = 0;
  fGemPointNumber = -1;
  for (int i = 0; i < branchNames->GetEntries(); i++) {
    TObjString *branchName = (TObjString *)branchNames->At(i);
    if (!branchName->GetString().BeginsWith("GEM"))
      continue;
    if (branchName->GetString() == "GEMPoint")
      fGemPointNumber = i;
    fGemData[nofGemBranches] = (TClonesArray *)ioman->GetObject(branchName->GetString().Data());
    fGemDataPointer[i] = nofGemBranches;
    cout << "GEM Data [" << nofGemBranches << "] = " << branchName->GetString().Data() << " referred to as " << i << (fGemPointNumber == i ? " ***" : "") << endl;
    nofGemBranches++;
  }

  CreateHistos();

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Private method ReInit   -----------------------------------------
InitStatus PndGemFindHitsQA::ReInit()
{

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Private method SetParContainers   -------------------------------
void PndGemFindHitsQA::SetParContainers()
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

  fNofEvents = 0;
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndGemFindHitsQA::Exec(Option_t *)
{

  Bool_t printInfo = kFALSE;

  fNofEvents++;

  Int_t nofGemHits = fGemHitArray->GetEntries();
  Int_t nofGemPnts = fMCPointArray->GetEntries();

  PndGemHit *gemHit = nullptr;
  PndGemMCPoint *gemPnt = nullptr;

  //  cout << "---------------------------------" << endl;
  //  cout << "gem " << fHistWidth[istat][isens] << " wi
  if (printInfo) {
    cout << "GEM Point Array has " << nofGemPnts << " entries" << endl;
    cout << "GEM   Hit Array has " << nofGemHits << " entries" << endl;
  }

  for (Int_t ipnt = 0; ipnt < nofGemPnts; ipnt++) {
    gemPnt = (PndGemMCPoint *)fMCPointArray->At(ipnt);
    Int_t sensorId = gemPnt->GetSensorId();
    Int_t station = fDigiPar->GetStationNr(sensorId) - 1;
    Int_t sensor = fDigiPar->GetSensorNr(sensorId) - 1;
    Double_t pntX = (gemPnt->GetX() + gemPnt->GetXOut()) / 2.;
    Double_t pntY = (gemPnt->GetY() + gemPnt->GetYOut()) / 2.;
    Int_t regX = fHistPlaneDivs + TMath::Floor(pntX / fHistWidth[station][sensor]);
    Int_t regY = fHistPlaneDivs + TMath::Floor(pntY / fHistWidth[station][sensor]);

    if (printInfo) {
      cout << "GEM Point at " << gemPnt->GetX() << " , " << gemPnt->GetY() << " , " << gemPnt->GetZ() << endl;
      cout << "----> Will go to [ " << station << " ] [ " << sensor << " ] [ " << regX << " ] [ " << regY << " ]" << endl;
    }

    Int_t nofCloseHits = 0;
    Double_t distToClosestHit = 100000.;
    for (Int_t ihit = 0; ihit < nofGemHits; ihit++) {
      gemHit = (PndGemHit *)fGemHitArray->At(ihit);
      if (gemHit->GetStationNr() != station + 1 || gemHit->GetSensorNr() != sensor + 1)
        continue;

      fhPointToHit[station][sensor][regX][regY]->Fill(pntX - gemHit->GetX(), pntY - gemHit->GetY());

      if (printInfo) {
        cout << "**** " << pntX << " : " << gemHit->GetX() << " *** " << pntY << " : " << gemHit->GetY() << " ***" << endl;
      }

      Double_t p2hDistSq = (pntX - gemHit->GetX()) * (pntX - gemHit->GetX()) + (pntY - gemHit->GetY()) * (pntY - gemHit->GetY());
      if (distToClosestHit > p2hDistSq)
        distToClosestHit = p2hDistSq;
      if (p2hDistSq < fPointEffDist * fPointEffDist) {
        if (printInfo) {
          cout << "POINT " << ipnt << " AND HIT " << ihit << " ARE CLOSE!!!" << endl;
        }
        nofCloseHits++;
      }
    }
    //    fhPointClosest     [station][sensor]->Fill(pntX,pntY,distToClosestHit);
    fhCloseHits[station][sensor]->Fill(nofCloseHits);
    fhPointNof[station][sensor]->Fill(pntX, pntY);
    fhPointRadNof[station][sensor]->Fill(TMath::Sqrt(pntX * pntX + pntY * pntY));
    if (nofCloseHits > 0) {
      fhPointReco[station][sensor]->Fill(pntX, pntY);
      fhPointRadReco[station][sensor]->Fill(TMath::Sqrt(pntX * pntX + pntY * pntY));
      if (nofCloseHits > 1) {
        fhHitMultipleRate[station][sensor]->Fill(pntX, pntY);
      }
    } else {
      if (fVerbose) {
        cout << "NO MATCHING POINT IN EVENT " << fNofEvents << " FOR POINT ( " << pntX << " , " << pntY << " , " << gemPnt->GetZ() << " ) "
             << "[" << gemPnt->GetX() << "X" << gemPnt->GetXOut() << "] [" << gemPnt->GetY() << "Y" << gemPnt->GetYOut() << "]" << endl;
      }
    }
  }

  for (Int_t ihit = 0; ihit < nofGemHits; ihit++) {
    gemHit = (PndGemHit *)fGemHitArray->At(ihit);

    Int_t station = gemHit->GetStationNr() - 1;
    Int_t sensor = gemHit->GetSensorNr() - 1;
    Double_t hitX = gemHit->GetX();
    Double_t hitY = gemHit->GetY();

    Bool_t hitHasMatchingPoint = kFALSE;
    //    Double_t distToClosestPoint = 100000.;
    for (Int_t ipnt = 0; ipnt < nofGemPnts; ipnt++) {
      gemPnt = (PndGemMCPoint *)fMCPointArray->At(ipnt);
      Int_t sensorId = gemPnt->GetSensorId();
      if (fDigiPar->GetStationNr(sensorId) != station + 1 || fDigiPar->GetSensorNr(sensorId) != sensor + 1)
        continue;

      Double_t pntX = (gemPnt->GetX() + gemPnt->GetXOut()) / 2.;
      Double_t pntY = (gemPnt->GetY() + gemPnt->GetYOut()) / 2.;

      //      Double_t p2hDistSq = (hitX-gemPnt->GetX())*(hitX-gemPnt->GetX())+(hitY-gemPnt->GetY())*(hitY-gemPnt->GetY());
      //      if ( distToClosestPoint > p2hDistSq )
      //	distToClosestPoint = p2hDistSq;
      if (TMath::Sqrt((hitX - pntX) * (hitX - pntX) + (hitY - pntY) * (hitY - pntY)) < fPointEffDist)
        hitHasMatchingPoint = kTRUE;
    }
    fhHitNof[station][sensor]->Fill(hitX, hitY);
    fhHitRadNof[station][sensor]->Fill(TMath::Sqrt(hitX * hitX + hitY * hitY));
    //    if ( distToClosestPoint > fPointEffDist*fPointEffDist )
    if (!hitHasMatchingPoint) {
      fhHitFake[station][sensor]->Fill(hitX, hitY);
      fhHitRadFake[station][sensor]->Fill(TMath::Sqrt(hitX * hitX + hitY * hitY));
    }
  }

  std::vector<std::pair<Int_t, Int_t>> mcMatchPointHit;

  Bool_t printMCMatching = kFALSE;
  if (fVerbose >= 1)
    printMCMatching = kTRUE;

  for (Int_t ihit = 0; ihit < nofGemHits; ihit++) {
    gemHit = (PndGemHit *)fGemHitArray->At(ihit);
    if (printMCMatching) {
      cout << "---> hit " << ihit << " has " << gemHit->GetNLinks() << " links" << endl;
      for (Int_t ilink = 0; ilink < gemHit->GetNLinks(); ilink++) {
        cout << " " << ilink << " > " << gemHit->GetLink(ilink).GetType() << " . " << gemHit->GetLink(ilink).GetIndex() << endl;
      }
    }

    Int_t quickPointIndex = -1;
    for (Int_t ilink = 0; ilink < gemHit->GetNLinks(); ilink++) {
      if (gemHit->GetLink(ilink).GetType() == fGemPointNumber) {
        if (quickPointIndex != -1) // already have found different point index - so it is a ghost
        {                          // but what about hits from clusters, if 98% one point, and only 2% other point???
          quickPointIndex = -2;
          break;
        }
        quickPointIndex = gemHit->GetLink(ilink).GetIndex();
      }
    }
    if (quickPointIndex >= 0) {
      pair<Int_t, Int_t> a(quickPointIndex, ihit);
      mcMatchPointHit.push_back(a);
    }

    if (quickPointIndex != -1)
      continue; // do not have any links to points
    if (gemHit->GetNLinks() == 2) {
      Int_t maxPnt0 = -1, maxPnt1 = -1;
      std::vector<Int_t> pointVector0;
      std::vector<Int_t> pointVector1;
      GetPointVector(gemHit->GetLink(0).GetType(), gemHit->GetLink(0).GetIndex(), pointVector0, printMCMatching);
      GetPointVector(gemHit->GetLink(1).GetType(), gemHit->GetLink(1).GetIndex(), pointVector1, printMCMatching);
      if (printMCMatching)
        cout << "VECT0: (" << gemHit->GetLink(0).GetIndex() << ") " << flush;
      for (size_t ipnt = 0; ipnt < pointVector0.size(); ipnt++) {
        if (printMCMatching)
          cout << pointVector0[ipnt] << " . " << flush;
        if (maxPnt0 < pointVector0[ipnt]) {
          maxPnt0 = pointVector0[ipnt];
        }
      }
      if (printMCMatching)
        cout << "\b\b" << endl;
      if (printMCMatching)
        cout << "VECT1: (" << gemHit->GetLink(1).GetIndex() << ") " << flush;
      for (size_t ipnt = 0; ipnt < pointVector1.size(); ipnt++) {
        if (printMCMatching)
          cout << pointVector1[ipnt] << " . " << flush;
        if (maxPnt1 < pointVector1[ipnt]) {
          maxPnt1 = pointVector1[ipnt];
        }
      }
      if (printMCMatching)
        cout << "\b\b" << endl;
      if (printMCMatching)
        cout << "highest points are " << maxPnt0 << " , " << maxPnt1 << endl;
      std::vector<Int_t> countPointV0(maxPnt0 + 1, 0);
      std::vector<Int_t> countPointV1(maxPnt1 + 1, 0);
      for (size_t ipnt = 0; ipnt < pointVector0.size(); ipnt++) {
        ++countPointV0[pointVector0[ipnt]];
      }
      for (size_t ipnt = 0; ipnt < pointVector1.size(); ipnt++) {
        ++countPointV1[pointVector1[ipnt]];
      }
      if (maxPnt0 > maxPnt1)
        maxPnt0 = maxPnt1;
      for (Int_t ipnt = 0; ipnt < maxPnt0 + 1; ipnt++) {
        if (printMCMatching)
          cout << ipnt << " - " << countPointV0[ipnt] << " ? " << countPointV1[ipnt] << endl;
        if (countPointV0[ipnt] > 0) {
          if (countPointV1[ipnt] > 0) {
            Double_t tempMatch = 100. * Double_t(countPointV0[ipnt] * countPointV1[ipnt]) / (Double_t(pointVector0.size() * pointVector1.size()));
            if (printMCMatching)
              cout << "HIT " << ihit << " IS " << countPointV0[ipnt] * countPointV1[ipnt] << " / " << pointVector0.size() * pointVector1.size() << " ( " << tempMatch
                   << " % ) POINT " << ipnt << endl;
            pair<Int_t, Int_t> a(ipnt, ihit);
            mcMatchPointHit.push_back(a);
          }
        }
      }
    }
  }

  std::vector<std::pair<Int_t, Int_t>>::iterator iter;
  for (iter = mcMatchPointHit.begin(); iter != mcMatchPointHit.end(); iter++) {
    gemPnt = (PndGemMCPoint *)fMCPointArray->At(iter->first);
    gemHit = (PndGemHit *)fGemHitArray->At(iter->second);

    Double_t pntX = (gemPnt->GetX() + gemPnt->GetXOut()) / 2.;
    Double_t pntY = (gemPnt->GetY() + gemPnt->GetYOut()) / 2.;

    Int_t station = gemHit->GetStationNr() - 1;
    Int_t sensor = gemHit->GetSensorNr() - 1;
    fhTrueMatchDiXYPerSt[station][sensor]->Fill(pntX - gemHit->GetX(), pntY - gemHit->GetY());
    fhTrueMatchDiXY->Fill(pntX - gemHit->GetX(), pntY - gemHit->GetY());
    Double_t tempDist = TMath::Sqrt((pntX - gemHit->GetX()) * (pntX - gemHit->GetX()) + (pntY - gemHit->GetY()) * (pntY - gemHit->GetY()));
    if (tempDist > 5.) {
      cout << "Event " << fNofEvents << ": point " << iter->first << " at ( " << gemPnt->GetX() << "," << gemPnt->GetY() << "," << gemPnt->GetZ() << " )"
           << " to ( " << gemPnt->GetXOut() << "," << gemPnt->GetYOut() << "," << gemPnt->GetZOut() << " )" << endl
           << "                hit " << iter->second << " at ( " << gemHit->GetX() << "," << gemHit->GetY() << "," << gemHit->GetZ() << " )" << endl
           << "          ---> dist " << tempDist << endl; //" with match value = " << tempMatch << endl;
    }
    fhTrueMatchDistPerSt[station][sensor]->Fill(tempDist);
    fhTrueMatchDist->Fill(tempDist);
    // fhTrueMatchValue                     ->Fill(tempMatch);
    // fhTrueMatchDistValue                 ->Fill(tempMatch,tempDist);
  }

  if (printMCMatching)
    cout << "True MC Matches betwen points and hits: " << mcMatchPointHit.size() << endl;

  std::vector<Int_t> nofMatchesPerHit(nofGemHits, 0);
  std::vector<Int_t> nofMatchesPerPoint(nofGemPnts, 0);
  for (iter = mcMatchPointHit.begin(); iter != mcMatchPointHit.end(); iter++) {
    ++nofMatchesPerPoint[iter->first];
    ++nofMatchesPerHit[iter->second];
  }
  for (Int_t ihit = 0; ihit < nofGemHits; ihit++) {
    fhTrueMatchNofPerHit->Fill(nofMatchesPerHit[ihit]);
  }
  for (Int_t ipnt = 0; ipnt < nofGemPnts; ipnt++) {
    fhTrueMatchNofPerPoint->Fill(nofMatchesPerPoint[ipnt]);
    gemPnt = (PndGemMCPoint *)fMCPointArray->At(ipnt);
    if (nofMatchesPerPoint[ipnt] > 0) {
      Double_t pntX = (gemPnt->GetX() + gemPnt->GetXOut()) / 2.;
      Double_t pntY = (gemPnt->GetY() + gemPnt->GetYOut()) / 2.;
      Int_t sensorId = gemPnt->GetSensorId();
      Int_t station = fDigiPar->GetStationNr(sensorId) - 1;
      Int_t sensor = fDigiPar->GetSensorNr(sensorId) - 1;
      fhPointMatch[station][sensor]->Fill(pntX, pntY);
      fhPointRadMatch[station][sensor]->Fill(TMath::Sqrt(pntX * pntX + pntY * pntY));
    }

    /*
  if ( nofMatchesPerPoint[ipnt] == 0 ) {
    // cout << "POINT " << ipnt << " AT "
    // 	   << gemPnt->GetX() << "-" << gemPnt->GetXOut() << "   "
    // 	   << gemPnt->GetY() << "-" << gemPnt->GetYOut() << "   "
    // 	   << gemPnt->GetZ() << "-" << gemPnt->GetZOut() << " WAS NOT RECONSTRUCTED" << endl;
    for ( Int_t ihit = 0 ; ihit < nofGemHits ; ihit++ ) {
gemHit = (PndGemHit*)fGemHitArray->At(ihit);

for ( Int_t ilink = 0 ; ilink < gemHit->GetNLinks() ; ilink++ ) {
  std::vector<Int_t> pointVector;
  Bool_t pointFoundHere = kFALSE;
  GetPointVector(gemHit->GetLink(ilink).GetType(),gemHit->GetLink(ilink).GetIndex(),pointVector,printMCMatching);
  for ( Int_t imatch = 0 ; imatch < pointVector.size() ; imatch++ ) {
    if ( pointVector[imatch] == ipnt ) {
      pointFoundHere = kTRUE;
    }
  }
  //	  if ( pointFoundHere ) cout << "FOUND POINT " << ipnt << " IN HIT " << ihit << " LINK " << ilink << endl;
}
    }
  }
    */
  }
}
// ------------------------------------------------------------

// -----   Private method GetPointVector, recurrence function  --------------------------------------------
Int_t PndGemFindHitsQA::GetPointVector(Int_t arrayId, Int_t entryId, std::vector<Int_t> &pointVector, Bool_t printInfo)
{
  if (printInfo) {
    for (Int_t itemp = 0; itemp < 10 - arrayId; itemp++)
      cout << " " << flush;
    cout << "GetPointVector(" << arrayId << ", " << entryId << ")" << endl;
  }
  if (arrayId == fGemPointNumber) {
    pointVector.push_back(entryId);
    return pointVector.size();
  }
  if (arrayId == 0) { // this is probably MCTrack!
    return 0;
  }

  FairMultiLinkedData *tempData = (FairMultiLinkedData *)fGemData[fGemDataPointer[arrayId]]->At(entryId);
  for (Int_t ilink = 0; ilink < tempData->GetNLinks(); ilink++) {
    GetPointVector(tempData->GetLink(ilink).GetType(), tempData->GetLink(ilink).GetIndex(), pointVector, printInfo);
  }
  return pointVector.size();
}
// ------------------------------------------------------------

// -----   Private method CreateHistos   --------------------------------------------
void PndGemFindHitsQA::CreateHistos()
{
  fHistoList = new TList();
  // number of mc tracks, reco tracks, efficiency as function of MOMENTUM

  Int_t nStations = fDigiPar->GetNStations();

  fHistPlaneDivs = 2;

  for (Int_t istat = 0; istat < nStations; istat++) {
    PndGemStation *station = (PndGemStation *)fDigiPar->GetStation(istat);
    Int_t nSensors = station->GetNSensors();
    for (Int_t isens = 0; isens < nSensors; isens++) {
      PndGemSensor *sensor = (PndGemSensor *)station->GetSensor(isens);
      Double_t sensOutRad = sensor->GetOuterRadius();
      fHistWidth[istat][isens] = TMath::Ceil(sensOutRad / fHistPlaneDivs); // make it an even number
      Double_t histBeginX = -fHistWidth[istat][isens] * fHistPlaneDivs;
      for (Int_t iregx = 0; iregx < 2 * fHistPlaneDivs; iregx++) {
        Double_t histBeginY = -fHistWidth[istat][isens] * fHistPlaneDivs;
        for (Int_t iregy = 0; iregy < 2 * fHistPlaneDivs; iregy++) {
          fhPointToHit[istat][isens][iregx][iregy] = new TH2F(Form("fhPointToHit_s%d_s%d_x%d_y%d", istat, isens, iregx, iregy),
                                                              Form("Points vs hits, station %d, sensor %d at z=%.1fcm, %.1f<x<%.1f, %.1f<y<%.1f", istat, isens, sensor->GetZ0(),
                                                                   histBeginX, histBeginX + fHistWidth[istat][isens], histBeginY, histBeginY + fHistWidth[istat][isens]),
                                                              2000, -1., 1., 2000, -1., 1.);
          histBeginY += fHistWidth[istat][isens];
          fHistoList->Add(fhPointToHit[istat][isens][iregx][iregy]);
        }
        histBeginX += fHistWidth[istat][isens];
      }
      // fhPointClosest     [istat][isens] = new TH3F(Form("fhPointClosest_s%d_s%d",istat,isens),
      // 						   Form("Distance from point to closest hits, station %d, sensor %d",istat,isens),
      // 						   2*TMath::Ceil(sensOutRad),-TMath::Ceil(sensOutRad),TMath::Ceil(sensOutRad),
      // 						   2*TMath::Ceil(sensOutRad),-TMath::Ceil(sensOutRad),TMath::Ceil(sensOutRad),
      // 						   1000,0.,1.);
      fhPointNof[istat][isens] = new TH2F(Form("fhPointNof_s%d_s%d", istat, isens), Form("Number of points, station %d, sensor %d", istat, isens), 2 * TMath::Ceil(sensOutRad),
                                          -TMath::Ceil(sensOutRad), TMath::Ceil(sensOutRad), 2 * TMath::Ceil(sensOutRad), -TMath::Ceil(sensOutRad), TMath::Ceil(sensOutRad));
      fhPointReco[istat][isens] =
        new TH2F(Form("fhPointReco_s%d_s%d", istat, isens), Form("Number of reconstructed points (point-hit %.2f cm), station %d, sensor %d", fPointEffDist, istat, isens),
                 2 * TMath::Ceil(sensOutRad), -TMath::Ceil(sensOutRad), TMath::Ceil(sensOutRad), 2 * TMath::Ceil(sensOutRad), -TMath::Ceil(sensOutRad), TMath::Ceil(sensOutRad));
      fhPointRecoEff[istat][isens] =
        new TH2F(Form("fhPointRecoEff_s%d_s%d", istat, isens), Form("Hit finding efficiency (point-hit %.2f cm), station %d, sensor %d", fPointEffDist, istat, isens),
                 2 * TMath::Ceil(sensOutRad), -TMath::Ceil(sensOutRad), TMath::Ceil(sensOutRad), 2 * TMath::Ceil(sensOutRad), -TMath::Ceil(sensOutRad), TMath::Ceil(sensOutRad));
      fhPointRadNof[istat][isens] = new TH1F(Form("fhPointRadNof_s%d_s%d", istat, isens), Form("Number of points, station %d, sensor %d", istat, isens), 1000, 0, 100);
      fhPointRadReco[istat][isens] = new TH1F(Form("fhPointRadReco_s%d_s%d", istat, isens),
                                              Form("Number of reconstructed points (point-hit %.2f cm), station %d, sensor %d", fPointEffDist, istat, isens), 1000, 0, 100);
      fhPointRadRecoEff[istat][isens] = new TH1F(Form("fhPointRadRecoEff_s%d_s%d", istat, isens),
                                                 Form("Hit finding efficiency (point-hit %.2f cm), station %d, sensor %d", fPointEffDist, istat, isens), 1000, 0, 100);

      fhPointMatch[istat][isens] =
        new TH2F(Form("fhPointMatch_s%d_s%d", istat, isens), Form("Number of matched points (point-hit %.2f cm), station %d, sensor %d", fPointEffDist, istat, isens),
                 2 * TMath::Ceil(sensOutRad), -TMath::Ceil(sensOutRad), TMath::Ceil(sensOutRad), 2 * TMath::Ceil(sensOutRad), -TMath::Ceil(sensOutRad), TMath::Ceil(sensOutRad));
      fhPointMatchEff[istat][isens] =
        new TH2F(Form("fhPointMatchEff_s%d_s%d", istat, isens), Form("Matched points efficiency (point-hit %.2f cm), station %d, sensor %d", fPointEffDist, istat, isens),
                 2 * TMath::Ceil(sensOutRad), -TMath::Ceil(sensOutRad), TMath::Ceil(sensOutRad), 2 * TMath::Ceil(sensOutRad), -TMath::Ceil(sensOutRad), TMath::Ceil(sensOutRad));
      fhPointRadMatch[istat][isens] = new TH1F(Form("fhPointRadMatch_s%d_s%d", istat, isens),
                                               Form("Number of matched points (point-hit %.2f cm), station %d, sensor %d", fPointEffDist, istat, isens), 1000, 0, 100);
      fhPointRadMatchEff[istat][isens] = new TH1F(Form("fhPointRadMatchEff_s%d_s%d", istat, isens),
                                                  Form("Matched points efficiency (point-hit %.2f cm), station %d, sensor %d", fPointEffDist, istat, isens), 1000, 0, 100);

      fhHitNof[istat][isens] =
        new TH2F(Form("fhHitNof_s%d_s%d", istat, isens), Form("Number of all hits (point-hit %.2f cm), station %d, sensor %d", fPointEffDist, istat, isens),
                 2 * TMath::Ceil(sensOutRad), -TMath::Ceil(sensOutRad), TMath::Ceil(sensOutRad), 2 * TMath::Ceil(sensOutRad), -TMath::Ceil(sensOutRad), TMath::Ceil(sensOutRad));
      fhHitFake[istat][isens] =
        new TH2F(Form("fhHitFake_s%d_s%d", istat, isens), Form("Number of fake hits (point-hit %.2f cm), station %d, sensor %d", fPointEffDist, istat, isens),
                 2 * TMath::Ceil(sensOutRad), -TMath::Ceil(sensOutRad), TMath::Ceil(sensOutRad), 2 * TMath::Ceil(sensOutRad), -TMath::Ceil(sensOutRad), TMath::Ceil(sensOutRad));
      fhHitFakeProb[istat][isens] =
        new TH2F(Form("fhHitFakeProb_s%d_s%d", istat, isens), Form("Hit fake probability (point-hit %.2f cm), station %d, sensor %d", fPointEffDist, istat, isens),
                 2 * TMath::Ceil(sensOutRad), -TMath::Ceil(sensOutRad), TMath::Ceil(sensOutRad), 2 * TMath::Ceil(sensOutRad), -TMath::Ceil(sensOutRad), TMath::Ceil(sensOutRad));
      fhHitRadNof[istat][isens] =
        new TH1F(Form("fhHitRadNof_s%d_s%d", istat, isens), Form("Number of all hits (point-hit %.2f cm), station %d, sensor %d", fPointEffDist, istat, isens), 1000, 0, 100);
      fhHitRadFake[istat][isens] =
        new TH1F(Form("fhHitRadFake_s%d_s%d", istat, isens), Form("Number of fake hits (point-hit %.2f cm), station %d, sensor %d", fPointEffDist, istat, isens), 1000, 0, 100);
      fhHitRadFakeProb[istat][isens] = new TH1F(Form("fhHitRadFakeProb_s%d_s%d", istat, isens),
                                                Form("Hit fake probability (point-hit %.2f cm), station %d, sensor %d", fPointEffDist, istat, isens), 1000, 0, 100);

      fhHitMultipleRate[istat][isens] = new TH2F(
        Form("fhHitMultipleRate_s%d_s%d", istat, isens), Form("Distance from point to closest hits (point-hit %.2f cm), station %d, sensor %d", fPointEffDist, istat, isens),
        2 * TMath::Ceil(sensOutRad), -TMath::Ceil(sensOutRad), TMath::Ceil(sensOutRad), 2 * TMath::Ceil(sensOutRad), -TMath::Ceil(sensOutRad), TMath::Ceil(sensOutRad));
      fhCloseHits[istat][isens] = new TH1F(Form("fhCloseHits_s%d_s%d", istat, isens),
                                           Form("Number of close hits per point (point-hit %.2f cm), station %d, sensor %d", fPointEffDist, istat, isens), 101, -0.5, 100.5);
      fhTrueMatchDiXYPerSt[istat][isens] =
        new TH2F(Form("fhTrueMatchDiXYPerSt_s%d_s%d", istat, isens), Form("True matches: difference in XY between points and hits, station %d, sensor %d", istat, isens), 200, -1.,
                 1., 200, -1., 1.);
      fhTrueMatchDistPerSt[istat][isens] =
        new TH1F(Form("fhTrueMatchDistPerSt_s%d_s%d", istat, isens), Form("True matches: distance between points and hits, station %d, sensor %d", istat, isens), 1000, 0, 10);
      //      fHistoList->Add(fhPointClosest     [istat][isens]);
      fHistoList->Add(fhPointNof[istat][isens]);
      fHistoList->Add(fhPointReco[istat][isens]);
      fHistoList->Add(fhPointRecoEff[istat][isens]);
      fHistoList->Add(fhPointRadNof[istat][isens]);
      fHistoList->Add(fhPointRadReco[istat][isens]);
      fHistoList->Add(fhPointRadRecoEff[istat][isens]);
      fHistoList->Add(fhPointMatch[istat][isens]);
      fHistoList->Add(fhPointMatchEff[istat][isens]);
      fHistoList->Add(fhPointRadMatch[istat][isens]);
      fHistoList->Add(fhPointRadMatchEff[istat][isens]);
      fHistoList->Add(fhHitNof[istat][isens]);
      fHistoList->Add(fhHitFake[istat][isens]);
      fHistoList->Add(fhHitFakeProb[istat][isens]);
      fHistoList->Add(fhHitRadNof[istat][isens]);
      fHistoList->Add(fhHitRadFake[istat][isens]);
      fHistoList->Add(fhHitRadFakeProb[istat][isens]);
      fHistoList->Add(fhHitMultipleRate[istat][isens]);
      fHistoList->Add(fhCloseHits[istat][isens]);
      fHistoList->Add(fhTrueMatchDiXYPerSt[istat][isens]);
      fHistoList->Add(fhTrueMatchDistPerSt[istat][isens]);
    }
  }
  fhTrueMatchDiXY = new TH2F("fhTrueMatchDiXY", "True matches: difference in XY between points and hits", 2000, -1., 1., 2000, -1., 1.);
  fhTrueMatchDist = new TH1F("fhTrueMatchDist", "True matches: distance between points and hits", 1000, 0, 10);
  fhTrueMatchValue = new TH1F("fhTrueMatchValue", "True matches: hit-point match value in pecent", 1010, 0., 101.);
  fhTrueMatchDistValue = new TH2F("fhTrueMatchDistValue", "True matches: hit-point match value vs distance", 101, 0., 101., 200, 0., 2.);

  fhTrueMatchNofPerHit = new TH1F("fhTrueMatchNofPerHit", "True matches: number of matched points per hit", 101, -0.5, 100.5);
  fhTrueMatchNofPerPoint = new TH1F("fhTrueMatchNofPerPoint", "True matches: number of matched hits per point", 101, -0.5, 100.5);

  fHistoList->Add(fhTrueMatchDiXY);
  fHistoList->Add(fhTrueMatchDist);
  fHistoList->Add(fhTrueMatchValue);
  fHistoList->Add(fhTrueMatchDistValue);

  fHistoList->Add(fhTrueMatchNofPerHit);
  fHistoList->Add(fhTrueMatchNofPerPoint);
}
// ------------------------------------------------------------

// -----   Private method DivideHistos  --------------------------------------------
void PndGemFindHitsQA::DivideHistos(TH1 *hist1, TH1 *hist2, TH1 *hist3)
{
  hist1->Sumw2();
  hist2->Sumw2();
  hist3->Divide(hist1, hist2, 1, 1, "B");
}
// ------------------------------------------------------------

// -----   Private method Finish   --------------------------------------------
void PndGemFindHitsQA::Finish()
{

  cout << "-------------------- PndGemFindHitsQA : Finish -------------------" << endl;
  //  cout << " dividing histos" << endl;

  Int_t nStations = fDigiPar->GetNStations();
  for (Int_t istat = 0; istat < nStations; istat++) {
    for (Int_t isens = 0; isens < 2; isens++) {
      //      cout << "doing " << istat << " / " << isens << endl;
      fhPointReco[istat][isens]->Sumw2();
      fhPointNof[istat][isens]->Sumw2();
      fhPointRecoEff[istat][isens]->Divide(fhPointReco[istat][isens], fhPointNof[istat][isens], 1., 1., "B");
      fhPointRecoEff[istat][isens]->Scale(100.);

      fhPointMatch[istat][isens]->Sumw2();
      fhPointMatchEff[istat][isens]->Divide(fhPointMatch[istat][isens], fhPointNof[istat][isens], 1., 1., "B");
      fhPointMatchEff[istat][isens]->Scale(100.);

      fhPointRadReco[istat][isens]->Sumw2();
      fhPointRadNof[istat][isens]->Sumw2();
      fhPointRadRecoEff[istat][isens]->Divide(fhPointRadReco[istat][isens], fhPointRadNof[istat][isens], 1., 1., "B");
      fhPointRadRecoEff[istat][isens]->Scale(100.);

      fhPointRadMatch[istat][isens]->Sumw2();
      fhPointRadMatchEff[istat][isens]->Divide(fhPointRadMatch[istat][isens], fhPointRadNof[istat][isens], 1., 1., "B");
      fhPointRadMatchEff[istat][isens]->Scale(100.);

      fhHitFake[istat][isens]->Sumw2();
      fhHitNof[istat][isens]->Sumw2();
      fhHitFakeProb[istat][isens]->Divide(fhHitFake[istat][isens], fhHitNof[istat][isens]);
      fhHitFakeProb[istat][isens]->Scale(100.);

      fhHitRadFake[istat][isens]->Sumw2();
      fhHitRadNof[istat][isens]->Sumw2();
      fhHitRadFakeProb[istat][isens]->Divide(fhHitRadFake[istat][isens], fhHitRadNof[istat][isens]);
      fhHitRadFakeProb[istat][isens]->Scale(100.);
    }
  }

  //  cout << "-------------------- PndGemFindHitsQA : Summary ------------------" << endl;

  cout << " Events:        " << setw(10) << fNofEvents << endl;

  cout << "-------------------- PndGemFindHitsQA : Efficiency ---------------" << endl;
  cout << "   efficiency = number of points that have hit closer than " << fPointEffDist << " cm / number of points" << endl;
  Int_t nofPointsAll = 0;
  Int_t nofPointsReco = 0;
  for (Int_t istat = 0; istat < nStations; istat++) {
    PndGemStation *station = (PndGemStation *)fDigiPar->GetStation(istat);
    Int_t nSensors = station->GetNSensors();
    for (Int_t isens = 0; isens < nSensors; isens++) {
      nofPointsAll += fhPointRadNof[istat][isens]->GetEntries();
      nofPointsReco += fhPointRadReco[istat][isens]->GetEntries();
      cout << "Station " << istat << " sensor " << isens
           << ", eff = " << ((Double_t)(fhPointRadReco[istat][isens]->GetEntries())) / ((Double_t)(fhPointRadNof[istat][isens]->GetEntries())) * 100. << "% ("
           << fhPointRadReco[istat][isens]->GetEntries() << " / " << fhPointRadNof[istat][isens]->GetEntries() << ")" << endl;
    }
  }
  cout << "            OVERALL EFF = " << ((Double_t)(nofPointsReco)) / ((Double_t)(nofPointsAll)) * 100. << "% (" << nofPointsReco << " / " << nofPointsAll << ")" << endl;
  cout << "o   o   o   o   o   o   o   o   o   o   o   o   o   o   o   o   o" << endl;
  Int_t pointsAll = fhTrueMatchNofPerPoint->GetEntries();
  Int_t pointsMCM = pointsAll - fhTrueMatchNofPerPoint->GetBinContent(fhTrueMatchNofPerPoint->FindBin(0.));
  cout << "         TRUE MATCH EFF = " << ((Double_t)(pointsMCM)) / ((Double_t)(pointsAll)) * 100. << "% (" << pointsMCM << " / " << pointsAll << ")" << endl;

  cout << "-----------------------------------------------------------------" << endl;

  FairSink *sink = FairRootManager::Instance()->GetSink();

  if (sink->GetSinkType() == kFILESINK) {
    TDirectory::TContext restorecwd{};
    TFile *outfile = dynamic_cast<FairRootFileSink *>(sink)->GetRootFile();
    outfile->mkdir("GemFindHitsQA");
    outfile->cd("GemFindHitsQA");
    TIter next(fHistoList); // this looks strange here
    while (TH1 *histo = ((TH1 *)next())) {
      outfile->WriteTObject(histo);
    }
    outfile->cd("..");
  }

  //  gDirectory->mkdir("GemFindHitsQA");
  //  gDirectory->cd("GemFindHitsQA");
  //  TIter next(fHistoList);
  //  while (TH1 *histo = ((TH1 *)next()))
  //    histo->Write();
  //  gDirectory->cd("..");
  //
  //  gFile = temp;
}
// ------------------------------------------------------------

ClassImp(PndGemFindHitsQA)
