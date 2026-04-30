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

#include "PndSTETask.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <chrono>

// Root includes
#include "TROOT.h"
#include "TString.h"
#include "TClonesArray.h"
//#include "TParticlePDG.h"

// framework includes
#include "FairRootManager.h"

#include "PndTrackCand.h"
#include "PndTrack.h"
#include "PndRiemannTrack.h"

// SttTrackExtrapolatorTaskIncludes
#include "PndSTETrackCreator.h"

using std::cout;
using std::endl;
using namespace std::chrono;

ClassImp(PndSTETask);

InitStatus PndSTETask::Init()
{

  FairRootManager *ioman = FairRootManager::Instance();

  if (!ioman) {
    LOG(debug) << "-E- PndSTETask::Init: "
               << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  fEventHeader = (TClonesArray *)ioman->GetObject("EventHeader.");
  if (!fEventHeader) {
    LOG(debug) << "-W-  PndSTETask::Init: No EventHeader array! Needed for EventNumber" << endl;
    return kERROR;
  }

  InitTrackArray(fInputTrackBranchName);

  if (fInputRiemannTrackBranchName != "NoNameGiven") {
    InitRiemannTrackArray(fInputRiemannTrackBranchName);
  }

  InitTrackCandArray(fInputTrackCandBranchName);

  // Reading in track
  if (fPndTrackArray.size() == 0) {
    LOG(debug) << "No InputBranches containing PndTrack data are initialized for the PndSTETask" << endl;
    return kERROR;
  }

  // Initiate the branch arrays for the chosen branch names

  fHitBranchName.clear();

  if (fIncludeMvd) {
    // TODO replace with input branch names
    /** @brief Standard input branch names are used */
    fHitBranchName.push_back("MVDHitsPixel");
    fHitBranchName.push_back("MVDHitsStrip");
    /** @brief The names below can be activated if a sorting of hits unsing the
     * PndGapEventBuilder is used prior to the tracking */
    // fHitBranchName.push_back("MVDSortedPixelDigis_event");
    // fHitBranchName.push_back("MVDSortedStripDigis_event");
  }

  if (fIncludeGem) {
    // TODO replace with input branch names
    /** @brief Standard input branch names are used */
    fHitBranchName.push_back("GEMHit");
  }
  if (fIncludeBtof) {
    // TODO replace with input branch names
    /** @brief Standard input branch names are used */
    fHitBranchName.push_back("SciTHit");
  }
  if (fHitBranchName.size() == 0) { // If true there is no input hit branch
    std::cout << "-E- PndSTETask::Init: "
              << "No Input Hit Branch!" << std::endl;
    return kERROR;
  }

  for (int i = 0; i < (int)fHitBranchName.size(); i++) {

    InitHitArray(fHitBranchName[i]);
  }

  /** @brief Register the output branches, only one PndTrack and one PndTrackCand containing all the hits which have been added from all detectors */
  fOutTrackCandArray = ioman->Register(fOutBranchNamePrefix + "TrackCand", "PndTrackCand", "SttTrackExtrapolator", fPersistence);
  fOutTrackArray = ioman->Register(fOutBranchNamePrefix + "Track", "PndTrack", "SttTrackExtrapolator", fPersistence);
  fOutRiemannTrackArray = ioman->Register(fOutBranchNamePrefix + "RiemannTrack", "PndRiemannTrack", "SttTrackExtrapolator", fPersistence);

  return kSUCCESS;
}

// -----   Public method Exec   --------------------------------------------
void PndSTETask::Exec(Option_t *)
{

  LOG(debug) << "------ PndSTETask::Exec() ------" << endl;

  // Filter out too short tracks
  // if (fRemoveShortTracks == true) {

  // The function below is currently not used but is planned to be in the future
  // FilterTracks(fPndTrackArray, fPndTrackCandArray, fPndRiemannTrackArray);
  // LOG(debug) << "Filtered Array size() " << fPndTrackFilteredArray->GetEntries() << endl;
  //}

  /** @brief The line below is for timing information */
  auto start_time = std::chrono::system_clock::now();

  PndSTETrackCreator *trackCreator = new PndSTETrackCreator();
  trackCreator->SetMagneticField();
  trackCreator->IncludeDetector(fIncludeMvd, fIncludeGem, fIncludeBtof);
  trackCreator->SetUseHelix(fUseHelix);
  trackCreator->SetUseIdealTrack(fIdealTrack);
  trackCreator->SetMvdHitDist(fMvdHitDist);
  trackCreator->SetGemHitDist(fGemHitDist);
  trackCreator->SetBtofHitDist(fBtofHitDist);
  trackCreator->SetUseHemisphere(fUseHemisphere);
  trackCreator->SetNoAreaExclusion(fNoMVDAreaExclusion);
  if (fWeightMVD > 0) {
    trackCreator->SetWeightsMvdHit(fWeightMVD);
  }
  trackCreator->SetRunIn3D(fRunIn3D);
  trackCreator->SetDrawTracks(fDrawTrack);

  /** @brief The following statement is to make sure that not both functions are used */
  if (fUseHelix == true && fUseRiemann == true) {
    fUseHelix = false;
  }
  if (fUseHelix == false && fUseRiemann == false) {
    fUseRiemann = true;
  }

  if (fIncludeMvd == true) {
    trackCreator->AddHitsToMvdTrack(fPndTrackArray.back(), fPndRiemannTrackArray.back(), fPndTrackCandArray.back(), fMvdHitsPixelArray.back(), fMvdHitsStripArray.back());
  }
  if (fIncludeGem == true) {
    trackCreator->AddHitsToGemOrBtofTrack(fPndTrackArray.back(), fPndTrackCandArray.back(), fGemHitsArray.back());
  }
  if (fIncludeBtof == true) {
    trackCreator->AddHitsToGemOrBtofTrack(fPndTrackArray.back(), fPndTrackCandArray.back(), fBtofHitsArray.back());
  }

  for (int i = 0; i < trackCreator->NumRiemannTracks(); i++) {
    new ((*fOutRiemannTrackArray)[i]) PndRiemannTrack(trackCreator->GetRiemannTrack(i));
  }
  for (int i = 0; i < trackCreator->NumTrackCands(); i++) {

    PndTrackCand *myCand = new ((*fOutTrackCandArray)[i]) PndTrackCand(trackCreator->GetTrackCand(i));
    PndTrack *myTrack = new ((*fOutTrackArray)[i]) PndTrack(trackCreator->GetTrack(i));
    myTrack->SetTrackCandRef(myCand);
    myTrack->SetTrackCand(*myCand);
  }

  /** @brief For timing information */
  auto end_time = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end_time - start_time;
  fSumTime += elapsed_seconds.count();

  LOG(debug) << "Total time so far: " << fSumTime << endl;
  /** @brief End of code block for timing information */
}

void PndSTETask::InitHitArray(TString branchName)
{

  // In this function the user can choose to initiate a certain type of hits, works for MvdHits, GemHits and barrel ToF hits
  // The function can be called up to four times depending on which branches are being used

  TClonesArray *tempArray = (TClonesArray *)FairRootManager::Instance()->GetObject(branchName);

  if (branchName.Contains("Pixel")) {
    fMvdHitsPixelArray.push_back(tempArray);

    cout << "InitHitArray with branch hit name: " << branchName << endl;
  }
  if (branchName.Contains("Strip")) {
    fMvdHitsStripArray.push_back(tempArray);

    cout << "InitHitArray with branch hit name: " << branchName << endl;
  }
  if (branchName.Contains("GEM")) {
    fGemHitsArray.push_back(tempArray);

    cout << "InitHitArray with branch hit name: " << branchName << endl;
  }
  if (branchName.Contains("Sci")) {
    fBtofHitsArray.push_back(tempArray);

    cout << "InitHitArray with branch hit name: " << branchName << endl;
  }
}

void PndSTETask::InitTrackArray(TString branchName)
{
  // In this function the user can choose to initiate the chosen type of track, e.g. a PndTrack, RiemannTrack etc...

  TClonesArray *tempArray = (TClonesArray *)FairRootManager::Instance()->GetObject(branchName);

  cout << "InitTrackArray with branch track name: " << branchName << endl;

  fPndTrackArray.push_back(tempArray);
}

void PndSTETask::InitRiemannTrackArray(TString branchName)
{
  // In this function the user can choose to initiate the chosen type of track, e.g. a PndTrack, RiemannTrack etc...

  TClonesArray *tempArray = (TClonesArray *)FairRootManager::Instance()->GetObject(branchName);

  cout << "InitRiemannTrackArray with branch track name: " << branchName << endl;

  fPndRiemannTrackArray.push_back(tempArray);
}

void PndSTETask::InitTrackCandArray(TString branchName)
{

  TClonesArray *tempArray = (TClonesArray *)FairRootManager::Instance()->GetObject(branchName);

  cout << "InitTrackCandArray with branch track name: " << branchName << endl;

  fPndTrackCandArray.push_back(tempArray);
}

// TODO: Finalize function below to filter out tracks with too few hits
// void PndSTETask::FilterTracks(std::vector<TClonesArray *> trackArr, std::vector<TClonesArray *> trackCandArr, std::vector<TClonesArray *> riemannTrackArr)
// {

//   // Must remove PndTracks, PndTrackCands and PndRiemannTracks
//   // If the reack contain less than 15 hits there is a big chance that the momentum resolution is not good enough for performing an extrapolation

//   fPndTrackFilteredArray = new TClonesArray();

//   PndTrack *track = nullptr;

//   int counter = 0;

//   for (int num_tracks = 0; num_tracks < trackArr.back()->GetEntries(); num_tracks++) {

//     LOG(debug) << "Number of tracks " << num_tracks << endl;

//     track = (PndTrack *)trackArr.back()->At(num_tracks);

//     if (track == nullptr)
//       continue;

//     FairMultiLinkedData links = track->GetLinksWithType(FairRootManager::Instance()->GetBranchId("STTHit"));

//     if (links.GetNLinks() > 15) {

//       LOG(debug) << "Counter " << counter << endl;

//       counter++;
//     }
//   }
// }

void PndSTETask::FinishEvent()
{

  fOutTrackArray->Delete();
  fOutRiemannTrackArray->Delete();

  if (fOutTrackCandArray->GetSize() > 0) {
    fOutTrackCandArray->Delete();
  }
}
