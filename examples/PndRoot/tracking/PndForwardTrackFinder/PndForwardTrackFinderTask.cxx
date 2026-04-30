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

#include "PndForwardTrackFinderTask.h"

ClassImp(PndForwardTrackFinderTask);

PndForwardTrackFinderTask::~PndForwardTrackFinderTask() {}

void PndForwardTrackFinderTask::SetParContainers() {}

InitStatus PndForwardTrackFinderTask::Init()
{
  fIoman = FairRootManager::Instance();
  // load hits
  fHits = (TClonesArray *)fIoman->GetObject("FTSHit"); // correct hits
  // planes
  planes = fIoman->Register("Planes", "PndTrack", "FTS", kTRUE);
  planesHit = fIoman->Register("PlanesHits", "PndFtsHit", "FTS", kTRUE);
  // tracklets
  fFirstTrackCandArray = fIoman->Register("FirstTrackCand", "PndTrackCand", "FTS", kTRUE);
  // corrected tracklets
  correctedTracklets = fIoman->Register("CorrectedTracklets", "PndTrack", "FTS", kTRUE);
  correctedHits = fIoman->Register("CorrectedHits", "PndFtsHit", "FTS", kTRUE);
  // layer tracks
  fLayerTracks = fIoman->Register("LayerTracks", "PndTrack", "FTS", kTRUE);
  fLayerTracksHits = fIoman->Register("LayerTracksHits", "PndFtsHit", "FTS", kTRUE);
  // global tracks befor and after magnet
  fGlobalTracks = fIoman->Register("GlobalTracks", "PndTrack", "FTS", kTRUE);
  fGlobalTracksHits = fIoman->Register("GlobalTracksHits", "PndFtsHit", "FTS", kTRUE);
  // final Solution
  fFinalSolution = fIoman->Register("FinalSolution", "PndTrack", "FTS", kTRUE);
  // track collection
  fTrackCollection = fIoman->Register("TrackCollection", "PndTrack", "FTS", kTRUE);
  fTrackCollectionHits = fIoman->Register("TrackCollectionHits", "PndFtsHit", "FTS", kTRUE);
  // fill the root-branches array
  rootBranches[0] = fLayerTracks;
  rootBranches[1] = fLayerTracksHits;
  rootBranches[2] = fGlobalTracks;
  rootBranches[3] = fGlobalTracksHits;
  fPndFtsCellTrackletGenerator = new PndFtsCellTrackletGenerator();
  fPndFtsLineApproximator = new PndFtsLineApproximator();
  fPndTrackCombiner = new PndTrackCombiner(planes, planes);
  fModuleCombiner = new PndModuleCombiner();
  fWayFollower = new PndWayFollower(planes, planesHit);

  for (int i = 0; i < 20; i++)
    dists[i] = 0;
  for (int i = 0; i < 20; i++)
    angles[i] = 0;
  return kSUCCESS;
}

