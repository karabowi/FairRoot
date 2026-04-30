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
// @Autors: I.Kulakov; M.Pugach; M.Zyzak; I.Kisel                                        *
// @e-mail: I.Kulakov@gsi.de; M.Pugach@gsi.de; M.Zyzak@gsi.de; I.Kisel@compeng.uni-frankfurt.de *
//                                                                             *
// *****************************************************************************

#ifndef PNDFTSCAGBTRACKER_H
#define PNDFTSCAGBTRACKER_H

#include "L1Timer.h"

#include "PndFTSCADef.h"
#include "PndFTSCATrackParam.h"
#include "PndFTSCAParam.h"
#include "PndFTSArray.h"
#include "PndFTSCAGBHit.h"
#include "FTSCAStrip.h"
#include "PndFTSCAGBTrack.h"

#include "FTSCAStationArray.h"
#include "FTSCATarget.h"
#include "FTSCANPlets.h"
#include "FTSCANPletsV.h"

class FTSCATarget;
class FTSCAHit;
class FTSCATrack;
class FTSCAHits;
class FTSCATracks;
class FTSCAHitV;
class FTSCAHitsV;

#include <cstdio>
#include <iostream>
#include <vector>
#include <string>
using std::string;
using std::vector;

// class PndFTSCAMerger;

class PndFTSCAGBTracker {
 public:
  PndFTSCAGBTracker();
  ~PndFTSCAGBTracker();
  void Init();

  void StartEvent();
  void SetNSlices(int N);
  void SetNHits(int nHits);

  // void SetGBHits();

  const PndFTSCAGBHit *Hits() const { return fHits.Data(); }
  const PndFTSCAGBHit &Hit(int index) const { return fHits[index]; }
  int NHits() const { return fNHits; }
  double Time() const { return fTime; }
  double StatTime(int iTimer) const { return fStatTime[iTimer]; }
  int NTimers() const { return fNTimers; }
  int StatNEvents() const { return fStatNEvents; }
  int NTracks() const { return fNTracks; }
  PndFTSCAGBTrack *Tracks() const { return fTracks; }
  PndFTSCAGBTrack *Tracks() { return fTracks; }
  const PndFTSCAGBTrack &Track(int i) const { return fTracks[i]; }
  int *TrackHits() const { return fTrackHits; }
  int *TrackHits() { return fTrackHits; }
  int TrackHit(int i) const { return fTrackHits[i]; }

  const PndFTSCAParam &GetParameters() const { return fParameters; }

  int NStations() const { return fParameters.NStations(); }

  /*
  void WriteSettings( std::ostream &out ) const;
  void WriteEvent( FILE *out ) const;
  void ReadEvent( FILE *in );
  void ReadTracks( std::istream &in );

  void SaveHitsInFile( string prefix ) const; // Save Hits in txt file. @prefix - prefix for file name. Ex: "./data/ev1"
  void SaveSettingsInFile( string prefix ) const; // Save geometry in txt file. @prefix - prefix for file name. Ex: "./data/"
  bool SaveTracksInFile( string prefix ) const;
  int ReadHitsFromFile( string prefix );
  void StoreToFile( const char *filename ) const;
  void RestoreFromFile( FILE *f );
  */

  bool ReadSettingsFromFile(string prefix);
  void ReadSettings(std::istream &in);
  double SliceTrackerTime() const { return fSliceTrackerTime; }
  double SliceTrackerCpuTime() const { return fSliceTrackerCpuTime; }

  void SetHits(std::vector<PndFTSCAGBHit> &hits);
  int GetHitsSize() const { return fHits.Size(); }

  PndFTSCAParam &GetParametersNonConst() { return fParameters; }

  void FindTracks();
  // void IdealTrackFinder(); // for debug

  void FitTracks();
  float_m FitTrack(PndFTSCATrackParamVector &t, uint_v &firstHits, uint_v::Memory &NTrackHits, int &nTracksV, float_m active0, bool dir);

  void InitialTrackApproximation(PndFTSCATrackParamVector &t, uint_v &firstHits, uint_v::Memory &NTrackHits, int &nTracksV, float_m active0);

  float_m FitTrackCA(PndFTSCATrackParamVector &t, uint_v &firstHits, uint_v::Memory &NTrackHits, int &nTracksV, float_m active0, bool dir, bool init = false);

  void CATrackFinder();
  // fuctions used by TrackFinder
  void EstimatePV(const FTSCAHitsV &hits, float &zPV);

