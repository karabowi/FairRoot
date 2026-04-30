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
// -----                      PndMQStraightLineTrackFinder                    -----
// -----                  Created 22/10/09  by M. Michel               -----
// -------------------------------------------------------------------------

#include "TClonesArray.h"
#include "TArrayD.h"
#include "TGeoManager.h"

#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"

#include "PndMQStraightLineTrackFinder.h"

#include "PndSdsDigiStrip.h"
#include "TStopwatch.h"
// #include "PndSdsPixelCluster.h"

// -----   Default constructor   -------------------------------------------
PndMQStraightLineTrackFinder::PndMQStraightLineTrackFinder() : fNLayers(4)
{
  fdXY = 0.1;
  fHitsPerLayer.resize(fNLayers);
}

// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndMQStraightLineTrackFinder::~PndMQStraightLineTrackFinder() {}
// -------------------------------------------------------------------------

// -----   Private method SortHitsByDet   --------------------------------------------
void PndMQStraightLineTrackFinder::SortHitsToLayers(std::vector<PndSdsHit> &hits)
{
  for (int i = 0; i < hits.size(); i++) {
    if (hits[i].GetSensorID() < fNLayers)
      fHitsPerLayer[hits[i].GetSensorID()].push_back(make_pair(hits[i], false));
  }
}
// -------------------------------------------------------------------------

std::vector<std::vector<int>> PndMQStraightLineTrackFinder::GetStartCombination(int firstLayer, int secondLayer)
{
  std::vector<std::vector<int>> result;
  for (int firstL = 0; firstL < fHitsPerLayer[firstLayer].size; firstL++) {
    for (int secondL = 0; secondL < fHitsPerLayer[secondLayer].size; secondL++) {
      std::vector<int> singleCombi(4, -1);
      singleCombi[firstLayer] = firstL;
      singleCombi[seondLayer] = secondL;
      result.push_back(singleCombi);
    }
  }
  return result;
}

StraightLineParams PndMQStraightLineTrackFinder::GetLineParameters(std::vector<int> startCombi)
{
  StraightLineParams result;
  TVector3 first, second;
  bool firstFound = false;
  for (int i = 0; i < startCombi.size(); i++) {
    if (startCombi[i] > -1) {
      if (firstFound == false) {
        PndSdsHit firstHit = fHitsPerLayer[i][startCombi[i]].first;
        first = firstHit.GetPosition();
        firstFound = true;
        result.origin = first;
      } else {

        PndSdsHit secondHit = fHitsPerLayer[i][startCombi[i]].first;
        second = secondHit.GetPosition();
        result.direction = (second - first).Unit();
        break;
      }
    }
  }
  return result;
}

TVector3 PndMQStraightLineTrackFinder::PropagateToXYPlane(StraightLineParams line, Double z)
{
  double t = 0;
  t = (z - line.origin.Z()) / line.direction.Z();
  TVector3 result;
  result = line.origin + line.direction * t;
  return result;
}

double PndMQStraightLineTrackFinder::DistanceOfPoints(TVector3 first, TVector3 second)
{
  return (first - second).Mag();
}

PndTrackCand PndMQStraightLineTrackFinder::FindTrack(std::vector<int> startCombi) {}

std::vector<PndTrackCand> PndMQStraightLineTrackFinder::FindTracks(std::vector<PndSdsHit> hits)
{
  SortHitsToLayers(hits);
  std::vector<std::vector<int>> startCombi = GetStartCombination(0, 1);
  for (int i = 0; i < startCombi.size(); i++) {
  }
}

