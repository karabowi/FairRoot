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
//  PndApolloniusTripletTrackFinderTask
//  Finds Track
/////////////////////////////////////////////////////////////////

/** PndApolloniusTripletTrackFinderTask
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
#include "PndApolloniusTripletTrackFinderTask.h"
#include "PndSegmentationPreselectSttHits.h"
#include "PndCAPreselectSttHits.h"
#include "PndHoughUtilities.h"

//#include "PndSttMapCreator.h"
#include "PndStt2GeoHandler.h"

#include "TFile.h"
// general
#include <vector>

// -----   Constructor   -------------------------------------------
PndApolloniusTripletTrackFinderTask::PndApolloniusTripletTrackFinderTask()
  : FairTask("ApolloniusTripletTrackFinderTask"), fWithTubeReduction(false), fWithCombiReduction(false), fSTT("STTHit")
{
}

// -----   Destructor   ----------------------------------------------------
PndApolloniusTripletTrackFinderTask::~PndApolloniusTripletTrackFinderTask()
{
  if (fFinder != nullptr)
    delete fFinder;
  delete fCATrackFinder;
}
// -----   Private method SetParContainers   -------------------------------

void PndApolloniusTripletTrackFinderTask::SetParContainers()
{
  FairRuntimeDb *rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fSttParameters = (PndGeoSttPar *)rtdb->getContainer("PndGeoSttPar");
}

// -----   Public method Init   --------------------------------------------
// This is the initialization method for the PndApolloniusTripletTrackFinderTask task.
// It is called once before analyzing all events and loads the main data from the simulation and digitization.
//
// @return     The initialize status.
//
InitStatus PndApolloniusTripletTrackFinderTask::Init()
{

  // Get RootManager
  ioman = FairRootManager::Instance();

  if (!ioman) {
    LOG(error) << "-E- PndApolloniusTripletTrackFinderTask::Init: "
               << "RootManager not instantiated!";
    return kFATAL;
  }

  if (fBranchMap.size() == 0) {
    // Use hits of all tracking subsystems if nothing is given
    AddBranchName("MVDHitsPixel");
    AddBranchName("MVDHitsStrip");
    AddBranchName("STTHit");
    AddBranchName("STTCombinedSkewedHits");
    AddBranchName("GEMHit");
  }

  for (auto branch : fBranchMap) {
    fBranchMap[branch.first] = dynamic_cast<TClonesArray *>(ioman->GetObject(branch.first));
    if (fBranchMap[branch.first] == nullptr) {
      LOG(error) << "-E- PndApolloniusTripletTrackFinderTask::Init() Branch does not exist in Tree: " << branch.first;
      fBranchMap.erase(branch.first);
    }
  }
  fBranchFoundTrack = (TClonesArray *)(ioman->GetObject(fBranchNameFoundTrack));
  fSttBranchOriginal = (TClonesArray *)(ioman->GetObject(fSTT));

  if (fBranchFoundTrack == nullptr) {
    LOG(error) << "-E- PndApolloniusTripletTrackFinderTask::Init() Branch does not exist: " << fBranchNameFoundTrack;
  }

  if (fSttBranchOriginal == nullptr) {
    LOG(error) << "-E- PndApolloniusTripletTrackFinderTask::Init() Branch does not exist: " << fSTT;
  }

  FairField *Field = FairRunAna::Instance()->GetField();
  Double_t po[3], BB[3];
  po[0] = 0.;
  po[1] = 0.;
  po[2] = 0.;
  Field->GetFieldValue(po, BB);
  fBz = BB[2] / 10.;

  fTrackCandArray = ioman->Register("ApolloniusTripletTrackCand", "PndTrackCand", "tracking", kTRUE);
  fTrackArray = ioman->Register("ApolloniusTripletTrack", "PndTrack", "tracking", kTRUE);

#ifdef DEBUGApolloniusTripletFinder
  fPreselectedTracksCands = ioman->Register("fPreselectedTracksCands", "PndTrackCand", "tracking", kTRUE);
  fPreselectedTracks = ioman->Register("fPreselectedTracks", "PndTrack", "tracking", kTRUE);
  fTripletsFirstCands = ioman->Register("fTripletsFirstCands", "PndTrackCand", "tracking", kTRUE);
  fTripletsFirst = ioman->Register("fTripletsFirst", "PndTrack", "tracking", kTRUE);
  fTripletsMidCands = ioman->Register("fTripletsMidCands", "PndTrackCand", "tracking", kTRUE);
  fTripletsMid = ioman->Register("fTripletsMid", "PndTrack", "tracking", kTRUE);
  fTripletsLastCands = ioman->Register("fTripletsLastCands", "PndTrackCand", "tracking", kTRUE);
  fTripletsLast = ioman->Register("fTripletsLast", "PndTrack", "tracking", kTRUE);
  fTripletsCombiCands = ioman->Register("fTripletsCombiCands", "PndTrackCand", "tracking", kTRUE);
  fTripletsCombi = ioman->Register("fTripletsCombi", "PndTrack", "tracking", kTRUE);
  fTripletTracksCands = ioman->Register("fTripletTracksCands", "PndTrackCand", "tracking", kTRUE);
  fTripletTracks = ioman->Register("fTripletTracks", "PndTrack", "tracking", kTRUE);
  fContinuousTripletTracksCands = ioman->Register("fContinuousTripletTracksCands", "PndTrackCand", "tracking", kTRUE);
  fContinuousTripletTracks = ioman->Register("fContinuousTripletTracks", "PndTrack", "tracking", kTRUE);
  fCombinedTripletTracksCands = ioman->Register("fCombinedTripletTracksCands", "PndTrackCand", "tracking", kTRUE);
  fCombinedTripletTracks = ioman->Register("fCombinedTripletTracks", "PndTrack", "tracking", kTRUE);
  fTripletTracksCandsAfterAdding = ioman->Register("fTripletTracksCandsAfterAdding", "PndTrackCand", "tracking", kTRUE);
  fTripletTracksAfterAdding = ioman->Register("fTripletTracksAfterAdding", "PndTrack", "tracking", kTRUE);
#endif

  //PndSttMapCreator *mapper = new PndSttMapCreator(fSttParameters);
  //fTubeArray = mapper->FillTubeArray();

  if (fPreselector == nullptr) {
    PndSegmentationPreselectSttHits *segPre = new PndSegmentationPreselectSttHits();

    if (fBranchMap.count("STTCombinedSkewedHits") > 0) {
      segPre->SetCombinedSkewed(fBranchMap["STTCombinedSkewedHits"]);
    } else {
      std::cout << "STTCombinedSkewedHits not Found!";
      return kFATAL;
    }
    fPreselector = segPre;
  }

  //  if (fPreselector == nullptr) {
  //    PndCAPreselectSttHits *segPre = new PndCAPreselectSttHits();
  //
  //    //    if (fBranchMap.count("STTCombinedSkewedHits") > 0) {
  //    //      segPre->SetCombinedSkewed(fBranchMap["STTCombinedSkewedHits"]);
  //    //    } else {
  //    //      std::cout << "STTCombinedSkewedHits not Found!";
  //    //      return kFATAL;
  //    //    }
  //    fPreselector = segPre;
  //  }

  // common for old and new geometry
  fSttGeoH = PndStt2GeoHandler::Instance(fSttParameters);
  //fSttGeoH = new PndStt2GeoHandler(fSttParameters);
  //fStrawMap = new PndSttStrawMap(fTubeArray);
  //fPreselector->Init(fBranchMap[fSTT], fTubeArray);
  //fCATrackFinder = new PndSttCA(fTubeArray);
  //fFinder = new PndApolloniusTripletTrackFinder(fTubeArray);
  fPreselector->Init(fBranchMap[fSTT], fSttGeoH);
  fCATrackFinder = new PndSttCA(fSttGeoH);
  fFinder = new PndApolloniusTripletTrackFinder(fSttGeoH);
  
  fFinder->SetPreselector(fPreselector);
  fFinder->SetWithTubeReduction(fWithTubeReduction);
  fFinder->SetBranchMap(fBranchMap);
  fFinder->SetWithCombiReduction(fWithCombiReduction);
  fFinder->SetSTTBranchOriginal(fSttBranchOriginal);
  fFinder->SetSTTName(fSTT);

  return kSUCCESS;
}

// -----   Public method Exec   --------------------------------------------
// This is the execution method for the PndApolloniusTripletTrackFinderTask task.
// It is the method that evaluates each event.
//
//
void PndApolloniusTripletTrackFinderTask::Exec(Option_t *)
{
  LOG(debug) << "*** Event: " << ioman->GetEntryNr();

  for (auto it = fBranchMap.begin(); it != fBranchMap.end(); it++) {
    LOG(debug) << it->first << " : ";
    for (int i = 0; i < it->second->GetEntries(); i++) { // TODO: fix in hit creators
      FairHit *hit = static_cast<FairHit *>(it->second->At(i));
      if (fBranchNameFoundTrack == "") {
        hit->SetEntryNr(FairLink(-1, ioman->GetEntryNr(), ioman->GetBranchId(it->first), i));
        LOG(debug) << i << " : " << hit->GetX() << "/" << hit->GetY() << "/" << hit->GetZ();
      }
    }
  }
  if (fWithTubeReduction) {
    fCATrackFinder->Reset();
    fCATrackFinder->AddHits(fBranchMap[fSTT], fSTT);
    fFinder->SetCATrackFinder(fCATrackFinder);
  }

  fFinder->Reset();

  fFinder->FindTracks();

  std::vector<TripletSolution> solutions = fFinder->GetSolutions();
  FillPndTrack(solutions);

#ifdef DEBUGApolloniusTripletFinder
  std::vector<TripletSolution> solutionsPreselectedTracks = fFinder->GetPreselectedTracks();
  std::vector<TripletSolution> solutionsTripletsFirst = fFinder->GetTripletsFirst();
  std::vector<TripletSolution> solutionsTripletsMid = fFinder->GetTripletsMid();
  std::vector<TripletSolution> solutionsTripletsLast = fFinder->GetTripletsLast();
  std::vector<TripletSolution> solutionsTripletsCombi = fFinder->GetTripletsCombi();
  std::vector<TripletSolution> solutionsTripletTracks = fFinder->GetTripletTracks();
  std::vector<TripletSolution> solutionsTripletTracksAfterAdding = fFinder->GetTripletTracksAfterAdding();
  std::vector<TripletSolution> solutionsContinuousTripletTracks = fFinder->GetContinuousTripletTracks();
  std::vector<TripletSolution> solutionsCombinedTripletTracks = fFinder->GetCombinedSolutions();
  std::cout << "solutionsTripletTracksAfterAdding: " << solutionsTripletTracksAfterAdding.size() << std::endl;
  FillPndTrackDebug(solutionsPreselectedTracks, fPreselectedTracks, fPreselectedTracksCands);
  FillPndTrackDebug(solutionsTripletsFirst, fTripletsFirst, fTripletsFirstCands);
  FillPndTrackDebug(solutionsTripletsMid, fTripletsMid, fTripletsMidCands);
  FillPndTrackDebug(solutionsTripletsLast, fTripletsLast, fTripletsLastCands);
  FillPndTrackDebug(solutionsTripletsCombi, fTripletsCombi, fTripletsCombiCands);
  FillPndTrackDebug(solutionsTripletTracks, fTripletTracks, fTripletTracksCands);
  FillPndTrackDebug(solutionsTripletTracksAfterAdding, fTripletTracksAfterAdding, fTripletTracksCandsAfterAdding);
  FillPndTrackDebug(solutionsContinuousTripletTracks, fContinuousTripletTracks, fContinuousTripletTracksCands);
  FillPndTrackDebug(solutionsCombinedTripletTracks, fCombinedTripletTracks, fCombinedTripletTracksCands);
#endif
}

void PndApolloniusTripletTrackFinderTask::FillPndTrack(std::vector<TripletSolution> &solutions)
{
  fTrackArray->Delete();
  fTrackCandArray->Delete();
  PndApollonius::ApolloniusTripletFunctions fFunc;
  if (fBranchNameFoundTrack != "") {

    // merger: merges tracks which are similar to tracks previously found
    // currently no merger! included -> using only circle information is not  sufficient
    std::vector<int> additionallyFoundSolutions;
    PndHoughUtilities *fPndHoughUtilities = new PndHoughUtilities();
    std::map<int, std::vector<int>> mergeOldSolutionsWithNew;
    for (int j = 0; j < solutions.size(); j++) {
      bool NewSolutionWasMerged = false;
      if (fBranchFoundTrack == nullptr) {
        LOG(error) << "-E- PndApolloniusTripletTrackFinderTask::FillPndTrack() Branch does not exist: " << fBranchNameFoundTrack;
      }
      /*
      for (int i = 0; i < fBranchFoundTrack->GetEntriesFast(); i++) {
        PndTrack *track = (PndTrack *)(fBranchFoundTrack->At(i));
        PndTrackCand trackCand = track->GetTrackCand();
        std::vector<double> circle = fPndHoughUtilities->getCircleFromPndTrack(*track);

        double d =
          sqrt((circle[0] - solutions[j].fTrack.X()) * (circle[0] - solutions[j].fTrack.X()) + (circle[1] - solutions[j].fTrack.Y()) * (circle[1] - solutions[j].fTrack.Y()));

        if (d < 7.) {
          NewSolutionWasMerged = true;
          if (mergeOldSolutionsWithNew.find(i) != mergeOldSolutionsWithNew.end()) {
            mergeOldSolutionsWithNew[i].push_back(j);
          } else {
            mergeOldSolutionsWithNew[i] = {j};
          }
        }
      }*/
      if (!NewSolutionWasMerged) {
        additionallyFoundSolutions.push_back(j);
      }
    }
    for (int i = 0; i < fBranchFoundTrack->GetEntriesFast(); i++) {
      if (fBranchFoundTrack == nullptr) {
        LOG(error) << "-E- PndApolloniusTripletTrackFinderTask::FillPndTrack() Branch does not exist: " << fBranchNameFoundTrack;
      }
      PndTrack *track = (PndTrack *)(fBranchFoundTrack->At(i));
      PndTrackCand trackCand = track->GetTrackCand();
      /*
      if (mergeOldSolutionsWithNew.find(i) != mergeOldSolutionsWithNew.end()) {
        for (int j = 0; j < mergeOldSolutionsWithNew[i].size(); j++) {
          for (auto hit : solutions[mergeOldSolutionsWithNew[i][j]].fAllHits) {
            FairLink link = hit->GetEntryNr();

            std::vector<PndTrackCandHit> tcHits = trackCand.GetSortedHits();
            auto it = std::find_if(tcHits.begin(), tcHits.end(), [&link](const PndTrackCandHit &obj) { return (FairLink)obj == link; });

            if (it == tcHits.end()) {
              trackCand.AddHit(hit->GetEntryNr(), trackCand.GetNHits());
            }
          }
          if (trackCand.GetNHits() < solutions[mergeOldSolutionsWithNew[i][j]].fAllHits.size()) {

            PndTrack track = fFunc.FromTripletSolutionToPndTrack(solutions[mergeOldSolutionsWithNew[i][j]], fBz, trackCand);
            new ((*fTrackCandArray)[fTrackCandArray->GetEntries()]) PndTrackCand(track.GetTrackCand());
            new ((*fTrackArray)[fTrackArray->GetEntries()]) PndTrack(track);
          } else {
            track->SetTrackCand(trackCand);
            new ((*fTrackCandArray)[fTrackCandArray->GetEntries()]) PndTrackCand(trackCand);
            new ((*fTrackArray)[fTrackArray->GetEntries()]) PndTrack(*track);
          }
        }
      } else {*/
      new ((*fTrackCandArray)[fTrackCandArray->GetEntries()]) PndTrackCand(trackCand);
      new ((*fTrackArray)[fTrackArray->GetEntries()]) PndTrack(*track);
    }
    for (int n : additionallyFoundSolutions) {
      PndTrack track = fFunc.FromTripletSolutionToPndTrack(solutions[n], fBz);
      PndTrackCand cand = track.GetTrackCand();
      if (!CheckZInfo(cand))
        continue;

      new ((*fTrackCandArray)[fTrackCandArray->GetEntries()]) PndTrackCand(track.GetTrackCand());
      new ((*fTrackArray)[fTrackArray->GetEntries()]) PndTrack(track);
    }
  } else {
    for (auto solution : solutions) {
      PndTrack track = fFunc.FromTripletSolutionToPndTrack(solution, fBz);
      new ((*fTrackCandArray)[fTrackCandArray->GetEntries()]) PndTrackCand(track.GetTrackCand());
      new ((*fTrackArray)[fTrackArray->GetEntries()]) PndTrack(track);
    }
  }
}

