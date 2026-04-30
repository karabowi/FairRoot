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
 * PndAnalysisForwardTask.cpp
 *
 *  Created on: Jul 15, 2016
 *      Author: kibellus
 */

#include "PndAnalysisForwardTask.h"

PndAnalysisForwardTask::~PndAnalysisForwardTask()
{
  // TODO Auto-generated destructor stub
}

InitStatus PndAnalysisForwardTask::Init()
{
  fIoman = FairRootManager::Instance();
  fHits = (TClonesArray *)fIoman->GetObject("FTSHit");
  fSolution = (TClonesArray *)fIoman->GetObject("FinalSolution");
  fMCTracks = (TClonesArray *)fIoman->GetObject("MCTrack");
  fIdealTrack = (TClonesArray *)fIoman->GetObject("IdealTrack");
  return kSUCCESS;
}

void PndAnalysisForwardTask::Exec(Option_t *)
{
  cout << "####################  Event nr " << eventNum << "  ####################" << endl;

  // get the data
  map<Int_t, vector<PndFtsHit *>> mcToHits = getMcTracks();
  map<Int_t, vector<PndFtsHit *> *> hitsToTrack = getReconstructedTracks();

  // analyze the tracks
  cout << "Found " << mcToHits.size() << " MC-Tracks" << endl;
  for (map<Int_t, vector<PndFtsHit *>>::iterator i = mcToHits.begin(); i != mcToHits.end(); i++) {
    Int_t mcTrackID = i->first;
    vector<PndFtsHit *> hits = i->second;
    cout << "-----------  Analyze MC-Track " << i->first << "  -----------" << endl;
    analyzeMCTracks(hits, hitsToTrack, mcTrackID, mcToHits.size());
  }

  anaSpecialCases(mcToHits);
  eventNum++;
}

void PndAnalysisForwardTask::FinishEvent()
{
  fHits->Delete();
}

void PndAnalysisForwardTask::Finish()
{
  cout << "--------------------- Quality Results: ----------------------" << endl;
  Int_t trackCount = completeClean + completeUnclean + incompleteClean + incompleteUnclean + notFound + notEnoughFound;
  Double_t completeCleanRel = ((Double_t)completeClean) / trackCount * 100;
  Double_t completeUncleanRel = ((Double_t)completeUnclean) / trackCount * 100;
  Double_t incompleteCleanRel = ((Double_t)incompleteClean) / trackCount * 100;
  Double_t incompleteUncleanRel = ((Double_t)incompleteUnclean) / trackCount * 100;
  Double_t notFoundRel = ((Double_t)notFound) / trackCount * 100;
  Double_t notEnoughFoundRel = ((Double_t)notEnoughFound) / trackCount * 100;
  Double_t foundTracksRel = 100 - notFoundRel - notEnoughFoundRel;
  cout << "Tracks: " << trackCount << endl;
  cout << "Found " << trackCount - notFound - notEnoughFound << " Tracks (" << foundTracksRel << "%)" << endl;
  cout << "	Found complete and clean     : " << completeClean << " (" << completeCleanRel << "%)" << endl;
  cout << "	Found complete and unclean   : " << completeUnclean << " (" << completeUncleanRel << "%)" << endl;
  cout << "	Found incomplete and clean   : " << incompleteClean << " (" << incompleteCleanRel << "%)" << endl;
  cout << "	Found incomplete and unclean : " << incompleteUnclean << " (" << incompleteUncleanRel << "%)" << endl;
  cout << "Not found(under 70%): " << notEnoughFound << " (" << notEnoughFoundRel << "%)" << endl;
  cout << "Not found : " << notFound << " (" << notFoundRel << "%)" << endl;

  cout << "correlation:" << endl;
  cout << "momentum:" << endl;
  for (int i = 0; i < 10; i++) {
    cout << momentumArray[i] << "  ";
  }
  cout << endl;
  cout << "hits:" << endl;
  for (int i = 0; i < 10; i++) {
    cout << hitArray[i] << "  ";
  }
  cout << endl;
  cout << "TrackCount:" << endl;
  for (int i = 0; i < 10; i++) {
    cout << trackCountArray[i] << "  ";
  }
  cout << endl;

  cout << "sum of all tracks:" << endl;
  cout << "momentum:" << endl;
  for (int i = 0; i < 10; i++) {
    cout << momentumArray2[i] << "  ";
  }
  cout << endl;
  cout << "hits:" << endl;
  for (int i = 0; i < 10; i++) {
    cout << hitArray2[i] << "  ";
  }
  cout << endl;
  cout << "TrackCount:" << endl;
  for (int i = 0; i < 10; i++) {
    cout << trackCountArray2[i] << "  ";
  }
  cout << endl;

  // write to file
  ofstream ofs;
  ofs.open("results.csv", ios::app);

  ofs << trackCount << ";";
  ofs << completeClean << ";";
  ofs << completeUnclean << ";";
  ofs << incompleteClean << ";";
  ofs << incompleteUnclean << ";";
  ofs << notEnoughFound << ";";
  ofs << notFound << ";" << endl;

  Double_t ohRel = 100 * ((Double_t)oneHitPerLayer) / trackCount;
  Double_t mhRel = 100 * ((Double_t)moreThan3Hits) / trackCount;
  Double_t mtRel = 100 * ((Double_t)twoTracksOneTube) / trackCount;
  cout << "Only one Hit per layer: " << oneHitPerLayer << ": (" << ohRel << "%)" << endl;
  cout << "More than 2 Hits per layer: " << moreThan3Hits << ": (" << mhRel << "%)" << endl;
  cout << "TwoTracksOneTube " << twoTracksOneTube << ": (" << mtRel << "%)" << endl;
}