// -----   Private method FindHitsII  --------------------------------------------
void PndMQStraightLineTrackFinder::FindHitsII(std::vector<PndTrackCand> &tofill, std::vector<std::vector<std::pair<Int_t, bool>>> &hitsd, Int_t nStripHits)
{

  std::vector<TVector3> trackStart, trackVec;   // pseudo tracks
  std::vector<TVector3> trackStartd, trackVecd; // save errors
  std::vector<Int_t> trackID1, trackID3;        // for TrackCand

  // iterate first discs-hits with all seconds, save pseudo-tracks
  TVector3 start, tmp, vec, dstart, dvec; // temp-vars

  if (hitsd.size() < 3)
    return;
  for (Int_t i = 0; i < hitsd.at(0).size(); i++) {
    if (hitsd.at(0).at(i).second)
      continue; // if already used
    PndSdsHit *hit1 = (PndSdsHit *)fHitArray->At(hitsd.at(0).at(i).first);
    start.SetXYZ(hit1->GetX(), hit1->GetY(), hit1->GetZ());
    dstart.SetXYZ(hit1->GetDx(), hit1->GetDy(), hit1->GetDz());
    for (Int_t k = 0; k < hitsd.at(2).size(); k++) {
      if (hitsd.at(2).at(k).second)
        continue; // if already used
      PndSdsHit *hit2 = (PndSdsHit *)fHitArray->At(hitsd.at(2).at(k).first);
      tmp.SetXYZ(hit2->GetX(), hit2->GetY(), hit2->GetZ());
      vec = tmp - start; // calc direction vector for FINDING
      dvec.SetXYZ(hit2->GetDx(), hit2->GetDy(), hit2->GetDz());
      if (vec.Theta() > 0.03 && vec.Theta() < 0.05 && vec.Phi() > -0.25 && vec.Phi() < 0.25) { // ignore vectors with theta outside 2-9 mrad
        // if(0<1){
        //	if(vec.Theta()<0.01){ //ignore vectors with theta outside 10 mrad
        trackStart.push_back(start);
        trackStartd.push_back(dstart);
        trackVec.push_back(vec);   // save vector from start to second
        trackVecd.push_back(dvec); // save error of second point for FIT, NOT error of direction vector
        // trackID1.push_back(hitsd.at(0).at(i).first);  //save Hit-Id's for TrackCand
        // trackID2.push_back(hitsd.at(1).at(k).first);
        trackID1.push_back(i); // save Hit-Id's for TrackCand
        trackID3.push_back(k);
      }
    } // end of disc 2 (or 3 if none in disc 2) hits
  }   // end of disc 1 hits

  if (fVerbose > 1)
    cout << "Pseudos L1L3: " << trackStart.size() << endl;

  std::vector<Int_t> ids;

  // check if other discs have hits in track, add points for fitting ---------------
  for (Int_t i = 0; i < trackStart.size(); i++) // pseudo-loop
  {
    ids.clear();
    Int_t pntcnt = 2;

    start = trackStart.at(i);
    dstart = trackStartd.at(i);
    vec = trackVec.at(i);
    dvec = trackVecd.at(i);
    ids.push_back(hitsd.at(0).at(trackID1.at(i)).first);
    ids.push_back(hitsd.at(2).at(trackID3.at(i)).first);
    std::vector<std::pair<Int_t, Int_t>> otherIDs;

    for (Int_t idet = 3; idet < 4; idet++) { // i know this is not a loop, but in case we someday will have more planes
      Double_t distClosest = 2 * idet * dXY; // just bigger as possible
      Bool_t firstp = true;
      for (Int_t ihit = 0; ihit < hitsd.at(idet).size(); ihit++) {
        PndSdsHit *hit = (PndSdsHit *)fHitArray->At(hitsd.at(idet).at(ihit).first);
        Double_t scale = (hit->GetZ() - start.z()) / vec.z();
        tmp = start + scale * vec; // extend search-vector to hit-plane
        Double_t distTmp = sqrt((tmp.x() - hit->GetX()) * (tmp.x() - hit->GetX()) + (tmp.y() - hit->GetY()) * (tmp.y() - hit->GetY()));
        if (distTmp < (idet * dXY)) { // if in diameter
          if (firstp) {
            pntcnt++;
            ids.push_back(hitsd.at(idet).at(ihit).first);
            otherIDs.push_back(make_pair(idet, ihit));
            distClosest = distTmp;
            firstp = false;
          } else {
            if (distTmp < distClosest) { // change if nearer
              ids.pop_back();
              otherIDs.pop_back();
              ids.push_back(hitsd.at(idet).at(ihit).first);
              otherIDs.push_back(make_pair(idet, ihit));
            }
          }
        } // endif in diameter
      }   // end of disc n hits
    }     // end of discs

    if (fVerbose > 2)
      cout << "  Track L1L3: " << i << "#Planes: " << ids.size() << endl;

    // create track für fitting
    if (ids.size() > 2) { // third hit found <=> !track found!
      PndTrackCand *myTCand = new PndTrackCand();

      for (Int_t id = 0; id < ids.size(); id++) {
        PndSdsHit *myHit = (PndSdsHit *)(fHitArray->At(ids.at(id)));
        PndSdsClusterStrip *myCluster;
        PndSdsDigiStrip *astripdigi;
        if (!fStripClusterArray || !fStripDigiArray) { // for ideal/fast Hit-Reco
          myTCand->AddHit(0, ids.at(id), myHit->GetPosition().Mag());
        } else {
          myCluster = (PndSdsClusterStrip *)(fStripClusterArray->At(myHit->GetClusterIndex()));
          astripdigi = (PndSdsDigiStrip *)fStripDigiArray->At(myCluster->GetDigiIndex(0));
          myTCand->AddHit(astripdigi->GetDetID(), ids.at(id), myHit->GetPosition().Mag());
        }
      }

      // mark used hits---------
      hitsd.at(0).at(trackID1.at(i)).second = true;
      hitsd.at(2).at(trackID3.at(i)).second = true;
      for (Int_t id = 0; id < otherIDs.size(); id++) {
        hitsd.at(otherIDs.at(id).first).at(otherIDs.at(id).second).second = true;
      }

      // ///Add seed information to track------------
      // PndSdsHit* myHit0 = (PndSdsHit*)(fStripHitArray->At(ids.at(0)));
      // PndSdsHit* myHit1 = (PndSdsHit*)(fStripHitArray->At(ids.at(1)));
      // TVector3 hit0 = myHit0->GetPosition(); TVector3 hit1 = myHit1->GetPosition();
      // TVector3 posSeed(hit0.X(),hit0.Y(),hit0.Z());
      // vec*=1./vec.Mag();
      // 	//shift trk out of plane [needed for correct treatment in Kalman Fillter and GEANE]
      // 	double sh_z = -0.035; //350 mkm
      // 	double sh_x = vec.X()*sh_z;
      // 	double sh_y = vec.Y()*sh_y;
      // 	TVector3 sh_point(sh_x,sh_y,sh_z);
      // 	posSeed +=sh_point;
      // myTCand->setTrackSeed(posSeed,vec,-1);
      // ///-------------------------------------

      tofill.push_back(*(myTCand)); // save Track Candidate
      // new((*fTrackCandArray)[trackCnt]) PndTrackCand(*(myTCand));
      delete myTCand;
    } // Track Cand build
  }   // end of pseudo-tracks
}
// -------------------------------------------------------------------------

