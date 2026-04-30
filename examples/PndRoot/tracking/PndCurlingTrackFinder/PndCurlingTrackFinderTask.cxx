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

/////////////////////////////////////////////////////////////
//  PndCurlingTrackFinderTask
//  Finds Track
/////////////////////////////////////////////////////////////////

/** PndCurlingTrackFinderTask
 *@author Anna Alicke <a.alicke@fz-juelich.de>
 *@since 14.09.2021
 *
 *@version 1.0
 **
 ** PANDA task class for finding tracks based on the triplet finder and the apollonius calculation
 ** Task level RECO
 **/

// Includes from base
#include "FairRunAna.h"
#include <FairField.h>
#include "FairLogger.h"
#include <FairRuntimeDb.h>
// Pnd includes
#include "PndTrack.h"
#include "PndTrackCand.h"
#include "PndCurlingTrackFinderTask.h"
#include "PndCAPreselectSttHits.h"

//#include "PndSttMapCreator.h"
#include "PndStt2GeoHandler.h"

#include "PndCircleTools.h"

#include "TFile.h"
// general
#include <vector>

// -----   Constructor   -------------------------------------------
PndCurlingTrackFinderTask::PndCurlingTrackFinderTask() : FairTask("ApolloniusTripletTrackFinderTask"), fWithTubeReduction(false), fVerbose(1) {}

// -----   Destructor   ----------------------------------------------------
PndCurlingTrackFinderTask::~PndCurlingTrackFinderTask()
{
  if (fFinder != nullptr)
    delete fFinder;
  delete fCATrackFinder;
}
// -----   Private method SetParContainers   -------------------------------

void PndCurlingTrackFinderTask::SetParContainers()
{
  FairRuntimeDb *rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fSttParameters = (PndGeoSttPar *)rtdb->getContainer("PndGeoSttPar");
}

