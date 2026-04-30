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

/*
 * PndIdealTrackFinders.cpp
 *
 *  Created on: Apr 12, 2010
 *      Author: stockman
 */

#include "PndIdealTrackFinder.h"
#include "FairRootManager.h"
#include "FairMCPoint.h"
#include "FairHit.h"
#include "FairLogger.h"

#include "PndTrackCand.h"
#include "PndTrack.h"
#include "PndMCTrack.h"
#include "PndGemMCPoint.h"
#include "PndGemHit.h"

#include "TRandom.h"
ClassImp(PndIdealTrackFinder);

PndIdealTrackFinder::PndIdealTrackFinder(TString name)
  : PndPersistencyTask(name), fOutBranchName("IdealTrack"), fTrackCand(0), fTrack(0), fMCTrack(0), fTrackSelector(nullptr), fPdg(0), fHitCount(0), fMomSigma(0, 0, 0), fDPoP(0.),
    fRunTimeBased(kFALSE), fRelative(kFALSE), fVtxSigma(0, 0, 0), fEfficiency(1.), fFunctor(nullptr)
{

  // TODO Replace hard coded names with variable names
  fPointBranchMap["MVDHitsPixel"] = "MVDPoint";
  fPointBranchMap["MVDHitsStrip"] = "MVDPoint";
  fPointBranchMap["STTHit"] = "STTPoint";
  fPointBranchMap["GEMHit"] = "GEMPoint";
  fPointBranchMap["FTSHit"] = "FTSPoint";

  fPointBranchMap["SciTHit"] = "SciTPoint";
  fPointBranchMap["MdtHit"] = "MdtPoint";

  fPointBranchMap["DircHit"] = ""; // no FairLinks Provided!
  fPointBranchMap["FTofHit"] = ""; // no FairLinks Provided!
  fPointBranchMap["RichHit"] = ""; // no FairLinks Provided!

  // Sorted hits for time based simulation

  fPointBranchMap["MVDSortedHitsPixels"] = "MVDPoint";
  fPointBranchMap["MVDSortedHitsStrips"] = "MVDPoint";
  fPointBranchMap["STTSortedHits"] = "STTPoint";
  fPointBranchMap["GEMSortedHits"] = "GEMPoint";
  fPointBranchMap["FTSSortedHits"] = "FTSPoint";

  SetPersistency(kTRUE);
}

PndIdealTrackFinder::~PndIdealTrackFinder()
{
  if (fTrackSelector != nullptr)
    delete fTrackSelector;
}

// -----   Public method Init   --------------------------------------------
InitStatus PndIdealTrackFinder::Init()
{

  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    std::cout << "-E- PndMCTestHitCompare::Init: "
              << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  if (fBranchNames.size() == 0) {
    // Use hits of all tracking subsystems if nothing is given
    AddBranchName("MVDHitsPixel");
    AddBranchName("MVDHitsStrip");
    AddBranchName("STTHit");
    AddBranchName("GEMHit");
    AddBranchName("FTSHit");
  }

  for (size_t i = 0; i < fBranchNames.size(); i++) {
    if (ioman->GetObject(fBranchNames[i]) != 0) {
      fBranchMap[fBranchNames[i]] = (TClonesArray *)ioman->GetObject(fBranchNames[i]);
      ioman->GetObject(fPointBranchMap[fBranchNames[i]]); // initialise the used FairMcPoint Branches
    }
  }

  fMCTrack = (TClonesArray *)ioman->GetObject("MCTrack");

  fTrackCand = new TClonesArray("PndTrackCand");
  ioman->Register(fOutBranchName + "Cand", "MC", fTrackCand, GetPersistency());
  fTrack = new TClonesArray("PndTrack");
  ioman->Register(fOutBranchName, "MC", fTrack, GetPersistency());

  if (fTrackSelector == nullptr) {
    LOG(warn) << " PndIdealTrackFinder::Init() no fTrackSelector set! All possible tracks will be generated!";
  }

  fStopTimeValue = 0.0;

  fFunctor = new StopTime();

  fPdg = new TDatabasePDG();

  return kSUCCESS;
}