// -----   Private method FindHitsI  --------------------------------------------
void PndMQStraightLineTrackFinder::FindHitsI(std::vector<PndTrackCand> &tofill, std::vector<std::vector<std::pair<Int_t, bool>>> &hitsd, Int_t nStripHits)
{

  std::vector<TVector3> trackStart, trackVec;   // pseudo tracks
  std::vector<TVector3> trackStartd, trackVecd; // save errors
  std::vector<Int_t> trackID1, trackID2;        // for TrackCand

  // iterate first discs-hits with all seconds, save pseudo-tracks
  TVector3 start, tmp, vec, dstart, dvec; // temp-vars

  if (hitsd.size() < 2)
    return;
  for (Int_t i = 0; i < hitsd.at(0).size(); i++) {
    PndSdsHit *hit1 = (PndSdsHit *)fHitArray->At(hitsd.at(0).at(i).first);
    start.SetXYZ(hit1->GetX(), hit1->GetY(), hit1->GetZ());
    dstart.SetXYZ(hit1->GetDx(), hit1->GetDy(), hit1->GetDz());
    for (Int_t k = 0; k < hitsd.at(1).size(); k++) {
      PndSdsHit *hit2 = (PndSdsHit *)fHitArray->At(hitsd.at(1).at(k).first);
      tmp.SetXYZ(hit2->GetX(), hit2->GetY(), hit2->GetZ());
      vec = tmp - start; // calc direction vector for FINDING
      dvec.SetXYZ(hit2->GetDx(), hit2->GetDy(), hit2->GetDz());
      // if(vec.Theta()>0.03 && vec.Theta()<0.05 && vec.Phi()>-0.25 && vec.Phi()<0.25){ //ignore vectors with theta outside 2-9 mrad
      //      if(vec.Theta()<0.01){ //ignore vectors with theta outside 10 mrad
      trackStart.push_back(start);
      trackStartd.push_back(dstart);
      trackVec.push_back(vec);   // save vector from start to second
      trackVecd.push_back(dvec); // save error of second point for FIT, NOT error of direction vector
      // trackID1.push_back(hitsd.at(0).at(i).first);  //save Hit-Id's for TrackCand
      // trackID2.push_back(hitsd.at(1).at(k).first);
      trackID1.push_back(i); // save Hit-Id's for TrackCand
      trackID2.push_back(k);
      //   }
    } // end of disc 2 (or 3 if none in disc 2) hits
  }   // end of disc 1 hits

  if (fVerbose > 1)
    cout << "Pseudos L1L2: " << trackStart.size() << endl;

  std::vector<Int_t> ids;

  // check if other discs have hits in track, add points for fitting ---------------
  for (Int_t i = 0; i < trackStart.size(); i++) // pseudo-loop
  {
    ids.clear();
    Int_t pntcnt = 2;

    start = trackStart.at(i);
    dstart = trackStartd.at(i);
    vec = trackVec.at(i);
    dvec = trackVecd.at(i);
    ids.push_back(hitsd.at(0).at(trackID1.at(i)).first);
    ids.push_back(hitsd.at(1).at(trackID2.at(i)).first);
    std::vector<std::pair<Int_t, Int_t>> otherIDs;

    for (Int_t idet = 2; idet < 4; idet++) {
      Double_t distClosest = 2 * idet * dXY; // just bigger as possible
      Bool_t firstp = true;
      for (Int_t ihit = 0; ihit < hitsd.at(idet).size(); ihit++) {
        PndSdsHit *hit = (PndSdsHit *)fHitArray->At(hitsd.at(idet).at(ihit).first);
        Double_t scale = (hit->GetZ() - start.z()) / vec.z();
        tmp = start + scale * vec; // extend search-vector to hit-plane
        Double_t distTmp = sqrt((tmp.x() - hit->GetX()) * (tmp.x() - hit->GetX()) + (tmp.y() - hit->GetY()) * (tmp.y() - hit->GetY()));
        if (distTmp < (idet * dXY)) { // if in diameter
          if (firstp) {
            pntcnt++;
            ids.push_back(hitsd.at(idet).at(ihit).first);
            otherIDs.push_back(make_pair(idet, ihit));
            distClosest = distTmp;
            firstp = false;
          } else {
            if (distTmp < distClosest) { // change if nearer
              ids.pop_back();
              otherIDs.pop_back();
              ids.push_back(hitsd.at(idet).at(ihit).first);
              otherIDs.push_back(make_pair(idet, ihit));
            }
          }
        } // endif in diameter
      }   // end of disc n hits
    }     // end of discs

    if (fVerbose > 2)
      cout << "  Track L1L2: " << i << "#Planes: " << ids.size() << endl;

    // create track für fitting
    if (ids.size() > 2) { // third hit found <=> !track found!
      //   if(ids.size()>3){ //4 hits are needed because 6 parameters are used in trk-fit!
      // // //third hit found <=> !track found!
      // // //check direction for reduction of ghost tracks
      // if(ids.size()>2 &&
      //    fabs(vec.Phi())<0.26 && vec.Theta()>3e-2 && vec.Theta()<5e-2){
      if (fVerbose > 2)
        cout << "  Track: " << i << "#Planes: " << ids.size() << endl;
      PndTrackCand *myTCand = new PndTrackCand();

      for (Int_t id = 0; id < ids.size(); id++) {
        PndSdsHit *myHit = (PndSdsHit *)(fHitArray->At(ids.at(id)));
        PndSdsClusterStrip *myCluster;
        PndSdsDigiStrip *astripdigi;

        myTCand->AddHit(FairRootManager::Instance()->GetBranchId(fHitBranchStrip), ids.at(id), myHit->GetPosition().Mag());
      }

      // mark used hits---------
      hitsd.at(0).at(trackID1.at(i)).second = true;
      hitsd.at(1).at(trackID2.at(i)).second = true;
      for (Int_t id = 0; id < otherIDs.size(); id++) {
        hitsd.at(otherIDs.at(id).first).at(otherIDs.at(id).second).second = true;
      }

      /// Add seed information to track------------
      // Double_t Z0 = 1099.;
      // //  Double_t Z0 = 0;
      // PndSdsHit* myHit0 = (PndSdsHit*)(fStripHitArray->At(ids.at(0)));
      // PndSdsHit* myHit1 = (PndSdsHit*)(fStripHitArray->At(ids.at(1)));
      // TVector3 hit0 = myHit0->GetPosition(); TVector3 hit1 = myHit1->GetPosition();
      // double p1seed = (hit0.X()-hit1.X())/(hit0.Z()-hit1.Z());
      // double p0seed = hit0.X() - p1seed*(hit0.Z()-Z0);
      // //      double p0seed = 0.5*(hit0.X()+hit1.X()-p1seed*(hit0.Z()+hit1.Z()-2*1099.)); //TO DO: don't use const
      // double p3seed = (hit0.Y()-hit1.Y())/(hit0.Z()-hit1.Z());
      // double p2seed = hit0.Y() - p3seed*(hit0.Z()-Z0);
      // //double p2seed = 0.5*(hit0.Y()+hit1.Y()-p1seed*(hit0.Z()+hit1.Z()-2*1099.)); //TO DO: don't use const
      // TVector3 posSeed(p0seed,p2seed,Z0);
      // TVector3 dirSeed(p1seed,p3seed,1.);
      //   cout<<"posSeed:"<<endl;
      //   posSeed.Print();
      //   cout<<"dirSeed:"<<endl;
      //   dirSeed.Print();

      // //  Double_t Z0 = 1099.;
      // //     TVector3 posSeed(start.X(),start.Y(),Z0);
      // TVector3 posSeed(start.X(),start.Y(),start.Z());
      // // cout<<"Trk cand: pos"<<endl;
      // // posSeed.Print();
      // vec*=1./vec.Mag();
      // //shift trk out of plane [needed for correct treatment in Kalman Fillter and GEANE]
      // double sh_z = -0.035; //350 mkm
      // double sh_x = vec.X()*sh_z;
      // double sh_y = vec.Y()*sh_y;
      // TVector3 sh_point(sh_x,sh_y,sh_z);
      // posSeed +=sh_point;

      // // cout<<"Trk cand: vec"<<endl;
      // // vec.Print();
      // myTCand->setTrackSeed(posSeed,vec,-1);
      // ///-------------------------------------

      tofill.push_back(*(myTCand)); // save Track Candidate
      // new((*fTrackCandArray)[trackCnt]) PndTrackCand(*(myTCand));
      delete myTCand;
    } // Track Cand build
  }   // end of pseudo-tracks
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndMQStraightLineTrackFinder::Exec(Option_t *)
{
  TStopwatch *timer_exec = new TStopwatch();
  if (fVerbose > 2)
    timer_exec->Start();
  if (fVerbose > 2)
    cout << "Evt " << FairRootManager::Instance()->GetEntryNr() << " started--------------" << endl << endl;

  // which combinations of planes to build pseudo-vectos
  // Int_t planeFlag=0; //flag=... 0: planes 1-2,   1: 1-2 & 1-3,   2: 1-2 & 1-3 & 2-3

  // Reset output array
  if (!fTrackCandArray)
    Fatal("Exec", "No trackCandArray");
  fTrackCandArray->Clear();

  Int_t nStripHits = fHitArray->GetEntriesFast();
  if (fVerbose > 2)
    cout << "# Hits: \t" << nStripHits << endl << endl;
  bool usedFlag[nStripHits]; // for pseudo-vector building
  for (Int_t seti = 0; seti < nStripHits; seti++)
    usedFlag[seti] = false;

  if (nStripHits < 2) {
    if (fVerbose > 2)
      cout << "Evt finsihed: too less hits-----" << endl << endl;
    return;
  }

  //-----do some sorting first----------
  std::vector<std::vector<std::pair<int, bool>>> hitsd(4);
  bool resSortHits;
  if (flagStipSens)
    resSortHits = SortHitsByDet(hitsd, nStripHits); //! strip sensors
  else {
    if (flagPixelSens)
      resSortHits = SortHitsByDet2(hitsd, nStripHits); //! pixel sensors
    else {
      std::cout << "Algorithm is needed sensor type! Please, set it via SetSensStripFlag(bool fS) or SetSensPixelFlag(bool fS)" << std::endl;
      return;
    }
  }
  if (!resSortHits) {
    if (fVerbose > 2)
      cout << "Evt finsihed: too less planes-----" << endl << endl;
    return;
  }

  if (fVerbose > 2) {
    cout << "HitMap size: " << hitsd.size() << endl;
    if (hitsd.size() > 0) {
      for (Int_t i = 0; i < hitsd.at(0).size(); i++) {
        PndSdsHit *hit = (PndSdsHit *)fHitArray->At(hitsd.at(0).at(i).first);
        cout << "Plane0 Hit=(" << hit->GetX() << ", " << hit->GetY() << ", " << hit->GetZ() << ")" << endl;
      }
    }
    if (hitsd.size() > 1) {
      for (Int_t i = 0; i < hitsd.at(1).size(); i++) {
        PndSdsHit *hit = (PndSdsHit *)fHitArray->At(hitsd.at(1).at(i).first);
        cout << "Plane1 Hit=(" << hit->GetX() << ", " << hit->GetY() << ", " << hit->GetZ() << ")" << endl;
      }
    }
    if (hitsd.size() > 2) {
      for (Int_t i = 0; i < hitsd.at(2).size(); i++) {
        PndSdsHit *hit = (PndSdsHit *)fHitArray->At(hitsd.at(2).at(i).first);
        cout << "Plane2 Hit=(" << hit->GetX() << ", " << hit->GetY() << ", " << hit->GetZ() << ")" << endl;
      }
    }
    if (hitsd.size() > 3) {
      for (Int_t i = 0; i < hitsd.at(3).size(); i++) {
        PndSdsHit *hit = (PndSdsHit *)fHitArray->At(hitsd.at(3).at(i).first);
        cout << "Plane3 Hit=(" << hit->GetX() << ", " << hit->GetY() << ", " << hit->GetZ() << ")" << endl;
      }
    }
  }

  std::vector<PndTrackCand> theCands; // Track Candidates

  //--------find some tracks--------------
  FindHitsI(theCands, hitsd, nStripHits);
  if (fFinderMode) {
      FindHitsII(theCands, hitsd, nStripHits);
    FindHitsIII(theCands, hitsd, nStripHits);
  }

  // fill tracklist für fitting
  for (int t = 0; t < theCands.size(); t++)
    new ((*fTrackCandArray)[t]) PndTrackCand(theCands.at(t));

  if (fVerbose > 2)
    cout << "Evt finsihed--------------" << endl << endl;
  if (fVerbose > 2) {
    timer_exec->Stop();
    Double_t rtime_exec = timer_exec->RealTime();
    Double_t ctime_exec = timer_exec->CpuTime();
    cout << "Real time for Exec:" << rtime_exec << " s, CPU time " << ctime_exec << " s" << endl;
    cout << endl;
  }
}
// -------------------------------------------------------------------------

Double_t PndMQStraightLineTrackFinder::GetTrackCurvature(PndMCTrack *myTrack)
{
  TVector3 p = myTrack->GetMomentum();
  return (2 / TMath::Sqrt(p.Px() * p.Px() + p.Py() * p.Py()));
}
// -------------------------------------------------------------------------

Double_t PndMQStraightLineTrackFinder::GetTrackDip(PndMCTrack *myTrack)
{
  TVector3 p = myTrack->GetMomentum();
  return (p.Mag() / TMath::Sqrt(p.Px() * p.Px() + p.Py() * p.Py()));
}