// -----   Public method Init   --------------------------------------------
// This is the initialization method for the PndCurlingTrackFinderTask task.
// It is called once before analyzing all events and loads the main data from the simulation and digitization.
//
// @return     The initialize status.
//
InitStatus PndCurlingTrackFinderTask::Init()
{

  // Get RootManager
  ioman = FairRootManager::Instance();

  if (!ioman) {
    std::cout << "-E- PndCurlingTrackFinderTask::Init: "
              << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  if (fBranchMap.size() == 0) {
    // Use hits of all tracking subsystems if nothing is given
    AddBranchName("MVDHitsPixel");
    AddBranchName("MVDHitsStrip");
    AddBranchName("STTHit");
    AddBranchName("GEMHit");
  }
  if (fBranchMapUnassigned.size() == 0) {
    // Use hits of all tracking subsystems if nothing is given
    AddBranchName("MVDHitsPixelUnassigned");
    AddBranchName("MVDHitsStripUnassigned");
    AddBranchName("STTHitUnassigned");
    AddBranchName("GEMHitUnassigned");
  }

  for (auto branch : fBranchMap) {
    fBranchMap[branch.first] = dynamic_cast<TClonesArray *>(ioman->GetObject(branch.first));
    if (fBranchMap[branch.first] == nullptr) {
      std::cout << "-E- PndCurlingTrackFinderTask::Init() Branch does not exist in Tree: " << branch.first << std::endl;
      fBranchMap.erase(branch.first);
    }
  }
  for (auto branch : fBranchMapUnassigned) {
    fBranchMapUnassigned[branch.first] = dynamic_cast<TClonesArray *>(ioman->GetObject(branch.first));
    if (fBranchMapUnassigned[branch.first] == nullptr) {
      std::cout << "-E- PndCurlingTrackFinderTask::Init() Branch does not exist in Tree: " << branch.first << std::endl;
      fBranchMapUnassigned.erase(branch.first);
    }
  }
  fBranchFoundTrack = dynamic_cast<TClonesArray *>(ioman->GetObject(fBranchNameFoundTrack));
  fBranchFoundTrackCand = dynamic_cast<TClonesArray *>(ioman->GetObject(fBranchNameFoundTrackCand));

  fFinalTrackCandArray = ioman->Register("FinalTrackCand", "PndTrackCand", "tracking", kTRUE);
  fFinalTrackArray = ioman->Register("FinalTrack", "PndTrack", "tracking", kTRUE);
  fTrackCandArray = ioman->Register("ApolloniusTripletTrackCand", "PndTrackCand", "tracking", kTRUE);
  fTrackArray = ioman->Register("ApolloniusTripletTrack", "PndTrack", "tracking", kTRUE);
  fTripletTracksCands = ioman->Register("fTripletTracksCands", "PndTrackCand", "tracking", kTRUE);
  fTripletTracks = ioman->Register("fTripletTracks", "PndTrack", "tracking", kTRUE);
  fContinuousTripletTracksCands = ioman->Register("fContinuousTripletTracksCands", "PndTrackCand", "tracking", kTRUE);
  fContinuousTripletTracks = ioman->Register("fContinuousTripletTracks", "PndTrack", "tracking", kTRUE);
  fTripletTracksCandsAfterAdding = ioman->Register("fTripletTracksCandsAfterAdding", "PndTrackCand", "tracking", kTRUE);
  fTripletTracksAfterAdding = ioman->Register("fTripletTracksAfterAdding", "PndTrack", "tracking", kTRUE);

  // common for old and new geometry
  fSttGeoH = PndStt2GeoHandler::Instance(fSttParameters);
  //fSttGeoH = new PndStt2GeoHandler(fSttParameters);
  //fTubeArray = fSttGeoH->GetTubeArray();
  //PndSttMapCreator *mapper = new PndSttMapCreator(fSttParameters);
  //fTubeArray = mapper->FillTubeArray();

  //fCATrackFinder = new PndSttCA(fTubeArray);
  //fFinder = new PndCurlingTrackFinder(fTubeArray);
  fCATrackFinder = new PndSttCA(fSttGeoH);
  fFinder = new PndCurlingTrackFinder(fSttGeoH);
  fFinder->SetWithTubeReduction(fWithTubeReduction);
  fFinder->SetBranchMap(fBranchMap);

  //fGeometryMap = new PndSttGeometryMap(fTubeArray, 1);

  return kSUCCESS;
}

// -----   Public method Exec   --------------------------------------------
// This is the execution method for the PndCurlingTrackFinderTask task.
// It is the method that evaluates each event.
//
//
void PndCurlingTrackFinderTask::Exec(Option_t *)
{
  if (fVerbose > 0) {
    std::cout << std::endl;
    std::cout << "*** Event: " << ioman->GetEntryNr() << std::endl;
  }
  fFinder->Reset();
  fTrackArray->Delete();
  fTrackCandArray->Delete();
  std::vector<PndSttHit *> UnassignedSttHits;
  UnassignedSttHits.clear();
  std::cout << "STTHitUnassigned hits: " << fBranchMapUnassigned["STTHitUnassigned"]->GetEntriesFast() << std::endl;
  if (fBranchMapUnassigned["STTHitUnassigned"]->GetEntriesFast() < 400) {

    for (auto it = fBranchMap.begin(); it != fBranchMap.end(); it++) {
      for (int i = 0; i < it->second->GetEntries(); i++) {
        if (it->first == "STTHit") {
          PndSttHit *hit = static_cast<PndSttHit *>(it->second->At(i));
          hit->SetEntryNr(FairLink(-1, ioman->GetEntryNr(), ioman->GetBranchId("STTHit"), i));
        } else {
          FairHit *hit = static_cast<FairHit *>(it->second->At(i));
          hit->SetEntryNr(FairLink(-1, ioman->GetEntryNr(), ioman->GetBranchId(it->first), i));
        }
      }
    }
    for (auto it = fBranchMapUnassigned.begin(); it != fBranchMapUnassigned.end(); it++) {
      for (int i = 0; i < it->second->GetEntries(); i++) {
        if (it->first == "STTHitUnassigned") {
          PndSttHit *hit = static_cast<PndSttHit *>(it->second->At(i));
          hit->SetEntryNr(FairLink(-1, ioman->GetEntryNr(), ioman->GetBranchId("STTHitUnassigned"), i));
        } else {
          FairHit *hit = static_cast<FairHit *>(it->second->At(i));
          hit->SetEntryNr(FairLink(-1, ioman->GetEntryNr(), ioman->GetBranchId(it->first), i));
        }
      }
    }

    for (int i = 0; i < fBranchMapUnassigned["STTHitUnassigned"]->GetEntriesFast(); i++) {
      PndSttHit *hit = (PndSttHit *)fBranchMapUnassigned["STTHitUnassigned"]->At(i);
      if (!fSttGeoH->GetTube(hit->GetTubeID())->IsSkew()) {
      //if (!fGeometryMap->IsSkewedStraw(hit->GetTubeID())) {
        for (int j = 0; j < fBranchMap["STTHit"]->GetEntriesFast(); j++) {
          PndSttHit *trueHit = (PndSttHit *)fBranchMap["STTHit"]->At(j);
          if (hit->GetTubeID() == trueHit->GetTubeID()) {
            UnassignedSttHits.push_back(trueHit);
          }
        }
      }
    }

    std::cout << "unassigned hits: " << UnassignedSttHits.size() << std::endl;
    if (UnassignedSttHits.size() < 600) {
      std::vector<PndSttHit *> SttHits;
      SttHits.clear();
      for (int i = 0; i < fBranchMap["STTHit"]->GetEntriesFast(); i++) {
        PndSttHit *hit = (PndSttHit *)fBranchMap["STTHit"]->At(i);
        if (!fSttGeoH->GetTube(hit->GetTubeID())->IsSkew()) {
        //if (!fGeometryMap->IsSkewedStraw(hit->GetTubeID())) {
          SttHits.push_back(hit);
        }
      }
      fCATrackFinder->Reset();
      fCATrackFinder->AddHits(fBranchMap["STTHit"], "STTHit");
      fFinder->SetCATrackFinder(fCATrackFinder);

      fFinder->SetUnassignedSttHits(UnassignedSttHits);
      fFinder->SetSttHits(SttHits);
      fFinder->Reset();
      fFinder->FindTracks();
    }
  }

  std::vector<TripletSolution> solutions = fFinder->GetSolutions();
  std::vector<TripletSolution> solutionsTripletTracks = fFinder->GetTripletTracks();
  std::vector<TripletSolution> solutionsTripletTracksAfterAdding = fFinder->GetTripletTracksAfterAdding();
  std::vector<TripletSolution> solutionsContinuousTripletTracks = fFinder->GetContinuousTripletTracks();
  std::cout << "number of soultions: " << solutions.size() << std::endl;
  FillPndTrack(solutions);
  FillPndTrackDebug(solutionsTripletTracks, fTripletTracks, fTripletTracksCands);
  FillPndTrackDebug(solutionsTripletTracksAfterAdding, fTripletTracksAfterAdding, fTripletTracksCandsAfterAdding);
  FillPndTrackDebug(solutionsContinuousTripletTracks, fContinuousTripletTracks, fContinuousTripletTracksCands);

  fFinalTrackArray->Delete();
  fFinalTrackCandArray->Delete();

  std::cout << "found tracks" << fBranchFoundTrack->GetEntriesFast() << std::endl;
  for (int i = 0; i < fBranchFoundTrack->GetEntriesFast(); i++) {
    PndTrack *track = static_cast<PndTrack *>(fBranchFoundTrack->At(i));
    PndTrackCand trackCand = track->GetTrackCand();
    new ((*fFinalTrackCandArray)[fFinalTrackCandArray->GetEntries()]) PndTrackCand(trackCand);
    new ((*fFinalTrackArray)[fFinalTrackArray->GetEntries()]) PndTrack(*track);
  }
  /*
  std::cout << "found trackCands" << fBranchFoundTrackCand->GetEntriesFast() << std::endl;
  for(int i = 0; i < fBranchFoundTrackCand->GetEntriesFast(); i++){
    PndTrackCand* trackCand = static_cast<PndTrackCand*>(fBranchFoundTrackCand->At(i));
    //PndTrack* track = static_cast<PndTrack*>(fBranchFoundTrack->At(i));
    //new ((*fFinalTrackCandArray)[fFinalTrackCandArray->GetEntries()]) PndTrackCand(trackCand);
    //new ((*fFinalTrackArray)[fFinalTrackArray->GetEntries()]) PndTrack(track);
  }
  */

  for (int i = 0; i < fTrackArray->GetEntriesFast(); i++) {
    PndTrack *track = ((PndTrack *)fTrackArray->At(i));
    PndTrackCand trackCand = track->GetTrackCand();
    new ((*fFinalTrackCandArray)[fFinalTrackCandArray->GetEntries()]) PndTrackCand(trackCand);
    new ((*fFinalTrackArray)[fFinalTrackArray->GetEntries()]) PndTrack(*track);
  }
  std::cout << "number of final tracks: " << fFinalTrackArray->GetEntriesFast() << std::endl;
}

void PndCurlingTrackFinderTask::FillPndTrack(std::vector<TripletSolution> &solutions)
{
  fTrackArray->Delete();
  fTrackCandArray->Delete();

  for (auto solution : solutions) {
    PndTrackCand cand;
    // for (auto hit : solution.fHits[TripletSolution::detID::STT]) {
    //  cand.AddHit(hit->GetEntryNr(), ((PndSttHit *)hit)->GetTubeID()); // todo: do not use tubeID as sorting parameter. Better would be arclength
    //}
    int i = 0;
    // for (auto hit : solution.fHits[TripletSolution::detID::MVDpixel]) {
    for (auto hit : solution.fAllHits) {
      cand.AddHit(hit->GetEntryNr(), i); // todo: do not use tubeID as sorting parameter. Better would be arclength
      i++;
    }
    /*
    i = 0;
    for (auto hit : solution.fHits[TripletSolution::detID::MVDstrip]) {
      cand.AddHit(hit->GetEntryNr(), i); // todo: do not use tubeID as sorting parameter. Better would be arclength
      i++;
    }
    i = 0;
    for (auto hit : solution.fHits[TripletSolution::detID::GEM]) {
      cand.AddHit(hit->GetEntryNr(), i); // todo: do not use tubeID as sorting parameter. Better would be arclength
      i++;
    }
    */
    // std::cout << "PndTrackCand: " << cand << std::endl;
    new ((*fTrackCandArray)[fTrackCandArray->GetEntries()]) PndTrackCand(cand);
    PndTrack track = GetPndTrack(2.0, solution.fAllHits, solution.fTrack, cand);
    // std::cout << "solution track: (" << solution.fTrack.X() << "," << solution.fTrack.Y() << "," << solution.fTrack.Z() << ")" << std::endl;
    // std::cout << "PndTrack: " << track << std::endl;
    new ((*fTrackArray)[fTrackArray->GetEntries()]) PndTrack(track);
  }
}

void PndCurlingTrackFinderTask::FillPndTrackDebug(std::vector<TripletSolution> &solutions, TClonesArray *tracks, TClonesArray *trackCands)
{
  trackCands->Delete();
  tracks->Delete();

  for (auto solution : solutions) {
    PndTrackCand cand;
    /*
    for (auto hit : solution.fHits[TripletSolution::detID::STT]) {
      cand.AddHit(hit->GetEntryNr(), ((PndSttHit *)hit)->GetTubeID()); // todo: do not use tubeID as sorting parameter. Better would be arclength
    }
    */
    int i = 0;
    // for (auto hit : solution.fHits[TripletSolution::detID::MVDpixel]) {
    for (auto hit : solution.fAllHits) {
      cand.AddHit(hit->GetEntryNr(), i); // todo: do not use tubeID as sorting parameter. Better would be arclength
      i++;
    }
    /*
    i = 0;
    for (auto hit : solution.fHits[TripletSolution::detID::MVDstrip]) {
      cand.AddHit(hit->GetEntryNr(), i); // todo: do not use tubeID as sorting parameter. Better would be arclength
      i++;
    }
    i = 0;
    for (auto hit : solution.fHits[TripletSolution::detID::GEM]) {
      cand.AddHit(hit->GetEntryNr(), i); // todo: do not use tubeID as sorting parameter. Better would be arclength
      i++;
    }
    */
    // std::cout << "PndTrackCand: " << cand << std::endl;
    new ((*trackCands)[trackCands->GetEntries()]) PndTrackCand(cand);
    PndTrack track = GetPndTrack(2.0, solution.fAllHits, solution.fTrack, cand);

    // std::cout << "PndTrack: " << track << std::endl;
    new ((*tracks)[tracks->GetEntries()]) PndTrack(track);
  }
}

PndTrack PndCurlingTrackFinderTask::GetPndTrack(double B, std::vector<FairHit *> &hits, TVector3 &circle, PndTrackCand &cand)
{

  if (hits.size() < 4)
    return PndTrack();

  TVector3 hitPos;
  TVector3 hitPosError(0.015, 0.015, 0.015);
  TVector3 mom;
  TVector3 momError(2, 2, 2);
  TVector3 dj(1, 0, 0);
  TVector3 dk(0, 1, 0);
  TVector3 origin(0, 0, 1);

  TVector2 hit2D = PANDA::CircleTools::PositionOnCircle((PndSttHit *)hits[0], circle);
  int direction = PANDA::CircleTools::RotationDirection((PndSttHit *)hits[0], (PndSttHit *)hits[1], circle);
  TVector2 pt = PANDA::CircleTools::PtOnCircle((PndSttHit *)hits[0], circle, direction, 2.0); // todo: replace with correct field

  hitPos.SetXYZ(hit2D.X(), hit2D.Y(), 0);
  mom.SetXYZ(pt.X(), pt.Y(), 0);

  FairTrackParP first(hitPos, mom, hitPosError, momError, direction, origin, dj, dk);

  hit2D = PANDA::CircleTools::PositionOnCircle((PndSttHit *)hits.back(), circle);
  pt = PANDA::CircleTools::PtOnCircle((PndSttHit *)hits.back(), circle, direction, 2.0); // todo: replace with correct field

  hitPos.SetXYZ(hit2D.X(), hit2D.Y(), 0);
  mom.SetXYZ(pt.X(), pt.Y(), 0);

  FairTrackParP last(hitPos, mom, hitPosError, momError, direction, origin, dj, dk);

  return PndTrack(first, last, cand);
}

void PndCurlingTrackFinderTask::Finish()
{
  fTrackArray->Delete();
  fTrackCandArray->Delete();
  fTripletTracks->Delete();
  fTripletTracksCands->Delete();
  fTripletTracksAfterAdding->Delete();
  fTripletTracksCandsAfterAdding->Delete();
  fContinuousTripletTracks->Delete();
  fContinuousTripletTracksCands->Delete();
}

ClassImp(PndCurlingTrackFinderTask);