void PndIdealTrackFinder::Exec(Option_t *)
{
  if (fRunTimeBased) {
    for (size_t i = 0; i < fBranchNames.size(); i++) {
      fBranchMap[fBranchNames[i]]->Delete(); // Clear the TClones array for the next time burst
    }
    fBranchMap.clear();
  }

  fTrackCand->Delete();
  fTrack->Delete();
  fTrackCandMap.clear();

  //	std::cout << "Event #" << FairRootManager::Instance()->GetEntryNr() << std::endl;

  if (fRunTimeBased) {
    fStopTimeValue += 2000.0; // In [ns]
    // std::cout << "fStopTime= " << fStopTimeValue << std::endl;

    for (size_t i = 0; i < fBranchNames.size(); i++) {
      // Start and stop functor and condition is needed
      // This can be used when objects are derived from FairTimeStamp
      fBranchMap[fBranchNames[i]] = FairRootManager::Instance()->GetData(fBranchNames[i], fFunctor, fStopTimeValue - 2000.0, fFunctor, fStopTimeValue);
      // std::cout << "IdealTrackFinder: " << fBranchMap[fBranchNames[i]]->GetEntriesFast() << std::endl;
    }
  }

  CreateTrackCands();

  FilterTrackCands();

  CreateTracks();
}

void PndIdealTrackFinder::CreateTrackCands()
{

  fHitCount = 0;

  // Iterating through the branches, the different hit branches which have been read in
  for (std::map<TString, TClonesArray *>::iterator iter = fBranchMap.begin(); iter != fBranchMap.end(); iter++) {

    // Second.GetEntries gives the number of objects in the TClonesArray which are present
    // This is done for each event separately in the event based case
    // For the time based case it does this for all hits which were collected within a certain timespan

    for (int i = 0; i < iter->second->GetEntriesFast(); i++) {

      FairMultiLinkedData array;
      FairMultiLinkedData_Interface *links = (FairMultiLinkedData_Interface *)iter->second->At(i);

      // Get a new hit branch in every iteration
      TString hitBranch = iter->first;

      FairMCPoint *point = GetFairMCPoint(hitBranch, links, array);
      if (point == 0) {

        // cout << "No MC Point" << endl;
        continue;
      }

      FairMCPoint firstpoint = *point;
      FairMCPoint lastpoint = *point;

      double tof = point->GetTime();

      // cout << "Time: " << tof << " from entry: " << i << endl;

      delete (point);

      // This loop is only used if hits are created from  several mc points
      // If hit is created from more than 1 point, the mean time will only be the actual time
      for (int ipnt = 1; ipnt < array.GetNLinks(); ipnt++) { // Array is over the FairLinks
        point = (FairMCPoint *)FairRootManager::Instance()->GetCloneOfLinkData(array.GetLink(ipnt));
        tof += point->GetTime();
        //  std::cout << ipnt << " " << tof << std::endl;
        if (point->GetTime() < firstpoint.GetTime())
          firstpoint = *point;
        if (point->GetTime() > lastpoint.GetTime())
          lastpoint = *point;
        delete (point);
      }
      tof /= array.GetNLinks();

      // cout << "Point time: " << tof << endl;

      // std::cout << i << " " << tof << std::endl;
      // .............................................

      // Get the MC track from the fair link from the hit branch
      FairMultiLinkedData mctracks = links->GetLinksWithType(FairRootManager::Instance()->GetBranchId("MCTrack"));

      for (int trackIndex = 0; trackIndex < mctracks.GetNLinks(); trackIndex++) {
        if (!fTrackCandMap.count(mctracks.GetLink(trackIndex))) {
          fTrackCandMap[mctracks.GetLink(trackIndex)] = PndTrackCand();
          fTrackCandMap[mctracks.GetLink(trackIndex)].SetInsertHistory(kTRUE);
          fFirstPointMap[mctracks.GetLink(trackIndex)] = firstpoint;
          // fFirstPointMap[mctracks.GetLink(trackIndex)].SetInsertHistory(kTRUE);
          fLastPointMap[mctracks.GetLink(trackIndex)] = lastpoint;
          // fLastPointMap[mctracks.GetLink(trackIndex)].SetInsertHistory(kTRUE);
        } else {
          FairMCPoint tmpfirstpoint = fFirstPointMap[mctracks.GetLink(trackIndex)];
          if (firstpoint.GetTime() < tmpfirstpoint.GetTime())
            fFirstPointMap[mctracks.GetLink(trackIndex)] = firstpoint;
          FairMCPoint tmplastpoint = fLastPointMap[mctracks.GetLink(trackIndex)];
          if (lastpoint.GetTime() > tmplastpoint.GetTime())
            fLastPointMap[mctracks.GetLink(trackIndex)] = lastpoint;
        }
        FairLink link(-1, FairRootManager::Instance()->GetEntryNr(), FairRootManager::Instance()->GetBranchId(iter->first), i);
        if (fRunTimeBased == kTRUE)
          link = links->GetEntryNr();
        // std::cout << "CreateTrackCands " << mctracks.GetLink(trackIndex) << " : " << link << std::endl;
        fTrackCandMap[mctracks.GetLink(trackIndex)].SetInsertHistory(kTRUE);
        // fTrackCandMap[mctracks.GetLink(trackIndex)].AddHit(link, fHitCount++);			//todo Rho is not properly calculated!
        fTrackCandMap[mctracks.GetLink(trackIndex)].AddHit(link, tof);
      }
    }
  }
}