map<Int_t, vector<PndFtsHit *>> PndAnalysisForwardTask::getMcTracks()
{
  Int_t branchID = fIoman->GetBranchId("FTSHit"); // right hits
  // Int_t mcTrackBranchId = fIoman->GetBranchId("MCTrack"); //[R.K.03/2017] unused variable
  map<Int_t, vector<PndFtsHit *>> mcTracksToHits;
  mcToHitCount.clear();
  momentum.clear();
  // cout << "Found " << fHits->GetEntries() << " FTSHits " << endl;
  /*for(int i=0;i<fHits->GetEntries();i++){
    PndFtsHit* hit = (PndFtsHit*) fHits->At(i);
    hit->SetEntryNr(FairLink(-1, fIoman->GetEntryNr(), fIoman->GetBranchId("FTSHit"), i));
    FairLink link = hit->GetEntryNr();
    PndFtsPoint* p = (PndFtsPoint*) fIoman->GetCloneOfLinkData(link);
    FairMultiLinkedData mld = p->GetLinksWithType(mcTrackBranchId);
    FairLink linksToMc = mld.GetLink(0);
    int trackID = linksToMc.GetIndex();
    mcTracksToHits[trackID].push_back(hit);
  }*/

  for (int i = 0; i < fIdealTrack->GetEntries(); i++) {
    PndTrack *t = (PndTrack *)fIdealTrack->At(i);
    Double_t mom = t->GetParamFirst().GetMomentum().Mag();
    momentum[i] = mom;
    // if(mom < 0.3) continue;
    PndTrackCand cand = t->GetTrackCand();
    set<FairLink> linksToHits = cand.GetLinksWithType(branchID).GetLinks();
    mcToHitCount[i] = linksToHits.size();
    for (std::set<FairLink>::iterator it = linksToHits.begin(); it != linksToHits.end(); it++) {
      FairLink link = *it;
      PndFtsHit *hit = (PndFtsHit *)fIoman->GetCloneOfLinkData(link);
      mcTracksToHits[i].push_back(hit);
    }
  }

  // get the tracks with more than 6 hits
  /*map<Int_t,vector<PndFtsHit*>> result;
  for(map<Int_t,vector<PndFtsHit*>>::iterator i=mcTracksToHits.begin();i!=mcTracksToHits.end();i++){
    if(i->second.size()>=6){ //min 1 FTS-Station
      result[i->first] = i->second;
    }
    mcToHitCount[i->first]=i->second.size();
  }*/
  // if(result.size()>5) result.clear();

  return mcTracksToHits;
}