  void CreateNPlets(const FTSCATarget &target, const FTSCAHitsV &hits, FTSCANPletsV &singlets);
  void CreateNPlets(const FTSCANPletsV &doublets, FTSCANPletsV &triplets);

  // fts--->>>stt import begin
  void PickUpHits(FTSCAElementsOnStation<FTSCANPletV> &a, FTSCAElementsOnStation<FTSCANPletV> &r, int iS);
  void Create1Plets(const FTSCATarget &target, const FTSCAHits &hits, FTSCAElementsOnStation<FTSCANPletV> &singlets, int iStation);
  void CreateNPlets(const FTSCATarget &target, const FTSCAHits &hits, FTSCAElementsOnStation<FTSCANPletV> &triplets, int iStation, int cellLength);
  // fts--->>>stt import end

  void FindNeighbours(FTSCANPlets &triplets);
  void CreateTracks(const FTSCANPlets &triplets, FTSCATracks &tracks);

  void InvertCholetsky(float a[15]) const;
  void MultiplySS(float const C[15], float const V[15], float K[5][5]) const;
  void MultiplyMS(float const C[5][5], float const V[15], float K[15]) const;
  void MultiplySR(float const C[15], float const r_in[5], float r_out[5]) const;
  void FilterTracks(float const r[5], float const C[15], float const m[5], float const V[15], float R[5], float W[15], float &chi2) const;
  void Merge(FTSCATracks &tracks);
  // FTSCAElementsOnStation<FTSCANPletV> Combine( const FTSCATarget& t, const FTSCAElementsOnStation<FTSCAHitV>& h);
  // FTSCAElementsOnStation<FTSCANPletV> Combine( const FTSCAElementsOnStation<FTSCANPletV>& a, const FTSCAElementsOnStation<FTSCANPletV>& b );
  float_m Refit(FTSCANPletV &triplet, const FTSCAHits &hits);
  void Refit_1(FTSCANPletV &triplet, const FTSCAHits &hits);

  void FindBestCandidate(int ista, FTSCATrack &best_tr, int currITrip, FTSCATrack &curr_tr, unsigned char min_best_l, const FTSCANPlets &triplets, unsigned int &nCalls);
  float_m IsEqual(const PndFTSCATrackParamVector &p, const FTSCAHit &h);

  // private

#if defined(PANDA_STT) || defined(PANDA_FTS)
  enum { fNFindIterations = 1 };
#elif defined(ALICE_ITS) || defined(STAR_HFT)
  enum { fNFindIterations = 2 };
#else
#error UNKNOWN DETECTOR
#endif
  enum {
    kFastPrimIter,    // primary fast tracks
    kAllPrimIter,     // primary all tracks
    kAllPrimJumpIter, // primary tracks with jumped triplets
    kAllSecIter       // secondary all tracks
  };
  int fFindIter; // current iteration number

  bool fFindGappedTracks; // = true -> construct triplets with skipped stations
  FTSCATarget fTarget;
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
  friend class PndFTSCAPerformance; // dbg

  PndFTSResizableArray<PndFTSCAGBHit> fHits; //* hit array

 protected:
  //    PndFTSResizableArray<PndFTSCAGBHit> fHits;     //* hit array
  PndFTSResizableArray<FTSCAStrip> fFStrips; //* front strips
  PndFTSResizableArray<FTSCAStrip> fBStrips; //* back strips
  int fNHits;                                //* N hits in event
  int *fTrackHits;                           //* track->hits reference array
  PndFTSCAGBTrack *fTracks;                  //* array of tracks
  int fNTracks;                              //* N tracks

  double fTime; //* total time
  static const int fNTimers = 25;
  double fStatTime[fNTimers]; //* timers
  int fStatNEvents;           //* n events proceed

  double fSliceTrackerTime;     // reco time of the slice tracker;
  double fSliceTrackerCpuTime;  // reco time of the slice tracker;
  L1CATFIterTimerInfo fGTi;     // global
  L1CATFTimerInfo fTi;          // for iterations
  L1CATFIterTimerInfo fStatGTi; // global
  L1CATFTimerInfo fStatTi;      // for iterations

  PndFTSCAParam fParameters;

 private:
  PndFTSCAGBTracker(const PndFTSCAGBTracker &);
  PndFTSCAGBTracker &operator=(const PndFTSCAGBTracker &);
};

#endif