void PndIdealTrackFinder::FilterTrackCands()
{
  if (fTrackSelector == nullptr) {
    return;
  }
  for (std::map<FairLink, PndTrackCand>::iterator iter = fTrackCandMap.begin(); iter != fTrackCandMap.end();) {
    if (!(*fTrackSelector)(iter->second.GetPointerToLinks(), true)) {
      fTrackCandMap.erase(iter++);
    } else {
      ++iter;
    }
  }
}

void PndIdealTrackFinder::CreateTracks()
{
  for (std::map<FairLink, PndTrackCand>::iterator iter = fTrackCandMap.begin(); iter != fTrackCandMap.end(); iter++) {

    PndMCTrack *mc = (PndMCTrack *)FairRootManager::Instance()->GetCloneOfLinkData(iter->first);
    if (mc == nullptr) {
      LOG(error) << " PndIdealTrackFinder::CreateTracks no mc track for link: " << iter->first;
    }

    PndTrackCand *myTrackCand = new ((*fTrackCand)[fTrackCand->GetEntriesFast()]) PndTrackCand(iter->second);
    myTrackCand->setMcTrackId(iter->first.GetIndex());
    myTrackCand->AddLink(iter->first);
    //		myTrackCand->SetTimeStamp(FairRootManager::Instance()->GetEventTime());  //this is only correct in the event based case.
    //      myTrackCand->SetTimeStamp(mc->GetStartTime());                         //todo: Add the event time of the MC Event to the MC Track Time
    myTrackCand->SetEntryNr(
      FairLink(-1, FairRootManager::Instance()->GetEntryNr(), FairRootManager::Instance()->GetBranchId(fOutBranchName + "Cand"), fTrackCand->GetEntriesFast() - 1));

    // ....... track

    int charge = 0;
    if (mc->GetPdgCode() < 100000000)
      charge = (Int_t)TMath::Sign(1.0, ((TParticlePDG *)fPdg->GetParticle(mc->GetPdgCode()))->Charge());
    else
      charge = 1;

    if (0 < fEfficiency && fEfficiency < 1) {
      if (gRandom->Rndm() > fEfficiency)
        continue;
    }

    // first
    FairMCPoint firstpoint = fFirstPointMap[iter->first];
    TVector3 firstpos(0, 0, 0), firstmom(0, 0, 0);
    if (myTrackCand->GetSortedHit(0).GetDetId() == FairRootManager::Instance()->GetBranchId("GEMHit")) {
      TClonesArray *gemhitarray = fBranchMap["GEMHit"];
      Int_t hitid = myTrackCand->GetSortedHit(0).GetHitId();
      PndGemHit *gemhit = (PndGemHit *)gemhitarray->At(hitid);
      FairMultiLinkedData gemhitlink = gemhit->GetLinksWithType(FairRootManager::Instance()->GetBranchId("GEMPoint"));
      PndGemMCPoint *gempoint = (PndGemMCPoint *)FairRootManager::Instance()->GetCloneOfLinkData(gemhitlink.GetLink(0));

      TVector3 posin(0, 0, 0), posout(0, 0, 0);
      gempoint->Position(posin);
      gempoint->PositionOut(posout);
      firstpos = 0.5 * (posin + posout);
    } else
      firstpoint.Position(firstpos);
    SmearVector(firstpos, fVtxSigma);

    firstpoint.Momentum(firstmom);
    if (fRelative) {
      fMomSigma.SetXYZ(fDPoP / TMath::Sqrt(2) * firstmom.X(), fDPoP / TMath::Sqrt(2) * firstmom.Y(), fDPoP * firstmom.Z());
    }
    SmearVector(firstmom, fMomSigma);

    FairTrackParP firstPar(firstpos, firstmom, fVtxSigma, fMomSigma, charge, firstpos, TVector3(1., 0., 0.), TVector3(0., 1., 0.));
    // last
    FairMCPoint lastpoint = fLastPointMap[iter->first];
    TVector3 lastpos(0, 0, 0), lastmom(0, 0, 0);

    if (myTrackCand->GetSortedHit(myTrackCand->GetNHits() - 1).GetDetId() == FairRootManager::Instance()->GetBranchId("GEMHit")) {
      TClonesArray *gemhitarray = fBranchMap["GEMHit"];
      Int_t hitid = myTrackCand->GetSortedHit(myTrackCand->GetNHits() - 1).GetHitId();
      PndGemHit *gemhit = (PndGemHit *)gemhitarray->At(hitid);
      FairMultiLinkedData gemhitlink = gemhit->GetLinksWithType(FairRootManager::Instance()->GetBranchId("GEMPoint"));
      PndGemMCPoint *gempoint = (PndGemMCPoint *)FairRootManager::Instance()->GetCloneOfLinkData(gemhitlink.GetLink(0));

      TVector3 posin(0, 0, 0), posout(0, 0, 0);
      gempoint->Position(posin);
      gempoint->PositionOut(posout);
      lastpos = 0.5 * (posin + posout);
    } else
      lastpoint.Position(lastpos);

    SmearVector(lastpos, fVtxSigma);
    lastpoint.Momentum(lastmom);
    SmearVector(lastmom, fMomSigma);
    FairTrackParP lastPar(lastpos, lastmom, fVtxSigma, fMomSigma, charge, lastpos, TVector3(1., 0., 0.), TVector3(0., 1., 0.));

    PndTrack *myTrack = new ((*fTrack)[fTrack->GetEntriesFast()]) PndTrack(firstPar, lastPar, *myTrackCand, 0, 0, 1, mc->GetPdgCode(), -1, -1);
    myTrack->SetEntryNr(FairLink(-1, FairRootManager::Instance()->GetEntryNr(), FairRootManager::Instance()->GetBranchId(fOutBranchName), fTrack->GetEntriesFast() - 1));
    myTrack->AddLink(FairLink(-1, FairRootManager::Instance()->GetEntryNr(), FairRootManager::Instance()->GetBranchId(fOutBranchName + "Cand"), fTrack->GetEntriesFast() - 1));
  }
}