map<Int_t, vector<PndFtsHit *> *> PndAnalysisForwardTask::getReconstructedTracks()
{
  map<Int_t, vector<PndFtsHit *> *> hitToTrack;
  cout << "Found " << fSolution->GetEntries() << " reconstructed Tracks" << endl;
  for (int i = 0; i < fSolution->GetEntries(); i++) {
    // get the track
    PndTrack *track = (PndTrack *)fSolution->At(i);
    PndTrackCand *trackCand = track->GetTrackCandPtr();
    // get the hits
    vector<PndFtsHit *> *hits = new vector<PndFtsHit *>();
    std::set<FairLink> linksToHits = trackCand->GetLinksWithType(fIoman->GetBranchId("FTSHit")).GetLinks();
    for (std::set<FairLink>::iterator iter = linksToHits.begin(); iter != linksToHits.end(); iter++) {
      FairLink link = *iter;
      PndFtsHit *hit = (PndFtsHit *)fIoman->GetCloneOfLinkData(link);
      hit->SetEntryNr(*iter);
      hits->push_back(hit);
    }
    // save the hits
    for (size_t j = 0; j < hits->size(); j++) {
      PndFtsHit *hit = (*hits)[j];
      hitToTrack[hit->GetTubeID()] = hits;
    }
  }
  return hitToTrack;
}

void PndAnalysisForwardTask::analyzeMCTracks(vector<PndFtsHit *> hits, map<Int_t, vector<PndFtsHit *> *> tracks, Int_t trackID, Int_t trackCount)
{
  // get all reconstructed tracks with hits from this mc track
  vector<vector<PndFtsHit *> *> tracksToMc;
  cout << "Track has " << hits.size() << " Hits" << endl;
  for (size_t i = 0; i < hits.size(); i++) {
    vector<PndFtsHit *> *track = tracks[hits[i]->GetTubeID()];
    if (track == 0)
      continue;

    Bool_t exists = kFALSE;
    for (size_t j = 0; j < tracksToMc.size(); j++) {
      if (tracksToMc[j] == track) {
        exists = kTRUE;
        break;
      }
    }

    if (!exists)
      tracksToMc.push_back(track);
  }
  // all tracks
  // hits
  Int_t h2 = mcToHitCount[trackID];
  if (h2 > 48)
    hitArray2[8]++;
  else if (h2 == 48)
    hitArray2[7]++;
  else if (h2 > 40)
    hitArray2[6]++;
  else if (h2 == 40)
    hitArray2[5]++;
  else
    hitArray2[h2 / 8]++;
  // momentum
  Double_t m2 = momentum[trackID];
  if (m2 > 0.9)
    momentumArray2[9]++;
  else
    momentumArray2[(Int_t)(m2 * 10)]++;
  // check track count
  if (trackCount > 9)
    trackCountArray2[9]++;
  else
    trackCountArray2[trackCount - 1]++;

  cout << "Found " << tracksToMc.size() << " reconstructed tracks to MC-track" << endl;
  if (tracksToMc.size() == 0) {
    cout << ">>Track not found                            --! (0 Hits)" << endl;
    notFound++;
    // check hit corralation
    Int_t h = mcToHitCount[trackID];
    if (h > 48)
      hitArray[8]++;
    else if (h == 48)
      hitArray[7]++;
    else if (h > 40)
      hitArray[6]++;
    else if (h == 40)
      hitArray[5]++;
    else
      hitArray[h / 8]++;
    // check momentum corralation
    Double_t m = momentum[trackID];
    if (m > 0.9)
      momentumArray[9]++;
    else
      momentumArray[(Int_t)(m * 10)]++;
    // check track count
    if (trackCount > 9)
      trackCountArray[9]++;
    else
      trackCountArray[trackCount - 1]++;
  } else {
    vector<Int_t> bestResult = analyzeMCTrack(hits, tracksToMc[0]);
    for (size_t i = 1; i < tracksToMc.size(); i++) {
      vector<Int_t> nextResult = analyzeMCTrack(hits, tracksToMc[i]);
      if (bestResult[1] < nextResult[1])
        bestResult = nextResult;
    }

    cout << "Mc-Hits: " << bestResult[0] << endl;
    cout << "Correct found: " << bestResult[1] << endl;
    cout << "Hits not found: " << bestResult[2] << endl;
    cout << "Wrong hits: " << bestResult[3] << endl;
    if (bestResult[0] == bestResult[1]) { // complete found
      if (bestResult[3] == 0) {
        completeClean++;
        cout << ">>Found complete track, track is clean(++)" << endl;
      } else {
        completeUnclean++;
        cout << ">>Found complete track, track is not clean(+-)" << endl;
      }
    } else if (bestResult[0] * 0.7 < bestResult[1]) { // fond more than 70%
      if (bestResult[3] == 0) {
        incompleteClean++;
        cout << ">>Found incomplete track, track is clean(-+)" << endl;
      } else {
        incompleteUnclean++;
        cout << ">>Found incomplete track, track is not clean(--)" << endl;
      }
      cout << "Found Hits: (" << bestResult[1] << "/" << bestResult[0] << ")" << endl;
    } else {
      notEnoughFound++;
      cout << ">>Track not found(Under 70%)                        --!" << endl;
      cout << "Found Hits: (" << bestResult[1] << "/" << bestResult[0] << ")" << endl;
      // check hit corralation
      Int_t h = mcToHitCount[trackID];
      if (h > 48)
        hitArray[8]++;
      else if (h == 48)
        hitArray[7]++;
      else if (h > 40)
        hitArray[6]++;
      else if (h == 40)
        hitArray[5]++;
      else
        hitArray[h / 8]++;
      // check momentum corralation
      Double_t m = momentum[trackID];
      if (m > 0.9)
        momentumArray[9]++;
      else
        momentumArray[(Int_t)(m * 10)]++;
      // check track count
      if (trackCount > 9)
        trackCountArray[9]++;
      else
        trackCountArray[trackCount - 1]++;
    }
  }
}

