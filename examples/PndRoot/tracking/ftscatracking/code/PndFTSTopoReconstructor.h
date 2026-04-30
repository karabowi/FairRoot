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

#ifndef PndFTSTopoReconstructor_H
#define PndFTSTopoReconstructor_H

/*
 * Interface class for use KFParticle Topology Reconstructor
 */

#include "KFPTopoReconstructor.h"
#include "KFPTrack.h"
#include <vector>

class PndFTSCAGBTracker;
class PndFTSCATrackParam;
class KFParticle;
class KFParticleSIMD;

class PndFTSTopoReconstructor {
 public:
  PndFTSTopoReconstructor() { fKFPTopoReconstructor = new KFPTopoReconstructor; };
  ~PndFTSTopoReconstructor();

  void Init(PndFTSCAGBTracker *tracker); // init array of particles

  void ReconstructPrimVertex(); // find primary vertex
  void ReconstructParticles();  // find short-lived particles

  /// Accessors
  KFParticle &GetPrimVertex() const { return fKFPTopoReconstructor->GetPrimVertex(); };
  vector<KFParticle> const &GetParticles() const { return fParticles; }
  vector<KFPTrack> const &GetTracks() const { return tmpTracks; }
  vector<int> const &GetRTrackIds() const { return fRTrackIds; }
  vector<float> const &GetChiPrim() const { return fChiToPrimVtx; }

 private:
  PndFTSTopoReconstructor &operator=(PndFTSTopoReconstructor &);
  PndFTSTopoReconstructor(PndFTSTopoReconstructor &);

  void GetChiToPrimVertex(vector<KFPTrack> &tracks, KFParticleSIMD &pv, vector<float> &chi);

  KFPTopoReconstructor *fKFPTopoReconstructor;

  std::vector<KFPTrack> tmpTracks;
  std::vector<const PndFTSCATrackParam *> tmpGbTracks; // pointer to the tracks in tracker

#ifdef DO_TPCCATRACKER_EFF_PERFORMANCE
  friend class PndFTSTopoPerformance;
#endif
  vector<int> fRTrackIds; // id of reco tracks in GBTracker indiced by id of tracks in KFPTopoReconstructor. Need for performance

  vector<float> fChiToPrimVtx;
  vector<KFParticle> fParticles;
}; // class PndFTSTopoReconstructor

#endif // PndFTSTopoReconstructor_H
