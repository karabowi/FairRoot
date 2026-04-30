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

#ifndef PNDRIEMANNTRACKFINDER_H_
#define PNDRIEMANNTRACKFINDER_H_

#include "PndRiemannTrack.h"
#include "PndRiemannHit.h"
#include "FairHit.h"
#include "PndSdsHit.h"
#include "TClonesArray.h"
#include "PndTrackCand.h"
#include "TH2F.h"

#include <vector>
#include <map>

class PndRiemannTrackFinder {
 public:
  PndRiemannTrackFinder();
  virtual ~PndRiemannTrackFinder();

  void FindTracks(); ///< Main function to start the riemann track finding
  void MergeTracks();
  void SetHits(std::vector<FairHit *> hits) { fHits = hits; }; ///< Replaces the existing array of hits with a new one
  void AddHits(std::vector<FairHit *> hits, Int_t branchId);   ///< Appends the new array of hits to the existing one
  void AddHits(TClonesArray *hits, Int_t branchId);            ///< Appends the new array of hits to the existing one
  void AddHit(FairHit *hit) { fHits.push_back(hit); };         ///< Adds one new hit to the array of hits
  void SetVerbose(int val) { fVerbose = val; }
  int NTracks() { return fTrackCand.size(); };            ///< Returns the number of found tracks
  PndRiemannTrack GetTrack(int i) { return fTracks[i]; }; ///< Returns the track with the index i
  PndTrack GetPndTrack(int i, double B) { return fTracks[i].getPndTrack(B); };
  std::set<Int_t> GetTrackCandidates(int i) { return fHitsInTracks[i]; }; ///< Returns the hits belonging to track i
  std::vector<PndTrackCand> GetTrackCand() { return fTrackCand; }
  std::vector<PndTrackCand> GetMergedTrackCands() { return fMergedTrackCand; }
  PndTrackCand GetTrackCand(int i) { return fTrackCand[i]; }
  double HitDistance(FairHit *h1, FairHit *h2);                                  ///< Calculates the distance between two hits
  int HitTooClose(std::set<Int_t> hitsInUse, FairHit *newHit, double threshold); ///< returns if and which hit was too close to the hit which is tested

  void SetMaxPlaneDistance(double val) { fMaxPlaneDist = val; }
  void SetMaxSZDist(double val) { fMaxSZDist = val; }
  void SetMaxSZChi2(double val) { fMaxSZChi2 = val; }
  void SetMinPointDist(double val) { fMinPointDist = val; }
  void SetUseZeroPos(bool val) { fUseZeroPos = val; }
  void SetMinNumberOfHits(int val) { fMinNumberOfHits = val; }
  void SetCurvDiff(double val) { fCurvDiff = val; }
  void SetDipDiff(double val) { fDipDiff = val; }

 protected:
  std::vector<FairHit *> fHits;                             ///< Vector of all FairHits used for track finding (fitting)
  std::vector<PndRiemannTrack> fTracks;                     ///< Resulting Riemann Tracks
  std::vector<std::set<Int_t>> fHitsInTracks;               ///< Vector of indizes which hits where used in which track
  std::vector<PndTrackCand> fTrackCand;                     ///< List of track candidates
  std::vector<std::pair<double, double>> fCurvAndDipOfCand; ///< Curvature and dip of fPndTrackCand
  std::vector<PndTrackCand> fMergedTrackCand;
  std::vector<std::set<int>> fHitsTooClose; ///< matrix of TrackNr and hits which are too close to one of the three starting points
  std::map<int, FairLink> fMapHitToID;      ///< map to convert the list of hits back into a FairLink
  std::map<FairLink, int> fMapIDtoHit;      ///< map to convert the list of detID/hitID hits into the list of hits for track finding
  double fMaxPlaneDist;                     ///< Distance cut between new point and riemann plane
  double fMaxSZDist;                        ///< Distance cut between s-z coordinate of a new point and the sz-fit of the hits in the track
  double fMinPointDist;                     ///< Minimum distance between two points to use them as point for the base plane
  double fMaxSZChi2;                        ///< Maximum allowed Chi2 in an sz fit
  int fMinNumberOfHits;                     ///< Minimum number of hits in track necessary for a match
  double fCurvDiff;                         ///< TrackMerger parameter
  double fDipDiff;                          ///< TrackMerger parameter
  double fMagField;                         ///< size of the magnetic field in Tesla

  int fVerbose;
  bool fUseZeroPos;

  std::vector<std::set<Int_t>> GetStartTracks();
  bool CheckHitDistance(int hit1, int hit2); ///< Tests if the distance is larger than fMinPointDistance
  bool CheckSZ(PndRiemannTrack aTrack);      ///< Tests the results of the sz fit
  bool CheckRiemannHit(PndRiemannTrack *track, PndRiemannHit *hit);
  bool CheckHitInSameSensor(int hit1, int hit2);             ///< Tests if hits in the same sensor are selected
  bool CheckHitInTrack(std::set<int> hitIds, int hit);       ///< Check if this HitId is used in the track already
  bool CheckZeroPassing(std::set<int> hitIds, int hit);      ///< If the track contains (0,0) all points have to go forward or all have to go backward
  PndRiemannTrack CreateRiemannTrack(std::set<Int_t> aHits); ///< Creates a PndRiemannTrack from an array of indices of Hits

  bool TrackExists(std::set<Int_t> hitsInTrack);
  std::vector<int> FindTracksWithSimilarParameters(int TrackInd, std::vector<int> &TracksToTest, double curvDiff, double dipDiff);
  std::vector<int> FindTracksWithSimilarHits(std::vector<int> &TracksToTest, std::vector<PndTrackCand> tempTrCnd, std::vector<int> &tempKillAfter); ///<------------added by me
  void RemoveTrack(int TrackInd, std::vector<int> &TrackList);
  //	void PrintTrackCand(PndTrackCand* cand);
  PndTrackCand CreateOneTrackCand(std::vector<int> tracks, std::vector<PndTrackCand> tempTrCnd); //////////////aded by AG
                                                                                                 // Fast method to figure out if a combination of hits is already used as a track
                                                                                                 // Problem: It does not work for more than 32 hits
  //	std::vector<Int_t> fHitsInTracks; ///< one int stands for the hits used in one track. The hits are encoded as bits in an int32

  // Solution: use a vector<vector <Int_t> > fHitsInTracks instead. This is not so fast but works for more than 32 hits

  //	void SetHitInTrack(Int_t position, Int_t& value);
  //	std::vector<int> GetHitsInTrack(Int_t value);
  //	bool TrackExists(Int_t hitsInTrack);

  //	std::vector<int> GetHitsInTrack(Int_t value);

 public:
  ClassDef(PndRiemannTrackFinder, 2)
};

#endif /*PNDRIEMANNTRACKFINDER_H_*/