vector<Int_t> PndAnalysisForwardTask::analyzeMCTrack(vector<PndFtsHit *> mcHits, vector<PndFtsHit *> *reconstructedHits)
{
  Int_t mcHitCount = mcHits.size();
  Int_t correctHits = 0;
  Int_t hitsNotFound = 0;
  Int_t wrongHits = 0;

  // search for found mc hits
  for (size_t i = 0; i < mcHits.size(); i++) {
    PndFtsHit *mcHit = mcHits[i];

    Int_t hitFound = kFALSE;
    for (size_t j = 0; j < reconstructedHits->size(); j++) {
      PndFtsHit *recoHit = (*reconstructedHits)[j];
      if (mcHit->GetTubeID() == recoHit->GetTubeID()) {
        hitFound = kTRUE;
        break;
      }
    }

    if (hitFound)
      correctHits++;
    else
      hitsNotFound++;
  }

  for (size_t i = 0; i < reconstructedHits->size(); i++) {
    PndFtsHit *recoHit = (*reconstructedHits)[i];
    Int_t hitFound = kFALSE;
    for (size_t j = 0; j < mcHits.size(); j++) {
      PndFtsHit *mcHit = mcHits[j];
      if (mcHit->GetTubeID() == recoHit->GetTubeID()) {
        hitFound = kTRUE;
        break;
      }
    }
    if (!hitFound)
      wrongHits++;
  }

  vector<Int_t> result;
  result.push_back(mcHitCount);
  result.push_back(correctHits);
  result.push_back(hitsNotFound);
  result.push_back(wrongHits);
  return result;
}

void PndAnalysisForwardTask::anaSpecialCases(map<Int_t, vector<PndFtsHit *>> mcTracks)
{

  for (map<Int_t, vector<PndFtsHit *>>::iterator i = mcTracks.begin(); i != mcTracks.end(); i++) {
    Int_t layer[24];
    Int_t tubeID[10000];
    for (int j = 0; j < 24; j++)
      layer[j] = 0;
    for (int j = 0; j < 10000; j++)
      tubeID[j] = 0;
    for (size_t j = 0; j < i->second.size(); j++) {
      PndFtsHit *hit = i->second[j];
      Int_t layerID = (hit->GetLayerID() - 1) / 2;
      layer[layerID]++;
      tubeID[hit->GetTubeID()]++;
    }

    for (int j = 0; j < 24; j++) {
      if (layer[j] == 1) {
        oneHitPerLayer++;
        break;
      }
    }

    for (int j = 0; j < 24; j++) {
      if (layer[j] > 2) {
        moreThan3Hits++;
        break;
      }
    }

    for (int j = 0; j < 10000; j++) {
      if (tubeID[j] > 1) {
        twoTracksOneTube++;
      }
    }
  }
}