void PndApolloniusTripletTrackFinderTask::FillPndTrackDebug(std::vector<TripletSolution> &solutions, TClonesArray *tracks, TClonesArray *trackCands)
{
  PndApollonius::ApolloniusTripletFunctions fFunc;
  trackCands->Delete();
  tracks->Delete();

  for (auto solution : solutions) {
    PndTrack track = fFunc.FromTripletSolutionToPndTrack(solution, fBz);

    new ((*trackCands)[trackCands->GetEntries()]) PndTrackCand(track.GetTrackCand());
    new ((*tracks)[tracks->GetEntries()]) PndTrack(track);
  }
}

bool PndApolloniusTripletTrackFinderTask::CheckZInfo(PndTrackCand &cand)
{
  int MvdGemCounter = 0;
  int SkewedCounter = 0;
  for (int i = 0; i < cand.GetNHits(); i++) {
    FairLink link = cand.GetSortedHit(i);
    if (ioman->GetBranchName(link.GetType()) == "MVDHitsPixel" || ioman->GetBranchName(link.GetType()) == "MVDHitsStrip" || ioman->GetBranchName(link.GetType()) == "GEMHit")
      MvdGemCounter++;
    else {
      Int_t tubeID = ((PndSttHit *)ioman->GetCloneOfLinkData(link))->GetTubeID();
      LOG(debug) << "tubeID: " << tubeID << " is skewed? " << fSttGeoH->IsSkewedStraw(tubeID) << std::endl;
      //LOG(debug) << "tubeID: " << tubeID << " is skewed? " << fStrawMap->IsSkewedStraw(tubeID) << std::endl;
      if (fSttGeoH->IsSkewedStraw(tubeID))
      //if (fStrawMap->IsSkewedStraw(tubeID))
        SkewedCounter++;
    }
  }
  LOG(debug) << "event: " << ioman->GetEntryNr() << " MVD+GEM: " << MvdGemCounter << ", skewed: " << SkewedCounter << std::endl;
  if (MvdGemCounter >= 2 || MvdGemCounter + SkewedCounter / 2 >= 2)
    return true;

  return false;
}

void PndApolloniusTripletTrackFinderTask::Finish()
{
  fTrackArray->Delete();
  fTrackCandArray->Delete();

#ifdef DEBUGApolloniusTripletFinder
  fPreselectedTracks->Delete();
  fPreselectedTracksCands->Delete();
  fTripletsFirst->Delete();
  fTripletsFirstCands->Delete();
  fTripletsMid->Delete();
  fTripletsMidCands->Delete();
  fTripletsLast->Delete();
  fTripletsLastCands->Delete();
  fTripletsCombi->Delete();
  fTripletsCombiCands->Delete();
  fTripletTracks->Delete();
  fTripletTracksCands->Delete();
  fTripletTracksAfterAdding->Delete();
  fTripletTracksCandsAfterAdding->Delete();
  fContinuousTripletTracks->Delete();
  fContinuousTripletTracksCands->Delete();
  fCombinedTripletTracks->Delete();
  fCombinedTripletTracksCands->Delete();
  fTripletTracksCandsAfterAdding->Delete();
  fTripletTracksAfterAdding->Delete();
#endif
}

ClassImp(PndApolloniusTripletTrackFinderTask);