void PndForwardTrackFinderTask::Exec(Option_t *)
{
  std::cout << ">>>>>>>>>>>>>>====================PndForwardTrackFinderTask::Exec====================<<<<<<<<<<<<<<" << std::endl;
  std::cout << "EventNumber:" << eventNumber++ << std::endl;
  // reset the TrackletGenerator
  fPndFtsCellTrackletGenerator->reset();
  // get the hits and add to the TrackletGenerator
  std::vector<PndFtsHit *> hits;
  for (int i = 0; i < fHits->GetEntries(); i++) {
    PndFtsHit *hit = (PndFtsHit *)(fHits->At(i));
    hit->SetEntryNr(FairLink(-1, fIoman->GetEntryNr(), fIoman->GetBranchId("FTSHit"), i));
    // hit->SetEntryNr(FairLink(-1, fIoman->GetEntryNr(), fIoman->GetBranchId("FTSHitErr"), i));
    hits.push_back(hit);
    fOriginalHits[hit->GetTubeID()] = hit;
  }
  fPndFtsCellTrackletGenerator->setHits(hits);

  fPndFtsCellTrackletGenerator->findTracks();

  // create first track cand
  map<Int_t, vector<PndTrackCand>> result = fPndFtsCellTrackletGenerator->getTracklets();
  fFirstTrackCand.clear();
  for (map<Int_t, vector<PndTrackCand>>::iterator it = result.begin(); it != result.end(); it++) {
    for (size_t i = 0; i < it->second.size(); i++)
      fFirstTrackCand.push_back(it->second[i]);
  }
  //	cout << fFirstTrackCand.size() << " Tracks found " << endl;
  if (fFirstTrackCand.size() > 100) {
    fFirstTrackCand.clear();
    result.clear();
    cout << "Too mutch tracks for reconstruction" << endl;
  }

  // save data in root-branch
  for (size_t i = 0; i < fFirstTrackCand.size(); i++) {
    new ((*fFirstTrackCandArray)[i]) PndTrackCand(fFirstTrackCand[i]); // PndTrackCand* myCand =  //[R.K.03/2017] unused variable
  }

  // create expanded track cands
  map<Int_t, vector<PndFtsExpandedTrackCand>> expandedTrackCands;
  for (map<Int_t, vector<PndTrackCand>>::iterator it = result.begin(); it != result.end(); it++) {
    vector<PndTrackCand> myCands = it->second;
    for (size_t i = 0; i < myCands.size(); i++) {
      PndTrackCand myCand = myCands[i];
      PndFtsExpandedTrackCand c = fPndFtsLineApproximator->createExpandedTrackCand(myCand);
      if (c.getLineApproximations().size() != 0)
        expandedTrackCands[it->first].push_back(c);
    }
  }
  saveCorrectedTracklets(expandedTrackCands);

  // combine the expanded track cands
  fPndTrackCombiner->init(expandedTrackCands);
  // cout << "----------------------combine module 1" << endl;
  vector<PndLineApproximation> combined1 = fPndTrackCombiner->combine(0);
  // cout << "-------------------------combine module 2" << endl;
  vector<PndLineApproximation> combined2 = fPndTrackCombiner->combine(1);
  // cout << "-----------------------------combine module 5" << endl;
  vector<PndLineApproximation> combined5 = fPndTrackCombiner->combine(4);
  // cout << "-------------------------------combine module 6" << endl;
  vector<PndLineApproximation> combined6 = fPndTrackCombiner->combine(5);

  combinedTracksHitNum = 0;
  combinedTracksTrackNum = 0;
  // cout << "save layer tracks " << endl;
  saveCombined(combined1, 0, fIoman->GetBranchId("LayerTracksHits"), kFALSE);
  saveCombined(combined2, 0, fIoman->GetBranchId("LayerTracksHits"), kFALSE);
  saveCombined(combined5, 0, fIoman->GetBranchId("LayerTracksHits"), kFALSE);
  saveCombined(combined6, 0, fIoman->GetBranchId("LayerTracksHits"), kFALSE);

  vector<PndLineApproximation> m12 = fModuleCombiner->combineModules(combined1, combined2);
  vector<PndLineApproximation> m56 = fModuleCombiner->combineModules(combined5, combined6);

  // way follower
  fWayFollower->init(m12, expandedTrackCands);
  vector<PndTrackCollection> ptc = fWayFollower->followLines(8);

  // createStatictcs(ptc,m56);

  fModuleCombiner->combineModules(ptc, m56);
  fModuleCombiner->addUnusedHits(hits, ptc);
  saveTrackCollection(ptc, kFALSE);

  // save
  combinedTracksHitNum = 0;
  combinedTracksTrackNum = 0;
  // cout << "save global " << endl;
  saveCombined(m12, 2, fIoman->GetBranchId("GlobalTracksHits"), kFALSE);
  // cout << "save final solution" << endl;
  saveTrackCollection(ptc, kTRUE);
}
void PndForwardTrackFinderTask::FinishEvent()
{
  fFirstTrackCandArray->Delete();
  fLayerTracks->Delete();
  correctedHits->Delete();
  correctedTracklets->Delete();
  fLayerTracksHits->Delete();
  fGlobalTracks->Delete();
  fGlobalTracksHits->Delete();
  planes->Delete();
  planesHit->Delete();
  fTrackCollection->Delete();
  fTrackCollectionHits->Delete();
  fFinalSolution->Delete();
}

void PndForwardTrackFinderTask::Finish()
{
  //	cout << "Dists:" << endl;
  //	for(int i=0;i<20;i++)
  //		cout << dists[i] <<";";
  //	cout << endl <<"Angles:" << endl;
  //	for(int i=0;i<20;i++)
  //		cout << angles[i] <<";";
}

