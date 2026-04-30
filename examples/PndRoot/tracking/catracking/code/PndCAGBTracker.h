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

//-*- Mode: C++ -*-
// *****************************************************************************
//                                                                             *
// @Autors: I.Kulakov; M.Zyzak; I.Kisel                                        *
// @e-mail: I.Kulakov@gsi.de; M.Zyzak@gsi.de; I.Kisel@compeng.uni-frankfurt.de *
//                                                                             *
// *****************************************************************************

#ifndef PNDCAGBTRACKER_H
#define PNDCAGBTRACKER_H

#include "PndCATimer.h"

#include "PndCADef.h"
#include "PndCATrackParam.h"
#include "PndCAParam.h"

#include "PndCAGBHit.h"
#include "PndCAGBTrack.h"

#include "PndCAStationArray.h"
#include "PndCATarget.h"
#include "PndCANPlets.h"
#include "PndCANPletsV.h"

#include "PndCAStationSTT.h"

class PndCATarget;
class PndCAHit;
class PndCATrack;
class PndCAHits;
class PndCATracks;
class PndCAHitV;
class PndCAHitsV;

#include <cstdio>
#include <iostream>
#include <vector>
#include <string>
using std::string;
using std::vector;

class PndCAMerger;

class PndCAGBTracker {
 public:
  PndCAGBTracker();
  ~PndCAGBTracker();
  void Init();

  void StartEvent();
  void SetNSlices(int N);

  const PndCAGBHit *Hits() const { return &fHits[0]; }
  const PndCAGBHit &Hit(int index) const
  {
    if (index < 0 || index >= (int)fHits.size()) {
      cout << "error hit index ind " << index << " size " << fHits.size() << " nHits " << fNHits << endl;
      exit(0);
    }
    return fHits[index];
  }

  int NHits() const { return fNHits; }
  double Time() const { return fTime; }
  double StatTime(int iTimer) const { return fStatTime[iTimer]; }
  int NTimers() const { return fNTimers; }
  int StatNEvents() const { return fStatNEvents; }
  int NTracks() const { return fNTracks; }
  PndCAGBTrack *Tracks() const { return fTracks; }
  PndCAGBTrack *Tracks() { return fTracks; }
  const PndCAGBTrack &Track(int i) const { return fTracks[i]; }
  int *TrackHits() const { return fTrackHits; }
  int *TrackHits() { return fTrackHits; }
  int TrackHit(int i) const { return fTrackHits[i]; }

  const PndCAParam &GetParameters() const { return fParameters; }
  PndCAParam &GetParametersNonConst() { return fParameters; }

  int NStations() const { return fParameters.NStations(); }

  void WriteSettings(std::ostream &out) const;
  void ReadSettings(std::istringstream &in);
  void WriteEvent(FILE *out) const;

  //   void ReadTracks( std::istream &in );

  void SaveHitsInFile(string prefix) const; // Save Hits in txt file. @prefix - prefix for file name. Ex: "./data/ev1"
                                            //   void SaveSettingsInFile( string prefix ) const; // Save geometry in txt file. @prefix - prefix for file name. Ex: "./data/"
  bool SaveTracksInFile(string prefix) const;
  bool ReadHitsFromFile(string prefix);
  //   bool ReadSettingsFromFile( string prefix );

  double SliceTrackerTime() const { return fSliceTrackerTime; }
  double SliceTrackerCpuTime() const { return fSliceTrackerCpuTime; }

  void StoreToFile(const char *filename) const;
  void RestoreFromFile(FILE *f);

  void SetHits(std::vector<PndCAGBHit> &hits); // need for StRoot
  int GetHitsSize() const { return fHits.size(); }

  void FindTracks();

  void CATrackFinder();

  // fuctions used by TrackFinder

  void Create1Plets(const PndCATarget &target, const PndCAHits &hits, PndCAElementsOnStation<PndCANPletV> &singlets, int iStation);
  void PickUpHits(PndCAElementsOnStation<PndCANPletV> &a, PndCAElementsOnStation<PndCANPletV> &r, int iS);

  void CreateNPlets(const PndCATarget &target, const PndCAHits &hits, PndCAElementsOnStation<PndCANPletV> &triplets, int iStation, int cellLength);

  void FindNeighbours(PndCANPlets &triplets);
  void CreateTracks(const PndCANPlets &triplets, PndCATracks &tracks);

  void InvertCholetsky(float a[15]) const;
  void MultiplySS(float const C[15], float const V[15], float K[5][5]) const;
  void MultiplyMS(float const C[5][5], float const V[15], float K[15]) const;
  void MultiplySR(float const C[15], float const r_in[5], float r_out[5]) const;
  void FilterTracks(float const r[5], float const C[15], float const m[5], float const V[15], float R[5], float W[15], float &chi2) const;
  void Merge(PndCATracks &tracks);

  void FindBestCandidate(int ista, PndCATrack &best_tr, int currITrip, PndCATrack &curr_tr, unsigned char min_best_l, const PndCANPlets &triplets, unsigned int &nCalls);

  // private

  enum {
    kFastPrimIter,    // primary fast tracks
    kAllPrimIter,     // primary all tracks
    kAllPrimJumpIter, // primary tracks with jumped triplets
    kAllSecIter       // secondary all tracks
  };
  int fFindIter; // current iteration number

  PndCATarget fTarget;
  float fMaxInvMom;         // max considered q/p for tracks
  float fPick_m,            // coefficient for size of region on middle station for add middle hits in triplets: Dx = Pick*sigma_x Dy = Pick*sigma_y
    fPick_r,                // same for right hits
    fPick;                  // pick for current procedure
  float fPickNeighbour;     // (PickNeighbour < dp/dp_error)  =>  triplets are neighbours
  float TRACK_PROB_CUT;     // = 0.01;
  float TRACK_CHI2_CUT;     // = 10.0;  // cut for tracks candidates. per one DoF
  float_v TRIPLET_CHI2_CUT; // = 5.0; // cut for selecting triplets before collecting tracks.per one DoF
  float fMaxDX0;            // Set correction in order to take into account overlaping

  /// Try to group close hits in row formed by one track. After sort hits.
  friend class PndCAPerformance; // dbg
 protected:
  vector<PndCAGBHit> fHits; //* hit array
  int fNHits;               //* N hits in event

  int *fTrackHits;       //* track->hits reference array
  PndCAGBTrack *fTracks; //* array of tracks
  int fNTracks;          //* N tracks

  double fTime; //* total time
  static const int fNTimers = 25;
  double fStatTime[fNTimers]; //* timers
  int fStatNEvents;           //* n events proceed

  double fSliceTrackerTime;      // reco time of the slice tracker;
  double fSliceTrackerCpuTime;   // reco time of the slice tracker;
  PndCATFIterTimerInfo fGTi;     // global
  PndCATFTimerInfo fTi;          // for iterations
  PndCATFIterTimerInfo fStatGTi; // global
  PndCATFTimerInfo fStatTi;      // for iterations

  PndCAParam fParameters;
  PndCAStationSTT fStations[50];

 private:
  PndCAGBTracker(const PndCAGBTracker &);
  PndCAGBTracker &operator=(const PndCAGBTracker &);
};

#endif