FairMCPoint *PndIdealTrackFinder::GetFairMCPoint(TString hitBranch, FairMultiLinkedData_Interface *links, FairMultiLinkedData &array)
{
  // get the mc point(s) from each reco hit ......
  FairMultiLinkedData mcpoints = links->GetLinksWithType(FairRootManager::Instance()->GetBranchId(fPointBranchMap[hitBranch]));
  // std::cout << "-I- PndIDealTrackFinder::GetFairMCPoint hit " << hitBranch << " connected to points " << mcpoints << std::endl;

  // There seems to be a bug with ghost hits from the GEM stations. If more than one
  // MC point is associated to a hit, there is a good chance for false assignments
  // leading to wrong tracks. For the moment, skip hits with more than 1 GEM point.

  if (hitBranch == "GEMHit" && mcpoints.GetNLinks() > 1)
    return nullptr;
  //	if ((*iter).first == "MVDHitsStrip" && mvdpoints.GetNLinks() > 1) return 0;

  array = mcpoints;

  if (array.GetNLinks() == 0) {

    return nullptr;
  }
  return (FairMCPoint *)FairRootManager::Instance()->GetCloneOfLinkData(array.GetLink(0));
}

void PndIdealTrackFinder::SmearVector(TVector3 &vec, const TVector3 &sigma)
{
  // gaussian smearing
  Double_t rannn = 0.;
  rannn = gRandom->Gaus(vec.X(), sigma.X());
  vec.SetX(rannn);

  rannn = gRandom->Gaus(vec.Y(), sigma.Y());
  vec.SetY(rannn);

  rannn = gRandom->Gaus(vec.Z(), sigma.Z());
  vec.SetZ(rannn);

  return;
}