void PndForwardTrackFinderTask::saveCombined(vector<PndLineApproximation> combined, Int_t branch, Int_t branchID, Bool_t useOrgHits)
{
  if (branchID == -1) {
    std::cout << "PndForwardTrackFinderTask::saveCombined branchID == -1" << std::endl;
  }
  TVector3 v(1, 1, 1);
  for (size_t i = 0; i < combined.size(); i++) {
    PndLineApproximation l = combined[i];
    PndLine l2 = l.getLine();
    PndTrackCand c;
    FairTrackParP tp1(v, v, v, v, 1, v, v, v);
    FairTrackParP tp2(l2.getP1(), 3 * l2.getDir().Unit(), v, v, 1, v, v, v);
    for (size_t j = 0; j < l.getHits().size(); j++) {
      PndFtsHit *hit = l.getHits()[j];
      if (!useOrgHits) {
        hit->SetEntryNr(FairLink(-1, fIoman->GetEntryNr(), branchID, combinedTracksHitNum));
        new ((*(rootBranches[branch + 1]))[combinedTracksHitNum++]) PndFtsHit(*hit); // PndFtsHit* myHit =  //[R.K.03/2017] unused variable
      } else {
        hit = fOriginalHits[hit->GetTubeID()];
      }
      c.AddHit(hit->GetEntryNr(), j);
    }
    new ((*(rootBranches[branch]))[combinedTracksTrackNum++]) PndTrack(tp1, tp2, c); // PndTrack* myCand =  //[R.K.03/2017] unused variable
  }
}

void PndForwardTrackFinderTask::saveCorrectedTracklets(map<Int_t, vector<PndFtsExpandedTrackCand>> res)
{
  Int_t trackNum = 0;
  Int_t hitNum = 0;
  for (map<Int_t, vector<PndFtsExpandedTrackCand>>::iterator it = res.begin(); it != res.end(); it++) {
    vector<PndFtsExpandedTrackCand> etc = it->second;
    for (size_t i = 0; i < etc.size(); i++) {
      PndFtsExpandedTrackCand cand = etc[i];
      vector<PndLineApproximation> approxs = cand.getLineApproximations();
      for (size_t j = 0; j < approxs.size(); j++) {
        PndLineApproximation approx = approxs[j];
        PndTrackCand trackCand;
        for (size_t k = 0; k < approx.getHits().size(); k++) {
          PndFtsHit *hit = approx.getHits()[k];
          hit->SetEntryNr(FairLink(-1, fIoman->GetEntryNr(), fIoman->GetBranchId("CorrectedHits"), hitNum));
          new ((*correctedHits)[hitNum++]) PndFtsHit(*hit); // PndFtsHit* myHit =  //[R.K.03/2017] unused variable
          trackCand.AddHit(hit->GetEntryNr(), k);
        }
        TVector3 v(1, 1, 1);
        FairTrackParP tp1(approx.getLine().getP1(), 3 * approx.getLine().getDir().Unit(), v, v, 1, v, v, v);
        FairTrackParP tp2(v, 2 * v, v, v, 1, v, v, v);
        // PndTrack* myCand =
        new ((*correctedTracklets)[trackNum++]) PndTrack(tp1, tp2, trackCand);
        //				std::cout << "PndForwardTrackFinderTask::saveCorrectedTracklets myCand: " << *myCand << std::endl;
      }
    }
  }
}

void PndForwardTrackFinderTask::saveTrackCollection(vector<PndTrackCollection> coll, Bool_t withOrgHits)
{
  Int_t trackNum = 0;
  Int_t hitNum = 0;
  TVector3 v(1, 1, 1);
  for (size_t k = 0; k < coll.size(); k++) {
    PndTrackCollection c = coll[k];
    if (withOrgHits) {
      new ((*fFinalSolution)[trackNum++]) PndTrack(c.getPndTrack(fOriginalHits)); // PndTrack* myCand =  //[R.K.03/2017] unused variable
    } else {
      vector<PndLineApproximation> *approx = c.getLines();
      for (size_t i = 0; i < approx->size(); i++) {
        PndLineApproximation a = (*approx)[i];
        PndTrackCand trackCand;
        for (size_t j = 0; j < a.getHits().size(); j++) {
          PndFtsHit *hit = a.getHits()[j];
          hit->SetEntryNr(FairLink(-1, fIoman->GetEntryNr(), fIoman->GetBranchId("TrackCollectionHits"), hitNum));
          new ((*fTrackCollectionHits)[hitNum++]) PndFtsHit(*hit); // PndFtsHit* myHit =  //[R.K.03/2017] unused variable
          trackCand.AddHit(hit->GetEntryNr(), j);
        }
        FairTrackParP tp1(a.getLine().getP1(), 3 * a.getLine().getDir().Unit(), v, v, 1, v, v, v);
        FairTrackParP tp2(v, 2 * v, v, v, 1, v, v, v);
        new ((*fTrackCollection)[trackNum++]) PndTrack(tp1, tp2, trackCand); // PndTrack* myCand =  //[R.K.03/2017] unused variable
      }
      FairTrackParP tp1(c.getCurrLine().getP1(), 3 * c.getCurrLine().getDir().Unit(), v, v, 1, v, v, v);
      FairTrackParP tp2(v, 2 * v, v, v, 1, v, v, v);
      PndTrackCand trackCand;
      new ((*fTrackCollection)[trackNum++]) PndTrack(tp1, tp2, trackCand); // PndTrack* myCand =  //[R.K.03/2017] unused variable
    }
  }
}

void PndForwardTrackFinderTask::createStatictcs(vector<PndTrackCollection> c, vector<PndLineApproximation> a)
{
  PndFtsLineComparator comparator(0, 0);
  for (size_t i = 0; i < c.size(); i++) {
    PndTrackCollection coll = c[i];
    PndLineApproximation appr = getBest(coll, a);
    if (appr.getLine().getRating() == -1)
      continue;
    PndLine l1 = coll.getLastLine();
    PndLine l2 = appr.getLine();
    comparator.setZValue((l1.getP1()[2] + l2.getP1()[2]) / 2);
    Double_t dist = comparator.getDist2D(l1.getP1(), l2.getP1(), l1.getDir(), l2.getDir());
    Double_t angle = comparator.getAngle2D(l1.getDir(), l2.getDir());
    //		cout << "Dist=" << (Int_t)(dist/5) << endl;
    //		cout << "Angle=" << (Int_t)(angle/5) << endl;;
    if (dist >= 100)
      dists[19]++;
    else
      dists[(Int_t)(dist / 5)]++;
    if (angle >= 100)
      angles[19]++;
    else
      angles[(Int_t)(angle / 5)]++;
  }
}

PndLineApproximation PndForwardTrackFinderTask::getBest(PndTrackCollection &c, vector<PndLineApproximation> a)
{
  Int_t mc = getMcId(c.getHits());
  for (size_t i = 0; i < a.size(); i++) {
    Int_t mc2 = getMcId(a[i].getHits());
    if (mc == mc2)
      return a[i];
  }
  PndLineApproximation appr;
  appr.getLine().setRating(-1);
  return appr;
}

Int_t PndForwardTrackFinderTask::getMcId(vector<PndFtsHit *> hits)
{
  Int_t mcTrackBranchId = fIoman->GetBranchId("MCTrack");
  vector<Int_t> trackIDs;
  for (size_t i = 0; i < hits.size(); i++) {
    PndFtsHit *hit = fOriginalHits[hits[i]->GetTubeID()];
    FairLink link = hit->GetEntryNr();
    PndFtsPoint *p = (PndFtsPoint *)fIoman->GetCloneOfLinkData(link);
    FairMultiLinkedData mld = p->GetLinksWithType(mcTrackBranchId);
    FairLink linksToMc = mld.GetLink(0);
    int trackID = linksToMc.GetIndex();
    trackIDs.push_back(trackID);
  }
  Int_t maxID = 0;
  Int_t maxVal = -1;
  map<Int_t, Int_t> m;
  for (size_t i = 0; i < trackIDs.size(); i++) {
    m[trackIDs[i]]++;
    if (m[trackIDs[i]] > maxVal) {
      maxVal = m[trackIDs[i]];
      maxID = trackIDs[i];
    }
  }
  return maxID;
}
